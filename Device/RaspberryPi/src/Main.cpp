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
//配置文件
#define CONFIG_PATH "/etc/smart_car_device.conf"
//硬盘大小获取地址
#define DISK_SIZE_PATH "/dev/root"
//定义指针方法
typedef void (*cfunc)(struct bufferevent *,Json::Value&);
//网卡名称
string network_card_name;
//设备名称
string device_name;
//API地址
string api_host;
//API端口
int api_port;
//BaseEvent
struct event_base* baseEvent;
//API列表
map<string,cfunc> client_api_list;
//获取基本信息
void handlerGetDeviceBaseInfo(struct bufferevent * bufEvent,Json::Value &data){
    char buffer[100];
    //获取内存大小
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    //获取磁盘大小
    struct statfs diskInfo;
    statfs(DISK_SIZE_PATH, &diskInfo);
    //格式化数据
    double totalMemSize = (double)memInfo.totalram/(1024.0*1024.0);
    double usedMemSize = (double)(memInfo.totalram-memInfo.freeram)/(1024.0*1024.0);
    double totalDiskSize = (double)(diskInfo.f_bsize*diskInfo.f_blocks)/(1024.0*1024.0);
    double usedDiskSize = (double)(diskInfo.f_bsize*diskInfo.f_blocks-diskInfo.f_bsize*diskInfo.f_bfree)/(1024.0*1024.0);
    //构造返回JSON
    Json::Value root;
    Json::Value re_data;
    root["is_app"] = false;
    root["protocol"] = API_DEVICE_BASE_INFO;
    //总内存大小
    sprintf(buffer,"%.2f",totalMemSize);
    re_data["mem_total"] = string(buffer);
    memset(buffer,0,100);
    //使用内存
    sprintf(buffer,"%.2f",usedMemSize);
    re_data["mem_used"] = string(buffer);
    memset(buffer,0,100);
    //总硬盘大小
    sprintf(buffer,"%.2f",totalDiskSize);
    re_data["disk_total"] = string(buffer);
    memset(buffer,0,100);
    //使用硬盘大小
    sprintf(buffer,"%.2f",usedDiskSize);
    re_data["disk_used"] = string(buffer);
    memset(buffer,0,100);
    //返回数据
    root["data"] = re_data;
    bufferevent_write(bufEvent, root.toStyledString().c_str(), root.toStyledString().length());
}
//键盘按下
void handlerKeyDown(struct bufferevent * bufEvent,Json::Value &data){
    printf("key down:%s\n", data["data"].toStyledString().c_str());
}
//获取MAC地址
string getMacAddress(){
    char buff[32];
    memset (buff ,'\0', sizeof(buff));
    string cmd = "ip addr |grep -A 2 "+network_card_name+" | awk 'NR>1'|awk 'NR<2'|awk '{print $2}'";
    // 通过管道来回去系统命令返回的值
    FILE *fstream = popen(cmd.c_str(), "r");
    if(fstream == NULL) {
        perror("popen");
        exit(0);
    }
    if(NULL == fgets(buff, sizeof(buff), fstream)){
        printf("not find mac address !!!\n");
        exit(0); 
    }    
    pclose(fstream);
    string mac;
    mac = string(buff);
    mac = mac.substr(0, s.length()-1);
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
