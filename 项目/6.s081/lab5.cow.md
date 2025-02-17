2024-7-17

虚拟内存提供了一层间接性：内核可以通过将页表条目（PTEs）标记为无效或只读来拦截内存引用，从而引发页故障，并且可以通过修改 PTEs 来改变地址的含义。计算机系统中有一句格言：任何系统问题都可以通过一层间接性来解决。懒惰分配实验提供了一个例子。本实验探讨了另一个例子：写时复制的 fork。

**问题**
在 xv6 中，`fork()` 系统调用会将父进程的所有用户空间内存复制到子进程中。如果父进程很大，复制过程可能需要很长时间。更糟糕的是，这项工作往往会被大量浪费；例如，子进程在 `fork()` 之后立即调用 `exec()`，会导致子进程丢弃已复制的内存，而且很可能从未使用过其中的大部分。另一方面，如果父进程和子进程都使用某一页内存，并且其中一个或两个进程对其进行写操作，那么确实需要进行复制。

**解决方案**

迟分配（Lazy Allocation）
+ 目的
  + 提高内存使用效率：在传统的内存分配方式中，当程序请求内存时，操作系统会立即为其分配物理内存页。而迟分配则是在程序真正访问这些内存页时才进行实际的物理内存分配。这样可以避免提前分配那些可能永远不会被使用的内存，从而提高了内存的整体使用效率。
  + 减少系统开销：立即分配物理内存需要操作系统进行一系列的操作，如查找空闲页框、更新页表等，这些操作会带来一定的系统开销。迟分配将这些操作推迟到实际需要时进行，减少了不必要的开销，提高了系统的性能。
+ 优点
  + 虚拟内存空间的高效利用：允许进程在不立即分配物理内存的情况下请求大量的虚拟内存。例如，一个程序可能会一次性请求一个很大的数组，但实际上只会使用其中的一小部分。迟分配使得操作系统可以先为其分配虚拟地址空间，而在真正访问这些地址时再分配物理内存，从而更有效地利用了有限的物理内存资源。
  + 快速进程创建：在创建新进程时，不需要为其立即分配大量的物理内存。进程可以在需要时逐步获得物理内存，加快了进程创建的速度。

写时复制（Copy - On - Write, COW）机制
+ 目的
  + 减少内存复制开销：在使用 fork() 系统调用创建子进程时，传统的做法是将父进程的整个地址空间复制到子进程中，这会带来大量的内存复制操作，消耗大量的时间和系统资源。COW 机制避免了这种不必要的复制，只有在父进程或子进程试图修改某个页面时，才会真正复制该页面。
  + 提高内存利用率：在 fork() 之后的一段时间内，父进程和子进程可以共享大部分的物理内存页面，只有在需要修改时才进行复制，从而减少了物理内存的使用量。
+ 优点
  + 高效的进程创建：fork() 操作变得非常快速，因为它只需要复制父进程的页表，而不需要复制实际的物理内存页面。这使得新进程可以迅速创建，提高了系统的响应速度。
  + 节省内存：在 fork() 之后，父进程和子进程可以共享相同的物理内存页面，直到其中一个进程需要修改这些页面。这种共享机制减少了物理内存的占用，特别是在父进程和子进程大部分时间都在读取相同数据的情况下。
  + 提高系统并发性能：由于 fork() 操作的开销降低，系统可以更轻松地创建大量的子进程，从而提高了系统的并发处理能力。例如，在多进程服务器应用中，可以快速创建多个子进程来处理客户端请求。


写时复制（Copy-On-Write, COW）`fork()` 的目标是推迟为子进程分配和复制物理内存页面，直到这些页面真正需要为止，如果它们从未需要，则根本不需要复制。

COW `fork()` 仅为子进程创建一个页表，其中用户内存的页表条目（PTEs）指向父进程的物理页面。COW `fork()` 将父进程和子进程中所有用户 PTEs 标记为不可写。当任一进程尝试写入这些 COW 页面时，CPU 将强制生成一个页故障。内核页故障处理程序检测到这种情况，为发生故障的进程分配一个物理页面，将原始页面的内容复制到新的页面，并修改发生故障的进程中的相应 PTE，使其指向新的页面，并将 PTE 标记为可写。当页故障处理程序返回时，用户进程将能够写入其页面的副本。

COW `fork()` 使得释放实现用户内存的物理页面变得稍微复杂一些。某个物理页面可能被多个进程的页表引用，只有当最后一个引用消失时，才能释放该物理页面。

希望这对你有帮助！如果有任何进一步的问题，请随时提问。


Implement copy-on write:
实现 fork 延迟复制机制，在进程 fork 后，不立刻复制内存页，而是将虚拟地址指向与父进程相同的物理地址。在父子任意一方尝试对内存页进行修改时，才对内存页进行复制。 物理内存页必须保证在所有引用都消失后才能被释放，这里需要有引用计数机制。

### 修改`uvmcopy()`
在`fork()`时，会调用`uvmcopy(p->pagetable, np->pagetable, p->sz)`复制父进程的内存到子进程;
我们要做的就是不立刻复制内存页，而是建立指向原物理页的映射，并将父子两端的页表项都设置为不可写。
当某个进程要修改内存时，触发`page faults`，重新分配一块物理地址，然后改该进程的虚拟地址指向该物理地址。

这时候就有一个问题，当某页物理地址，其他进程全部重新分配直到另一页内存，只剩下一个进程使用这块内存时，当该进程要写入时，我们就不必再重新分配物理地址了，直接使用这块物理地址就行了。后面分配内存的时候会用到

注意1：当父进程中有些页面是只读页面，那么我们就不必打上COW标记了，父进程和子进程都指向这块内存，都是只读。若在父进程中这块内存可写，则修改控制位：不可写且可COW。

注意2：使用`mappages`映射物理内存;从虚拟地址`i` 开始的大小为 `PGSIZE`的区域,创建到物理地址`pa` 的映射，并设置相应的权限 `perm`.并在子进程`new=p->pagetable`存储页表条目。

注意3：子进程和父进程都指向这块物理内存，所以引用数加1
```cpp
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walk(old, i, 0)) == 0)
      panic("uvmcopy: pte should exist");
    if((*pte & PTE_V) == 0)
      panic("uvmcopy: page not present");
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    if(flags & PTE_W) //注意 1
    {
        flags = (flags | PTE_COW) & (~PTE_W);
        *pte = PA2PTE(pa) | flags;
    }
    if(mappages(new, i, PGSIZE, (uint64)pa, flags) != 0){ //注意 2
      goto err;
    }
    krefpage((void *)pa); //注意 3
  }
  return 0;

 err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  return -1;
}
```
### 定义PTE_COW标志位

PTE_COW标志位用于标示一个映射对应的物理页是否是延迟复制页。
在`riscv.h`中定义:
`#define PTE_COW (1L << 8)`

这样，fork 时就不会立刻复制内存，只会创建一个映射了。这时候如果尝试修改懒复制的页，会出现 page fault 被 usertrap() 捕获。接下来需要在 usertrap() 中捕捉这个 page fault，并在尝试修改页的时候，执行实复制操作。

### `usertrap(void)`修改
在这里不需要修改 epc，在调用系统调用时，我们把 epc+4，是因为下一步将调用下一条指令。但是在这里，触发page fault后我们重新执行这条命令，重新进行读写操作。，这样就会在新的页面上操作了
在 usertrap() 中添加对 page fault 的检测，并在当前访问的地址符合懒复制页条件时，对懒复制页进行实复制操作：

```cpp
 if(r_scause() == 15 || r_scause() == 13){
          uint64 va = r_stval();
          if(!iscowpage(va))
            p->killed = 1;
          else if(uvmcowcopy(va)==-1)
          {
            p->killed =1;
          }
  }
```

1. `iscowpage(va)`
在`vm.c`中定义`iscowpage(va)`用于检测虚拟地址`va`是不是符合懒复制页条件；
要在`defs.h`中声明创建的函数；
```cpp
...
#include "spinlock.h"
#include "proc.h"
...
int iscowpage(uint64 va)
{
    pte_t* pte;
    struct proc* p = myproc();
    return va < p->sz
            && ((pte =walk(p->pagetable, va,0))!=0 && (*pte & PTE_V))
            && (*pte & PTE_COW);
}
```
2. `uvmcowcopy(va)`
实现分配物理地址函数;
这里调用`kcopy_n_deref()`分配内存的原因就是为了处理只剩下一个进程使用这块内存的问题；
```cpp
int uvmcowcopy(uint64 va)
{
    pte_t* pte;
    struct proc* p = myproc();
    if((pte = walk(p->pagetable, va, 0))==0)
    {
        panic("uvmcowcopy: walk\n");
    }
    uint64 pa = PTE2PA(*pte);
    uint64 new = (uint64)kcopy_n_deref((void *)pa);
    if(new == 0)
        return -1;
    // 重新映射为可写，并清除 PTE_COW 标记
    uint64 flags = (PTE_FLAGS(*pte) | PTE_W) & ~PTE_COW;
    uvmunmap(p->pagetable, PGROUNDDOWN(va), 1, 0); //取消映射
    if(mappages(p->pagetable, va, 1, new, flags)==-1)  //chong映射
        panic("uvmcowcopy: mappages\n");
    return 0;
}
```
### `kalloc.c`实现
1. `kcopy_n_deref(void *pa)`
```cpp
void *kcopy_n_deref(void *pa)
{
    acquire(&pgreflock);
    if(PA2PGERF(pa) <= 1) // 只有 1 个引用，无需复制
    {
        release(&pgreflock);
        return pa;
    }
    // 分配新的内存页，并复制旧页中的数据到新页
    uint64 newpa = (uint64)kalloc();
    if(newpa == 0)
    {
        release(&pgreflock);
        return 0;
    }
    memmove((void *)newpa, (void *)pa, PGSIZE);
    // 旧页的引用减 1
    PA2PGERF(pa)--;
    release(&pgreflock);
    return (void *)newpa;
}
```
2. 实现计数引用
```cpp
// kalloc.c
#define PA2PGREF_ID(p) ((p-KERNBASE)/PGSIZE)
#define PGREF_MAX_ENTRIES PA2PGREF_ID(PHYSTOP)
#define PA2PGERF(p) pageref[PA2PGREF_ID((uint64)p)]
struct spinlock pgreflock;
int pageref[PGREF_MAX_ENTRIES];
void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&pgreflock, "pgref");
  freerange(end, (void*)PHYSTOP);
}

void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

acquire(&pgreflock);
if(--PA2PGERF(pa)<=0){
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}
release(&pgreflock);
}

void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    PA2PGERF(r)=1;
  }
  return (void*)r;
}
//计数
void krefpage(void *pa)
{
    acquire(&pgreflock);
    PA2PGERF(pa)++;
    release(&pgreflock);
}
```

### `copyout()`实现
copyout() 由于是软件访问页表，不会触发缺页异常，所以需要手动添加同样的监测代码（同 lab5），检测接收的页是否是一个懒复制页，若是，执行实复制操作

```cpp
int
copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
  uint64 n, va0, pa0;

  while(len > 0){
    if(iscowpage(dstva)) // 检查每一个被写的页是否是 COW 页
        uvmcowcopy(dstva);
    va0 = PGROUNDDOWN(dstva);
    pa0 = walkaddr(pagetable, va0);
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (dstva - va0);
    if(n > len)
      n = len;
    memmove((void *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  return 0;
}
```



分割线********************************************************************************



2024-7-16:cowtest能够通过，但是usertests不能全部通过，得分80/110
实在改不下去了，打算看看人家怎么写的，重新梳理一下；
后续再回来修改


根据提示来写：
### 0. 定义`PTE_COW`标志位
1. 在 riscv.h 或相关头文件中定义 PTE_COW 标志位
`#define PTE_COW (1L << 8)  // 使用第8位作为PTE_COW标志`
### 2. 需要一个计数器，来判断这个物理页是否要释放。因为子进程和父进程共享一块区域，当父进程完成的时候，我们并不能立即释放这块内存，因为子进程有可能还在使用。所以，我们给每个物理页设置一个计数器，当计数器减为0时，我们再释放该内存
#### 1. 定义计数器数组
首先，我们需要在kalloc.c中定义一个数组来存储每个物理页的引用计数。假设物理内存的最高地址是PHYSTOP，我们可以使用物理页地址除以页面大小（4096）作为数组的索引。
在`kalloc.c`中
```cpp
#define MAX_PAGES (PHYSTOP / PGSIZE)
int ref_count[MAX_PAGES];

void
increase_ref_count(uint64 pa)
{
  int index = pa / PGSIZE;
  acquire(&kmem.lock);
  ref_count[index]++;
  release(&kmem.lock);
}

void
decrease_ref_count(uint64 pa)
{
  int index = pa / PGSIZE;
  acquire(&kmem.lock);
  if(--ref_count[index] == 0)
    kfree((void*)pa);
  release(&kmem.lock);
}

```
#### 2. 初始化数组
```cpp
void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
  for(int i=0;i<MAX_PAGES;i++)
    ref_count[i]=0;
}
```
#### 3. 修改`kalloc`函数
在分配页面时。初始化引用计数为1
```cpp
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    increase_ref_count((uint64)r);
  }
  return (void*)r;
}
```
#### 4. 修改`uvmcopy`
在 `uvmcopy` 函数中，我们需要将父进程的物理页面映射到子进程，并清除写权限，同时设置 `PTE_COW` 标志：
使父进程和子进程共享物理内存,并且子进程和进程只能读。那么写的时候就会触发页面错误，再分配内存
在fork时增加引用计数，（Copy user memory from parent to child）
将父进程的物理页映射到子进程。
清除写权限，同时设置 PTE_COW 标志。
增加物理页的引用计数
```cpp
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
//   char *mem;

  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walk(old, i, 0)) == 0)
      panic("uvmcopy: pte should exist");
    if((*pte & PTE_V) == 0)
      panic("uvmcopy: page not present");
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    // if((mem = kalloc()) == 0)  //分配一个页面
    //   goto err;
    // memmove(mem, (char*)pa, PGSIZE);
    if(mappages(new, i, PGSIZE, pa, (flags & ~PTE_W) | PTE_COW) != 0){
      goto err;
    }
    *pte = (*pte & ~PTE_W) | PTE_COW;  // 清除写权限并设置COW标志
    increase_ref_count(pa);
  }
  return 0;

 err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  return -1;
}
```
#### 5. 修改usertrap()
修改usertrap(),当触发页面错误的时候，分配内存
处理页面错误时检测并处理 COW 页面。
```cpp
if((r_scause() & 0xff)==0x0f) // page fault
  {
    uint64 va = r_stval();
    printf("page fault &p\n",va);
    va = PGROUNDDOWN(va);
    pte_t *pte = walk(p->pagetable, va, 0);
    if(pte == 0){
      printf("usertrap: page fault at %p, but no PTE\n", va);
      p->killed = 1;
    } else if(*pte & PTE_COW) {
        uint64 pa = PTE2PA(*pte);
        uint64 ka = (uint64)kalloc();
        if(ka == 0){
            printf("usertrap: kalloc failed\n");
            p->killed = 1;
        } else {
            memmove((void*)ka, (void*)pa, PGSIZE);
            *pte = PA2PTE(ka) | PTE_FLAGS(*pte) | PTE_W;
            decrease_ref_count(pa);
        } 
    } else {
      printf("usertrap: page fault at %p, not COW\n", va);
      p->killed = 1;
    } 
  } 
```
#### 6. 修改`uvmunmap`函数
在释放页面时减少引用计数，并在引用计数为0时真正释放页面
```cpp
void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)
{
  uint64 a;
  pte_t *pte;

  if((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  for(a = va; a < va + npages*PGSIZE; a += PGSIZE){
    if((pte = walk(pagetable, a, 0)) == 0)
      panic("uvmunmap: walk");
    if((*pte & PTE_V) == 0)
      panic("uvmunmap: not mapped");
    if(PTE_FLAGS(*pte) == PTE_V)
      panic("uvmunmap: not a leaf");
    if(do_free){
      uint64 pa = PTE2PA(*pte);
      decrease_ref_count(pa); //注意
    }
    *pte = 0;
  }
}
```
#### 7.修改`copyout`函数以处理COW页面
修改copyout函数，在遇到COW页面错误时，使用与页面错误处理相同的方案
```cpp
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
//   char *mem;

  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walk(old, i, 0)) == 0)
      panic("uvmcopy: pte should exist");
    if((*pte & PTE_V) == 0)
      panic("uvmcopy: page not present");
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    // if((mem = kalloc()) == 0)  //分配一个页面
    //   goto err;
    // memmove(mem, (char*)pa, PGSIZE);
    if(mappages(new, i, PGSIZE, pa, (flags & ~PTE_W) | PTE_COW) != 0){
      goto err;
    }
    *pte = (*pte & ~PTE_W) | PTE_COW;  // 清除写权限并设置COW标志
    increase_ref_count(pa);
  }
  return 0;

 err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  return -1;
}
```

