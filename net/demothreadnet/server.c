#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char *msg, int len);
void error_handling(char * msg);
int clnt_cnt = 0;
struct thread_data
{
    int clnt_sock;
    int thread_id;
};
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;
    struct thread_data my_data;
    my_data.thread_id=1000;

    if (argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    //创建互斥量
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); //阻断，监听客服端连接请求

        //临界区
        pthread_mutex_lock(&mutx); //加锁
        my_data.thread_id++;
        my_data.clnt_sock=clnt_sock;
        clnt_socks[clnt_cnt++] = clnt_sock; //新连接的客服端保存到clnt_socks数组里
        pthread_mutex_unlock(&mutx); //释放锁

        //创建线程
        pthread_create(&t_id, NULL, handle_clnt, (void*)&my_data);
        pthread_detach(t_id); //销毁线程，线程return后自动调用销毁，不阻断

        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }

    close(serv_sock);
    return 0;
}

//线程执行
void * handle_clnt(void * arg)
{
    int clnt_sock = *((int *)arg);
    int str_len = 0, i;
    char msg[BUF_SIZE];

    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    //从数组中移除当前客服端
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
    {
        if (clnt_sock == clnt_socks[i])
        {
            while (i++ < clnt_cnt - 1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

//向所有连接的客服端发送消息
void send_msg(char * msg, int len)
{
    int i;
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}