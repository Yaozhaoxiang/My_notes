### bmap()

bmap 函数用于将文件中的逻辑块号（bn）映射到磁盘上的实际物理块号。它处理直接块、间接块（单层间接块），并在需要时分配新的块。

**参数**
  `struct inode *ip`: 文件的 inode，包含文件的元数据和数据块地址。
  `uint bn`: 文件中的逻辑块号。

```cpp
static uint
bmap(struct inode *ip, uint bn)
{
  uint addr, *a;
  struct buf *bp;

  if(bn < NDIRECT){  //处理直接块的情况
    if((addr = ip->addrs[bn]) == 0) // 检查 inode 中直接块的地址是否存在。如果不存在，则分配一个新块。
      ip->addrs[bn] = addr = balloc(ip->dev); //如果块地址为空，调用 balloc 为该块分配磁盘块。
    return addr; //返回直接块的地址。
  }
  bn -= NDIRECT;

  if(bn < NINDIRECT){
    // Load indirect block, allocating if necessary.
    if((addr = ip->addrs[NDIRECT]) == 0) //检查 inode 中的间接块地址是否存在。如果不存在，则分配一个新块。
      ip->addrs[NDIRECT] = addr = balloc(ip->dev);
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
    if((addr = a[bn]) == 0){ //如果间接块中的地址为空，调用 balloc 为该块分配磁盘块。
      a[bn] = addr = balloc(ip->dev);
      log_write(bp);
    }
    brelse(bp);
    return addr;
  }

  panic("bmap: out of range");
}
```
ip->addrs[bn] 数组保存了文件的直接块的地址。直接块直接映射到磁盘块。就是文件的直接块的物理地址。

处理单层间接块：ip->addrs[NDIRECT] 存储了间接块的地址。这个间接块本身包含了一系列的块地址，这些块地址又指向实际的数据块。

### readi()

`struct inode *ip`: 指向要读取的文件的 inode。
`int user_dst`: 表示目标地址是否在用户空间（0 为内核空间，1 为用户空间）。
`uint64 dst`: 目标地址，即将数据拷贝到的地址。
`uint off`: 文件中的起始偏移量。
`uint n`: 要读取的字节数

```cpp
int
readi(struct inode *ip, int user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  struct buf *bp;

  if(off > ip->size || off + n < off)
    return 0;
  if(off + n > ip->size)
    n = ip->size - off;

  for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
    bp = bread(ip->dev, bmap(ip, off/BSIZE));//bmap(ip, off / BSIZE) 计算文件偏移量 off 对应的数据块号。
    m = min(n - tot, BSIZE - off%BSIZE); //计算当前读取操作的字节数，取最小值以防止越界。
    if(either_copyout(user_dst, dst, bp->data + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      break;
    }
    brelse(bp);
  }
  return tot;
}
```

### dirlookup()

根据给定的名称查找目录中的条目，并返回指向相应 inode 的指针。
dp: 指向目录 inode 的指针。
name: 要查找的文件名。
poff: 指向 uint 的指针，用于返回目录项的偏移量。

```cpp
struct inode*
dirlookup(struct inode *dp, char *name, uint *poff)
{
  uint off, inum;
  struct dirent de;

  if(dp->type != T_DIR)
    panic("dirlookup not DIR");

  for(off = 0; off < dp->size; off += sizeof(de)){ //遍历目录 inode 的所有数据块。每次处理一个目录项的大小 

    if(readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de)) //使用 readi 从目录 inode 中读取目录项。如果读取失败，程序会触发 panic。
      panic("dirlookup read");
    if(de.inum == 0) //如果目录项的 inum 为 0，表示该目录项为空，跳过
      continue;
    if(namecmp(name, de.name) == 0){
      // entry matches path element
      if(poff)
        *poff = off;
      inum = de.inum;
      return iget(dp->dev, inum);
    }
  }

  return 0;
}
```

### sys_link
用于创建一个硬链接。硬链接使得一个文件可以有多个名字，并且这些名字都指向同一个 inode。这种机制允许你在文件系统中用不同的名字引用同一个文件。


old: 要创建硬链接的现有文件的路径。
new: 硬链接的新路径。
```cpp
uint64
sys_link(void)
{
  char name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  struct inode *dp, *ip;

  if(argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    return -1;

  begin_op(); //开始一个文件系统操作事务，确保操作的原子性。
  if((ip = namei(old)) == 0){ //使用 namei 查找 old 文件的 inode。
    end_op();
    return -1;
  }

  ilock(ip);//锁定 inode ip。检查 ip 是否为目录 (T_DIR)。如果是目录，则不能创建硬链接，解锁并释放 inode，结束操作并返回 -1。
  if(ip->type == T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }

  ip->nlink++; //增加 inode 的链接计数 nlink。调用 iupdate 更新 inode 的信息，然后解锁 inode。
  iupdate(ip);
  iunlock(ip);

  if((dp = nameiparent(new, name)) == 0)//使用 nameiparent 查找 new 路径的父目录，并获取父目录的 inode dp。如果失败，跳转到错误处理。
    goto bad;
  ilock(dp);//锁定父目录 dp。检查父目录和文件是否在同一设备上。使用 dirlink 函数在父目录中添加硬链接。如果操作失败，解锁并释放父目录，然后跳转到错误处理。成功创建硬链接后，解锁父目录并释放 ip。
  if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  return 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  return -1;
}

```

### dirlink
dirlink 函数用于在目录中创建一个新的目录项（directory entry），该目录项将指定的文件（由 inum 表示）与给定的名字 name 关联。以下是对 dirlink 函数的详细分析：

dp: 目录 inode，表示要在其中添加新目录项的目录。
name: 要创建的目录项的名称。
inum: 要关联到目录项的文件 inode 号。
```cpp
int
dirlink(struct inode *dp, char *name, uint inum)
{
  int off;
  struct dirent de;
  struct inode *ip;

  // Check that name is not present.
  if((ip = dirlookup(dp, name, 0)) != 0){
    iput(ip);
    return -1;
  }

  // Look for an empty dirent.
  for(off = 0; off < dp->size; off += sizeof(de)){
    if(readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
      panic("dirlink read");
    if(de.inum == 0)
      break;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  if(writei(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de)) //使用 writei 函数将新的目录项写入目录。如果写入失败，则触发 panic。
    panic("dirlink");

  return 0;
}
```

### create 函数
用于在指定路径 path 下创建一个新的文件或目录，并返回指向该 inode 的指针。

```cpp
static struct inode*
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  // 获取路径的父目录和名称
  if((dp = nameiparent(path, name)) == 0)
    return 0;

  ilock(dp);  // 锁定父目录 inode

  // 检查是否已存在同名文件或目录
  if((ip = dirlookup(dp, name, 0)) != 0){
    iunlockput(dp);  // 解锁并释放父目录 inode
    ilock(ip);  // 锁定找到的 inode
    if(type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      return ip;  // 如果是文件且类型匹配，返回现有的 inode
    iunlockput(ip);  // 解锁并释放 inode
    return 0;
  }

  // 分配新的 inode
  if((ip = ialloc(dp->dev, type)) == 0)
    panic("create: ialloc");  // 分配失败则引发 panic

  ilock(ip);  // 锁定新分配的 inode
  ip->major = major;  // 设置设备号（适用于设备文件）
  ip->minor = minor;  // 设置次设备号（适用于设备文件）
  ip->nlink = 1;  // 设置链接计数
  iupdate(ip);  // 更新 inode

  // 如果创建的是目录，则需要设置目录项 ". " 和 ".."
  if(type == T_DIR){
    dp->nlink++;  // 更新父目录的链接计数
    iupdate(dp);
    // 创建 "." 和 ".." 目录项
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      panic("create dots");  // 如果创建失败则引发 panic
  }

  // 在父目录中创建目录项
  if(dirlink(dp, name, ip->inum) < 0)
    panic("create: dirlink");  // 如果创建失败则引发 panic

  iunlockput(dp);  // 解锁并释放父目录 inode

  return ip;  // 返回新创建的 inode
}

```

### sys_open(void)
sys_open 是用于打开文件或设备的系统调用。它处理各种打开模式，检查文件/设备的有效性，并将打开的文件描述符返回给用户进程

```cpp
int
sys_open(void)
{
  char path[MAXPATH];
  int fd, omode;
  struct file *f;
  struct inode *ip;
  int n;

  // 获取系统调用的参数
  if((n = argstr(0, path, MAXPATH)) < 0 || argint(1, &omode) < 0)
    return -1;

  begin_op();  // 开始一个事务

  // 如果是创建模式，则调用 create 函数创建文件
  if(omode & O_CREATE){
    ip = create(path, T_FILE, 0, 0);  // 创建文件
    if(ip == 0){
      end_op();  // 结束事务
      return -1;
    }
  } else {
    // 否则，查找现有文件
    if((ip = namei(path)) == 0){
      end_op();  // 结束事务
      return -1;
    }
    ilock(ip);  // 锁定 inode
    // 如果是目录且打开模式不是只读模式，则返回错误
    if(ip->type == T_DIR && omode != O_RDONLY){
      iunlockput(ip);  // 解锁并释放 inode
      end_op();  // 结束事务
      return -1;
    }
  }

  // 检查设备类型
  if(ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)){
    iunlockput(ip);  // 解锁并释放 inode
    end_op();  // 结束事务
    return -1;
  }

  // 分配文件结构体和文件描述符
  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);  // 释放文件结构体
    iunlockput(ip);  // 解锁并释放 inode
    end_op();  // 结束事务
    return -1;
  }

  // 设置文件结构体的类型和权限
  if(ip->type == T_DEVICE){
    f->type = FD_DEVICE;
    f->major = ip->major;
  } else {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);  // 读取权限
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);  // 写入权限

  // 如果打开模式包括 O_TRUNC，则截断文件
  if((omode & O_TRUNC) && ip->type == T_FILE){
    itrunc(ip);
  }

  iunlock(ip);  // 解锁 inode
  end_op();  // 结束事务

  return fd;  // 返回文件描述符
}

```





