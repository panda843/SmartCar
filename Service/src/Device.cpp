#include "Device.h"

Device::Device(){
    this->initApiList();
    this->mysql = new MysqlHelper();
}

Device::~Device(){
    delete this->mysql;
}

void Device::setConfig(const char* path){
    this->AddSignalEvent(SIGINT, Device::QuitCb);
    Config config;
    //检测配置文件是否存在
    if(!config.FileExist(path)){
      printf("not find config file\n");
      exit(0);
    }
    //读取配置
    config.ReadFile(path);     
    string api_host = config.Read("SERVER_HOST", api_host);
    string mysql_host = config.Read("MYSQL_HOST", mysql_host);
    string mysql_user = config.Read("MYSQL_USER", mysql_user);
    string mysql_pass = config.Read("MYSQL_PASS", mysql_pass);
    string mysql_db = config.Read("MYSQL_DB", mysql_db);
    int mysql_port = config.Read("MYSQL_PORT", mysql_port);
    int api_port = config.Read("DEVICE_PORT", api_port);
    //设置监听
    this->SetAddress(api_host.c_str()); 
    this->SetPort(api_port);
    //设置mysql信息
    this->mysql->init(mysql_host.c_str(), mysql_user.c_str(), mysql_pass.c_str(), mysql_db.c_str(),"",mysql_port);
    try {
        this->mysql->connect();
    } catch (MysqlHelper_Exception& excep) {
        printf("%s\n",excep.errorInfo.c_str() );
        exit(0);
    }  
}

void Device::initApiList() {
  this->device_api_list[API_DEVICE_INFO] = &Device::handlerDeverInfo;
  this->device_api_list[API_DEVICE_BASE_INFO] = &Device::handlerGetDeviceBaseInfo;
  this->device_api_list[API_DEVICE_KEY_DOWN] = &Device::handlerKeyDown;
  this->device_api_list[API_SET_CAMERA_POWER] = &Device::setCameraPower;
}

void Device::call(Conn* &conn, Json::Value &request_data,const string func){
  if(func.length() == 0){
    return;
  }
  if (this->device_api_list.count(func)) {
    (this->*(this->device_api_list[func]))(conn,request_data);
  }
}

void Device::sendData(Conn* &conn,const string resp_data){
    char* data = new char[resp_data.length()+1];
    resp_data.copy(data,resp_data.length(),0);
    conn->AddToWriteBuffer(data, resp_data.length());
    delete[] data;
}
void Device::handlerKeyDown(Conn* &conn, Json::Value &request_data){
    string keycode = request_data["data"]["key"].asString();
    if(keycode.compare("0") !=0 ){
        this->sendData(conn,request_data.toStyledString().c_str());
    }
}
void Device::handlerGetDeviceBaseInfo(Conn* &conn, Json::Value &request_data){
    Json::Value temp = request_data;
    if(temp["is_api"].asBool()){
        this->sendData(conn,request_data.toStyledString().c_str());
    }else{
        this->sendApiData(request_data.toStyledString().c_str());
    }
}

void Device::setCameraPower(Conn* &conn, Json::Value &request_data){
    Json::Value temp = request_data;
    if(temp["is_api"].asBool()){
        this->sendData(conn,request_data.toStyledString().c_str());
    }else{
        this->sendApiData(request_data.toStyledString().c_str());
    }
}

void Device::handlerDeverInfo(Conn* &conn, Json::Value &request_data){
    string sql = "select * from device where ";
    string name = request_data["data"]["name"].asString();
    string mac = request_data["data"]["mac"].asString();
    //获取fd并转string
    int fd = conn->GetFd();
    char intToStr[12];
    sprintf(intToStr,"%d",fd);
    string str_fd = string(intToStr);
    //检查设备是否存在
    sql = sql+"mac=\""+mac+"\"";
    MysqlHelper::MysqlData dataSet = this->mysql->queryRecord(sql);
    if (dataSet.size() == 0) {
        //不存在,新增
        MysqlHelper::RECORD_DATA record;
        record.insert(make_pair("name",make_pair(MysqlHelper::DB_STR,name)));
        record.insert(make_pair("mac",make_pair(MysqlHelper::DB_STR,mac)));
        record.insert(make_pair("online",make_pair(MysqlHelper::DB_INT,"1")));
        record.insert(make_pair("status",make_pair(MysqlHelper::DB_INT,"1")));
        record.insert(make_pair("sock_fd",make_pair(MysqlHelper::DB_INT,str_fd)));
        this->mysql->insertRecord("device",record);
    }else{
        //存在,更新状态
        string up_sql = "where mac = \""+mac+"\"";
        MysqlHelper::RECORD_DATA recordChange;
        recordChange.insert(make_pair("name",make_pair(MysqlHelper::DB_STR,name)));
        recordChange.insert(make_pair("online",make_pair(MysqlHelper::DB_INT,"1")));
        recordChange.insert(make_pair("status",make_pair(MysqlHelper::DB_INT,"1")));
        recordChange.insert(make_pair("sock_fd",make_pair(MysqlHelper::DB_INT,str_fd)));
        this->mysql->updateRecord("device",recordChange,up_sql);
    }
    //设置上线消息
    Json::Value root;
    Json::Value data;
    root["protocol"] = "addMessage";
    data["level"] = MSG_LEVEL_SYSTEM;
    data["title"] = "设备("+name+")上线了";
    data["content"] = "设备("+name+")上线了";
    root["data"] = data;
    this->sendApiData(root.toStyledString().c_str());
}

void Device::ReadApiEvent(const char *str){
    Json::Reader reader;
    Json::Value data;
    string msg(str,strlen(str));
    reader.parse(msg.c_str(), data);
    string func = data["protocol"].asString();
    int sock_fd = atoi(data["data"]["sockfd"].asString().c_str());
    Conn* conn = this->getConnBaySocketFd(sock_fd);
    if(conn != NULL){
        this->call(conn,data,func);
    }else{
        this->SetDeviceOffline(sock_fd);
        Json::Value root;
        Json::Value data;
        root["protocol"] = API_NOT_FIND_DEVICE;
        root["data"] = data;
        this->sendApiData(root.toStyledString().c_str());
    }
}

void Device::SetDeviceOffline(int fd){
    char str_fd[12];
    sprintf(str_fd,"%d",fd);
    string sql = "select * from device where sock_fd = "+string(str_fd);
    MysqlHelper::MysqlData dataSet = this->mysql->queryRecord(sql);
    if (dataSet.size() != 0) {
        //添加下线消息
        Json::Value root;
        Json::Value data;
        root["protocol"] = "addMessage";
        data["level"] = MSG_LEVEL_SYSTEM;
        data["title"] = "设备("+dataSet[0]["name"]+")下线了";
        data["content"] = "设备("+dataSet[0]["name"]+")下线了";
        root["data"] = data;
        this->sendApiData(root.toStyledString().c_str());
        //更新状态
        string up_sql = "where  mac = \""+dataSet[0]["mac"]+"\"";
        MysqlHelper::RECORD_DATA recordChange;
        recordChange.insert(make_pair("online",make_pair(MysqlHelper::DB_INT,"2")));
        recordChange.insert(make_pair("sock_fd",make_pair(MysqlHelper::DB_INT,"0")));
        this->mysql->updateRecord("device",recordChange,up_sql);
    }
}

void Device::ReadEvent(Conn *conn){
    Json::Reader reader;
    Json::Value data;
    //读取客户端数据
    int len = conn->GetReadBufferLen();
    char* str = new char[len+1];
    memset(str,0,len+1);
    conn->GetReadBuffer(str,len);
    //解析数据
    if(reader.parse(str, data)){
        string func = data["protocol"].asString();
        this->call(conn,data,func);
    }
    delete[] str;
}

void Device::WriteEvent(Conn *conn){
    
}

void Device::ConnectionEvent(Conn *conn){
    int sock_fd = conn->GetFd();
    this->sock_list[sock_fd] = conn;
}

Conn* Device::getConnBaySocketFd(int sock_fd){
    Conn* conn = NULL;
    map<int,Conn*>::iterator iter;
    for(iter=this->sock_list.begin(); iter!=this->sock_list.end(); iter++){
        if (iter->first == sock_fd){  
            conn = iter->second; 
        }
    }
    return conn;
}

void Device::CloseEvent(Conn *conn, short events){
    this->SetDeviceOffline(conn->GetFd());
    map<int,Conn*>::iterator iter;
    for(iter=this->sock_list.begin(); iter!=this->sock_list.end();){
        int fd = iter->first;
        ++iter;
        if (fd == conn->GetFd()){  
            this->sock_list.erase(fd);  
        }
    }
}

void Device::QuitCb(int sig, short events, void *data)
{ 
    Device *me = (Device*)data;
    me->StopRun(NULL);
}
