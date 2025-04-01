1. ls：列出当前目录中的文件和子目录
2. pwd：显示当前工作目录的路径
3. cd：切换工作目录
4. mkdir：创建新目录
5. rmdir：删除空目录
6. rm：删除文件或目录
7. cp：复制文件或目录
8. mv：移动或重命名文件或目录

9. touch：创建空文件或更新文件的时间戳
10. cat：连接和显示文件内容

11. more/less：逐页显示文本文件内容
12. head/tail：显示文件的前几行或后几行

13. grep：在文件中搜索指定文本
14. ps：显示当前运行的进程

15. kill：终止进程
16. ifconfig/ip：查看和配置网络接口信息

17. ping：测试与主机的连通性
18. wget/curl：从网络下载文件

19. chmod：修改文件或目录的权限
20. chown：修改文件或目录的所有者

21. tar：用于压缩和解压文件和目录
22. df/du：显示磁盘使用情况
23. mount/umount：挂载和卸载文件系统
24. top/htop：显示系统资源的实时使用情况和进程信息
25. top/htop：显示系统资源的实时使用情况和进程信息

26. ssh：远程登录到其他计算机
27. scp：安全地将文件从本地复制到远程主机，或从远程主机复制到本地

28. find：在文件系统中查找文件和目录
29. grep：在文本中搜索匹配的行，并可以使用正则表达式进行高级搜索
30. echo：将文本输出到标准输出
31. ln：创建硬链接或符号链接
32. uname：显示系统信息
33. shutdown/reboot：关闭或重新启动系统
34. who/w：显示当前登录的用户信息
35. passwd：更改用户密码
36. hostname：显示或设置计算机的主机名
37. free：显示系统内存使用情况
38. sed： 用于对文本进行过滤和转换操作。

```cpp
# 将文件中所有的 "apple" 替换为 "banana"
sed 's/apple/banana/g' input.txt

# 只替换每行中第一个匹配到的 "apple"
sed 's/apple/banana/' input.txt

# 直接修改文件内容
sed -i 's/apple/banana/g' input.txt
```


下载包 ： wget
安装包 ： 
+ 包管理器安装
    + sudo apt install nginx
    + sudo apt remove ngix
+ wget
    + tar -zxvf nginx-1.24.0.tar.gz
    + cd nginx-1.24.0
    + ./configure

查看服务器状态：
+ top 实时显示系统中各个进程的资源占用状况
+ lscpu 显示 CPU 的详细信息
+ vmstat命令


修改文件权限： chmod [三位数字权限码] 文件名
读权限用4表示，写权限用2表示，执行权限用1表示，没有权限用0表示
+ 将文件权限设置为**所有者**有读、写、执行权限，**所属组**有读、执行权限，**其他用户**有读权限
chmod 754 test.txt


+ 将文件权限设置为所有用户都有读、写、执行权限
chmod 777 test.txt


查看linux版本：cat /etc/os-release 

git 查看某行代码作者：git blame 是专门用于查看文件中每一行代码最后一次修改的作者、提交哈希、提交时间等信息的命令。



