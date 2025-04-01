# docker



## docker 的使用

总体框架：

![](./图片/docker_1.png)

什么是容器：

![](./图片/docker_2.png)


## 1. 怎么添加国内镜像源

查看安装的Docker版本 

```bash
sudo docker version
```

1. 添加国内镜像源： 

首先运行 sudo vim /etc/docker/daemon.json 来创建daemon.json文件，然后将以下内容复制到daemon.json文件中。

```bash
{
    "registry-mirrors": [
        "https://registry.docker-cn.com",
        "http://hub-mirror.c.163.com",
        "https://docker.mirrors.ustc.edu.cn",
        "https://kfwkfulq.mirror.aliyuncs.com"
    ]
}
```

2. 重启Docker

sudo service docker restart

3. 查看添加的国内源是否生效

sudo docker info | grep Mirrors -A 4

```bash
root@hcss-ecs-796d:~# sudo docker info | grep Mirrors -A 4
 Registry Mirrors:
  https://registry.docker-cn.com/
  http://hub-mirror.c.163.com/
  https://docker.mirrors.ustc.edu.cn/
  https://kfwkfulq.mirror.aliyuncs.com/
```

4. 设置开机启动docker

systemctl enable docker


https://blog.csdn.net/2301_79849395/article/details/142829852

## 2. 使用

下载镜像：

检索： docker search
下载： docker pull
列表： docker images
删除： docker rmi 
    docker rmi image_id 或者 docker rmi name:tag

在搜索镜像时加上国内源的域名，这样就不用梯子也可以:
docker search docker-0.unsee.tech/mysql:5.7


启动容器：
运行：docker run
    -d: 后台启动
    --name: 容器名字
    -p 88:80: 端口映射,外部88->内部80
    --network: 加入自定义网络
查看：docker ps -a
停止：docker stop
启动：docker start
重启：docker restart
状态：docker stats
日志：docker logs
进入：docker exec
    -it ：交互模式 
    /bin/bash : bash界面
删除：docker rm


保存镜像：
提交：docker commit
保存：docker save
加载：docker load

**注意：只有服务器安全组设置可以访问端口，才可以访问；**

## 3. dockers存储

目录挂载,外部的目录挂到内部的目录上：
-v /app/nghtml:/usr/share/nginx/html

这样启动的时候，外面的目录就是容器内部的目录；如果外面是空的，那么就认为是空的。


卷映射： -v ngconf:/etc/nginx
卷也就是不以 "/" 开始;
这个卷的存放位置就是：/var/lib/docker/volumes/<volume-name>
卷映射可以看成，外部刚开始就是内部的一个映射。也就是说刚开始就是复制过来的。

列出所有的卷：`docker volume ls`
列出卷的详情：`docker volume inspect name`
删除卷：`docker volume rm `

## 4. docker 网络

docker0 默认不支持主机域名；
创建自定义网络，容器名就是稳定域名；

创建自定义网络： docker network create mynet
查看： docker network ls
查看网络：docker inspect name

这样后续就可以 通过域名访问了；

## 5. 启动一个redix主从同步集群

![](./图片/docker_3.png)

为了方便配置redis集群，可以直接使用：bitnami/redis 镜像；
在这里只需要配置镜像环境变量即可

```bash
root@hcss-ecs-796d:~/app# docker run -d -p 6379:6379 \
> -v /app/rd1:/bitnami/redis/data \
> -e REDIS_REPLICATION_MODE=master \
> -e REDIS_PASSWORD=123456 \
> --network mynet --name redis01 \
> bitnami/redis
```

\ :表示换行；
-e : 添加环境变量；

注意要更改权限： chmod -R 777 rd2;


从机配置：
```bash
root@hcss-ecs-796d:/app# docker run -d -p 6380:6379 \
> -v /app/rd2:/bitnami/redis/data \
> -e REDIS_REPLICATION_MODE=slave \
> -e REDIS_MASTER_HOST=redis01 \
> -e REDIS_MASTER_PORT_NUMBER=6379 \
> -e REDIS_MASTER_PASSWORD=123456 \
> -e REDIS_PASSWORD=123456 \
> --network mynet --name redis02 \
> bitnami/redis
```

## 6. 配置一个容器 

配置一个容器要考虑的地方：
![](./图片/docker_4.png)

比如这里配置一个mysql: 端口 -> 配置 -> 数据 -> 环境变量

```bash
root@hcss-ecs-796d:/app# 
docker run -d -p 3306:3306 
-v /app/myconf:/etc/mysql/conf.d 
-v /app/data:/var/lib/mysql 
-e MYSQL_ROOT_PASSWORD=123456 
mysql
```

## 7. docker compose

用来批量管理容器；

通过配置： compose.yaml

上线：docker compose up -d
下线：docker compose down

启动：docker compose start x1 x2 x3
停止：docker compose stop x1 x3
扩容：docker compose scale x2=3


compose.yaml 的编写：

name: 名字
services:服务
nerworks:网络
volumes:卷
configs:配置
secrets:密钥

其实compose.yaml的编写，和docker run的编写差不多：

![](./图片/docker_5.png)


## 8. dockerfile

制作镜像；

![](./图片/docker_6.png)

![](./图片/docker_7.png)














