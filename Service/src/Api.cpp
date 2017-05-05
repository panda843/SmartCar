#include "Api.h"

typedef void (Api::*pmf)();

map<string, pmf> api_list;

Api* apiThisPointer;

Api::Api(const char* ip, unsigned int port) {
  apiThisPointer = this;
  this->ip = new char[strlen(ip) + 1];
  strcpy(this->ip, ip);
  this->port = port;
  this->initApiList();
  this->mysql = new MysqlHelper();
  this->mysql->init("127.0.0.1", "root", "root", "smart_car");
  try {
    this->mysql->connect();
  } catch (MysqlHelper_Exception& excep) {
    cout << excep.errorInfo;
    exit(0);
  }
}

Api::~Api() {
  delete this->ip;
  delete this->mysql;
  delete this->request_action;
  delete this->request;
  free(this->request_header);
  free(this->response_header);
  // 释放HTTP 资源
  evhttp_free(this->httpServer);
  // 释放事件资源
  event_base_free(this->eventBase);
}

char* Api::strlwr(char* str) {
  if (str == NULL) return NULL;
  char* p = str;
  while (*p != '\0') {
    if (*p >= 'A' && *p <= 'Z') *p = (*p) + 0x20;
    p++;
  }
  return str;
}

void Api::user_login() {
  if (evhttp_request_get_command(request) == EVHTTP_REQ_GET) {
    Json::Value root;
    Json::Value data;
    MysqlHelper::MysqlData dataSet =
        this->mysql->queryRecord("select * from user");
    root["status"] = Json::Value(true);
    root["message"] = Json::Value("ok");
    if (dataSet.size() != 0) {
      for (size_t i = 0; i < dataSet.size(); ++i) {
        data["id"]= Json::Value(dataSet[i]["id"]);  
        data["username"]= Json::Value(dataSet[i]["username"]); 
        data["password"]= Json::Value(dataSet[i]["password"]);   
        data["nickname"]= Json::Value(dataSet[i]["nickname"]);  
        data["head"]= Json::Value(dataSet[i]["head"]);  
      }
    }
    root["data"] = data;
    string json = root.toStyledString();
    this->sendJson(json.c_str()); 
  }else {
    evhttp_send_error(this->request, HTTP_BADREQUEST, 0);
  }
}

void Api::user_register() {
  this->sendJson("{\"status\":true,\"message\":\"register ok\"}");
}

void Api::initApiList() {
  api_list["user_login"] = &Api::user_login;
  api_list["user_register"] = &Api::user_register;
}

void Api::call(const char* str) {
  string func = string(str, strlen(str));
  if (api_list.count(func)) {
    (this->*(api_list[func]))();
  } else {
    evhttp_send_error(this->request, HTTP_BADREQUEST, 0);
  }
}

POST_DATA Api::getPostData(const string key){
  POST_DATA post_data; 
  if(this->request_post_data.count(key)){
    post_data = this->request_post_data[key];
  }else{
    const char* val = evhttp_find_header(apiThisPointer->request_header, key.c_str());
    post_data.val = string(val,strlen(val));
    post_data.is_file = false;
  }
  return post_data;
}

void Api::getRquestAction(const char* url) {
  char* buf = new char[strlen(url) + 1];
  strcpy(buf, url);
  char* contr = strtok(buf, "/");
  if (contr != NULL) {
    // favicon.ico
    if (strcmp(contr, FAVICON) == 0) {
      this->is_favicon = true;
    } else {
      this->is_favicon = false;
      char* act = strtok(NULL, "/");
      if (act != NULL) {
        char* is_par = strtok(NULL, "?");
        if (is_par == NULL) {
          act = strtok(act, "?");
        }
      }
      char* str = new char[strlen(contr) + strlen(act) + 2];
      memset(str, 0, strlen(str));
      contr = this->strlwr(contr);
      act = this->strlwr(act);
      strcat(str, contr);
      strcat(str, "_");
      strcat(str, act);
      if (this->request_action != NULL) {
        delete this->request_action;
      }
      this->request_action = str;
    }
  }
  delete buf;
}

void Api::sendJson(const char* json) {
  struct evbuffer* evb = evbuffer_new();
  //setting response header
  evhttp_add_header(this->response_header, "Content-Type", "text/html; charset=utf-8");

  evbuffer_add_printf(evb, json);
  evhttp_send_reply(this->request, HTTP_OK, "OK", evb);

  evbuffer_free(evb);
}

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
      string startSign = string(line,strlen(line));
      char* tempKey = strtok(NULL,"\r");
      string key = string(tempKey,strlen(tempKey)); 
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
        strtok(NULL,"\n");
        char* val = strtok(NULL,"\r");
        while(strcmp(val,startSign.c_str()) != 0){
          string str(val,strlen(val));
          content = content + str;
          val = strtok(NULL,"\r");
        }
        POST_DATA post;
        post.is_file = true;
        post.name = file_name;
        post.mime = mime;
        post.val = content;
        this->request_post_data[key_name] = post;
        line = val;
      }else{
        strtok(NULL,"\r");
        char* value = strtok(NULL,"\r");
        string key_val = string(value,strlen(value));
        key_val = key_val.substr(1,key_val.length()-1);
        printf("%s:%s\n",key.c_str(),key_val.c_str() );
        POST_DATA post;
        post.is_file = false;
        post.val = key_val;
        this->request_post_data[key] = post;
        //evhttp_add_header(this->request_header, key.c_str(), key_val.c_str());
      }
      line = strtok(NULL,"\r");
    }
  }
  delete buf;
  delete post_data;
}

void requestHandler(struct evhttp_request* request, void* args) {
  apiThisPointer->request = request;
  //获取URL路径
  const char* uri = evhttp_request_get_uri(request);
  // URL路径解码
  const char* decoded_uri = evhttp_decode_uri(uri);
  //获取Action
  apiThisPointer->getRquestAction(decoded_uri);
  //判断favicon
  if (apiThisPointer->is_favicon) {
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
  apiThisPointer->parseFormData(content_type);
  //设置返回头
  apiThisPointer->response_header = evhttp_request_get_output_headers(request);
  //调用对应的action
  apiThisPointer->call(apiThisPointer->request_action);
  return;
}

void Api::start() {
  this->eventBase = event_base_new();
  if (!this->eventBase) {
    printf("create event_base failed!\n");
    return;
  }
  this->httpServer = evhttp_new(this->eventBase);
  if (!this->httpServer) {
    printf("create evhttp failed!\n");
    return;
  }
  if (evhttp_bind_socket(this->httpServer, this->ip, this->port) != 0) {
    printf("bind socket failed! port:%d\n", this->port);
    return;
  }
  // 设置事件触发后的回调函数
  evhttp_set_gencb(this->httpServer, requestHandler, NULL);
  // 设置服务超时时间，单位为秒
  evhttp_set_timeout(this->httpServer, 120);
  //  循环处理事件
  event_base_dispatch(this->eventBase);  
}
