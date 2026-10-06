#include "client.h"

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