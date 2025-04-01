# gozero_api

go-zero 文档： https://go-zero.dev/docs/tutorials

api文件就是对这个服务所有api的描述

服务名，函数名，路径，请求方法，请求参数，响应参数

我们以用户管理的两个重要接口为例，去编写它的api文件

```golang
type (
  expandReq {
    shorten string `form:"shorten"`
  }

  expandResp {
    url string `json:"url"`
  }
)

type (
  shortenReq {
    url string `form:"url"`
  }

  shortenResp {
    shorten string `json:"shorten"`
  }
)

service shorturl-api {
  @handler ShortenHandler
  get /shorten(shortenReq) returns(shortenResp)

  @handler ExpandHandler
  get /expand(expandReq) returns(expandResp)
}
```

> 上面就是一个简单的 api 文件：

+ service shorturl-api { 这一行定义了 service 名字
+ @server 部分用来定义 server 端用到的属性
+ handler 定义了服务端 handler 名字
+ get /shorten(shortenReq) returns(shortenResp) 定义了 get 方法的路由、请求参数、返回参数等

> 然后：在 api 目录下，使用 goctl 生成 API Gateway 代码

goctl api go -api shorturl.api -dir .

+ -api：这是一个参数标志，用于指定 API 规范文件的路径。
+ shorturl.api：具体的 API 规范文件，该文件使用 Goctl 规定的 API 描述语言来定义 API 的接口信息，例如路由、请求参数、响应结构等。

+ -dir：这是一个参数标志，用于指定生成代码的目标目录。
+ .：表示当前目录，也就是说生成的代码会被放置在执行该命令时所在的目录。


生成的文件结构：
```bash
.
├── api
│   ├── etc
│   │   └── shorturl-api.yaml         // 配置文件
│   ├── internal
│   │   ├── config
│   │   │   └── config.go             // 定义配置
│   │   ├── handler
│   │   │   ├── expandhandler.go      // 实现 expandHandler
│   │   │   ├── routes.go             // 定义路由处理
│   │   │   └── shortenhandler.go     // 实现 shortenHandler
│   │   ├── logic
│   │   │   ├── expandlogic.go        // 实现 ExpandLogic
│   │   │   └── shortenlogic.go       // 实现 ShortenLogic
│   │   ├── svc
│   │   │   └── servicecontext.go     // 定义 ServiceContext
│   │   └── types
│   │       └── types.go              // 定义请求、返回结构体
│   ├── shorturl.api
│   └── shorturl.go                   // main 入口定义
├── go.mod
└── go.sum
```

整个流程：
+ 首先，通过handler 已经注册好处理函数了。这里是通过 ServiceContext 来传递上下文的，可以发现 ServiceContext 这里面保存了所有的上下文；
+ 在rotues中生成处理函数；
    + 给每一个处理函数都有一个字节的结构体，结构体包含 ctx,svcCtx
    + l := logic.NewExpandLogic(r.Context(), svcCtx)、
    r.Context() 提供了请求的上下文信息，svcCtx 包含了服务的上下文信息。
    全部保存到 结构中；


## RPC

```go
syntax = "proto3"; // Protobuf 语法版本为 proto3

package transform; // 命名空间

option go_package = "./transform"; // 生成的 Go 代码的包路径

message expandReq{
  string shorten = 1;
}

message expandResp{
  string url = 1;
}

message shortenReq{
  string url = 1;
}

message shortenResp{
  string shorten = 1;
}

service  transformer{
  rpc expand(expandReq) returns(expandResp);
  rpc shorten(shortenReq) returns(shortenResp);
}
```

+ service 关键字用于定义一个服务，类似于编程语言中的接口。
t+ ransformer 是服务的名称，表示提供 URL 长短链接转换功能的服务。
+ rpc expand(expandReq) returns(expandResp); 定义了一个名为 expand 的远程过程调用（RPC）方法，它接收一个 expandReq 类型的请求消息，返回一个 expandResp 类型的响应消息，用于将短链接扩展为长链接。
+ rpc shorten(shortenReq) returns(shortenResp); 定义了一个名为 shorten 的远程过程调用（RPC）方法，它接收一个 shortenReq 类型的请求消息，返回一个 shortenResp 类型的响应消息，用于将长链接缩短为短链接。



goctl rpc protoc transform.proto --go_out=. --go-grpc_out=. --zrpc_out=.

```bash
rpc/transform
├── etc
│   └── transform.yaml              // 配置文件
├── internal
│   ├── config
│   │   └── config.go               // 配置定义
│   ├── logic
│   │   ├── expandlogic.go          // expand 业务逻辑在这里实现
│   │   └── shortenlogic.go         // shorten 业务逻辑在这里实现
│   ├── server
│   │   └── transformerserver.go    // 调用入口, 不需要修改
│   └── svc
│       └── servicecontext.go       // 定义 ServiceContext，传递依赖
├── transform
│   ├── transform.pb.go
│   └── transform_grpc.pb.go
├── transform.go                    // rpc 服务 main 函数
├── transform.proto
└── transformer
    └── transformer.go              // 提供了外部调用方法，无需修改
```

注意需要 启动 etcd

## 怎么用 api 调用 rpc？

1. 在api配置文件中添加etcd
```go
Transform:
  Etcd:
    Hosts:
      - localhost:2379
    Key: transform.rpc
```
通过 etcd 自动去发现可用的 transform 服务

2. 在 api config添加 服务依赖

```go
type Config struct {
  rest.RestConf
  Transform zrpc.RpcClientConf     // 手动代码
}
```
3. 修改上下文 ServiceContext

```go
type ServiceContext struct {
  Config    config.Config
  Transformer transformer.Transformer                                          // 手动代码
}

func NewServiceContext(c config.Config) *ServiceContext {
  return &ServiceContext{
    Config:    c,
    Transformer: transformer.NewTransformer(zrpc.MustNewClient(c.Transform)),  // 手动代码
  }
}
```

通过 ServiceContext 在不同业务逻辑之间传递依赖

4. 然后再 api 中的hander中调用

通过调用 transformer 的 Expand 方法实现短链恢复到 url

```go
func (l *ExpandLogic) Expand(req types.ExpandReq) (types.ExpandResp, error) {
  // 手动代码开始 
  rpcResp, err := l.svcCtx.Transformer.Expand(l.ctx, &transformer.ExpandReq{
      Shorten: req.Shorten,
  })
  if err != nil {
      return nil, err
  }

  return &types.ExpandResp{
      Url: rpcResp.Url,
  }, nil
    // 手动代码结束
}
```

至此就可以调用了rpc了。调用的就是 rpc 中的logic中。


## 使用数据库表


















