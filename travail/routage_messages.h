#ifndef ROUTAGE_MESSAGES_H
#define ROUTAGE_MESSAGES_H

#include "msg_struct.h"
#include "gestion_clients.h"


void traiter_nouveau_pseudo(struct client_info *tete, int fd_client, struct message *msg);
void diffuser_message_public(struct client_info *tete, int fd_expediteur, struct message *msg, char *buffer);
void envoyer_message_prive(struct client_info *tete, int fd_expediteur, struct message *msg, char *buffer);
void envoyer_annuaire_clients(struct client_info *tete, int fd_demandeur);
#endif