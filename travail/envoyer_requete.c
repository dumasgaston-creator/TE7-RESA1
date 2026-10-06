#include "client.h"

void envoyer_requete(int fd, uint16_t type, const char *infos, const char *contenu, int taille_contenu) {
    struct message req;
    memset(&req, 0, sizeof(struct message));
    req.type = type;
    req.pld_len = taille_contenu;

    // S'il y a un pseudo ou un paramètre à copier
    if (infos != NULL) {
        strncpy(req.infos, infos, INFOS_LEN - 1);
        req.infos[strcspn(req.infos, "\n")] = '\0'; // Nettoyage automatique
    }

    // 1er envoi : l'enveloppe
    write(fd, &req, sizeof(struct message));

    // 2ème envoi : le reste
    if (contenu != NULL && taille_contenu > 0) {
        write(fd, contenu, taille_contenu);
    }
}