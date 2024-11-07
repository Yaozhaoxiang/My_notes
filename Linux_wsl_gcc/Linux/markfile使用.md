
原文链接：https://blog.csdn.net/weixin_38391755/article/details/80380786

# 0. 编译和链接
在此，我想多说关于程序编译的一些规范和方法，一般来说，无论是C、++是pas，首先要把源文件编译成中间代码件，在Windows下也就是 obj件，UNIX下是 .o 文件，即 Object File，这个动作叫做（compile）。后再把大量Object File合成执行文件，这个动作叫接（link）。   

编译时，编译器需要的是语法的正确，函数与变量的声明的正确。对于者常是你需要告诉编译器头文件的所在位置（文件中应该只是声明，定应放在C/C++文件中），只要所有的语法正确，编译器就可以编译出间标文件一般来说，每源文件都应该对应于一个中间目标文件（O文件OBJ文件）。 
链接时，主要是链接函数和全局变量，所以，我们可以使用这些中间目文（文件或是OBJ文件）来链接我们的应用序。链接器并不管函数所在源件，管函数的中间目标文件（Object File），在大多数时候，由于文太多，译生成的间目标文件太多，而在链接时需要明显地指出中间标件名，这于编译很不方便，所以，我们要给中间目标文件打个Windows下这种包“库文件”（Library File)，也就是 .lib 文件，在UN下，是ArchiveFile，也就是 .a 文件。

总结一下，源文件首先会生成中间目标文件，再由中间目标文件生成执件。在编译时，编译器只检测程序语法，和数、变量是否被声明。如函未被声明，编译器会给出一个警告，但可以生成Object File。而在接程时，链接器会在有的Object File中找寻函数的实现，如果找到，那到会报链接错误码（Linker Error），在VC下，这种错误一般Link 2001误，意思说是说，链接器未能找到函数的实现。你需要指定数的bjectFile.

```sh
CXX = g++
TARGET = hello
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC)) 
CXXFLAGS = -c -Wall
$(TARGET): $(OBJ)
    $(CXX) -o $@ $^
%.o: %.cpp
    $(CXX) $(CXXFLAGS) $< -o $@
.PHONY: clean  #防止有个文件叫clean
clean:
    rm -f *.o $(TARGET)
    
```

# 一、Makefile 介绍

其次，Makefile的基本语法是：
目标文件：依赖文件
[TAB]指令

=	替换
+=	追加
:=	恒等于

%.o	任意的.o文件
*.o	所有的.o文件

$^	所有依赖文件
$@	所有目标文件
$<	所有依赖文件的第一个文件

make命令执行时，需要一个 Makefile 文件，以告诉make命令需要怎么样的去编译和链接程序。


我们要写一个Makefile来告诉make命令如何编译和链接这几个文件。我们的规则是：
  1.  如果这个工程没有编译过，那么我们的所有C文件都要编译并被链接。
  2.  如果这个工程的某几个C文件被修改，那么我们只编译被修改的C文件，并链接目标程序
  3. . 如果这个工程的头文件被改变了，那么我们需要编译引用了这几个头文件的C文件，并链接目标程序。

## 1.1 Makefile的规则

target... : prerequisites ...
[TAB] command

 target也就是一个目标文件，可以是Object File，也可以是执行文件。还可以是一个标签（Label），对于标签这种特性，在后续的“伪目标”章节中会有叙述。

  prerequisites就是，要生成那个target所需要的文件或是目标。

       command也就是make需要执行的命令。（任意的Shell命令）

这是一个文件的依赖关系，也就是说，target这一个或多个的目标文件依赖于prerequisites中的文件，其生成规则定义在command中。说白一点就是说，prerequisites中如果有一个以上的文件比target文件要新的话，command所定义的命令就会被执行。这就是Makefile的规则。也就是Makefile中最核心的内容。

>$@，$^，$<代表的意义分别是：

 $@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。

在定义好依赖关系后，后续的那一行定义了如何生成目标文件的操作系统命令，一定要以一个Tab键作为开头。记住，make并不管命令是怎么工作的，他只管执行所定义的命令。make会比较targets文件和prerequisites文件的修改日期，如果prerequisites文件的日期要比targets文件的日期要新，或者target不存在的话，那么，make就会执行后续定义的命令。

## 1.2 make是如何工作的

在默认的方式下，也就是我们只输入make命令。那么，
  1. make会在当前目录下找名字叫“Makefile”或“makefile”的文件。
  2. 如果找到，它会找文件中的第一个目标文件（target），在上面的例子中，他会找到“edit”这个文件，并把这个文件作为最终的目标文件。
  3.  如果edit文件不存在，或是edit所依赖的后面的 .o 文件的文件修改时间要比edit这个文件新，那么，他就会执行后面所定义的命令来生成edit这个文件
  4.  如果edit所依赖的.o文件也存在，那么make会在当前文件中找目标为.o文件的依赖性，如果找到则再根据那一个规则生成.o文件。（这有点像一个堆栈的过程）
  5.  当然，你的C文件和H文件是存在的啦，于是make会生成 .o 文件，然后再用 .o 文件声明make的终极任务，也就是执行文件edit了。

   这就是整个make的依赖性，make会一层又一层地去找文件的依赖关系，直到最终编译出第一个目标文件。在找寻的过程中，如果出现错误，比如最后被依赖的文件找不到，那么make就会直接退出，并报错，而对于所定义的命令的错误，或是编译不成功，make根本不理。make只管文件的依赖性，即，如果在我找了依赖关系之后，冒号后面的文件还是不在，那么对不起，我就不工作啦。

通过上述分析，我们知道，像clean这种，没有被第一个目标文件直接或间接关联，那么它后面所定义的命令将不会被自动执行，不过，我们可以显示要make执行。即命令——“make clean”，以此来清除所有的目标文件，以便重编译。

## 1.4 makefile中使用变量

比如，我们声明一个变量，叫objects, OBJECTS, objs, OBJS, obj, 或是 OBJ，反正不管什么啦，只要能够表示obj文件就行了。我们在makefile一开始就这样定义：

    objects = main.o kbd.o command.o display.o \

             insert.o search.o files.o utils.o

于是，我们就可以很方便地在我们的makefile中以“$(objects)”的方式来使用这个变量了，于是我们的改良版makefile就变成下面这个样子：

objects = main.o kbd.o command.o display.o \
             insert.osearch.o files.o utils.o 
   edit : $(objects)
           cc -o edit $(objects)
   main.o : main.c defs.h
           cc -c main.c
   kbd.o : kbd.c defs.h command.h
           cc -c kbd.c
   command.o : command.c defs.h command.h
           cc -c command.c
   display.o : display.c defs.h buffer.h
           cc -c display.c
   insert.o : insert.c defs.h buffer.h
           cc -c insert.c
   search.o : search.c defs.h buffer.h
           cc -c search.c
   files.o : files.c defs.h buffer.h command.h
           cc -c files.c
   utils.o : utils.c defs.h
           cc -c utils.c
   clean :
           rm edit $(objects)

## 1.5 让make自动推导

GNU的make很强大，它可以自动推导文件以及文件依赖关系后面的命令，于是我们就没必要去在每一个[.o]文件后都写上类似的命令，因为，我们的make会自动识别，并自己推导命令。

只要make看到一个[.o]文件，它就会自动的把[.c]文件加在依赖关系中，如果make找到一个whatever.o，那么whatever.c，就会是whatever.o的依赖文件。并且 cc -c whatever.c 也会被推导出来，于是，我们的makefile再也不用写得这么复杂。我们的是新的makefile又出炉了。

 objects = main.o kbd.o command.o display.o \
             insert.o search.o files.o utils.o
 
   edit : $(objects)
           cc -o edit $(objects)
 
   main.o : defs.h
   kbd.o : defs.h command.h
   command.o : defs.h command.h
   display.o : defs.h buffer.h
   insert.o : defs.h buffer.h
   search.o : defs.h buffer.h
   files.o : defs.h buffer.h command.h
   utils.o : defs.h
 
   .PHONY : clean
   clean :
           rm edit $(objects)

这种方法，也就是make的“隐晦规则”。上面文件内容中，“.PHONY”表示，clean是个伪目标文件。










