#include "Device.h"

Device::Device(const char* ip, unsigned int port){

}

Device::~Device(){

}

void Device::start(){
    char buf[] = {"this loop dev write log\n"};
    while (true){
        sleep(8);
        FILE *fp = fopen("log.txt","a+");
        fwrite(buf, 1, strlen(buf), fp);
        fclose(fp);
    }  
}
