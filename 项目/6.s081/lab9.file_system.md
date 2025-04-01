关于文件描述符和inode：
https://blog.csdn.net/wwwlyj123321/article/details/100298377

# Large files (moderate)

在这个作业中，你将增加 xv6 文件的最大大小。目前，xv6 文件的大小限制为 268 个块，即 268*BSIZE 字节（在 xv6 中，BSIZE 为 1024）。这个限制源于 xv6 的 inode 包含 12 个“直接”块号和一个“单级间接”块号。该单级间接块号指向一个块，这个块可以存储多达 256 个更多的块号，因此总共是 12+256=268 个块。

测试失败是因为 bigfile 期望能够创建一个包含 65803 个块的文件，但未修改的 xv6 将文件大小限制为 268 个块。

你需要修改 xv6 文件系统代码，以支持每个 **inode 中包含一个“双级间接”块**，该块包含 256 个“单级间接”块的地址，每个单级间接块可以包含最多 256 个数据块的地址。这样，文件最多可以包含 65803 个块，即 256*256+256+11 个块（11 个而不是 12 个，因为我们将牺牲一个直接块号用于双级间接块）。

mkfs 程序创建 xv6 文件系统的磁盘镜像，并确定文件系统的总块数；这个大小由 kernel/param.h 中的 FSSIZE 控制。你会看到本实验的代码库中 FSSIZE 设置为 200,000 个块。你应该在 make 输出中看到 mkfs/mkfs 的以下输出：

这一行描述了 mkfs/mkfs 构建的文件系统：它有 70 个元数据块（用于描述文件系统的块）和 199,930 个数据块，总共 200,000 个块。如果在实验过程中你发现需要从头重建文件系统，你可以运行 make clean，这会强制 make 重新构建 fs.img。

磁盘上 inode 的格式由 fs.h 中的 struct dinode 定义。你特别关注 NDIRECT、NINDIRECT、MAXFILE 和 struct dinode 中的 addrs[] 元素。请查看 xv6 文档中的图 8.3，以获取标准 xv6 inode 的示意图。

查找文件数据的代码位于 fs.c 中的 bmap() 函数。查看它并确保理解其功能。bmap() 在读取和写入文件时都会被调用。在写入时，bmap() 会根据需要分配新的块以存储文件内容，同时如果需要，还会分配一个间接块来存储块地址。

bmap() 处理两种类型的块号。bn 参数是一个“逻辑块号”——相对于文件起始位置的文件内块号。ip->addrs[] 中的块号以及 bread() 的参数是磁盘块号。你可以将 bmap() 看作是将文件的逻辑块号映射到磁盘块号的过程。

>修改 bmap() 以实现一个双级间接块，除了直接块和单级间接块之外。你需要将直接块的数量减少到 11 个，而不是 12 个，以腾出空间用于新的双级间接块；你不能更改磁盘上 inode 的大小。ip->addrs[] 的前 11 个元素应为直接块，第 12 个应为单级间接块（就像现在的块一样），第 13 个应为新的双级间接块。当 bigfile 能够写入 65803 个块且 usertests 成功运行时，你就完成了这个任

Hints:

确保你理解 bmap()。绘制出 ip->addrs[]、间接块、双级间接块以及它所指向的单级间接块和数据块之间的关系图。理解为什么添加一个双级间接块会使最大文件大小增加 256*256 块（实际上是 -1，因为你必须将直接块的数量减少一个）。

考虑一下如何用逻辑块号来索引双级间接块以及它所指向的间接块。

如果你更改了 NDIRECT 的定义，你可能需要在 file.h 中更改 struct inode 中的 addrs[] 的声明。确保 struct inode 和 struct dinode 中的 addrs[] 数组具有相同数量的元素。

如果更改了 NDIRECT 的定义，确保创建一个新的 fs.img，因为 mkfs 使用 NDIRECT 来构建文件系统。

如果你的文件系统出现了不良状态，例如崩溃，从 Unix 删除 fs.img（不要从 xv6 删除）。make 将为你构建一个新的干净的文件系统镜像。

不要忘记对每个你读取的块调用 brelse()。

你应该像原始的 bmap() 一样，仅在需要时分配间接块和双级间接块。

确保 itrunc 能够释放文件的所有块，包括双级间接块。

## 解

注意：每个 bread() 调用后都应使用 brelse() 释放缓存块。为什么？
  brelse() 函数的作用是释放之前用 bread() 或其他类似函数（如 bwrite()）分配的缓存块。当你不再需要对这个块进行操作时，应该调用 brelse() 来释放它，这样缓冲区就可以被其他操作重新使用。

  1. 内存管理：缓冲区的数量是有限的，使用 brelse() 可以确保你不占用过多的内存资源。每个被读取的块都需要在内存中保留一个 buf 结构体，释放它可以避免内存泄漏。
  2. 缓冲区重用：操作系统使用缓冲区池来管理内存中的块。当一个块被释放时，它可以被重新分配给其他操作。这样，brelse() 使得缓冲区可以有效地重用，提高系统的效率。
  3. 数据一致性：如果你在处理一个块时没有释放它，可能会导致数据一致性问题，尤其是在多线程或多进程环境中。释放块确保所有对该块的修改都已完成，并且其他操作可以安全地访问或修改相同的块。
  4. 防止死锁：如果不释放缓冲区，可能会导致死锁或资源耗尽问题。brelse() 的调用可以避免这种情况，保持系统的稳定性

### 结构体
```cpp
//fs.h
#define NDIRECT 11
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT + NINDIRECT*NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEVICE only)
  short minor;          // Minor device number (T_DEVICE only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+2];   // Data block addresses
};
```
```cpp
//file.h
// in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+2];
};
```

### bmap()
```cpp
static uint
bmap(struct inode *ip, uint bn)
{
  uint addr, *a, *a2;
  uint addr2;
  struct buf *bp, *bp2;
  int n;

  if(bn < NDIRECT){
    if((addr = ip->addrs[bn]) == 0){
      ip->addrs[bn] = addr = balloc(ip->dev);
    }
    return addr;
  }
  bn -= NDIRECT;

  if(bn < NINDIRECT){ //12是单级间接块
    // Load indirect block, allocating if necessary.
    if((addr = ip->addrs[NDIRECT]) == 0)
      ip->addrs[NDIRECT] = addr = balloc(ip->dev);
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
    if((addr = a[bn]) == 0){
      a[bn] = addr = balloc(ip->dev);
      log_write(bp);
    }
    brelse(bp);
    return addr;
  }

bn -= NINDIRECT;
n = bn/NINDIRECT; //bn属于哪一个一级间接块,即下标

if(bn < NINDIRECT*NINDIRECT) //13双级间接块
{
    if((addr = ip->addrs[NDIRECT+1]) == 0) //找到二级间接块的地址
    {
        ip->addrs[NDIRECT+1] = addr = balloc(ip->dev);
    }
    //读取二级间接块的内容
    bp = bread(ip->dev, addr); //有256个，每一个都是一个一级间接块
    a = (uint*)bp->data;
    //读取对应一级间接块
    if((addr2 = a[n])==0){
        a[n] =addr2 = balloc(ip->dev);
        log_write(bp);
    }
    brelse(bp);
    
    bp2 = bread(ip->dev, addr2);// 读取一级间接块
    a2 = (uint*)bp2->data;
    if((addr2 = a2[bn%NINDIRECT]) == 0){
        a2[bn%NINDIRECT] = addr2 = balloc(ip->dev);
        log_write(bp2);
    }
    brelse(bp2);
    return addr2;
}

  panic("bmap: out of range");
}
```
### itrunc()
```cpp
void
itrunc(struct inode *ip)
{
  int i, j;
  struct buf *bp, *bp2;
  uint *a, *a2;

  for(i = 0; i < NDIRECT; i++){
    if(ip->addrs[i]){
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  if(ip->addrs[NDIRECT]){
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint*)bp->data;
    for(j = 0; j < NINDIRECT; j++){
      if(a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

if(ip->addrs[NDIRECT+1])
{
    bp = bread(ip->dev, ip->addrs[NDIRECT+1]);
    a = (uint*)bp->data;
    for (i = 0; i < NINDIRECT; i++) {
        if(a[i])
        {
            bp2 = bread(ip->dev, a[i]);
            a2 = (uint*)bp2->data;
            for(j = 0; j < NINDIRECT; j++){
                if(a2[j])
                    bfree(ip->dev, a2[j]);
            }
            brelse(bp2);
            bfree(ip->dev, a[i]);
        }
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT+1]);
    ip->addrs[NDIRECT+1] = 0;
}

  ip->size = 0;
  iupdate(ip);

}
```
### 遇到的问题

1. panic: log_write outside of trans

没有进行事务处理的上下文中调用了 log_write()。

在处理文件系统的块时，需要确保 log_write() 只在事务上下文中调用。通常，这意味着你需要确保在调用 log_write() 之前，事务已经开始并且在完成操作后提交或回滚事务。

发现是忘记加`log_write(bp);`了；确保 bread() 和 brelse() 的使用：在涉及间接块的情况下，你需要读取缓冲区并在修改后调用 log_write() 记录修改。
  
2. bigfile: read the wrong data (65802) for block 267
读取文件的时候，不能正常的读取二级间接块；
发现是条件写错了；
```cpp
    if((addr2 = a2[bn%NINDIRECT]) == 0){
        a2[bn%NINDIRECT] = addr2 = balloc(ip->dev);
        log_write(bp2);
    }
```
忘记加（）了，导致addr2==0,不能正常读取二级块

# Symbolic links (moderate)

在这个练习中，你将向 xv6 添加符号链接（或**软链接**）。**符号链接通过路径名引用一个链接文件；当打开符号链接时，内核会跟随链接到所引用的文件**。符号链接类似于硬链接，但硬链接限制只能指向同一磁盘上的文件，而符号链接可以跨磁盘设备。虽然 xv6 不支持多个设备，但实现这个系统调用是一个很好的练习，有助于理解路径名查找的工作原理。

>你将实现 symlink(char *target, char *path) **系统调用**调用**，该调用在 path 处创建一个新的符号链接，指向由 target 命名的文件。有关更多信息，请参见 symlink 的手册页。为了测试，请将 symlinktest 添加到 Makefile 中并运行它。当测试输出以下结果（包括 usertests 成功）时，你的解决方案就完成了

Hints:
  首先，为 symlink 创建一个新的系统调用编号，并在 user/usys.pl、user/user.h 中添加一个条目，同时在 kernel/sysfile.c 中实现一个空的 sys_symlink 函数。

  在 kernel/stat.h 中添加一个新的文件类型 (T_SYMLINK)，用于表示符号链接。

  在 kernel/fcntl.h 中添加一个新的标志 (O_NOFOLLOW)，可以与 open 系统调用一起使用。注意，传递给 open 的标志是使用按位 OR 运算符组合的，因此你的新标志不应与现有标志重叠。这将使你能够在将其添加到 Makefile 后编译 user/symlinktest.c。

  实现 symlink(target, path) 系统调用，在 path 处创建一个新的符号链接，指向 target。注意，target 不需要存在以使系统调用成功。你需要选择一个地方存储符号链接的目标路径，例如，在 inode 的数据块中。symlink 应返回一个整数，表示成功（0）或失败（-1），类似于 link 和 unlink。

  修改 open 系统**调用以处理路径引用符号链接**的情况。如果文件不存在，open 必须失败。当进程在 open 的标志中指定 O_NOFOLLOW 时，open 应打开符号链接（而不是跟随符号链接）。 

  如果链接的文件也是符号链接，你必须递归地跟随它，直到达到非链接文件。如果链接形成一个循环，你必须返回一个错误代码。你可以通过在链接深度达到某个阈值（例如 10）时返回错误代码来近似处理。

  其他系统调用（例如 link 和 unlink）不能跟随符号链接；这些系统调用操作的是符号链接本身。

  本实验中不需要处理符号链接到目录的情况。



### 解

硬链接的过程，书中有讲，大致流程：

+ name 数组用于存储新路径的最后一个文件名部分
+ new 和 old 数组分别用于存储新路径和旧路径
+ dp 和 ip 是指向索引节点（inode）的指针，dp 用于指向新路径的父目录的 inode，ip 用于指向旧文件的 inode。

1. 查找旧路径对应的 inode，并将其指针赋值给 ip
2. 检查旧文件的类型是否为目录，如果是目录，则不允许为目录创建硬链接
3. 增加旧文件的链接计数
4. 查找新路径的父目录的 inode，并将其指针赋值给 dp，同时将新路径的最后一个文件名部分存储在 name 数组中。
5. 创建新的硬链接，在父目录 dp 中创建一个新的目录项，该目录项指向旧文件的 inode 编号 ip->inum，文件名是 name。



实现系统调用：

主要函数：
创建一个 inode，这个inode的类型是 T_SYMLINK，然后把路径写道数据块中；

```cpp
uint64
sys_symlink(void)
{
    char target[MAXPATH], path[MAXPATH];
    struct inode *ip;
    // 从用户空间获取目标路径和符号链接路径
    if(argstr(0, target, MAXPATH) < 0 || argstr(1, path, MAXPATH) < 0)
        return -1;

    begin_op();

    if((ip=create(path, T_SYMLINK, 0, 0))==0)
    {
        end_op();
        return -1;
    }

    // 将目标路径写入符号链接的 inode 数据块
    if(writei(ip, 0, (uint64)target, 0, strlen(target) + 1) != strlen(target) + 1){
        end_op();
        return -1;
    }
    //解锁,因为create返回一个已锁定的 `inode`
    iunlockput(ip);

    end_op();
    return 00;
}
```


### sys_open(void):
需要在打开文件时处理符号链接的解析，并处理 O_NOFOLLOW 标志以控制是否跟随符号链接。

在 sys_open 中，需要检查 O_NOFOLLOW 标志。如果设置了这个标志，则在打开符号链接时，不应跟随符号链接，而是直接打开符号链接本身。

```cpp
 ....
if(omode & O_CREATE){
    ip = create(path, T_FILE, 0, 0);
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {
    if((ip = namei(path)) == 0){
      end_op();
      return -1;
    }
    ilock(ip);
    if(ip->type == T_DIR && omode != O_RDONLY){
      iunlockput(ip);
      end_op();
      return -1;
    }
    int depth=0;
    while(ip->type==T_SYMLINK)
    {
        if(++depth > 10)
        {
            iunlockput(ip);
            end_op();
            return -1;
        }
        //读取符号链接的目标路径
        char link_target[MAXPATH];
        if(readi(ip, 0, (uint64)link_target, 0, MAXPATH)<0)
        {
            iunlockput(ip);
            end_op();
            return -1;
        }
        // 如果设置了 O_NOFOLLOW 标志
        if(omode & O_NOFOLLOW)
        {
            iunlockput(ip);
            end_op();
            return -1;
        }

        //查找符号链接指向的实际文件
        iunlockput(ip);
        if((ip=namei(link_target))==0)
        {
            end_op();
            return -1;
        }
        ilock(ip);
    }
  }
  ....
```

![](./图片/lab9res.png)





# 总结
```cpp
struct dirent {
ushort inum;
char name[DIRSIZ];
};

struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+1];
};

```
读取文件的过程：
  不管是目录还是文件，都有自己的`dirent`,可以通过`inum`来找到自己的`inode`；
  假如要读取文件，则调用`readi`来读取内容就行了，只不过目录和文件包含的内容是不一样的，文件包含的就是文件内容；
  目录包含的是多个`dirent`，因为目录下面就是各个文件名；

  `readi`读取`inode`,通过调用`bread`读取想要扇区的内容:
  `bp = bread(ip->dev, bmap(ip, off/BSIZE));`
  off/BSIZE就是扇区号，bmap找到扇区保存的内容；

整个实验就是围绕着读取文件和目录，很有条理性 



复习：
acquiresleep 是一个阻塞的锁获取函数，它会等待直到块的锁可用;
acquiresleep 确保对块的操作是线程安全的。在多线程环境中，不同线程可能同时尝试访问或修改相同的块，因此需要保证只有一个线程能够访问或修改块，其他线程需要等待。

旋锁（Spinlock）和睡眠锁（Sleep Lock，通常也称为阻塞锁）在锁的实现和使用上有显著的不同。以下是它们的主要区别：

自旋锁:自旋锁的线程在等待锁释放时会忙等待（即不断地轮询检查锁的状态），而不是进入休眠状态。适合短时间的锁定，因为它避免了线程上下文切换的开销.

```cpp
// 自旋锁实现示例
void acquire_spinlock(spinlock_t *lock) {
    while (__sync_lock_test_and_set(&lock->locked, 1)) {
        // 自旋等待
    }
}

void release_spinlock(spinlock_t *lock) {
    __sync_lock_release(&lock->locked);
}

```

睡眠锁:睡眠锁会使线程在等待锁释放时进入休眠状态，而不是忙等待。这意味着线程会被挂起并由操作系统调度其他线程运行。适合长时间持有的锁，因为线程不会持续占用CPU资源。

```cpp
// 睡眠锁实现示例
void acquire_sleep_lock(sleep_lock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->locked) {
        pthread_cond_wait(&lock->cond, &lock->mutex);
    }
    lock->locked = 1;
    pthread_mutex_unlock(&lock->mutex);
}

void release_sleep_lock(sleep_lock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->locked = 0;
    pthread_cond_signal(&lock->cond);
    pthread_mutex_unlock(&lock->mutex);
}

```


