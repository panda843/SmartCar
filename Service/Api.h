#ifndef _API_H_
#define _API_H_

#include "Sock.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/syslog.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

class Api{
    public:
        Api(const char* ip, unsigned int port);
        ~Api();
        void start(void);
    private:
    
};

#endif
