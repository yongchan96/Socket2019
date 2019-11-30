#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
void *do_chat(void *); //채팅 메세지를 보내는 함수
int pushClient(int); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제
pthread_t thread;
pthread_mutex_t mutex;
#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000
struct client {
	int    list_c;
	char nickname[20];
};
struct client info[MAX_CLIENT];


char    escape[ ] = "exit";
char    greeting[ ] = "Welcome to chatting room\n";
char    CODE200[ ] = "Sorry No More Connection\n";
int main(int argc, char *argv[ ])
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int    len;
    int    i, j, n;
    int    res;
    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Can not create mutex\n");
        return -1;
    }
    s_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        printf("Can not Bind\n");
        return -1;
    }
    if(listen(s_socket, MAX_CLIENT) == -1) {
        printf("listen Fail\n");
        return -1;
    }
    for(i = 0; i < MAX_CLIENT; i++)
	{
        info[i].list_c = INVALID_SOCK;
		 strcpy(info[i].nickname,"init");
	}
    while(1) {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		  
        res = pushClient(c_socket);
        if(res < 0) { //MAX_CLIENT만큼 이미 클라이언트가 접속해 있다면,
            write(c_socket, CODE200, strlen(CODE200));
            close(c_socket);
        } else {
            write(c_socket, greeting, strlen(greeting));
            pthread_create(&thread,NULL,do_chat,(void *)&c_socket);
        }
    }
}
void *do_chat(void *arg)
{
    int c_socket = *((int *)arg);
    char chatData[CHATDATA];
	 char whisper[3] = "/w ";
	char username[20],send[20],content[CHATDATA],*token;
    int i, n;
    while(1) 
	{
        memset(chatData, 0, sizeof(chatData));
        if((n = read(c_socket, chatData, sizeof(chatData))) > 0) 
		  {
			for(i = 0; i < MAX_CLIENT; i++)
			{	
				if((info[i].list_c != INVALID_SOCK) && (strstr(chatData,whisper) == NULL))
					write(info[i].list_c,chatData,n);
			}
			
      			
           if(strstr(chatData, escape) != NULL)
			 {
                popClient(c_socket);
                break;
			 }
			else if(strstr(chatData,whisper) != NULL)
			{
				token = strtok(chatData," \n");
				strcpy(username,token);
				for(i=0;i<2;i++)
					token = strtok(NULL," \n");
				strcpy(send,token);
				token = strtok(NULL,"\0");

				for(i=0;i<MAX_CLIENT;i++)
						if(info[i].list_c == c_socket)
							sprintf(content,"From [%s] to [%s] -> %s",info[i].nickname,send,token);
				
				for(i = 0;i<MAX_CLIENT;i++)
				{
					if(strcmp(send,info[i].nickname) == 0)
						write(info[i].list_c,content,strlen(content));
				}
			}
            }
        }
 }

int pushClient(int c_socket) {
	int i,n;
	char readname[20],join_user[100];
	int usercnt = 1;
	for(i = 0; i<MAX_CLIENT;i++)
		if(info[i].list_c != INVALID_SOCK)
			usercnt++;
	
	if(usercnt <= MAX_CLIENT)
	{
		printf("유저수 : %d\n",usercnt);
		n=read(c_socket,readname,sizeof(readname));
		readname[n] = '\0';
		pthread_mutex_lock(&mutex);
		for(i=0;i<MAX_CLIENT;i++)
		{
			if((info[i].list_c == INVALID_SOCK) && (strcmp(info[i].nickname,"init")) == 0)
			{
				info[i].list_c = c_socket;
				strcpy(info[i].nickname,readname);
				sprintf(join_user,"[%s] 님이 채팅에 참여했습니다.\n",info[i].nickname);
				break;
			}
		}
		
		for(i=0;i<MAX_CLIENT;i++)
			if(info[i].list_c != INVALID_SOCK)
				write(info[i].list_c,join_user,strlen(join_user));
		pthread_mutex_unlock(&mutex);
		return 0;
	}
	else
	{
		return -1;
	}
    
}
int popClient(int c_socket)
{
    
	int i;
	char pop_user[100];
	pthread_mutex_lock(&mutex);
	for(i=0;i<MAX_CLIENT;i++)
	{
		if(info[i].list_c == c_socket)
		{
			sprintf(pop_user,"[%s] 님이 채팅방에서 나갔습니다.\n",info[i].nickname);
			info[i].list_c = INVALID_SOCK;
			strcpy(info[i].nickname,"init");
			break;
		}
	}
	for(i=0;i<MAX_CLIENT;i++)
		if(info[i].list_c != INVALID_SOCK)
				write(info[i].list_c,pop_user,strlen(pop_user));
	pthread_mutex_unlock(&mutex);
	close(c_socket);
	return 0;
    
}
