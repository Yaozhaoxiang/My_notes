# go_zero

web + rpc

1. 什么是微服务
2. 什么是RPC
3. etcd
4. rpc使用
5. jwt

## 1. 微服务

> 微服务

微服务是由多个功能模块（小服务）组成，共同完成一件事（组成可用的一整个系统），各个服务之间使用RPC进行通信。大系统按照功能或者产品进行服务拆分，形成一个独立的服务。

+ 好处就在于各个服务是独立的个体，其代表了一种编程思维方式，服务需要具备独立性，扩展性，容错性，高可用性等，适用于高并发等大型系统
+ 坏处就是复杂性大大提高，管理维护变得困难

> protoc

+ protobuf是一种数据序列化和反序列化标准，谷歌出品，和json，xml类似，性能非常高。
+ protoc命令是protobuf的编译器，将.proto后缀的文件编译成对应的开发语言文件
+ protoc-gen-go是protoc的一个插件，用于生成go语言代码（protoc原生不支持生成go语言文件）
+ protoc-gen-go-grpc是protoc的go grpc插件，可以生成grpc相关的go语言文件。

## 2. 什么是RPC

远程过程调用；是一种协议，是用来屏蔽分布式计算中的各自调用细节，使得可以像本地调用一样直接调用一个远程的函数。

RPC（Remote Procedure Call，远程过程调用）是一种概念，而 gRPC 是基于该概念实现的一个具体的开源 RPC 框架。

+ gRPC：gRPC 强制使用 HTTP/2 作为传输协议。HTTP/2 具有二进制分帧、多路复用、头部压缩、服务器推送等特性，这些特性使得 gRPC 在性能和效率上有很大的优势。例如，多路复用允许在一个连接上同时处理多个请求和响应，提高了并发处理能力。
+ gRPC：默认使用 **Protocol Buffers** 进行数据序列化。Protocol Buffers 是一种高效的二进制序列化协议，它具有序列化速度快、数据压缩率高的特点。通过预定义的数据结构和编码规则，Protocol Buffers 可以将数据高效地序列化为二进制格式，减少了数据传输量和序列化 / 反序列化的时间开销。

grpc认证：多个server和client之间，如何识别对方，并且可以安全的进行数据传输。
+ SSL/TLS 认证方式（采用http2方式）
+ 基于Token的认证方式（基于安全连接）
+ 不采用任何措施的连接，这是不安全的连接
+ 自定义的身份认证

TLS（安全传输层）是建立在传输层TCP协议上的协议，服务与应用层，它的前身是SSL（安全套接字层），它实现了将应用层的报文进行加密后再交给TCP层传输。




## 4. rpc使用

1. 写 .proto 文件
2. goctl rpc protoc user/rpc/user.proto --go_out=user/rpc/types --go-grpc_out=user/rpc/types --zrpc_out=user/rpc/
3. 在/logic 写逻辑



goctl rpc protoc user.proto --go_out=./types --go-grpc_out=./types 

--zrpc_out=. 用于指定生成的 Go-Zero 风格的 gRPC 服务代码的输出目录。
--go_out= : 用于指定生成的普通 Go 代码的输出目录。
--go-grpc_out= : 指定生成的 gRPC 相关的 Go 代码的输出目录。

向grpc注册服务；
注册反射服务，在grpc中，反射是一种机制，允许客户端不知道服务定义（即.proto文件）的情况下查询服务端上的 gRPC服务信息。





## 5. jwt

JSON Web Token,用于作为 JSON 对象在各方之间安全地传输信息。此信息可以验证和信任，因为它是数字签名的。jwt可以使用秘密〈使用HNAC算法）或使用RSA或ECDSA的公钥/私钥对进行签名。


+ JWT作用：
    + 授权：一旦用户登录，每个后续请求将包括JWT，从而允许用户访问该令牌允许的路由，服务和资源。它的开销很小并且可以在不同的域中使用。如：单点登录。
    + 信息交换：在各方之间安全地传输信息。JWT可进行签名（如使用公钥/私钥对)，因此可确保发件人。由于签名是使用标头和有效负载计算的，因此还可验证内容是否被篡改。


1.传统Session

1.1.认证方式

http协议本身是一种无状态的协议，如果用户向服务器提供了用户名和密码来进行用户认证，下次请求时，用户还要再一次进行用户认证才行。因为根据http协议，服务器并不能知道是哪个用户发出的请求，所以为了让我们的应用能识别是哪个用户发出的请求，我们只能在服务器存储─份用户登录的信息，这份登录信息会在响应时传递给浏览器，告诉其保存为cookie,以便下次请求时发送给我们的应用，这样应用就能识别请求来自哪个用户。

1.2.暴露的问题
+ 用户经过应用认证后，应用都要在服务端做一次记录，以方便用户下次请求的鉴别，通常而言session都是保存在内存中，而随着认证用户的增多，服务端的开销会明显增大；
+ 用户认证后，服务端做认证记录，如果认证的记录被保存在内存中的话，用户下次请求还必须要请求在这台服务器上，这样才能拿到授权的资源。在分布式的应用上，限制了负载均衡器的能力。以此限制了应用的扩展能力；
+ session是基于cookie来进行用户识别，cookie如果被截获，用户很容易受到CSRF（跨站伪造请求攻击)攻击；
+ 在前后端分离系统中应用解耦后增加了部署的复杂性。通常用户一次请求就要转发多次。如果用session每次携带sessionid到服务
器，服务器还要查询用户信息。同时如果用户很多。这些信息存储在服务器内存中，给服务器增加负担。还有就是sessionid就是一个特征值，表达的信息不够丰富。不容易扩展。而且如果你后端应用是多节点部署。那么就需要实现session共享机制。不方便集群应用。

jwt验证：

![](./图片/jwt验证.png)

2.1.认证流程
+ 前端通过Web表单将自己的用户名和密码发送到后端的接口。该过程一般是HTTP的POST请求。建议的方式是通过SSL加密的传输(https协议)，从而避免敏感信息被嗅探。
+ 后端核对用户名和密码成功后，将用户的id等其他信息作为JWT Payload(负载)，将其与头部分别进行Base64编码拼接后签名，形成一个JWT(Token)。
+ 后端将JWT字符串作为登录成功的返回结果返回给前端。前端可以将返回的结果保存在localStorage（浏览器本地缓存）或sessionStorage（session缓存）上，退出登录时前端删除保存的JWT即可。
+ 前端在每次请求时将JWT放入HTTP的Header中的Authorization位。(解决XSS和XSRF问题）HEADER
+ 后端检查是否存在，如存在验证JWT的有效性。例如，检查签名是否正确﹔检查Token是否过期;检查Token的接收方是否是自己(可选）
+ 验证通过后后端使用JWT中包含的用户信息进行其他逻辑操作，返回相应结果。


2.2.JWT优点
+ 简洁(Compact)：可以通过URL，POST参数或者在HTTP header发送，数据量小，传输速度也很快；
+ 自包含(Self-contained)：负载中包含了所有用户所需要的信息，避免了多次查询数据库；
+ Token是以JSON加密的形式保存在客户端，所以JWT是跨语言的，原则上任何web形式都支持。
+ 不需要在服务端保存会话信息，特别适用于分布式微服务。I


3.JWT结构

令牌组成：
1.标头（Header）
2.有效载荷（Payload）
3.签名（Signature）
token格式：head.payload.singurater 如：xxxxx.yyyy.zzzz



## 6. 连接mysql

写出sql 创建表语句，然后执行 goctl model mysql ddl --src ./user.sql 
根据sql文件生成 crud 操作函数；


## 7. logx 使用

    // 记录不同级别的日志
    logx.Info("This is an info log")
    logx.Warn("This is a warning log")
    logx.Error("This is an error log")


    name := "John"
    age := 30
    logx.Infof("User %s is %d years old", name, age)
    logx.Warnf("User %s might have some issues", name)
    logx.Errorf("Failed to process user %s: %v", name, "some error")

