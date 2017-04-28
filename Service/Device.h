#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "Sock.h"

class Device{
    public:
        Device(const char* ip, unsigned int port);
        ~Device();
        void start(void);
    private:

};

#endif