#include "client.h"

int main(int argc, char *argv[]){

	// Req 1.1
	if(argc != 3){
		printf("Mauvais nombre d'arguments");
		exit(EXIT_FAILURE);
	}

	int client_fd = initialiser_connexion(argv[1], argv[2]);

	// Req 1.5
	struct pollfd fds[3];
	fds[0].fd = 0;
    fds[0].events = POLLIN;
	fds[0].revents = 0;

	fds[1].fd = client_fd;
    fds[1].events = POLLIN;
	fds[1].revents = 0;

    fds[2].fd = -1; // pour que poll ignore
    fds[2].events = POLLIN;
    fds[2].revents = 0;

	// Req 1.4
	char buffer[BUFFER_SIZE];

    // comm entre clients
    int attente_transfert = 0;
    char demandeur[INFOS_LEN] = {0};
    char fichier_attendu[BUFFER_SIZE] = {0};

	while(1){
		int nbfds = poll(fds, 3, -1);
		if(-1 == nbfds){
			perror("Interruption");
		}

		if(fds[0].revents & POLLIN){  // Si activité du clavier

			int lu = read(0, buffer, BUFFER_SIZE - 1);
			buffer[lu] = '\0';

            if(1 == attente_transfert){
                if (buffer[0] == 'N' || buffer[0] == 'n'){
                    envoyer_requete(client_fd, FILE_REJECT, demandeur, NULL, 0);
                    attente_transfert = 0;
                }
                else if (buffer[0] == 'Y' || buffer[0] == 'y'){
                    int port = 8081; 
                    int listen_fd = initialiser_serveur_client(port); // socket de nos clients

                    // Préparation adresse à envoyer
                    char adresse[BUFFER_SIZE];
                    sprintf(adresse, "127.0.0.1:%d", port);

                    // Envoi du FILE_ACCEPT
                    envoyer_requete(client_fd, FILE_ACCEPT, demandeur, adresse, strlen(adresse));
    
                    printf("Transfert accepté. En attente de la connexion...\n");

                    attente_transfert = 0;

                    fds[2].fd = listen_fd; // hop écoute
                }
                else{
                    printf("Veuillez répondre par Y ou N :  ");
                    //fflush(stdout);
                }
                continue;
            }

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
                else if(0 == strncmp(buffer, "/send ", 6)){
                    char *ptr = strchr(buffer + 6, ' ');
                    if (ptr == NULL) { //segfault si pas d'argument après /send sans ça
                        printf("Erreur de syntaxe. Utilisation : /send <pseudo> <fichier>\n");
                        continue; // on reprend au début
                    }
                    *ptr = '\0';

                    char *chemin_complet = ptr + 1;
                    char *newline = strchr(chemin_complet, '\n');
                    if (newline != NULL) {
                        *newline = '\0';
                    }
                    char *nom_fichier = chemin_complet;
                    char *dernier_slash = strrchr(chemin_complet, '/');
        
                    if (dernier_slash != NULL) {
                        nom_fichier = dernier_slash + 1;
                    }

                    printf("Demande d'envoi du fichier '%s'...\n", nom_fichier);
                    envoyer_requete(client_fd, FILE_REQUEST, buffer + 6, chemin_complet, strlen(chemin_complet));
                    *ptr = ' '; 
                }
                else {
                    envoyer_requete(client_fd, ECHO_SEND, NULL, buffer, lu);
                }  
        }

		if(fds[1].revents & POLLIN){  //si activité du serveur
			traiter_reception_serveur(client_fd, buffer, &attente_transfert, demandeur, fichier_attendu);
		}

        if(fds[2].revents & POLLIN){  //qqn se connecte
            struct sockaddr_in adresse_emetteur;
            socklen_t taille_adresse = sizeof(adresse_emetteur);

            // accept + tuyau
            int socket_emetteur = accept(fds[2].fd, (struct sockaddr *)&adresse_emetteur, &taille_adresse);
            die(socket_emetteur, "erreur accept emetteur");

            printf("L'émetteur est connecté ! Prêt à recevoir le fichier.\n");

            recevoir_fichier(socket_emetteur, fichier_attendu);

            close(fds[2].fd); 
            fds[2].fd = -1; // re désactive
	    }
    }
    
}