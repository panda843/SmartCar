#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include <string>  
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include "event/event.h"  
#include "event/bufferevent.h" 
#include "event/buffer.h"
#include "event/thread.h"
using namespace std;

class Device{
public:
    Device();
    virtual ~Device();
    void startRun();
protected:
    struct event_base* baseEvent;
    struct bufferevent* bufferEvent;
    void setIpAddress(const char * ip);
    void setPort(const int port);
private:
    const char *ip;
    int port;
    virtual void ReadEvent(struct bufferevent * bufEvent) {};
    virtual void WriteEvent(struct bufferevent * bufEvent) {};
    virtual void SignalEvent(struct bufferevent * bufEvent, short sEvent) {};
    //读操作
    static void ReadEventCb(struct bufferevent *bufEvent, void *args);
    //写操作
    static void WriteEventCb(struct bufferevent *bufEvent, void *args); 
    //关闭
    static void SignalEventCb(struct bufferevent * bufEvent, short sEvent, void * args);
};
#endif