#include "Api.h"
/**
 * 函数地址定义
 */
typedef void (Api::*pmf)();
/**
 * API列表定义
 */
map<string, pmf> api_list;
/**
 * this指针定义
 */
Api* apiThisPointer;

/**
 * 构造函数
 */
Api::Api(const char* ip, unsigned int port) {
  apiThisPointer = this;
  this->ip = new char[strlen(ip) + 1];
  strcpy(this->ip, ip);
  this->port = port;
  this->initApiList();
  this->token_list = new TOKEN();
  this->mysql = new MysqlHelper();
  this->mysql->init("127.0.0.1", "root", "root", "smart_car");
  try {
    this->mysql->connect();
  } catch (MysqlHelper_Exception& excep) {
    printf("%s\n",excep.errorInfo.c_str() );
    exit(0);
  }
}
/**
 * 析构函数
 */
Api::~Api() {
  if(this->ip != NULL){
    delete []this->ip;
    this->ip = NULL;
  }
  if(this->mysql != NULL){
    delete this->mysql;
    this->mysql = NULL;
  }
  if(this->request_action != NULL){
    delete []this->request_action;
    this->request_action = NULL;
  }
  if(this->request != NULL){
    this->request = NULL;
  }
  if(this->request_header != NULL){
    this->request_header = NULL;
  }
  if(this->response_header != NULL){
    this->response_header = NULL;
  }
  if(this->token_list != NULL){
    delete this->token_list;
    this->token_list = NULL;
  }
}
/**
 * 字符串转小写
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    str                              转换前字符串
 * @return                                    转换后字符串
 */
char* Api::strlwr(char* str) {
  if (str == NULL) return NULL;
  char* p = str;
  while (*p != '\0') {
    if (*p >= 'A' && *p <= 'Z') *p = (*p) + 0x20;
    p++;
  }
  return str;
}
/**
 * 初始化Api列表
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 */
void Api::initApiList() {
  api_list["user_login"] = &Api::user_login;
  api_list["user_register"] = &Api::user_register;
  api_list["device_list"] = &Api::device_list;
}
/**
 * 调用请求对应方法
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    str                              请求方法
 */
void Api::call(const char* str) {
  string func = string(str, strlen(str));
  if (api_list.count(func)) {
    (this->*(api_list[func]))();
  } else {
    evhttp_send_error(this->request, HTTP_BADREQUEST, 0);
  }
}
/**
 * 获取Post提交的数据
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    key                              key
 * @return                                    val
 */
POST_DATA Api::getPostData(const string key){
  POST_DATA post_data; 
  if(this->request_post_data.count(key)){
    //key存在从Map里获取
    post_data = this->request_post_data[key];
  }else{
    //不存在,检查request_header是否存在
    const char* val = evhttp_find_header(apiThisPointer->request_header, key.c_str());
    post_data.val = string(val,strlen(val));
    post_data.is_file = false;
  }
  return post_data;
}
/**
 * 获取请求地址
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    url                              Url地址
 */
void Api::getRquestAction(const char* url) {
  char* buf = new char[strlen(url) + 1];
  strcpy(buf, url);
  char* contr = strtok(buf, "/");
  if (contr != NULL) {
    // 判断favicon.ico
    if (strcmp(contr, FAVICON) == 0) {
      this->is_favicon = true;
    } else {
      this->is_favicon = false;
      //截取Action
      char* act = strtok(NULL, "/");
      if (act != NULL) {
        //存在,检查是否有?
        char* is_par = strtok(NULL, "?");
        //有?截取?前面的内容
        if (is_par == NULL) {
          act = strtok(act, "?");
        }
      }
      //设置action地址
      int len = strlen(contr) + strlen(act) + 2;
      char* str = new char[len];
      memset(str, 0, len);
      //转换小写
      contr = this->strlwr(contr);
      act = this->strlwr(act);
      //拼接格式:user_login
      strcat(str, contr);
      strcat(str, "_");
      strcat(str, act);
      if (this->request_action != NULL) {
        delete []this->request_action;
        this->request_action = NULL;
      }
      this->request_action = str;
    }
  }
  delete []buf;
}
/**
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    json                             返回数据内容
 */
void Api::sendJson(const char* json) {
  struct evbuffer* evb = evbuffer_new();
  //setting response header
  evhttp_add_header(this->response_header, "Content-Type", "text/html; charset=utf-8");
  evbuffer_add_printf(evb, json);
  evhttp_send_reply(this->request, HTTP_OK, "OK", evb);
  evbuffer_free(evb);
}
/**
 * Form数据解析
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    content_type                     Content-Type
 */
void Api::parseFormData(const char* content_type){
  //获取POST数据
  size_t post_size = evbuffer_get_length(this->request->input_buffer);
  char * post_data = new char[post_size+1];
  memset(post_data,0,post_size+1);
  evbuffer_copyout(this->request->input_buffer,post_data,post_size+1);
  //数据长度判断
  if(post_size < 0 ){
    return;
  }
  char* buf = new char[strlen(content_type) + 1];
  strcpy(buf, content_type);
  //判断x-www-form-urlencoded并解析
  if(strcmp(buf,"application/x-www-form-urlencoded") == 0){
    //解析并放到request_header里面
    string str = "/?"+string(post_data,strlen(post_data));
    evhttp_parse_query(str.c_str(), evhttp_request_get_input_headers(this->request));
    return;
  }
  //截取multipart/form-data
  char* content = strtok(buf, ";");
  if(!content){
    return;
  }
  //判断multipart/form-data
  if(strcmp(content,"multipart/form-data") == 0){
    char* line = strtok(post_data,"\r");
    while(line != NULL){
      //获取开始标记
      string startSign = string(line,strlen(line));
      //获取key内容
      char* tempKey = strtok(NULL,"\r");
      string key = string(tempKey,strlen(tempKey)); 
      //获取key位置
      int len = key.length();
      int pos = key.find("name=")+6;
      int end = key.length()-(pos+1);
      if(len < pos){
        break;
      }
      //截取KEY
      key = key.substr(pos,end);
      int find = key.find("filename=");
      //检测是不是文件
      if(find > 0){
        //获取文件名
        int file_start = key.find("filename=")+10;
        int file_end = key.length()-file_start;
        string file_name = key.substr(file_start,file_end);
        //获取KEY名字
        int key_name_pos = key.find("\";");
        string key_name = key.substr(0,key_name_pos);
        //获取文件类型
        char* type = strtok(NULL,"\r");
        string mime(type,strlen(type));
        mime = mime.substr(13,mime.length()-13);
        //获取文件内容
        string content = "";
        //跳过空行
        strtok(NULL,"\n");
        //获取第一行数据
        char* val = strtok(NULL,"\r");
        //获取文件内容
        while(strcmp(val,startSign.c_str()) != 0){
          string str(val,strlen(val));
          content = content + str;
          val = strtok(NULL,"\r");
        }
        //构造Post数据
        POST_DATA post;
        post.is_file = true;
        post.name = file_name;
        post.mime = mime;
        post.val = content;
        this->request_post_data[key_name] = post;
        //获取下一行
        line = val;
      }else{
        //跳过空行
        strtok(NULL,"\r");
        //获取Value
        char* value = strtok(NULL,"\r");
        //解析Value
        string key_val = string(value,strlen(value));
        key_val = key_val.substr(1,key_val.length()-1);
        //构造Post数据
        POST_DATA post;
        post.is_file = false;
        post.val = key_val;
        this->request_post_data[key] = post;
      }
      line = strtok(NULL,"\r");
    }
  }
  delete []buf;
  delete []post_data;
}
/**
 * 创建Token
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @return   [description]
 */
string Api::createToken(){
  //创建随机字符
  char str[33];
  int i;
  srand(time(NULL));
  for(i=0;i<31;++i){
    str[i]='a'+rand()%26;
  }
  str[++i]='\0';
  string token_key(str,strlen(str));
  //获取当前时间
  time_t current_time = time(NULL);
  //加1小时
  struct tm *ptm = gmtime(&current_time);
  ptm->tm_hour = ptm->tm_hour+1;
  current_time = mktime(ptm);
  //放入token列表
  this->token_list->insert(make_pair(token_key, current_time));
  return token_key;
}
/**
 * 检查Token
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    token                            token
 * @return                                    token是否正常
 */
bool Api::checkToken(const string token){
  TOKEN::iterator iter;
  //获取当前时间
  time_t current_time = time(NULL);
  //删除过期token
  for(iter = this->token_list->begin(); iter != this->token_list->end(); ++iter){
    time_t token_time = iter->second;
    //检查token是否过期
    if(current_time < token_time){
      this->token_list->erase(iter);
    }
  }
  //检查token是否存在
  if(this->token_list->count(token)){
    return true;
  }else{
    return false;
  }
}
/**
 * 请求处理
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    request                          http request 结构体
 * @param    args                             参数
 */
void requestHandler(struct evhttp_request* request, void* args) {
  apiThisPointer->request = request;
  //获取URL路径
  const char* uri = evhttp_request_get_uri(request);
  // URL路径解码
  char* decoded_uri = evhttp_decode_uri(uri);
  //获取Action
  apiThisPointer->getRquestAction(decoded_uri);
  //判断favicon
  if (apiThisPointer->is_favicon) {
    //释放资源
    free(decoded_uri);
    evhttp_send_reply(request, HTTP_OK, "OK", NULL);
    return;
  }
  //获取提交的Header
  apiThisPointer->request_header = evhttp_request_get_input_headers(request);
  //获取Content-Type
  const char* content_type = evhttp_find_header(apiThisPointer->request_header, "Content-Type");
  //解析提交的GET参数
  evhttp_parse_query(decoded_uri, evhttp_request_get_input_headers(request));
  //解析POST的Form表单
  if (evhttp_request_get_command(request) == EVHTTP_REQ_POST) {
    apiThisPointer->parseFormData(content_type);
  }
  //设置返回头
  apiThisPointer->response_header = evhttp_request_get_output_headers(request);
  //释放资源
  free(decoded_uri);
  //调用对应的action
  apiThisPointer->call(apiThisPointer->request_action);
  return;
}

/**
 * libevent 信号处理
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 * @param    sig                              信号ID
 * @param    events                           [description]
 * @param    user_data                        baseEvent
 */
void signal_cb(evutil_socket_t sig, short events, void * user_data)
{
    struct event_base * base = (struct event_base *)user_data;
    event_base_loopexit(base, NULL);
}
/**
 * -----------------------------------------------------------------------------------------------------
 * API列表
 * -----------------------------------------------------------------------------------------------------
 */
/**
 * API运行
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 */
void Api::start() {
  //创建eventBase
  this->eventBase = event_base_new();
  if (!this->eventBase) {
    printf("create event_base failed!\n");
    return;
  }
  //新建httpServer
  this->httpServer = evhttp_new(this->eventBase);
  if (!this->httpServer) {
    printf("create evhttp failed!\n");
    return;
  }
  //绑定端口
  if (evhttp_bind_socket(this->httpServer, this->ip, this->port) != 0) {
    printf("bind socket failed! port:%d\n", this->port);
    return;
  }
  // 初始化信号处理event 
  struct event * signal_event = evsignal_new(this->eventBase, SIGINT, signal_cb, (void * )this->eventBase);
  // 把这个callback放入base中 
  if (!signal_event || event_add(signal_event, NULL)<0) {
    printf("Listen SIGINT error \n");
    return;
  }
  // 设置事件触发后的回调函数
  evhttp_set_gencb(this->httpServer, requestHandler, NULL);
  // 设置服务超时时间，单位为秒
  evhttp_set_timeout(this->httpServer, 120);
  //  循环处理事件
  event_base_dispatch(this->eventBase);  
  //释放signal资源
  event_free(signal_event);
  // 释放HTTP 资源
  evhttp_free(this->httpServer);
  // 释放事件资源
  event_base_free(this->eventBase);
}
/**
 * 用户登录
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 */
void Api::user_login() {
  if (evhttp_request_get_command(request) == EVHTTP_REQ_POST) {
    string sql = "select * from user where ";
    Json::Value root;
    Json::Value data;
    //检查参数
    POST_DATA username = this->getPostData("username");
    POST_DATA password = this->getPostData("password");

    if(username.val.length() == 0 || password.val.length() == 0){
      evhttp_send_error(this->request, HTTP_BADREQUEST, 0);
      return;
    }
    //查询数据
    sql = sql+"username=\""+username.val+"\" and password=\""+MD5(password.val).toStr()+"\"";
    //查找用户是否存在
    MysqlHelper::MysqlData dataSet = this->mysql->queryRecord(sql);
    if (dataSet.size() != 0) {
      //用户存在创建Token
      root["status"] = Json::Value(true);
      root["message"] = Json::Value("ok");
      data["token"] = this->createToken();
    }else{
      root["status"] = Json::Value(false);
      root["message"] = Json::Value("账号或密码不正确");
    }
    root["data"] = data;
    string json = root.toStyledString();
    this->sendJson(json.c_str()); 
  }else {
    evhttp_send_error(this->request, HTTP_BADREQUEST, 0);
  }
}
/**
 * 用户注册
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 */
void Api::user_register() {
  this->sendJson("{\"status\":false,\"message\":\"Not open registration\"}");
}
/**
 * 获取设备列表
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-05-08
 */
void Api::device_list(){
  if (evhttp_request_get_command(request) == EVHTTP_REQ_GET) {
    const char* token = evhttp_find_header(this->request_header,"token");
    //判断token是否为空
    if(token == NULL){
      evhttp_send_error(this->request, 401, 0);
      return;
    }
    //检查token是否合法
    if(!this->checkToken(string(token,strlen(token)))){
      evhttp_send_error(this->request, 401, 0);
      return;
    }
    this->sendJson("token ok");
  }else{
    evhttp_send_error(this->request, HTTP_BADREQUEST, 0);
  }
}

