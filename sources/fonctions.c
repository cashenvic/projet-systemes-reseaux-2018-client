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

            for (i = 0; i < taille; i++) {
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
    for (i = 0; i < nbImg; i++) {
        strcat(chaine, images[i].nom_fichier);
        printf("nom image dans structure: %s\n", images[i].nom_fichier);
        strcat(chaine, ":");
        strcat(chaine, images[i].contenu_fichier);
        if (i < nbImg - 1)
            strcat(chaine, ":");
    }
}

void convertir_image(int socket, char *cheminFichier, char *buffer) {
    FILE *fichier;
    char tampon[512];
    char repertoire[256] = "./images/";
    //strcpy(repertoire, );
    strcat(repertoire, cheminFichier);
    printf("++++Le chemin du fichier %s", repertoire);
    if ((fichier = fopen(repertoire, "r")) == NULL) {
        perror("fopen");
        exit(-1);
    }
    printf("successfull opening of file %s\n", cheminFichier);
    strcpy(tampon, cheminFichier);
    send(socket, tampon, strlen (tampon), 0);

    memset(tampon, '0', 512);
    int paquetEnv;
    while ((paquetEnv = fread(tampon, sizeof (char), 512, fichier)) > 0) {
        send(socket, tampon, paquetEnv, 0);
        printf("packet envoyé\n");
        memset(tampon, '0', 512);
        if (paquetEnv == 0 || paquetEnv != 512) {
            break;
        }
    }

    fclose(fichier);
}

void affiche_aide() {
    printf("Usage: \nclient <nom du serveur> <numero port>\n");
    printf("\tExemple: client 192.168.1.23 20000\n");
    exit(-1);
}


