#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/syslog.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

class Device{
    public:
        Device(const char* ip, unsigned int port);
        ~Device();
        void start();
    private:

};

#endif
