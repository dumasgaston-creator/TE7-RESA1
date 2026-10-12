#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "common.h"
#include "msg_struct.h"
#include "routage_messages.h"
#include "gestion_clients.h"
#include "init_reseau.h"

void die(int ret, char* msg);