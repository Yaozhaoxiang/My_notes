#include "lst_timer.h"

constexpr int FD_LIMIT = 65535; /* 文件描述符数量限制 */
constexpr int MAX_EVENT_NUMBER = 1024;
constexpr int TIMESLOT = 5;

static int pipefd[2];
static sort_timer_lst timer_lst;
static int epollfd = 0;
int set_nonblocking(int sockfd) {
    int old_flags = fcntl(sockfd, F_GETFL, 0);
    if (old_flags == -1) {
        perror("fcntl(F_GETFL)");
        return -1;
    }
    int new_flags = old_flags | O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, new_flags) == -1) {
        perror("fcntl(F_SETFL)");
        return -1;
    }
    return old_flags;
}
void addfd(int epollfd, int fd){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    set_nonblocking(fd);
}
void sig_handler(int sig){
    printf("sig_handler\n");
    int save_error = errno;
    int msg = sig;
    send(pipefd[1], (char*)&msg, 1, 0);
    errno = save_error;
}
void addsig( int sig )
{
    struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset( &sa.sa_mask );
    assert( sigaction( sig, &sa, NULL ) != -1 );
}
void timer_handler(){
    timer_lst.tick();
    alarm(TIMESLOT);
}
void cb_func(client_data* user_data){
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    printf("close fd %d\n", user_data->sockfd);
}
void error_handling(const char *message);
int main( int argc, char* argv[] )
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

    epoll_event events [ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, serv_sock);

    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
    assert( ret != -1 );
    set_nonblocking( pipefd[1] );
    addfd( epollfd, pipefd[0] );

    addsig(SIGALRM);
    addsig(SIGTERM);
    bool stop_server = false;

    client_data* users = new client_data[FD_LIMIT];
    bool timeout = false;
    alarm(TIMESLOT);
    while(!stop_server){
        printf("wait\n");
        int num = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if((num < 0) && (errno != EINTR)){
            printf("epoll failure\n");
            break;
        }

        for(int i=0; i<num; ++i){
            printf("num : %d\n",num);
            int sockfd = events[i].data.fd;
            if(sockfd==serv_sock){
                clnt_addr_sz = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
                if(clnt_sock ==-1){
                    error_handling("accept() error");
                }else{
                    printf("Connected client %d \n",i+1);
                }
                addfd(epollfd, clnt_sock);
                users[clnt_sock].address = clnt_addr;
                users[clnt_sock].sockfd = clnt_sock;
                util_timer* timer = new util_timer;
                timer->user_data = &users[clnt_sock];
                timer->cb_func = cb_func;
                time_t cur = time(NULL);
                timer->expire = cur + 3*TIMESLOT;
                users[clnt_sock].timer = timer;
                timer_lst.add_timer(timer);
            }else if((sockfd==pipefd[0])&&(events[i].events & EPOLLIN)){
                printf("pipefd[0]\n");
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                if(ret==-1){
                    /* 处理错误 */
                    continue;
                }else if(ret ==0 ){
                    continue;
                }else{
                    for(int i=0;i<ret;++i){
                        switch (signals[i])
                        {
                        case SIGALRM:
                            {
                                timeout = true;
                                break;
                            }
                        case SIGTERM:
                        {
                            stop_server = true;
                        }
                        }
                    }
                }

            }else if(events[i].events & EPOLLIN){
                memset( users[sockfd].buf, '\0', BUFFER_SIZE );
                ret = recv( sockfd, users[sockfd].buf, BUFFER_SIZE-1, 0 );
                printf( "get %d bytes of client data %s from %d\n", ret, users[sockfd].buf, sockfd );
                util_timer* timer = users[sockfd].timer;
                if( ret < 0 )
                {
                    if( errno != EAGAIN )
                    {
                        cb_func( &users[sockfd] );
                        if( timer )
                        {
                            timer_lst.del_timer( timer );
                        }
                    }
                }
                else if( ret == 0 )
                {
                    cb_func( &users[sockfd] );
                    if( timer )
                    {
                        timer_lst.del_timer( timer );
                    }
                }
                else
                {
                    send( sockfd, users[sockfd].buf, BUFFER_SIZE-1, 0 );
                    if( timer )
                    {
                        time_t cur = time( NULL );
                        timer->expire = cur + 3 * TIMESLOT;
                        printf( "adjust timer once\n" );
                        timer_lst.adjust_timer( timer );
                    }
                }
            }else{
                // others
            }
        }
        if( timeout )
        {
            printf("timeout\n");
            timer_handler();
            timeout = false;
        }
    }
    close( serv_sock );
    close( pipefd[1] );
    close( pipefd[0] );
    delete [] users;
    return 0;
}
void error_handling(const char *message)
{
    fputs(message,stderr);
    fputc('\n', stderr);
    exit(1);
}


