#include "client.h"

int initialiser_connexion(const char *ip, const char *port_str) {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == client_fd) {
        perror("Erreur création de la socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port_str));
    inet_aton(ip, &server_addr.sin_addr);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur en cours de connexion");
        exit(EXIT_FAILURE);
    }
    return client_fd;
}

int initialiser_serveur_client(int port) {
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    client_addr.sin_addr.s_addr = INADDR_ANY;
    
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int ret_value;
    
    ret_value = bind(listen_fd, (struct sockaddr *)&client_addr, sizeof(client_addr));
    die(ret_value, "erreur bind");

    ret_value = listen(listen_fd, SOMAXCONN);
    die(ret_value, "erreur listen");  
    
    printf("Serveur - Client temporaire en écoute sur le port %d...\n", port);
    
    return listen_fd; //ce qu'on veut quoi
}