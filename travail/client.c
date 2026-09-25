#include<stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include<netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#include <poll.h>

#include "common.h"

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

		if(fds[0].revents & POLLIN){
			int lu = read(0, buffer, BUFFER_SIZE - 1);
			buffer[lu] = '\0';

			if(strcmp(buffer, "/quit\n") == 0){ // Req 1.7
				close(client_fd);
				exit(EXIT_SUCCESS);
			}

			else{
				write(client_fd, &lu, sizeof(int));
				write(client_fd, buffer, lu);
			}
		}

		if(fds[1].revents & POLLIN){
			int taille_msg;
			int ret = read(client_fd, &taille_msg, sizeof(int));
			if(0 == ret){
				printf("Le serveur est déconnecté");
				close(client_fd);
				exit(EXIT_FAILURE);
			}

			int lu_serveur = read(client_fd, buffer, taille_msg);
			buffer[lu_serveur] = '\0';
			printf("Serveur a lu et repond : %s\n", buffer);

		}
	}
}