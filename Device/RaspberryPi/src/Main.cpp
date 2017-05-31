#include "Main.h"
  
int main(){  
    Client* client = new Client();
    client->setConfig(CONFIG_PATH);
    client->startRun();
    return 0;  
}  

