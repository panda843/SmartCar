#include "Main.h"
  
int main(){  
    Client* client = new Client();
    client->start("127.0.0.1",5124);
    return 0;  
}  

