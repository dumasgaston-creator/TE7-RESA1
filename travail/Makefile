CC=gcc
CFLAGS=-Wall -Werror
LDFLAGS=-lpthread

all: client server

server: server.c gestion_clients.c routage_messages.c init_reseau.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Ajoute bien tous les .c nécessaires à côté de client.c
client: client.c reseau.c gestion_messages.c 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f client server