#ifndef _API_H_
#define _API_H_

#include "Sock.h"

class Api{
    public:
        Api(const char* ip, unsigned int port);
        ~Api();
        void start(void);
    private:
    
};

#endif