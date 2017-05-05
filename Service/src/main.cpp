#include "Api.h"
#include "Device.h"
#include <signal.h> 

Api* api;

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

void apiSignalHandler(int sig){
    delete api;
}

int main(){
    // pid_t pid_api,pid_dev;
    // //创建API进程
    // if((pid_api = fork()) < 0){
    //     //创建进程失败
    //     printf("fork api error\n");
    //     exit(EXIT_FAILURE);
    // }else if(pid_api == 0){
        //子进程
        // initDaemon();
        signal(SIGINT, apiSignalHandler);
        api = new Api("127.0.0.1",5123);
        api->start();
        // return 0;
    // }
    // //创建DEV进程
    // if((pid_dev = fork()) < 0){
    //     //创建进程失败
    //     printf("fork dev error\n");
    //     exit(EXIT_FAILURE);
    // }else if(pid_dev == 0){
    //     //子进程
    //     initDaemon();
    //     Device* dev = new Device("127.0.0.1",5124);
    //     dev->start();
    //     delete dev;
    //     return 0;
    // }
    // //父进程
    // exit(EXIT_SUCCESS);
    return 0;
}
