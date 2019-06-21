#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
char name[50];
char msg[200];

void* send_msg(void *arg);
void* recv_msg(void* arg);
void error_handling(char *message);


int main(int argc, char * argv [ ])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread,rcv_thread;
    void * thread_return;
    if(argc != 4)
    {
        printf("Usage: %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }
    sprintf(name, "[%s]", argv[3]); //聊天人名字，配置到编译器参数里
    sock=socket(PF_INET,SOCK_STREAM,0);
    if(sock==-1)
        error_handling("socket() error\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");
    pthread_create(&snd_thread,NULL,send_msg,(void *)&sock);
    pthread_create(&rcv_thread,NULL,recv_msg,(void *)&sock);
    pthread_join(snd_thread,&thread_return);
    pthread_join(rcv_thread,&thread_return);
    close(sock);
    return 0;
}
void* send_msg(void *arg)
{
    int sock=*(int*)arg;
    char name_msg[256];
    while(1)
    {
        fgets(msg,100,stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q \n")) {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s:%s", name, msg);
        printf("sendmsg=%s\n",name_msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}
void* recv_msg(void* arg)
{
    int sock = *((int *)arg);
    char name_msg[256];
    int str_len;
    while(1)
    {
        str_len = read(sock, name_msg, 255);
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }
    return NULL;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


