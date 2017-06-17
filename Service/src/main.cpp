#include "Api.h"
#include "Device.h"

#include <signal.h> 
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <string>

using namespace std;

#define CONFIG_PATH "/etc/smartcar.conf"

void initDaemon(){
    //设置新会话组长，脱离终端
    setsid();
    //关闭所有从父进程继承的不再需要的文件描述符
    for (int i =0 ; i<NOFILE ;close(i++ ));
    //改变工作目录到/tmp 
    chdir("/tmp");
    //重设文件创建掩模 
    umask(0);
    //忽略信号，终端中断 
    signal(SIGINT,  SIG_IGN); 
    // 连接挂断 
    signal(SIGHUP,  SIG_IGN);
    // 终端退出  
    signal(SIGQUIT, SIG_IGN);  
    // 向无读进程的管道写数据
    signal(SIGPIPE, SIG_IGN);  
    // 后台程序尝试写操作 
    signal(SIGTTOU, SIG_IGN); 
    // 后台程序尝试读操作
    signal(SIGTTIN, SIG_IGN);
    // 终止   
    signal(SIGTERM, SIG_IGN); 
}

int main(){
    pid_t pid_api,pid_dev;
    int fd_writ[2],fd_read[2];
    //创建写管道
    if( pipe(fd_writ) < 0 || pipe(fd_read) < 0){  
        printf("create pipe error\n");  
        exit(EXIT_FAILURE);
    }  
    //创建API进程
    if((pid_api = fork()) < 0){
        //创建进程失败
        printf("fork api error\n");
        exit(EXIT_FAILURE);
    }else if(pid_api == 0){
        //子进程
        initDaemon();
        Api* api = new Api();
        api->setPipe(fd_writ,fd_read);
        api->setConfig(CONFIG_PATH);
        api->startRun();
        delete api;
        return 0;
    }
    //创建DEV进程
    if((pid_dev = fork()) < 0){
        //创建进程失败
        printf("fork dev error\n");
        exit(EXIT_FAILURE);
    }else if(pid_dev == 0){
        //子进程
        initDaemon();
        Device* device = new Device();
        device->setPipe(fd_writ,fd_read);
        device->setConfig(CONFIG_PATH);
        device->StartRun();
        delete device;
        return 0;
    }
    //父进程
    while(1){

    }
    exit(EXIT_SUCCESS);
    return 0;
}
