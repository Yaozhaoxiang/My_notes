## RISC-V assembly
小端模式和大端模式：
>在小端（Little Endian）模式下，内存中存储数据的顺序与数字的自然顺序是相反的。换句话说，较低的字节地址存储数据的最低有效字节（LEB），较高的字节地址存储数据的最高有效字节（MEB）。
对于数值 0x00646c72，其字节顺序为：
0x00(最高位)
0x64
0x6c
0x72（最低为）
    
>在大端模式下，数据的最高有效字节存储在内存的最低地址。在这种情况下，整数 0x00646c72 将按以下顺序存储：0x00 0x64 0x6c 0x72输出的将是从最低地址到最高地址的数据

### RISC-V assembly
1. Which registers contain arguments to functions? For example, which register holds 13 in main's call to printf?
这个再lab2的时候，已经知道了：系统调用的前 8 个参数通过寄存器 a0 到 a7 传递，超过8个参数：如果系统调用需要超过8个参数，额外的参数将被压入栈中，内核在处理系统调用时会从栈中读取这些参数。
1. Where is the call to function f in the assembly code for main? Where is the call to g? (Hint: the compiler may inline functions.)
    没有这样的代码。 g(x) 被内链到 f(x) 中，然后 f(x) 又被进一步内链到 main() 中
2. At what address is the function printf located?
答：0x630
1. What value is in the register ra just after the jalr to printf in main?
答：0x38
1. What is the output? Here's an ASCII table that maps bytes to characters.
The output depends on that fact that the RISC-V is little-endian. If the RISC-V were instead big-endian what would you set i to in order to yield the same output? Would you need to change 57616 to a different value?
答：HE110 World
大端：0x72626400
不需要，57616 的十六进制是 110，无论端序（十六进制和内存中的表示不是同个概念）
1. In the following code, what is going to be printed after 'y='? (note: the answer is not a specific value.) Why does this happen?
答：x=3 y=5221
输出的是一个受调用前的代码影响的“随机”的值。因为 printf 尝试读的参数数量比提供的参数数量多。
第二个参数 `3` 通过 a1 传递，而第三个参数对应的寄存器 a2 在调用前不会被设置为任何具体的值，而是会
包含调用发生前的任何已经在里面的值。

## Backtrace
对于调试来说，反向跟踪通常是有用的：在错误发生点上方的堆栈上的函数调用列表。

在kernel/printf.c中实现一个backtrace（）函数。在sys_sleep中插入对该函数的调用，然后运行调用sys_sleep的bttest。您的输出应该如下所示：

编译器在每个堆栈帧中放入一个帧指针，该帧指针保存调用者的帧指针的地址。您的反向跟踪应该使用这些帧指针来遍历堆栈，并在每个堆栈帧中打印保存的返回地址。


### 1.关于`uint64 sys_sleep(void)`这个函数
调用`sleep`时，会传递一个参数n，比如`sleep(1)`,即`n=1`
使用`ticks`这个全局变量，来记录时间，通常在操作系统内核中，ticks 用于记录系统运行的时钟滴答数（通常是时钟中断发生的次数），它会随着时间的推移而增加。
```cpp
uint64
sys_sleep(void)
{
int n;
uint ticks0;
// 读取系统调用的第一个参数，存储到变量 n 中
if(argint(0, &n) < 0)
    return -1;
// 获取锁 tickslock，用于保护全局变量 ticks
acquire(&tickslock);
// 记录当前的 ticks 值
ticks0 = ticks;
// 当 ticks 变量的值与 ticks0 的差值小于 n 时，继续循环
while(ticks - ticks0 < n){
    // 检查当前进程是否被杀死，如果是，释放锁并返回 -1
    if(myproc()->killed){
    release(&tickslock);
    return -1;
    }
    // 将当前进程睡眠，并释放锁，直到 ticks 变量被改变
    sleep(&ticks, &tickslock);
}
// 释放锁 tickslock
release(&tickslock);
return 0;
}
```
`sleep(&ticks, &tickslock)`, 是一个让当前进程进入睡眠状态的系统调用，并在特定条件下被唤醒。
`&ticks`
这是一个指向等待条件的指针，表示进程正在等待 `ticks` 变量的变化
在这个例子中，进程希望在 `ticks` 变量达到一定值后被唤醒。
`&tickslock`
这是一个指向自旋锁的指针，表示当前持有的锁。
在进入睡眠之前，进程持有 tickslock 锁，以确保在检查 ticks 变量和进入睡眠之间没有竞争条件。
sleep 函数会先释放 tickslock 锁，然后将当前进程的状态设置为睡眠，并加入到等待队列中，等待 ticks 变量的变化。
当条件满足时，进程会被唤醒，并重新获得 tickslock 锁

所以可以把`backtrace()`,放到`release(&tickslock);`之前
### 2. 栈分布
![栈分布图](.\图片\函数调用栈分布.png)
这张图就是函数调用时栈的分布；
每一块都是一个栈帧，由函数调用生成，栈是从高地址到低地址；
由于函数帧包括寄存器、局部变量等可变原因，所以就使得栈帧不一定相等；
但是，返回地址总是在第一位，其次是上一个栈帧的位置；
两个重要的指针：`sp`栈顶，`fp`指向当前帧的顶部；
所以，返回地址和上一栈帧的位置是相对固定的位置，位于`fp`下面
Note that the return address lives at a fixed offset (-8) from the frame pointer of a stackframe, and that the saved frame pointer lives at fixed offset (-16) from the frame pointer.   
栈的这部分是使用汇编语言实现的。

### 3. `backtrace`实现
   ```cpp
    void
    backtrace(void)
    {
        printf("backtrace:\n");
        uint64 fp = r_fp(); //获得当前fp
        uint64 top = PGROUNDUP(fp);//底部
        uint64 return_address;
        while(fp!=top)
        {
            return_address = *((uint64*)(fp - 8)); //返回地址，输出
            fp = *((uint64*)(fp - 16)); //上一个fp位置
            printf("%p\n",return_address);
        }
    }
   ```

## Alarm
在这个练习中，你将为 xv6 添加一个功能，该功能会在进程**使用 CPU 时间时定期发出警报**。这可能对希望限制占用多少 CPU 时间的计算密集型进程有用，或者对既想进行计算又想定期执行某些操作的进程有用。更一般地说，你将实现一种用户级中断/异常处理程序的原始形式；例如，你可以使用类似的方法来处理应用程序中的页面错误。如果你的解决方案通过了 alarmtest 和 usertests，那么你的解决方案就是正确的。

你应该添加一个新的 sigalarm(interval, handler) 系统调用。如果应用程序调用 sigalarm(n, fn)，那么在**程序每消耗 n 个“滴答”（tick）的 CPU 时间后，内核应触发应用程序函数 fn 的调用。当 fn 返回时，应用程序应从中断的地方继续执行**。在 xv6 中，“滴答”是一个相当任意的时间单位，由硬件定时器生成中断的频率决定。如果应用程序调用 sigalarm(0, 0)，内核应停止生成定期的警报调用。

你会在你的 xv6 仓库中找到一个文件 user/alarmtest.c。将其添加到 Makefile 中。在你添加 sigalarm 和 sigreturn 系统调用之前，它不会正确编译（见下文）。

alarmtest 在 test0 中调用 sigalarm(2, periodic)，要求内核每 2 个“滴答”强制调用一次 periodic()，然后自旋一段时间。你可以在 user/alarmtest.asm 中看到 alarmtest 的汇编代码，这对于调试可能很有帮助。



流程：用户态调用`sigalarm(2, periodic);`，这是一个系统调用;
下一步执行`uservec`，从用户态进入到内核态，由于是系统调用则会触发`usertrap`；
在`usertrap`先调用`sigalarm`这个系统调用，获取到参数`2, periodic`，保存到进程中的`trapframe`中，然会运行`usertrapret();`返回到用户态；
在这期间，内核态中每个`tick`都会发生时钟中断，然后就会执行`if(which_dev == 2)`下的命令，然后返回到用户态。直到其中一个时钟中断满足条件,使得`p->trapframe->epc = p->alarm_handler;`，然后返回到用户态，用户态执行`alarm_handler`函数，在这之前用户态一直执行自己的命令。
**详细流程**
  1.  用户态调用 sigalarm(2, periodic)
  用户态调用 sigalarm(2, periodic) 时，首先会触发一个系统调用。
  2. 进入内核态并执行 usertrap
   当系统调用发生时，CPU 会从用户态切换到内核态，并触发 usertrap 函数。
   在 usertrap 中，系统调用会被识别和处理
  3.  执行 syscall
   syscall 函数会调用具体的系统调用处理函数 sys_sigalarm。
   在 sys_sigalarm 中，系统会获取传入的参数 2 和 periodic，并将它们保存到当前进程的结构体中
  4. 返回到用户态
   完成系统调用后，usertrap 会调用 usertrapret 将控制权返回到用户态
  5. 处理时钟中断
   每次时钟中断都会触发 usertrap，并进入如下的代码块：`if((which_dev = devintr()) != 0)`
   如果时钟中断 `(which_dev == 2)`，系统会增加当前进程的 `ticks_since_alarm` 计数器：
  6.  执行 `alarm_handler`
    一旦 `ticks_since_alarm` 达到` alarm_interval`，系统会将当前的 `trapframe` 保存到 `alarm_tf` 中，并将 `epc` 设置为 `alarm_handler` 的地址。这样，下一次返回用户态时，用户进程会从 a`larm_handler` 开始执行。
  7.  返回到用户态并执行 `alarm_handler`
   当返回到用户态时，进程会从 `alarm_handler` 开始执行。处理完` alarm_handler` 后，用户代码需要调用 `sigreturn` 系统调用，以便恢复被中断的代码状态。
  8. `sigreturn` 系统调用
   `sigreturn` 系统调用会恢复被保存的` trapframe`，确保进程在执行完 `alarm_handler` 后继续执行被中断的代码：

### 1. `usertrap`：这是内核中的一个C函数，负责处理从用户空间发生的陷阱事件。
   理解这个函数代码：
   which_dev 这个变量
   ```cpp
    void
    usertrap(void)
    {
    int which_dev = 0;
    // 确保当前处理器处于用户态，如果不是则触发 panic，表示发生了不正常的状态。
    //  usertrap() 期望处理器之前是在用户模式下运行
    if((r_sstatus() & SSTATUS_SPP) != 0)
        panic("usertrap: not from user mode");

    // send interrupts and exceptions to kerneltrap(),
    // since we're now in the kernel.
    // 将异常处理向量设置为 kernelvec，因为现在处理器已经在内核态了。
    //这行代码的作用是将内核陷阱处理程序的入口地址写入到 stvec 寄存器中，以便处理器在发生陷阱时能够正确跳转到内核陷阱处理程序。
    w_stvec((uint64)kernelvec);

    struct proc *p = myproc();
    
    // save user program counter.
    p->trapframe->epc = r_sepc();
    
    if(r_scause() == 8){
        // system call

        if(p->killed) //如果进程被标记为要终止，则调用 exit(-1) 终止进程
        exit(-1);

        // sepc points to the ecall instruction,
        // but we want to return to the next instruction.
        p->trapframe->epc += 4;

        // an interrupt will change sstatus &c registers,
        // so don't enable until done with those registers.
        intr_on();

        syscall();
    } else if((which_dev = devintr()) != 0){ //如果没有发生系统调用，但发生了设备中断，则调用 devintr() 处理设备中断，并将结果保存在 which_dev 变量中。
        // ok
    } else {
        printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
        printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
        p->killed = 1;
    }

    if(p->killed) 
        exit(-1);

    // give up the CPU if this is a timer interrupt.
    if(which_dev == 2)
        yield(); // yield() 函数的作用是让出 CPU 给其他就绪状态的进程执行

    usertrapret();
    }
   ```
  yield() 函数的作用是让出 CPU 给其他就绪状态的进程执行
   ```cpp
    void
    yield(void)
    {
    struct proc *p = myproc();
    acquire(&p->lock);
    p->state = RUNNABLE;  // 将当前进程的状态设置为就绪状态
    sched();             // 调用调度程序选择下一个进程执行
    release(&p->lock);   // 释放当前进程的锁
    }
   ```
### 2. 根据上面的流程
1. 由于使用`alarmtest.c`来测试的，所以先在`Makefile`中加上`$U/_alarmtest\`,进行编译。
2. 我们需要在用户态`syscall.h`声明系统调用，并且在 `usys.pl`中包含系统调用
3. 在内核态`syscall.c`声明系统调用函数，和`syscall.h`系统调用号,在`sysproc.c`中实现函数
4. 首先，先在`proc.h`中，在进程中声明我们想要的参数
   1. 警报间隔
   2. 警报处理函数指针
   3. 距离上次警报处理的ticks数
   4. 一个`trapframe`指针，用来恢复现场
   5. 一个标记，用来标记当前是否在执行警报处理函数
   ```cpp
     struct proc {
         int alarm_interval;          //警报间隔
         uint64 alarm_handler; //警报处理函数
         int ticks_since_alarm;  //距离上次警报处理的ticks数
         struct trapframe* alarm_tf; //用来恢复现场
         int in_alarm_handler;  //标记当前是否在执行警报处理函数
     }
   ```
5. 初始化
   我们知道，当创建一个进程时，我们需要分配和初始化一个新的进程结构体，所以需要给这些变量初始化
   ```cpp
     static struct proc*
     allocproc(void)
     {
         if((p->alarm_tf = (struct trapframe *)kalloc()) == 0){
         freeproc(p);
         release(&p->lock);
         return 0;
         }
         p->alarm_interval = 0;
         p->alarm_handler = 0;
         p->ticks_since_alarm = 0;
         p->in_alarm_handler = 0;
     }
   ```
   当进程结束后，要释放内存；
   ```cpp
     static void
     freeproc(struct proc *p)
     {
         if(p->alarm_tf)
             kfree((void*)p->alarm_tf);
         p->alarm_interval = 0;
         p->alarm_handler = 0;
         p->ticks_since_alarm = 0;
         p->in_alarm_handler = 0;
     }
   ```
6. 实现`sys_sigalarm()`这个系统调用
   因为，在用户态传递两个参数，一个是警报间隔，一个是要执行的函数指针；
 并且需要把这些变量保存到进程中，从而在时钟中断时，用来判断是否满足要求，从而让用户态运行警报函数。并且初始化一些需要的变量
 ```cpp
     uint64 sys_sigalarm()
     {
     printf("sys_sigalarm\n");
     int interval;
     uint64 handler;
     if(argint(0, &interval) < 0)
         return -1;
     if (argaddr(1, &handler) < 0)
         return -1;
     struct proc *p = myproc();
     p->alarm_interval = interval;
     p->alarm_handler = handler;;
     p->ticks_since_alarm = 0;
     p->in_alarm_handler = 0;
     return 0;
     }
 ```
8. 修改``usertrap`函数
  我们知道，每个`trick`都会触发一个时钟中断，每发生一个时钟中断，就让间隔`ticks_since_alarm`加1，当`ticks_since_alarm`大于警报间隔时，我们就让用户态执行警报函数。
  首先需要防止对处理函数的重入调用——如果处理函数尚未返回，内核不应再次调用它，我们只让`p->in_alarm_handler==0`才开始计数；
  由于`epc`保留着用户态程序计数器，所以可以直接修改这个值为警报函数指针即可；
  而且，我们想执行问警报函数后，返回到原来的位置，所以我们需要保存现场，即保存当前的`trapframe`
  ```cpp
     if((which_dev = devintr()) != 0){
         // ok
         if(which_dev == 2)
         {
             if(p->alarm_interval > 0 && p->in_alarm_handler==0)
             {
                 p->ticks_since_alarm++;
                 if(p->ticks_since_alarm >= p->alarm_interval)
                 {
                     p->ticks_since_alarm = 0;
                     memmove(p->alarm_tf, p->trapframe, PGSIZE);
                     p->trapframe->epc = p->alarm_handler;
                     p->in_alarm_handler = 1;
                 }
             }
             yield();
         }
     } 
  ```
9. 当调用过`sys_sigalarm`后，便调用`sys_sigreturn`用来恢复原来的状态
  只需根据之前保留的`trapframe`恢复即可，
  并且设置，当前不处于执行问警报函数状态
  ```cpp
     uint64 sys_sigreturn(void)
     {
         struct proc *p = myproc();
         memmove(p->trapframe, p->alarm_tf, PGSIZE);
         p->in_alarm_handler = 0;
     return 0;
     }
  ```

![结果图](.\图片\lab4结果图片.png)





2024-7-12
