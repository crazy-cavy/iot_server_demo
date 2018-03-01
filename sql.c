#include "sql.h"

int main(int argc,char **argv)
{

	//检查用户输入
	if(argc != 3)
	{
		fprintf(stderr,"missing operand\n");
		exit(-1);
	}
	
	//定义连接句柄
	MYSQL mysql ={0};
	//初始化数据库
	init_mysql(&mysql,argv[1],argv[2]);


	user user = {"cavy","192.168.1.1",1};
	//插入数据
	mysql_insert(&mysql,user);
	
	show_table(&mysql,"user");
	//update
	mysql_update(&mysql,user,!user.online);

	show_table(&mysql,"user");
	//delete
	mysql_delete(&mysql,user);

	show_table(&mysql,"user");
	//mysql数据库连接
	mysql_close(&mysql);

	exit(0);
}

int init_mysql(MYSQL *mysql,char *user,char* password)
{
	const char *dbname = "iot_server_db";
	
	//初始化连接句柄
	if(!mysql_init(mysql))
	{
	 	//初始化失败
		fprintf(stderr,"mysql_init() failed:%s\n",mysql_error(mysql));
		exit(-1);
	}
	else
	{
		printf("mysql_init():successful\n");
	}
	
	
	//连接数据库
	if(!mysql_real_connect(mysql, "localhost", user, password, dbname, 3306, NULL, 0)
	)
	{
		//连接数据库失败
		fprintf(stderr,"mysql_real_mysqlect() failed:%s\n",mysql_error(mysql));
		return -1;
	}
	else
	{
		//连接数据库成功
		printf("mysql_real_mysqlect():successful\n");
	}
	return 0;
}

static int sql_command(MYSQL *mysql,user user,const char *str)
{

	char flag = 0;
	char *command = NULL;
	command = malloc(sizeof(user)+strlen(str)+15);
	memset(command,0,sizeof(user)+strlen(str)+15);
	if(!strncmp(str,"Insert",6))
	{
		sprintf(command,"%s('%s','%s','%d')\n",str,user.name,user.addr,user.online);
	}
	else if(!strncmp(str,"Delete",6))
	{
		sprintf(command,"%s'%s'\n",str,user.name);
	}
	else 
		return -2;
	#ifdef DEBUG
		printf("command : %s\n",command);
	#endif
	/*命令成功则返回0*/  
	flag = mysql_real_query(mysql, command, (unsigned int)strlen(command));  
	
	if(flag) 
	{  
		free(command);
		return -1;  
	}

	free(command);
	command = NULL;
	return 0;
}
int mysql_insert(MYSQL *mysql,user user)
{
	char flag = 0;
	char *str = "Insert into user values";
	flag = sql_command(mysql,user,str);
	
	#ifdef DEBUG
	if(flag) {  
		printf("Insert data failure!\n");  
		return -1;  
	}
	else 
	{  
		printf("Insert data success!\n");  
	}
	#endif

	return 0;
}

int mysql_delete(MYSQL *mysql,user user)
{
	char flag = 0;
	char *str = "Delete from user where name=";
	flag = sql_command(mysql,user,str);
	
	#ifdef DEBUG
	if(flag) 
	{
		printf("delete data failure!\n");
		return -1;
	}
	else 
	{
		printf("delete data success!\n");
	}
	#endif

	return 0;

}

int mysql_update(MYSQL *mysql,user user,char value)
{
	char flag = 0;
	char command[50];

	sprintf(command,"Update user set online=%d where name='%s'\n",value,user.name);

	flag = mysql_real_query(mysql, command, (unsigned int)strlen(command));  
	
	#ifdef DEBUG
	printf("command : %s",command);
	if(flag) 
	{
		printf("update data failure!\n");
		return -1;
	}
	else 
	{
		printf("update data success!\n");
	}
	#endif

	return 0;

}

int show_table(MYSQL *mysql,char *table_name)
{
	MYSQL_RES *res;  
	MYSQL_ROW row;
	//放置sql语句
	char command[50];

	sprintf(command,"select * from %s\n",table_name);

	/*查询，成功则返回0*/  
	if( mysql_real_query(mysql, command, (unsigned int)strlen(command)) )
	{  
		printf("Query failed!\n");  
		return 0;  
	}
	else 
	{  
		printf("[%s] made...\n", command);  
	}

	/*mysql_store_result讲所有的查询结果读取到client*/  
	res = mysql_store_result(mysql);  
	/*mysql_fetch_row检索结果集的下一行*/  
	while(row = mysql_fetch_row(res)) 
	{  
		/*mysql_num_fields返回结果集中的字段数目*/  
		int t;
		for(t=0; t<mysql_num_fields(res); t++)  
		{  
			printf("%s\t", row[t]);  
		}  
		printf("\n");  
	}  


}





