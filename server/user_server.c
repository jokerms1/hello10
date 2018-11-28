/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2018年11月07日 星期三 20时25分23秒
 ************************************************************************/

#include"./head.h"  
#define BACKLOG 5      
#define BUF_SIZE 200 
  
int fd_A[BACKLOG];  
int conn_amount;  
int login_amout;
int  server_search(char name[])                   //查询用户函数
{
	int i=0,flag = 0;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char server[] = "localhost";
    char user[] = "root";
    char password[] = "12345678";
    char database[] = "mysql";
    char sql[256];
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server,user, password, database, 0, NULL, 0)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    
    sprintf(sql,"select * from user.student where name = '%s'",name);
    if(mysql_query(conn,sql))
    {
    	fprintf(stderr,"%s\n",mysql_error(conn));
    	exit(1);
    }
    res = mysql_use_result(conn);
    if( (row = mysql_fetch_row(res)) != NULL )
    {
    	flag = 1;
    }
    mysql_free_result(res);
    mysql_close(conn);
    return flag;
}

int  server_login(char name[],char passwd[])        //用户在服务器上登陆
{
	int i=0,flag = 0;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char server[] = "localhost";
    char user[] = "root";
    char password[] = "12345678";
    char database[] = "mysql";
    char sql[256];
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server,user, password, database, 0, NULL, 0)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    
    sprintf(sql,"select * from user.student where name = '%s' and passwd = '%s'",name,passwd);
    if(mysql_query(conn,sql))
    {
    	fprintf(stderr,"%s\n",mysql_error(conn));
    	exit(1);
    }
    res = mysql_use_result(conn);
    if( (row = mysql_fetch_row(res)) != NULL )
    {
    	flag = 1;
    }
    
    mysql_free_result(res);
    mysql_close(conn);
    return flag;
}

int  server_registration(char name[],char passwd[],char varification[])     //用户在服务器上注册
{
	int flag = 0;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char server[] = "localhost";
    char user[] = "root";
    char password[] = "12345678";
    char database[] = "mysql";
    char sql[256];
    conn = mysql_init(NULL);
    
    if (!mysql_real_connect(conn, server,user, password, database, 0, NULL, 0)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    
    sprintf(sql,"insert into user.student values('%s','%s','%s')",name,passwd,varification);
    if(mysql_query(conn,sql))
    {
        fprintf(stderr,"%s\n",mysql_error(conn));
        exit(1);
    }
    else
    {
        flag = 1;
    }
    mysql_free_result(res);
    mysql_close(conn);
    return flag;
}

int  server_confirm_varification(char name[],char varification[])       //用户在服务上确认验证信息
{
	int i=0,flag = 0;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char server[] = "localhost";
    char user[] = "root";
    char password[] = "12345678";
    char database[] = "mysql";
    char sql[256];
    
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server,user, password, database, 0, NULL, 0)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    
    sprintf(sql,"select * from user.student where name = '%s' and varification ='%s'",name,varification);
    if(mysql_query(conn,sql))
    {
    	fprintf(stderr,"%s\n",mysql_error(conn));
    	exit(1);
    }
    res = mysql_use_result(conn);
    if( (row = mysql_fetch_row(res)) != NULL )
    {
    	flag = 1;
    }
    
    mysql_free_result(res);
    mysql_close(conn);
    return  flag;
}

int  server_change_password(char name[],char passwd[],char varification[])     //用户在服务器上修改密码
{
	int i=0,flag = 0;
    MYSQL *conn;
    MYSQL_ROW row;
    char server[] = "localhost";
    char user[] = "root";
    char password[] = "12345678";
    char database[] = "mysql";
    char sql[256];
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server,user, password, database, 0, NULL, 0)) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    sprintf(sql,"update user.student set passwd = '%s' where name = '%s'",passwd,name);
    if(mysql_query(conn,sql))
    {
    	fprintf(stderr,"%s\n",mysql_error(conn));
    	return 0;
    	exit(1);
    }
    flag =1;
    mysql_close(conn);
    return  flag;
}


int main() 
{ 
    int sock_fd, new_fd;  
    struct sockaddr_in server_addr;    
    struct sockaddr_in client_addr; 
    socklen_t sin_size; 
    int yes = 1; 
    char buf[BUF_SIZE]; 
    int ret; 
    int i,j; 
    struct msg *rm;
    struct msg *sm;
    struct student *head,*p,*q,*l;
    head = (struct student *)malloc(sizeof(struct student));
    p = head;
    rm = (struct msg *)malloc(sizeof(struct msg));
    sm = (struct msg *)malloc(sizeof(struct msg));
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { 
        perror("socket"); 
        exit(1); 
    } 
  
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    { 
        perror("setsockopt"); 
        exit(1); 
    } 
    
    server_addr.sin_family = AF_INET;         
    server_addr.sin_port = htons(3490);     
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero)); 
    int opt;

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    { 
        perror("bind"); 
        exit(1); 
    } 
  
    if (listen(sock_fd, BACKLOG) == -1)
    { 
        perror("listen"); 
        exit(1);
    } 
    
    
    fd_set fdsr; 
    int maxsock; 
    struct timeval tv; 
  
    conn_amount = 0; 
    sin_size = sizeof(client_addr); 
    maxsock = sock_fd; 
    while (1)
    { 
        FD_ZERO(&fdsr); 
        FD_SET(sock_fd, &fdsr); 

        tv.tv_sec = 30; 
        tv.tv_usec = 0; 

        for (i = 0; i < BACKLOG; i++) 
        { 
            if (fd_A[i] != 0) 
            { 
                FD_SET(fd_A[i], &fdsr); 
            } 
        } 
  
        ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv); 
        if (ret < 0) 
        { 
            perror("select"); 
            break; 
        } else if (ret == 0)
        { 
            printf("timeout\n"); 
            continue; 
        } 
        for (i = 0; i < conn_amount; i++)
        { 
            if (FD_ISSET(fd_A[i], &fdsr))         
            { 
            	bzero(rm,sizeof(struct msg));
                ret = recv(fd_A[i], (void *)rm, sizeof(struct msg), 0);
                if(ret <= 0 )
                {
                	printf("client[%d] close\n", i); 
                    close(fd_A[i]); 
                    FD_CLR(fd_A[i], &fdsr); 
                    fd_A[i] = 0; 
                    conn_amount--;   
                }   /*根据信息类型对客户端执行相应的操作*/
                if(rm->type == MSG_LOGIN)                          
                { 
                    q = head->next;
                    while(q != NULL)
                    {
                    	if(strcmp(rm->name,q->name)==0)
                    	{
                    		break;
                    	}
                    	q = q->next;
                    }  
                    if(q != NULL)
                    {
                    	bzero(sm,sizeof(struct msg));
                    	sm->type = MSG_LOGIN_FAILED;
                    	if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                    	{
                    		perror("send");
                    	}
                    }
                    else
                	{
                	if(server_login(rm->name,rm->passwd) == 0)
                	{
                		bzero(sm,sizeof(struct msg));
                		sm->type = MSG_LOGIN_FAILED;
                		if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) <0 )
                		{
                			perror("send");
                			exit(1);
                		}
                	}
                	else 
                	{
                		bzero(sm,sizeof(struct msg));
                		struct student *s;
                        s = (struct student *)malloc(sizeof(struct student));
                        strcpy(s->name,rm->name);
                        s->fd = fd_A[i];
                        login_amout++;
                        p = head;
                        if(head->next == NULL)
                        {
                        	s->style = 2;
                        }
                        else
                        {
                        	s->style = 0;
                        }
                        s->next = NULL;
                        while( p->next != NULL)
                        {
                        	p = p->next;
                        }
                        p->next = s;
                        sm->style = s->style;
                        strcpy(sm->name,rm->name);
                		sm->type = MSG_LOGIN;
                		if(send( fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                		{
                			perror("send");
                			exit(1);
                		}
                	}
                    }
                }
                else if(rm->type == MSG_NAME)
                {
                    if(server_search(rm->name) == 0)
                    {
                        bzero(sm,sizeof(struct msg));
                		sm->type = MSG_EXCEPTION;
                		if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) <=0 )
                		{
                			perror("send");
                			exit(1);
                		}
                    }
                    else
                    {
                    	bzero(sm,sizeof(struct msg));
                		sm->type = MSG_ACK;
                		if(send( fd_A[i],(void *)sm,sizeof(struct msg),0) <= 0)
                		{
                			perror("send");
                			exit(1);
                		}
                    }

                }
                else if(rm->type == MSG_REGISTRATION)
                {
                    if(server_registration(rm->name,rm->passwd,rm->varification) == 0)
                    {
                    	bzero(sm,sizeof(struct msg));
                		sm->type = MSG_EXCEPTION;
                		if(send(fd_A[i],(void *)sm,sizeof(sm),0) <0 )
                		{
                			perror("send");
                			exit(1);
                		}
                    }
                    else
                    {
                    	bzero(sm,sizeof(struct msg));
                		sm->type = MSG_ACK;
                		if(send( fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                		{
                			perror("send");
                			exit(1);
                		}
                    }
                }
                else if(rm->type == MSG_CONFIRM_VARIFICATION)
                {
                    if(server_confirm_varification(rm->name,rm->varification) == 0)
                    {
                    	bzero(sm,sizeof(struct msg));
                		sm->type = MSG_EXCEPTION;
                		if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) <0 )
                		{
                			perror("send");
                			exit(1);
                		}
                    }
                    else
                    {
                    	bzero(sm,sizeof(struct msg));
                		sm->type = MSG_ACK;
                		if(send( fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                		{
                			perror("send");
                			exit(1);
                		}
                    }
                }
                else if(rm->type == MSG_CHANGE)
                {
                    if(server_change_password(rm->name,rm->passwd,rm->varification) == 0)
                    {
                    	bzero(sm,sizeof(struct msg));
                		sm->type = MSG_EXCEPTION;
                		if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) <0 )
                		{
                			perror("send");
                			exit(1);
                		}
                    }
                    else
                    {
                    	bzero(sm,sizeof(struct msg));
                		sm->type = MSG_ACK;
                		if(send( fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                		{
                			perror("send");
                			exit(1);
                		}
                    }
                }
                else if(rm->type == MSG_SIG)
                {
                	q = head->next;
                	while(q != NULL)
                	{
                		if(strcmp(q->name,rm->name2) == 0)
                		{
                			break;
                		}
                		q = q->next;
                	}
                	if(q->style == 1)
                	{
                		bzero(sm,sizeof(struct msg));
                		sm->type = MSG_BAND;
                		if(send(q->fd,(void *)sm,sizeof(struct msg),0) < 0)
                		{
                			perror("send");
                			exit(1);
                		}
                	}
                	else 
                	{
                		q = head->next;
                		while(q != NULL)
                		{
                			if(strcmp(q->name,rm->name) ==0)
                				break;
                			q = q->next;
                		}

                		if(q == NULL)
                		{
                			bzero(sm,sizeof(struct msg));
                			sm->type = MSG_EXCEPTION;
                			if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                			{
                				perror("send");
                				exit(1);
                			}
                		}
                		else
                		{
                			bzero(sm,sizeof(struct msg));
                			strcpy(sm->name2,rm->name2);
                			sm->type = MSG_SIG;
                			strcpy(sm->data,rm->data);
                			if(send(q->fd,(void *)sm,sizeof(struct msg),0) < 0)
                			{
                				perror("send");
                				exit(1);
                			}
                		}
                	}
                	
                }
                else if(rm->type == MSG_MULT)
                {
                	q = head->next;
                	while(q != NULL)
                	{
                		if(strcmp(q->name,rm->name2) == 0)
                		{
                			break;
                		}
                		q = q->next;
                	}
                	if(q->style == 1)
                	{
                		bzero(sm,sizeof(struct msg));
                		sm->type = MSG_BAND;
                		if(send(q->fd,(void *)sm,sizeof(struct msg),0) < 0)
                		{
                			perror("send");
                			exit(1);
                		}
                	}
                	else 
                	{
                		bzero(sm,sizeof(struct msg));
                		strcpy(sm->name2,rm->name2);
                		strcpy(sm->data,rm->data);
                		sm->type = MSG_MULT;
                		for( j = 0 ; j < conn_amount ; j++)
                		{
                			if( i != j)
                			{
                                if(send(fd_A[j],(void *)sm,sizeof(struct msg),0) < 0)
                				{
                					perror("send");
                					exit(1);
                				}
                			}
                		}
                	}
                } 
                else if(rm->type == MSG_NUMBER)
                {
                	bzero(sm,sizeof(struct msg));
                	sm->type = MSG_NUMBER;
                	sm->len = login_amout;
                	if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                	{
                		perror("send");
                		exit(1);
                	}
                }
                else if(rm->type == MSG_NUMBER_NAME)
                {
                	bzero(sm,sizeof(struct msg));
                	sm->type = MSG_NUMBER_NAME;
                    q = head->next;
                	while(q!=NULL)
                	{
                		if(strcmp(q->name,rm->name) ==0)
                			break;
                		strcpy(sm->name2,q->name);
                		if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                		{
                			perror("send");
                			exit(1);
                		}
                		q = q->next;
                	}
                }
                else if(rm->type == MSG_LOGOT)
                {
                	bzero(sm,sizeof(struct msg));
                	l = head;
                	q = head->next;
                	while(q != NULL)
                    {
                    	if(q->fd == fd_A[i])
                    	{
                    		break;
                    	}
                    	l = q;
                    	q = q->next;
                    }
                    if(q != NULL)
                    {
                    	l->next = q->next;
                    	login_amout--;
                    	free(q);
                    	sm->type = MSG_LOGOT;
                    	if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                    	{
                    		perror("send");
                    		exit(1);
                    	}
                    }
                }
                else if(rm->type == MSG_KICK)
                {
                	q = head->next;
                	if(strcmp(q->name,rm->name2) == 0)
                	{
                    	l = head;
                		q = head->next;
                		while(q!=NULL)
                    	{
                    		if(strcmp(q->name,rm->name) == 0)
                    		{
                    			break;
                    		}
                    		l = q;
                    		q = q->next;
                    	}
                    	sm->fd = q->fd;
                    	sm->type = MSG_KICK;
                    	if(send(sm->fd,(void *)sm,sizeof(struct msg),0) < 0)
                    	{
                    		perror("send");
                    		exit(1);
                    	}
                	}
               		else
                	{
                        bzero(sm,sizeof(struct msg));
                        sm->type = MSG_NOT_LORD;
                        if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                        {
                         	perror("send");
                         	exit(1);
                        }
                	}
                }
                else if(rm->type == MSG_BAND)
                {
                	q = head->next;
                	if(strcmp(q->name,rm->name2) == 0)
                	{
                		bzero(sm,sizeof(struct msg));
                		q = head->next;
                		while(q != NULL)
                		{
                			if(strcmp(q->name,rm->name) == 0)
                			{
                				break;
                			}
                			q = q->next;
                		}
                		if(q != NULL)
                		{
                        	q->style =  1;
                        	sm->type = MSG_DONE;
                        	if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                        	{
                        		perror("send");
                        		exit(1);
                        	}
                		}
                		else 
                		{
                			sm->type = MSG_EXCEPTION;
                			if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                			{
                				perror("send");
                				exit(1);
                			}
                		}
                	}
                	else
                	{
                        bzero(sm,sizeof(struct msg));
                        sm->type = MSG_NOT_LORD;
                        if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                        {
                         	perror("send");
                         	exit(1);
                        }
                	}
                }
                else if(rm->type == MSG_LIFT)
                {
                	q = head->next;
                	if(strcmp(q->name,rm->name2) == 0)
                	{
                		bzero(sm,sizeof(struct msg));
                		q = head->next;
                		while(q != NULL)
                		{
                			if(strcmp(q->name,rm->name) == 0)
                			{
                				break;
                			}
                			q = q->next;
                		}
                		if(q != NULL)
                		{
                        	q->style =  0;
                        	sm->type = MSG_DONE;
                        	if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                        	{
                        		perror("send");
                        		exit(1);
                        	}
                		}
                		else 
                		{
                			sm->type = MSG_EXCEPTION;
                			if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                			{
                				perror("send");
                				exit(1);
                			}
                		}
                	}
                	else
                	{
                		bzero(sm,sizeof(struct msg));
                        sm->type = MSG_NOT_LORD;
                        if(send(fd_A[i],(void *)sm,sizeof(struct msg),0) < 0)
                        {
                         	perror("send");
                         	exit(1);
                        }
                	}
                }
                else if(rm->type == MSG_FILENAME)
                {
                	bzero(sm,sizeof(struct msg));
                	sm->type = MSG_FILENAME;
                	q = head->next;
                	while(q != NULL)
                	{
                		if(strcmp(q->name,rm->name) == 0)
                		{
                            break;
                		}
                		q = q->next;
                	}
                	if(q != NULL)
                	{
                		sm->fd = fd_A[i];
                    	strcpy(sm->name,rm->name);
                    	strcpy(sm->name2,rm->name2);
                    	strcpy(sm->data,rm->data);
                    	if(send(q->fd,(void *)sm,sizeof(struct msg),0) < 0)
                    	{
                    		perror("send");
                    		exit(1);
                    	}
                	}
                	else 
                	{
                		sm->type = MSG_EXCEPTION;
                		if(send(fd_A[i],(void  *)sm,sizeof(struct msg),0) < 0)
                		{
                			perror("send");
                			exit(1);
                		}
                	}
                }
            } 
        } 
        if (FD_ISSET(sock_fd, &fdsr)) 
        { 
            new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size); 
            if (new_fd <= 0) 
            { 
                perror("accept"); 
                continue; 
            } 
  
            if (conn_amount < BACKLOG)
            { 
                fd_A[conn_amount++] = new_fd; 
                printf("new connection client[%d] %s:%d\n", conn_amount, 
                        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
                if (new_fd > maxsock) 
                    maxsock = new_fd; 
            } 
            else 
            { 
                printf("max connections arrive, exit\n"); 
                send(new_fd, "bye", 4, 0); 
                close(new_fd); 
                break; 
            } 
        } 
    } 
    for (i = 0; i < BACKLOG; i++) 
    { 
        if (fd_A[i] != 0) 
        { 
            close(fd_A[i]); 
        } 
    } 
  
    exit(0); 
} 
