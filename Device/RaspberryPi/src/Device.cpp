#include "Device.h"

Device::Device(){

}

Device::~Device(){
    delete[] this->ip;
}
void Device::startRun(){
    //创建事件驱动句柄
    this->baseEvent = event_base_new();
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
void Device::setIpAddress(const char * ip){
    char* str = new char[sizeof(ip)];
    strcpy(str,ip);
    this->ip = str;
}
void Device::setPort(const int port){
    this->port = port;
}
//读操作
void Device::ReadEventCb(struct bufferevent *bufEvent, void *args){
    Device* device = (Device*)args;
    device->ReadEvent(bufEvent);
}
//写操作
void Device::WriteEventCb(struct bufferevent *bufEvent, void *args){
    Device* device = (Device*)args;
    device->WriteEvent(bufEvent);
}
//关闭
void Device::SignalEventCb(struct bufferevent * bufEvent, short sEvent, void * args){
    Device* device = (Device*)args;
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