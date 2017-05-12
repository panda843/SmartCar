#include "Device.h"

map<string,pfunc> device_api_list;

Device::Device(){
    this->mysql = new MysqlHelper();
    this->mysql->init("127.0.0.1", "root", "root", "smart_car");
    this->initApiList();
}

Device::~Device(){

}

void Device::initApiList() {
  device_api_list[API_DEVICE_INFO] = &Device::handlerDeverInfo;
}

void Device::call(Conn* &conn, Json::Value &request_data,const string func){
  if(func.length() == 0){
    Json::Value root;
    Json::Value data;
    root["protocol"] = API_NOT_FIND;
    root["data"] = data;
    this->sendData(conn,root.toStyledString());
    return;
  }
  if (device_api_list.count(func)) {
    (this->*(device_api_list[func]))(conn,request_data);
  } else {
    Json::Value root;
    Json::Value data;
    root["protocol"] = API_NOT_FIND;
    root["data"] = data;
    this->sendData(conn,root.toStyledString());
  }
}

void Device::sendData(Conn* &conn,const string resp_data){
    char* data = new char[resp_data.length()+1];
    resp_data.copy(data,resp_data.length(),0);
    conn->AddToWriteBuffer(data, resp_data.length());
    delete[] data;
}

void Device::handlerDeverInfo(Conn* &conn, Json::Value &request_data){
    Json::Value root;
    Json::Value data;
    string sql = "select * from device where ";
    string name = request_data["name"].asString();
    string mac = request_data["mac"].asString();
    //获取fd并转string
    int fd = conn->GetFd();
    char intToStr[12];
    sprintf(intToStr,"%d",fd);
    string str_fd = string(intToStr);
    //检查设备是否存在
    sql = sql+"name=\""+name+"\" and mac=\""+mac+"\"";
    MysqlHelper::MysqlData dataSet = this->mysql->queryRecord(sql);
    if (dataSet.size() == 0) {
        //不存在,新增
        MysqlHelper::RECORD_DATA record;
        record.insert(make_pair("name",make_pair(MysqlHelper::DB_STR,name)));
        record.insert(make_pair("mac",make_pair(MysqlHelper::DB_STR,mac)));
        record.insert(make_pair("online",make_pair(MysqlHelper::DB_INT,"1")));
        record.insert(make_pair("status",make_pair(MysqlHelper::DB_INT,"1")));
        record.insert(make_pair("sock_fd",make_pair(MysqlHelper::DB_INT,str_fd)));
        int insert_id = this->mysql->insertRecord("device",record);
        data["id"] = insert_id;
    }else{
        //存在,更新状态
        string up_sql = "where  mac = \""+mac+"\"";
        MysqlHelper::RECORD_DATA recordChange;
        recordChange.insert(make_pair("online",make_pair(MysqlHelper::DB_INT,"1")));
        recordChange.insert(make_pair("status",make_pair(MysqlHelper::DB_INT,"1")));
        recordChange.insert(make_pair("sock_fd",make_pair(MysqlHelper::DB_INT,str_fd)));
        this->mysql->updateRecord("device",recordChange,up_sql);
        data["id"] = dataSet[0]["id"];
    }
    root["protocol"] = API_DEVICE_INFO;
    root["data"] = data;
    this->sendData(conn,root.toStyledString());
}

void Device::start(const char* ip,unsigned int port){
    this->AddSignalEvent(SIGINT, Device::QuitCb);
    this->SetPort(port);
    this->SetAddress(ip);
    this->StartRun();
}

void Device::ReadEvent(Conn *conn){
    Json::Reader reader;
    Json::Value data;
    //读取客户端数据
    int len = conn->GetReadBufferLen();
    char* str = new char[len+1];
    conn->GetReadBuffer(str,len);
    //解析数据
    if(reader.parse(str, data)){
        string func = data["protocol"].asString();
        this->call(conn,data["data"],func);
    }else{
        Json::Value root;
        Json::Value data;
        root["protocol"] = API_NOT_FIND;
        root["data"] = data;
        this->sendData(conn,root.toStyledString());
    }
}

void Device::WriteEvent(Conn *conn){
    
}

void Device::ConnectionEvent(Conn *conn){
    Device *me = (Device*)conn->GetThread()->tcpConnect;
    me->vec.push_back(conn);
    printf("new client fd: %d\n", conn->GetFd());
}

void Device::CloseEvent(Conn *conn, short events){
    char str_fd[12];
    sprintf(str_fd,"%d",conn->GetFd());
    string sql = "select * from device where sock_fd = "+string(str_fd);
    MysqlHelper::MysqlData dataSet = this->mysql->queryRecord(sql);
    if (dataSet.size() != 0) {
        string up_sql = "where  mac = \""+dataSet[0]["mac"]+"\"";
        MysqlHelper::RECORD_DATA recordChange;
        recordChange.insert(make_pair("online",make_pair(MysqlHelper::DB_INT,"2")));
        recordChange.insert(make_pair("sock_fd",make_pair(MysqlHelper::DB_INT,"0")));
        this->mysql->updateRecord("device",recordChange,up_sql);
    }else{
        printf("close sock_fd error,not find mysql socke_fd !!\n");
    }
    printf("connection closed: %d\n", conn->GetFd());
    // vector<Conn*>::iterator it;
    // for(it=this->vec.begin();it!=this->vec.end();){
    //     if(*(it)->GetFd() == data[0]["id"]){
    //         it=iVec.erase(it);
    //     }else{
    //         it++;
    //     }
    // }
}

void Device::QuitCb(int sig, short events, void *data)
{ 
    Device *me = (Device*)data;
    me->StopRun(NULL);
}
