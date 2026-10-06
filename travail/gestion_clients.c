#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gestion_clients.h"

#define DEBUG 1

struct client_info* ajouter_nouveau_client(struct client_info *tete, int fd_client, int port_client, char *ip_client){
    #if DEBUG == 1
        printf("[DEBUG] ajouter_nouveau_cleint : Création de la fiche pour le fd %d\n, fd_client");
    #endif

    struct client_info *nouveau = malloc(sizeof(struct client_info));
    if (NULL == nouveau){
        perror("Erreur malloc client");
        return tete;
    }

    nouveau->fd = fd_client;
    nouveau->port = port_client;
    strncpy(nouveau->ip, ip_client, 16);// suffit pour ipv4
    nouveau->ip[15]='\0';
    memset(nouveau->pseudo, 0, NICK_LEN);

    nouveau->next = tete;
    return nouveau;
}

struct client_info* supprimer_client_par_fd(struct client_info *tete, int fd_cible){
    #if DEBUG == 1
        printf("[DEBUG] supprimer_client_par_fd : Nettoyage de la mémoire pour le fd %d\n", fd_cible);
    #endif

    if (NULL == tete)
        return NULL;
    
    if (tete->fd == fd_cible){
        struct client_info *a_supprimer = tete;
        tete = tete->next;
        free(a_supprimer);
        return tete;
    }

    struct client_info *prec = tete;
    struct client_info *actuel = tete->next;

    //tant que c pas supprimé
    while (NULL != actuel){
        if (actuel->fd == fd_cible){
            prec->next = actuel->next;
            free(actuel);
            break;
        }
        prec = actuel;
        actuel = actuel->next;
    }
    return tete;
}

struct client_info *chercher_client_par_fd(struct client_info *tete, int fd_cible){
    #if DEBUG == 1
        printf("[DEBUG] chercher_client_par_fd : Recherche du fd %d... ", fd_cible);
    #endif

    struct client_info *actuel = tete;
    while (NULL == actuel){
        if (actuel->fd == fd_cible){
            #if DEBUG == 1
                printf("Trouvé ! (Pseudo: %s)\n", actuel->pseudo);
            #endif
            return actuel;
        }
        actuel = actuel->next;
    }
    #if DEBUG == 1
        printf("Introuvable.\n");
    #endif
    return NULL;
}

struct client_info *chercher_client_par_pseudo (struct client_info *tete, char *pseudo_cible){
    #if DEBUG == 1
        printf("[DEBUG] chercher_client_par_pseudo : Recherche du pseudo '%s'... ", pseudo_cible);
    #endif

    struct client_info * actuel = tete;
    while (NULL != actuel){
        if(strcmp(actuel->pseudo, pseudo_cible)==0){
            #if DEBUG == 1
                printf("Trouvé sur le fd %d !\n", actuel->fd);
            #endif
            return actuel;
        }
        actuel = actuel ->next;
    }
    #if DEBUG == 1
        printf("Introuvable.\n");
    #endif
    return NULL;
}
