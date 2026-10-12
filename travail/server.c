#include "server.h"

int main(int argc, char *argv[]) {

    if (argc != 2){
        fprintf(stderr, "Il faut un numero de port en argument : %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct client_info *tete = NULL; // on initialise les données à zero (req 1.8)
    int port = atoi(argv[1]);

    int listen_fd = demarrer_serveur_ecoute(port);
    
    printf("Serveur en écoute sur le port %d...\n", port);

    struct pollfd fds[MAX_CLIENTS];
    fds[0].fd = listen_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    for (int i = 1; i < MAX_CLIENTS; i++){
        fds[i].fd = -1;
        fds[i].events = POLLIN; 
    }

    //poll surveille pls fd ( sockets) en meme temps
    while(1){
        if (poll(fds, MAX_CLIENTS, -1) < 0){
            perror("Erreur poll");
            break;
        }
        
        // Si case vide -> RIEN
        for (int i = 0; i < MAX_CLIENTS; i++){
            if (fds[i].fd== -1 || fds[i].revents == 0){
                continue;
            }
        
            // Si fds[0] reçoit un POLLIN
            if (i == 0 && (fds[i].revents & POLLIN)){
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(struct sockaddr_in);

                // on accepte le nouveau client
                int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
                die(client_fd, "Erreur accept \n");
                printf("Nouveau client [%d] connecté\n", client_fd);

                //BOOM lets go ça marche jpp (ajout en 1 ligne !)
                tete = ajouter_nouveau_client(tete, client_fd, ntohs(client_addr.sin_port), inet_ntoa(client_addr.sin_addr));

                // On range le nouveau fd dans le tableau poll
                for (int j = 1; j < MAX_CLIENTS; j++){
                    if (fds[j].fd == -1){
                        fds[j].fd = client_fd;
                        fds[j].events = POLLIN;
                        fds[j].revents = 0;
                        break;
                    }
                }
            }
            
            // si c le client existant qui envoie des données
            else if (i > 0 && (fds[i].revents & POLLIN)){

                struct message msg_recu;
                char *buffer = NULL;

                int lu_client = read(fds[i].fd, &msg_recu, sizeof(struct message));
                
                // Si client deconnecté ou erreur 
                if (lu_client <= 0){
                    printf("Client de la socket %d s'est deconnecté\n", fds[i].fd);
                    
                    // BOOM ! Le grand ménage de la mémoire en une seule ligne :
                    tete = supprimer_client_par_fd(tete, fds[i].fd);

                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                
                // Si on a reçu l'enveloppe (req)' on lit la suite
                else if(lu_client > 0){
                    
                    // Si texte attaché à l'enveloppe, on le recupere
                    if (msg_recu.pld_len > 0){
                        buffer = malloc(msg_recu.pld_len + 1);
                        if(buffer != NULL){
                            lu_client = read(fds[i].fd, buffer, msg_recu.pld_len);
                            if (lu_client >= 0) 
                                buffer[lu_client]='\0';
                        }
                    }

                    //CLient veut faire quoi (Notre routeur prend le relais)
                    switch(msg_recu.type){
                        
                        // CLIENT CHANGE DE PSEUDO
                        case NICKNAME_NEW:
                            traiter_nouveau_pseudo(tete, fds[i].fd, &msg_recu);
                            break;

                        // CLIENT DIFFUSE UN MESSAGE
                        case BROADCAST_SEND : 
                            diffuser_message_public(tete, fds[i].fd, &msg_recu, buffer);
                            break;
                            
                        // CLIENT ENVOIE MESSAGE PRIVE + P2P
                        case UNICAST_SEND : 
                        case FILE_REQUEST :
                        case FILE_ACCEPT :
                        case FILE_REJECT : 
                            envoyer_message_prive(tete, fds[i].fd, &msg_recu, buffer);
                            break;
                            
                        // DEMANDE LISTE DES UTILISATEURS
                        case NICKNAME_LIST:
                            envoyer_annuaire_clients(tete, fds[i].fd);
                            break;

                        // TEST ECHO
                        case ECHO_SEND:
                            printf("Le client veut faire un echo \n");
                            write(fds[i].fd, &msg_recu, sizeof(struct message));
                            
                            // On renvoie le texte s'il y en a un
                            if (msg_recu.pld_len > 0 && buffer != NULL) {
                                write(fds[i].fd, buffer, msg_recu.pld_len);
                            }
                            break;
                        
                        default :
                            printf("Cas non géré par le serveur \n");
                            break;
                    }
                    
                    // TJS FREE
                    if (buffer != NULL) {
                        free(buffer);
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS;
}