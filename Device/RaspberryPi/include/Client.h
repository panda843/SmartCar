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
#include <map>
#include "Protocol.h"
#include "json/json.h"
#include "event/event.h"  
#include "event/bufferevent.h" 
#include "event/buffer.h"
#include "event/thread.h"

#define DEVICE_NAME "SmartCar"
#define WLAN_NAME "wlan0"

class Client;

#ifndef _CLIENT_STRUCT_
#define _CLIENT_STRUCT_
typedef void (Client::*cfunc)(struct bufferevent *,Json::Value*);
#endif

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
    map<string,cfunc> client_api_list;
    struct bufferevent* bufferEvent;
    struct event_base* baseEvent;
    void initApiList();
    void sendDeviceInfo(struct bufferevent * bufEvent);
    string getMacAddress();
    void call(struct bufferevent * bufEvent,Json::Value &request_data,const string func);
    void handlerGetDeviceBaseInfo(struct bufferevent * bufEvent,Json::Value *data);
};

#endif  