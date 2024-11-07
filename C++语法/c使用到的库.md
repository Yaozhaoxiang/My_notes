#  <string.h>

## 1. `memmove`
  memmove 是一个标准的 C 库函数，用于将内存块从一个位置复制到另一个位置。与 memcpy 不同的是，memmove 处理重叠内存区域时能保证正确性

  函数原型：
  `void *memmove(void *dest, const void *src, size_t n);`
  参数：
  dest：指向目标内存区域的指针。
  src：指向源内存区域的指针。
  n：要复制的字节数。
  返回值：
  返回指向目标内存区域的指针。
  功能：
  memmove 函数从源内存区域 src 复制 n 个字节到目标内存区域 dest，并处理重叠内存区域的情况。它确保即使源和目标内存区域重叠，数据也能正确复制。

## 2. `memset`
   用于将一块内存区域设置为指定的值。

   函数原型
   `void *memset(void *s, int c, size_t n);`
   参数：
   s：指向要设置的内存区域的指针。
   c：要设置的值（将被转换为 unsigned char 类型）。
   n：要设置的字节数。
   返回值：
   返回指向目标内存区域的指针。
   memset 函数将 s 指向的内存区域的前 n 个字节设置为值 c。通常用于初始化数组或内存块。
## 3. `strcspn`
  `size_t strcspn(const char *str1, const char *str2);`
  str1：要搜索的目标字符串。
  str2：包含要匹配的字符集合的字符串。
  返回 str1 中第一个匹配 str2 中任意字符的字符的索引位置。即，它返回 str1 中首个出现的 str2 中字符的偏移量。

找出字符串中第一个出现的特定分隔符的位置。例如，在读取用户输入时，可以用 strcspn 来查找换行符的位置，从而去掉用户输入中的换行符

# <stdio.h>
## 1. 函数指针
  1. 声明函数指针：
     要声明一个指向特定类型函数的函数指针，必须指定函数的返回类型和参数列表。
     `int (*func_ptr)(int, int);`
  2. 初始化函数指针
     可以将一个已定义的函数赋给函数指针
     ```cpp
    // 定义一个函数
    int add(int a, int b) {
        return a + b;
    }

    int main() {
        // 初始化函数指针
        int (*func_ptr)(int, int) = add;

        // 使用函数指针调用函数
        int result = func_ptr(2, 3);
        printf("Result: %d\n", result);

        return 0;
    }
     ```
  3. 通过函数指针调用函数
     可以使用函数指针像普通函数一样进行调用：
     `int result = func_ptr(2, 3);`
## 2. `sprintf` 
   将变量格式化为字符串
   `sprintf(message, "The number is %d\n", number);`
   ```cpp
    #include <stdio.h>

    int main() {
        char message[100];
        int number = 42;
        
        // 使用 sprintf 将变量格式化为字符串
        sprintf(message, "The number is %d\n", number);
        
        // 使用 fputs 输出格式化后的字符串
        fputs(message, stdout);
        
        return 0;
    }
   ```
## 3. `snprintf`
  是一个用于格式化字符串的函数，它类似于 sprintf，但 snprintf 提供了一个额外的功能，即指定一个缓冲区的最大大小，以避免缓冲区溢出
  `int snprintf(char *str, size_t size, const char *format, ...);`
  str：指向用于存储格式化字符串的缓冲区的指针。
  size：缓冲区的最大大小（以字节为单位）。snprintf 会确保不会写入超过这个大小的内容。
   format：格式化字符串，类似于 printf 的格式化字符串。
  ...：格式化字符串中的格式化参数。\

## vsnprintf
用于将格式化的字符串输出到一个字符数组中
```cpp
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

```
    
参数说明：
 + str：指向一个字符数组，用于存储格式化后的字符串。
 + size：指定字符数组的大小，包括最后的空字符 '\0'。
 + format：一个字符串，其中包含文本和格式化指令，用于指定如何格式化输出
 + ap：一个 va_list 类型的变量，包含了要格式化的数据。

返回值：
 + vsnprintf 函数返回一个整数，表示写入到字符数组中的字符数（不包括最后的空字符）。如果返回值大于等于 size，则表示字符数组可能溢出。

```cpp
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void safe_vprintf(const char *format, ...) {
    char buffer[1024]; // 定义一个足够大的缓冲区
    va_list args;

    va_start(args, format); // 初始化可变参数列表
    int result = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
    va_end(args); // 清理可变参数列表

    if (result < 0) {
        printf("Error in formatting output\n");
    } else if (result >= (int)(sizeof(buffer) - 1)) {
        printf("Output was truncated due to buffer size limit\n");
    } else {
        buffer[sizeof(buffer) - 1] = '\0'; // 确保字符串以空字符终止
        printf("%s", buffer);
    }
}

int main() {
    safe_vprintf("Hello, %s! Your age is %d.\n", "Alice", 30);
    return 0;
}
```


## 4. `strtok`
   是一个用于拆分字符串的函数，它会将字符串分割成一系列标记（tokens），并使用指定的分隔符进行拆分
   `char *strtok(char *str, const char *delim);`
   str：要被分割的字符串。如果 str 是 NULL，strtok 会继续处理上一个字符串。
  delim：一个包含所有分隔符的字符串。strtok 使用这些分隔符来确定在哪里拆分字符串

  返回指向分隔出的第一个标记的指针。
  如果没有更多标记可返回，返回 NULL。
  ```cpp
    #include <stdio.h>
    #include <string.h>

    int main() {
        char buffer[] = "Hello\nWorld\nExample";
        char *msg;

        // 获取第一个标记
        msg = strtok(buffer, "\n");
        while (msg != NULL) {
            printf("Token: %s\n", msg);
            // 获取下一个标记
            msg = strtok(NULL, "\n");
        }

        return 0;
    }
  ```
strtok(buffer, "\n")：第一次调用时，它会查找 buffer 中的第一个 \n 字符，并返回第一个标记（即 \n 前的部分 "Hello"）

之后，调用 strtok(NULL, "\n") 会继续处理 buffer 剩余的部分，返回下一个标记（即 \n 后的部分 "World"），直到没有更多的标记。

strtok 会修改原始字符串，将分隔符替换为 '\0'（空字符）。因此，buffer 字符串会被修改。

strtok 是线程不安全的，因为它使用静态数据存储上一个调用的状态。在多线程环境中，应该使用 strtok_r（可重入的版本）来替代。

strtok 使用 NULL 作为第一个参数来继续处理上一次调用中的字符串

## 5. `strtok_r`
strtok_r 是 strtok 的线程安全版本。与 strtok 不同，strtok_r 设计用于在多线程环境中安全地拆分字符串。strtok 是线程不安全的，因为它使用静态存储来保存当前拆分的状态，而 strtok_r 允许你为每个线程提供独立的状态数据，从而避免线程间的干扰

`char *strtok_r(char *str, const char *delim, char **saveptr);`
  str：要拆分的字符串。如果是第一次调用，传递要拆分的字符串；后续调用中，传递 NULL。
  delim：分隔符字符串，用于定义拆分点。
  saveptr：用于保存拆分状态的指针，strtok_r 使用它来跟踪拆分的位置。

  返回指向下一个标记的指针。
  如果没有更多标记可返回，返回 NULL。
```cpp
#include <stdio.h>
#include <string.h>

int main() {
    char buffer[] = "Hello\nWorld\nExample";
    char *msg;
    char *saveptr;

    // 获取第一个标记
    msg = strtok_r(buffer, "\n", &saveptr);
    while (msg != NULL) {
        printf("Token: %s\n", msg);
        // 获取下一个标记
        msg = strtok_r(NULL, "\n", &saveptr);
    }

    return 0;
}
```
strtok_r(buffer, "\n", &saveptr)：第一次调用时，buffer 是要拆分的字符串，\n 是分隔符，saveptr 用于保存当前拆分位置的状态。

strtok_r(NULL, "\n", &saveptr)：后续调用时，str 参数为 NULL，strtok_r 继续处理上一个拆分的字符串。

线程安全性：
  strtok 是线程不安全的，因为它使用静态数据保存拆分状态。
  strtok_r 是线程安全的，它通过 saveptr 参数保存拆分状态，允许在多个线程中独立操作。

使用方式:
  strtok 的每次调用都需要传递要拆分的字符串，除非是后续调用，它会继续处理上一个字符串。

  strtok_r 的第一次调用需要传递要拆分的字符串，后续调用则传递 NULL，并使用 saveptr 参数保存和恢复拆分状态。
## 6. `strcpy`
  `char *strcpy(char *dest, const char *src);`
  注意：strcpy 函数需要一个可写的目标缓冲区，即a[20]这种

  将 src 指向的字符串复制到 dest 指向的内存区域。
  复制包括源字符串的终止空字符（\0） 

  不检查缓冲区大小：strcpy 不会检查 dest 是否有足够的空间来存储 src，如果 dest 太小，可能会导致缓冲区溢出。
  终止符：复制的过程中包括源字符串的终止符，使得目标字符串总是以 '\0' 结束

## 7. `strncpy`
   `char *strncpy(char *dest, const char *src, size_t n);`
  从 src 指向的字符串中复制最多 n 个字符到 dest 指向的内存区域。
  如果 src 的长度小于 n，则 strncpy 会在 dest 中填充空字符，直到复制 n 个字符

  缓冲区安全性：strncpy 允许指定要复制的字符数，提供了一种缓冲区大小检查的方式，减少了缓冲区溢出的风险。
  终止符：如果 src 的长度大于或等于 n，则 dest 不会以 '\0' 结束。如果需要保证终止符，可以在使用后手动添加 '\0'。
## 8. `scanf` 和 `fgets`
`scanf`
功能：scanf 用于从标准输入（通常是键盘）中读取格式化输入。它根据指定的格式字符串解析输入数据并将其存储到指定的变量中。

格式化输入：scanf 可以解析多种数据类型，包括整数、浮点数、字符串等。例如，scanf("%d", &num); 用于读取一个整数，scanf("%s", str); 用于读取一个字符串。

缓冲区问题：scanf 读取数据时**不会清除输入缓冲区中的换行符**，因此在连续调用 scanf 读取不同类型的数据时，可能会遇到问题。如果在读取字符串之后使用 scanf 读取整数，可能会遇到问题。

错误处理：scanf 的错误处理相对简单，它通常返回成功读取的项数。如果遇到错误或到达文件末尾，则返回 EOF。

```cpp
scanf("%d",&opnd_cnt);
scanf("%d",&opnd_cnt);
```
  1. 假设用户输入了以下内容：
    ```cpp
        10
        20
    ```
  2. 第一次 scanf("%d", &opnd_cnt);：
    用户输入 10，然后按下 Enter 键。
    scanf 读取 10 并将其存储到 opnd_cnt 中。
    换行符 \n 留在输入流中。
  3. 第二次 scanf("%d", &opnd_cnt);：
    scanf 读取输入流中的换行符 \n，然后读取 20 并将其存储到 opnd_cnt 中。
    处理完 20 后，换行符 \n 仍然留在输入流中，直到被其他操作（如 fgets）处理。

`fgets`
功能：fgets 用于从文件流（包括标准输入）中读取一行文本。它将读取的文本存储到指定的字符数组中，并确保不超过数组的大小。

读取整行：fgets 读取一整行，包括空格和其他特殊字符，直到遇到换行符、文件末尾或达到指定的最大字符数。

换行符处理：fgets 会将换行符包括在读取的字符串中（如果换行符被读取）。如果读取到文件末尾，它不会去掉换行符。

安全性：fgets 比 scanf 更安全，因为它可以避免缓冲区溢出问题。你可以控制读取的最大字符数，确保不会超过缓冲区的大小。

错误处理：fgets 在遇到错误或文件末尾时返回 NULL，可以通过检查返回值来确定是否成功读取。
## 9. `fwrite` 和 `fread`
用于文件的写入和读取操作

`fwrite`
fwrite 函数用于将数据从内存写入到文件中。其函数原型如下：
`size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);`
ptr：指向要写入的数据的指针。
size：每个数据项的字节数。
count：数据项的数量。
stream：文件指针，表示写入数据的目标文件。

返回实际写入的项数。如果返回值小于 count，说明发生了错误或文件已到达末尾
```cpp
#include <stdio.h>

int main() {
    FILE *file = fopen("example.bin", "wb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    int data[] = {1, 2, 3, 4, 5};
    size_t written = fwrite(data, sizeof(int), 5, file);
    if (written != 5) {
        perror("fwrite");
    }

    fclose(file);
    return 0;
}
```

`fread`
fread 函数用于从文件中读取数据并存储到内存中。其函数原型如下：
`size_t fread(void *ptr, size_t size, size_t count, FILE *stream);`
ptr：指向接收数据的内存区域的指针。
size：每个数据项的字节数。
count：要读取的数据项的数量。
stream：文件指针，表示读取数据的源文件。

返回实际读取的项数。如果返回值小于 count，说明发生了错误、文件已到达末尾，或文件中没有足够的数据。
```cpp
#include <stdio.h>

int main() {
    FILE *file = fopen("example.bin", "wb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    int data[] = {1, 2, 3, 4, 5};
    size_t written = fwrite(data, sizeof(int), 5, file);
    if (written != 5) {
        perror("fwrite");
    }

    fclose(file);
    return 0;
}

```
在处理文件读取时，需要确保没有错误发生。如果 fread 在读取文件时遇到错误，返回值可能小于实际读取的字节数。应添加对 fread 错误的检查。
```cpp
if (ferror(file)) {
    perror("fread() error");
    fclose(file);
    close(client_sock);
    close(server_sock);
    exit(1);
}
```
## 10. `fseek`
用于在文件中移动文件位置指针（文件游标）

函数原型
```cpp
int fseek(FILE *stream, long offset, int whence);
```
参数说明
  stream: 指向文件流的指针（FILE*），该文件流必须已经用 fopen 打开。
  offset: 要移动的字节数。它是一个长整型数值（long），表示相对于 whence 参数指定的位置的偏移量。
  whence: 参考位置，用于计算 offset 的基准。可以取以下值：
    SEEK_SET: 文件开头。offset 是相对于文件开头的偏移量。
    SEEK_CUR: 当前文件位置。offset 是相对于当前位置的偏移量。
    SEEK_END: 文件末尾。offset 是相对于文件末尾的偏移量。

返回值
  成功时，返回 0。
  失败时，返回一个非零值，并且 errno 会设置为具体的错误码

```cpp
FILE *file = fopen("example.txt", "r");
if (file == nullptr) {
    perror("Error opening file");
    return 1;
}

fseek(file, 0, SEEK_SET); // 将文件指针移动到文件开头

fseek(file, 0, SEEK_END); // 将文件指针移动到文件末尾
```
典型应用场景
  获取文件大小：通常使用 fseek 将文件指针移动到文件末尾，然后使用 ftell 来获取文件的大小。
  随机访问：fseek 允许在文件中进行随机访问，这对于处理大型文件或进行复杂文件操作很有用。

## 11. `ftell`
用于获取文件流的当前文件位置指针（文件游标）的偏移量

函数原型
```cpp
long ftell(FILE *stream);
```
参数说明
  stream: 指向 FILE 结构体的指针，代表要操作的文件流。这个文件流必须已经用 fopen 打开。

返回值
  成功时: 返回当前文件位置指针的偏移量（以字节为单位），从文件开头算起。
  失败时: 返回 -1L，并设置 errno 为具体的错误码。

1. 获取文件大小
```cpp
FILE *file = fopen("example.txt", "r");
// 移动文件指针到文件末尾
    fseek(file, 0, SEEK_END);

    // 获取文件大小
    long file_size = ftell(file);
    printf("File size: %ld bytes\n", file_size);
```
2. 保存和恢复文件位置
```cpp
#include <stdio.h>

int main() {
    FILE *file = fopen("example.txt", "r");
    if (file == nullptr) {
        perror("Error opening file");
        return 1;
    }

    // 保存当前文件位置
    long position = ftell(file);
    if (position == -1L) {
        perror("Error getting file position");
        fclose(file);
        return 1;
    }

    // 移动文件指针到文件开头
    fseek(file, 0, SEEK_SET);

    // 读取文件内容或执行其他操作

    // 恢复文件指针到之前的位置
    fseek(file, position, SEEK_SET);

    fclose(file);
    return 0;
}
```
注意事项
  + 文件指针有效性: 确保在调用 ftell 之前，文件指针指向一个有效的位置。
  + 文件打开模式: ftell 可以用于文本文件和二进制文件，但当文件流被以文本模式打开时，文件的实际偏移量可能会受到文件的换行处理的影响。
  + 错误处理: 使用 ftell 时，建议检查其返回值，以确保操作成功。

## 二、 文件

## 1. 文件输入输出

`fputs` 和 `fgets` 是 C 语言标准库中的两个用于处理文件输入输出的函数，分别用于写入和读取文件中的文本数据。

### 1. `fputs` 作用: `fputs` 用于将一个字符串写入到指定的文件流中。
  `int fputs(const char *str, FILE *stream);`
  如果成功，fputs 返回非负值。
  如果失败，返回 EOF（通常是 -1），并且可以通过 ferror 函数检查具体的错误。

### 2. `fgets`作用: `fgets` 用于从指定的文件流中读取一行文本（包括换行符）到一个字符串缓冲区。
用途: 用于从文件中读取一行字符，并将其存储在字符串缓冲区中。fgets 会读取最多 n-1 个字符或在遇到换行符（\n）或文件结束符（EOF）时停止读取。它会在读取的字符串末尾自动添加一个空字符（\0）。

典型用途: 适用于读取文本文件中的一行数据，通常用于读取包含换行符的字符串。

  `char *fgets(char *str, int n, FILE *stream);`
  str：用于存储读取数据的字符数组。
  n：要读取的最大字符数（包括终止符 '\0'）。
  stream：指向 FILE 类型的文件流指针，指定要读取的源文件。

  如果成功，返回 str，即指向读取到的字符串。
  如果遇到文件结束符 (EOF) 或发生读取错误，返回 NULL。如果是读取错误，文件流的错误标志可能会被设置，可以使用 ferror 函数检查。

```cpp
    FILE *file = fopen("example.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("Read: %s", buffer);
    }
    fputs("Hello, World!\n", file);
```

`stdout` 是 C 标准库中定义的一个文件流指针，表示标准输出流。默认情况下，标准输出流通常是终端或控制台。

`stdin` 是 C 语言标准库中的一个文件流指针，用于表示标准输入流。标准输入流通常是从键盘输入的数据流。stdin 允许程序从键盘或其他输入设备读取数据。

### 3. `fwrite` 
用于文件的写入和读取操作
`fwrite`
fwrite 函数用于将数据从内存写入到文件中。其函数原型如下：
`size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);`
ptr：指向要写入的数据的指针。
size：每个数据项的字节数。
count：数据项的数量。
stream：文件指针，表示写入数据的目标文件。

返回实际写入的项数。如果返回值小于 count，说明发生了错误或文件已到达末尾
```cpp
#include <stdio.h>

int main() {
    FILE *file = fopen("example.bin", "wb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    int data[] = {1, 2, 3, 4, 5};
    size_t written = fwrite(data, sizeof(int), 5, file);
    if (written != 5) {
        perror("fwrite");
    }

    fclose(file);
    return 0;
}
```

### 4. `fread`
用途: 用于从文件中读取原始数据块，并将它们存储在内存缓冲区中。fread 可以读取 size 字节大小的 count 个数据块，即总共 size * count 字节的数据。fread 不会添加空字符，并且不会在读取时考虑行的概念。

典型用途: 适用于从文件中读取二进制数据或多个连续的数据块，如读取二进制文件或特定大小的数据结构。

fread 函数用于从文件中读取数据并存储到内存中。其函数原型如下：
`size_t fread(void *ptr, size_t size, size_t count, FILE *stream);`
ptr：指向接收数据的内存区域的指针。
size：每个数据项的字节数。
count：要读取的数据项的数量。
stream：文件指针，表示读取数据的源文件。

返回实际读取的项数。如果返回值小于 count，说明发生了错误、文件已到达末尾，或文件中没有足够的数据。
```cpp
#include <stdio.h>

int main() {
    FILE *file = fopen("example.bin", "wb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    int data[] = {1, 2, 3, 4, 5};
    size_t written = fwrite(data, sizeof(int), 5, file);
    if (written != 5) {
        perror("fwrite");
    }

    fclose(file);
    return 0;
}

```
在处理文件读取时，需要确保没有错误发生。如果 fread 在读取文件时遇到错误，返回值可能小于实际读取的字节数。应添加对 fread 错误的检查。
```cpp
if (ferror(file)) {
    perror("fread() error");
    fclose(file);
    close(client_sock);
    close(server_sock);
    exit(1);
}

```
### 5. `feof`
feof 是 C 标准库中的一个函数，用于检测文件流是否到达了文件结束标志（EOF, End of File）。
`int feof(FILE *stream);`
如果文件流的结束标志（EOF）已被设置，feof 返回一个非零值（通常为1）。
如果未到达文件结束，feof 返回 0。

feof 函数并不直接读取文件数据。相反，它用来检查之前的文件读取操作是否遇到了文件的结尾（EOF）。
当一个读取函数（如 fgetc, fgets, fread 等）尝试从文件中读取数据但已经到达文件末尾时，文件流的 EOF 标志会被设置。此时，feof 可以用来判断是否已经读到文件末尾。

```cpp
#include <stdio.h>

int main() {
    FILE *file = fopen("example.txt", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    if (feof(file)) {
        printf("\nEnd of file reached.\n");
    } else {
        printf("\nAn error occurred while reading the file.\n");
    }

    fclose(file);
    return 0;
}
```
### 6. `fflush`
fflush 是 C 标准库中的一个函数，用于刷新输出缓冲区，确保缓冲区中的数据被立即写入文件或显示设备.它的原型定义在 <stdio.h> 头文件中，具体如下：
`int fflush(FILE *stream);`
功能
  输出流: 对于一个输出流（例如 stdout 或打开文件的写操作），fflush 会将缓冲区中的数据立即写入到相应的文件或显示设备中。

  输入流: 对于一个输入流，fflush 的行为未定义。在一些实现中，它可能会清除输入缓冲区，但这种行为并不被标准支持，因此不建议对输入流使用 fflush。

  NULL 参数: 如果 stream 参数为 NULL，fflush 会将所有打开的输出流的缓冲区内容刷新。
返回值
  成功时，fflush 返回 0。
  如果发生错误，则返回 EOF（通常为 -1），并设置 errno 来指示具体的错误。

### 7. `fputs 和 fprintf`
都是 C 语言标准库中的函数，用于将数据写入文件或其他输出流中。
`int fprintf(FILE *stream, const char *format, ...);`
功能
  fprintf 是一个功能强大的函数，用于将格式化的字符串写入指定的输出流（stream）。
  它类似于 printf，但输出目的地是文件或流，而不是标准输出（stdout）。
返回值
  成功时返回写入的字符数（不包括空字符 \0）。
  失败时返回负值，并设置错误标志。
特性
  格式化输出: fprintf 支持多种格式化输出，比如整数、浮点数、字符串等。你可以通过格式化字符串指定输出的形式。
  灵活性: fprintf 的多样性使其适用于各种复杂的输出需求，如日志记录、数据报告等。

fputs 只能输出简单的字符串，不包括格式化功能。
fprintf 能输出格式化的数据，适用于更复杂的场景。

fputs 用于简单的字符串写入。
fprintf 用于需要格式化输出的场合，如带变量的字符串输出。

fputs 通常比 fprintf 更高效，因为它不需要解析格式字符串。
fprintf 的复杂性使得它在性能上略逊于 fputs，尤其是当大量格式化操作时。

如果只需写入简单的字符串，且不涉及变量或格式化，fputs 是更好的选择。
如果需要将数据与文本组合，或需要控制输出格式，fprintf 是更合适的选择。

### 8. `fscanf 和 fgets`
都是 C 语言中用于从输入流中读取数据的函数。尽管它们的目的都是读取数据，它们的使用场景和工作方式却有明显的区别。

`int fscanf(FILE *stream, const char *format, ...);`
功能
  fscanf 用于从指定的输入流（stream）中根据指定的格式（format）读取数据，并将读取的数据存储到提供的变量中。
  它类似于 scanf，但读取来源是文件或输入流，而不是标准输入（stdin）。
返回值
  成功读取并存储的项的数量（类型与格式说明符匹配的变量）。
  如果遇到文件结束或读取错误，则返回 EOF。

```cpp
    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    int num;
    char str[100];

    fscanf(file, "%d %s", &num, str);
    printf("Read number: %d, string: %s\n", num, str);
    fclose(file);
```
格式化输入: fscanf 支持格式化输入，可以根据给定的格式说明符从文件中读取各种类型的数据（如整数、浮点数、字符串等）。

自动解析: 根据格式说明符，fscanf 会自动解析并转换输入数据，适合用于读取结构化的数据。

比较

fscanf 用于格式化输入，可以根据指定的格式从文件中读取不同类型的数据。
fgets 用于读取整个行的文本数据，不会解析数据，而是直接读取字符串。

fscanf 会根据格式说明符自动解析数据，可能会跳过空白字符，适合读取结构化的数据。
fgets 读取的数据包括换行符，是逐行读取，不进行数据解析，适合读取纯文本或非结构化的数据。

fscanf 返回成功读取的项的数量，如果遇到错误或文件结束，返回 EOF。
fgets 成功时返回指向字符串的指针，失败或到达文件末尾时返回 NULL。

如果需要读取并解析结构化的数据（例如数字、字符串等），fscanf 是合适的选择。
如果只需要逐行读取文本数据，或者保留文件中的空白字符和格式，fgets 是更好的选择。



## 2.标准错误
stderr（标准错误）是一个用于输出错误信息的标准流，主要作用是将程序运行中的错误或警告信息输出到终端或文件，以便用户或开发者能够及时了解程序运行中的异常情况。

1. 分离错误信息和正常输出：
stderr 与 stdout（标准输出）分开，允许程序将正常的输出（例如计算结果或日志）和错误信息（如运行时错误或异常情况）分开处理。这样可以更容易地管理和查看错误信息，尤其是在重定向或管道操作中。

2. 独立于标准输出：
stderr 通常不会被标准输出重定向。即使标准输出被重定向到文件，错误信息依然会输出到终端（除非明确重定向 stderr）。这使得错误信息可以在程序的输出被重定向时仍然可见。

`perror`是一个 C 标准库函数，定义在 <stdio.h> 头文件中。它的作用是根据最近一次系统调用或库函数发生的错误，打印一个描述性错误信息
   ` perror("write() error");`

## stat 函数

stat 函数是 POSIX 标准中的一个系统调用，用于获取文件的状态信息。它返回关于文件的详细信息，包括文件大小、权限、所有者、时间戳等。

函数原型
```cpp
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stat(const char *path, struct stat *buf);

```
path：文件的路径（绝对路径或相对路径）。
buf：指向 stat 结构体的指针，用于存储文件的状态信息。

返回值
  成功：返回 0。
  失败：返回 -1 并设置 errno 来指示错误原因。

stat 结构体
  stat 函数会将文件的状态信息存储在一个 struct stat 结构体中。这个结构体定义在 <sys/stat.h> 头文件中，其常见成员包括：
```cpp
struct stat {
    dev_t     st_dev;     // 文件所在设备的ID
    ino_t     st_ino;     // 文件的inode号
    mode_t    st_mode;    // 文件的类型和权限
    nlink_t   st_nlink;   // 硬链接的数量
    uid_t     st_uid;     // 所有者的用户ID
    gid_t     st_gid;     // 所有者的组ID
    dev_t     st_rdev;    // 设备ID（如果是特殊文件）
    off_t     st_size;    // 文件大小（以字节为单位）
    blksize_t st_blksize; // 文件系统的块大小
    blkcnt_t  st_blocks;  // 分配给该文件的块数量
    time_t    st_atime;   // 最后一次访问时间
    time_t    st_mtime;   // 最后一次修改时间
    time_t    st_ctime;   // 最后一次状态改变时间
};

```
下面是一个简单的示例，展示如何使用 stat 函数来获取文件信息：
```cpp
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    struct stat fileStat;
    
    // 调用 stat 函数
    if (stat("example.txt", &fileStat) < 0) {
        perror("stat");
        return 1;
    }

    // 打印文件信息
    printf("File Size: \t\t%ld bytes\n", fileStat.st_size);
    printf("Number of Links: \t%ld\n", fileStat.st_nlink);
    printf("File inode: \t\t%ld\n", fileStat.st_ino);

    printf("File Permissions: \t");
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");

    printf("Last access time: \t%s", ctime(&fileStat.st_atime));
    printf("Last modification time: \t%s", ctime(&fileStat.st_mtime));
    printf("Last status change time: \t%s", ctime(&fileStat.st_ctime));
    
    return 0;
}

```
其他相关函数
  `fstat(int fd, struct stat *buf)`：类似于 stat，但接收一个文件描述符 fd，而不是文件路径。
  `lstat(const char *path, struct stat *buf)`：类似于 stat，但它会返回符号链接本身的信息，而不是符号链接所指向的文件的信息。







