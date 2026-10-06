#include "client.h"

void traiter_reception_serveur(int client_fd, char *texte) {
    struct message reponse;
    int ret = read(client_fd, &reponse, sizeof(struct message));
    
    // Vérif de la connexion
    if (ret <= 0) {
        printf("Le serveur est déconnecté\n");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Lecture contenu
    if (reponse.pld_len > 0) {
        int lu_serveur = read(client_fd, buffer, reponse.pld_len);
        buffer[lu_serveur] = '\0';
    } else {
        buffer[0] = '\0'; // si rien reçu vide buffer
    }

    // pour fair ejoli
    afficher_message_serveur(reponse, buffer);
}