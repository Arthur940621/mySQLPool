#include <iostream>
#include <thread>
#include "MySQLPool/ConnectPool.h"

using namespace std;
using namespace chrono;

void op1(int begin, int end) {
    for (int i = begin; i != end; ++i) {
        MySQLConn conn;
        conn.connect("root", "xxx", "testdb", "localhost", 3306);
        string sql = "insert into user_tb values(null, 'xxx', 'xxx')";
        conn.update(sql);
    }
}

void op2(ConnectPool* pool, int begin, int end) {
    for (int i = begin; i != end; ++i) {
        std::shared_ptr<MySQLConn> conn = pool->get_connection();
        string sql = "insert into user_tb values(null, 'xxx', 'xxx')";
        conn->update(sql);
    }
}

void test1() {
#if 0 // 34000
    steady_clock::time_point begin = steady_clock::now();
    op1(0, 5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "It takes " << (length.count() / 1000000) << " ms for a single thread not to use the connection pool" << endl;
#else // 37329
    ConnectPool* pool = ConnectPool::get_connect_pool();
    steady_clock::time_point begin = steady_clock::now();
    op2(pool, 0, 5000);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "It takes " << (length.count() / 1000000) << " ms for a single thread to use the connection pool" << endl;
#endif
}

void test2() {
#if 0 // 10442
    steady_clock::time_point begin = steady_clock::now();
    thread t1(op1, 0, 1000);
    thread t2(op1, 1000, 2000);
    thread t3(op1, 2000, 3000);
    thread t4(op1, 3000, 4000);
    thread t5(op1, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "It takes " << (length.count() / 1000000) << " ms for a multiple thread not to use the connection pool" << endl;

#else // 9794
    ConnectPool* pool = ConnectPool::get_connect_pool();
    steady_clock::time_point begin = steady_clock::now();
    thread t1(op2, pool, 0, 1000);
    thread t2(op2, pool, 1000, 2000);
    thread t3(op2, pool, 2000, 3000);
    thread t4(op2, pool, 3000, 4000);
    thread t5(op2, pool, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "It takes " << (length.count() / 1000000) << " ms for a multiple thread to use the connection pool" << endl;
#endif
}

int main() {
    test2();
    return 0;
}