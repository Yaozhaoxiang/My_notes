# gdb调试

一、什么是GDB
> GDB 是由 GUN 软件系统社区提供的调试工具，同 GCC 配套组成了一套完整的开发环境，GDB 是 Linux 和许多 类Unix系统的标准开发环境。

二、gdb在那个开发版本（debug/release）中进行应用的
debug:调试版本
release：发布版本
`g++ main.cpp Add.cpp -o main -g`
⚡ ：程序的发布方式有两种，debug模式和release模式
⚡ ：Linux gcc/g++出来的二进制程序，默认是release模式
⚡ ：要使用gdb调试，必须在源代码生成二进制程序的时候, 加上`-g`选项
如果gdb ./main 出现：`No debuffing symbols`，就是没有加上`-g`生成调试信息；

三、指令
`set args`:设置主程序的参数；当参数有特殊字符时，可以用双引号括住；比如文件名中有空格"sf afasf"
```sh
注：（）括号里面是该指令的全称 
💜  l(list) 行号/函数名 —— 显示对应的code，每次10行

💜r(run) —— F5【无断点直接运行、有断点从第一个断点处开始运行】

💜b(breakpoint) + 行号 —— 在那一行打断点

💜b 源文件：函数名 —— 在该函数的第一行打上断点

💜b 源文件：行号 —— 在该源文件中的这行加上一个断点吧

💜info b —— 查看断点的信息
breakpoint already hit 1 time【此断点被命中一次】
💜d(delete) + 当前要删除断点的编号 —— 删除一个断点【不可以d + 行号】

若当前没有跳出过gdb，则断点的编号会持续累加
💜d + breakpoints —— 删除所有的断点

💜disable b(breakpoints) —— 使所有断点无效【默认缺省】

💜enable b(breakpoints) —— 使所有断点有效【默认缺省】

💜disable b(breakpoint) + 编号 —— 使一个断点无效【禁用断点】

💜enable b(breakpoint) + 编号 —— 使一个断点有效【开启断点】

相当于VS中的空断点
💜enable breakpount —— 使一个断点有效【开启断电】

💜n(next) —— 逐过程【相当于F10，为了查找是哪个函数出错了】

💜s(step) —— 逐语句【相当于F11，】

💜bt —— 看到底层函数调用的过程【函数压栈】

💜set var —— 修改变量的值

💜p(print) 变量名 —— 打印变量值

💜ptype p ---查看变量p的类型
 p /t val:输入二进制
 p /x val:输出十六进制
💜print *p --查看指针变量所指向的值

💜display —— 跟踪查看一个变量，每次停下来都显示它的值【变量/结构体…】

💜undisplay + 变量名编号 —— 取消对先前设置的那些变量的跟踪

排查问题三剑客🗡
💜until + 行号 —— 进行指定位置跳转，执行完区间代码

💜finish —— 在一个函数内部，执行到当前函数返回，然后停下来等待命令

💜c(continue) —— 从一个断点处，直接运行至下一个断点处【VS下不断按F5】
————————————————

                            版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
                        
原文链接：https://blog.csdn.net/weixin_45031801/article/details/134399664
```


## 4. 调试core文件
   程序挂掉时，系统不会生成core文件
     1. ulimit -a 查看系统参数
     2. ulimit -c unlimit 把core文件的大小设置为无限制
     3. 运行程序，生成core文件
     4. gdb 程序名 core文件名
 
## 5. 调试进程

   如果一个进程通过 fork 系统调用创建子进程，gdb会继续调试原来的进程，子进程正常进行；


   调试父进程： set follow-fork-mode parent
   调试子进程: set follow-fork-mode child

   先把调试模式跳到off，才可以查看进程
   设置调试模式：set detach-on-fork on/off
   表示调试当前进程的时候，其他的进程继续运行，如果用off，调试当前进程的时候，其他的进程被gdb挂起

   查看调试进程：info inferiors
   切换当前调试的进程：inferior 进程id

## 6. gdb调试多进程
gdb有一组命令课辅助多线程程序的调试

+ info threads 显示当前可调试的所有线程。 gdb会为每个线程分配一个 ID，我们可以使用这个ID来操作对应的线程。ID前面有 * 号的线程是当前被调试的线程 

+ thread ID 调试目标ID指定的线程

+ set scheduler-locking [off/on/step]: 调试多线程程序时，默认除了被调试的线程在执行外，其他线程也在继续执行，但有的时候我们希望只让被调试的线程运行。这可以通过这个命令来实现。该命令设置scheduler-locking的值：off表示不锁定任何线程，即所有线程都可以继续执行，这是默认值；on表示只有当前被调试的线程会继续执行；step表示在单步执行的时候，只有当前线程会执行。

## 6. 在xv6中调试
   
   在 64 位(x86) 的电脑上模拟 RISC-V 架构的 CPU。在计组实验中，通过硬件电路设计 CPU，而 qemu 通过 C 语言来模拟 CPU 的取指、译码和执行操作。
       qemu是一种开源的虚拟化和仿真软件工具。它可以执行和模拟一台完整的计算机，包括其处理器、内存、硬盘、网络接口等硬件组件

   在 xv6-labs-2020 目录下输入 make qemu 并回车，随后出现单独窗口。qemu 的虚拟 BIOS 将从 xv6.img 文件中含有的虚拟硬盘映像加载引导文件，随后启动 xv6 内核程序。


   1. 开启gdb状态下的qemu
      执行下列指令可以在 qemu 下运行 xv6 程序，同时支持远程调试
      ` make CPUS=1 qemu-gdb`
      此时虚拟机启动，并在执行第一条指令之前停止运行，等待远程 gdb 的连接
   2. 开启gdb
      接着使 gdb 连接到 qemu 的 stub。打开新的终端窗口，切换至相同的 xv6 目录，输入
      `riscv64-unknown-elf-gdb kernel/kernel`
      注意此时虽然执行了 gdb xxx，但是并没有开始调试程序。因为此时 gdb 为待执行的程序提供的是用户模式下的 Linux 进程，而内核程序 kernel 希望在原生的 x86 硬件环境中以特权模式运行。
      虽然内核程序不在此时的 gdb 环境下运行，但 gdb 仍需要读入此时内核的 ELF 程序映像，使得 gdb 可以提取到调试过程中所需信息，比如 C 函数的地址、符号、以及源码的行号。
   3. 使用 gdb 远程连接 qemu
      `target remote localhost:26000`
      target remote 127.0.0.1:26000
    4. 在用户态调试，需要跳转文件： `file user/_call`
    5. tui enable
        layout asm
        layout reg
        layout split
        layout src
   4.  x/i 用于查看当前指令地址处的汇编指令    

   4. 开始调试
      此时我们在 exec 函数部分设置断点，接着运行虚拟机，直到虚拟机运行至断点处停止
      ```sh
        (gdb) b exec
        Breakpoint 1 at 0x80004b0c: file kernel/exec.c, line 14.
        (gdb) c
        Continuing.
        [Switching to Thread 1.2]

        Thread 2 hit Breakpoint 1, exec (path=path@entry=0x3fffffdf00 "/init", 
            argv=argv@entry=0x3fffffde00) at kernel/exec.c:14
        14      {
        (gdb) 
      ```
      于此同时，第一次开启的终端 qemu-gdb 界面显示
      ```sh
        xv6 kernel is booting

        hart 2 starting
        hart 1 starting
      ```
      此时 xv6 内核进行初始化，并开始载入并执行它的第一个用户模式进程，/init 程序，故执行到 exec 处，触发中断。

      若此时接着运行，输入 c 并回车

      ```sh
        (gdb) c
        Continuing.
        [Switching to Thread 1.1]

        Thread 1 hit Breakpoint 1, exec (path=path@entry=0x3fffffbf00 "sh", 
            argv=argv@entry=0x3fffffbe00) at kernel/exec.c:14
        14      {
        (gdb) 
      ```
      在第一次开启的中断 qemu-gdb 界面显示
      ```sh
        init: starting sh
      ```
      在此次运行中，xv6 内核继续执行，直至使用 exec 调用 sh，现在如果继续输入 c 并回车，会发现 gdb 调试界面持续显示 continuing，因为此时 shell 程序已经启动，在等待用户输入交互指令。
      ```sh
        (gdb) cContinuing.
      ```
        此时，如果在 qemu 界面输入 cat README，则在 gdb 界面再次中断
        ```sh
        (gdb) cContinuing.Thread 1 hit Breakpoint 1, exec (path=path@entry=0x3fffff9f00     "cat",     argv=argv@entry=0x3fffff9e00) at kernel/exec.c:1414      {(gdb) 
        ```
      说明此时 shell 调用 exec 函数执行 cat 指令，执行到 exec 时产生中断。如果在 gdb 界面输入如下指令
      ```sh
      (gdb) p argv[0]$1 = 0x87f49000 "cat"(gdb) p argv[1]$2 = 0x87f48000 "README"(gdb)   p argv[2]$3 = 0x0(gdb) 
      ```
      可以发现 gdb 的 p 指令打印出了 exec 函数入口中的 argv[] 参数值，符合我们在 qemu 中的输入指令。




       











