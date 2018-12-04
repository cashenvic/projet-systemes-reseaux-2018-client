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

/** @brief Affiche la liste des fichiers dans le répértoire courant
 *  @param 
 *  @return char*
 **/
char** lister_image() {
    //char* tab_images[500];

    //struct fichier tab_images[10];
    char** tab_images;
    int i = 0;
    struct dirent *lecture;
    DIR *reponse;
    reponse = opendir(".");
    if (reponse != NULL) {
        tab_images = (char**)malloc(50*sizeof(char));
        printf("\n \nListe des fichiers du répértoire courant est: \n");
        while ((lecture = readdir(reponse))) {
            struct stat st;
            tab_images[i] = (char*)malloc(256*sizeof(char));
            strcpy(tab_images[i], lecture->d_name);

            stat(lecture->d_name, &st);
            {
                /* date de modification des fichiers */
                time_t t = st.st_mtime;
                struct tm tm = *localtime(&t);
                char s[32];
                strftime(s, sizeof s, "%d/%m/%Y %H:%M:%S", &tm);

                //printf ("\n %-14s %s\n", lecture->d_name, s);
               //printf ("\n %-14s \n", tab_images[i]);
                i++;
            }
        }
        closedir(reponse), reponse = NULL;
        //free(tab_images);
    }
    return tab_images;
}

/*
 * 
 */
int main(int argc, char** argv) {
    /*char** tab_image = NULL;*/
    int i = 0;

    //affichage de la liste des fichiers dans le répértoire courant
    

    char** tab_image = lister_image();

    while (tab_image[i] != NULL) {
       printf ("et je boucle \n"); 
       printf ("\n %-14s \n", tab_image[i]);
    }

    return (EXIT_SUCCESS);
}
