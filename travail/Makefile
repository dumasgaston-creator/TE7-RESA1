# CFLAGS=-Wall -Werror
# LDFLAGS=-lpthread
# all: client server
# clean:
# 	rm -f client server

CFLAGS=-Wall -Werror
LDFLAGS=-lpthread

all: client server

client: client.c reseau.c gestion_messages.c
	gcc $(CFLAGS) -o client client.c reseau.c gestion_messages.c $(LDFLAGS)

clean:
	rm -f client server