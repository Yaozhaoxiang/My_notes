在TCP/IP中的makefile迭代

# 1.
```sh
# Makefile
# 变量定义
CC = gcc
CFLAGS = -Wall -g

TARGET = hserver hclient tclient tserver eclient eserver \
		 oserver oclient fserver userver uclient

# 默认目标
all: $(TARGET)

# 生成 server 可执行文件
hserver: hello_server.c
	$(CC) $(CFLAGS) hello_server.c -o hserver
# 生成 client 可执行文件
hclient: hello_client.c
	$(CC) $(CFLAGS) hello_client.c -o hclient
tclient: tcp_client.c
	$(CC) $(CFLAGS) tcp_client.c -o tclient
tserver: tcp_server.c
	$(CC) $(CFLAGS) tcp_server.c -o tserver 
eclient: echo_client.c
	$(CC) $(CFLAGS) echo_client.c -o eclient
eserver : echo_server.c
	$(CC) $(CFLAGS) echo_server.c -o eserver 
oserver : op_server.c
	$(CC) $(CFLAGS) op_server.c -o oserver
oclient : op_client.c
	$(CC) $(CFLAGS) op_client.c -o oclient
fserver : file_server.c
	$(CC) $(CFLAGS) file_server.c -o fserver
userver : uecho_server.c
	$(CC) $(CFLAGS) uecho_server.c -o userver
uclient : uecho_client.c
	$(CC) $(CFLAGS) uecho_client.c -o uclient

# 清理目标
.PHONY: clean
clean:
	rm -f $(TARGET)
```

# 2.

1. 目标文件列表：
   使用 TARGETS 变量存储所有目标文件的名称。
2. 源文件列表：
   使用 SRCS 变量存储所有源文件的名称。这个列表不仅是为了参考，实际在规则中并未使用，但可以用于其他目的，比如自动化生成规则或依赖关系。
3. 自动生成规则：
   使用模式规则 $(TARGETS): %: %.c，这样可以为每个目标自动生成编译规则。
   $< 是第一个依赖文件的自动变量，这里指的是源文件。
   $@ 是规则中的目标文件。
4. 默认目标：
   all 目标会编译所有指定的目标文件。

如果希望目标文件和源文件之间的名字完全相等（即没有前缀或后缀的变化），那么 SRCS 变量就不再需要，因为可以直接通过目标文件的名字来推导出源文件的名字。
```sh
# Makefile
# 变量定义
CC = gcc
CFLAGS = -Wall -g

# 目标文件列表
TARGETS = hserver hclient tclient tserver eclient eserver oserver oclient fserver userver uclient

# 源文件对应的目标
SRCS = hello_server.c hello_client.c tcp_client.c tcp_server.c \
       echo_client.c echo_server.c op_server.c op_client.c \
       file_server.c uecho_server.c uecho_client.c

# 自动生成目标的规则
# 这里假设每个目标文件的名字与源文件名字前缀相同
$(TARGETS): %: %.c
	$(CC) $(CFLAGS) $< -o $@

# 默认目标
all: $(TARGETS)

# 清理目标
.PHONY: clean
clean:
	rm -f $(TARGETS)

```

# 3 不同文件夹下的文件，加入makefile
```sh
# 编译器
CC = g++
# 编译选项
CFLAGS = -Wall -g
# 宏定义
DEFINE = -D_REENTRANT
# 线程库
LIBS = -pthread
# 可执行文件
TARGET = chat_clnt thread1 chat_server

# 文件夹路径
SRC_DIR1 = src/client
SRC_DIR2 = src/server
SRC_DIR3 = src/threading

# 默认目标：编译所有可执行文件
all: $(TARGET)

chat_clnt : $(SRC_DIR1)/chat_clnt.cpp
	$(CC) $(CFLAGS) $(SRC_DIR1)/chat_clnt.cpp -o chat_clnt $(LIBS)

thread1 : $(SRC_DIR3)/thread1.cpp
	$(CC) $(CFLAGS) $(DEFINE) $(SRC_DIR3)/thread1.cpp -o thread1 $(LIBS)

chat_server : $(SRC_DIR2)/chat_server.cpp
	$(CC) $(CFLAGS) $(DEFINE) $(SRC_DIR2)/chat_server.cpp -o chat_server $(LIBS)

# 清理目标
.PHONY: clean
clean:
	rm -f $(TARGET)

```


# 4.
```sh
# Makefile

CC = g++
CFLAGS = -Wall -g -std=c++17 # 开启警告信息、生成调试信息、使用c++17标准

SRCDIR = src # 存放源代码 (.cpp 文件) 的目录
OBJDIR = obj # 存放目标文件 (.o 文件) 的目录。
INCLUDEDIR = include # 存放头文件 (.h 文件) 的目录。

SOURCES = $(wildcard $(SRCDIR)/*.cpp) # 找到所有源文件：
# $(wildcard $(SRCDIR)/*.cpp)：这部分代码使用 wildcard 函数来查找 src 目录中所有 .cpp 文件，并将它们存储在 SOURCES 变量中。例如，如果 src 目录下有 main.cpp 和 http_conn.cpp，那么 SOURCES 的值就会是 src/main.cpp src/http_conn.cpp。#

OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
# patsubst 是 Makefile 的模式替换函数。
# 它将所有 .cpp 文件的路径替换成相应的 .o 文件路径。
# $(SRCDIR)/%.cpp 表示源文件路径模式，如 src/main.cpp。
# $(OBJDIR)/%.o 表示目标文件路径模式，如 obj/main.o。
# 结果是将 src/main.cpp 转换成 obj/main.o，生成目标文件列表。

EXECUTABLE = project # 变量定义了最终生成的可执行文件名。

all: $(EXECUTABLE) # all 目标是默认目标，当运行 make 时，如果没有指定其他目标，默认会编译生成 EXECUTABLE。

$(EXECUTABLE): $(OBJECTS) # 这个规则定义了如何从目标文件生成可执行文件。
	$(CC) $(CFLAGS) -o $@ $^
# $@ 表示目标文件名（即 myproject）
# $^ 表示所有依赖的目标文件（即 http_conn.o 和 main.o）。

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
    $(CC) $(CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# $(OBJDIR)/%.o：目标文件路径模式，例如 obj/main.o。
# $(SRCDIR)/%.cpp：依赖文件，即源代码文件路径，例如 src/main.cpp。
# $(CC)：编译器（这里是 g++）。
# $(CFLAGS)：编译选项（比如 -Wall -g -std=c++17）。
# -I$(INCLUDEDIR)：指定头文件的搜索路径为 include 目录。
# -c $<：$< 是第一个依赖文件（源文件 .cpp），表示要编译的源文件。
# -o $@：$@ 表示目标文件（.o 文件），生成的目标文件放在 obj 目录中
# -c 表示只编译，不链接。

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
```


















