#define MSG_LEN 1024
#define SERV_PORT "8080"
#define SERV_ADDR "127.0.0.1"
#define MAX_CLIENTS 100
#define BUFFER_SIZE 256


struct info{
    short s;
    long l;
};

void die(int ret, char* msg){
	if(ret<0){
		perror(msg);
		exit(EXIT_FAILURE);
	}

}