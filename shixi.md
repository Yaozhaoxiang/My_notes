# nccl_test

setlinebuf 函数用于将指定的流（stream）设置为行缓冲模式。在这个特定的调用中，stdout 是标准输出流，它通常对应着控制台输出。因此，setlinebuf(stdout) 的作用是将标准输出流设置为行缓冲模式。

行缓冲模式的特点
+ 缓冲机制：在行缓冲模式下，输出数据会被暂时存储在缓冲区中，直到遇到换行符 '\n'、缓冲区满或者程序正常结束，才会将缓冲区中的数据实际输出到目标设备（如控制台）。
+ 效率优势：行缓冲可以减少实际的 I/O 操作次数，提高程序的效率，因为它将多次小的输出操作合并为一次大的操作。

```cpp
#include <stdio.h>

void setlinebuf(FILE *stream);
```

## 1. 段错误 segmentation fault

当在执行 make run 时出现 “Segmentation fault”（段错误），这通常意味着程序试图访问未被分配给它的内存区域，或者以不被允许的方式访问内存。

1.  空指针解引用

原因：程序尝试对一个空指针进行解引用操作，也就是访问空指针所指向的内存地址，而空指针并不指向任何有效的内存区域。

```cpp
#include <iostream>

int main() {
    int* ptr = nullptr;
    *ptr = 10; // 对空指针解引用，会导致段错误
    return 0;
}
```

+ 解决办法：在使用指针之前，确保指针已经被正确初始化，指向有效的内存地址。可以通过动态内存分配（如 new 运算符）或者将指针指向已存在的对象来实现。

```cpp
#include <iostream>

int main() {
    int num = 0;
    int* ptr = &num;
    *ptr = 10;
    std::cout << *ptr << std::endl;
    return 0;
}
```

2. 数组越界访问

原因：程序试图访问数组中超出其有效索引范围的元素，这可能会导致访问到未分配给该数组的内存区域。

```cpp
#include <iostream>

int main() {
    int arr[5];
    for (int i = 0; i <= 5; ++i) { // 数组越界访问，有效索引为 0 到 4
        arr[i] = i;
    }
    return 0;
}
```

3.  栈溢出

原因：递归调用过深或者在栈上分配了过大的局部变量，导致栈空间耗尽。

```cpp
#include <iostream>

void recursiveFunction() {
    recursiveFunction(); // 无限递归，会导致栈溢出
}

int main() {
    recursiveFunction();
    return 0;
}
```

4. 释放后使用（Use - After - Free）
原因：程序在释放内存后仍然尝试访问该内存区域。

```cpp
#include <iostream>

int main() {
    int* ptr = new int;
    delete ptr; // 释放内存
    *ptr = 10;  // 释放后使用，会导致段错误
    return 0;
}
```
解决办法：在释放内存后，将指针设置为 nullptr，避免后续对该指针的误操作。

## 2. string 问题

在SDC RV上，new 和 delete操作是不允许的。一旦程序中涉及到 new/delete操作，程序便会hang住。在c++中，较短的string的初始化不涉及到new操作，std::ostringstream的初始化也不涉及new操作。但是在如下场景下，new操作会被触发。
+ string 长度超过100
+ 拼接操作 + +=
+ reserve 内存空间
+ ostringstrem中输出了过大的字符串

## 3. O1/O2编译下，变量的内容突然变为0

main函数中有一个宏定义，使用了 #define func(){};
宏定义中的大括号导致其中声明的变量全部变为临时变量。这些临时变量的内存空间在O0时不会被回收，但在O1时会被回收，从而导致内存踩踏的发生。






# 通信组库
v2.23.4.1
## 1. 建立Bootstrap网络

目的：线程或进程通过TCP网络建立一个环，所有线程或进程通过Allgather方式同步信息，主要用于整个初始化过程。AllGather方式，传输的方式通过TCP建立的环。

0. 建环过程
+ 假设有三个GPU，每个GPU对应一个进程，RANK也是唯一标识一个GPU的ID。启动NCCL初始化中会遍历主机中的网卡，对应3个GPU，创建三个进程。通过TCP建立一个环。
+ NCCL创建一个Root线程，这个线程只有一个，Rank0创建；
+ 每个Rank进程都创建两个TCP服务监听端口，端口1为了让其他Rank来凝结到本地，端口2让root连接。
+ root线程建立一个监听端口，等待所有rank连接
+ root通过使用UniqueID建立Socket监听，其他rank通过这个链接root线程。
+ 发送本地rank的服务器监听端口1给root线程进行一个汇总，统一管理，此时Root线程知道了所有的rank的监听端口1的socket；
+ 在root线程中，按照rank顺序建立ring，然后rank0发送各个地址到其他rank上；
+建立一个环后，通过allgather同步所有的rank的ip和port，每个rank就有了全局所有的rank的ip和port，创建的bootstrap网络会报错在ncclComm里面。

1. ncclGetUniqueId

```cpp
  if (myRank == 0) ncclGetUniqueId(&id);
  MPICHECK(MPI_Bcast((void *)&id, sizeof(id), MPI_BYTE, 0, MPI_COMM_WORLD));
```

首先 rank0 调用 ncclGetUniqueId(&id):
+ 首先调用 bootstrapNetInit() : 拿到网络接口地址，存储在 bootstrapNetIfAddr（ip+端口） 中，并将接口名称存储在 bootstrapNetIfName 中

+ 然后调用 bootstrapGetUniqueId(), 把拿到的网络接口地址和一个随机数 生成一个Uniqueid,标识符。这样其他的进程就可以通过这个addr连接到本进程。
    + 调用 bootstrapCreateRoot(uniqueid, false) : 在这里监听 unqueid；
    + 然后创建一个线程 bootstrapRoot() 参数 args：listensocket + uniqueid->magic; 用来处理客户端的连接。 然后使用 pthread_detach 分离线程，使得该线程在结束时资源能自动回收。
    + bootstrapRoot() ： 这里用来接收客户端连接，接收客户端发送的 info，这个结构体包含：rank,nrank,extAddressListenRoot（监听root）,extAddressListen（监听其他）。这两个ext一个是客户端用来监听root的连接，一个用来监听其他进程的连接。收到后就关闭。
        + 接收完后， 在一次连接每个 rank_i，向其发送 next_addr。


通过 MPI_Bcast广播 ： 在 0 进程进行广播，其他进程接收得到 uniqueid;

> IP 地址配置在网络接口上：一个网络接口可以配置一个或多个 IP 地址，这些 IP 地址用于标识该接口在网络中的位置。例如，一台计算机可能有一个以太网接口和一个无线接口，每个接口都可以配置各自的 IP地址。
> 网络接口是 IP 地址的载体：IP 地址依赖于网络接口来实现数据的收发。当计算机发送数据时，操作系统会根据目标 IP 地址选择合适的网络接口进行数据传输；当接收到数据时，网络接口会将数据传递给对应的 IP 地址进行处理。

2. ncclCommInitRank

```cpp
  //initializing NCCL
  NCCLCHECK(ncclCommInitRank(&comm, nRanks, id, myRank));
```

各个rank都将调用 ncclCommInitRank；
+ 首先调用 ncclInit -> bootstrapNetinit() 拿到网卡；然后调用 bootstrapInit(uniqueid, comm) 初始化通信器，在这里实现发送和接收：
    + 首先创建两个socket，一个用来处理root的连接；一个用来处理其他rank的连接；
    + 各个rank把自己的 info（监听地址） 发送给root；
    + 然后root线程发送next_addr给各个rank,其他 rank 接收root发送给自己的 next rank；
    + 各个rank之间进行互连；
    + 然后调用 bootstrapAllGather() -> bootstrapRingAllGather() -> bootstrapNetSendRecv() -> bootstrapNetSendRecv(), 每个进程获取所有的addr；
        + 先发送 data->size;
        + 再发送 data->data->ptr;

运行问题：

我的makefile：
run:
  export LD_LIBRARY_PATH=$(SDC_CCL)/build/lib; \
  mpirun -n 2 -N 1 --hostfile hostfile ./build/hello_sdc
但是make run后：
./build/hello_sdc : error while loading shared libraries : libsscl.so.2 : cannot open shared object file : NO such file or directory
------
prtrrun detected thar one or more processes exited with non-zero status,
thus causing the job to be terminated. the firsr process to do so was:
但是当我修改为：
run:
  export LD_LIBRARY_PATH=$(SDC_CCL)/build/lib; \
  mpirun -n 2  ./build/hello_sdc
又可以正常运行了

+ 环境变量传递问题：在 Makefile 里执行 export LD_LIBRARY_PATH=$(SDC_CCL)/build/lib，仅仅是在当前的 shell 进程里设置了该环境变量。当使用 --hostfile 时，mpirun 会在指定的多个计算节点上启动进程，而这些节点可能没有正确继承这个环境变量。
+ 节点环境差异：不同的计算节点可能有着不同的环境配置，要是共享库路径没有在所有节点上都正确设置，就会出现找不到共享库的错误。

mpirun 提供了 -x 选项，能够将本地环境变量传递到所有的计算节点上。你可以按照下面的方式修改 Makefile：

> mpirun -n 2 -N 1 --hostfile hostfile -x LD_LIBRARY_PATH=$(SDC_CCL)/build/lib ./build/hello_sdc


git常用命令：
git cherry-pick commit-id : 将一条commit合并到当前分支；
git stash : 暂存修改代码
git stash pop : 恢复暂存代码











