#include "Device.h"

Device::Device(){

}

Device::~Device(){

}

void Device::start(const char* ip,unsigned int port){
    this->AddSignalEvent(SIGINT, Device::QuitCb);
    this->SetPort(port);
    this->SetAddress(ip);
    this->StartRun();
}

void Device::ReadEvent(Conn *conn)
{
    conn->MoveBufferData();
}

void Device::WriteEvent(Conn *conn)
{

}

void Device::ConnectionEvent(Conn *conn)
{
    Device *me = (Device*)conn->GetThread()->tcpConnect;
    printf("new connection: %d\n", conn->GetFd());
    me->vec.push_back(conn);
}

void Device::CloseEvent(Conn *conn, short events)
{
    printf("connection closed: %d\n", conn->GetFd());
}

void Device::QuitCb(int sig, short events, void *data)
{ 
    me->StopRun(NULL);
}
