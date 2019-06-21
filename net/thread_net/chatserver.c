#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>


void *handle_clnt(void* arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);
int clnt_cnt=0;
struct thread_data
{
    int clnt_sock;
    int thread_id;
};
int clnt_socks[256];
pthread_mutex_t mutx;



int main(int argc,char *argv[])
{
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;
    struct thread_data my_data;
    my_data.thread_id=1000;

    if (argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }
    pthread_mutex_init(&mutx,NULL);
    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr *) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock,5)==-1)
        error_handling("listen() error");
    while(1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
        pthread_mutex_lock(&mutx);
        my_data.thread_id++;
        my_data.clnt_sock=clnt_sock;
        clnt_socks[clnt_cnt++]=clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&my_data);
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void* arg)
{
    struct thread_data *mydata=((struct thread_data*)arg);
    int strlen=0,i=0;
    char msg[256];
    char temp[256];
    while((strlen=read(mydata->clnt_sock,msg,sizeof(msg)))!=0)//当收到客户端发来的close(sock)时会跳出这个 while循环
    {
        sprintf(temp, "%s", msg);
        printf("tid=%d,rcvmsg=%s\n",mydata->thread_id,temp);
        //sprintf(name_msg, "%s  %s", name, msg);
        //printf("rcvmsg=%s\n",msg);
		//pthread_mutex_lock(&mutx);
        //write(mydata->clnt_sock,msg,strlen);
	//	pthread_mutex_unlock(&mutx);
	send_msg(msg, str_len);
    }
    printf("x");
    pthread_mutex_lock(&mutx);
    for(i=0;i<clnt_cnt;i++)
    {
        if(mydata->clnt_sock==clnt_socks[i])
        {
            printf("close %d\n",mydata->thread_id);
            while(i++<clnt_cnt-1)
                clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);//这里一定要unlock，开始笔误写成lock，客户端输入q断开连接后，服务器就会来到这里删除连接
    //但是是lock，没有释放，当客户端再次建立新连接时，在主线程pthread_mutex_lock(&mutx);        my_data.thread_id++;my_data.clnt_sock=clnt_sock;clnt_socks[clnt_cnt++]=clnt_sock;处
    //因为申请不到锁，会卡在那里，所以新连接不能建立，感觉好像客户端只能连一次服务端
    close(mydata->clnt_sock);
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
    fputc('\n',stderr);
    exit(1);
}
