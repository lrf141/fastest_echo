#ifndef _NETWORK_H
#define _NETWORK_H

#include <net/sock.h> //struct socket

#define DEFAULT_PORT 8888

struct socket *server;
struct socket *client;
struct sockaddr_in socket_in;




#endif
