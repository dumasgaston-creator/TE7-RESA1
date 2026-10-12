#include "client.h"
#include <fcntl.h>

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

void recevoir_fichier(int socket_emetteur, const char *nom_fichier) {
    // notre fichier
    int fd_fichier = open(nom_fichier, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if (fd_fichier < 0) {
        perror("Erreur lors de la création du fichier sur le disque");
        //si peut pas
        close(socket_emetteur);
        return; 
    }

    char buffer_fichier[BUFFER_SIZE];
    ssize_t octets_recus; //signed

    printf("Début du téléchargement du fichier '%s'...\n", nom_fichier);

    while ((octets_recus = read(socket_emetteur, buffer_fichier, BUFFER_SIZE)) > 0) {
        
        // éccit nb octets
        ssize_t octets_ecrits = write(fd_fichier, buffer_fichier, octets_recus);
        
        if (octets_ecrits < 0) {
            perror("Erreur pendant l'écriture sur le disque");
            break;
        }
    }

    // verif
    if (octets_recus < 0) {
        perror("Erreur réseau pendant la réception");
    } else {
        printf("Transfert terminé avec succès. Fichier '%s' enregistré.\n", nom_fichier);
    }

    close(fd_fichier);
    close(socket_emetteur);
}