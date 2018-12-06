#ifndef FONCTIONS_H
#define FONCTIONS_H

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

#include "definitions.h"

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
void convertir_image(int socket, char *cheminFichier, char *buffer);

/** @brief Affiche l'aide quand l'utilisation du programme
 **/
void affiche_aide();

#endif /* FONCTIONS_H */

