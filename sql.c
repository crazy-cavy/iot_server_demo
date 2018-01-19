#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

int main(int argc,char **argv)
{
	//定义连接句柄
	MYSQL conn ={0};

	//检查用户输入
	if(argc!=4)
	{
		fprintf(stderr,"missing operand\n");
		exit(-1);
	}

	//初始化连接句柄
	if(!mysql_init(&conn))
	{
	 	//初始化失败
		fprintf(stderr,"mysql_init() failed:%s\n",mysql_error(&conn));
		exit(-1);
	}
	else
	{
		printf("mysql_init():successful\n");
	}
	
	//连接数据库
	if(!mysql_real_connect(	&conn,
				"localhost",
				argv[1],
				argv[2],
				argv[3],
				3306,
				NULL,
				0)
	)
	{
		//连接数据库失败
		fprintf(stderr,"mysql_real_connect() failed:%s\n",mysql_error(&conn));
	}
	else
	{
		//连接数据库成功
		printf("mysql_real_connect():successful\n");
	}
	//断开并关闭数据库连接
	mysql_close(&conn);

	exit(0);
}


