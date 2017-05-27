#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "MysqlHelper.h"
#include "Protocol.h"
#include "TcpEvent.h"
#include "json/json.h"
#include <sys/fcntl.h>
#include <set>
#include <map>
#include <string>
#include <string.h>

using namespace std;
using namespace mysqlhelper;

#define SOCK_PIPE_MAXDATA 2048

class Device : public TcpEventServer{
    public:
        Device();
        ~Device();
        //设置通信管道
        void setPipe(int *read_fd,int *write_fd);
        //启动
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
        //向Api进程写输入
        void writePipe(const char *str);
        //读Api进程输入
        char* readPipe();
    private:
        map<int,Conn*> sock_list;
        MysqlHelper* mysql;
        //通信管道
        int* sock_write_pipe;
        int* sock_read_pipe;
        void handlerDeverInfo(Conn* &conn, Json::Value &request_data);
        void initApiList();
        void sendData(Conn* &conn,const string resp_data);
        void call(Conn* &conn, Json::Value &request_data,const string func);
};

/**
 * 函数地址定义
 */
typedef void (Device::*pfunc)(Conn*&, Json::Value&);

#endif
