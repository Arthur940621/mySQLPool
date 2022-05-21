#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include "MySQLConn.h"

class ConnectPool {
public:
    static ConnectPool* get_connect_pool();
    ConnectPool(const ConnectPool& obj) = delete;
    ConnectPool& operator=(const ConnectPool& obj) = delete;
    std::shared_ptr<MySQLConn> get_connection();
private:
    ConnectPool();
    ~ConnectPool();
    bool parse_json_file();
    void add_connection();
    std::string ip_;
    std::string user_;
    std::string passwd_;
    std::string db_name_;
    unsigned short port_;
    int max_conn_;
    int timeout_;
    std::mutex mtx_;
    std::condition_variable cond_;
    std::queue<MySQLConn*> mysql_conn_que_;
};