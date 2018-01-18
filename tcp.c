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
#define CFD_MAX 2
#define TID_MAX 100

pthread_t ntid[TID_MAX];
pthread_mutex_t mutex;

struct addr
{
	struct sockaddr_in host_claddr;
	struct sockaddr_in net_claddr;
	int *host_cfd;
	int *net_cfd;
};
void *tcp_recv(void *arg)
{
	struct addr *addr = (struct addr *)arg;

	socklen_t addrlen = sizeof(struct sockaddr_in);//套接字地址大小
	char buf[BUFSIZE]={0};

	printf("cfd=%d\n",*(addr->host_cfd));
	//4.监听连接

	//主循环，等待连接
	while(1)
	{
		//6.接收/发送数据
		//6.1 接受数据
		if(recv(*(addr->host_cfd),buf,BUFSIZE,0) == -1)
		{
			perror("recv()"),exit(-1);
		}	
		//6.2 发送数据
		else
		{
			pthread_mutex_lock(&mutex);

			printf("net_cfd is %s:%u\n",inet_ntoa(addr->net_claddr.sin_addr),ntohs(addr->net_claddr.sin_port));
			if(*(addr->net_cfd) == 0)
			{
				printf("hasn't net_client\n");
				continue;
			}
			else
			{
				send(*(addr->host_cfd),"ok!",strlen("ok!"),0);
				send(*(addr->net_cfd),buf,strlen(buf),0);
			}

			//6.3 打印数据
			printf("host_cfd is %s:%u\n",inet_ntoa(addr->host_claddr.sin_addr),ntohs(addr->host_claddr.sin_port));

			pthread_mutex_unlock(&mutex);

			printf("%s\n",buf);
			memset(buf,'\0',sizeof(buf));
		}
	}

	//7.关闭Socket	
	close(*(addr->host_cfd));	

}
void tcp_init()
{

	int lfd=0,cfd[CFD_MAX]={0};		//监听Socket与客户端套接字描述符 
	int cfd_num=0;
	struct sockaddr_in svaddr = {0};
	socklen_t addrlen = sizeof(struct sockaddr_in);//套接字地址大小
	char buf[BUFSIZE]={0};
	int port = 9000; 

	struct sockaddr_in claddr1 ={0};//客户端1Soclet地址
	struct sockaddr_in claddr2 ={0};//客户端2Soclet地址
	struct addr addr[2]={ {claddr1,claddr2,&cfd[0],&cfd[1]},
			      {claddr2,claddr1,&cfd[1],&cfd[0]},};
	printf("%p %p\n",(addr[0].host_cfd),(addr[0].net_cfd));
	printf("%p %p\n",(addr[1].host_cfd),(addr[1].net_cfd));



	//1.创建监听 Socket
	if((lfd=socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket()"),exit(-1);
	}
	
	//2.配置Soket地址
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(port);
	svaddr.sin_addr.s_addr = inet_addr("192.168.1.179");

	// 设置套接字选项避免地址使用错误  
	int on=1;  
	if((setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
	{  
		perror("setsockopt failed");  
		exit(EXIT_FAILURE);  
	}

	//3.绑定Socket文件
	if(bind(lfd,(struct sockaddr *)&(svaddr),sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind"),exit(1);
	}
	
	while(1)
	{
		if(cfd_num < CFD_MAX)
		{
			//4.监听连接
			if(listen(lfd,CFD_MAX) == -1)
			{
				perror("listen()"),exit(1);
			}

			//5.接受连接
			
			//pthread_mutex_lock(&mutex);

			if((*(addr[cfd_num].host_cfd)=accept(lfd,(struct sockaddr *)&(addr[cfd_num].host_claddr),&addrlen)) == -1)
			{
				perror("accept()"),exit(1);
			}
			else 
			{
				printf("receive data from:%s:%d\n",inet_ntoa(addr[cfd_num].host_claddr.sin_addr
							),ntohs(addr[cfd_num].host_claddr.sin_port));
				printf("send data to:%s:%d\n",inet_ntoa(addr[cfd_num].net_claddr.sin_addr
							),ntohs(addr[cfd_num].net_claddr.sin_port));

				printf("cfd=%d\n",*(addr[cfd_num].host_cfd));
				
				int err;
				err = pthread_create(&ntid[cfd_num],NULL,tcp_recv,(void *)&addr[cfd_num]);
				if(err!=0)
				{
					printf("can't create thread:%s\n",strerror(err));	
				}
				printf("create thread sucess!\n");
				printf("cfd_num=%d!\n",cfd_num);
				cfd_num++;
			}

			//pthread_mutex_unlock(&mutex);
		}
		else
		{
			cfd_num=0;
		}
	}

	close(lfd);	

}


int main(int argc, char **argv)
{

//	struct sockaddr_in svaddr1;//服务器1Socket地址
//	struct sockaddr_in claddr1;//客户端1Soclet地址
//	struct sockaddr_in svaddr2;//服务器2Socket地址
//	struct sockaddr_in claddr2;//客户端2Soclet地址
//	int cfd1,cfd2;
//	struct addr addr1={svaddr1,claddr1,svaddr2,claddr2,cfd1,cfd2};
//	struct addr addr2={svaddr2,claddr2,svaddr1,claddr1,cfd2,cfd1};

	tcp_init();
	//int err;
//	err = pthread_create(&ntid1,NULL,tcp_fun,(void *)&addr1);
//	if(err!=0)
//	{
//		printf("can't create thread1:%s\n",strerror(err));	
//	}
//	printf("create thread1 sucess!\n");
//	err = pthread_create(&ntid2,NULL,tcp_fun,(void *)&addr2);
//	if(err!=0)
//	{
//		printf("can't create thread1:%s\n",strerror(err));	
//	}
//	printf("create thread2 sucess!\n");
	while(1)
	{
		sleep(3);	
	}
	return 0; 


}
 
