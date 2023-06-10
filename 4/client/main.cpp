#include <QCoreApplication>
#include<lilin.h>
#include<time.h>

#define PORT 8888
#define MAXLINE 1024

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int sockfd,maxfd,len;
    char msg1[MAXLINE+1];
    char msg2[MAXLINE+1];
    char msg3[MAXLINE+1];
    char name[40];

    static char timestr[40];

    struct sockaddr_in servaddr;

    time_t t;
    struct tm *nowtime;
    fd_set allset;

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        printf("socket error :%d %s\n",errno,strerror(errno));
        exit(1);
    }




    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    printf("正在连接服务器... ...\n");
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==-1)
    {
        printf("connect error :%s\n",strerror(errno));
        exit(1);
    }
    printf("连接服务器成功！\n");
    printf("请输入你的昵称：");
    scanf("%s",name);
    printf("%s 欢迎来到聊天室！(quit->退出)\n",name);

    maxfd=sockfd;

    while(1)
    {
        FD_ZERO(&allset);
        FD_SET(sockfd,&allset);
        FD_SET(0,&allset);
        memset(msg1,0,sizeof(msg1));
        memset(msg2,0,sizeof(msg1));
        memset(msg3,0,sizeof(msg1));

        select(maxfd+1,&allset,NULL,NULL,NULL);
        if(FD_ISSET(0,&allset))
        {
            fgets(msg1,sizeof(msg1),stdin);
            if(strcmp(msg1,"quit")==0)
            {
                close(sockfd);
                printf("已经断开与服务器的连接，退出聊天室！\n");
            }
            else
            {
                time(&t);
                nowtime=localtime(&t);
                strftime(timestr,sizeof(timestr),"%Y-%M-%D %H:%M:%S",nowtime);
                snprintf(msg2,sizeof(msg2),"%s %s:%s",timestr,name,msg1);

                send(sockfd,msg2,sizeof(msg2),0);
            }
        }
        if(FD_ISSET(sockfd,&allset))
        {
            len=recv(sockfd,msg3,sizeof(msg3),0);
            if(len<=0)
            {
                close(sockfd);
                printf("服务器出现故障，已经断开连接！\n");
            }
            else
            {
                printf("%s",msg3);
            }
        }
    }



    return a.exec();
}
