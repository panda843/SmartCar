#include "Client.h"

Client::Client(){

}

Client::~Client(){
    
}

void Client::start(const char* ip,unsigned int port){
    struct sockaddr_in sin;  
    memset(&sin, 0, sizeof(sin));  
    sin.sin_family = AF_INET;  
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_port = htons(port);  
    // build event base  
    this->baseEvent = event_base_new();
    this->client_fd = socket(AF_INET, SOCK_STREAM, 0);  
    this->connect = bufferevent_socket_new(this->baseEvent,this->client_fd,BEV_OPT_CLOSE_ON_FREE); 
    int enable = 1;
    if(setsockopt(this->client_fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable)) < 0){
        printf("ERROR: TCP_NODELAY SETTING ERROR!\n");
    }
    bufferevent_enable(this->connect, EV_WRITE);
    if(bufferevent_socket_connect(this->connect,(struct sockaddr*)&sin,sizeof(sin)) == 0){
        printf("connect success\n");
    }

    // // start to send data  
    // bufferevent_write(this->connect,mesg,length);  
    // // check the output evbuffer  
    // struct evbuffer* output = bufferevent_get_output(conn);  
    // int len = 0;  
    // len = evbuffer_get_length(output);  
    // printf("output buffer has %d bytes left\n", len);  
    
    event_base_dispatch(this->baseEvent);  
    
    // free(mesg);  
    // mesg = NULL;  
    
    bufferevent_free(this->connect);  
    event_base_free(this->baseEvent);
}