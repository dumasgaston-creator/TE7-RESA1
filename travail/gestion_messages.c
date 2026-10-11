#include "client.h"

char buffer[BUFFER_SIZE];

int valider_pseudo(const char *pseudo) {
    int len = strlen(pseudo);
    if (0 == len || len >= NICK_LEN) { // verif longueur
        printf("Erreur : Pseudo vide ou trop long\n");
        return 0; // 0 = Faux
    }
    for (int i = 0; i < len; i++) { // verif caractères
        if (!isalnum(pseudo[i]) && pseudo[i] != '\n') {
            printf("Erreur : Le pseudo ne doit contenir que des lettres et des chiffres.\n");
            return 0; // 0 = Faux
        }
    }
    return 1; // 1 = Vrai
}

void envoyer_requete(int fd, uint16_t type, const char *infos, const char *contenu, int taille_contenu) {
    struct message req;
    memset(&req, 0, sizeof(struct message));
    req.type = type;
    req.pld_len = taille_contenu;

    // S'il y a un pseudo ou un paramètre à copier
    if (infos != NULL) {
        strncpy(req.infos, infos, INFOS_LEN - 1);
        char *newline = strchr(req.infos, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
    }

    // 1er envoi : l'enveloppe
    write(fd, &req, sizeof(struct message));

    // 2ème envoi : le reste
    if (contenu != NULL && taille_contenu > 0) {
        write(fd, contenu, taille_contenu);
    }
}

void afficher_message_serveur(struct message reponse, const char *texte) {
    if (reponse.type == UNICAST_SEND) {
        // Message privé
        printf("[Message privé de %s] : %s\n", reponse.nick_sender, buffer);
    } 
    else if (reponse.type == BROADCAST_SEND) {
        // Message public
        printf("[%s] : %s\n", reponse.nick_sender, buffer);
    } 
    else if (reponse.type == NICKNAME_LIST) {
        // Liste des connectés (/who)
        printf("[Info Serveur] :\n%s\n", buffer);
    } 
    else {
        // Autres messages (Echo, erreurs, etc.)
        if (reponse.pld_len > 0) {
            printf("[Serveur] : %s\n", buffer); 
        } else {
            // si le serveur envoie juste une enveloppe sans texte
            printf("[Info Serveur] Commande %d validée.\n", reponse.type);
        }
    }
}

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