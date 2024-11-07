#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<assert.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<sys/epoll.h>

constexpr int MAX_EVENT_NUMBER = 1024;
constexpr int BUFFER_SIZE = 1024;
/* 用来传参，pthread */
struct fds{
    int epollfd;
    int sockfd;
};

int setnonblockingmode(int fd)
{
    int flag=fcntl(fd,F_GETFL,0);
    if(flag==-1){
        perror("fctnl");
        return -1;
    }
    if(fcntl(fd,F_SETFL,flag | O_NONBLOCK)==-1){
        perror("fcntl");
        return -1;
    }
    return 0;
}
/* 将 fd 上的 EPOLLIN 和 EPOLLET 事件注册到epollfd指示的epoll内核事件表中，
参数oneshot指定是否注册fd上的 EPOLLONESHOT */
void addfd(int epollfd, int fd, bool oneshot){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if(oneshot){
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblockingmode(fd);
}
/* 重置fd上的事件。这样操作之后，尽管fd上的EPOLLONESHOT事件被注册，但是操作系统仍然会触发
fd上的EPOLLIN事件，且制备触发一次 */
void reset_oneshot(int epollfd, int fd){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}
/* 工作线程 */
void* worker(void* arg){
    int sockfd = ((fds*)arg)->sockfd;
    int epollfd = ((fds*)arg)->epollfd;
    printf("start new thread to receive data on fd: %d\n", sockfd);
    char buf[BUFFER_SIZE];
    memset(buf, '\0', BUFFER_SIZE);
    /* 循环读取sockfd上的数据，直到遇到 EAGAIN */
    while(1){
        int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
        if(ret == 0){
            close(sockfd); /* 这里没有从epoll删除？ */
            printf("foreiner closed the connection\n");
            break;
        }else if(ret < 0){     
            if(errno == EAGAIN){  
                reset_oneshot(epollfd, sockfd);
                printf("read ;ater\n");
                break;
            }
        }else{
            printf("get content: %s\n", buf);
            sleep(5);
        }    
    }
    printf("end thread receiving data on fd: %d\n", sockfd);
    return nullptr;
}
void error_handling(const char *message);
int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    int option;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t oplen,clnt_addr_sz;
    if(argc<=1){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(serv_sock >= 0);

    //禁止time-wait状态
    oplen=sizeof(option);
    option=1;
    if(setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR,(void*)&option,oplen)){
        perror("setsocket");
        close(serv_sock);
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("bind() error");
    }

    if(listen(serv_sock, 5)==-1){
        error_handling("listen() error");
    }

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    /* 注意：监听serv-sock是不能注册EPOLLONESHOT的，否则应用程序只能处理一个客户端
    因为后面的客户请求将不再触发EPOLLIN事件 */
    addfd(epollfd, serv_sock, false);

    while(1){
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if(ret<0){
            perror("epoll_wait");
            break;
        }
        for(int i=0;i<ret;i++){
            int sockfd = events[i].data.fd;
            if(sockfd==serv_sock){
                clnt_addr_sz = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
                if(clnt_sock ==-1){
                    error_handling("accept() error");
                }else{
                    printf("Connected client %d \n",i+1);
                }
                addfd(epollfd, clnt_sock, true);
            }else if(events[i].events & EPOLLIN){
                pthread_t thread;
                fds fds_for_new_worker;
                fds_for_new_worker.epollfd = epollfd;
                fds_for_new_worker.sockfd = sockfd;
                /* 新启动一个线程为sockfd服务 */
                pthread_create(&thread, NULL, worker, (void*)&fds_for_new_worker);
            }else{
                printf("something else happened \n");
            }
        }
    }
    close(serv_sock);
    return 0;
}
void error_handling(const char *message)
{
    fputs(message,stderr);
    fputc('\n', stderr);
    exit(1);
}







