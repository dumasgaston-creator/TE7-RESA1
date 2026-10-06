#include "client.h"

int main(int argc, char *argv[]){

	// Req 1.1
	if(argc != 3){
		printf("Mauvais nombre d'arguments");
		exit(EXIT_FAILURE);
	}

	int client_fd = initialiser_connexion(argv[1], argv[2]);

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

			if (strcmp(buffer, "/quit\n") == 0) {
                    close(client_fd);
                    exit(EXIT_SUCCESS);
                }
                else if (0 == strncmp(buffer, "/nick ", 6)) {
                    if (valider_pseudo(buffer + 6)) {
                        envoyer_requete(client_fd, NICKNAME_NEW, buffer + 6, NULL, 0);
                    }
                }
                else if (0 == strcmp(buffer, "/who\n")) {
                    envoyer_requete(client_fd, NICKNAME_LIST, NULL, NULL, 0);
                }
                else if (0 == strncmp(buffer, "/whois ", 7)) {
                    envoyer_requete(client_fd, NICKNAME_INFOS, buffer + 7, NULL, 0);
                }
                else if (0 == strncmp(buffer, "/msgall ", 8)) {
                    envoyer_requete(client_fd, BROADCAST_SEND, NULL, buffer + 8, strlen(buffer + 8));
                }
                else if (0 == strncmp(buffer, "/msg ", 5)) {
                    char *ptr = strchr(buffer + 5, ' ');
                    if (ptr != NULL) {
                        *ptr = '\0'; 
                        envoyer_requete(client_fd, UNICAST_SEND, buffer + 5, ptr + 1, strlen(ptr + 1));
                        *ptr = ' ';  
                    } else {
                        printf("Erreur : pas de message fourni\n");
                    }
                }
                else {
                    envoyer_requete(client_fd, ECHO_SEND, NULL, buffer, lu);
                }  
        }

		if(fds[1].revents & POLLIN){  //si activité du serveur
			traiter_reception_serveur(client_fd, buffer);
		}
	}
}