#include "Mysql.h"

Mysql::Mysql(){
    //    已经连接到服务器
    if ( this->isConnected ){        
        return;    
    } 
    //    初始化相关对象 
    mysql_init(&this->client);
    //    连接到服务器 
    if ( !mysql_real_connect(&this->client, HOST, USERNAME, PASSWORD, DBNAME, PORT, NULL, 0) ){

    }
    //    修改连接标识
    this->isConnected = true;
}

Mysql::~Mysql(){
    mysql_close(&this->client);
    //    修改连接标识
    this->isConnected = false;
    mysql_library_end();
}

void Mysql::exec(const char* sql){
    printf("call exec sql %s\n",sql );
    // mysql_query()执行成功返回0，失败返回非0值。与PHP中不一样  
    if(mysql_query(&this->client, sql))  {  
        cout << "Query Error:" << mysql_error(&this->client);  
    }else{  
        this->result = mysql_use_result(&this->client); // 获取结果集  
        // mysql_field_count()返回connection查询的列数  
        for(int i=0; i < mysql_field_count(&this->client); ++i)  {  
            // 获取下一行  
            this->row = mysql_fetch_row(this->result);  
            if(this->row <= 0){  
                break;  
            }  
            // mysql_num_fields()返回结果集中的字段数  
            for(int j=0; j < mysql_num_fields(this->result); ++j){  
                cout << this->row[j] << " ";  
            }  
            cout << endl;  
        }  
        // 释放结果集的内存  
        mysql_free_result(this->result);  
    }  
}