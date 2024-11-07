1. cat指令
用于连接文件并将其内容输出到标准输出（通常是终端）
基本语法`cat [选项] [文件...]`
   **常见用法:**
   1. 显示文件内容
   显示单个文件的内容：`cat file.txt`
   显示多个文件的内容，按顺序连接：`cat file1.txt file2.txt`
   2. 重定向输出
   将文件内容复制到另一个文件：`cat file1.txt > file2.txt`
   将多个文件的内容合并到一个文件：`cat file1.txt file2.txt > combined.txt`
   3. 追加内容
   将一个文件的内容追加到另一个文件的末尾：`cat file1.txt >> file2.txt`
   4. 显示行号
   使用 `-n` 选项显示行号：`cat -n file.txt`
   5. 显示不可见字符
   使用 `-v` 选项显示不可见字符（如制表符和换行符）：cat -v file.txt
   6. 显示制表符
   使用 `-T`选项将制表符显示为 ^I:`cat -T file.txt`
   7. 压缩空行
   使用 `-s` 选项压缩连续的空行为一行：`cat -s file.txt`

2. vim指令
Vim 是一种功能强大且广泛使用的文本编辑器，支持多种操作模式和丰富的命令集
   1. 在命令行输入 `vim filename` 打开指定文件，或者直接输入 `vim` 打开一个空白文件。
   2. 退出vim： 
   `:q`：退出。
   `:q!`：不保存退出。
   `:wq` 或 :x：保存并退出。

3. 查看版本信息
`lsb_release -a`
```cpp
Distributor ID: Ubuntu    //类别是ubuntu
Description:    Ubuntu 22.04.3 LTS  //14年2月4月发布的稳定版本，LTS是Long Term Support：长时间支持版本  三年 ，一般是18个月
Release:    22.04  //发行日期或者是发行版本号
Codename:   jammy //ubuntu的代号名称
```
4. 用vscode打开代码
在 Ubuntu 中，在项目文件夹下，输入命令：
`$ code .`
随后就会自动打开 Windows 下的 vscode，并将 Ubuntu 中的项目文件夹作为工作目录，随后，想怎么开发就可以怎么开发了。

5. 返回上一层目录
在命令行中，要返回上一层目录，可以使用 cd 命令结合 .. 来实现。.. 表示上一层目录。以下是具体的步骤：
`cd ..`

6. 删除文件夹  
删除空文件夹:`rmdir directory_name`
删除非空文件夹： `rm -r directory_name`
使用 `sudo` 命令以管理员权限运行 rm 命令。示例：`sudo rm -r csapplab`

7. 下载文件
`wget`是一个用于在 Linux 和 Unix 系统上从网络下载文件的命令行工具。它支持 HTTP、HTTPS 和 FTP 协议，并且可以使用代理服务器。
   1. 下载文件
     `wget http://example.com/file.zip`
     这将从指定的 URL 下载文件到当前目录。
   2. 保存到指定文件名
     `wget -O newname.zip http://example.com/file.zip`
     使用 `-O`选项可以将文件保存为指定的文件名。
   3. 断点续传
     `wget -c http://example.com/file.zip`
     使用 `-c` 选项可以在下载过程中断的情况下继续下载。
   4. 后台下载
     `wget -b http://example.com/file.zip`
     使用 `-b` 选项可以在后台进行下载，下载过程会写入一个名为 `wget-log` 的日志文件中。
   5. 限制下载速度
     `wget --limit-rate=200k http://example.com/file.zip`
     使用 `--limit-rate` 选项可以限制下载速度，例如上面的命令将下载速度限制为每秒 200 KB。
   6. 递归下载 
     `wget -r http://example.com/`
     使用 `-r`选项可以递归下载整个网站。
   7. 镜像网站
     `wget -m http://example.com/`
     使用 `-m` 选项可以镜像网站，这与 `-r` 类似，但会附加一些额外的选项来更好地支持网站镜像。
常用选项
`-q`：安静模式（不输出信息）
`-nv`：不显示下载进度条，但显示其他信息
`-np`：不追踪到父目录
`-R`：排除下载某些文件类型（例如 -R jpg 排除所有 JPG 文件）
`--user` 和 `--password`：用于指定用户名和密码进行 HTTP 基本认证

8. 创建文件夹
在 `Linux` 系统中，可以使用 `mkdir` 命令来创建文件夹。以下是一些常见的用法和示例：
   1. 创建单个目录
    `mkdir dirname` 
    例如，创建一个名为 `new_folder` 的目录：
    ```bat
    mkdir new_folder
    ```
   2. 创建多个目录
     `mkdir dir1 dir2 dir3`
     例如，创建三个目录 `folder1`、`folder2` 和 `folder3`：
     ```bat
     mkdir folder1 folder2 folder3
     ```
   3. 递归创建目录
     如果要创建的目录路径中有不存在的父目录，可以使用 `-p` 选项：
     ```bat
     mkdir -p parent/child/grandchild
     ```
     这将递归地创建 parent、child 和 grandchild 目录，即使它们之前不存在。

9. 编译
   1.  反编译：`objdump -d a.out`
   2.  `gcc -c hello.c` 是用来编译 C 语言源文件 `hello.c` 的命令，但它只会生成目标文件（`.o` 文件），而不会生成可执行文件。
      具体来说，`-c` 选项告诉 `gcc`（GNU Compiler Collection）只进行编译和汇编，而不进行链接。 `-c`：选项指定编译和汇编步骤，生成目标文件（`.o`文件）
   3.  生成可执行文件:可以使用 gcc 不带 -c 选项来编译并链接，生成可执行文件：`gcc hello.c -o hello`.这会生成一个名为 `hello` 的可执行文件。
   4.  如果你已经有了目标文件 hello.o，可以单独进行链接：`gcc hello.o -o hello`
   5.  运行：执行生成的可执行文件`./hello`

10. 查看ELF
    1.  直接输出目标文件：`gcc -c hello.c -o hello.o` 
    2.  使用readelf查看可执行文件的头信息：`readelf -h hello.o`，显示详细的信息`readelf -S`
    3.  使用ldd命令查看程序依赖的动态库:`ldd hello.o`
    4.  使用nm命令查看elf文件的符号表信息:`nm hello.o`
    5.  strings查看可执行文件中的字符串:` strings hello.o`
    6.   Linux下有个最常用的通用命令，来分析任何文件的基本格式，那就是file:`file hello.o`

11. `objdump`:是一个功能强大的工具，用于显示二进制文件（如可执行文件、目标文件和库文件）的详细信息。使用 `objdump` 可以查看二进制文件的各个部分、汇编代码、符号表等。的名字反映了其主要功能：转储和查看（dump）目标文件（object file）的内容
    1. `objdump -h filename` 选项用于显示文件头（header）信息，包括各个段（section）的详细信息。
    2. `objdump -d filename` 是用来反汇编二进制文件（如可执行文件、目标文件或库文件）的命令。这个命令会显示每个函数的机器代码和对应的汇编代码。
    3. 显示符号表：`objdump -t filename`
    4. 同时显示反汇编代码和源代码：`objdump -S filename`
    5. `objdump -s -d hello.o`同时显示：`-s`可以将所有段的内容以十六进制的方式打印出来；
    6. `objdump -r hello.o`查看要重定位的地方
12. `grep` 文本搜索工具，用于在文件或输入流中搜索指定的模式（字符串或正则表达式），并输出匹配的行。
    grep fork sh.c 这个命令在 Unix 或类 Unix 操作系统中用于搜索 sh.c 文件中包含字符串 fork 的所有行，并将这些行输出到终端。
    命令执行步骤
    grep 读取文件 sh.c 的每一行，并搜索包含字符串 fork 的行。
    对于每一行，如果发现包含 fork 字符串，则将该行输出到终端。
13. `wc` 是一个用于计算文件中行数、字数和字符数的命令行工具
    基本用法：
    `wc [OPTION]... [FILE]...`
    -l：打印行数。
    -w：打印单词数。
    -c：打印字节数。
    -m：打印字符数。
    -L：打印最长行的长度（以字符数表示）
    如果不带参数，则输出行数、单词书、字节数
    ```sh
    $ wc example.txt
    3  10 57 example.txt
    ```
    管道和wc
    可以将其他命令的输出通过管道传递给 wc，例如:统计 grep 输出的行数
    ```sh
    $ grep fork sh.c | wc -l
    5
    ```
14. `echo `是一个用于在终端输出文本或变量值的命令行工具
    基本用法：
    `echo [选项] [字符串...]`
    选项：
       -n：在输出文本后不自动换行。
       -e：启用反斜杠转义（解释特殊字符序列）。
       -E：禁用反斜杠转义（这是默认行为）。
    ```sh
    # 基本输出：
    $ echo "Hello, World!"
    Hello, World!

    # 输出多个字符串：
    $ echo Hello World!
    Hello World!

    #输出变量的值
    $ name="Alice"
    $ echo "Hello, $name!"
    Hello, Alice!
    ```
    重定向和`echo`
    echo 可以与重定向符号（>, >>）一起使用，将输出写入文件。
    ```sh
    # 覆盖文件内容
    $ echo "Hello, World!" > example.txt

    # 追加内容到文件
    $ echo "Hello again!" >> example.txt
    ```
    `echo > b`利用 echo 命令和输出重定向来创建或清空一个文件。
15. 硬链接
    是 Unix 和类 Unix 文件系统中的一种文件链接类型，它允许多个文件名指向同一个文件数据块。
    特点：
       指向同一个数据块：硬链接是直接指向存储在硬盘上的相同数据块，所有指向该数据块的硬链接都视为同一个文件。
       共享相同的 inode：硬链接文件共享相同的 inode 号，因此它们是完全相同的文件，而不仅仅是指向同一个文件的数据。
       删除一个硬链接不会影响数据：只要有一个硬链接存在，文件数据就不会被删除。只有当所有指向该数据的硬链接都被删除后，文件数据才会被删除。
       无法跨文件系统：硬链接只能在同一个文件系统内创建，无法跨不同的文件系统。
       不可链接目录：出于安全和文件系统一致性的考虑，硬链接不能用于目录。
    创建硬链接：
    ```sh
    ln existing_file new_link
    ```
    实例：
    ```sh
    # 假设有一个文件 file1.txt：
    echo "This is a file." > file1.txt

    # 创建硬链接 file2.txt 指向 file1.txt：
    ln file1.txt file2.txt

    # 现在，file1.txt 和 file2.txt 都指向相同的数据块：
    $ ls -li
    total 8
    1234567 -rw-r--r-- 2 user group 20 Jun 13 12:00 file1.txt
    1234567 -rw-r--r-- 2 user group 20 Jun 13 12:00 file2.txt
    # 注意：这两个文件具有相同的 inode 号（1234567）和相同的文件大小。

    # 修改 file1.txt 或 file2.txt 中的内容，另一文件的内容也会随之改变，因为它们指向相同的数据块。

    echo "Adding more content." >> file1.txt

    cat file2.txt
    # Output will be:
    # This is a file.
    # Adding more content.
    ```
16. chdir 改变当前工作目录
    它允许进程在不同的目录之间切换
    ```cpp
    #include <unistd.h>

    int chdir(const char *path);
    ```
    path：要切换到的目标目录的路径名。
    成功：返回 0。
    失败：返回 -1，并设置 errno 来指示错误类型。
    path 可以是相对于当前工作目录的相对路径，也可以是绝对路径。
    每个进程都有自己的当前工作目录。chdir 调用只影响调用它的进程，不会影响其他进程
    getcwd：获取当前工作目录。
    mkdir：创建新目录。
    rmdir：删除空目录。
17. find
    用于在目录树中搜索文件和目录
    `find [路径] [条件] [操作]`
      路径：指定搜索的起始目录。可以是绝对路径或相对路径。如果省略，默认从当前目录开始搜索。
      条件：指定搜索文件的条件，如文件名、类型、大小等。
      操作：对找到的文件执行的操作，如删除、打印等。默认操作是打印文件路径。
18. xargs
    从标准输入读取项并将其作为参数传递给指定的命令
19. 查看进程使用的文件描述符
   lsof（List Open Files）是一个非常强大的工具，可以列出系统上所有打开的文件，包括文件描述符。
   `lsof -p 1234`:这个命令将列出进程 1234 所有打开的文件，包括文件描述符。

   2. 使用 ss 或 netstat 命令
    如果你特别关心网络套接字，ss 或 netstat 可以帮助查看进程打开的网络连接对应的文件描述符。
    `ss -p`






```sh
  1 --archive-headers 
  2 -a 
  3 显示档案库的成员信息,类似ls -l将lib*.a的信息列出。 
  4  
  5 -b bfdname 
  6 --target=bfdname 
  7 指定目标码格式。这不是必须的，objdump能自动识别许多格式，比如： 
  8  
  9 objdump -b oasys -m vax -h fu.o 
 10 显示fu.o的头部摘要信息，明确指出该文件是Vax系统下用Oasys编译器生成的目标文件。objdump -i将给出这里可以指定的目标码格式列表。 
 11  
 12 -C 
 13 --demangle 
 14 将底层的符号名解码成用户级名字，除了去掉所开头的下划线之外，还使得C++函数名以可理解的方式显示出来。 
 15  
 16 --debugging 
 17 -g 
 18 显示调试信息。企图解析保存在文件中的调试信息并以C语言的语法显示出来。仅仅支持某些类型的调试信息。有些其他的格式被readelf -w支持。 
 19  
 20 -e 
 21 --debugging-tags 
 22 类似-g选项，但是生成的信息是和ctags工具相兼容的格式。 
 23  
 24 --disassemble 
 25 -d 
 26 从objfile中反汇编那些特定指令机器码的section。 
 27  
 28 -D 
 29 --disassemble-all 
 30 与 -d 类似，但反汇编所有section. 
 31  
 32 --prefix-addresses 
 33 反汇编的时候，显示每一行的完整地址。这是一种比较老的反汇编格式。 
 34  
 35 -EB 
 36 -EL 
 37 --endian={big|little} 
 38 指定目标文件的小端。这个项将影响反汇编出来的指令。在反汇编的文件没描述小端信息的时候用。例如S-records. 
 39  
 40 -f 
 41 --file-headers 
 42 显示objfile中每个文件的整体头部摘要信息。 
 43  
 44 -h 
 45 --section-headers 
 46 --headers 
 47 显示目标文件各个section的头部摘要信息。 
 48  
 49 -H 
 50 --help 
 51 简短的帮助信息。 
 52  
 53 -i 
 54 --info 
 55 显示对于 -b 或者 -m 选项可用的架构和目标格式列表。 
 56  
 57 -j name
 58 --section=name 
 59 仅仅显示指定名称为name的section的信息 
 60  
 61 -l
 62 --line-numbers 
 63 用文件名和行号标注相应的目标代码，仅仅和-d、-D或者-r一起使用使用-ld和使用-d的区别不是很大，在源码级调试的时候有用，要求编译时使用了-g之类的调试编译选项。 
 64  
 65 -m machine 
 66 --architecture=machine 
 67 指定反汇编目标文件时使用的架构，当待反汇编文件本身没描述架构信息的时候(比如S-records)，这个选项很有用。可以用-i选项列出这里能够指定的架构. 
 68  
 69 --reloc 
 70 -r 
 71 显示文件的重定位入口。如果和-d或者-D一起使用，重定位部分以反汇编后的格式显示出来。 
 72  
 73 --dynamic-reloc 
 74 -R 
 75 显示文件的动态重定位入口，仅仅对于动态目标文件意义，比如某些共享库。 
 76  
 77 -s 
 78 --full-contents 
 79 显示指定section的完整内容。默认所有的非空section都会被显示。 
 80  
 81 -S 
 82 --source 
 83 尽可能反汇编出源代码，尤其当编译的时候指定了-g这种调试参数时，效果比较明显。隐含了-d参数。 
 84  
 85 --show-raw-insn 
 86 反汇编的时候，显示每条汇编指令对应的机器码，如不指定--prefix-addresses，这将是缺省选项。 
 87  
 88 --no-show-raw-insn 
 89 反汇编时，不显示汇编指令的机器码，如不指定--prefix-addresses，这将是缺省选项。 
 90  
 91 --start-address=address 
 92 从指定地址开始显示数据，该选项影响-d、-r和-s选项的输出。 
 93  
 94 --stop-address=address 
 95 显示数据直到指定地址为止，该项影响-d、-r和-s选项的输出。 
 96  
 97 -t 
 98 --syms 
 99 显示文件的符号表入口。类似于nm -s提供的信息 
100  
101 -T 
102 --dynamic-syms 
103 显示文件的动态符号表入口，仅仅对动态目标文件意义，比如某些共享库。它显示的信息类似于 nm -D|--dynamic 显示的信息。 
104  
105 -V 
106 --version 
107 版本信息 
108  
109 --all-headers 
110 -x 
111 显示所可用的头信息，包括符号表、重定位入口。-x 等价于-a -f -h -r -t 同时指定。 
112  
113 -z 
114 --disassemble-zeroes 
115 一般反汇编输出将省略大块的零，该选项使得这些零块也被反汇编。 
116  
117 @file 
118 可以将选项集中到一个文件中，然后使用这个@file选项载入。
```

1.  pwd 显示当前路径
print working directory










