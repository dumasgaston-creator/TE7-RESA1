#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"

void die(int ret, char* msg){
	if(ret<0){
		perror(msg);
		exit(EXIT_FAILURE);
	}

}

int main(int argc, char *argv[]) {

	if (argc =! 2){
		die(argc, " Mets l'argument\n");
	}
	int port = atoi(argv[1]);

	int listen_fd = socket(AF_INET,SOCK_STREAM, 0);
    die(listen_fd, "On creating");

    

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;// s.addr stocke l'@ ip sous forme de bit

    int ret_value;
    ret_value = bind( listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    die(ret_value, "on binding");

    ret_value = listen(listen_fd, BACKLOG);
    die(ret_value, "on listening");  
    
    struct sockaddr_in client_addr;    
    socklen_t adress_len = sizeof(struct sockaddr_in);  
    return EXIT_SUCCESS;
}