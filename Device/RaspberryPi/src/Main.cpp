#include "Main.h"

//设置码率
void set_speed(int fd, int speed){
    int speed_arr[] = {B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200, B9600, B4800, B2400, B1200, B300};  
    int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  19200,  9600, 4800, 2400, 1200,  300};   
    struct termios   Opt; 
    int i; 
    tcgetattr(fd, &Opt);  
    for(i= 0;  i < 14;  i++){  
        if(speed == name_arr[i]){  
            tcflush(fd, TCIOFLUSH);  
            cfsetispeed(&Opt, speed_arr[i]);  
            cfsetospeed(&Opt, speed_arr[i]);  
            if(tcsetattr(fd, TCSANOW, &Opt) != 0){
                perror("tcsetattr fd1");
            }
        }
        tcflush(fd,TCIOFLUSH);    
    }  
}

bool set_parity(int fd,int databits,int stopbits,int parity){  
    struct termios options;  
    if( tcgetattr( fd,&options)  !=  0){  
        perror("SetupSerial 1");  
        return false;  
    }  
    options.c_cflag &= ~CSIZE;
    /*设置数据位数*/  
    switch (databits){  
        case 7:  
            options.c_cflag |= CS7;  
        break;  
        case 8:  
            options.c_cflag |= CS8;  
        break;  
        default:  
            fprintf(stderr,"Unsupported data size\n");  
            return false;  
    }  
    switch (parity){  
        case 'n':  
        case 'N':  
            options.c_cflag &= ~PARENB;   /* Clear parity enable */  
            options.c_iflag &= ~INPCK;     /* Enable parity checking */  
        break;  
        case 'o':  
        case 'O':  
            options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/   
            options.c_iflag |= INPCK;             /* Disnable parity checking */  
        break;  
        case 'e':  
        case 'E':  
            options.c_cflag |= PARENB;     /* Enable parity */  
            options.c_cflag &= ~PARODD;   /* 转换为偶效验*/    
            options.c_iflag |= INPCK;       /* Disnable parity checking */  
        break;  
        case 'S':  
        case 's':  /*as no parity*/  
            options.c_cflag &= ~PARENB;  
            options.c_cflag &= ~CSTOPB;  
        break;  
        default:  
            fprintf(stderr,"Unsupported parity\n");  
            return false;  
    }  
    /* 设置停止位*/     
    switch (stopbits){  
        case 1:  
            options.c_cflag &= ~CSTOPB;  
        break;  
        case 2:  
            options.c_cflag |= CSTOPB;  
        break;  
        default:  
            fprintf(stderr,"Unsupported stop bits\n");  
            return false;  
    }  
    /* Set input parity option */  
    if(parity != 'n'){  
        options.c_iflag |= INPCK;  
        options.c_cc[VTIME] = 150; // 15 seconds  
        options.c_cc[VMIN] = 0;
    }  
    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */  
    if(tcsetattr(fd,TCSANOW,&options) != 0){  
        perror("SetupSerial 3");  
        return false;  
    }  
    return true;  
 }

int initArduino(){
    int fd = open("/dev/ttyS0", O_RDWR );
    if(fd < 0){
       perror("intArduino"); 
       exit(0);
    }
    //设置通信
    set_speed(fd,9600);
    //设置数据格式
    set_parity(fd,8,1,'N');
    //返回文件描述符
    return fd;
}

//发送串口数据
void sendArduinoData(const char* data){
    if(arduino_fd > 0){
        printf("send key:%s\n",data);
        write(arduino_fd, data, strlen(data)+1);
    }
}
//读取串口数据
void readArduinoData(char* read_buf){
    int readnum = 0;
    char read_nBytes[10] = {0};
    bzero(read_nBytes,sizeof(read_nBytes));
    if(arduino_fd > 0){
        while((readnum = read(arduino_fd,read_nBytes,8))>0){
            if(readnum == 8){
                read_nBytes[readnum] = '\0';
                strcat(read_buf,read_nBytes); 
                for(int i=0;i<10;i++){
                  if(read_nBytes[i] == '\n'){
                    tcflush(TCIFLUSH);
                    return;
                  }  
                }   
            }  
            if(readnum > 1 && readnum < 8){  
                read_nBytes[readnum] = '\0';
                strcat(read_buf,read_nBytes);
                tcflush(TCIFLUSH);
                return;  
            } 
        }
    }
}
//获取摄像头状态
int checkCameraStatus(){
    //检查进程是否存在
    string checkCmd = "ps -ef|grep raspivid | grep -v grep |awk 'NR<2{print $2}'";
    //打开popen并执行检查命令
    char buff[32];
    memset(buff ,'\0', sizeof(buff));
    // 通过管道来回去系统命令返回的值
    FILE *fstream = popen(checkCmd.c_str(), "r");
    if(fstream != NULL) {
        if(NULL == fgets(buff, sizeof(buff), fstream)){
           return 0;
        }
        if(0 == strlen(buff)){
            return 0;
        }else{
            return atoi(buff);
        }
        pclose(fstream); 
    }
    return 0;
}

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
    memset(buffer,0,100);
    sprintf(buffer,"%.2f",totalMemSize);
    re_data["mem_total"] = string(buffer);
    //使用内存
    memset(buffer,0,100);
    sprintf(buffer,"%.2f",usedMemSize);
    re_data["mem_used"] = string(buffer);
    //总硬盘大小
    memset(buffer,0,100);
    sprintf(buffer,"%.2f",totalDiskSize);
    re_data["disk_total"] = string(buffer);
    //使用硬盘大小
    memset(buffer,0,100);
    sprintf(buffer,"%.2f",usedDiskSize);
    re_data["disk_used"] = string(buffer);
    //视频状态
    if(checkCameraStatus() == 0){
        re_data["video_enable"] = false;
    }else{
        re_data["video_enable"] = true;
        re_data["video_hls"] = string(VIDEO_PLAY_HLS)+string(VIDEO_NAME)+"/index.m3u8";
        re_data["video_rtmp"] = string(VIDEO_SERVER_PATH)+string(VIDEO_NAME);
    }
    //返回数据
    root["data"] = re_data;
    bufferevent_write(bufEvent, root.toStyledString().c_str(), root.toStyledString().length());
}

void setCameraPower(struct bufferevent * bufEvent,Json::Value &data){
    //构造返回json串
    Json::Value root;
    Json::Value re_data;
    //判断相机状态
    int pid = checkCameraStatus();
    if( pid == 0){
        //开启相机
        string startCmd = "nohup raspivid -fl -t 0 -w "+string(VIDEO_WIDTH)+" -h "+string(VIDEO_HEIGHT)+" -b 1200000 -fps "+string(VIDEO_FPS)+" -pf baseline -o - | ffmpeg -f h264 -i - -c copy -an -f flv -y "+string(VIDEO_SERVER_PATH)+string(VIDEO_NAME)+" > /dev/null &";
        FILE *fstream = popen(startCmd.c_str(), "r");
        if(fstream != NULL){
            pclose(fstream);
        }
        //判断命令是否执行成功
        re_data["video_enable"] = true;
        if(checkCameraStatus() == 0){
            re_data["status"] = false;
            re_data["message"] = "开启相机失败";
        }else{
            re_data["status"] = true;
            re_data["message"] = "开启相机成功";
            re_data["video_hls"] = string(VIDEO_PLAY_HLS)+string(VIDEO_NAME)+"/index.m3u8";
            re_data["video_rtmp"] = string(VIDEO_SERVER_PATH)+string(VIDEO_NAME);
        }
    }else{
        //关闭相机
        char stopCmd[100];
        sprintf(stopCmd,"kill -9 %d",pid);
        FILE *fstream = popen(stopCmd, "r");
        if(fstream != NULL){
            pclose(fstream);
        }
        //判断命令是否执行成功
        re_data["video_enable"] = false;
        if(checkCameraStatus() == 0){
            re_data["status"] = true;
            re_data["message"] = "关闭相机成功";
        }else{
            re_data["status"] = false;
            re_data["message"] = "关闭相机失败";
        }
    }
    //构造返回JSON
    root["is_app"] = false;
    root["data"] = re_data;
    root["protocol"] = API_SET_CAMERA_POWER;
    bufferevent_write(bufEvent, root.toStyledString().c_str(), root.toStyledString().length());
}
//键盘按下
void handlerKeyDown(struct bufferevent * bufEvent,Json::Value &data){
    Json::Value key_map = data["data"];
    //sendArduinoData(key_map["key"].asString().c_str());
    sendArduinoData("1234567\n");
    char buff[512];
    memset(buff,0,sizeof(char)*512);
    readArduinoData(buff);
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
    mac = mac.substr(0, mac.length()-1);
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
        string mac = getMacAddress();
        if(mac.length() == 0){
            printf("MAC地址获取错误请检查网卡配置\n");
            event_base_loopexit(baseEvent, NULL);
            exit(0);
        }
        sendDeviceInfo(bufEvent);
        //重新注册可读事件
        bufferevent_enable(bufEvent, EV_READ);
        //设置读超时时间 10s
        struct timeval tTimeout = {10, 0};
        bufferevent_set_timeouts( bufEvent, &tTimeout, NULL);
    }
    return ;
}
//设置配置文件
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
//开始
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
  client_api_list[API_SET_CAMERA_POWER] = &setCameraPower;
}
  
int main(){
    //加载API列表
    initApiList();
    //加载配置文件
    setConfig(CONFIG_PATH);
    //加载通信串口
    arduino_fd = initArduino();
    //启动sockt
    startRun(api_host.c_str(),api_port);
    return 0;
}  
