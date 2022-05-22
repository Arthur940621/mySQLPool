#include "ConnectPool.h"
#include "../Json/Json.h"
#include <fstream>
#include <thread>

ConnectPool* ConnectPool::get_connect_pool() {
    static ConnectPool pool;
    return &pool;
}

void ConnectPool::add_connection() {
    MySQLConn* conn = new MySQLConn;
    conn->connect(user_, passwd_, db_name_, ip_, port_);
    mysql_conn_que_.push(conn);
}

ConnectPool::ConnectPool() {
    if (!parse_json_file()) {
        return;
    }
    for (int i = 0; i != max_conn_; ++i) {
        add_connection();
    }
}

ConnectPool::~ConnectPool() {
    std::lock_guard<std::mutex> locker(mtx_);
    while (!mysql_conn_que_.empty()) {
        MySQLConn* conn = mysql_conn_que_.front();
        mysql_conn_que_.pop();
        delete conn;
    }
    mysql_library_end(); 
}

std::shared_ptr<MySQLConn> ConnectPool::get_connection() {
    std::unique_lock<std::mutex> locker(mtx_);
     while (mysql_conn_que_.empty()) {
        if (std::cv_status::timeout == cv_.wait_for(locker, std::chrono::milliseconds(time_out_))) {
            if (mysql_conn_que_.empty()) {
                // return nullptr;
                continue;
            }
        }
     }
    std::shared_ptr<MySQLConn> conn_ptr(mysql_conn_que_.front(), [this](MySQLConn* conn){
        std::lock_guard<std::mutex> locker(mtx_);
        mysql_conn_que_.push(conn);
        cv_.notify_all();
    });
    mysql_conn_que_.pop();
    return conn_ptr;
}

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
        time_out_ = json["time_out"].toNumber();
        return true;
    }
    return false;
}