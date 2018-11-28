/*************************************************************************
	> File Name: head.h
	> Author: 
	> Mail: 
	> Created Time: 2018年11月21日 星期三 09时30分37秒
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <mysql/mysql.h>


#define  MSG_FILENAME 1    //传输文件
#define  MSG_ACK 3
#define  MSG_DONE 4        //执行操作完成
#define  MSG_EXCEPTION 5   //操作错误
#define  MSG_SIG 6         //私人聊天
#define  MSG_MULT 7        //多人聊天
#define  MSG_LOGIN 8       //用户登陆
#define  MSG_REGISTRATION 9   //用户注册
#define  MSG_CHANGE  10       //用户密码改变
#define  MSG_NAME   11        //查找用户是否存在
#define  MSG_CONFIRM_VARIFICATION 12     //用户验证信息判断
#define  MSG_NUMBER 13                   //用户人数
#define  MSG_NUMBER_NAME 14              //在线用户名字
#define  MSG_LOGOT  15                   //用户退出
#define  MSG_BAND   16                   //禁言用户
#define  MSG_LIFT   17                   //解禁用户
#define  MSG_KICK   18                   //踢掉用户
#define  MSG_LOGIN_FAILED 19             //用户登陆失败
#define  MSG_NOT_LORD  20                //不是群主




#define  STYLE_LIFT  0                 //用户解禁
#define  STYLE_BAN   1                 //用户禁言
#define  STYLE_LORD  2                 //用户是群主
#define  STYLE_NOT   4                 
struct msg
{
	int type;                    //信息类型
	int len;                     //发送信息长度
	int fd;                      //用户文件描述符
	int style;                   //用户是什么类型,(禁言，解禁，群主)
	char name[50];               //发送到目的用户名称
	char name2[50];              //发送源用户
	char passwd[20];             //用户密码
	char data[256];              //用户信息
	char varification[20];       //用户验证信息
};

struct student
{
	int fd;
	char name[50];
	int style;
	struct student *next;
};
#endif
