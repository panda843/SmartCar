# Service
>智能小车云平台API
# 目录结构
```bash
.
├── bin //应用程序
├── include //头文件
│   ├── Api.h
│   ├── Device.h
│   ├── event
│   ├── json
│   ├── mysql
│   └── MysqlHelper.h
├── library //动态库
│   ├── libevent-2.0.so.5
│   ├── libevent_core-2.0.so.5
│   ├── libevent_extra-2.0.so.5
│   ├── libevent_openssl-2.0.so.5
│   ├── libevent_pthreads-2.0.so.5
│   ├── libjson.so
│   ├── libmysqlclient_r.so
│   ├── libmysqlclient.so
│   ├── libmysqlclient.so.18
│   └── libmysqlclient.so.18.0.0
├── Makefile
├── README.md
├── sql //数据库结构
│   └── smart_car.sql
└── src //源代码
    ├── Api.cpp
    ├── Device.cpp
    ├── main.cpp
    └── MysqlHelper.cpp
11 directories, 389 files
```
# 编译
>make编译 
# 运行说明
运行前在当前目录执行shell命令:
```bash
    #添加环境变量
    export LD_LIBRARY_PATH=$(pwd)/library
    #执行程序
    ./bin/smartCar
```