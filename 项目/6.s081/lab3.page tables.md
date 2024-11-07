这个lab主要看第3个

# Speed up system calls  加快系统调用
一些操作系统（例如Linux）通过**在用户空间和内核之间共享只读区域中的数据来加速某些系统调用**。这消除了在执行这些系统调用时跨越内核的需要。为了帮助您了解如何将映射插入页表，您的第一个任务是为xv6中的getpid（）系统调用实现这种优化。

在创建每个进程时，在usycall（在memlayout.h中定义的一个VA）上映射一个只读页面。在本页的开头，存储一个结构体usycall（也在memlayout.h中定义），并将其初始化以存储当前进程的PID。对于这个实验，ugetpid（）已经在用户空间端提供，并将自动使用usycall映射。如果在运行pgtbltest时ugetpid测试用例通过，您将获得这部分实验的全部学分。


就是把进程的pid放到页表中
分配进程的时候初始化：
```cpp
  //分配进程
  if((p->usyscall = (struct usyscall *)kalloc()) == 0){
  freeproc(p);
  release(&p->lock);
  return 0;
  }
  p->usyscall->pid=p->pid;
```

清理的时候，要释放页表
```cpp
// 内核页和用户也共享一块内存
{ ....
  if(mappages(pagetable, USYSCALL, PGSIZE, (uint64)(p->usyscall), PTE_R | PTE_U) < 0){
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmunmap(pagetable, TRAPFRAME, 1, 0);
    uvmfree(pagetable, 0);
    return 0;
  }

  return pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmunmap(pagetable, USYSCALL, 1, 0);
  uvmfree(pagetable, sz);
}

```

# vmprint 打印页表
为了帮助您可视化RISC-V页表，也可能是为了帮助将来的调试，您的第二个任务是编写一个打印页表内容的函数。

定义一个名为vmprint（）的函数。它应该接受一个pagetable_t参数，并以下面描述的格式打印该页表。在exec.c返回参数argc之前插入if(p->pid==1) vmprint(p->pagetable)，打印第一个进程的页表。如果你通过了make等级的打印测试，你将获得这部分实验的全部学分。


这个函数就是遍历页表，然后输出就行了.
需要注意的是：
  页表条目（PTE）用来表示页表层次结构的不同级别。一个页表条目可以指向
  1. 一个具体的物理页框，表示虚拟地址到物理地址的直接映射。
     如果一个页表条目指向一个物理页框，那么这个条目通常会有至少一个读、写、执行权限中的一个或多个。
     也就是说，PTE_R、PTE_W 或 PTE_X 中至少有一个会被设置。
  2. 下一级页表，表示页表的层级结构。
     如果一个页表条目指向下一级页表，那么这个条目本身不会有读、写、执行权限，因为这些权限是针对实际内存页框的。因此，对于指向下一级页表的条目，PTE_R、PTE_W 和 PTE_X 标志位都应该是0。
通过检查 (pte & (PTE_R|PTE_W|PTE_X)) == 0，可以判断该页表条目是否指向下一级页表。如果这个条件为真，则说明这个条目没有任何读、写、执行权限，因此它指向的是下一级页表，而不是一个物理页框。

```cpp
void
printpag(pagetable_t pagetable, int n)
{
    for (int i = 0; i < 512; i++)
    {
        pte_t pte = pagetable[i];
       if((pte & PTE_V) && (pte & (PTE_R|PTE_W|PTE_X)) == 0)
        {
            uint64 pd = PTE2PA(pte);
            for(int i=0;i<n;i++)
                printf(".. ");
            printf(".. %d: pte %p pa %p\n",i ,pte, pd);
            printpag((pagetable_t)pd, n+1);
        }else if(pte & PTE_V){
            for(int i=0;i<n;i++)
                printf(".. ");
            uint64 pd = PTE2PA(pte);
            printf("%d: pte %p pa %p\n",i ,pte, pd);
        }
    }   
}

void
vmprint(pagetable_t pagetable)
{
    printf("page table %p\n",pagetable);
    printpag(pagetable, 0);
}
```


# sys_pgaccess() 检测访问了哪些页面
一些垃圾收集器（自动内存管理的一种形式）可以从访问了哪些页面（读或写）的信息中获益。在本部分的实验中，您将向xv6添加一个新特性，该特性通过检查RISC-V页表中的访问位来检测并向用户空间报告这些信息。RISC-V硬件页读取器在解析TLB缺失时在PTE中标记这些位。

您的任务是实现pgaccess()，这是一个报告**访问了哪些页面的系统调用**。这个系统调用有三个参数。首先，它使用要检查的第一个用户页面的起始虚拟地址。其次，它需要检查的页数。最后，它需要一个用户地址到缓冲区，将结果存储到位掩码（一种每页使用一位的数据结构，其中第一页对应于最低有效位）中。如果pgaccess测试用例在运行pgtbltest时通过，您将获得这部分实验的全部学分。


任务：实现一个系统调用`pgaccess()`，返回那个页已经被访问了。这个系统调用有三个参数。
    `pgaccess(buf, 32, &abits)`，
    第一个：虚拟地址
    第二个：多少个页需要检查
    第三个：存储结果，结果的每一位代表一个页是否被访问，假如第一页已经被访问了，那么第一位就是1，第二页没有访问则返回01.

因为是实现系统调用，根据lab2，我们可以先看一下`syscall.c sycall.h usys.pl`文件，可以看到系统调用的声明和系统调用号已经写好了。

剩下的就是在`sysproc.c`中实现改函数了。

1. 因为用户态传递三个参数调用该系统调用，所以首先我们先从用户态获取这三个参数，用`argaddr() argint()`这两个函数可以从用户态拿取到参数。在lab2我们已经知道，系统调用是通过寄存器传递参数的，那么传递三个参数，就用到三个寄存器a0 a1 a2,然后就可以写出获取参数的代码了。
用于验证，我们可以在`pgtbltest.c`中也加入输出语句，验证输出。
在终端运行`pgtbltest`,可以发现结果一致。
```cpp
  //获取参数
  // 虚拟地址
    uint64 vaddr;
    if(argaddr(0,&vaddr)<0)
        return -1;
  //数量
    int num;
    if(argint(1,&num)<0)
        return -1;
  //储存地址
    uint64 uaddr;  
    if(argaddr(2,&uaddr)<0)
        return -1;   
    printf("sys_pgaccess: %d %d %d\n",vaddr,num,uaddr);
    return 0;
```
```cpp
  //pgtbltest.c
  buf = malloc(32 * PGSIZE);
  printf("pgaccess_test: %d %d %d\n",buf,32,&abits);
  if (pgaccess(buf, 32, &abits) < 0)
    err("pgaccess failed");
```
2. 然后，我们就是在当前进程的页表检查一组虚拟内存页是否被访问过，并返回一个位掩码，表示哪些页被访问过。每个虚拟内存页都有一个访问位（Accessed bit），操作系统可以通过这个位来跟踪页面是否被读取或写入

首先，先定义`PTE_A`,在`riscv.h`中`#define PTE_A (1L << 6)`

然后就是遍历每一页，检查每一页的访问位。如果访问位被设置，则表示该页已经被访问过。之后，它会将相应的位掩码位置1，并清除该页的访问位

```cpp
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  //获取参数
  // 虚拟地址
    uint64 vaddr;
    if(argaddr(0,&vaddr)<0)
        return -1;
  //数量
    int num;
    if(argint(1,&num)<0)
        return -1;
  //储存地址
    uint64 uaddr;  
    if(argaddr(2,&uaddr)<0)
        return -1;   

    struct proc *p = myproc();
    uint64 mask=0;
    // printf("sys_pgaccess: %d %d %d\n",vaddr,num,uaddr);
    for(int i=0;i<num;i++)
    {
        pte_t* pte = walk(p->pagetable,vaddr+i*PGSIZE,0); //每一页的大小PGSIZE
        if(pte && (*pte & PTE_A))
        {
            mask = mask | (1<<i);
            // printf("mask=%d\n",mask);
            *pte = (*pte) & (~PTE_A);
        } 
    }
    if(copyout(p->pagetable,uaddr,(char*)&mask,sizeof(mask))<0)
        return -1;
    // printf("sys_pgaccess: %d %d %d\n",vaddr,num,uaddr);
    return 0;
}
```

要注意，我们需要声明`walk`函数,在`"defs.h"`中

3. `walk 函数`用于在页表中找到给定虚拟地址（va）对应的页表项（PTE）。它会遍历多级页表，直到找到所需的页表项。如果页表项不存在，并且 alloc 参数为真，它会分配新的页表项
可以反向最后的返回值，` return &pagetable[PX(0, va)];`返回的是第三层的包含物理地址的那一部分。
```cpp
pte_t *
walk(pagetable_t pagetable, uint64 va, int alloc)
{
  if(va >= MAXVA)
    panic("walk");

  for(int level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    if(*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } else {
      if(!alloc || (pagetable = (pde_t*)kalloc()) == 0)
        return 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  return &pagetable[PX(0, va)];
}
```
看一下`walkaddr`这个函数，就知道walk函数得到pte后，先提取页框号（去掉后10位，标志位，然后加上12位0），然后加上虚拟地址的偏移就可以组成真正的物理地址了。

