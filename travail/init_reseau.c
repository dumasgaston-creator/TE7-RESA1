#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "init_reseau.h"

#define DEBUG 1 

void die(int ret, char* msg){
    if(ret < 0){
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

int demarrer_serveur_ecoute(int port){
    #if DEBUG == 1
        printf("[DEBUG] Démarrage : Création de la socket...\n");
    #endif
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);//ipv4 & protocole TCP 
    die (listen_fd, "erreur creation");

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;//ipv4
    server_addr.sin_port = htons(port); 
    server_addr.sin_addr.s_addr = INADDR_ANY;//ecoute sur toutes les interfaces de l'ordi
    
    #if DEBUG == 1
        printf("[DEBUG] Démarrage : port %d (bind)...\n", port);
    #endif
    int ret_value = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    die(ret_value,"erreur bind");

    return listen_fd; 
}