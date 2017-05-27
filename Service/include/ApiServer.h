#ifndef _API_SERVER_H_
#define _API_SERVER_H_

#include <signal.h> 
#include <string.h>
#include <ctype.h>
#include <exception>
#include <iostream>
#include <ctime>
#include <map>

#include "event/buffer.h"
#include "event/event.h"
#include "event/http.h"
#include "event/http_struct.h"
#include "event/http_compat.h"

using namespace std;

#ifndef API_SERVER_STRUC
#define API_SERVER_STRUC

#define FAVICON "favicon.ico"
//定义token列表结构
typedef map<string,time_t> TOKEN;
//post数据结构
typedef struct POST_DATA_S{
  bool is_file;
  string val;
  string mime;
  string name;
}POST_DATA;

#endif

class ApiServer{
public:
    ApiServer();
    virtual ~ApiServer();
    void setAddress(const char *ip);
    void setPort(const int port);
    void startRun();
    void getRquestAction(const char* url);
    struct event_base* getEventBase();
    bool isFavicon();
    void setRequestHeader(struct evkeyvalq* header);
    void setResponseHeader(struct evkeyvalq* header);
    struct evkeyvalq* getRequestHeader();
    struct evkeyvalq* getResponseHeader();
    void parseFormData(struct evhttp_request* request,const char* content_type);
    char* getRequestAction();
private:
    //是否是Favicon
    bool is_favicon = false;
    //请求方法
    char* request_action = NULL;
    //event base
    struct event_base* eventBase = NULL;
    //request header 信息
    struct evkeyvalq* request_header;
    //response header 信息
    struct evkeyvalq* response_header;
    //Post数据
    map<string,POST_DATA> request_post_data;
    //token列表
    TOKEN* token_list;
    const char *ip;
    int port;
    //转小写方法
    char* strlwr(char* str);
    //libevent http 请求处理
    static void requestHandler(struct evhttp_request* request, void* args);
    //libevent signal 信号处理
    static void signalHandler(evutil_socket_t sig, short events, void* args);
protected:
    //获取Post数据
    POST_DATA getPostData(const string key);
    //创建token
    string createToken();
    //检查token
    bool checkToken(const string token);
    //返回数据
    void sendJson(struct evhttp_request* request,const char* json);
    //libevent http 请求处理
    virtual void read_cb(struct evhttp_request* request){};
    //libevent signal 信号处理
    virtual void signal_cb(evutil_socket_t sig, short events, struct event_base* evnet){};
};

#endif