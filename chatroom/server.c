#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define SERVER_PORT 6789 //服务器端口

#define MAX_LOGIN 10 //最大客户端连接数

#define BUF_SIZE 500 //输入内容缓冲区大小

int fd_ID[MAX_LOGIN]; //客户端标识符
int online_num;  //在线人数
char users[MAX_LOGIN][16];//用户列表
char *time_str;  //当前系统时间
char *get_ip;  //获取当前连接IP

struct bag {
	char message_option;
	char message_dest[16];
	char message_src[16];
	char message_data[450];
} info;

struct db {  //用户名和密码
	char name[16];
	char pass[16];
} db_user[MAX_LOGIN],db_tmp;

/*时间*/
char * now_time(int type)
{
	char *string;
	time_t timep;
	time(&timep);
	if(type == 1) {
		printf("%s",ctime(&timep));
	} else { 
		string = ctime(&timep);
	}
	return (string);
}

/*系统与志*/
void sys_logs(int type)
{
	FILE *fp;
	char *logs;
	int size;

	logs = (char *)malloc(540);
	time_str = now_time(2);
	if(type == 1) { //db_user.txt open error!
		size = sprintf(logs,"%sdb_user.txt open error!",time_str);
	} else if(type == 2) { //用户名已存在，注册失败
		size = sprintf(logs,"%s[%s] reg faile!\nIP:%s\nError:username have exist!",time_str,info.message_dest,get_ip);
	} else if(type == 3) { //注册成功
		size = sprintf(logs,"%s[%s] reg success!\nIP:%s",time_str,info.message_dest,get_ip);
	} else if(type == 4){ //db_user.txt open error!
		size = sprintf(logs,"%sdb_user.txt open error!",time_str);
	} else if(type == 5) { //密码输入正确
    	size = sprintf(logs,"%s[%s] login succcess!\nIP:%s",time_str,info.message_dest,get_ip);
	} else if(type == 6) { //密码输入错误
    	size = sprintf(logs,"%s[%s] login failed!\nIP:%s\nError:passwrod worng!",time_str,info.message_dest,get_ip);
	} else if(type == 7) { //成功失败(用户名不存在)
		size = sprintf(logs,"%sError:username [%s] do not exist!\nIP:%s",time_str,info.message_dest,get_ip);
	} else if(type == 8) { //无此用户
		size = sprintf(logs,"%sError:info couldn't arrive!\n[%s] -/-> [%s]",time_str,info.message_dest,info.message_src);
	} else if(type == 9) { //断开连接
		size = sprintf(logs,"%s[%s] connetion is down!",time_str,info.message_dest);
	} else if(type == 10) { //新连接
		size = sprintf(logs,"%sNew client[%d]\nIP:%s",time_str,online_num,get_ip);
	} else if(type == 11) { //连接已满
		size = sprintf(logs,"%sError:client number full!",time_str);
	} else {
		size = sprintf(logs,"%sError:unknow worng!",time_str);
	}

	fp = fopen("sys_logs.txt","a+");
	fwrite(logs,size,1,fp);
	fputc('\n',fp);
	fclose(fp);
	free(logs);
}

/*用户注册*/
int reg_user(int new_fd,struct bag info,struct sockaddr_in client_addr)
{
	int i;
	FILE *fp;

	struct bag reg_bag = {'C',"0","0","0"};
	fp = fopen("db_user.txt","a+");

	if(!fp) {
		printf("db_user.txt open error!\n");
		sys_logs(1);
		exit(1);
	}
	for(i = 0;i < MAX_LOGIN;i ++) {
		fread(&db_user[i],sizeof(db_user[i]),1,fp);
	}
	for(i = 0;i < MAX_LOGIN;i ++) {
		if(strcmp(db_user[i].name,info.message_dest) == 0) { //用户名已存在，注册失败
			strncpy(reg_bag.message_data,"Reg faile!",11);
			strncpy(reg_bag.message_dest,"FAILE",6);
			send(new_fd,(void *)&reg_bag,sizeof(struct bag)+1,0);
	        printf("[%s] reg faile!\nIP:%s\n",info.message_dest,inet_ntoa(client_addr.sin_addr));
			printf("Error:username have exist!\n");
			sys_logs(2);
			fclose(fp);
			return -1;
		} else {     //注册成功
			strncpy(reg_bag.message_data,"Reg success!",13);
			send(new_fd,(void *)&reg_bag,sizeof(struct bag)+1,0);
			printf("[%s] reg success!\nIP:%s\n",info.message_dest,inet_ntoa(client_addr.sin_addr));
			sys_logs(3);
			strcpy(db_tmp.name,info.message_dest);
			strcpy(db_tmp.pass,info.message_src);
			fwrite(&db_tmp,sizeof(db_tmp),1,fp);
	}
	rewind(fp);
	fclose(fp);

	return 1;
	}
}

/*用户登录*/
int login_user(int new_fd,struct bag info,struct sockaddr_in client_addr)
{
	int i;
	FILE *fp;
		
	struct bag log_bag = {'B',"0","0","0"};
	fp = fopen("db_user.txt","r");

	if(!fp) {
		printf("db_user.txt open error!\n");
		sys_logs(4);
		exit(1);
	}
	now_time(1);
	for(i = 0;i < MAX_LOGIN;i ++) {
		fread(&db_user[i],sizeof(db_user[i]),1,fp);
	}
	for(i = 0;i < MAX_LOGIN;i ++) {
		if(strcmp(db_user[i].name,info.message_dest) == 0) {  //成功登录(用户名存在)
			if(strcmp(db_user[i].pass,info.message_src) == 0) {  //密码输入正确
				strncpy(log_bag.message_data,"Log success!",13);
				send(new_fd,(void *)&log_bag,sizeof(struct bag)+1,0);
				printf("[%s] login succcess!\nIP:%s\nOnline number:%d\n",info.message_dest,inet_ntoa(client_addr.sin_addr),online_num);
				sys_logs(5);
				fclose(fp);
				return 1;
	    	} else {  //密码输入错误
				strncpy(log_bag.message_data,"Error:password worng!",22);
				strncpy(log_bag.message_dest,"FAILE",6);
				send(new_fd,(void *)&log_bag,sizeof(struct bag)+1,0);
				printf("[%s] login failed!\nIP:%s\n\n",info.message_dest,inet_ntoa(client_addr.sin_addr));
				printf("Error:passwrod worng!\n");
				sys_logs(6);
				fclose(fp);
				return -1;
    	    }
		}
	}								

	fclose(fp);   //成功失败(用户名不存在)
	strncpy(log_bag.message_data,"username do not exist!",23);
	strncpy(log_bag.message_dest,"FAILE",6);
	send(new_fd,(void *)&log_bag,sizeof(struct bag)+1,0);
	printf("Error:username [%s] do not exist!\n",info.message_dest);
	printf("IP:%s\n",inet_ntoa(client_addr.sin_addr));
	sys_logs(7);
	return -1;
}

/*遍历查找用户名*/
int find(int id_num,struct bag info)
{
	int i,j;
	int t = -1;

	strcpy(users[id_num],info.message_dest);
	
	for(i = 0;i < MAX_LOGIN;i ++) {
		if(strcmp(users[i],info.message_src) == 0) {
			break;
		}
	}

	return i;
}

/*显示在线人数*/
void show_clients(int fd_id,struct bag info)
{
	int i;
	char string[256];
	for(i = 0;i < online_num;i ++) {
		strcat(users[i],"\n");
		strncat(string,users[i],sizeof(users[i]));
	}
	strncpy(info.message_data,string,sizeof(string));
	send(fd_id,(void *)&info,sizeof(info),0);
	now_time(1);
	printf("[%s] show request success!\n",info.message_dest);
}

/*接收数据包的转发*/
data_forward(int fd_id,int id_num,struct bag info) 
{
	int i;
	int num;
	char user_id[16];

	num = find(id_num,info);

	if((strcmp(info.message_src,users[num]))== 0) { //转发给私聊的人
		if((send(fd_ID[num],(void *)&info,sizeof(info)+1,0)) == -1) {
			perror("forward one");
			exit(1);
		}
		printf("[%s] --> [%s]\n",info.message_dest,info.message_src);
	} 
	else if((strcmp(info.message_src,"TO_ALL"))== 0) { //转发给所有人
		for(i = 0;i < online_num;i ++) {
			if(fd_id != fd_ID[i]) {
			  if((send(fd_ID[i],(void *)&info,sizeof(info)+1,0))== -1) {
				  perror("forward all");
				  exit(1);
			  }
			}
		}
	}
	else {
		printf("Error:info couldn't arrive!\n");
		printf("[%s] -/-> [%s]\n",info.message_dest,info.message_src);
		sys_logs(8);
	}
}

/*初始化网络*/
int setup()
{
	int i;
	int sock_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int option_value = 1;

	/*创建套接字*/
	if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perror("socket");
		exit(1);
	}

	/*设置套接字使之可以重新绑定端口*/
	if((setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&option_value,sizeof(int)))== -1) {
		perror("setsockopt");
		exit(1);
	}
	
	/*初始化套接字地址*/
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(server_addr.sin_zero,'\0',sizeof(server_addr.sin_zero));

	/*绑定本地端口*/
	if(bind(sock_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) == -1) {
		perror("bind");
		exit(1);
	}

	/*监听本地端口*/
	if(listen(sock_fd,MAX_LOGIN) == -1) {
		perror("listen");
		exit(1);
	}
	now_time(1);
	printf("listen port %d\n",SERVER_PORT);

	return sock_fd;
}

int runing(int sock_fd)
{
	int i;
	int new_fd;
	int maxsock;
	int ret1,ret2,ret3;
	fd_set fd_sum;
	socklen_t client_len;
	struct sockaddr_in client_addr;


	/*初始化*/
	online_num = 0;
	client_len = sizeof(struct sockaddr);
	maxsock = sock_fd;

    while(1) {

		/*初始化文件描述符集合*/
		FD_ZERO(&fd_sum);
		FD_SET(sock_fd,&fd_sum);

		/*添加文件描述符*/
		for(i = 0;i < MAX_LOGIN;i ++) {
			if(fd_ID[i] != 0) {
				FD_SET(fd_ID[i],&fd_sum);
			}
		}
		
		/*监控文件描述符的变化个数*/
		ret1 = select(maxsock+1,&fd_sum,NULL,NULL,NULL);
		if(ret1 < 0) {
			perror("select");
			break;
		}
		

		/*循环判断监视集合中的文件描述符是否可读写*/
		for(i = 0;i < online_num;i ++) {
			if(FD_ISSET(fd_ID[i],&fd_sum)) {  
				ret2 = recv(fd_ID[i],(void *)&info,sizeof(info),0); //接收数据

				/*判断接受到数据大小*/
            	if((strncmp(info.message_data,"exit",4))== 0 || ret2 == 0) {
					now_time(1);
				    if(ret2 != 0) {
						printf("[%s] leave!\n",info.message_dest);
					} else {
						printf("Anonymous request has down!\n");
					}
					close(fd_ID[i]);
					FD_CLR(fd_ID[i],&fd_sum);
					fd_ID[i] = 0;
					online_num --;
					sys_logs(9);
				} else {
					if(ret2 > 1 && ret2 < BUF_SIZE) {
						memset(&info.message_data[ret2],'\0',1); //
					   	if(info.message_option == 'A') { //接收正常消息
							now_time(1);
							printf("[%s] ==> %s\n",info.message_dest,info.message_data);
							data_forward(fd_ID[i],i,info);
						}
						if(info.message_option == 'B') { //接收登录信息
							ret3 = login_user(fd_ID[i],info,client_addr);
							if(!ret3) {
								close(fd_ID[i]);
								FD_CLR(fd_ID[i],&fd_sum);
								fd_ID[i] = 0;
								online_num --;
							}							
						}
						if(info.message_option == 'C') { //接收注册信息
							ret3 = reg_user(fd_ID[i],info,client_addr);
							if(!ret3) {
								close(fd_ID[i]);
								FD_CLR(fd_ID[i],&fd_sum);
								fd_ID[i] = 0;
								online_num --;
							}				
						}
						if(info.message_option == 'D') { //接收在线人数请求
							show_clients(fd_ID[i],info);
						}
				    } 
				} 
			} 
		}    	

		/*接受新连接*/
		if(FD_ISSET(sock_fd,&fd_sum)) {
			new_fd = accept(sock_fd,(struct sockaddr *)&client_addr,&client_len);
			if(new_fd <= 0) {
     			perror("accept");
	    		continue;
			}

			/*判断当前连接数目*/
			if(online_num < MAX_LOGIN) {
				fd_ID[online_num ++] = new_fd;
				printf("\n");
				now_time(1);
				printf("New client request [%d]\nIP:%s\nPort:%d\n\n",online_num,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
				get_ip = inet_ntoa(client_addr.sin_addr);
				sys_logs(10);
				if(new_fd > maxsock) {
				maxsock = new_fd;
				}
			}
			else {
				now_time(1);
				printf("Error:client number full!\n");
				sys_logs(11);
				close(new_fd);
				break;
			}
    	}	
	}
	/*关闭所有连接*/
	for(i = 0;i < MAX_LOGIN;i ++) {
		if(fd_ID[i] != 0) {
			close(fd_ID[i]);
		}
	exit(0);
	}
}

int main()
{
	int sock_fd;

	sock_fd = setup();
	runing(sock_fd);

	return 0;	
}
