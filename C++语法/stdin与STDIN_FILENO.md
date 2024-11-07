# `STDIN_FILENO` 和 `stdin`

## 1. 类型区别

`STDIN_FILENO`:
 + 它是一个整数常量，定义为文件描述符 0。
 + 用于表示标准输入文件描述符。
 + 它在 `unistd.h` 头文件中定义
```cpp
#define STDIN_FILENO 0

```

`stdin`:
 + 它是一个指向 FILE 类型的指针，表示标准输入流。
 + 它在 `stdio.h` 头文件中定义，通常用于高级 I/O 函数（如 fscanf, fgets 等）。
  
```cpp
extern FILE *stdin;

```
## 2. 使用场景
STDIN_FILENO:
 + 通常用于系统调用和低级 I/O 操作，例如 read, write, dup2, pipe 等。
 + 直接与文件描述符打交道，非常适合操作系统级别的编程

```cpp
char buffer[100];
ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));

```

stdin:
 + 通常用于标准库函数和高级 I/O 操作，例如 scanf, fgets, fread 等。
 + 更适合处理缓冲区管理、格式化输入等高级操作

```cpp
char buffer[100];
fgets(buffer, sizeof(buffer), stdin);

```
## 3.缓冲区管理
STDIN_FILENO:
 + 不涉及缓冲区管理，操作系统直接从输入设备读取数据并返回。
 + 适合需要精确控制输入/输出时机的场景，例如实时数据处理或设备驱动程序。

stdin:
 + 包含缓冲区管理，输入流通常经过缓冲区处理，以提高 I/O 性能。
 + 适合一般的文本处理、文件操作等场景，标准库会处理缓冲区管理。


# stdout 和 STDOUT_FILENO
stdout 和 STDOUT_FILENO 都表示标准输出，但它们的使用场景和类型有所不同
和上面一样的区别

















