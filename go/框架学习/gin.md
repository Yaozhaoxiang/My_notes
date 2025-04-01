# gin

1. 基本路由
2. API参数




## 1. 基本路由

gin 框架中采用的路由库是基于httprouter做的

```go
package main

import (
    "net/http"

    "github.com/gin-gonic/gin"
)

func main() {
    r := gin.Default()
    r.GET("/", func(c *gin.Context) {
        c.String(http.StatusOK, "hello word")
    })
    r.POST("/xxxpost",getting)
    r.PUT("/xxxput")
    //监听端口默认为8080
    r.Run(":8000")
}
```

## 2. API参数

可以通过Context的Param方法来获取API参数

```go
func(c *gin.Context) {
        name := c.Param("name")
        action := c.Param("action")
        //截取/
        action = strings.Trim(action, "/")
        c.String(http.StatusOK, name+" is "+action)
    }
```

这里的Param函数原型,可以看到如果有 / 则返回带有 / 参数的字符串。
```go
//	router.GET("/user/:id", func(c *gin.Context) {
//	    // a GET request to /user/john
//	    id := c.Param("id") // id == "john"
//	    // a GET request to /user/john/
//	    id := c.Param("id") // id == "/john/"
//	})
func (c *Context) Param(key string) string {
	return c.Params.ByName(key)
}
```

### 2.1 参数的实现

gin框架使用的是定制版本的httprouter，其路由的原理是大量使用公共前缀的树结构，它基本上是一个紧凑的Trie tree（或者只是Radix Tree）。具有公共前缀的节点也共享一个公共父节点。










