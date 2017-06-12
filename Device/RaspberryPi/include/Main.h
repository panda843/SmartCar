#ifndef _MAIN_H_
#define _MAIN_H_

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
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
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
#define DISK_SIZE_PATH "/"
//流服务器地址
#define VIDEO_SERVER_PATH "rtmp://123.207.18.40:1935/live/"
//视频推流名称
#define VIDEO_NAME "SmartCar"
//视频宽度
#define VIDEO_WIDTH "760"
//视频高度
#define VIDEO_HEIGHT "299"
//视频整数
#define VIDEO_FPS "15"
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

#endif  