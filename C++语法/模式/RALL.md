# RALL 模式
RAII（Resource Acquisition Is Initialization 资源获取即初始化）类是一种设计模式，用于在 C++ 中管理资源。使用局部对象来管理资源的技术称为资源获取即初始化；这里的资源主要是指操作系统中有限的东西如内存、网络套接字等等，局部对象是指存储在栈的对象，它的生命周期是由操作系统来管理的，无需人工介入；


## RAII的原理
RAII，它充分的利用了C++语言局部对象自动销毁的特性来控制资源的生命周期。

局部对象的自动销毁的特性：声明一个局部对象的时候，会自动调用构造函数进行对象的初始化，当整个函数执行完成后，自动调用析构函数来销毁对象，整个过程无需人工介入，由操作系统自动完成；于是，很自然联想到，当我们在使用资源的时候，在构造函数中进行初始化，在析构函数中进行销毁。整个RAII过程总结四个步骤：
  a.设计一个类封装资源
  
  b.在构造函数中初始化
  
  c.在析构函数中执行销毁操作
  
  d.使用时声明一个该对象的类

RAII 类的主要特点
1. 资源获取：
  在对象构造时获取所需的资源。
  例如，打开文件、获取互斥锁、分配内存等。
2. 资源释放：
  在对象析构时释放之前获取的资源。
  例如，关闭文件、释放互斥锁、释放内存等。
3. 对象生命周期：
  对象的生命周期决定了资源的生命周期。
  当对象不再使用时，析构函数会被自动调用，从而释放资源。
4. 自动管理：
  利用 C++ 的对象生命周期自动管理资源，无需显式调用释放资源的函数。

## RALL应用
比如封装一下POSIX标准的互斥锁：
```cpp
#include <pthread.h>
#include <cstdlib>
#include <stdio.h>

class Mutex {
 public:
  Mutex();
  ~Mutex();

  void Lock();
  void Unlock(); 

 private:
  pthread_mutex_t mu_;

  // No copying
  Mutex(const Mutex&);
  void operator=(const Mutex&);
};


#include "mutex.h"

static void PthreadCall(const char* label, int result) {
  if (result != 0) {
    fprintf(stderr, "pthread %s: %s\n", label, strerror(result));
  }
}

Mutex::Mutex() { PthreadCall("init mutex", pthread_mutex_init(&mu_, NULL)); }

Mutex::~Mutex() { PthreadCall("destroy mutex", pthread_mutex_destroy(&mu_)); }

void Mutex::Lock() { PthreadCall("lock", pthread_mutex_lock(&mu_)); }

void Mutex::Unlock() { PthreadCall("unlock", pthread_mutex_unlock(&mu_)); }
```
写到这里其实就可以使用Mutex来锁定临界区，但我们发现Mutex只是用来对锁的初始化和销毁，我们还得在代码中调用Lock和Unlock函数，这又是一个对立操作，所以我们可以继续使用RAII进行封装，代码如下：
```cpp
#include "mutex.h"

class  MutexLock {
 public:
  explicit MutexLock(Mutex *mu)
      : mu_(mu)  {
    this->mu_->Lock();
  }
  ~MutexLock() { this->mu_->Unlock(); }

 private:
  Mutex *const mu_;
  // No copying allowed
  MutexLock(const MutexLock&);
  void operator=(const MutexLock&);
};
```
到这里我们就真正封装了互斥锁，下面我们来通过一个简单的例子来使用它，代码如下：
```cpp
#include "mutexlock.hpp"
#include <unistd.h>
#include <iostream>

#define    NUM_THREADS     10000

int num=0;
Mutex mutex;

void *count(void *args) {
    MutexLock lock(&mutex);
    num++;
}


int main() {
    int t;
    pthread_t thread[NUM_THREADS];

    for( t = 0; t < NUM_THREADS; t++) {   
        int ret = pthread_create(&thread[t], NULL, count, NULL);
        if(ret) {   
            return -1;
        }   
    }

    for( t = 0; t < NUM_THREADS; t++)
        pthread_join(thread[t], NULL);
    std::cout << num << std::endl;
    return 0;
}
```
比如这里刚开始定义Mutex，构造函数负责初始化，析构负责销毁，那么在一个函数中创建一个Mutex对象后，在使用完后，由于是局部对象，函数执行完后这个对象将自动销毁，从而销毁pthread_mutex_t；

在MutexLock中封装了 Mutex 的上锁 和 释放锁的过程；那么在函数中，比如这里的count()函数，进入这个函数时，则构造 MutexLock对象，调用构造函数，这个时候上锁，那么这个函数就相当于一个大锁，函数内部全部加锁，在这个函数执行完后，再释放锁。

所以说RAII机制是一种对资源申请、释放这种成对的操作的封装 通过这种方式实现在局部作用域内申请资源然后销毁资源。RALL机制便是通过利用对象的自动销毁，使得资源也具有了生命周期，有了自动销毁（自动回收）的功能。

## 在 webserve中 连接池中使用
```cpp
/* 连接池 */
class connection_pool
{
public:
	MYSQL *GetConnection();				 //获取数据库连接
	bool ReleaseConnection(MYSQL *conn); //释放连接
	int GetFreeConn();					 //获取连接
	void DestroyPool();					 //销毁所有连接

	//单例模式，这是一个常见的设计模式，确保全局只会有一个连接池实例
	static connection_pool *GetInstance();

	void init(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn, int close_log); 

private:
	connection_pool();
	~connection_pool();

	int m_MaxConn;  //最大连接数
	int m_CurConn;  //当前已使用的连接数
	int m_FreeConn; //当前空闲的连接数
	locker lock;
	list<MYSQL *> connList; //连接池
	sem reserve;

public:
	string m_url;			 //主机地址
	string m_Port;		 //数据库端口号
	string m_User;		 //登陆数据库用户名
	string m_PassWord;	 //登陆数据库密码
	string m_DatabaseName; //使用数据库名
	int m_close_log;	//日志开关
};

class connectionRAII{

public:
	connectionRAII(MYSQL **con, connection_pool *connPool);
	~connectionRAII();
	
private:
	MYSQL *conRAII; /* 用于存储从连接池获取的 MySQL 连接。 */
	connection_pool *poolRAII; /* 用于存储指向连接池的指针，在析构时将连接归还到该连接池。 */
};
```

connectionRAII 类就是一个很好的 RAII 类的例子。从线程池中拿去连接和释放连接是一对操作，所以封装成connectionRAII 类，它负责管理数据库连接，确保连接在使用完毕后能够被正确释放。

使用 connectionRAII 类，connectionRAII构造函数是从线程池中获取一个 MYSQL 链接，保存到 conRAII 中；析构函数是 释放连接返回到连接池中。每次使用完后不用显示释放。

下面是如何使用 connectionRAII 类来自动管理数据库连接的示例：
```cpp
#include <iostream>
#include <thread>

void testDatabaseOperation(connection_pool *connPool) {
    //先从连接池中取一个连接
    MYSQL *mysql = NULL;
    connectionRAII mysqlcon(&mysql, connPool);

    //在user表中检索username，passwd数据，浏览器端输入
    if (mysql_query(mysql, "SELECT username,passwd FROM user"))
    {
        LOG_ERROR("SELECT error:%s\n", mysql_error(mysql));
    }

    // 使用连接执行数据库操作
    // ...
    // 无需显式释放连接，connRAII 的析构函数会在作用域结束时自动释放连接
}

int main() {
    // 初始化连接池
    connection_pool *pool = connection_pool::GetInstance();
    pool->init("localhost", "root", "password", "database", 3306, 10, 0);

    // 创建多个线程来测试连接池
    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(testDatabaseOperation, pool);
    }

    // 等待所有线程结束
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
```









参考链接：
c++经验之谈一：RAII原理介绍 - allen的文章 - 知乎
https://zhuanlan.zhihu.com/p/34660259


