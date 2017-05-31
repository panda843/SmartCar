#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <map>
#include "Protocol.h"
#include "json/json.h"
#include "Device.h"
#include "Config.h"

class Client;

#ifndef _CLIENT_STRUCT_
#define _CLIENT_STRUCT_
typedef void (Client::*cfunc)(struct bufferevent *,Json::Value&);
#endif

using namespace std;

class Client : public Device{
public:
    Client();
    ~Client();
    void setConfig(const char* path);
protected:
    void ReadEvent(struct bufferevent * bufEvent);
    void WriteEvent(struct bufferevent * bufEvent);
    void SignalEvent(struct bufferevent * bufEvent, short sEvent);
private:
    map<string,cfunc> client_api_list;
    string network_card_name;
    string device_name;
    void initApiList();
    void sendDeviceInfo(struct bufferevent * bufEvent);
    string getMacAddress();
    void call(struct bufferevent * bufEvent,Json::Value &request_data,const string func);
    void handlerGetDeviceBaseInfo(struct bufferevent * bufEvent,Json::Value &data);
    void handlerKeyDown(struct bufferevent * bufEvent,Json::Value &data);
};

#endif  