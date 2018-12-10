#include "../headers/fonctions.h"

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
    if ((n = recv(socket, buffer, sizeof buffer - 1, 0)) == -1) {
        perror("recv()");
        exit(-1);
    }
    return buffer;
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
            if ((strcmp(lecture->d_name, ".") != 0) && (strcmp(lecture->d_name, "..") != 0)) {
                strcat(tab[i].info, lecture->d_name);
            }

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

void receptionFichier(int socket, char *buffer) {
    char tampon[512];
    FILE * fichier_recu = NULL;
    chemin_de_fichier mes_images[20];
    chemin_de_fichier images_choisies[20];
    //image img;
    //char ch;
    char nomF[128], repertoire[128] = "./images/";
    int nb_img_attendus = 0, taille_img_attendu = 0, paquetRec = 0;
    int i = 0, choix = -1, taille_img_tab;

    //recuperer la liste des images disponibles
    recv(socket, mes_images, 2048, 0);
    //recuperer la taille du tableau d'images
    read(socket, &taille_img_tab, sizeof (int));
    printf("Il y a %d fichiers sur le serveur\n", taille_img_tab);

    while (i < taille_img_tab) {
        printf("\n%d- %s \n", i, mes_images[i].info);
        i++;
    }

    //choix du client
    nb_img_attendus = saisir("Combien d'images voulez-vous télécharger?", taille_img_tab);
    printf("Vous avez choisi d'envoyer %d images\n", choix);

    //envoi du nombre de fichiers demandés
    write(socket, &nb_img_attendus, sizeof (int));

    i = 0;
    while (i < nb_img_attendus) {
        printf("\nImage n°%d\n", i + 1);
        choix = saisir("Choisissez le fichier", taille_img_tab - 1);
        printf("Choix n°%d: (image n°%d) %s\n", i + 1, choix, mes_images[choix].info);
        strcpy(images_choisies[i].info, mes_images[choix].info);
        i++;
    }

    //envoi choix du client au serveur
    send(socket, images_choisies, 2048, 0);

    /*if (nb_img_attendus > 0){*/

    i = 0;
    while (i < nb_img_attendus) {
        printf("\n\nreception du %de fichier\n", i+1);
        read(socket, &taille_img_attendu, sizeof (int)); //recuperer la taille du fichier
        printf("taille attendue %d\n", taille_img_attendu);

        strcpy(nomF, images_choisies[i].info);
        
        strcpy(repertoire, "./images/");
        strcat(repertoire, nomF);
        printf("son nom est %s\n", nomF);
        //fichier_recu = fopen(repertoire, "w+");
        if ((fichier_recu = fopen(repertoire, "w+")) == NULL) {
            perror("fopen");
            exit(-1);
        }
        int pa;
        int lu = 0;
        memset(tampon, '0', 512);

        while (paquetRec < taille_img_attendu) {
            recv(socket, tampon, 512, 0) > 0;
            pa = fwrite(tampon, sizeof (char), 512, fichier_recu);
            memset(tampon, '0', 512);
            paquetRec += pa;
        }
        //compare_type(fichier_recu);
        printf(" paquetRec = %d recus / %d envoyés\n", paquetRec, taille_img_attendu);

        //liberation/reinitialisation des ressources
        memset(tampon, '0', 512);
        printf("fichier %s recu\n", nomF);
        strcpy(nomF, "");
        lu = 0;
        paquetRec = 0;
        taille_img_attendu = 0;
        visualiser_image(repertoire);
        i++;
        fclose(fichier_recu);
        fichier_recu = NULL;
    }
}
//printf("Reçu: %s\n", recu);
//doit appeler la verification d'iamges
//}

void convertir_image(int socket, char *cheminFichier, char *buffer) {
    FILE *fichier;
    char tampon[512];
    char repertoire[256] = "./images/";
    int paquetEnv = 0;

    strcat(repertoire, cheminFichier);
    //printf("++++Le chemin du fichier %s\n", repertoire);
    if ((fichier = fopen(repertoire, "r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    fseek(fichier, 0, SEEK_END);
    int taille_image = ftell(fichier);
    write(socket, &taille_image, sizeof (int)); //envoi de la taille du fichier

    strcpy(tampon, cheminFichier);
    send(socket, tampon, 128, 0); //envoi du nom de fichier

    memset(tampon, '0', 512);
    fseek(fichier, 0, SEEK_SET);

    int lu = 0;
    while (paquetEnv < taille_image) {
        lu = fread(tampon, sizeof (char), 512, fichier);
        send(socket, tampon, 512, 0);
        paquetEnv += lu;
        memset(tampon, '0', 512);
    }
    printf("paquetRec = %d envoyés / %d\n", paquetEnv, taille_image);

    int recevable = 0;
    read(socket, &recevable, sizeof (int));
    if (recevable < 0) { //l'image envoyée n'est pas recevable
        printf("Le fichier envoyé (%s) n'etait pas recevable et a été supprimé\n\n", cheminFichier);
    } else {//l'image est recevable
        printf("Le fichier envoyé (%s) etait recevable et a été enregistré\n\n", cheminFichier);
    }

    fclose(fichier);
}

void affiche_aide() {
    printf("Usage: \nclient <nom du serveur> <numero port>\n");
    printf("\tExemple: client 192.168.1.23 20000\n");
    exit(-1);
}

void visualiser_image(char* image) {
    char *com[3] = {"xdg-open", image, (char *) 0};
    /* on forke le processus */
    switch (fork()) {
        case -1:
            perror("fork erreur");
            exit(-1);
        case 0:
            //comportement du fils
            if (execvp("xdg-open", com) == -1) {
                perror("execvp");
                exit(0);
            }
        default:
            while (wait(NULL) != -1);
    }

}
