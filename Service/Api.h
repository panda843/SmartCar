#ifndef _API_H_
#define _API_H_

#include <event2/event.h>  
#include <event2/http.h>  
#include <event2/buffer.h>

class Api{
    public:
        Api(const char* ip, unsigned int port);
        ~Api();
        void start();
        friend void requestHandler(struct evhttp_request *request, void *args);
    private:
        const char* ip;
        unsigned int port;
        struct evhttp* httpServer = NULL;
        struct event_base* eventBase = NULL;
};

#endif
