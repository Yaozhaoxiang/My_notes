#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<assert.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sendfile.h>

constexpr int BUFFER_SIZE = 4096; /* 读缓冲区大小 */
/* 主状态两种状态：
    1. 当前正在分析请求行
    2. 当前正在分析头字段 */
enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER };
/* 从状态机：
    1. 读取到一个完整的行
    2. 行出错
    3. 行数据尚且不完整 */
enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };
/* 服务器处理 HTTP 请求的结果：
    NO_REQUEST：请求不完整，需要继续读取客户数据
    GET_REQUEST：获得一个完整的客户请求
    BAD_REQUEST：客户请求由语法错误
    FORBIDDEN_REQUEST：客户对资源没有足够的访问权限
    INTERNAL_ERROR：服务器内部错误
    CLOSE_CONNECTION： 客户端关闭连接*/
enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST,
FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSE_CONNECTION };
/* 这里不返回完整的 HTTP请求，只验证服务器处理结果是否成功 */
static const char* szret[] = {"I get a correct result\n", "Something wrong\n"};

/* 从状态机，用于解析出一行的内容 */
LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index){
    char temp;
    /* checked_index指向buffer中当前正在分析的字节，read_index指向buffer中客户数据的尾部下一个字节。
    buffer中0~checked_index都已经被分析完毕，第checked_index~(read_index-1)字节由下面遍历 */
    for(; checked_index < read_index; ++checked_index){
        /* 获取当前要分析的字节 */
        temp = buffer[checked_index];
        /* 如果当前的字节是 \r 则说明可能读取到一个完整的行 */
        if(temp == '\r'){
            /* 如果 \r 字符恰好是目前buffer中的最后一个已经被读入的客户数据，那么这次分析没有读取到一个完整的行，
            返回 LINE_OPEN 一表示还需要继续读取客户端数据 */
            if( (checked_index + 1) == read_index){
                return LINE_OPEN;
            }
            /* 如果下一个是 \n 则说明读取到一个完整的行 */
            else if( buffer[checked_index + 1] == '\n'){
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            /* 否则，说明客户发送的 HTTP 请求语法有问题 */
            return LINE_BAD;
        }
        /* 如果当前字节是 \n ，则说明可能读取到一个完整的行 */
        else if( temp == '\n'){
            if( (checked_index > 1) && buffer[checked_index - 1] == '\r'){
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    /* 如果所有的内容都分析完也没有遇到 \r ，则说明还需要继续读取 */
    return LINE_OPEN;
}

/* 分析请求行 */
HTTP_CODE parse_requestline( char* temp, CHECK_STATE& checkstate){
    /* 使用 strpbrk(temp, " \t") 在请求行中查找第一个空白字符（空格或制表符）。
    空白字符是分隔 HTTP 请求方法（如 GET）和 URL 的标志。 */
    char* url = strpbrk( temp, " \t");
    /* 如果请求行中没有空白字符或 \t 字符，则HTTP请求必有问题 */
    if( !url ){
        return BAD_REQUEST;
    }
    /* *url++ = '\0'; 将空白字符替换为字符串结束符 \0，将 temp 字符串截断，temp 现在只包含请求方法。
    然后，通过 url++ 移动指针，跳过这个空白字符，url 现在指向 URL 部分的起始位置。*/
    *url++ = '\0';

    char* method = temp;
    if( strcasecmp( method, "GET") == 0){
        /* 目前仅支持GET方法 */
        printf( "The request method is GET \n");
    }else{
        return BAD_REQUEST;
    }

    /* url += strspn(url, " \t"); 跳过 URL 开头的空白字符，使 url 指向实际的 URL 字符。 */
    url += strspn( url, " \t");
    /* char* version = strpbrk(url, " \t"); 在 URL 后查找下一个空白字符，该字符分隔 URL 和 HTTP 版本号。 */
    char* version = strpbrk( url, " \t");
    if( !version ){
        return BAD_REQUEST;
    }
    /* 同样地，将空白字符替换为字符串结束符，分割出 URL，并通过 version++ 移动指针，
    跳过这个空白字符，使 version 指向 HTTP 版本号。 */
    *version++ = '\0';
    /* 跳过版本号前面的空白字符。 */
    version += strspn( version, " \t");
    /* 仅支持 HTTP/1.1 */
    if( strcasecmp(version, "HTTP/1.1")!=0){
        return BAD_REQUEST;
    }
    /* 检查url是否合法 */
    if(strncasecmp(url, "http://",7)==0){
        url += 7;
        url = strchr(url, '/');
    }
    /* 如果路径部分未找到 / 或 URL 不合法（例如，URL 为空或第一个字符不是 /），则返回 BAD_REQUEST。 */
    if(!url || url[0]!='/'){
        return BAD_REQUEST;
    }
    printf("The request url is: %s\n",url);
    /* HTTP请求行处理完毕，状态转移到头部字段的分析 */
    checkstate = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

/* 分析头部字段 */
HTTP_CODE parse_headers( char* temp ){
    /* 遇到一个空行，说明得到一个完整正确的HTTP请求 */
    if( temp[0] == '\0' ){
        return GET_REQUEST;
    }else if( strncasecmp(temp, "Host:", 5) == 0){
        temp += 5;
        temp += strspn( temp, " \t");
        printf("the request host is: %s\n",temp);
    }else{
        printf("I can not handle this header\n");
    }
    return NO_REQUEST; 
}

/* 分析 HTTP 请求的入口函数 */
HTTP_CODE parse_content( char* buffer, int& checked_index, CHECK_STATE& checkstate,
int& read_index, int& start_line){
    LINE_STATUS linestatus = LINE_OK; /* 记录当前行读取的状态 */
    HTTP_CODE retcode = NO_REQUEST; /* 记录HTTP请求的处理结果 */
    /* 主状态机，用于从buffer中取出完整的行 */
    while((linestatus = parse_line(buffer, checked_index, read_index)) == LINE_OK){
        char* temp = buffer + start_line; /* 当前起始位置 */
        start_line = checked_index; /* 记录下一行的起始位置 */
        switch (checkstate)
        {
        case CHECK_STATE_REQUESTLINE:
            {
                retcode = parse_requestline( temp, checkstate );
                if( retcode == BAD_REQUEST){
                    return BAD_REQUEST;
                }
                break;
            }        
        case CHECK_STATE_HEADER:
        {
            retcode = parse_headers( temp );
            if( retcode == BAD_REQUEST){
                return BAD_REQUEST;
            }else if(retcode == GET_REQUEST){
                return GET_REQUEST;
            }
            break;
        }
        default:
            return INTERNAL_ERROR;
            break;
        }
    }
    if(linestatus == LINE_OPEN){
        return NO_REQUEST;
    }else{
        return BAD_REQUEST;
    }
}

void error_handling (const char* message);
int main(int argc, char* argv[]){
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

    clnt_addr_sz = sizeof(clnt_addr);
    for(int i=0; i<5;i++){
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
        if(clnt_sock ==-1){
            error_handling("accept() error");
        }else{
            printf("Connected client %d \n",i+1);
        }

        char buffer[BUFFER_SIZE];
        memset(buffer, '\0', BUFFER_SIZE);
        int data_read = 0;
        int read_index = 0;
        int checked_index = 0;
        int start_line = 0;
        CHECK_STATE checkstate =  CHECK_STATE_REQUESTLINE;
        while(1){
            data_read = recv( clnt_sock, buffer + read_index, BUFFER_SIZE - read_index, 0);
            if(data_read == -1){
                printf(" reading failed\n ");
                break;
            }else if( data_read == 0 ){
                printf("remote client has closeed the connection\n");
                break;
            }
            read_index += data_read;
            HTTP_CODE result = parse_content(buffer, checked_index, checkstate, read_index, start_line);
            if(result==NO_REQUEST){
                continue;
            }else if(result == GET_REQUEST){
                send(clnt_sock, szret[0], strlen(szret[0]),0);
                break;
            }else{
                send(clnt_sock, szret[1], strlen(szret[1]),0);
                break;
            }
        }
        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}
void error_handling(const char* message){
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}


