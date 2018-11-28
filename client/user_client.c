#include<stdio.h>
#include"./head.h"

void select3();
void select1(char name2[]);
void select2(char name2[]);
void logot();
pthread_t th2 = -1;
int sockfd;
pthread_t th = -1;
int flag2 = 0,flag3 = 0,flag4 = 0,flag5 = 1;
char filename[50],name[50];
void window()              //用户主菜单
{
	printf("*************chat system***************\n");
	printf("*                                     *\n");
	printf("*       1:user registration           *\n");
	printf("*       2:user change password        *\n");
	printf("*       3:user login                  *\n");
	printf("*       4:user exit                   *\n");
	printf("*                                     *\n");
	printf("***************************************\n");
}

void window1()           //用户聊天模块菜单
{
	printf("*************chat system***************\n");
	printf("*                                     *\n");
	printf("*       1:online number query         *\n");
	printf("*       2:single chat                 *\n");
	printf("*       3:Multi_person chat           *\n");
	printf("*       4:logot                       *\n");
	printf("*       5:kicking people              *\n");
	printf("*       6:Banned                      *\n");
	printf("*       7:Lift the ban                *\n");
	printf("*       8:file transfer               *\n");
	printf("*                                     *\n");
	printf("***************************************\n");
}

void *p_recv(void *argc)      //用于接受服务器信息的模块
{
	struct msg *rm;
	struct msg *sm;
	char file[100];
	int flag = 0;
	int len;
	sm = (struct msg*)malloc(sizeof(struct msg));
	rm = (struct msg*)malloc(sizeof(struct msg));
	while(flag == 0)
	{
		bzero(rm,sizeof(struct msg));
        len = recv(sockfd,(void *)rm,sizeof(struct msg),0);
        if(len <= 0)
        {
        	break;
        }
        else
        {       /*根据信息类型打印相应的结果*/
        	if(rm->type == MSG_FILENAME)           //用于写入文件
        	{
        		//bzero(sm,sizeof(struct msg));
        		sprintf(file,"../file//%s",rm->name2);
        		FILE *fp;
                if((fp = fopen(file,"a")) == NULL)
                {
                    perror("fopen");
                    exit(1);
                }
                fwrite(rm->data,sizeof(rm->data)-1,1,fp);
                fclose(fp);
                /*sm->type =MSG_ACK;
                sm->fd  = rm->fd;
                if(send(sockfd,(void *)sm,sizeof(struct msg),0) < 0)
                {
                	perror("send");
                	exit(1);
                }*/
        	}
        	else if(rm->type == MSG_LOGOT)         //用户退出
        	{
                flag = 1;
        	}
        	if(rm->type == MSG_SIG)                //接受私人信息
        	{
        		printf("this is a private chat from %s:",rm->name2);
        		printf("%s\n",rm->data);
        	}
        	else if(rm->type == MSG_MULT)         //接受多人信息
        	{
        		printf("\nthis is a group chat from %s:",rm->name2);
        		printf("%s\n",rm->data);
        	}
        	else if(rm->type == MSG_EXCEPTION)    //用于接受错误的操作，例如登陆失败
        	{
        		printf("Failed  operation\n");
        	}
        	else if(rm->type == MSG_NUMBER)       //查看人数
        	{
        		printf("The online number is %d\n",rm->len);
        	}
        	else if(rm->type == MSG_NUMBER_NAME)   //查看在线用户名字
        	{
        		printf("online member:%s\n",rm->name2);
        	}
        	else if(rm->type == MSG_DONE)          //操作完毕
        	{
        		printf("Successful operation\n");
        	}
        	else if(rm->type == MSG_BAND)
        	{
        		printf("\nYou have been banned, you can't send messages\n");
                
        	}
        	else if(rm->type == MSG_KICK)
        	{
        		printf("\nThe group leader kicked you out of the group\n");
        		//logot();
        		flag4 = 1;
        	}
            else if(rm->type == MSG_LOGIN)
            {
                flag2 = 1;
            }
            else if(rm->type == MSG_LOGIN_FAILED)
            {
                printf("FAILED login\n");
                flag3 = 1;
            }
            else if(rm->type == MSG_NOT_LORD)
            {
                printf("you is not lord,you can't do that\n");
            }
            /*else if(rm->type == MSG_ACK)
            {
                flag5 = 1;
            }*/
        }
	}
}


void  *p_send(void *argv)       //发送文件
{
	long len,len1;
    FILE *fp;
    struct msg *sm;
    sm = (struct msg *)malloc(sizeof(struct msg));
    bzero(sm,sizeof(struct msg));
    if((fp=fopen(filename,"r")) == NULL)
    {
        perror("fopen");
        exit(1);
    }
    fseek(fp,0,2);
    len = ftell(fp)/sizeof(char);
    rewind(fp);
    while(1)
    {
    	usleep(5);
    	bzero(sm,sizeof(struct msg));
    	memset(sm->data,'\0',sizeof(sm->data));
        strcpy(sm->name,name);
        strcpy(sm->name2,filename);
        if((fread(&sm->data,1,sizeof(sm->data)-1,fp)) > 0)
        {
        	sm->type = MSG_FILENAME;
        	usleep(10);
            if(send(sockfd,(void *)sm,sizeof(struct msg),0) <= 0)
        	{
        		perror("send");
        		exit(1);
        	}
        }
        if(feof(fp))
        {
        	break;
        }
    }
    printf("\nsend success\n");
    pthread_exit(0);
}

void kicking_people(char name2[])
{
	struct msg *rm;
	struct msg *sm;
	int byte;
	sm=(struct msg *)malloc(sizeof(struct msg));
	bzero(sm,sizeof(struct msg));
	sm->type = MSG_KICK;
    	strcpy(sm->name2,name2);
	printf("Please enter the name of the person you want to kick:");
	scanf("%s",sm->name);
	getchar();
	if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1)
	{
		perror("send");
		exit(1);
	}
    sleep(2);
}
void Lift_the_ban(char name2[])
{
	struct msg *rm;
	struct msg *sm;
	int byte;
	sm=(struct msg *)malloc(sizeof(struct msg));
	bzero(sm,sizeof(struct msg));
	sm->type = MSG_LIFT;
    	strcpy(sm->name2,name2);
	printf("Please enter the name of the person you want to release the ban:");
	scanf("%s",sm->name);
	if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1)
	{
		perror("send");
		exit(1);
	}
    	sleep(2);
}

void Banned(char name2[])
{
	struct msg *sm;
	struct msg *rm;
	int byte;
	sm = (struct msg *)malloc(sizeof(struct msg));
	bzero(sm,sizeof(struct msg));
	sm->type = MSG_BAND;
    	strcpy(sm->name2,name2);
	printf("Please enter the name you want to ban:");
	scanf("%s",sm->name);
    if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1) 
    { 
        perror("send"); 
        exit(1); 
    }
    sleep(2);
}
void  logot()
{
	struct msg *sm;
	int byte;
	sm = (struct msg *)malloc(sizeof(struct msg));
	bzero(sm,sizeof(struct msg));
	sm->type = MSG_LOGOT;
	if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1)
	{
		perror("send");
		exit(1);
	}
    sleep(1);
    select3();
}
void  online_number_query()
{
    struct msg *rm;
    struct msg *sm;
    char s[20]={0};
    int byte;
    sm=(struct msg *)malloc(sizeof(struct msg));
    bzero(sm,sizeof(struct msg));
    sm->type = MSG_NUMBER;
    if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1) 
    { 
        perror("send"); 
        exit(1); 
    }
    sleep(1);
    printf("Do your want to cat online user name(Y/N):");
    scanf("%s",s);
    getchar();
    if((strcmp(s,"Y") == 0) ||(strcmp(s,"y") == 0) )
    {
    	sm->type = MSG_NUMBER_NAME;
    	if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1)
    	{
    		perror("send");
    		exit(1);
    	}
    }
    sleep(1);
    printf("Please press the Enter key to end:");
    getchar();
}

void  Multi_person_chat(char name2[])
{
    struct msg *sm;
    int byte;
    sm=(struct msg *)malloc(sizeof(struct msg));
    bzero(sm,sizeof(struct msg));
    strcpy(sm->name2,name2);
    sm->type = MSG_MULT;
    while(1)
    {
        if(flag4 == 1)
        {
            logot();
            break;
        }
    	fgets(sm->data,sizeof(sm->data),stdin);
        sm->data[strlen(sm->data)-1]='\0';
        if(flag4 == 1)
        {
            logot();
            break;
        }
        if(strcmp(sm->data,"exit") == 0)
        {
            break;
        }
        if(strcmp(sm->data, "exit") == 0)
        { 
            if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1) 
            { 
                perror("send"); 
                exit(1); 
            }            
            break; 
        }                   
        if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1) 
        { 
            perror("send"); 
            exit(1); 
        } 
    }
    free(sm);
}


void  single_chat(char name2[])
{
    struct msg *sm;
    int byte;
    char s[10];
    sm =(struct msg *)malloc(sizeof(struct msg));
    bzero(sm,sizeof(struct msg));
    strcpy(sm->name2,name2);
    sm->type = MSG_SIG;
    while(1)
    { 
        if(flag4 == 1)
        {
            logot();
            sleep(1);
            break;
        }
    	scanf("%s",sm->name);
    	getchar();
        if(strcmp(sm->name,"exit") == 0)
        {
            break;
        }
        fgets(sm->data,sizeof(sm->data),stdin);
        sm->data[strlen(sm->data)-1] = '\0';
        if(strcmp(sm->name,"exit") == 0)
        {
            break;
        }
        if(flag4 == 1)
        {
            logot();
            sleep(1);
            break;
        }
        if(strcmp(sm->data, "exit") == 0)
        { 
            if((byte=send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1) 
            { 
                perror("send"); 
                exit(EXIT_FAILURE); 
            }            
            break; 
        }                   
        if((byte = send(sockfd,(void *)sm,sizeof(struct msg),0)) == -1) 
        { 
            perror("send"); 
            exit(1); 
        } 
        sleep(1);
    }
    free(sm);
}

void select1(char name2[])            /*聊天模块选择*/
{
    int n;
    int i = 0; 
    system("clear");
    pthread_create(&th,NULL,p_recv,NULL);
    window1();
    printf("请输入你的选择(1-8):");
    scanf("%d",&n);
    getchar();
    while(i == 0)
    {
	switch(n)
	{
	    case 1:
	    {
            system("clear");
		    online_number_query();
            system("clear");
            if(flag4 == 1)
            {
                logot();
            }
            window1();
	       	printf("请输入你的选择(1-8):");
            scanf("%d",&n);
	        getchar();
            if(flag4 == 1)
            {
                logot();
            }
            break;
	    }
	    case 2:
	    {
            system("clear");
		    single_chat(name2);
            if(flag4 == 1)
            {
                logot();
            }
            system("clear");
            window1();
		    printf("请输入你的选择(1-8):");
            scanf("%d",&n);
	        getchar();
            if(flag4 == 1)
            {
                logot();
            }
            break;
        }
	    case 3:
	    {
		    system("clear");
		    Multi_person_chat(name2);
            if(flag4 == 1)
            {
                logot();
            }
            system("clear");
            window1();
		    printf("请输入你的选择(1-8):");
                scanf("%d",&n);
	        getchar();
            if(flag4 == 1)
            {
                logot();
            }
            break;
	    }
	    case 4:
	    {
		    system("clear");
		    logot();
            break;
	    }
	    case 5:
	    {
		    system("clear");
		    kicking_people(name2);
            system("clear");
            if(flag4 == 1)
            {
                logot();
            }
            window1();
    		printf("请输入你的选择(1-8):");
            scanf("%d",&n);
	        getchar();
            break;
	    }
	    case 6:
	    {
		    system("clear");
	    	Banned(name2);
            if(flag4 == 1)
            {
                logot();
            }
            system("clear");
            window1();
    		printf("请输入你的选择(1-8):");
            scanf("%d",&n);
	        getchar();
            break;
	    }
	    case 7:
	    {
    		system("clear");
	   	    Lift_the_ban(name2);
            if(flag4 == 1)
            {
                logot();
            }
            system("clear");
            window1();
	   	    printf("请输入你的选择(1-8):");
            scanf("%d",&n);
	        getchar();
            break;
	    }
	    case 8:
	    {
	   	    system("clear");
		    printf("please input your filename:");
            scanf("%s",filename);
            getchar();
            printf("Please enter the username to send:");
            scanf("%s",name);
            getchar();
            struct msg *sm;
            sm =(struct msg *)malloc(sizeof(struct msg));

            pthread_create(&th2,NULL,p_send,NULL);
                //pthread_join(th2,NULL);
            if(flag4 == 1)
            {
                logot();
            }
            sleep(1);
            window1();
    		printf("请输入你的选择(1-8):");
            scanf("%d",&n);
	        getchar();
            break;
	    }
	    default:
	    {
                printf("选择有误,请重新输入:");
                scanf("%d",&n);
                getchar();
        }
	}             
    }
}


void user_connect() 
{ 
    
    int len; 
    struct sockaddr_in address;    
    int result; 
    int i,byte,ret; 
    int n;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1) 
    { 
        perror("socket"); 
        exit(EXIT_FAILURE); 
    } 
    
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {      
        perror("sock");      
        exit(1); 
    }  
    bzero(&address,sizeof(address));  
    address.sin_family = AF_INET;  
    address.sin_port = htons(3490);   
    inet_pton(AF_INET,"192.168.246.159",&address.sin_addr);
    len = sizeof(address); 
    if((result = connect(sockfd, (struct sockaddr *)&address, len)) == -1) 
    { 
        perror("connect"); 
        exit(1); 
    } 
}

void user_login()
{
    struct msg *sm;
    struct msg *rm;
    char s[10];
    sm = (struct msg *)malloc(sizeof(struct msg));
    rm = (struct msg *)malloc(sizeof(struct msg));
    bzero(sm,sizeof(struct msg));
    bzero(rm,sizeof(struct msg));
    while(1)
    {
	sm->type = MSG_LOGIN;
	printf("please input your login name:");
        scanf("%s",sm->name);
        getchar();
        printf("please input your login passwd:");
        scanf("%s",sm->passwd);
        getchar();
        if(send(sockfd,(void *)sm,sizeof(struct msg),0) <= 0)
        {
        	perror("send");
        	exit(1);
        }
        sleep(1);
        if( recv(sockfd,(void *)rm,sizeof(struct msg),0) <= 0)
        {
            perror("recv");
            exit(1);
        }
        if(rm->type == MSG_LOGIN)
        {
            flag4 = 0;
            select1(rm->name);
            if(flag4 == 1)
            {
                printf("You were kicked out by the group owner.\n");
                sleep(1);
                select3();
            }
            break;
        }
        printf("login failed,");
    	printf("Do your want to login again?(Y/N):");
        flag3 = 0;
    	scanf("%s",s);
    	getchar();
    	if((strcmp(s,"Y")==0) || (strcmp(s,"y") == 0))
    	{
    		continue;
    	}
    	else
    	{
    		break;
    	}
    }
}

void user_registration()   //用户注册
{
	struct msg *rm;
	struct msg *sm;
	int flag  = 0;
	char s[10];
	sm = (struct msg *)malloc(sizeof(struct msg));
	rm = (struct msg *)malloc(sizeof(struct msg));
	bzero(sm,sizeof(struct msg));
	bzero(rm,sizeof(struct msg));
	char passwd1[20];
	while(flag == 0)
	{
	    while(1)
	    {
		printf("please input your registration name:");
                scanf("%s",sm->name);
                getchar();
        	printf("please input your registration passwd:");
        	scanf("%s",sm->passwd);
        	getchar();
        	printf("please confirm your registration passwd:");
        	scanf("%s",passwd1);
        	getchar();
        	if(strcmp(sm->passwd,passwd1) ==0 )
        	{
        		printf("confirm success\n");
        		break;
        	}
        	else
        	{
        		printf("confirm failed:please restart input\n");
        	}
	    }
	    sm->type = MSG_NAME;
            if(send(sockfd,(void *)sm,sizeof(struct msg),0) <= 0)
            {
        	 perror("send");
        	 exit(1);
            }
            if(recv(sockfd,(void *)rm,sizeof(struct msg),0) <= 0)
            {
        	perror("recv");
        	exit(1);
            }
            if(rm->type == MSG_ACK)
            {
                printf("user name is exist,please restart input\n");
                continue;
            }
 
            sm->type = MSG_REGISTRATION;
            printf("Verification problem:");
    	    printf("What animal do you like?\n");
    	    scanf("%s",sm->varification);
    	    getchar();
            if(send(sockfd,(void *)sm,sizeof(struct msg),0) <= 0)
            {
        	perror("send");
        	exit(1);
            }
            if(recv(sockfd,(void *)rm,sizeof(struct msg),0) <= 0)
            {
        	perror("recv");
        	exit(1);
            }
            if(rm->type == MSG_ACK)
            {
        	printf("registration success\n");
        	break;
            }
            else
            {
        	printf("registration failed\n");
        	printf("Do your want to registration again(Y/N):");
        	scanf("%s",s);
        	getchar();
        	if((strcmp(s,"Y")==0)||(strcmp(s,"y")))
        	{
        		continue;
        	}
        	else
        	{
        		flag = 1;
        	}
            }
      }
}

void user_change_password()    //用户修改密码
{
	struct msg *rm;
	struct msg *sm;
	sm = (struct msg *)malloc(sizeof(struct msg));
	rm = (struct msg *)malloc(sizeof(struct msg));
	bzero(sm,sizeof(struct msg));
	bzero(rm,sizeof(struct msg));
	char passwd1[20];
	while(1)
	{
		
	    printf("please input your need change name:");
            scanf("%s",sm->name);
            getchar();
	    sm->type = MSG_NAME;
            if(send(sockfd,(void *)sm,sizeof(struct msg),0) <= 0)
            {
        	perror("send");
        	exit(1);
            }
            if(recv(sockfd,(void *)rm,sizeof(struct msg),0) <= 0)
            {
        	perror("recv");
        	exit(1);
            }
            if(rm->type != MSG_ACK)
            {
                printf("user name is not exist,please restart input\n");
                continue;
            }

            sm->type = MSG_CONFIRM_VARIFICATION ;
            printf("Verification problem:");
    	    printf("What animal do you like?\n");
    	    scanf("%s",sm->varification);
    	    getchar();
            if(send(sockfd,(void *)sm,sizeof(struct msg),0) <= 0)
            {
        	perror("send");
        	exit(1);
            }
            if(recv(sockfd,(void *)rm,sizeof(struct msg),0) <= 0)
            {
        	perror("recv");
        	exit(1);
            }
            if(rm->type == MSG_ACK)
            {
        	printf("verify success\n");
        	break;
            }
            else
            {
        	printf("verify failed,please restart input\n");
        	continue;
            }
	}
	sm->type = MSG_CHANGE;
	while(1)
	{
            printf("please input your change passwd:");
	    scanf("%s",sm->passwd);
	    getchar();
	    printf("please confirm your change passwd:");
	    scanf("%s",passwd1);
	    getchar();
	    if(strcmp(sm->passwd,passwd1) == 0)
	    {
		printf("confirm success\n");
		break;
	    }
	    else
	    {
		printf("confirm failed\n");
		continue;
	    }
	}
    if(send(sockfd,(void *)sm,sizeof(struct msg),0) <=0 )
    {
    	perror("send");
    	exit(1);
    }
    if(recv(sockfd,(void *)rm,sizeof(struct msg),0) <=0 )
    {
    	perror("recv");
    	exit(1);
    }
    if(rm->type == MSG_ACK)
    {
    	printf("change password success\n");
    }
    else
    {
    	printf("change password failed\n");
    }
    
}

void select3()        //主菜单选择
{
	int n;
    int flag3 =0;
	system("clear");
    window();
	printf("请输入你的选择(1-4):");
	scanf("%d",&n);
	getchar();
	while(flag3 == 0)
	{
		switch(n)
		{
			case 1:
			{
                system("clear");
				user_registration();
                window();
				printf("请输入你的选择(1-4):");
                scanf("%d",&n);
	            getchar();
                break;
			}
			case 2:
			{
                system("clear");
				user_change_password();
                window();
				printf("请输入你的选择(1-4):");
                scanf("%d",&n);
	            getchar();
                break;
			}
			case 3:
			{
				system("clear");
				user_login();
                window();
				printf("请输入你的选择(1-4):");
                scanf("%d",&n);
	            getchar();
                break;
			}
			case 4:
			{
				system("clear");
				close(sockfd);
				exit(1);
			}
			default:
			{
                printf("选择有误,请重新输入:");
                printf("请输入你的选择(1-4):");
                scanf("%d",&n);
                getchar();
                break;
            }
		}
    }
}

int main() 
{ 
    user_connect();   //用户连接服务器
    select3();
}
