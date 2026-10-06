#ifndef GESTION_CLIENT_H
#define GESTION_CLIENT_H

#include "msg_struct.h"

struct client_info {
    int fd;
    int port;
    char ip[16];
    char pseudo[NICK_LEN];
    struct client_info *next;
};

struct client_info* ajouter_nouveau_client(struct client_info *tete, int fd_client, int port_client, char *ip_client);
struct client_info* supprimer_client_par_fd(struct client_info *tete, int fd_cible);
struct client_info* chercher_client_par_fd(struct client_info *tete, int fd_cible);
struct cleint_info* chercher_client_par_pseudo(struct client_info *tete, char *pseudo_cible);

#endif