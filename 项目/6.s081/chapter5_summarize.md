## 控制台的输入
### 1. 关于`scause`寄存器

在第四章知道了，执行一个陷阱时会设置`scause`寄存器，设置为反映陷阱原因的值，分析一下全过程：

scause 寄存器在发生异常或中断时，由**硬件自动赋值**。这个过程由处理器的异常和中断处理机制来完成

`scause`寄存器的格式：
  scause 是一个 64 位的寄存器，其中每一位或每几位表示特定的信息：
  位 63：中断标志位 (Interrupt)
  +  1 表示中断
  +  0 表示异常
  位 62-0：中断或异常代码 (Exception Code or Interrupt Code)
`scause` 的定义
  中断类型
  如果位 63 为 1，表示是中断，低 62-0 位的值表示中断原因。常见的中断类型包括：
  + 1: 软件中断 (Supervisor Software Interrupt)
  + 5: 计时器中断 (Supervisor Timer Interrupt)
  + 9: 外部中断 (Supervisor External Interrupt)
  异常类型
  如果位 63 为 0，表示是异常，低 62-0 位的值表示异常原因。常见的异常类型包括：
  + 0: 用户环境调用 (Instruction Address Misaligned)
  + 1: 指令地址对齐错误 (Instruction Address Misaligned)
  + 2: 非法指令 (Illegal Instruction)
  + 5: 环境调用 (Environment Call from U-mode)
  + 8: 指令页错误 (Instruction Page Fault)
  + 12: 负载页错误 (Load Page Fault)
  + 15: 存储/AMO 页错误 (Store/AMO Page Fault)
使用实例：
```cpp
void usertrap(void) {
  uint64 scause = r_scause();  // 读取 scause 寄存器的值

  if (scause & 0x8000000000000000L) {  // 检查是否为中断
    // 处理中断
    if ((scause & 0xff) == 9) {  // 检查是否为外部中断
      // 处理外部中断
    }
  } else {
    // 处理异常
    if ((scause & 0xff) == 15) {  // 检查是否为页错误
      // 处理页错误
    }
  }
}
```

`scause` 寄存器在发生异常或中断时，由硬件自动赋值。这个过程由处理器的异常和中断处理机制来完成。具体来说，当处理器在执行过程中遇到异常或中断事件时，它会自动执行以下操作：
  1. 保存当前状态：处理器保存当前的程序计数器（PC）等状态信息，以便稍后可以恢复程序的正常执行。
  2. 设置 scause 寄存器：处理器将异常或中断的原因编码写入 scause 寄存器。
  3. 跳转到异常或中断处理程序：处理器根据异常或中断类型，跳转到对应的异常或中断处理程序（通常是在操作系统内核中定义的处理函数）。
具体的复制过程：
当发生异常或中断时，处理器会按照以下规则对 scause 寄存器进行赋值：
  1. 确定是中断还是异常：
   + 如果是中断，则将 scause 寄存器的最高位（位 63）设置为 1。
   + 如果是异常，则将 scause 寄存器的最高位（位 63）设置为 0。
  2. 设置中断或异常代码：
   + 如果是中断，则将 scause 寄存器的低 62 位设置为中断代码（例如，外部中断代码为 9）。
   + 如果是异常，则将 scause 寄存器的低 62 位设置为异常代码（例如，页错误代码为 15）。

异常和中断是处理器在执行程序过程中遇到的特殊情况，两者虽然都需要处理器暂停当前指令流并跳转到特定的处理程序，但它们的触发机制不同：
  + 异常（Exception）: 由程序的指令触发。例如，除以零、非法指令访问和系统调用等。
  + 中断（Interrupt）: 由外部设备或硬件计时器触发。例如，键盘输入、中断控制器等。
系统调用通常通过执行特定指令（如ecall）触发。这种指令会导致处理器产生一个异常，异常的原因会被写入scause寄存器中，并且控制权会转移到异常处理程序。
系统调用处理流程：
  1. 用户程序执行ecall指令:
   + 用户程序执行ecall指令，硬件检测到该指令触发异常。
  2. 硬件自动处理:
   + 将异常原因写入scause（8）寄存器（scause值为8，表示系统调用）。
   + 将当前程序计数器（PC）保存到sepc寄存器。
   + 跳转到由stvec寄存器指定的异常处理程序入口地址。
  3. 操作系统异常处理程序:
    + 操作系统的异常处理程序根据scause寄存器的值识别异常类型（系统调用），并调用相应的系统调用处理函数。
  4. 返回用户程序:
   + 系统调用处理完成后，操作系统恢复用户程序的上下文，并从sepc寄存器保存的地址继续执行用户程序。

### 2. 分析`devintr`函数
`devintr` 函数用于处理设备中断。它通过读取 scause 寄存器来识别和处理中断来源
```cpp
// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();
  // 通过检查 scause 的最高位是否为1（表示外部中断）以及低 8 位是否等于 9 来确定是否是外部中断。
  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    // plic_claim 函数获取触发中断的设备的 IRQ 号
    // 根据不同的 IRQ 号调用相应的中断处理函数：
    // UART0_IRQ 调用 uartintr() 处理 UART 中断
    // VIRTIO0_IRQ 调用 virtio_disk_intr() 处理 VIRTIO 磁盘中断
    // 其他未知 IRQ 打印警告信息。
    int irq = plic_claim();


    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    // 通知 PLIC 设备中断已处理完毕，允许再次中断：
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000001L){
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.
    // 只有在 CPU ID 为 0 时才调用 clockintr() 处理定时器中断。
    if(cpuid() == 0){
      clockintr();
    }
    
    // acknowledge the software interrupt by clearing
    // the SSIP bit in sip.
    w_sip(r_sip() & ~2);

    return 2;
  } else {
    return 0;
  }
}
```
### 3. `console.c`初始化UART硬件
1. 
```cpp
void
consoleinit(void)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}
```
首先是`uartinit()`:初始化UART
要知道：UART硬件对软件来说表现为一组内存映射寄存器，也就是说，有一些物理地址由PISC-V硬件连接到UART设备上，使得加载和存储操作与设备交互，而不是与RAM交互。UART的内存映射地址从at 0x10000000, or UART0。UART有少量的寄存器，每个寄存器的宽度为一字节。他们相对于UART0的偏移量定义。定义在`uart.c`中。所有配置uart时，只需对这些寄存器配置即可。

现在看一下`uartinit()`怎么配置的，在`uart.c`中
可以发现，这些操作都是对寄存器进行配置
```cpp
void
uartinit(void)
{
  // disable interrupts.
  WriteReg(IER, 0x00);

  // special mode to set baud rate.
  WriteReg(LCR, LCR_BAUD_LATCH);

  // LSB for baud rate of 38.4K.
  WriteReg(0, 0x03);

  // MSB for baud rate of 38.4K.
  WriteReg(1, 0x00);

  // leave set-baud mode,
  // and set word length to 8 bits, no parity.
  WriteReg(LCR, LCR_EIGHT_BITS);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initlock(&uart_tx_lock, "uart");
}
```
看一下这些寄存器的格式，都是保存层以`UART0`的偏移；

2. 
下面就是分析一下：
  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;

可以发现:`struct devsw devsw[NDEV];`是一个结构数组；
代表着：是设备交换表（device switch table）的一个实例。它包含了所有设备的读和写操作的函数指针。其中每个元素代表一个设备。每个元素包含该设备的操作函数指针，如读操作和写操作。设备交换表使得操作系统可以通过统一的接口来操作不同的设备，而不需要知道具体设备的实现细节。

设备是指计算机系统中的硬件组件或外围设备，这些设备可以是：控制台（Console）,磁盘（Disk）,网络接口（Network Interface）,显示器（Monitor）,键盘（Keyboard）,鼠标（Mouse）

结构定义：
```cpp
struct devsw {
  int (*read)(int, char*, int);
  int (*write)(int, char*, int);
};
```
每个 devsw 元素包含两个函数指针：read 和 write，分别指向设备的读和写操作函数。


比如说磁盘设备初始化：
```cpp
void diskinit(void) {
  initlock(&disk.lock, "disk"); // 初始化磁盘锁

  // 将磁盘设备的读操作和写操作分别指向 diskread 和 diskwrite 函数
  devsw[DISK].read = diskread;
  devsw[DISK].write = diskwrite;
}
```
使用设备交换表：
```cpp
char buffer[128];
devsw[CONSOLE].read(0, buffer, sizeof(buffer));
```

看一下这两个语句：
`devsw[CONSOLE].read = consoleread;`
  1. 索引控制台设备：
    CONSOLE 是一个常量，表示设备交换表中控制台设备的位置。它通常被定义为某个整数值。例如：
    `#define CONSOLE 1`
  2. 将控制台设备的读操作指向 consoleread 函数：
   `devsw[CONSOLE].read` 是一个函数指针，指向控制台设备的读操作函数。`consoleread` 是实现实际读操作的函数。通过这行代码，将控制台设备的读操作函数指针设置为 `consoleread`。
`consoleread` 函数
```cpp
int consoleread(int user_dst, char *dst, int n) {
  // 函数实现，用于从控制台读取数据
}
```
这个函数实现了从控制台读取数据的具体逻辑。
`user_dst` 参数指示目标缓冲区是否位于用户空间，
`dst` 是目标缓冲区，
`n` 是读取的字节数。

**设备交换表的作用**
设备交换表的作用是为不同类型的设备提供统一的接口。在 xv6 中，不同设备（如控制台、磁盘等）的读写操作通过 devsw 数组中的函数指针进行调用。这样，文件系统或其他系统组件可以通过统一的接口调用不同设备的读写操作，而无需关心具体设备的实现细节。

现在看一下`consoleread()`函数的实现
+ `user_dst`: 标志位，指示目标地址是否是用户空间。
+ `dst`: 目标地址，读取的数据将被复制到该地址。
+ `n`: 要读取的字节数。
```cpp
int
consoleread(int user_dst, uint64 dst, int n)
{
  uint target;  //保存初始的字节数 n，用于计算已读取的字节数。
  int c;
  char cbuf; // 用于将读取的字符复制到用户空间。

  target = n;
  acquire(&cons.lock);
  while(n > 0){
    // wait until interrupt handler has put some
    // input into cons.buffer.
    while(cons.r == cons.w){
      if(myproc()->killed){
        // while 循环: 等待控制台缓冲区 cons.buf 中有数据可读。如果读指针 cons.r 等于写指针 cons.w，表示缓冲区为空，当前进程进入睡眠状态，直到有新的输入数据
        release(&cons.lock);
        return -1;
      }
      sleep(&cons.r, &cons.lock);
    }
    //从缓冲区读取一个字符 c，并更新读指针 cons.r。
    c = cons.buf[cons.r++ % INPUT_BUF];

    if(c == C('D')){  // end-of-file
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      break;
    }

    // copy the input byte to the user-space buffer.
    //将读取的字符 c 存储到 cbuf，然后使用 either_copyout 函数将 cbuf 中的字符复制到用户空间地址 dst。如果复制失败，则结束读取。
    cbuf = c;
    if(either_copyout(user_dst, dst, &cbuf, 1) == -1)
      break;

    dst++;
    --n;

    if(c == '\n'){
      // a whole line has arrived, return to
      // the user-level read().
      break;
    }
  }
  release(&cons.lock);

  return target - n;
}
```



**怎么输出到频幕上呢？**
  `consolewrite` 函数会将数据写入到控制台缓冲区，通过调用`uartputs`函数。

`uartputc` 函数用于将一个字符写入 UART（通用异步收发传输器）输出缓冲区，并处理缓冲区满的情况。
```cpp
void
uartputc(int c)
{
  acquire(&uart_tx_lock); // 获取锁，确保对缓冲区的访问是原子的

  if(panicked){
    for(;;)
      ; // 如果系统处于 panic 状态，陷入无限循环
  }

  while(1){
    if(((uart_tx_w + 1) % UART_TX_BUF_SIZE) == uart_tx_r){
      // 缓冲区满了
      // 等待 uartstart() 函数腾出缓冲区空间
      sleep(&uart_tx_r, &uart_tx_lock); // 进入睡眠状态，等待缓冲区有空闲位置
    } else {
      uart_tx_buf[uart_tx_w] = c; // 将字符写入缓冲区
      uart_tx_w = (uart_tx_w + 1) % UART_TX_BUF_SIZE; // 更新写指针
      uartstart(); // 启动 UART 传输
      release(&uart_tx_lock); // 释放锁
      return;
    }
  }
}
```

`uartstart` 函数负责将数据从 UART 的发送缓冲区（uart_tx_buf）发送到实际的 UART 设备
```cpp
void
uartstart()
{
  while(1){
    if(uart_tx_w == uart_tx_r){
      // 发送缓冲区为空。
      return;
    }
    
    if((ReadReg(LSR) & LSR_TX_IDLE) == 0){
      // UART 发送保持寄存器已满，
      // 因此不能再给它发送字节。
      // 当寄存器准备好接收新字节时会触发中断。
      return;
    }
    
    int c = uart_tx_buf[uart_tx_r];
    uart_tx_r = (uart_tx_r + 1) % UART_TX_BUF_SIZE;
    
    // 可能 uartputc() 在等待缓冲区空间。
    wakeup(&uart_tx_r);
    
    WriteReg(THR, c);
  }
}

```


**看一下uart.c中所有函数的作用**
`void  uartinit(void) `： 初始化UART,在consoleinit(void)调用
`void  uartputc(int c)`： 用于将一个字符写入 UART输出缓冲区，在 consolewrite()调用
`void  uartputc_sync(int c)`:用于同步地将一个字符发送到 UART 设备

### 4. 控制台输入的流程

xv6的`main`函数调用`consoleinit`来初始化UART硬件。这个代码配置UART以在每接受一个输入字节时生成一个接受中断，并在每次完成发送一个输出字节时生成一个发送完成中断(kernel/uart.c:53)。

xv6的shell通过由`init.c`打开打开的文件描述符从控制台读取。对`read`系统调用的调用通过内核传递到`consoleread`。`consoleread`等待输入到达(通过中断)并缓存在`cons.buf`中，将输入复制到用户空间，并在整行到达后返回给用户进程。如果用户尚未输入完整的一行，任何读取进程将会在`sleep`调用中等待。

当用户输入一个字符时，UART硬件请求RISC-V触发一个中断，从而激活xv6的陷阱处理程序。陷阱处理程序调用`devintr`，它查看RISC-V的`scause`寄存器，发现中断来自外部设备。然后，它请求一个名位PLIVC的硬件单元告知是那台设备触发了中断。如果是UART触发了中断，`devintr`会调用`uartintr`。

`uartintr`从UART硬件读取所有等待的输入字符，并将他们交给`consoleintr`；它不会等待字符，因为未来的输入会触发新的中断。`consoleintr`的任务是将输入字符累积到`cons.buf`中，直到整行到达。`consoleintr`特别处理退格和其他几个字符。当新行到达时，`consoleread`唤醒正在等待的`consoleread`（如果有的话）。

一旦被唤醒，`consoleread`将在`con.buf`中看到完整的一行，将其复制到用户空间，并返回（通过系统调用机制）到用户空间


控制台输入
控制台输入是指用户通过键盘等输入设备向终端提供的数据。例如，当你在终端中输入命令或数据时，这些输入就是控制台输入。init 程序启动的 shell 会读取来自控制台的输入，以便用户可以交互式地输入命令。
## 控制台的输出

### 1. `consolewrite`函数
`consolewrite` 函数用于将数据写入控制台。在 xv6 中，该函数主要负责将数据从用户空间复制到内核空间，并通过 `UART` 将其输出到控制台
+ `user_src`：表示源地址是否在用户空间。user_src 为非零值时表示源地址在用户空间，为零时表示在内核空间。
+ `src`：源地址，即要写入的数据的起始地址。
+ `n`：要写入的数据的字节数。
```cpp
int
consolewrite(int user_src, uint64 src, int n)
{
  int i;

  acquire(&cons.lock);
  for(i = 0; i < n; i++){
    char c;
    // 从用户空间或内核空间复制一个字节的数据到字符 c。如果复制失败，则中断循环
    if(either_copyin(&c, user_src, src+i, 1) == -1)
      break;
    uartputc(c); // 过 UART 将字符 c 输出到控制台。
  }
  release(&cons.lock);

  return i;
}
```
uartputc 函数是 xv6 操作系统中用于将字符输出到 UART 的实现。这个函数不仅将字符输出到 UART，还管理了一个输出缓冲区，以确保字符可以连续输出而不会丢失
```cpp
void
uartputc(int c)
{
  acquire(&uart_tx_lock);

  if(panicked){
    for(;;)
      ;
  }

  while(1){
    if(((uart_tx_w + 1) % UART_TX_BUF_SIZE) == uart_tx_r){
      // buffer is full.
      // wait for uartstart() to open up space in the buffer.
      sleep(&uart_tx_r, &uart_tx_lock);
    } else {
      uart_tx_buf[uart_tx_w] = c;
      uart_tx_w = (uart_tx_w + 1) % UART_TX_BUF_SIZE;
      uartstart(); // 调用 uartstart() 函数以确保字符开始传输。
      release(&uart_tx_lock);
      return;
    }
  }
}
```
### 2. 输出流程

对连接到控制台的文件描述符进行的`write`系统调用最终到达`uartputc`。设备驱动程序维护一个输出缓冲区`uart_tx_buf`,以便写入进程不必等待UART完成发送操作；相反，`uartpusc`将每个字符附加到缓冲区，调用`uartstart`启动设备传输（如果设备还未在传输），然后返回。唯一使`uartputs`需要等待的情况使缓冲区已经满了。

每次UART发送完一个字节时，它会生成一个中断。`uartintr`调用`uartstart`，后者检查设备是否确实完成了发送，并将下一个缓冲输出字符交给设备。因此，如果一个进程向控制台写入多个字节，通常第一个字节会通过`uartputc`调用`uartstart`发送。


一个需要注意的一般模式是通过缓冲和中断实现设备活动和进程活动的解耦。即使没有进程在等待读取，控制台驱动程序也可以处理输入；后续的读取操作将看到这些输入。同样，进程可以发送输出而无需等待设备。这种解耦可以通过允许进程与设备i/o并发执行来提高性能，当设备速度较慢或需要立即处理（如回显输入字符）时，这尤其重要。这种想法被称为I/O并发。


控制台输出
控制台输出是指程序向终端显示的信息。例如，当你运行一个程序并看到其在屏幕上打印的文字时，那些文字就是控制台输出。在 init 程序的上下文中，控制台输出可能是调试信息、错误消息或正常的程序输出，如 printf 函数打印的内容。

## 键盘输入到显示输出的过程
1. 键盘输入
当你在键盘上按下一个键时，键盘控制器会捕获这个按键事件，并生成一个对应的扫描码。
2. 中断请求
键盘控制器将扫描码发送给计算机的中断控制器，中断控制器会生成一个中断请求（IRQ）。操作系统的内核会捕捉到这个中断请求，并将其转交给键盘中断处理程序。
3. 键盘中断处理程序
键盘中断处理程序（通常是在内核中实现的）会处理这个中断请求。它会从键盘控制器中读取扫描码，并将其转换为对应的字符（根据当前键盘布局）。接着，它会将这个字符存储到键盘缓冲区中，并唤醒等待输入的进程。
```cpp
void keyboard_interrupt_handler(void) {
    char scancode = read_scancode(); // 读取扫描码
    char ch = convert_scancode_to_char(scancode); // 将扫描码转换为字符
    buffer_write(ch); // 将字符写入键盘缓冲区
    wakeup(&buffer); // 唤醒等待缓冲区数据的进程
}
```
4. 字符设备接口
在字符设备接口层，键盘设备会被映射到一个虚拟文件，例如 /dev/tty。应用程序可以通过读取这个文件来获取键盘输入。操作系统使用 devsw 数组来管理不同的设备：

字符设备接口是操作系统提供的用于处理字符设备（如键盘和显示器）的抽象层。在这个阶段，用户进程（如终端应用程序）通过读取键盘设备文件（如 /dev/tty）来获取输入字符

```cpp
// 定义设备的读写函数指针
struct devsw {
    int (*read)(int, uint64, int);
    int (*write)(int, uint64, int);
};

// 将键盘设备的读写函数指针注册到 devsw 数组中
devsw[KEYBOARD].read = keyboard_read;
devsw[KEYBOARD].write = keyboard_write;
```
5. 读取键盘输入
用户进程通过读取 /dev/tty 文件来获取键盘输入：
```cpp
int keyboard_read(int user_dst, uint64 dst, int n) {
    acquire(&buffer_lock);
    while (buffer_empty()) {
        sleep(&buffer, &buffer_lock); // 如果缓冲区为空，睡眠等待数据
    }
    char ch = buffer_read(); // 从缓冲区读取一个字符
    either_copyout(user_dst, dst, &ch, 1); // 将字符复制到用户空间
    release(&buffer_lock);
    return 1;
}
```
从键盘缓冲区中读取字符后，可以对其进行处理。这包括对输入进行解析和处理，例如在命令行中显示、执行命令或进行其他操作。

6. 显示输出
应用程序处理完输入后，将处理后的输出（包括染色的文本）写入到标准输出（通常是显示器）。显示器作为字符设备，也有对应的设备文件，应用程序通过写入这个设备文件，将输出字符发送到显示器。

7. 显示器渲染
显示器的驱动程序负责将应用程序发送的字符渲染到屏幕上。字符包括 ANSI 转义序列，这些序列会被解释为相应的颜色或格式化指令。最终，用户在屏幕上看到带颜色的文本输出。





