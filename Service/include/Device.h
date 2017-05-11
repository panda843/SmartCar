#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "TcpEvent.h"
#include <set>
#include <vector>

// #include <string.h>
// #include <string>
// #include <arpa/inet.h>
// #include <stdlib.h>

// #include "event/event.h"
// #include "event/listener.h"

using namespace std;

class Device : public TcpEventServer{
    public:
        Device();
        ~Device();
        void start(const char* ip,unsigned int port);
        //退出事件，响应Ctrl+C
        static void QuitCb(int sig, short events, void *data);
        //定时器事件，每10秒向所有客户端发一句hello, world
        static void TimeOutCb(int id, int short events, void *data);
    protected:
        //重载各个处理业务的虚函数
        void ReadEvent(Conn *conn);
        void WriteEvent(Conn *conn);
        void ConnectionEvent(Conn *conn);
        void CloseEvent(Conn *conn, short events);
    private:
        vector<Conn*> vec;
      //event base
      // struct event_base* eventBase = NULL;

};

#endif
