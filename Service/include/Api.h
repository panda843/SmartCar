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

#include <string.h>
#include <ctype.h>
#include <exception>
#include <iostream>
#include <map>

#define FAVICON "favicon.ico"

using namespace std;
using namespace mysqlhelper;

class Api {
 public:
  Api(const char* ip, const unsigned int port);
  ~Api();
  void start();
  friend void requestHandler(struct evhttp_request* request, void* args);
  char* strlwr(char* str);

 private:
  char* ip;
  char* request_action;
  unsigned int port;
  MysqlHelper* mysql;
  bool is_favicon;
  struct evhttp_request* request;
  struct evkeyvalq* request_header;
  struct evkeyvalq* response_header;
  struct evhttp* httpServer = NULL;
  struct event_base* eventBase = NULL;
  void getRquestAction(const char* url);
  void initApiList();
  void user_login();
  void user_register();
  void call(const char* str);
  void sendJson(const char* json);
};

#endif
