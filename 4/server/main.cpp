#include <QCoreApplication>
#include<lilin.h>

#define PORT 8888
#define MAXLINE 1024
#define MAXLISTEN 20

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    int listenfd,connfd,client[MAXLISTEN],sockfd;
    int maxfd,i,k,n=0,len;
    char buff[MAXLINE+1];
    struct sockaddr_in servaddr,cliaddr;



    fd_set allset;
    socklen_t clilen;

    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        printf("socket error :%s\n",strerror(errno));
        exit(1);
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    for(i=0;i<MAXLISTEN;i++)
    {
        client[i]=-1;
    }

    if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==-1)
    {
        printf("bind error :%s\n",strerror(errno));
        exit(1);
    }

    if(listen(listenfd,MAXLISTEN)==-1)
    {
        printf("listen error :%s\n",strerror(errno));
        exit(1);
    }

    maxfd=listenfd;
    printf("等待客户端连接... ...\n");
    while(1)
    {
        FD_ZERO(&allset);
        FD_SET(listenfd,&allset);
        for(i=0;client[i]>0;i++)
        {
            FD_SET(client[i],&allset);
        }

        select(maxfd+1,&allset,NULL,NULL,NULL);
        if(FD_ISSET(listenfd,&allset))
        {
            printf("有新用户正在连接服务器... ...\n");
            clilen=sizeof(cliaddr);
            if((connfd=accept(listenfd,(struct sockaddr *)&cliaddr,&clilen))==-1)
            {
                printf("accept error:%s",strerror(errno));
                exit(1);
            }
            for(i=0;i<MAXLINE;i++)
            {
                if(client[i]<0)
                {
                    client[i]=connfd;
                    break;
                }
            }
            if(i==MAXLISTEN)
            {
                printf("人数已经达到服务器允许的上限！无法再建立新的连接！\n");
                break;
            }
            else{
            printf("新用户连接服务器成功！获取客户端信息:");
            printf("IP:%s  ,端口：%d  ,套接字：%d\n",inet_ntoa(cliaddr.sin_addr),
                   ntohs(cliaddr.sin_port),client[i]);
            FD_SET(connfd,&allset);
            if(connfd>maxfd)
                maxfd=connfd;
            n++;

            printf("目前服务器在线人数：%d\n",n);
            }
        }
        for(i=0;i<MAXLISTEN;i++)
        {
            if((sockfd=client[i])<0)
            {
                continue;
            }
            else
            {
                if(FD_ISSET(sockfd,&allset))
                {
                    memset(buff,0,sizeof(buff));


                    len=recv(sockfd,buff,sizeof(buff),0);

                    if(len<=0)
                    {
                        printf("用户 已经掉线\n");
                        close(sockfd);
                        client[i]=-1;

                        FD_CLR(sockfd,&allset);
                        n--;
                    }
                    else
                    {
                        printf("%s",buff);
                        for(k=0;client[k]!=-1;k++)
                        {
                            send(client[k],buff,len,0);
                        }

                        memset(buff,0,sizeof(buff));
                        break;
                    }
                }
            }
        }

    }
    return a.exec();
}
