#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#include <poll.h>
#include <ctype.h>
#include <stdint.h>

#include "common.h"
#include "msg_struct.h"

// Prototypes des fonctions annexes
int initialiser_connexion(const char *ip, const char *port_str);
int valider_pseudo(const char *pseudo);
void envoyer_requete(int fd, uint16_t type, const char *infos, const char *contenu, int taille_contenu);
void afficher_message_serveur(struct message reponse, const char *texte);
void traiter_reception_serveur(int client_fd, char *buffer);

#endif