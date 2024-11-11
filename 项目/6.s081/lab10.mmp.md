# Lab: mmap (hard)
mmap 和 munmap 系统调用允许 UNIX 程序对其地址空间进行详细控制。它们可以**用于进程之间共享内存**，将文件映射到进程的地址空间中，以及作为用户级页错误处理方案的一部分，例如讲座中讨论的垃圾回收算法。在这个实验中，你将为 xv6 添加 mmap 和 munmap，重点关注内存映射文件。

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);

mmap 可以以多种方式调用，但本实验只需要其子集的功能，相关于内存映射一个文件。你可以假设 addr 总是零，意味着内核应该决定映射文件的虚拟地址。mmap 返回这个地址，如果失败则返回 0xffffffffffffffff。length 是要映射的字节数，它可能与文件的长度不同。prot 指示内存是否应该被映射为可读、可写和/或可执行；你可以假设 prot 是 PROT_READ 或 PROT_WRITE 或两者兼有。flags 将是 MAP_SHARED，意味着对映射内存的修改应写回到文件中，或 MAP_PRIVATE，意味着修改不应写回。你不需要实现 flags 中的其他位。fd 是**要映射的文件的打开文件描述符**。你可以假设 offset 为零（这是文件中要映射的起始点）。

如果映射同一 MAP_SHARED 文件的进程不共享物理页面也是可以的。

munmap(addr, length) 应该移除在指定地址范围内的 mmap 映射。如果进程已经修改了内存，并且内存是以 MAP_SHARED 方式映射的，**这些修改应首先写回到文件中**。一个 munmap 调用可能只覆盖 mmap 区域的一部分，但你可以假设它要么在区域的开始处取消映射，要么在区域的结束处取消映射，或者取消整个区域的映射（但不会在区域的中间留下空洞）。

>你应该实现足够的 mmap 和 munmap 功能，以使 mmaptest 测试程序能够正常工作。如果 mmaptest 不使用某个 mmap 功能，你不需要实现那个功能。


提示：
  从将 _mmaptest 添加到 UPROGS 开始，以及添加 mmap 和 munmap 系统调用，以便编译 user/mmaptest.c。目前，只需在 mmap 和 munmap 中返回错误。我们已经在 kernel/fcntl.h 中为你定义了 PROT_READ 等宏。运行 mmaptest，它将在第一次 mmap 调用时失败。

  填充页面表：在响应页面错误时，懒惰地进行处理。即，mmap 不应分配物理内存或读取文件。相反，应在页面错误处理代码中完成这些操作（或由 usertrap 调用，如同懒惰页面分配实验中）。选择懒惰的原因是确保 mmap 大文件时速度较快，并且 mmap 比物理内存大的文件是可能的。

  跟踪每个进程的 mmap：定义一个对应于讲座 15 中描述的 **VMA（虚拟内存区域）的结构**，记录由 mmap 创建的虚拟内存范围的地址、长度、权限、文件等信息。由于 xv6 内核没有内存分配器，所以声明一个固定大小的 VMA 数组并根据需要从该数组中分配是可以的。大小为 16 应该足够。

  实现 mmap：在进程的地址空间中找到一个未使用的区域以映射文件，并将 VMA 添加到进程的映射区域表中。VMA 应包含一个指向文件结构的指针；mmap 应增加文件的引用计数，以确保当文件关闭时结构不会消失（提示：参见 filedup）。运行 mmaptest：第一次 mmap 应成功，但第一次访问映射的内存将导致页面错误并杀死 mmaptest。

  添加代码：在 mmap 区域中发生页面错误时，分配一页物理内存，将 4096 字节的相关文件读入该页，并将其映射到用户地址空间中。使用 readi 读取文件，它接受一个用于读取文件的偏移量参数（但你需要锁定/解锁传递给 readi 的 inode）。不要忘记正确设置页面的权限。运行 mmaptest；它应该到达第一个 munmap。

  实现 munmap：找到地址范围的 VMA 并取消映射指定的页面（提示：使用 uvmunmap）。如果 munmap 删除了以前 mmap 的所有页面，则应减少相应文件结构的引用计数。如果一个未映射的页面已被修改且文件是 MAP_SHARED 映射的，则将页面写回文件。参见 filewrite 以获取灵感。

  理想情况下，你的实现应该只写回程序实际修改的 MAP_SHARED 页面。RISC-V PTE 中的脏位（D）指示页面是否已被写入。然而，mmaptest 不检查未脏页面是否被写回，因此你可以在不查看 D 位的情况下写回页面。

  修改 exit：以 munmap 被调用的方式取消映射进程的映射区域。运行 mmaptest；mmap_test 应通过，但可能不会通过 fork_test。

  修改 fork：确保子进程具有与父进程相同的映射区域。不要忘记增加 VMA 的文件结构的引用计数。在子进程的页面错误处理程序中，分配一个新的物理页面而不是与父进程共享页面是可以的。后者会更酷，但需要更多的实现工作。运行 mmaptest；它应该通过 mmap_test 和 fork_test。


# 解
为了尽量使得 map 的文件使用的地址空间不要和进程所使用的地址空间产生冲突，我们选择将 mmap 映射进来的文件 map 到尽可能高的位置，也就是刚好在 trapframe 下面。并且若有多个 mmap 的文件，则向下生长。


```cpp
// kernel/memlayout.h

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE (MAXVA - PGSIZE)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#define KSTACK(p) (TRAMPOLINE - ((p)+1)* 2*PGSIZE)

// User memory layout.
// Address zero first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   mmapped files
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)
// MMAP 所能使用的最后一个页+1
#define MMAPEND TRAPFRAME

```

接下来定义 vma 结构体，其中包含了 mmap 映射的内存区域的各种必要信息，比如开始地址、大小、所映射文件、文件内偏移以及权限等。

并且在 proc 结构体末尾为每个进程加上 16 个 vma 空槽。

```cpp
// kernel/proc.h

struct vma {
  int valid;
  uint64 vastart;
  uint64 sz;
  struct file *f;
  int prot;
  int flags;
  uint64 offset;
};

#define NVMA 16

// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  struct proc *parent;         // Parent process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  struct vma vmas[NVMA];       // virtual memory areas
};

```

实现 mmap 系统调用。函数原型请参考 man mmap。函数的功能是在进程的 16 个 vma 槽中，找到可用的空槽，并且顺便计算所有 vma 中使用到的最低的虚拟地址（作为新 vma 的结尾地址 vaend，开区间），然后将当前文件映射到该最低地址下面的位置（vastart = vaend - sz）。

最后记得使用 filedup(v->f);，将文件的引用计数增加一。


```cpp
// kernel/sysfile.c

uint64
sys_mmap(void)
{
  uint64 addr, sz, offset;
  int prot, flags, fd; struct file *f;

  if(argaddr(0, &addr) < 0 || argaddr(1, &sz) < 0 || argint(2, &prot) < 0
    || argint(3, &flags) < 0 || argfd(4, &fd, &f) < 0 || argaddr(5, &offset) < 0 || sz == 0)
    return -1;
  
  if((!f->readable && (prot & (PROT_READ)))
     || (!f->writable && (prot & PROT_WRITE) && !(flags & MAP_PRIVATE)))
    return -1;
  
  sz = PGROUNDUP(sz);

  struct proc *p = myproc();
  struct vma *v = 0;
  uint64 vaend = MMAPEND; // non-inclusive
  
  // mmaptest never passed a non-zero addr argument.
  // so addr here is ignored and a new unmapped va region is found to
  // map the file
  // our implementation maps file right below where the trapframe is,
  // from high addresses to low addresses.

  // Find a free vma, and calculate where to map the file along the way.
  for(int i=0;i<NVMA;i++) {
    struct vma *vv = &p->vmas[i];
    if(vv->valid == 0) {
      if(v == 0) {
        v = &p->vmas[i];
        // found free vma;
        v->valid = 1;
      }
    } else if(vv->vastart < vaend) {
      vaend = PGROUNDDOWN(vv->vastart);
    }
  }

  if(v == 0){
    panic("mmap: no free vma");
  }
  
  v->vastart = vaend - sz;
  v->sz = sz;
  v->prot = prot;
  v->flags = flags;
  v->f = f; // assume f->type == FD_INODE
  v->offset = offset;

  filedup(v->f);

  return v->vastart;
}

```

映射之前，需要注意文件权限的问题，
如果尝试将一个只读打开的文件映射为可写，并且开启了回盘（MAP_SHARED），则 mmap 应该失败。否则回盘的时候会出现回盘到一个只读文件的错误情况。

由于需要对映射的页实行懒加载，仅在访问到的时候才从磁盘中加载出来，这里采用和 lab5: Lazy Page Allocation 类似的方式实现。

```cpp
// kernel/trap.c
void
usertrap(void)
{
  int which_dev = 0;

  // ......

  } else if((which_dev = devintr()) != 0){
    // ok
  } else {
    uint64 va = r_stval();
    if((r_scause() == 13 || r_scause() == 15)){ // vma lazy allocation
      if(!vmatrylazytouch(va)) {
        goto unexpected_scause;
      }
    } else {
      unexpected_scause:
      printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
      printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
      p->killed = 1;
    }
  }

  // ......

  usertrapret();
}

```

```cpp
// kernel/sysfile.c

// find a vma using a virtual address inside that vma.
struct vma *findvma(struct proc *p, uint64 va) {
  for(int i=0;i<NVMA;i++) {
    struct vma *vv = &p->vmas[i];
    if(vv->valid == 1 && va >= vv->vastart && va < vv->vastart + vv->sz) {
      return vv;
    }
  }
  return 0;
}

// finds out whether a page is previously lazy-allocated for a vma
// and needed to be touched before use.
// if so, touch it so it's mapped to an actual physical page and contains
// content of the mapped file.
int vmatrylazytouch(uint64 va) {
  struct proc *p = myproc();
  struct vma *v = findvma(p, va);
  if(v == 0) {
    return 0;
  }

  // printf("vma mapping: %p => %d\n", va, v->offset + PGROUNDDOWN(va - v->vastart));

  // allocate physical page
  void *pa = kalloc();
  if(pa == 0) {
    panic("vmalazytouch: kalloc");
  }
  memset(pa, 0, PGSIZE);
  
  // read data from disk
  begin_op();
  ilock(v->f->ip);
  readi(v->f->ip, 0, (uint64)pa, v->offset + PGROUNDDOWN(va - v->vastart), PGSIZE);
  iunlock(v->f->ip);
  end_op();

  // set appropriate perms, then map it.
  int perm = PTE_U;
  if(v->prot & PROT_READ)
    perm |= PTE_R;
  if(v->prot & PROT_WRITE)
    perm |= PTE_W;
  if(v->prot & PROT_EXEC)
    perm |= PTE_X;

  if(mappages(p->pagetable, va, PGSIZE, (uint64)pa, PTE_R | PTE_W | PTE_U) < 0) {
    panic("vmalazytouch: mappages");
  }

  return 1;
}

```

到这里应该可以通过 mmap 测试了，接下来实现 munmap 调用，将一个 vma 所分配的所有页释放，并在必要的情况下，将已经修改的页写回磁盘。

```cpp
// kernel/sysfile.c

uint64
sys_munmap(void)
{
  uint64 addr, sz;

  if(argaddr(0, &addr) < 0 || argaddr(1, &sz) < 0 || sz == 0)
    return -1;

  struct proc *p = myproc();

  struct vma *v = findvma(p, addr);
  if(v == 0) {
    return -1;
  }

  if(addr > v->vastart && addr + sz < v->vastart + v->sz) {
    // trying to "dig a hole" inside the memory range.
    return -1;
  }

  uint64 addr_aligned = addr;
  if(addr > v->vastart) {
    addr_aligned = PGROUNDUP(addr);
  }

  int nunmap = sz - (addr_aligned-addr); // nbytes to unmap
  if(nunmap < 0)
    nunmap = 0;
  
  vmaunmap(p->pagetable, addr_aligned, nunmap, v); // custom memory page unmap routine for mmapped pages.

  if(addr <= v->vastart && addr + sz > v->vastart) { // unmap at the beginning
    v->offset += addr + sz - v->vastart;
    v->vastart = addr + sz;
  }
  v->sz -= sz;

  if(v->sz <= 0) {
    fileclose(v->f);
    v->valid = 0;
  }

  return 0;  
}

```

这里首先通过传入的地址找到对应的 vma 结构体（通过前面定义的 findvma 方法），然后检测了一下在 vma 区域中间“挖洞”释放的错误情况，计算出应该开始释放的内存地址以及应该释放的内存字节数量（由于页有可能不是完整释放，如果 addr 处于一个页的中间，则那个页的后半部分释放，但是前半部分不释放，**此时该页整体不应该被释放**）。

计算出来释放内存页的开始地址以及释放的个数后，调用自定义的 vmaunmap 方法（vm.c）对物理内存页进行释放，并在需要的时候将数据写回磁盘。将该方法独立出来并写到 vm.c 中的理由是方便调用 vm.c 中的 walk 方法。

在调用 vmaunmap 释放内存页之后，对 v->offset、v->vastart 以及 v->sz 作相应的修改，并在所有页释放完毕之后，关闭对文件的引用，并完全释放该 vma。

```cpp
// kernel/vm.c
#include "fcntl.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "proc.h"

// Remove n BYTES (not pages) of vma mappings starting from va. va must be
// page-aligned. The mappings NEED NOT exist.
// Also free the physical memory and write back vma data to disk if necessary.
void
vmaunmap(pagetable_t pagetable, uint64 va, uint64 nbytes, struct vma *v)
{
  uint64 a;
  pte_t *pte;

  // printf("unmapping %d bytes from %p\n",nbytes, va);

  // borrowed from "uvmunmap"
  for(a = va; a < va + nbytes; a += PGSIZE){
    if((pte = walk(pagetable, a, 0)) == 0)
      panic("sys_munmap: walk");
    if(PTE_FLAGS(*pte) == PTE_V)
      panic("sys_munmap: not a leaf");
    if(*pte & PTE_V){
      uint64 pa = PTE2PA(*pte);
      if((*pte & PTE_D) && (v->flags & MAP_SHARED)) { // dirty, need to write back to disk
        begin_op();
        ilock(v->f->ip);
        uint64 aoff = a - v->vastart; // offset relative to the start of memory range
        if(aoff < 0) { // if the first page is not a full 4k page
          writei(v->f->ip, 0, pa + (-aoff), v->offset, PGSIZE + aoff);
        } else if(aoff + PGSIZE > v->sz){  // if the last page is not a full 4k page
          writei(v->f->ip, 0, pa, v->offset + aoff, v->sz - aoff);
        } else { // full 4k pages
          writei(v->f->ip, 0, pa, v->offset + aoff, PGSIZE);
        }
        iunlock(v->f->ip);
        end_op();
      }
      kfree((void*)pa);
      *pte = 0;
    }
  }
}

```

这里的实现大致上和 uvmunmap 相似，查找范围内的每一个页，检测其 dirty bit (D) 是否被设置，如果被设置，则代表该页被修改过，需要将其写回磁盘。注意不是每一个页都需要完整的写回，这里需要处理开头页不完整、结尾页不完整以及中间完整页的情况。

xv6中本身不带有 dirty bit 的宏定义，在 riscv.h 中手动补齐：

```cpp
// kernel/riscv.h

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3) 
#define PTE_U (1L << 4) // 1 -> user can access
#define PTE_G (1L << 5) // global mapping
#define PTE_A (1L << 6) // accessed
#define PTE_D (1L << 7) // dirty


```
最后需要做的，是在 proc.c 中添加处理进程 vma 的各部分代码。

+ 让 allocproc 初始化进程的时候，将 vma 槽都清空
+ freeproc 释放进程时，调用 vmaunmap 将所有 vma 的内存都释放，并在需要的时候写回磁盘
+ fork 时，拷贝父进程的所有 vma，但是不拷贝物理页

```cpp
// kernel/proc.c

static struct proc*
allocproc(void)
{
  // ......

  // Clear VMAs
  for(int i=0;i<NVMA;i++) {
    p->vmas[i].valid = 0;
  }

  return p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
static void
freeproc(struct proc *p)
{
  if(p->trapframe)
    kfree((void*)p->trapframe);
  p->trapframe = 0;
  for(int i = 0; i < NVMA; i++) {
    struct vma *v = &p->vmas[i];
    vmaunmap(p->pagetable, v->vastart, v->sz, v);
  }
  if(p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->parent = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = UNUSED;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
int
fork(void)
{
  // ......

  // copy vmas created by mmap.
  // actual memory page as well as pte will not be copied over.
  for(i = 0; i < NVMA; i++) {
    struct vma *v = &p->vmas[i];
    if(v->valid) {
      np->vmas[i] = *v;
      filedup(v->f);
    }
  }

  safestrcpy(np->name, p->name, sizeof(p->name));

  pid = np->pid;

  np->state = RUNNABLE;

  release(&np->lock);

  return pid;
}


```
由于 mmap 映射的页并不在 [0, p->sz) 范围内，所以其页表项在 fork 的时候并不会被拷贝。我们只拷贝了 vma 项到子进程，这样子进程尝试访问 mmap 页的时候，会重新触发懒加载，重新分配物理页以及建立映射。

链接：https://juejin.cn/post/7022394470419136542

