#ifndef _API_H_
#define _API_H_

#include <map>
#include <exception>
#include <ctype.h>
#include <string.h>
#include <event2/event.h>  
#include <event2/http.h>  
#include <event2/buffer.h>

#define FAVICON "favicon.ico"

using namespace std;

class Api{
    public:
        Api(const char* ip, const unsigned int port);
        ~Api();
        void start();
        friend void requestHandler(struct evhttp_request *request, void *args);
        char* strlwr(char* str);
    private:
        char* ip;
        char* request_action;
        unsigned int port;
        struct evhttp* httpServer = NULL;
        struct event_base* eventBase = NULL;
        void getRquestAction(const char* url);
        void initApiList();
        void user_login();
        void call(const char* str);
};

#endif
