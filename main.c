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
#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define N_PORT 20000 //le meme numero de port que celui utilisé sur le serveur
#define T_BUFF 256

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
/*
 * 
 */
int main(int argc, char** argv) {
    struct sockaddr_in client_add, server_add; // adresse du serveur
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

    client_add.sin_family = AF_INET;
    client_add.sin_addr.s_addr = (struct sockaddr *) infos_server->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure infos_server */
    client_add.sin_port = htons(N_PORT);

    if (connect(socket_client, (struct sockaddr *) & server_add, sizeof (server_add)) == -1) {
        perror("connect");
        exit(-1);
    }

    sendToServer(socket_client, buffer);
    
    buffer[n] = '\0';
    printf("Reception de %s", buffer);
    close(socket_client);

    
    return (EXIT_SUCCESS);
}

void sendToServer(int socket, char *buffer) {
    if (send(socket, buffer, strlen(buffer), 0) == -1) {
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
