## 1.安装 g++ 和 clang
1. 安装 g++
g++ 是 GNU C++ 编译器，通常是 GNU 编译器套件的一部分。你可以通过以下命令安装：
```bash
sudo apt install g++
g++ --version
```
2. 安装 Clang
clang 是一个由 LLVM 项目开发的编译器前端，支持 C、C++ 和 Objective-C 等语言。你可以通过以下命令安装：
```bash
sudo apt install clang
clang --version
```

3. 安装特定版本（可选）
安装特定版本的 g++：
```bash
sudo apt install g++-<version>
sudo apt install clang-<version>
```
例如，安装 g++ 10：安装 clang 12：
```bash
sudo apt install g++-10
sudo apt install clang-12
```

4. 切换版本（可选）
如果你安装了多个版本的编译器，可以使用 update-alternatives 来切换默认版本。
```bash
sudo update-alternatives --config g++
sudo update-alternatives --config clang
```

## 任务1.

>Assignment: Now that you know how to fetch a Web page by hand, show us you can! Use the above technique to fetch the URL http://cs144.keithw.org/lab0/sunetid, replacing sunetid with your own primary SUNet ID. You will receive a secret code in the X-Your-Code-Is: header. Save your SUNet ID and the code for inclusion in your writeup

http://cs144.keithw.org/lab0/sunetid
```sh
telnet cs144.keithw.org http
GET /lab0/1111 HTTP/1.1
Host: cs144.keithw.org
Connection: close
```

## 任务2.Listening and connecting

本地通信，使用相同的端口
```sh
netcat -v -l -p 9090  //服务端

telnet localhost 9090  //客户端
```
## 任务3 实现geturl
>You’ll write a program called “webget” that creates a TCP stream socket, connects to a Web server, and fetches a page

题目要求：
>In the get URL function, implement the simple Web client as described in this file, using the format of an HTTP (Web) request that you used earlier. Use the TCPSocket and Address classes.

使用 TCPSocket and Address 这两个类

```cpp
void get_URL( const string& host, const string& path )
{
    TCPSocket client_sock;
    string buf;
    client_sock.connect(Address(host,"http"));
    client_sock.write("GET "+ path + " HTTP/1.1\r\n");
    client_sock.write("Host: "+ host + "\r\n");
    client_sock.write("Connection: close\r\n\r\n");

    client_sock.shutdown(SHUT_WR);
    while(!client_sock.eof())
    {
        client_sock.read(buf);
        cout << buf;
    }
    client_sock.close();
    cerr << "Function called: get_URL(" << host << ", " << path << ")\n";
//   cerr << "Warning: get_URL() has not been implemented yet.\n";
}
```
## 任务4. byte stream
翻译：
到目前为止，你已经看到了可靠的字节流抽象在互联网通信中的有用性，即使互联网本身只提供“尽力而为”（不可靠）的数据报服务。
为了完成本周的实验，你将在单台计算机的内存中实现一个提供这种抽象的对象。（你可能在 CS 110/111 中做过类似的事情。）字节从“输入”端写入，并可以按相同的顺序从“输出”端读取。字节流是有限的：写入者可以结束输入，之后不能再写入更多字节。当读取者读取到流的末尾时，它将达到“EOF”（文件结束），不能再读取更多字节。

你的字节流还将进行流量控制，以限制其在任何给定时间的内存消耗。对象初始化时有一个特定的“容量”：即它愿意在任何时候存储的最大字节数。字节流将限制写入者在任何给定时刻可以写入多少数据，以确保流不会超过其存储容量。随着读取者读取字节并从流中排出字节，写入者将被允许写入更多数据。

你的字节流仅用于单线程——你不必担心并发的写入者/读取者、锁定或竞态条件。
明确一点：字节流是有限的，但在写入者结束输入并完成流之前，它可以几乎是任意长的。你的实现必须能够处理比容量长得多的流。容量限制了在任何给定时间存储在内存中的字节数（已写入但尚未读取的字节数），但不限制流的长度。一个容量仅为一个字节的对象仍然可以传输数太字节的数据流，只要写入者一次写入一个字节，而读取者在写入者被允许写入下一个字节之前读取每个字节即可。


### 解
实现一个reliable byte stream；
和输入缓存和输出缓存类似；

这里我一直出错，最后发现问题在下面的函数：
由于 `string_view` 的构建需要指针，但是我不知道 deque 怎么返回头指针，所以就创建一个变量传递，这就是错误的原因；
因为当函数返回时，临时变量销毁，那么 `string_view` 的指针就指向未定义了，所以这样做是错误的；
```cpp
string_view Reader::peek() const
{
  // Your code here.
    std::string str(buffer_.begin(), buffer_.end());
    return {str};
}
```
然后修改为：
```cpp
string_view Reader::peek() const
{
  // Your code here.
    return { string_view( &buffer_.front(), buffer_.size()) };
}
```
但是又出现：
  ExpectationViolation: Expected exactly "\x1aCh>EXj:<\x03e\x17@\x0aIV%" at front of stream, but found "\x1aCh>EXj:<\x03e\x17@\x00\x00\x00\x00"，

可以发现读取错误；
其原因是：deque底层通常由多个固定大小的内存块（称为块或段）组成。每个块包含一部分数据，这些块并不是连续的内存地址。所以，这里读取其他数据；

随后改为下面，就正确了。
```cpp
string_view Reader::peek() const
{
  // Your code here.
    return { string_view( &buffer_.front(), 1 ) };
}
```

代码：
```cpp
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <deque>
class Reader;
class Writer;

class ByteStream
{
public:
  explicit ByteStream( uint64_t capacity );

  // Helper functions (provided) to access the ByteStream's Reader and Writer interfaces
  Reader& reader();
  const Reader& reader() const;
  Writer& writer();
  const Writer& writer() const;

  void set_error() { error_ = true; };       // Signal that the stream suffered an error.
  bool has_error() const { return error_; }; // Has the stream had an error?

protected:
  // Please add any additional state to the ByteStream here, and not to the Writer and Reader interfaces.
  uint64_t capacity_;
  bool error_ {};
//流是否关闭
  bool closed_;
//存取数据
  std::deque<char> buffer_;
//输入的字节数
  int total_push_;
  int total_pop_;
};

class Writer : public ByteStream
{
public:
  void push( std::string data ); // Push data to stream, but only as much as available capacity allows.
  void close();                  // Signal that the stream has reached its ending. Nothing more will be written.

  bool is_closed() const;              // Has the stream been closed?
  uint64_t available_capacity() const; // How many bytes can be pushed to the stream right now?
  uint64_t bytes_pushed() const;       // Total number of bytes cumulatively pushed to the stream
};

class Reader : public ByteStream
{
public:
  std::string_view peek() const; // Peek at the next bytes in the buffer
  void pop( uint64_t len );      // Remove `len` bytes from the buffer

  bool is_finished() const;        // Is the stream finished (closed and fully popped)?
  uint64_t bytes_buffered() const; // Number of bytes currently buffered (pushed and not popped)
  uint64_t bytes_popped() const;   // Total number of bytes cumulatively popped from stream
};

/*
 * read: A (provided) helper function thats peeks and pops up to `len` bytes
 * from a ByteStream Reader into a string;
 */
void read( Reader& reader, uint64_t len, std::string& out );

```
```cpp
#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ),error_(false),closed_(false),total_push_(0),total_pop_(0){}

bool Writer::is_closed() const
{
    
  // Your code here.
  return closed_;
}

void Writer::push( string data )
{
  // Your code here.
  if(closed_){
    return;
  }

  uint64_t writeable = available_capacity();
  uint64_t towrite = std::min(writeable,static_cast<uint64_t>(data.size()));


  for(uint64_t i=0;i<towrite;++i){
    buffer_.push_back(data[i]);
  }
  total_push_+=towrite;
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return total_push_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return closed_ && buffer_.empty();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return total_pop_;
}

string_view Reader::peek() const
{
  // Your code here.
    return { string_view( &buffer_.front(), 1 ) }; 

}

void Reader::pop( uint64_t len )
{
  // Your code here.
  uint64_t topop = std::min(len, buffer_.size());
    for(uint64_t i=0;i<topop;++i){
    buffer_.pop_front();
  }
  total_pop_ += topop;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffer_.size();
}

```

### 优化1：
参考文链接：https://blog.csdn.net/J__M__C/article/details/131713326

上面跑的结果：ByteStream throughput: 0.44 Gbit/s
分析：由于push传递的是一个 string 连续的内存，所以需要把string 拆成char这个过程时间复杂度是 O(n);

那么如何完美的转发呢？push收到的是连续的空间，我们就直接完美转发就行了。这意味着我们需要维护一片一片的连续内存，很容易想到，我们需要维护一个queue<string>，对于每个收到的字符串，我们直接转发进这个队列就行了，这样可以让我们的push操作本身达到 O(1)O(1)时间复杂度，

这位大佬写的非常好，而且代码质量很高：ByteStream throughput: 12.48 Gbit/s
```cpp
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <queue>
class Reader;
class Writer;

class ByteStream
{
public:
  explicit ByteStream( uint64_t capacity );

  // Helper functions (provided) to access the ByteStream's Reader and Writer interfaces
  Reader& reader();
  const Reader& reader() const;
  Writer& writer();
  const Writer& writer() const;

  void set_error() { error_ = true; };       // Signal that the stream suffered an error.
  bool has_error() const { return error_; }; // Has the stream had an error?

protected:
  // Please add any additional state to the ByteStream here, and not to the Writer and Reader interfaces.
  uint64_t capacity_;
  bool error_ {};
//流是否关闭
  bool closed_ {};
//存取数据
  std::queue<std::string> buffer_data {};
  std::string_view buffer_view;
//输入的字节数
  uint64_t bytes_pushed_ {}; // 已写入的字节数
  uint64_t bytes_popped_ {}; // 已弹出的字节数
};

class Writer : public ByteStream
{
public:
  void push( std::string data ); // Push data to stream, but only as much as available capacity allows.
  void close();                  // Signal that the stream has reached its ending. Nothing more will be written.

  bool is_closed() const;              // Has the stream been closed?
  uint64_t available_capacity() const; // How many bytes can be pushed to the stream right now?
  uint64_t bytes_pushed() const;       // Total number of bytes cumulatively pushed to the stream
};

class Reader : public ByteStream
{
public:
  std::string_view peek() const; // Peek at the next bytes in the buffer
  void pop( uint64_t len );      // Remove `len` bytes from the buffer

  bool is_finished() const;        // Is the stream finished (closed and fully popped)?
  uint64_t bytes_buffered() const; // Number of bytes currently buffered (pushed and not popped)
  uint64_t bytes_popped() const;   // Total number of bytes cumulatively popped from stream
};

/*
 * read: A (provided) helper function thats peeks and pops up to `len` bytes
 * from a ByteStream Reader into a string;
 */
void read( Reader& reader, uint64_t len, std::string& out );

```
```cpp

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ){}

bool Writer::is_closed() const
{
    
  // Your code here.
  return closed_;
}

void Writer::push( string data )
{
  // Your code here.
  uint64_t towrite = std::min(available_capacity(),static_cast<uint64_t>(data.size()));
  if(towrite == 0){
    return;
  }else if(towrite < data.size()){
    data.resize(towrite);
  }

  buffer_data.push( std::move(data) );
  if( buffer_data.size() == 1){
    buffer_view = buffer_data.front();
  }
  bytes_pushed_ +=towrite;
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - reader().bytes_buffered();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return (closed_ && (bytes_buffered() == 0));
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_;
}

string_view Reader::peek() const
{
  // Your code here.
    return buffer_view;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if( len > bytes_buffered())
    return;
  bytes_popped_ += len;

// 将 buffer 中的数据弹出
  while ( len > 0 ) {
    if ( len >= buffer_view.size() ) {
      len -= buffer_view.size();
      buffer_data.pop();
      buffer_view = buffer_data.front(); // 最开始就保证了 buffer_data 不为空
    } else {
      buffer_view.remove_prefix( len );
      len = 0;
    }
  }
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return writer().bytes_pushed() - bytes_popped();
}

```

### 优化2
上述代码中感觉有个问题，就是下面这段；
参考read的函数使用，当 len > bytes_buffered() 不应该直接返回，应该弹出两种较小者；
```cpp
void Reader::pop( uint64_t len )
{
  // Your code here.
  if( len > bytes_buffered())
    return;
  bytes_popped_ += len;

// 将 buffer 中的数据弹出
  while ( len > 0 ) {
    if ( len >= buffer_view.size() ) {
      len -= buffer_view.size();
      buffer_data.pop();
      buffer_view = buffer_data.front(); // 最开始就保证了 buffer_data 不为空
    } else {
      buffer_view.remove_prefix( len );
      len = 0;
    }
  }
}
```
修改并采用 std::queue<std::string_view> buffer_view 管理
```cpp
#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ){}

bool Writer::is_closed() const
{
  // Your code here.
  return closed_;
}

void Writer::push( string data )
{
  // Your code here.
  uint64_t towrite = std::min(available_capacity(),static_cast<uint64_t>(data.size()));
  if(towrite == 0){
    return;
  }else if(towrite < data.size()){
    data.resize(towrite);
  }

  buffer_data.push( std::move(data) );
  buffer_view.push( {buffer_data.back().c_str(),towrite});

  bytes_pushed_ +=towrite;
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - reader().bytes_buffered();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return (closed_ && (bytes_buffered() == 0));
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_;
}

string_view Reader::peek() const
{
  // Your code here.
  if( buffer_view.empty() ){
    return {};
  }
    return buffer_view.front();
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  auto n = min(len, reader().bytes_buffered());
  while(n>0)
  {
    auto sz = buffer_view.front().size();
    if( n < sz ){
        buffer_view.front().remove_prefix(n);
        bytes_popped_ += n;
        return;
    }
    buffer_data.pop();
    buffer_view.pop();
    n-=sz;
    bytes_popped_ += sz;
  }  
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return writer().bytes_pushed() - bytes_popped();
}
```



# 总结：
主要是第4个任务：
实现双向字节流；

任务：
>在单台计算机上的内存中实现一个对象，提供这个抽象。在输入端写入字节，然后可以从输出端以相同的顺序读取。字节流是有限的：写入者可以结束输入，然后就不能再写入更多字节了。当读者读到流的末尾时，它将达到 EOF（文件结束），不能再读取更多字节。
你的字节流还将进行流量控制，以限制其在任何给定时间的内存消耗。该对象被初始化为特定的容量：在任何给定时刻，它愿意存储在自己的内存中的最大字节数。字节流将限制写入者在任何给定时刻可以写入多少字节，以确保流不超过其存储容量。随着读者读取字节并从流中排空它们，写入者将被允许写入更多。你的字节流用于单线程，你不必担心并发的写入者/读者、锁定或竞态条件。
明确一下：字节流是有限的，但在写入者结束输入并完成流之前，它几乎可以是任意长的。你的实现必须能够处理比容量长得多的流。容量限制了在给定时刻保存在内存中的字节数（已写入但尚未读取的字节），但不限制流的长度。具有只有一个字节容量的对象仍然可以携带非常长的流，只要写入者一次写入一个字节，读者在允许写入者写入下一个字节之前读取每个字节。

为了使其效率更高，使用了queue<string>，利用 move 实现复杂度为o(1)的移动；







