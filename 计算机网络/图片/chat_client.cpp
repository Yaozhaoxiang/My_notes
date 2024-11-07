#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
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
#define BUF_SIZE 1024
void error_handling(const char *message);
int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr;
    if(argc!=3){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sockfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("connect() error");
    }else{
        puts("Connecting...");
    }

    pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLRDHUP;
    fds[1].revents = 0;

    char read_buf[BUF_SIZE];
    int pipefd[2];
    int ret = pipe(pipefd);
    assert(ret!=-1);

    while(1){
        ret = poll(fds, 2, -1);
        if(ret < 0){
            printf("poll failure \n");
            break;
        }
        if(fds[1].revents & POLLRDHUP){
            printf("server close the connection \n");
            break;
        }else if(fds[1].revents & POLLIN){
            memset(read_buf, '\0', BUF_SIZE);
            recv(fds[1].fd, read_buf, BUF_SIZE-1, 0);
            printf("%s\n", read_buf);
        }
        if(fds[0].revents & POLLIN){
            /* 使用splice将用户输入的数据直接写道sockfd上 零拷贝 */
            ret = splice(0, NULL, pipefd[1], NULL, 32768, SPLICE_F_MOVE | SPLICE_F_MORE);
            ret = splice(pipefd[0], NULL, sockfd, NULL, 32768, SPLICE_F_MOVE | SPLICE_F_MORE);
        }
    }
    close(sockfd);
    return 0;
}
void error_handling(const char *message)
{
    fputs(message,stderr);
    fputc('\n', stderr);
    exit(1);
}

