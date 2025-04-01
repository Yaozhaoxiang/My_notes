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

```cpp
static inline void ringRS(ScclKernelArgs* args){
    if(args->size & args->comm.nrank!=0){
        return;
    }
    const int nrank = args->com.nranks;
    const ScclRing * ring = &args->channel.ring;

    PrimsHandle prims = {
        args->comm.rank,
        args->src,
        args->dst,
        args->dtypr,
        1,
        1,
        &args->channel.conns[ring->prev],
        &args->channel.conns[ring->next],
        SCCL_SLICE_SIZE,
        args->sram,
        args->sram_size,
        false,false,false,false
    };
    LOGI("prims.send->buffer->tensor_ptr address %s, ring->next is %d",FMT_ADDR(prims.send->buffer->tensor_ptr),ring->next);

    size_t part_size = args->size;
    size_t part;
    size_t src_offset;
    size_t dst_offset;

    LOGI("prims.send->buffer->tensor_ptr address %s, ring->next is %d",FMT_ADDR(prims.send->buffer->tensor_ptr),ring->next);

    for(size_t chunk_base = 0;chunk_base < part_size; chunk_base += SCCL_CHUNK_SIZE){
        size_t chunk_size = MIN(part_size - chunk_base,SCCL_CHUNK_SIZE);

        part = ring->rankmap[nranks-1];
        src_offset = chunk_base+part*part_size;
        dst_offset = chunk_base;
        LOGI("prims.send->buffer->tensor_ptr address %s, ring->next is %d",FMT_ADDR(prims.send->buffer->tensor_ptr),ring->next);
        
        pSend(&prims, src_offset,dst_offset,chunk_size);
        for(itn i=2;i<nranks;i++){
            part=ring->rankmap[nranks-i];
            src_offset=chunk_base+part*part_size;
            dst_offset=chunk_base;
            pRecvEeduceSend(&prims,src_offset,dst_offset,chunk_size);
        }
        part=ring->rankmap[0];
        src_offset=chunk_base+part*part_size;
        dst_offset=chunk_base;
        pRecvReduceCopy(&prims,src_offset,dst_offset,chunk_size);
    }
}
```

其中的FMT_ADDR是一个宏函数，函数内存在静态数组与C++对象：

```cpp
template<typrname T,int FMT_LENGTH=FMT_LENGTH>
static const ccl::string<FMT_LENGTH> _fmtSdcAddr(const T*addr){
    uint64_t uint_addr = (uint64_t) addr;

    uint64_t die_id = (uint_addr>>38) & 0x3;
    uint64_t tile_id = (uint_addr>>40) & 0xf;

    char buffer[FMT_LENGTH];
    sprintf(
        buffer, "0x%02x\'%04x\'%04x (die #%d, %s, %s)",
        ((uint_addr>>32)&0xFF),
        ((uint_addr>>16)&0XFFFF),
        ((uint_addr)&0xFFFF),
        die_id,
        getTileName(tile_id).c_str(),
        getPhyOffsetStr(uint_addr).c_str()
    );
    const ccl::string<FMT_LENGTH> fmt_str = buffer;
    LOGT("buffer addr %p",buffer);
    LOGT("fmt_str addr %p",&fmt_str);
    return fmt_str
}
#defint FMT_ADDR(addr) (__fmtSdcAddr(addr).c_str())
```

其中三个LOGI语句，他们的内容完全一样，均为打印 prims.send->buffer->tensor_ptr 的地址，这一地址经过 FMT_ADDR 的调用。在 O0编译选项下，输出结果一样：

```cpp
[ring RS]:prims.send->buffer->tensor_ptr address 0x04'1000'0000 (die #0, compute tile #0, ??? B), ring->next is 2
[ring RS]:prims.send->buffer->tensor_ptr address 0x04'1000'0000 (die #0, compute tile #0, ??? B), ring->next is 2
[ring RS]:prims.send->buffer->tensor_ptr address 0x04'1000'0000 (die #0, compute tile #0, ??? B), ring->next is 2
```

但在O1/O2编译下，输出是这样的：
```cpp
[ring RS]:prims.send->buffer->tensor_ptr address 0x04'1000'0000 (die #0, compute tile #0, ??? B), ring->next is 2
[ring RS]:prims.send->buffer->tensor_ptr address 0x04'0000'0000 (die #0, tile local, -256MB), ring->next is 2
[ring RS]:prims.send->buffer->tensor_ptr address 0x04'0000'0000 (die #0, tile local, -256MB), ring->next is 2
```

并且tensor_ptr被修改的时刻与 FMT_ADDR的时候有关。此处程序一旦出现第二次FMT_ADDR,那么tensor_ptr的取值必定被修改。

打印 O0与 O1的变量地址空间，结果分别为：

O0:
```CPP
[rank 0] prims.send->buffer->tensor_ptr address 0x04'1000'0000 (die #0, compute tile #0, ??? B), ring->next is 2
[rank 0] buffer addr 00000000FFFF470
[rnak 0] fmt_str addr 00000000FFFF850
[rank 0] &prims.send->buffer->tensor_ptr address 0x00'0fff'fd18 (die #0, title local, -0.73KB),ring->next is 2
```

O1:
```CPP
[rank 0] prims.send->buffer->tensor_ptr address 0x04'1020'0000 (die #0, compute tile #0, ??? B), ring->next is 3
[rank 0] buffer addr 00000000FFFF968
[rnak 0] fmt_str addr 00000000FFFFCB8
[rank 0] &prims.send->buffer->tensor_ptr address 0x00'0fff'fd30 (die #0, title local, -0.70KB),ring->next is 3
```
```CPP
[rank 0] prims.send->buffer->tensor_ptr address 0x04'1030'0000 (die #0, compute tile #0, ??? B), ring->next is 1
[rank 0] buffer addr 00000000FFFF960
[rnak 0] fmt_str addr 00000000FFFFCB8
[rank 0] &prims.send->buffer->tensor_ptr address 0x00'0fff'fce0 (die #0, title local, -0.78KB),ring->next is 1
```

其中fmt_str包含的长度为200的char[]变量，但物理地址距离 &tensor_ptr 明显小于200，导致内存践踏，&tensor_ptr 存储的值被覆写。

经过逐行排查代码，发现在main函数中的一个宏定义中，使用下面的语句：

```cpp
#define INIT_KERNEL_ARGS(a,b,nrank,d,e,,g,h) {
    ScclConn _conns[nranks];
    args.channel.conns = _conns;
    TensorDescriptor t[nranks];
    for(int i=0;i<nranks;i++){
        _conns[i].buffer = &t[i];
    }
    int _rankmap[nranks];
    args.channel.ring.rankmap = _rankmap;
    _init_kernel_args(a,b,nranks,d,e,f,g,h);
}

```

宏定义中的大括号导致其中声明的变量全部为临时变量。这些临时变量的内存空间在 O0 时不会被回收，但在 O1 时会被回收，从而导致了内存踩踏的发送。


### o0 o1 o2是什么？区别是什么

O0、O1、O2 是编译器的优化等级选项

含义
+ O0：表示不做任何优化，是编译器默认的编译选项。编译器会按照代码编写的原样进行编译，生成的目标代码与源代码几乎是一一对应的关系，便于开发者进行调试。
+ O1：对程序做部分编译优化，编译器会尝试减小生成代码的尺寸，以及缩短执行时间，但并不执行需要占用大量编译时间的优化。
+ O2：是比 O1 更高级的优化选项，会进行更多的优化。Gcc 将执行几乎所有的不包含时间和空间折中的优化，能使程序的编译效率大大提升，从而减少程序的运行时间，达到优化的效果。


区别
+ 优化程度
    + O0：不进行优化，保留所有代码细节和原始结构。
    + O1：进行有限的优化，如删除死代码、消除未使用的变量等。
    + O2：在 O1 基础上，还会采用如循环展开、程序内联、常量折叠、公共子表达式消除等更多优化措施，优化更为全面和深入。

+ 编译时间
    + O0：编译时间最短，因为不需要进行任何优化处理。
    + O1：编译时间比 O0 略长，由于要进行部分优化，需要额外的时间和计算资源。
    + O2：编译时间比 O1 更长，由于执行了更多复杂的优化操作，需要更多的时间来分析和处理代码

+ 目标代码体积
    + O0：生成的目标代码体积通常较大，因为包含了所有原始代码信息。
    + O1：相比 O0，目标代码体积有所减小，通过删除死代码等优化手段，减少了不必要的代码。
    + O2：目标代码体积可能比 O1 更小，也可能更大，一些优化措施如内联函数可能会使代码体积增加，但常量折叠等操作又会减小代码体积。

+ 运行效率
    + O0：运行效率相对较低，因为没有经过优化，程序按照原始代码逻辑执行。
    + O1：运行效率比 O0 有一定提升，通过一些简单优化提高了执行速度。
    + O2：运行效率通常比 O1 更高，经过更多优化后，程序的执行路径更高效，能充分利用硬件资源等。

使用：
```cpp
gcc -O0 -o test_O0 test.c
```

比较不同优化选项的编译时间和运行效果
你可以使用time命令来比较不同优化选项下的编译时间，例如：
```cpp
time gcc -O0 -o test_O0 test.c
time gcc -O1 -o test_O1 test.c
time gcc -O2 -o test_O2 test.c
```


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











