#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <string> 
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "Protocol.h"
#include "json/json.h"
#include "event/event.h"  
#include "event/bufferevent.h" 
#include "event/buffer.h"
#include "event/thread.h"

#define DEVICE_NAME "SmartCar"
#define WLAN_NAME "enp4s0"

using namespace std;

class Client{
public:
    Client();
    ~Client();
    void start(const char* ip,unsigned int port);
    static void responseHandler(struct bufferevent * bufEvent, void * args);
    static void requestHandler(struct bufferevent * bufEvent, void * args);
    static void eventHandler(struct bufferevent * bufEvent, short sEvent, void * args);
private:
    struct bufferevent* bufferEvent;
    struct event_base* baseEvent;
    void sendDeviceInfo(struct bufferevent * bufEvent);
    string getMacAddress();
};

#endif  