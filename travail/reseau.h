#ifndef RESEAU_H
#define RESEAU_H

int initialiser_connexion(const char *ip, const char *port_str);
int initialiser_serveur_client(int port);
void recevoir_fichier(int socket_emetteur, const char *nom_fichier);

#endif