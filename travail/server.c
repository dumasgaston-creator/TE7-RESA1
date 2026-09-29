
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "common.h"
#include "msg_struct.h"

//req 1.8 declaration du struct qui stocke les données client
struct client_info {
	int fd;
	int port;
	char ip[16];//suffit pour une ipv4
	char pseudo[NICK_LEN];// prend en compte le pseudo du client
	struct client_info *next;// lier ce client au prochain dans la memeoire
};

int main(int argc, char *argv[]) {

	if (argc != 2){
		fprintf(stderr, "Il faut un numero de port en argument : %s\n", argv[0]);
        exit(EXIT_FAILURE);
	}
	struct client_info *tete = NULL; // on initialise les données à zero (req 1.8)
	int port = atoi(argv[1]);

	int listen_fd = socket(AF_INET,SOCK_STREAM, 0);
    die(listen_fd, "erreur create");

    

    struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));//mettre à zero la structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;// s.addr stocke l'@ ip sous forme de bit

    int ret_value;
	//bind lie une socket à une adresse IP (port. + @ip)
    ret_value = bind( listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    die(ret_value, "erreur bind");

	// listen_fd est modifiée
    ret_value = listen(listen_fd, SOMAXCONN);// const definit la taille max de la file d'attente
    die(ret_value, "erreur listen");  
    
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
			if (fds[i].fd == -1 || fds[i].revents == 0){
				continue;
			}
		
		// Si fds[0] reçoit un POLLIN
			if (i == 0 && (fds[i].revents & POLLIN)){
				struct sockaddr_in client_addr;
				socklen_t addr_len = sizeof(struct sockaddr_in);

		// on accepte le nouveau client
				int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
				die(client_fd, "Erreur accept \n");
				printf("Nouveau client connecté ! \n");
				//allocation dynamique
				struct client_info *new_client = malloc(sizeof(struct client_info));

				if (new_client == NULL){
					perror("Erreur malloc\n");
		
				}else {
					// On remplit la structure avec les données du client
					new_client->fd = client_fd;
					new_client->port = ntohs(client_addr.sin_port);//network to host 
					strcpy(new_client->ip, inet_ntoa(client_addr.sin_addr));//inet_notoa -> @ip dans client_addr binaire en char[]
					//strcpy la copie dans le tableau de char 

					// Insertion dans la liste chainée
					new_client->next = tete;
					tete = new_client;

					

				}

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
				struct client_info *actuel = NULL;

				int lu_client = read(fds[i].fd, &msg_recu, sizeof(struct message));
				
				
				// Si client deconnecté ou erreur 
                if (lu_client <= 0){
                    printf("Client de la socket %d s'est deconnecté\n", fds[i].fd);
                    
                    // req
                    // Cas 1 : Le client à supprimer est la tête
                    if (tete != NULL && tete->fd == fds[i].fd) {
                        struct client_info *a_supprimer = tete;
                        tete = tete->next; // La tête devient le maillon suivant
                        free(a_supprimer); // On détruit l'ancien
                    } 
                    // Cas 2 : Le client est au milieu ou à la fin
                    else if (tete != NULL) {
                        struct client_info *prec = tete;
                        actuel = tete->next;
                        
                        while (actuel != NULL) {
                            if (actuel->fd == fds[i].fd) {
                                prec->next = actuel->next; // On raccorde le précédent au suivant (on "saute" le client actuel)
                                free(actuel); 
                                break;
                            }
                      
                            prec = actuel;
                            actuel = actuel->next;
                        }
                    }

                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
				// Si on a reçu l'enveloppe (req)' on renvoie le meme message au client 
				else if(lu_client > 0){
					
					// Si texte attaché à l'enveloppe
					if (msg_recu.pld_len >0){
						buffer = malloc(msg_recu.pld_len + 1);
						if(buffer != NULL){
							lu_client = read(fds[i].fd, buffer, msg_recu.pld_len);
							if (lu_client >= 0) 
								buffer[lu_client]='\0';
							

    
						}
					}


					//CLient veut faire quoi
					
					switch(msg_recu.type){
						// CLIENT CHANGE DE PSEUDO
						case NICKNAME_NEW:
							printf ("Le client veut s'appeler : %s\n", msg_recu.infos);
							// Req2.1 : On cherche le client dans la liste pour lui donner son pseudo
							actuel = tete;
                            while (actuel != NULL) {
                                if (actuel->fd == fds[i].fd) {
                                    // On a trouvé le bon client ! On copie le pseudo
                                    strncpy(actuel->pseudo, msg_recu.infos, NICK_LEN);
				
                                    actuel->pseudo[NICK_LEN - 1] = '\0'; // Sécurité pour forcer la fin de chaîne
                                    printf("Succès : Le client sur la socket %d s'appelle maintenant %s\n", fds[i].fd, actuel->pseudo);
                                    break; // On a trouvé, on arrête de chercher
                                }
                                actuel = actuel->next; 
                            }
                            break;

						// CLIENT DIFFUSE UN MESSAGE
						case BROADCAST_SEND : 
							actuel = tete;
							while (actuel != NULL){
								if( actuel -> fd == fds[i].fd){
									strncpy(msg_recu.nick_sender, actuel->pseudo, NICK_LEN);
									msg_recu.nick_sender[NICK_LEN - 1]= '\0';
									printf("Super, le client sur la socket %d envoie le message : %s", fds[i].fd, actuel->pseudo);
									//boucle pour envoyer à tt le monde
									for (int j = 1; j < MAX_CLIENTS; j++){
										if(fds[j].fd != -1 && fds[j].fd != fds[i].fd){
											write(fds[j].fd, &msg_recu, sizeof(struct message));
											if (msg_recu.pld_len != 0)
												write(fds[j].fd, buffer, msg_recu.pld_len);
										}
									
									}
									break;
								
								}
								actuel = actuel->next;
							}
					
							
							break;
						case UNICAST_SEND : 
							printf("Le client veut envoyer un message privé à : %s\n", msg_recu.infos);
							//trouver expediteur
							actuel = tete;
							while(actuel!=NULL){
								if (actuel-> fd == fds[i].fd){
									strncpy(msg_recu.nick_sender, actuel->pseudo, NICK_LEN);
									msg_recu.nick_sender[NICK_LEN-1]='\0';
									break;
									
								}
								actuel = actuel->next;
							}
							// trouver le destinataire
								int trouve = 0;
								actuel = tete;
								while (actuel != NULL){
									if (strcmp(actuel->pseudo, msg_recu.infos) == 0){
										trouve = 1;

										write(actuel->fd, &msg_recu, sizeof(struct message));
										// si texte on envoit
										if(msg_recu.pld_len > 0){
											write(actuel->fd, buffer, msg_recu.pld_len);
										}
										break;
									}
									actuel = actuel->next;
								}
								if(trouve == 0){
									priintf("Impossible de livrer : le client '%s' n'existe pas .\n", msg_recu.infos);
									
								}

							

							break;
						case ECHO_SEND:
							printf("Le client veut faire un echo \n");
							write(fds[i].fd, &msg_recu, sizeof(struct message));
							
							// On renvoie le texte s'il y en a un
							if (msg_recu.pld_len > 0 && buffer != NULL) {
                                write(fds[i].fd, buffer, msg_recu.pld_len);
                                printf("Client de la socket %d dit : %s\n", fds[i].fd, buffer);
							}
							break;
						
						
						default :
							printf("Cas non géré par le serveur \n");
							break;
					

					
                    
                	}
					if (buffer != NULL) {
                        free(buffer);
					}
					
				}
			}
		}
    }
	return EXIT_SUCCESS;
}
