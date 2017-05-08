#ifndef _API_H_
#define _API_H_

#include "MysqlHelper.h"
#include "json/json.h"
#include "event/buffer.h"
#include "event/event.h"
#include "event/http.h"
#include "event/http_struct.h"
#include "event/http_compat.h"
#include "event/util.h"
#include "MD5.h"

#include <signal.h> 
#include <string.h>
#include <ctype.h>
#include <exception>
#include <iostream>
#include <ctime>
#include <map>

#define FAVICON "favicon.ico"

using namespace std;
using namespace mysqlhelper;

//post数据结构
typedef struct POST_DATA_S{
  bool is_file;
  string val;
  string mime;
  string name;
}POST_DATA;
//定义token列表结构
typedef map<string,time_t> TOKEN;

class Api {
 public:
  //构造函数
  Api(const char* ip, const unsigned int port);
  //析构函数
  ~Api();
  //API运行
  void start();
  //libevent http 请求处理
  friend void requestHandler(struct evhttp_request* request, void* args);
  //libevent signal 信号处理
  friend void signal_cb(evutil_socket_t sig, short events, void * user_data);
  //转小写方法
  char* strlwr(char* str);

 private:
  //监听IP
  char* ip = NULL;
  //监听端口
  unsigned int port;
  //请求方法
  char* request_action = NULL;
  //Mysql
  MysqlHelper* mysql;
  //是否是Favicon
  bool is_favicon;
  //request信息
  struct evhttp_request* request;
  //request header 信息
  struct evkeyvalq* request_header;
  //response header 信息
  struct evkeyvalq* response_header;
  //httpServer
  struct evhttp* httpServer = NULL;
  //event base
  struct event_base* eventBase = NULL;
  //Post数据
  map<string,POST_DATA> request_post_data;
  //token列表
  TOKEN* token_list;
  //获取Post数据
  POST_DATA getPostData(const string key);
  //获取请求方法
  void getRquestAction(const char* url);
  //初始化API列表
  void initApiList();
  //创建token
  string createToken();
  //检查token
  bool checkToken(const string token);
  //调用请求对应的方法
  void call(const char* str);
  //返回数据
  void sendJson(const char* json);
  //解析form数据
  void parseFormData(const char* content_type);
  //用户登录
  void user_login();
  //用户注册
  void user_register();
  //设备列表
  void device_list();
};

#endif
