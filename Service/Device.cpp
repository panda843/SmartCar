#include "Device.h"

Device::Device(const char* ip, unsigned int port){

}

Device::~Device(){

}

void Device::start(void){
	time_t ticks;
	/*忽略可能的终端信号*/
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGHUP,SIG_IGN);
    //创建子进程，父进程推出
    if( (pid=fork())){
        exit(EXIT_SUCCESS);//是父进程，结束父进程，子进程继续
    }else if(pid < 0){
        perror("fork error!\n");
        exit(EXIT_FAILURE);
    } 
    //设置新会话组长，脱离终端
    setsid();
    for(i=0;i<NOFILE;i++){
    	close(i);
    }
    open("/dev/null",O_RDONLY);
    open("/dev/null",O_RDWR);
    open("/dev/null",O_RDWR);
     
    chdir("/");
    umask(0);
    signal(SIGCHLD,SIG_IGN);
   	openlog("dev",LOG_PID,LOG_KERN);
   	syslog(LOG_INFO,"%s",asctime(localtime(&ticks)));//写日志信息
    return void();
}