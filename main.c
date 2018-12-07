#include "fonctions.h"

int main(int argc, char** argv) {
    struct sockaddr_in server_add = {0}; // adresse du serveur
    struct hostent *infos_server = NULL;
    int socket_client;
    int choix = -1, nbre_images; // temoin pour la lecture avec le buffer
    int i = 0, taille_liste_fichier;
    chemin_de_fichier chemins_images[20];
    chemin_de_fichier chemins_img_choisis[20]; //allocation dynamique plus bas si on connait le nombre d'image
    image images[20]; //allocation dynamique plus bas si on connait le nombre d'image
    char ch_to_send[800000];
    char buffer[T_BUFF];
    const char *hostname = "localhost"; // nom du serveur

    if (argc != 3) {
        affiche_aide();
    }
    hostname = argv[1];
    const int N_PORT = atoi(argv[2]);
    system("clear");
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
            if ((strcmp(chemins_images[choix].info, ".") == 0)
                    && (strcmp(chemins_images[choix].info, "..") == 0)) {
                printf("Ce choix n'est pas valide");
            } else
                strcpy(chemins_img_choisis[i].info, chemins_images[choix].info);
            i++;
        }
        i = 0;
        while (i < nbre_images) {
            convertir_image(socket_client, chemins_img_choisis[i].info, ch_to_send);
            strcpy(images[i].nom_fichier, chemins_img_choisis[i].info);
            strcpy(images[i].contenu_fichier, ch_to_send);
            i++;
        }
        strcpy(ch_to_send, "");
        chaine_structure_Contenu(ch_to_send, images, nbre_images);
        //envoiFichier(socket_client, ch_to_send, buffer);
        printf("fichier envoyé: %s", ch_to_send);
        //sendToServer(socket_client, ch_to_send);
    } else if (choix == 1) {
        printf("en attente d'un fichier\n");
        receptionFichier(socket_client, buffer);
    }
    write(socket_client, &choix, sizeof (int));
    close(socket_client);

    return (EXIT_SUCCESS);
}



