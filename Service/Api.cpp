#include "Api.h"


Api* apiThisPointer;

Api::Api(const char* ip, unsigned int port){
    apiThisPointer = this;
    this->ip = ip;
    this->port = port;
}

Api::~Api(){

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
