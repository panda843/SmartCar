#ifndef _MYSQL_H_
#define _MYSQL_H_

#include "Mysql.h"
#include <iostream>
#include <string>
#include <string.h>
#include <mysql/mysql.h>

#define HOST "127.0.0.1"
#define USERNAME "root"
#define PASSWORD "root"
#define DBNAME "smart_car"
#define PORT (unsigned int)3306

using namespace std;

class Mysql{
    public:
        Mysql();
        ~Mysql();
        void exec(const char* sql);
    private:
        MYSQL client;
        MYSQL_RES *result;  
        MYSQL_ROW row;  
        bool isConnected;
};
#endif