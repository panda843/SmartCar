#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include "event/event.h"  
#include "event/bufferevent.h" 
#include "event/buffer.h"
#include <netinet/tcp.h>
#include <arpa/inet.h>

class Client{
public:
    Client();
    ~Client();
    void start(const char* ip,unsigned int port);
private:
    evutil_socket_t client_fd;
    struct bufferevent* connect;
    struct event_base* baseEvent;
};

#endif  