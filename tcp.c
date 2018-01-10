/*DATA:			2015-4-20
 *AUTHOR:		WJ
 *DESCRIPTION:	多线程TCP服务器
 *	1、int socket(int domain, int type, int protocol);
 *	2、int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
 *	3、int listen(int sockfd, int backlog);
 *	4、int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
 *	5、read、write
 */	
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE 4096

pthread_t ntid1,ntid2;

struct addr
{
	struct sockaddr_in *h_svaddr;
	struct sockaddr_in *h_claddr;
	struct sockaddr_in *s_svaddr;
	struct sockaddr_in *s_claddr;
}addr1,addr2;

void *tcp_fun(void *arg)
{
	struct addr *addr = (struct addr *)arg;
	struct sockaddr_in *svaddr = (struct sockaddr_in *)addr->h_svaddr;//服务器1Socket地址
	struct sockaddr_in *claddr = (struct sockaddr_in *)addr->h_claddr;//客户端1Soclet地址
	struct sockaddr_in *svaddr2 = (struct sockaddr_in *)addr->s_svaddr;//服务器2Socket地址
	struct sockaddr_in *claddr2 = (struct sockaddr_in *)addr->s_claddr;//客户端2Soclet地址

	int lfd=0,cfd=0;		//监听Socket与客户端套接字描述符
	socklen_t addrlen = sizeof(struct sockaddr_in);//套接字地址大小
	char buf[BUFSIZE]={0};

	//1.创建监听 Socket
	if((lfd=socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket()"),exit(-1);
	}
	
	//2.配置Soket地址
	svaddr->sin_family = AF_INET;
	svaddr->sin_port = htons(9999);
//	svaddr.sin_addr.s_addr = inet_addr("119.23.27.79");
	
	svaddr->sin_addr.s_addr = inet_addr("192.168.1.179");
	//3.绑定Socket文件
	if(bind(lfd,(struct sockaddr *)svaddr,sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind"),exit(1);
	}
	
	//4.监听连接
	if(listen(lfd,4) == -1)
	{
		perror("listen()"),exit(1);
	}

	//5.接受连接
	if((cfd=accept(lfd,(struct sockaddr *)claddr,&addrlen)) == -1)
	{
		perror("accept()"),exit(1);
	}
	else 
	{
		printf("receive data from:%s:%d\n",inet_ntoa(claddr->sin_addr
		),ntohs(claddr->sin_port));
	}
	//主循环，等待连接
	while(1)
	{
		//6.接收/发送数据
		//6.1 接受数据
		if(recv(cfd,buf,BUFSIZE,0) == -1)
		{
			perror("recv()"),exit(-1);
		}	
		//6.2 发送数据
		else
			send(cfd,"ok!",strlen("ok!"),0);
		
		//6.3 打印数据
		printf("%s:%u\n",inet_ntoa(claddr->sin_addr),ntohs(claddr->sin_port));
		printf("%s\n",buf);
		memset(buf,'\0',sizeof(buf));
	}

	//7.关闭Socket	
	close(cfd);	
	close(lfd);

}

int main(int argc, char **argv)
{

	struct sockaddr_in *svaddr1 = {0};//服务器1Socket地址
	struct sockaddr_in *claddr1 = {0};//客户端1Soclet地址
	struct sockaddr_in *svaddr2 = {0};//服务器2Socket地址
	struct sockaddr_in *claddr2 = {0};//客户端2Soclet地址
	struct addr addr1={svaddr1,claddr1,svaddr2,claddr2};
	struct addr addr2={svaddr2,claddr2,svaddr1,claddr1};


	int err;
	err = pthread_create(&ntid1,NULL,tcp_fun,(void *)&addr1);
	if(err!=0)
	{
		printf("can't create thread1:%s\n",strerror(err));	
	}
	printf("create thread1 sucess!\n");
	err = pthread_create(&ntid2,NULL,tcp_fun,(void *)&addr2);
	if(err!=0)
	{
		printf("can't create thread1:%s\n",strerror(err));	
	}
	printf("create thread2 sucess!\n");
	while(1)
	{
	
	}
	return 0; 


}
 
