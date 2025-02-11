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

## 1. 建立Bootstrap网络

目的：线程或进程通过TCP网络建立一个环，所有线程或进程通过Allgather方式同步信息，主要用于整个初始化过程。AllGather方式，传输的方式通过TCP建立的环。

1. 建环过程
+ 假设有三个GPU，每个GPU对应一个进程，RANK也是唯一标识一个GPU的ID。启动NCCL初始化中会遍历主机中的网卡，对应3个GPU，创建三个进程。通过TCP建立一个环。
+ NCCL创建一个Root线程，这个线程只有一个，Rank0创建；
+ 每个Rank进程都创建两个TCP服务监听端口，端口1为了让其他Rank来凝结到本地，端口2让root连接。
+ root线程建立一个监听端口，等待所有rank连接
+ root通过使用UniqueID建立Socket监听，其他rank通过这个链接root线程。
+ 











