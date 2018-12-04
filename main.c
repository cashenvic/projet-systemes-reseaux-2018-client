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
#define N_PORT 20000 //le meme numero de port que celui utilisé sur le serveur
#define T_BUFF 1024

typedef struct {
    char info[24];
} chemin_de_fichier; 

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
void lister_image(chemin_de_fichier tab[10], int *taille);

/** @brief Permet la saisie des choix client en controlant les limites
 *  @param char message: message à afficher pour inviter à la saisie
 *  @return int: le choix saisi par le client
 **/
int saisir(char message[], int nbr_choix);

/** @brief Affiche le menu au client afin qu'il choisisse ce qu'il veut faire
 *  @return int : Retourne la saisie du client
 **/
int menu_client();

/** 
 *  
 */

int main(int argc, char** argv) {
    struct sockaddr_in client_add, server_add = {0}; // adresse du serveur
    struct hostent *infos_server = NULL;
    int socket_client;
    int n = 0, choix = 0; // temoin pour la lecture avec le buffer
    int i = 0, taille_liste_fichier;
    chemin_de_fichier tab[20];
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
    if (choix == 2) {
        printf("envoi d'un fichier\n");
        lister_image(tab, &taille_liste_fichier);
        //prompt choisir le ou les fichiers à envoyer
        //envoiFichier(socket_client, "2018-web.pdf", buffer);
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

    do {
        printf("\n--%s \n---> : ", message);
        scanf("%d", &choix);
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

void lister_image(chemin_de_fichier tab[10], int *taille) {

    int i = 0;
    struct dirent *lecture;
    DIR *reponse;
    reponse = opendir("./images");
    if (reponse != NULL) {
        printf("\n \nListe des fichiers du repertoire d'image: \n");
        while ((lecture = readdir(reponse))) {
            struct stat st;

            strcpy(tab[i].info, lecture->d_name);
            stat(lecture->d_name, &st);
            {
                /* date de modification des fichiers */
                time_t t = st.st_mtime;
                struct tm tm = *localtime(&t);
                char s[32];
                strftime(s, sizeof s, "%d/%m/%Y %H:%M:%S", &tm);

                printf("\n%d- %-14s     %s\n", i, lecture->d_name, s);
            }
            i++;
        }
        *taille = i;
        closedir(reponse), reponse = NULL;
    }
}

void envoiFichier(int socket, char *cheminFichier, char *buffer) {
    //lis le fichier à partir du rep client
    FILE *fichier;
    char ch;
    int i = 0;

    if ((fichier = fopen(cheminFichier, "r")) == NULL) {
        perror("fopen");
        exit(-1);
    }
    printf("successfull opening of file %s\n", cheminFichier);

    while ((ch = fgetc(fichier)) != EOF) {
        strcat(buffer, &ch);
        printf("%d\n", i);
        if ((i == T_BUFF - 1)) {
            printf("Ok j'envoie tout ça\n");
            sendToServer(socket, buffer);
            i = 0;
            strcpy(buffer, "");
            //strcat(buffer, &ch);
        }
        i++;
    }
    if (strcmp(buffer, "") != 0) {
        printf("Ok j'envoie tout ça\n");
        sendToServer(socket, buffer);
        strcpy(buffer, "");
    }
    
    fclose(fichier);

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
