#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "common.h"

void die(int ret, char* msg){
	if(ret<0){
		perror(msg);
		exit(EXIT_FAILURE);
	}

}

int main(int argc, char *argv[]) {

	if (argc != 2){
		fprintf(stderr, "Il faut un numero de port en argument : %s\n", argv[0]);
        exit(EXIT_FAILURE);
	}
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


    struct pollfd fds[MAX_CLIENTS];// 
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
			
			

			}
		}
	}
    return EXIT_SUCCESS;
}