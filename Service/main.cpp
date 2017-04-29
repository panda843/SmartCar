#include "Api.h"
#include "Device.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/syslog.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

int main(){
    Api api = new Api("127.0.0.1",5123);
    api->start();
    Device dev = new Device("127.0.0.1",5124);
    dev->start();

    delete api;
    delete dev;
    return 0;
}
