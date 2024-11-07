#define _XOPEN_SOURCE 700
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<assert.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<fcntl.h>
#include<poll.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<sys/epoll.h>

constexpr int USER_LIMIN = 5; /* 最大用户数量 */
constexpr int BUFFER_SIZE = 64; /* 读缓冲区大小 */
constexpr int FD_LIMIT = 65535; /* 文件描述符数量限制 */
/* 客户端数据： 
    客户端socket地址，
    代写到客户端的数据位置，
    从客户端读入的数据 */
struct client_data
{
    sockaddr_in address;
    char* write_buf;
    char buf[BUFFER_SIZE];
};
int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return -1;
    }
    
    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl(F_SETFL)");
        return -1;
    }
    
    return flags;
}
void error_handling(const char *message);
int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    int option;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t oplen,clnt_addr_sz;
    if(argc <= 1){
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
    client_data* users = new client_data[FD_LIMIT];
    /* 限制用户数量 */
    pollfd fds[USER_LIMIN + 1];
    int user_counter = 0;
    for(int i = 1; i <= USER_LIMIN ; ++i){
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    fds[0].fd = serv_sock;
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;
    int ret;
    while(1){
        ret = poll(fds, user_counter+1, -1);
        if(ret < 0){
            error_handling("poll() error");
        }
        for(int i=0; i<user_counter+1;++i){
            if((fds[i].fd==serv_sock) && (fds[i].revents & POLLIN)){
                clnt_addr_sz = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
                if(clnt_sock ==-1){
                    error_handling("accept() error");
                }else{
                    printf("Connected client %d \n",i+1);
                }
                /* 如果请求太多，则关闭新到的连接 */
                if(user_counter >= USER_LIMIN){
                    const char* info = "too many users\n";
                    printf("%s", info);
                    send(clnt_sock, info, strlen(info), 0);
                    close(clnt_sock);
                    continue;
                }
                /* 对于新来的连接，同时修改 fds users */
                user_counter++;
                users[clnt_sock].address = clnt_addr;
                set_nonblocking(clnt_sock);
                fds[user_counter].fd = clnt_sock;
                fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
                fds[user_counter].revents = 0;
                printf("come a new user, now have %d users\n",user_counter);
            }else if(fds[i].revents & POLLERR){
                printf("get an error from %d\n", fds[i].fd);
                char errors[100];
                memset(errors, '\0', 100);
                socklen_t len = sizeof(errors);
                if(getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, &errors, &len) < 0){
                    printf("get socket optin failed\n");
                }
                continue;
            }else if(fds[i].revents & POLLRDHUP){
                /* 如果客户端关闭连接，则服务器也关闭对应的连接，并将用户总数减1
                直接覆盖即可 */
                users[fds[i].fd] = users[fds[user_counter].fd];
                close(fds[i].fd);
                fds[i] = fds[user_counter];
                i--;
                user_counter--;
                printf("a client left\n");
            }else if(fds[i].revents & POLLIN){
                int connfd = fds[i].fd;
                memset(users[connfd].buf, '\0', BUFFER_SIZE);
                ret = recv(connfd, users[connfd].buf, BUFFER_SIZE-1, 0);
                printf("get %d byters of client data %s from %d\n", ret, users[connfd].buf, connfd);
                if(ret < 0){
                    /* 如果读取错误，则关闭连接 */
                    if(errno != EAGAIN){
                        close(connfd);
                        users[fds[i].fd] = users[fds[user_counter].fd];
                        fds[i] = fds[user_counter];
                        i--;
                        user_counter--;
                    }
                }else if(ret == 0){

                }else{
                    /* 如果接受到客户数据，则通知其他socket连接准备写数据 */
                    for(int i=1; i<=user_counter;++i){
                        if(fds[i].fd==connfd){
                            continue;
                        }
                        fds[i].events |= ~POLLIN;
                        fds[i].events |= POLLOUT;
                        users[fds[i].fd].write_buf = users[connfd].buf;
                    }
                }
            }else if(fds[i].revents & POLLOUT){
                int connfd = fds[i].fd;
                if(!users[connfd].write_buf){
                    continue;
                }
                ret = send(connfd, users[connfd].write_buf, strlen(users[connfd].write_buf), 0);
                users[connfd].write_buf = NULL;
                fds[i].events |= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }
    delete [] users;
    close(serv_sock);
    return 0;
}
void error_handling(const char *message)
{
    fputs(message,stderr);
    fputc('\n', stderr);
    exit(1);
}
