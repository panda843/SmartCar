#include <stdio.h>
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <sys/vfs.h>
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
#define CONFIG_PATH "/etc/smart_car_device.conf"
typedef void (*cfunc)(struct bufferevent *,Json::Value&);
string network_card_name;
string device_name;
string api_host;
int api_port;
struct event_base* baseEvent;
map<string,cfunc> client_api_list;
//获取基本信息
void handlerGetDeviceBaseInfo(struct bufferevent * bufEvent,Json::Value &data){
    //获取内存大小
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    double totalMemSize = (double)memInfo.totalram/(1024.0*1024.0);
    double usedMemSize = (double)(memInfo.totalram-memInfo.freeram)/(1024.0*1024.0);
    //获取磁盘大小
    struct statfs diskInfo;
    statfs("/", &diskInfo);
    double totalDiskSize = (double)(diskInfo.f_bsize*diskInfo.f_blocks)/(1024.0*1024.0);
    double usedDiskSize = (double)(diskInfo.f_bsize*diskInfo.f_blocks-diskInfo.f_bsize*diskInfo.f_bfree)/(1024.0*1024.0);
    printf("mem total:%.2fMB,mem used:%.2fMB,disk total:%.2fMB,disk used:%.2fMB\n",totalMemSize,usedMemSize,totalDiskSize,usedDiskSize );
    Json::Value root;
    Json::Value re_data;
    root["is_app"] = false;
    root["protocol"] = API_DEVICE_BASE_INFO;
    root["data"] = re_data;
    bufferevent_write(bufEvent, root.toStyledString().c_str(), root.toStyledString().length());
}
//键盘按下
void handlerKeyDown(struct bufferevent * bufEvent,Json::Value &data){
    printf("key down:%s\n", data["data"].toStyledString().c_str());
}
//获取MAC地址
string getMacAddress(){
    char returnData[64];
    FILE *fp = popen("/sbin/ifconfig wlan0", "r");
    if(fgets(returnData, 64, fp) != NULL){
        char* mac_addr = strtok(returnData, "HWaddr ");
        if( mac_addr != NULL){
            printf("mac:%s\n",mac_addr );
        }
    }

    pclose(fp);


    string mac;
    struct ifreq        ifr;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("socket");
    }
    strncpy(ifr.ifr_name,network_card_name.c_str(), network_card_name.length());
    printf("ifr_name:%s\n", network_card_name.c_str());
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == 0) {
        if(ifr.ifr_hwaddr.sa_data != NULL){
            char* mac_c = new char[strlen(ifr.ifr_hwaddr.sa_data)+1];
            sprintf(mac_c,"%02x:%02x:%02x:%02x:%02x:%02x", ifr.ifr_hwaddr.sa_data[0]&0xff, ifr.ifr_hwaddr.sa_data[1]&0xff, ifr.ifr_hwaddr.sa_data[2]&0xff, ifr.ifr_hwaddr.sa_data[3]&0xff, ifr.ifr_hwaddr.sa_data[4]&0xff, ifr.ifr_hwaddr.sa_data[5]&0xff);
            mac = string(mac_c);
            delete[] mac_c;
            printf("mac:%s\n", mac.c_str());
        }
    }
    perror("ioctl");
    return mac;
}
//调用方法
void callFunc(struct bufferevent * bufEvent,Json::Value &request_data,const string func){
    if(func.length() == 0){
        return;
    }
    if (client_api_list.count(func)) {
        (*(client_api_list[func]))(bufEvent,request_data);
    }
}

void sendDeviceInfo(struct bufferevent * bufEvent){
    Json::Value root;
    Json::Value data;
    //获取MAC地址
    data["mac"] = getMacAddress();
    data["name"] = device_name;
    root["protocol"] = API_DEVICE_INFO;
    root["is_app"] = false;
    root["data"] = data;
    string json = root.toStyledString();
    bufferevent_write(bufEvent, json.c_str(), json.length());
}

//读操作
void ReadEventCb(struct bufferevent *bufEvent, void *args){
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
        callFunc(bufEvent,data,func);
    }
    delete[] body;
    return ;
}
//写操作
void WriteEventCb(struct bufferevent *bufEvent, void *args){

}
//关闭
void SignalEventCb(struct bufferevent * bufEvent, short sEvent, void * args){
    //请求的连接过程已经完成
    if(BEV_EVENT_CONNECTED == sEvent){
        bufferevent_enable(bufEvent, EV_READ);
        //设置读超时时间 10s
        struct timeval tTimeout = {10, 0};
        bufferevent_set_timeouts( bufEvent, &tTimeout, NULL);
        string mac = getMacAddress();
        if(mac.length() == 0){
            printf("MAC地址获取错误请检查网卡配置\n");
            event_base_loopexit(baseEvent, NULL);
            exit(0);
        }
        //发送基本信息
        sendDeviceInfo(bufEvent);
    }
    //写操作发生事件
    if(BEV_EVENT_WRITING & sEvent){}
    //操作时发生错误
    if (sEvent & BEV_EVENT_ERROR){
        perror("event");
        event_base_loopexit(baseEvent, NULL);
    }
    //结束指示
    if (sEvent & BEV_EVENT_EOF){
        perror("event");
        event_base_loopexit(baseEvent, NULL);
    }
    //读取发生事件或者超时处理
    if(0 != (sEvent & (BEV_EVENT_TIMEOUT|BEV_EVENT_READING)) ){
        //发送心跳包
        //
        //重新注册可读事件
        bufferevent_enable(bufEvent, EV_READ);
    }
    return ;
}

void setConfig(const char* config_path){
    Config config;
    //检测配置文件是否存在
    if(!config.FileExist(config_path)){
      printf("config: not find config file\n");
      exit(0);
    }
    //读取配置
    config.ReadFile(config_path);     
    api_host = config.Read("SERVER_HOST", api_host);
    api_port = config.Read("API_PORT", api_port);
    network_card_name = config.Read("NETWORK_CARD", network_card_name);
    device_name = config.Read("DEVICE_NAME", device_name);
}

void startRun(const char* ip,int port){
    //创建事件驱动句柄
    baseEvent = event_base_new();
    //创建socket类型的bufferevent
    struct bufferevent* bufferEvent = bufferevent_socket_new(baseEvent, -1, 0);
    //构造服务器地址
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET; 
    sin.sin_addr.s_addr = inet_addr(api_host.c_str());
    sin.sin_port = htons(api_port); 
    //连接服务器
    if( bufferevent_socket_connect(bufferEvent, (struct sockaddr*)&sin, sizeof(sin)) < 0){
        perror("socket");
        return;
    }
    //设置回调函数, 及回调函数的参数
    bufferevent_setcb(bufferEvent, ReadEventCb, WriteEventCb, SignalEventCb,NULL);
    //开始事件循环
    event_base_dispatch(baseEvent);
    //事件循环结束 资源清理
    bufferevent_free(bufferEvent);
    event_base_free(baseEvent);
}
//初始化API列表
void initApiList() {
  client_api_list[API_DEVICE_BASE_INFO] = &handlerGetDeviceBaseInfo;
  client_api_list[API_DEVICE_KEY_DOWN] = &handlerKeyDown;
}
  
int main(){
    //加载API列表
    initApiList();
    //加载配置文件
    setConfig(CONFIG_PATH);
    //启动sockt
    startRun(api_host.c_str(),api_port);
    return 0;
}  
