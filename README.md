
# 什么是数据库连接池？

池是一组资源的集合，这组资源在服务器启动之初就被完全创建好并初始化。池是资源的容器，本质上是对资源的复用。

数据库连接池连接池中的资源为一组数据库连接，由程序动态地对池中的连接进行使用，释放。

当系统开始处理客户请求的时候，如果它需要相关的资源，可以直接从池中获取，无需动态分配。当服务器处理完一个客户连接后,可以把相关的资源放回池中，无需执行系统调用释放资源。

# 为什么要创建连接池？

若系统需要频繁访问数据库，则需要频繁创建和断开数据库连接，而创建数据库连接是一个很耗时的操作，也容易对数据库造成安全隐患。

在程序初始化的时候，集中创建多个数据库连接，并把他们集中管理，供程序使用，可以保证较快的数据库读写速度，更加安全可靠。

# 数据库连接池

项目中 [MySQLConn](./MySQLPool/MySQLConn.h) 是对 `MySQLConn` 的封装，使用 [config](./config.json) 进行配置，对 `json` 的解析用到了之前实现的 [Json](https://github.com/Arthur940621/myJson) 解析器。数据库连接池 [ConnectPool](./MySQLPool/ConnectPool.h) 使用单例模式，使用队列保存创建的 [MySQLConn](./MySQLPool/MySQLConn.h) 连接，实现对数据库连接资源的复用。

# 使用方法

```cpp
#include "MySQLPool/ConnectPool.h" // 包含头文件

ConnectPool* pool = ConnectPool::get_connect_pool(); // 初始化连接池

std::shared_ptr<MySQLConn> conn = pool->get_connection(); // 从连接池中取出一个连接

string sql = "insert into user_tb values(null, 'xxx', 'xxx')";
conn->update(sql); // 对连接进行操作
```