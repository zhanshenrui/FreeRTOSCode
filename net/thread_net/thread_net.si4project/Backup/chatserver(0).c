#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
pthread_mutex_t mutx;
int clnt_socks[256];
int clnt_cnt=0;


int main(int argc,char *argv[])
{
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    pthread_t t_id;
    pthread_mutex_init(&mutx,NULL);
    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    memset(serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr *) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock,5)==-1)
        error_handling("lister() error");
    while(1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
        pthread_mutex_lock(mutx);
        clnt_socks[clnt_cnt++]=clnt_sock
        pthread_mutex_unlock(mutx);

        pthread_create(&t_id, NUL, handle_clnt, (void)&clnt_sock);
        pthread_detach(t_id);
        printf("connected from IP:%s\n",inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void* arg)
{
    int clnt_sock=*((int*)arg);
    int strlen=0,i=0;
    char msg[256];
    while(strlen=read(clnt_sock,msg,sizeof(msg)))
    {
        printf("strlen=%d\n",strlen);
        write(clnt_sock,msg,strlen);
    }
    pthread_mutex_lock(mutx);
    for(i=0;i<clnt_cnt;i++)
    {
        if(clnt_sock==clnt_socks[i])
        {
            while(i++<clnt_cnt-1)
                clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_lock(mutx);
    close(clnt_sock);
    return;
}
void error_handling(char *msg)
{
    fputs(msg,stderr);
    fputc('\n',stderr);
    exit(1);
}