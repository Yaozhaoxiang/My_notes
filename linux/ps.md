ps 命令用于报告当前系统的进程状态。可以通过它获取进程的内存使用信息。

```bash
ps -p <pid> -o rss,vsize
```

<pid> 是你要监控的进程 ID。
rss（Resident Set Size）表示进程当前驻留在物理内存中的大小（以千字节为单位）。
vsize（Virtual Size）表示进程的虚拟内存大小（以千字节为单位）。








