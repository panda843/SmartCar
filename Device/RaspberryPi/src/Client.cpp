#include "Client.h"

Client::Client(){
    this->initApiList();
}

Client::~Client(){
    delete[] this->ip;
}
void Client::initApiList() {

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
void Client::startRun(){
    //创建事件驱动句柄
    printf("pl");
    //this->baseEvent = event_base_new();
    if(this->baseEvent == NULL){
	printf("event_base_new error\n");
	exit(0);
    }
    //创建socket类型的bufferevent
    this->bufferEvent = bufferevent_socket_new(this->baseEvent, -1, 0);
    //构造服务器地址
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET; 
    sin.sin_addr.s_addr = inet_addr(this->ip);
    sin.sin_port = htons(this->port); 
    //连接服务器
    if( bufferevent_socket_connect(this->bufferEvent, (struct sockaddr*)&sin, sizeof(sin)) < 0){
        perror("socket");
        return;
    }
    //设置回调函数, 及回调函数的参数
    bufferevent_setcb(this->bufferEvent, ReadEventCb, WriteEventCb, SignalEventCb, (void*)this);
    //开始事件循环
    event_base_dispatch(this->baseEvent);
    //事件循环结束 资源清理
    bufferevent_free(this->bufferEvent);
    event_base_free(this->baseEvent);
}
void Client::setIpAddress(const char * ip){
    char* str = new char[sizeof(ip)];
    strcpy(str,ip);
    this->ip = str;
}
void Client::setPort(const int port){
    this->port = port;
}
//读操作
void Client::ReadEventCb(struct bufferevent *bufEvent, void *args){
    Client* device = (Client*)args;
    device->ReadEvent(bufEvent);
}
//写操作
void Client::WriteEventCb(struct bufferevent *bufEvent, void *args){
    Client* device = (Client*)args;
    device->WriteEvent(bufEvent);
}
//关闭
void Client::SignalEventCb(struct bufferevent * bufEvent, short sEvent, void * args){
    Client* device = (Client*)args;
    //请求的连接过程已经完成
    if(BEV_EVENT_CONNECTED == sEvent){
        bufferevent_enable(bufEvent, EV_READ);
        //设置读超时时间 10s
        struct timeval tTimeout = {10, 0};
        bufferevent_set_timeouts( bufEvent, &tTimeout, NULL);
    }
    //写操作发生事件
    if(BEV_EVENT_WRITING & sEvent){}
    //操作时发生错误
    if (sEvent & BEV_EVENT_ERROR){
        perror("event");
        event_base_loopexit(device->baseEvent, NULL);
    }
    //结束指示
    if (sEvent & BEV_EVENT_EOF){
        perror("event");
        event_base_loopexit(device->baseEvent, NULL);  
    }
    //读取发生事件或者超时处理
    if(0 != (sEvent & (BEV_EVENT_TIMEOUT|BEV_EVENT_READING)) ){
        //发送心跳包
        //
        //重新注册可读事件
        bufferevent_enable(bufEvent, EV_READ);
    }
    device->SignalEvent(bufEvent,sEvent);
    return ;
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

void Client::call(struct bufferevent * bufEvent,Json::Value &request_data,const string func){
    if(func.length() == 0){
        return;
    }
    if (client_api_list.count(func)) {
        (this->*(client_api_list[func]))(bufEvent,request_data);
    }
}
