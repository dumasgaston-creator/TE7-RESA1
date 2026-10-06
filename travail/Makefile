CC=gcc
CFLAGS=-Wall -Werror
LDFLAGS=-lpthread

all: client server


server: server.c gestion_clients.c routage_messages.c init_reseau.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client: client.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f client server