1. get请求一般是去取获取数据（其实也可以提交，但常见的是获取数据）；
post 请求一般是去提交数据

2. get因为参数会放在url中，所以隐私性，安全性较差，请求的数据长度是有限制的，
不同的浏览器和服务器不同，一般限制在 2~8K 之间，更加常见的是 1k 以内；
post请求是没有的长度限制，请求数据是放在body中；

3. get请求刷新服务器或者回退没有影响，post请求回退时会重新提交数据请求。

4. get请求可以被缓存，post请求不会被缓存。

5. get请求会被保存在浏览器历史记录当中，post不会。get请求可以被收藏为书签，因为参数就是url中，但post不能。它的参数不在url中。

6. get请求只能进行url编码（appliacation-x-www-form-urlencoded）,post请求支持多种（multipart/form-data等）

深入理解
1.GET 和 POST都是http请求方式， 底层都是 TCP/IP协议；通常GET 产生一个 TCP 数据包；POST 产生两个 TCP 数据包（但firefox是发送一个数据包），

2.对于 GET 方式的请求，浏览器会把 http header 和 data 一并发送出去，服务器响应 200
（返回数据）表示成功；

而对于 POST，浏览器先发送 header，服务器响应 100， 浏览器再继续发送 data，服
务器响应 200 （返回数据）。


