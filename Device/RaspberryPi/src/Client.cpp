#include "Client.h"

Client::Client(){
    this->initApiList();
}

Client::~Client(){
    
}

void Client::setConfig(const char* path){
    Config config;
    //检测配置文件是否存在
    if(!config.FileExist(path)){
      printf("config: not find config file\n");
      exit(0);
    }
    //读取配置
    config.ReadFile(path);     
    string api_host = config.Read("SERVER_HOST", api_host);
    int api_port = config.Read("API_PORT", api_port);
    this->network_card_name = config.Read("NETWORK_CARD", this->network_card_name);
    this->device_name = config.Read("DEVICE_NAME", this->device_name);
    this->setIpAddress(api_host.c_str());
    this->setPort(api_port);
}
void Client::ReadEvent(struct bufferevent * bufEvent){
    Json::Reader reader;
    Json::Value data;
    //获取输入缓存
    struct evbuffer * pInput = bufferevent_get_input(bufEvent);
    //获取输入缓存数据的长度
    int len = evbuffer_get_length(pInput);
    //获取数据
    char* body = new char[len+1];
    memset(body,0,sizeof(char)*(len+1));
    evbuffer_remove(pInput, body, len);
    if(reader.parse(body, data)){
        string func = data["protocol"].asString();
        this->call(bufEvent,data,func);
    }
    delete[] body;
    return ;

}
void Client::WriteEvent(struct bufferevent * bufEvent){

}
void Client::SignalEvent(struct bufferevent * bufEvent, short sEvent){
    //请求的连接过程已经完成
    if(BEV_EVENT_CONNECTED == sEvent){
        string mac = this->getMacAddress();
        if(mac.length() == 0){
            printf("MAC地址获取错误请检查网卡配置\n");
            event_base_loopexit(this->baseEvent, NULL);
            exit(0);
        }
        //发送基本信息
        this->sendDeviceInfo(bufEvent);
    }
}

void Client::initApiList() {
  this->client_api_list[API_DEVICE_BASE_INFO] = &Client::handlerGetDeviceBaseInfo;
  this->client_api_list[API_DEVICE_KEY_DOWN] = &Client::handlerKeyDown;
}

string Client::getMacAddress(){
    string mac;
    struct ifreq        ifr;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(ifr.ifr_name,this->network_card_name.c_str(), this->network_card_name.length());  
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == 0) {
        if(ifr.ifr_hwaddr.sa_data != NULL){
            char* mac_c = new char[strlen(ifr.ifr_hwaddr.sa_data)+1];
            sprintf(mac_c,"%02x:%02x:%02x:%02x:%02x:%02x", ifr.ifr_hwaddr.sa_data[0]&0xff, ifr.ifr_hwaddr.sa_data[1]&0xff, ifr.ifr_hwaddr.sa_data[2]&0xff, ifr.ifr_hwaddr.sa_data[3]&0xff, ifr.ifr_hwaddr.sa_data[4]&0xff, ifr.ifr_hwaddr.sa_data[5]&0xff);
            mac = string(mac_c);
        }
    }
    return mac;
}

void Client::sendDeviceInfo(struct bufferevent * bufEvent){
    Json::Value root;
    Json::Value data;
    //获取MAC地址
    data["mac"] = this->getMacAddress();
    data["name"] = this->device_name;
    root["protocol"] = API_DEVICE_INFO;
    root["is_app"] = false;
    root["data"] = data;
    string json = root.toStyledString();
    bufferevent_write(bufEvent, json.c_str(), json.length());
}

void Client::handlerGetDeviceBaseInfo(struct bufferevent * bufEvent,Json::Value &data){
    Json::Value root;
    Json::Value re_data;
    root["is_app"] = false;
    root["protocol"] = API_DEVICE_BASE_INFO;
    root["data"] = re_data;
    bufferevent_write(bufEvent, root.toStyledString().c_str(), root.toStyledString().length());
}

void Client::handlerKeyDown(struct bufferevent * bufEvent,Json::Value &data){
    printf("key down:%s\n", data["data"].toStyledString().c_str());
}

void Client::call(struct bufferevent * bufEvent,Json::Value &request_data,const string func){
    if(func.length() == 0){
        return;
    }
    if (client_api_list.count(func)) {
        (this->*(client_api_list[func]))(bufEvent,request_data);
    }
}