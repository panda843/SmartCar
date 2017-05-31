//TcpEvent.cpp
#include "TcpEvent.h"

Conn::Conn(int fd) : m_fd(fd)
{
    m_Prev = NULL;
    m_Next = NULL;
}

Conn::~Conn()
{

}

ConnQueue::ConnQueue()
{
    //建立头尾结点，并调整其指针
    m_head = new Conn(0);
    m_tail = new Conn(0);
    m_head->m_Prev = m_tail->m_Next = NULL;
    m_head->m_Next = m_tail;
    m_tail->m_Prev = m_head;
}

ConnQueue::~ConnQueue()
{
    Conn *tcur, *tnext;
    tcur = m_head;
    //循环删除链表中的各个结点
    while( tcur != NULL )
    {
        tnext = tcur->m_Next;
        delete tcur;
        tcur = tnext;
    }
}

Conn *ConnQueue::InsertConn(int fd, LibeventThread *t)
{
    Conn *c = new Conn(fd);
    c->m_Thread = t;
    Conn *next = m_head->m_Next;

    c->m_Prev = m_head;
    c->m_Next = m_head->m_Next;
    m_head->m_Next = c;
    next->m_Prev = c;
    return c;
}

void ConnQueue::DeleteConn(Conn *c)
{
    c->m_Prev->m_Next = c->m_Next;
    c->m_Next->m_Prev = c->m_Prev;
    delete c;
}

/*
void ConnQueue::PrintQueue()
{
    Conn *cur = m_head->m_Next;
    while( cur->m_Next != NULL )
    {
        printf("%d ", cur->m_fd);
        cur = cur->m_Next;
    }
    printf("\n");
}
*/

TcpEventServer::TcpEventServer()
{
    //初始化各项数据
    m_ThreadCount = THREAD_NUMBER;
    m_MainBase = new LibeventThread;
    m_Threads = new LibeventThread[m_ThreadCount];
    m_MainBase->tid = pthread_self();
    m_MainBase->base = event_base_new();

    //初始化各个子线程的结构体
    for(int i=0; i<m_ThreadCount; i++)
    {
        SetupThread(&m_Threads[i]);
    }

}

TcpEventServer::~TcpEventServer()
{
    //停止事件循环（如果事件循环没开始，则没效果）
    StopRun(NULL);

    //释放内存
    event_base_free(m_MainBase->base);
    for(int i=0; i<m_ThreadCount; i++)
        event_base_free(m_Threads[i].base);

    delete m_MainBase;
    delete [] m_Threads;
    delete[] m_Ip;
}

void* TcpEventServer::createPthreadSendPipeData(void *arg){
    TcpEventServer* ser = (TcpEventServer*)arg;
    while(true){
        if(strcmp(ser->write_pipe_data, "\0") != 0){
            pthread_mutex_lock(&ser->mutex_write);
            write(ser->sock_write_pipe[1], ser->write_pipe_data, sizeof(ser->write_pipe_data));
            memset(ser->write_pipe_data,0,sizeof(ser->write_pipe_data));
            pthread_mutex_unlock(&ser->mutex_write);
        }
    }
    return NULL;
}
void* TcpEventServer::createPthreadReadPipeData(void *arg){
    TcpEventServer* ser = (TcpEventServer*)arg;
    while(true){
        char str[SOCK_PIPE_MAXDATA] = {0};
        read(ser->sock_read_pipe[0], str, SOCK_PIPE_MAXDATA);
        ser->ReadApiEvent(str);
    }
    return NULL;
}

void TcpEventServer::setPipe(int *read_fd,int *write_fd){
  pthread_t thread_write_pid,thread_read_pid;
  this->sock_write_pipe = write_fd;
  this->sock_read_pipe = read_fd;
  close(this->sock_write_pipe[0]);
  close(this->sock_read_pipe[1]);
  if(pthread_mutex_init(&mutex_write,NULL) != 0){  
    printf("Init metux error.");  
    exit(0);  
  }  
  if(!pthread_create(&thread_write_pid,NULL,createPthreadSendPipeData,(void*)this)){
    pthread_detach(thread_write_pid);
  }else{
    printf("Device PIPE Pthread create failed\n");
    exit(0);
  }
  if(!pthread_create(&thread_read_pid,NULL,createPthreadReadPipeData,(void*)this)){
    pthread_detach(thread_read_pid);
  }else{
    printf("Device PIPE Pthread create failed\n");
    exit(0);
  }
}
void TcpEventServer::sendApiData(const char* str){
    pthread_mutex_lock(&this->mutex_write);
    memset(this->write_pipe_data,0,sizeof(this->write_pipe_data));
    strcpy(this->write_pipe_data,str);
    pthread_mutex_unlock(&this->mutex_write);
}

void TcpEventServer::ErrorQuit(const char *str)
{
    //输出错误信息，退出程序
    fprintf(stderr, "%s", str);   
    if( errno != 0 )    
        fprintf(stderr, " : %s", strerror(errno));    
    fprintf(stderr, "\n");        
    exit(1);    
}

void TcpEventServer::SetupThread(LibeventThread *me)
{
    //建立libevent事件处理机制
    me->tcpConnect = this;
    me->base = event_base_new();
    if( NULL == me->base )
        ErrorQuit("event base new error");

    //在主线程和子线程之间建立管道
    int fds[2];
    if(pipe(fds) == -1){
        ErrorQuit("create pipe error");
    }
    me->notifyReceiveFd = fds[0];
    me->notifySendFd = fds[1];

    //让子线程的状态机监听管道
    event_set( &me->notifyEvent, me->notifyReceiveFd,
        EV_READ | EV_PERSIST, ThreadProcess, me );
    event_base_set(me->base, &me->notifyEvent);
    if ( event_add(&me->notifyEvent, 0) == -1 )
        ErrorQuit("Can't monitor libevent notify pipe\n");
    return;
}

void *TcpEventServer::WorkerLibevent(void *arg)
{
    //开启libevent的事件循环，准备处理业务
    LibeventThread *me = (LibeventThread*)arg;
    //printf("thread %u started\n", (unsigned int)me->tid);
    event_base_dispatch(me->base);
    //printf("subthread done\n");
    return arg;
}

void TcpEventServer::StartRun()
{
    evconnlistener *listener;

    //如果端口号不是EXIT_CODE，就监听该端口号
    if( m_Port != EXIT_CODE )
    {
        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr(m_Ip);  
        sin.sin_port = htons(m_Port);
        listener = evconnlistener_new_bind(m_MainBase->base, 
            ListenerEventCb, (void*)this,
            LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
            (sockaddr*)&sin, sizeof(sockaddr_in));
        if( NULL == listener )
            ErrorQuit("TCP listen error");
    }

    //开启各个子线程
    for(int i=0; i<m_ThreadCount; i++)
    {
        pthread_create(&m_Threads[i].tid, NULL,  
            WorkerLibevent, (void*)&m_Threads[i]);
    }

    //开启主线程的事件循环
    event_base_dispatch(m_MainBase->base);

    //事件循环结果，释放监听者的内存
    if( m_Port != EXIT_CODE )
    {
        //printf("free listen\n");
        evconnlistener_free(listener);
    }
}

void TcpEventServer::StopRun(timeval *tv)
{
    int contant = EXIT_CODE;
    //向各个子线程的管理中写入EXIT_CODE，通知它们退出
    for(int i=0; i<m_ThreadCount; i++)
    {
        write(m_Threads[i].notifySendFd, &contant, sizeof(int));
    }
    //结果主线程的事件循环
    event_base_loopexit(m_MainBase->base, tv);
    return;
}

void TcpEventServer::ListenerEventCb(struct evconnlistener *listener, 
                                    evutil_socket_t fd,
                                    struct sockaddr *sa, 
                                    int socklen, 
                                    void *user_data)
{
    TcpEventServer *server = (TcpEventServer*)user_data;

    //随机选择一个子线程，通过管道向其传递socket描述符
    int num = rand() % server->m_ThreadCount;
    int sendfd = server->m_Threads[num].notifySendFd;
    write(sendfd, &fd, sizeof(evutil_socket_t));
    return;
}

void TcpEventServer::ThreadProcess(int fd, short which, void *arg)
{
    LibeventThread *me = (LibeventThread*)arg;

    //从管道中读取数据（socket的描述符或操作码）
    int pipefd = me->notifyReceiveFd;
    evutil_socket_t confd;
    read(pipefd, &confd, sizeof(evutil_socket_t));

    //如果操作码是EXIT_CODE，则终于事件循环
    if( EXIT_CODE == confd )
    {
        event_base_loopbreak(me->base);
        return;
    }

    //新建连接
    struct bufferevent *bev;
    bev = bufferevent_socket_new(me->base, confd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev)
    {
        fprintf(stderr, "Error constructing bufferevent!");
        event_base_loopbreak(me->base);
        return;
    }

    //将该链接放入队列
    Conn *conn = me->connectQueue.InsertConn(confd, me);

    //准备从socket中读写数据
    bufferevent_setcb(bev, ReadEventCb, WriteEventCb, CloseEventCb, conn);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_enable(bev, EV_READ);

    //调用用户自定义的连接事件处理函数
    me->tcpConnect->ConnectionEvent(conn);
}

void TcpEventServer::ReadEventCb(struct bufferevent *bev, void *data)
{
    Conn *conn = (Conn*)data;
    conn->m_ReadBuf = bufferevent_get_input(bev);
    conn->m_WriteBuf = bufferevent_get_output(bev);

    //调用用户自定义的读取事件处理函数
    conn->m_Thread->tcpConnect->ReadEvent(conn);
} 

void TcpEventServer::WriteEventCb(struct bufferevent *bev, void *data)
{
    Conn *conn = (Conn*)data;
    conn->m_ReadBuf = bufferevent_get_input(bev);
    conn->m_WriteBuf = bufferevent_get_output(bev);

    //调用用户自定义的写入事件处理函数
    conn->m_Thread->tcpConnect->WriteEvent(conn);

}

void TcpEventServer::CloseEventCb(struct bufferevent *bev, short events, void *data)
{
    Conn *conn = (Conn*)data;
    //调用用户自定义的断开事件处理函数
    conn->m_Thread->tcpConnect->CloseEvent(conn, events);
    conn->GetThread()->connectQueue.DeleteConn(conn);
    bufferevent_free(bev);
}

bool TcpEventServer::AddSignalEvent(int sig, void (*ptr)(int, short, void*))
{
    //新建一个信号事件
    event *ev = evsignal_new(m_MainBase->base, sig, ptr, (void*)this);
    if ( !ev || 
        event_add(ev, NULL) < 0 )
    {
        event_del(ev);
        return false;
    }

    //删除旧的信号事件（同一个信号只能有一个信号事件）
    DeleteSignalEvent(sig);
    m_SignalEvents[sig] = ev;

    return true;
}

bool TcpEventServer::DeleteSignalEvent(int sig)
{
    map<int, event*>::iterator iter = m_SignalEvents.find(sig);
    if( iter == m_SignalEvents.end() )
        return false;

    event_del(iter->second);
    m_SignalEvents.erase(iter);
    return true;
}

event *TcpEventServer::AddTimerEvent(void (*ptr)(int, short, void *),timeval tv, bool once){
    int flag = 0;
    if( !once )
        flag = EV_PERSIST;

    //新建定时器信号事件
    event *ev = new event;
    event_assign(ev, m_MainBase->base, -1, flag, ptr, (void*)this);
    if( event_add(ev, &tv) < 0 )
    {
        event_del(ev);
        return NULL;
    }
    return ev;
}

bool TcpEventServer::DeleteTImerEvent(event *ev)
{
    int res = event_del(ev);
    return (0 == res);
}