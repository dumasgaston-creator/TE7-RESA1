#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "common.h"

//req 1.8 declaration du struct qui stocke les données client
struct client_info {
	int fd;
	int port;
	char ip[16];//suffit pour une ipv4
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
						break;
					}
				}
			}
		// si c le client existant qui envoie des données
			else if (i > 0 && (fds[i].revents & POLLIN)){
				int taille_msg;
				int lu_client = read(fds[i].fd, &taille_msg, sizeof(int));
				
				// Si client deconnecté ou erreur 
				if (lu_client <= 0){
					printf("Client de la socekt %d s'est deconnecté\n", fds[i].fd);
					close(fds[i].fd);
					fds[i].fd = -1;
					
				}
				// Si on a reçu la taille du message on renvoie le meme message au client 
				else if(lu_client > 0){
					char buffer[taille_msg + 1];
					lu_client = read (fds[i].fd, buffer, taille_msg);
					buffer[lu_client]='\0';
					int envoi_msg = write(fds[i].fd, &taille_msg,sizeof(int));
					envoi_msg = write(fds[i].fd, buffer,taille_msg);
					die(envoi_msg, "Erreur lors de l'envoi du msg\n");
					printf("Client de la socket %d dit : %s\n",fds[i].fd, buffer);

				}
				
			}
		}
	}
    return EXIT_SUCCESS;
}