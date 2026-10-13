#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "routage_messages.h"
#include "common.h"

#define DEBUG 1

void traiter_nouveau_pseudo(struct client_info *tete, int fd_client, struct message *msg){
    #if DEBUG == 1
        printf("[DEBUG] routage : Changement de pseudo demandé par le fd %d -> %s\n", fd_client, msg->infos);
    #endif

    struct client_info *client = chercher_client_par_fd(tete, fd_client);
    if (NULL != client){
        strncpy(client->pseudo, msg->infos, NICK_LEN);
        client->pseudo[NICK_LEN -1] = '\0';//secu
        printf("Succès : Le client sur la socket %d s'appelle maintenant %s\n", fd_client, client->pseudo);

    }
}

void diffuser_message_public(struct client_info *tete, int fd_expediteur, struct message *msg, char *buffer){
    #if DEBUG == 1
        printf("[DEBUG] routage :  le fd expediteur est %d\n", fd_expediteur);
    #endif

    // On trouve qui parle ?
    struct client_info *expediteur = chercher_client_par_fd(tete, fd_expediteur);
    if (NULL != expediteur){
        strncpy(msg->nick_sender, expediteur->pseudo, NICK_LEN);
        msg->nick_sender[NICK_LEN-1]= '\0';
    }
    // On envoie à tt le monde SAUF EXPEDITEUR
    struct client_info *actuel = tete;
    while (NULL != actuel){
        if (actuel->fd != fd_expediteur){
            write(actuel->fd, msg, sizeof(struct message));
            if (msg->pld_len > 0 && NULL != buffer){
                write(actuel->fd, buffer, msg->pld_len);
            }
        }
        actuel = actuel->next;
    }
}
void envoyer_message_prive(struct client_info *tete, int fd_expediteur, struct message *msg, char *buffer){
    #if DEBUG == 1
        printf("[DEBUG] routage :  le fd expediteur est %d\n", fd_expediteur);
    #endif
    // On trouve qui parle ?
    struct client_info *expediteur = chercher_client_par_fd(tete, fd_expediteur);
    if (NULL != expediteur){
        strncpy(msg->nick_sender, expediteur->pseudo, NICK_LEN);
        msg->nick_sender[NICK_LEN-1]= '\0';
    }

    
    
    //On cherche le destinataire 

    struct client_info *destinataire = chercher_client_par_pseudo(tete, msg->infos);
    if(NULL != destinataire){
        write(destinataire->fd, msg, sizeof(struct message));
        if(msg->pld_len >0 && NULL != buffer){
            write(destinataire->fd, buffer, msg->pld_len);
        }
    } 
    else {
        #if DEBUG == 1
            printf("[DEBUG] routage : Destinataire '%s' introuvable.\n", msg->infos);
        #endif

        char buf[MSG_LEN];
        sprintf(buf, "Erreur : L'utilisateur '%s' n'existe pas ou n'est pasconnecté\n.", msg->infos);

        struct message reponse_erreur;
        memset(&reponse_erreur, 0, sizeof(struct message));
        reponse_erreur.type = UNICAST_SEND;
        reponse_erreur.pld_len = strlen(buf);
        strncpy(reponse_erreur.nick_sender, " Serveur de LAGAST", NICK_LEN);
        reponse_erreur.nick_sender[NICK_LEN -1] = '\0';
        write(fd_expediteur, &reponse_erreur, sizeof(struct message));
        write(fd_expediteur, buf, reponse_erreur.pld_len);
    }   
}

void envoyer_annuaire_clients(struct client_info *tete, int fd_demandeur){
    #if DEBUG == 1
        printf("[DEBUG] routage : Le fd %d demande l'annuaire.\n", fd_demandeur);
    #endif

    char buf[1024];
    strcpy(buf, "Les utilisateurs qui sont connectés sont : \n");
    struct client_info *actuel = tete;
    while ( NULL != actuel){
        strcat(buf, "- ");
        //Si pseudo est vide ou non
        if(strlen(actuel->pseudo) > 0){
            strcat(buf, actuel->pseudo);
        }
        else {
            strcat(buf, "Inconnu");
        }
        strcat (buf, "\n");
        actuel = actuel->next;
    }

    struct message reponse_serveur;
    memset(&reponse_serveur, 0, sizeof(struct message));
    reponse_serveur.type = NICKNAME_LIST;
    reponse_serveur.pld_len = strlen(buf);
    strncpy(reponse_serveur.nick_sender, "Serveur", NICK_LEN);
    reponse_serveur.nick_sender[NICK_LEN-1]= '\0';

    write(fd_demandeur, &reponse_serveur, sizeof(struct message));
    write(fd_demandeur, buf, reponse_serveur.pld_len);
}

void envoyer_infos_client(struct client_info *tete, int fd_demandeur, char *pseudo_cible){
    #if DEBUG == 1
        printf("[DEBUG] routage : Le fd %d demande un WHOIS sur '%s'\n", fd_demandeur, pseudo_cible);
    #endif

    char buf[MSG_LEN];
    struct client_info *cible = chercher_client_par_pseudo(tete, pseudo_cible);
    
    if (NULL != cible){
        // Si client trouvé
        sprintf(buf, "L'utilisateur %s est connecté depuis l'IP %s sur le port %d", cible->pseudo, cible->ip, cible->port);

    }else{
        sprintf(buf, "erreur : l'utilisateur '%s' est introuvable.", pseudo_cible);
    }
    struct message reponse_serveur;
    memset(&reponse_serveur, 0, sizeof(struct message));
    reponse_serveur.type = NICKNAME_INFOS;
    reponse_serveur.pld_len = strlen(buf);
    strncpy(reponse_serveur.nick_sender, "LE SERVEUR GENIAL DE LAGAST", NICK_LEN);
    reponse_serveur.nick_sender[NICK_LEN -1]='\0';

    write(fd_demandeur, &reponse_serveur, sizeof(struct message));
    write(fd_demandeur, buf, reponse_serveur.pld_len);
}
