#include "Api.h"
#include "Mysql.h"
Api* apiThisPointer;
typedef void (Api::*pmf)(); 
map<string, pmf> api_list; 

Api::Api(const char* ip, unsigned int port){
    apiThisPointer = this;
    this->ip = new char[strlen(ip)+1];
    strcpy(this->ip,ip);
    this->port = port;
    this->initApiList();
    this->mysql = new Mysql();
}

Api::~Api(){

}

char* Api::strlwr(char* str){
    if(str == NULL)
        return NULL;
    char *p = str;
    while (*p != '\0'){
        if(*p >= 'A' && *p <= 'Z')
            *p = (*p) + 0x20;
        p++;
    }
    return str;
}

void Api::user_login(){
    this->mysql->exec("select * from user");
    printf("call user_login function \n");
}

void Api::initApiList(){
    api_list["user_login"] = &Api::user_login; 
}

void Api::call(const char* str){
    string func = string(str,strlen(str));
    if(api_list.count(func)){
     (this->*(api_list[func]))(); 
    }else{
        printf("not find function\n"); 
    }
}


void Api::getRquestAction(const char* url){
    char *buf = new char[strlen(url)+1];
    strcpy(buf, url);
    char* contr = strtok( buf, ",/");
    if(contr != NULL){
        //favicon.ico
        if(strcmp(contr,FAVICON) == 0){
            return;
        }else{
            char* act = strtok( NULL, "/");
            if(act != NULL){
                char* is_par = strtok( NULL, "?");
                if(is_par == NULL){
                    act = strtok( act, "?");
                }
            }
            char* str = new char[strlen(contr)+strlen(act)+2];
            memset(str,0,strlen(str));
            contr = this->strlwr(contr); 
            act = this->strlwr(act); 
            strcat(str,contr);
            strcat(str,"_");
            strcat(str,act);
            if(this->request_action != NULL){
                delete this->request_action;
            }
            this->request_action = str;
        }
    }
    delete buf;
}



void requestHandler(struct evhttp_request *request, void *args){
    // switch(evhttp_request_get_command(request)){
    //     case EVHTTP_REQ_GET:
    //     break;
    //     case EVHTTP_REQ_POST:
    //     break;
    // }
    const char *uri = evhttp_request_get_uri(request);
    const char *decoded_uri = evhttp_decode_uri(uri);
    // 获取Action
    apiThisPointer->getRquestAction(decoded_uri);
    //调用对应的action
    apiThisPointer->call(apiThisPointer->request_action);

    /* Decode the URI */
    struct evhttp_uri* decoded = evhttp_uri_parse(uri);
    if (!decoded) {
        evhttp_send_error(request, HTTP_BADREQUEST, 0);
        return;
    }
        
    /* Let's see what path the user asked for. */
    const char *path = evhttp_uri_get_path(decoded);
        
    struct evbuffer *evb = evbuffer_new();
    // setting response header
    struct evkeyvalq *headers = evhttp_request_get_output_headers(request);
    evhttp_add_header(headers, "Content-Type", "text/json; charset=utf-8");

    evbuffer_add_printf(evb, "Server response. Your request url is %s\n", decoded_uri);
    evhttp_send_reply(request, HTTP_OK, "OK", evb);
        
    evbuffer_free(evb);
}

void Api::start(){
    this->eventBase = event_base_new();
    if (!this->eventBase){
        printf("create event_base failed!\n");
        return;
    }
    this->httpServer = evhttp_new(this->eventBase);
    if (!this->httpServer){
        printf("create evhttp failed!\n");
        return;
    }
    if (evhttp_bind_socket(this->httpServer, this->ip, this->port) != 0){
        printf("bind socket failed! port:%d\n", this->port);
        return;
    }
    // 设置事件触发后的回调函数
    evhttp_set_gencb(this->httpServer, requestHandler, NULL);
    // 设置服务超时时间，单位为秒
    evhttp_set_timeout(this->httpServer, 120);
    //  循环处理事件
    event_base_dispatch(this->eventBase);  
    // 释放HTTP 资源
    evhttp_free(this->httpServer);   
    // 释放事件资源
    event_base_free(this->eventBase);
}
