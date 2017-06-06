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
#include "Config.h"
#include "json/json.h"
#include "Protocol.h"
using namespace std;

class Client;

#ifndef _CLIENT_STRUCT_
#define _CLIENT_STRUCT_
typedef void (Client::*cfunc)(struct bufferevent *,Json::Value&);
#endif

class Client{
public:
    Client();
    ~Client();
    void startRun();
    void setConfig(const char* path);
protected:
    struct event_base* baseEvent;
    struct bufferevent* bufferEvent;
    void setIpAddress(const char * ip);
    void setPort(const int port);
    void ReadEvent(struct bufferevent * bufEvent);
    void WriteEvent(struct bufferevent * bufEvent);
    void SignalEvent(struct bufferevent * bufEvent, short sEvent);
private:
    const char *ip;
    int port;
    string network_card_name;
    string device_name;
    map<string,cfunc> client_api_list;
    string getMacAddress();
    void initApiList();
    void call(struct bufferevent * bufEvent,Json::Value &request_data,const string func);
    //读操作
    static void ReadEventCb(struct bufferevent *bufEvent, void *args);
    //写操作
    static void WriteEventCb(struct bufferevent *bufEvent, void *args); 
    //关闭
    static void SignalEventCb(struct bufferevent * bufEvent, short sEvent, void * args);
    void sendDeviceInfo(struct bufferevent * bufEvent);
};
#endif