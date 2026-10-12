#ifndef GESTION_MESSAGES_H
#define GESTION_MESSAGES_H

#include <stdint.h>

int valider_pseudo(const char *pseudo);
void envoyer_requete(int fd, uint16_t type, const char *infos, const char *contenu, int taille_contenu);
void afficher_message_serveur(struct message reponse, const char *texte);
void traiter_reception_serveur(int client_fd, char *texte, int *attente, char *demandeur, char *fichier, char *fichier_a_envoyer);

#endif