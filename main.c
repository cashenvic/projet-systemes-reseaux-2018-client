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
char* lister_image();

/*
 * 
 */
int main(int argc, char** argv) {
    struct sockaddr_in client_add, server_add = {0}; // adresse du serveur
    struct hostent *infos_server = NULL;
    int socket_client;
    const char *hostname = "localhost"; // nom du serveur
    char buffer[T_BUFF];
    int n = 0; // temoin pour la lecture avec le buffer

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

    //read(socket_client, buffer, T_BUFF);
    printf("Reception du message : %s", buffer);
    //close(socket_client);

    //affichage de la liste des fichiers dans le répértoire courant
    printf("\n \n La liste des fichiers du répértoire courant est :");

    char* tab_image[500] = {0};
    int i = 0;
    *tab_image = lister_image();
    while (tab_image[i] != 0) {
        printf("le nom du fichier est : %s", tab_image[i]);
        i++;

    }
    /*
        strcpy(buffer, "Coucou serveur");
        printf("Connexion etablie\n");
        //buffer[n] = '\0';
        printf("Envoi de %s", buffer);
        sendToServer(socket_client, buffer);
     */
    //write(server_add)

    envoiFichier(socket_client, "2018-web.pdf", buffer);

    close(socket_client);

    return (EXIT_SUCCESS);
}

void sendToServer(int socket, char *buffer) {
    if (write(socket, buffer, strlen(buffer)) == -1) {
        perror("send");
        exit(-1);
    }
}

char * receiveFromServer(int socket, char *buffer, int n) {
    if ((n = recv(socket, buffer, sizeof buffer - 1, 0)) == -1) {
        perror("recv()");
        exit(-1);
        return buffer;
    }
}

char* lister_image() {
    char* tab_images[500];
    int i = 0;
    struct dirent *lecture;
    DIR *reponse;
    reponse = opendir(".");
    if (reponse != NULL) {
        while ((lecture = readdir(reponse))) {
            struct stat st;

            stat(lecture->d_name, &st);
            {
                /* date de modification des fichiers */
                time_t t = st.st_mtime;
                struct tm tm = *localtime(&t);
                char s[32];
                strftime(s, sizeof s, "%d/%m/%Y %H:%M:%S", &tm);

                // printf ("\n %-14s %s\n", lecture->d_name, s);
                tab_images[i] = lecture->d_name;
                i++;

            }
        }
        closedir(reponse), reponse = NULL;
    }
    return *tab_images;
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

    while ((ch = fgetc(fichier)) != EOF) {
        strcat(buffer, &ch);
        if ((i == T_BUFF - 1) || ch == EOF) {
            sendToServer(socket, buffer);
            i = 0;
            strcpy(buffer, "");
            //strcat(buffer, &ch);
        }
        i++;
    }

    /*while (read(fichier, buffer, strlen(buffer))) {
        printf("%s", buffer);
        //write(socket_server, buffer, strlen(buffer);
    }*/
    fclose(fichier);

    //ecrire dans la socket serveur
}
