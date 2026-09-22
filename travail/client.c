#include<stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include<netdb.h>
#include <arpa/inet.h>

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

	//Req 1.4

	char buffer[256];
	int msg_size = strlen(buffer);

	int size_sent = write(client_fd, &msg_size, sizeof(int));
	size_sent = write(client_fd, client_fd, msg_size);

	// Req 1.5

	struct pollfd fds[2];
	fds[0].fd = 0;
    fds[0].events = POLLIN;

	fds[1].fd = client_fd;
    fds[0].events = POLLIN;



}