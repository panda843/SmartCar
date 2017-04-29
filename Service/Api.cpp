#include "Api.h"

Api::Api(const char* ip, unsigned int port){

}

Api::~Api(){

}

void Api::start(){
    char buf[] = {"this loop api write log\n"};
    while (true){
        sleep(8);
        FILE *fp = fopen("log.txt","a+");
        fwrite(buf, 1, strlen(buf), fp);
        fclose(fp);
    }   
}
