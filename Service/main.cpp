#include "Api.h"
#include "Device.h"
#include <pthread.h>

void* apiService(void* context){
    Api* api = new Api("127.0.0.1",8080);
    api->start();
    delete api;
    pthread_detach(pthread_self());
}

void* devService(void* context){
    Device* dev = new Device("127.0.0.1",8081);
    dev->start();
    delete dev;
    pthread_detach(pthread_self());
}

int main(){
    pthread_t t_api_id,t_dev_id;

    int ret_api = pthread_create(&t_api_id, NULL,apiService,NULL);
    int ret_dev = pthread_create(&t_dev_id, NULL,devService,NULL);

    return 0;
}