#include "ConnectPool.h"
#include "../Json/Json.h"
#include <fstream>
#include <thread>
#include <iostream>

bool ConnectPool::parse_json_file() {
    std::ifstream ifs("../config.json");
    std::string json_str;
    while (ifs) {
        std::string line;
        std::getline(ifs, line);
        json_str += line + "\n";
    }
    std::string err_msg;
    myJson::Json json = myJson::Json::parse(json_str, err_msg);
    if (err_msg == "" && json.isObject()) {
        ip_ = json["ip"].toString();
        user_ = json["user"].toString();
        passwd_ = json["passwd"].toString();
        db_name_ = json["db_name"].toString();
        port_ = json["port"].toNumber();
        max_conn_ = json["max_conn"].toNumber();
        max_idle_time_ = json["max_idle_time"].toNumber();
        return true;
    }
    return false;
}

void ConnectPool::recycle_connection() {
    while (true) {
        std::cout << mysql_conn_que_.size() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> locker(mtx_);
        while (static_cast<int>(mysql_conn_que_.size()) > max_conn_) {
            MySQLConn* conn = mysql_conn_que_.front();
            if (conn->get_alive_time() >= max_idle_time_) {
                mysql_conn_que_.pop();
                delete conn;
            }
            else {
                break;
            }
        }
    }
}

ConnectPool::ConnectPool() {
    if (!parse_json_file()) {
        return;
    }
    for (int i = 0; i != max_conn_; ++i) {
        add_connection();
    }
    std::thread recycler(&ConnectPool::recycle_connection, this);
    recycler.detach();
}

ConnectPool* ConnectPool::get_connect_pool() {
    static ConnectPool pool;
    return &pool;
}

void ConnectPool::add_connection() {
    MySQLConn* conn = new MySQLConn;
    conn->connect(user_, passwd_, db_name_, ip_, port_);
    conn->refresh_alive_time();
    mysql_conn_que_.push(conn);
}

std::shared_ptr<MySQLConn> ConnectPool::get_connection() {
    std::unique_lock<std::mutex> locker(mtx_);
    if (mysql_conn_que_.empty()) {
        std::cout << "tianjiaxiancheng";
        add_connection();
    }
    std::shared_ptr<MySQLConn> conn_ptr(mysql_conn_que_.front(), [this](MySQLConn* conn){
        std::lock_guard<std::mutex> locker(mtx_);
        conn->refresh_alive_time();
        mysql_conn_que_.push(conn);
    });
    mysql_conn_que_.pop();
    return conn_ptr;
}

ConnectPool::~ConnectPool() {
    while (!mysql_conn_que_.empty()) {
        MySQLConn* conn = mysql_conn_que_.front();
        mysql_conn_que_.pop();
        delete conn;
    }
    mysql_library_end(); 
}