#include<stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include<netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#include <poll.h>
#include<ctype.h>

#include "common.h"
#include "msg_struct.h"

int main(int argc, char *argv[]){

	// Req 1.1

	if(argc != 3){
		printf("Mauvais nombre d'arguments");
		exit(EXIT_FAILURE);
	}

	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == client_fd){
		printf("Erreur création de la socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &server_addr.sin_addr);

	int ret_value;
    ret_value = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret_value == -1){
        perror("En cours de connexion");
        exit(EXIT_FAILURE);
    }

	// Req 1.5

	struct pollfd fds[2];
	fds[0].fd = 0;
    fds[0].events = POLLIN;
	fds[0].revents = 0;

	fds[1].fd = client_fd;
    fds[1].events = POLLIN;
	fds[1].revents = 0;

	// Req 1.4
	char buffer[BUFFER_SIZE];
	while(1){
		int nbfds = poll(fds, 2, -1);
		if(-1 == nbfds){
			perror("Interruption");
		}

		if(fds[0].revents & POLLIN){  // Si activité du clavier

			int lu = read(0, buffer, BUFFER_SIZE - 1);
			buffer[lu] = '\0';

			if(strcmp(buffer, "/quit\n") == 0){ // Req 1.7
				close(client_fd);
				exit(EXIT_SUCCESS);
			}

			else if(0 == strncmp(buffer, "/nick ", 6)){ // gestion du pseudo
				struct message req;
				memset(&req, 0, sizeof(struct message)); 
				req.type = NICKNAME_NEW;
				req.pld_len = 0;  //cf énoncé

				strncpy(req.infos, buffer + 6, INFOS_LEN - 1);
				req.infos[strcspn(req.infos, "\n")] = '\0';

				// Vérif de la validité du pseudo
				int valide __attribute__((unused)) = 1;
				int len = strlen(req.infos);
				if(0 == len || len >= NICK_LEN){ //verif longueur
					valide = 0;
					printf("Pseudo vide ou trop long\n");
				}
				else{
					for(int i = 0; i < len; i++){   //verif caractères
						if(!isalnum(req.infos[i])){
                            valide = 0;
                            printf("Erreur : Le pseudo ne doit contenir que des lettres et des chiffres.\n");
                            break;
						}
					}
				}

				// expedition
				if(1 == valide){
					write(client_fd, &req, sizeof(struct message));
				}
			}

			else if(0 == strcmp(buffer, "/who\n")){  // REQ 2.5
				struct message req;
				memset(&req, 0, sizeof(struct message)); 
				req.type = NICKNAME_LIST;
				req.pld_len = 0;
				write(client_fd, &req, sizeof(struct message));
			}

			else if(0 == strncmp(buffer, "/whois ", 7)){  // REQ 2.6
				struct message req;
				memset(&req, 0, sizeof(struct message)); 
				req.type = NICKNAME_INFOS;
				req.pld_len = 0;
				strncpy(req.infos, buffer + 7, INFOS_LEN - 1);
				req.infos[strcspn(req.infos, "\n")] = '\0';
				write(client_fd, &req, sizeof(struct message));
			}

			else if(0 == strncmp(buffer, "/msgall ", 8)){  // REQ 2.7
				struct message req;
				memset(&req, 0, sizeof(struct message)); 
				req.type = BROADCAST_SEND;
				req.pld_len = strlen(buffer + 8);
				write(client_fd, &req, sizeof(struct message));
				write(client_fd, buffer + 8, req.pld_len);
			}

			else if(0 == strncmp(buffer, "/msg ", 5)){  // REQ 2.9
				char *ptr = strchr(buffer + 5, ' '); //pour gérer les epaces
				if(NULL == ptr){  // on sait jamais
					printf("pas de message\n");
					continue;  // pour renvoyer au début du while
				}
				else{  //transmission message + pseudo destinatauire au serveur
					struct message req;
					memset(&req, 0, sizeof(struct message));
					req.type = UNICAST_SEND;
					strncpy(req.infos, buffer + 5, ptr - (buffer + 5));
					req.infos[strcspn(req.infos, "\n")] = '\0';
					req.pld_len = strlen(ptr + 1);
					write(client_fd, &req, sizeof(struct message));
					write(client_fd, ptr + 1, req.pld_len);
				}
			}

			else if(0 == strncmp(buffer, "/send ", 6)){
				struct message req;
				memset(&req, 0, sizeof(struct message));
				req.type = FILE_REQUEST;
				req.pld_len = strlen(buffer + 6);
			}

			else{
				struct message req;
				memset(&req, 0, sizeof(struct message)); 

				// remplissage info obligatoires
				req.pld_len = lu;  //taille du texte qu'on a au clavier
				req.type = ECHO_SEND;  //simple echo pour le serveur

				// expedition
				write(client_fd, &req, sizeof(struct message)); 
				write(client_fd, buffer, req.pld_len);
			}
		}

		if(fds[1].revents & POLLIN){  //si activité du serveur
			struct message reponse;
			int ret = read(client_fd, &reponse, sizeof(struct message));
			if(0 == ret){
				printf("Le serveur est déconnecté\n");
				close(client_fd);
				exit(EXIT_FAILURE);
			}

			int lu_serveur = read(client_fd, buffer, reponse.pld_len);
			buffer[lu_serveur] = '\0';
			printf("Serveur a lu ton message qui disait : %s\n", buffer);

		}
	}
}