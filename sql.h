#ifndef _SQL_H
#define _SQL_H
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>

typedef struct user_data
{
	char name[20];
	char addr[15];
	char online;
}user;

int init_mysql(MYSQL *mysql,char *user,char* password);
int mysql_insert(MYSQL *mysql,user user);
int mysql_update(MYSQL *mysql,user user,char value);
#endif
