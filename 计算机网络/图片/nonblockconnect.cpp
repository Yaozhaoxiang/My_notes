#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    
    return 0;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 设置非阻塞模式
    if (set_nonblocking(sockfd) == -1) {
        close(sockfd);
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 尝试连接
    int ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0 && errno != EINPROGRESS) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    // 连接正在进行中
    if (errno == EINPROGRESS) {
        printf("Connecting...\n");

        fd_set write_fds;
        FD_ZERO(&write_fds);
        FD_SET(sockfd, &write_fds);

        struct timeval tv;
        tv.tv_sec = 5;  // 超时5秒
        tv.tv_usec = 0;

        // 使用 select 等待连接完成
        ret = select(sockfd + 1, NULL, &write_fds, NULL, &tv);
        if (ret > 0 && FD_ISSET(sockfd, &write_fds)) {
            int so_error;
            socklen_t len = sizeof(so_error);
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
            if (so_error == 0) {
                printf("Connection established!\n");
            } else {
                printf("Connection failed: %s\n", strerror(so_error));
                close(sockfd);
                return -1;
            }
        } else {
            printf("Connection timed out or failed.\n");
            close(sockfd);
            return -1;
        }
    } else {
        printf("Connection established immediately.\n");
    }

    // 连接成功后可以继续读写数据
    // ...

    close(sockfd);
    return 0;
}
