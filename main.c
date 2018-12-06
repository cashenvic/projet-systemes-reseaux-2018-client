/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: cash
 *
 * Created on 1 décembre 2018, 21:37
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>


#define N_PORT 20000 //le meme numero de port que celui utilisé sur le serveur
#define T_BUFF 1000*1024

/** @brief Respresente un fichier dans le listing des fichiers d'un repertoire
 *  @struct
 **/
typedef struct {
    char info[256];
    int taille;
} chemin_de_fichier;

/** @brief Represente une image avec le nom et le contenu du fichier
 *  @struct
 **/
typedef struct {
    char nom_fichier[256];
    char contenu_fichier[1000];
} image;

//_(°_°)_Tous les prototypes devront finir dans des fichiers headers (*.h)

/** @brief Envoi d'une demande de connexion vers le serveur
 *  @param int socket : socket (descripteur de fichier) du client qui fait la requete
 *  @param char *buffer : buffer (tableau de caractères) qui contient la requete envoyée.
 *  @return void : Pas de valeur de retour
 **/
void sendToServer(int socket, char *buffer);

/** @brief Reception des données reçues du serveur
 *  @param int socket : socket (descripteur de fichier) du client qui fait la requete
 *  @param char *buffer : buffer (tableau de caractères) qui contient les données reçues.
 *  @param int c: Etat de la lecture du buffer
 *  @return char * : Retourne la valeur reçue et lue sous forme de tableau de caractères
 **/
char * receiveFromServer(int socket, char *buffer, int n);

/** @brief Reception des données reçues du serveur
 *  @param int socket : socket (descripteur de fichier) du client qui fait la requete
 *  @param char *buffer : buffer (tableau de caractères) qui contient les données reçues.
 *  @param int c: Etat de la lecture du buffer
 *  @return char * : Retourne la valeur reçue et lue sous forme de tableau de caractères
 **/
void receptionFichier(int socket, char *buffer);

/** @brief Lis un fichier depuis le repertoire du client et l'envoie au serveur
 *  @param int socket : socket dans lequel il ecrit
 *  @param char *cheminFichier : chemin du fichier à lire
 *  @return void
 **/
void envoiFichier(int socket, char *cheminFichier, char *buffer);

/** @brief Affiche la liste des fichiers dans le répértoire courant
 *  @param 
 *  @return char*
 **/
void lister_image(char *repertoire, chemin_de_fichier tab[10], int *taille);

/** @brief Permet la saisie des choix client en controlant les limites
 *  @param char message: message à afficher pour inviter à la saisie
 *  @param int nbr_choix: quantité maximale autorisé de choix
 *  @return int: le choix saisi par le client
 **/
int saisir(char message[], int nbr_choix);

/** @brief Affiche le menu au client afin qu'il choisisse ce qu'il veut faire
 *  @return int : Retourne la saisie du client
 **/
int menu_client();

/** @brief Ouvre un fichier selon le mode selectionné et gère les erreurs d'ouverture
 *  @param char *cheminFichier: chemin du fichier à ouvrir
 *  @param char* mode: mode d'ouverture du fichier
 *  @return FILE: Retourne le fichier qu'il ouvert
 **/
FILE* ouvrirFichier(char *cheminFichier, char* mode);

/** @brief Conversion struct -> char*; char* -> struct
 *  @param char *cheminFichier: chemin du fichier à ouvrir
 *  @param char* mode: mode d'ouverture du fichier
 *  @return FILE: Retourne le fichier qu'il ouvert
 **/
void chaine_structure_liste(char p2[120], chemin_de_fichier tab [10], int taille, int choix);

/** @brief contruit une chaine avec le nom de fichier et son contenu
 *  @param char p2[]: chaine finale retournée
 *  @param image tab [10]: structure image à serialiser
 *  @param int taille: taille de tab
 **/
void chaine_structure_Contenu(char chaine[], image images[10], int nbImg);

/** @brief Lis le contenu d'image et l'associe à son nom
 *  @param char *cheminFichier: chemin du fichier à lire
 *  @param char *buffer: contenu lu
 **/
void convertir_image(char *cheminFichier, char *buffer);

/** 
 *  
 */

int main(int argc, char** argv) {
    struct sockaddr_in client_add, server_add = {0}; // adresse du serveur
    struct hostent *infos_server = NULL;
    int socket_client;
    int n = 0, choix = -1, nbre_images; // temoin pour la lecture avec le buffer
    int i = 0, taille_liste_fichier;
    chemin_de_fichier chemins_images[20];
    chemin_de_fichier chemins_img_choisis[20]; //allocation dynamique plus bas si on connait le nombre d'image
    image images[20]; //allocation dynamique plus bas si on connait le nombre d'image
    char ch_to_send[40 * N_PORT];
    char buffer[T_BUFF];
    const char *hostname = "localhost"; // nom du serveur

    socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client == -1) {
        perror("socket_client");
        exit(-1);
    }

    infos_server = gethostbyname(hostname); /* on récupère les informations du serveur */

    if (infos_server == NULL) {
        perror("infos_server");
        exit(-1);
    }

    server_add.sin_family = AF_INET;
    memcpy(&(server_add.sin_addr.s_addr), infos_server->h_addr, sizeof (u_long));
    server_add.sin_port = htons(N_PORT);

    if (connect(socket_client, (struct sockaddr *) &server_add, sizeof (server_add)) == -1) {
        perror("connect");
        exit(-1);
    }

    choix = menu_client();
    write(socket_client, &choix, sizeof (int));
    //sendToServer(socket_client, &choix);
    if (choix == 2) {
        choix = -1;
        printf("envoi d'un fichier\n");
        i = 0;
        lister_image("./images/", chemins_images, &taille_liste_fichier);
        //prompt choisir le nombre de fichiers à envoyer
        nbre_images = saisir("Combien d'images voulez-vous envoyer", taille_liste_fichier);
        printf("Choix de %d images\n", nbre_images);
        choix = -1;
        //prompt choix du/des fichier(s) à envoyer
        i = 0;
        while (i < nbre_images) {
            printf("\nImage n°%d\n", i + 1);
            choix = saisir("Choisissez le fichier", taille_liste_fichier);
            printf("Choix n°%d: (image n°%d) %s\n", i + 1, choix, chemins_images[choix].info);
            //construire un tableau avec les noms de fichier choisis
            strcpy(chemins_img_choisis[i].info, chemins_images[choix].info);
            i++;
        }
        i = 0;
        while (i < nbre_images) {
            convertir_image(chemins_img_choisis[i].info, ch_to_send);
            strcpy(images[i].nom_fichier, chemins_img_choisis[i].info);
            strcpy(images[i].contenu_fichier, ch_to_send);
            i++;
        }
        strcpy(ch_to_send, "");
        chaine_structure_Contenu(ch_to_send, images, nbre_images);
        //envoiFichier(socket_client, ch_to_send, buffer);
        printf("fichier envoyé: %s", ch_to_send);
        sendToServer(socket_client, ch_to_send);
    } else if (choix == 1) {
        printf("en attente d'un fichier\n");
        receptionFichier(socket_client, buffer);
    }
    close(socket_client);

    return (EXIT_SUCCESS);
}

//https://www.developpez.net/forums/d967275/general-developpement/programmation-systeme/linux/securiser-saisie-c-scanf/
int saisir(char message[], int nbr_choix) {
    int choix = -1;
    char saisie[255];

    do {
        printf("\n--%s \n---> : ", message);
        fgets(saisie, 255, stdin);
        if (sscanf(saisie, "%d", &choix) == 1) break;
        //scanf("%d", &choix);
        if (choix < 0 || choix > nbr_choix) {
            printf("Choix indisponible. Merci de choisir entre 0 et %d", nbr_choix);
        }
    } while (choix < 0 || choix > nbr_choix);

    return choix;
}

int menu_client() {
    int nbr_max_choix = 2;
    char message[] = "\nQue voulez-vous faire? Tapez 0 pour quitter :";
    printf("             **********Menu Client************\n");
    printf("   1. Telecharger fichier(s) image.\n");
    printf("   2. Televerser (Upload) fichier(s) image. \n");
    return saisir(message, nbr_max_choix);
}

void sendToServer(int socket, char *buffer) {
    if (write(socket, buffer, strlen(buffer)) == -1) {
        perror("send");
        exit(-1);
    }
}

char * receiveFromServer(int socket, char *buffer, int n) {
    char received[256];
    if ((n = recv(socket, buffer, sizeof buffer - 1, 0)) == -1) {
        perror("recv()");
        exit(-1);
        return buffer;
    }
}

void lister_image(char *repertoire, chemin_de_fichier tab[10], int *taille) {

    int i = 0;
    struct dirent *lecture;
    DIR *reponse;
    reponse = opendir(repertoire);
    if (reponse != NULL) {
        printf("\n \nListe des fichiers du repertoire d'image: \n");
        while ((lecture = readdir(reponse))) {
            //Amelioration: essayer de pas tenir compte de .. et .
            strcpy(tab[i].info, "");
            //strcat(tab[i].info, repertoire);
            strcat(tab[i].info, lecture->d_name);
            struct stat st;

            stat(lecture->d_name, &st);
            {
                /* date de modification des fichiers */
                time_t t = st.st_mtime;
                struct tm tm = *localtime(&t);
                char s[32];
                strftime(s, sizeof s, "%d/%m/%Y %H:%M:%S", &tm);
            }
            i++;
        }
        *taille = i;
        i = 0;
        closedir(reponse), reponse = NULL;
        while (i < *taille) {
            printf("\n%d- %s \n", i, tab[i].info);
            i++;
        }
    }
}

FILE* ouvrirFichier(char *cheminFichier, char* mode) {
    FILE *fichier;

    if ((fichier = fopen(cheminFichier, mode)) == NULL) {
        perror("fopen");
        exit(-1);
    }
    printf("successfull opening of file %s\n", cheminFichier);
    return fichier;
}

void envoiFichier(int socket, char *cheminFichier, char *buffer) {
    sendToServer(socket, buffer);

    //ecrire dans la socket serveur
}

void receptionFichier(int socket, char *buffer) {
    char* recu;
    while (read(socket, buffer, T_BUFF)) {
        //strcat(recu, &buffer);
        //printf("Reçu: rw%s", recu);
        printf("Reçu: b%s", buffer);
    }
    //printf("Reçu r: %s", recu);
}

void chaine_structure_liste(char p2[120], chemin_de_fichier tab [10], int taille, int choix) {
    int i = 0;
    char str[10];
    switch (choix) {
        case 1: /*Choix de Structure vers Chaine de caractere*/

            for (i; i < taille; i++) {
                sprintf(str, "%d", i);
                strcat(p2, str);
                strcat(p2, "|");
                strcat(p2, tab[i].info);
                if (i < taille - 1)
                    strcat(p2, ":");
            }
            break;
        case 2: /*Choix de chaine de caractaire vers Structure*/

            break;
        default: printf("--erreur de choix !!\n");
            break;
    }
}

void chaine_structure_Contenu(char chaine[], image images[10], int nbImg) {
    int i = 0;
    printf("On est bien dans chaine structure\n");
    for (i; i < nbImg; i++) {
        strcat(chaine, images[i].nom_fichier);
        printf("nom image dans structure: %s\n", images[i].nom_fichier);
        strcat(chaine, ":");
        strcat(chaine, images[i].contenu_fichier);
        if (i < nbImg - 1)
            strcat(chaine, ":");
    }
}

void convertir_image(char *cheminFichier, char *buffer) {
    FILE *fichier;
    char ch;
    int i = 0;
    char repertoire[256];
    strcpy(repertoire, "");
    strcat(repertoire, "./images/");
    strcat(repertoire, cheminFichier);

    if ((fichier = fopen(cheminFichier, "rb")) == NULL) {
        perror("fopen");
        exit(-1);
    }
    printf("successfull opening of file %s\n", cheminFichier);
    fseek(fichier, 0, SEEK_END);
    unsigned long taille_img = ftell(fichier);
    //buffer = (long*) malloc(taille_img * sizeof (long));

    char buff[taille_img];
    fseek(fichier, 0, SEEK_SET);
    if (fread(buff, sizeof (buff), 1024, fichier) != taille_img) {
        perror("fread");
    }
    printf("buffer %s\n", buff);
    /*
        while ((ch = fgetc(fichier)) != EOF) {
            strcat(buffer, &ch);
        }
     */
    fclose(fichier);
}
