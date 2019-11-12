#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>

#define PORT 10000
#define BUFSIZE 10000


char buffer[BUFSIZE]="HI, I'm Server";
//sizeof 배열의 크기 = 100
//strlen 배열에 저장된 문자열의 길이 = 15
char rcvbuffer[100];

void do_service(int c_socket);
void sig_handler(int signo);

int numClient = 0;
int main()
{
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;
	signal(SIGCHLD,sig_handler); // 첫번째 인자 : 시그널 번호, 두번째 인자 : 첫번째 인자의 시그널이 발생했을때 실행하는 함수
	
	//1. 서버 소켓 생성
	// 서버 소켓 = 클라이언트의 접속 요청을 처리(허용)해 주기 위한 소켓
	s_socket = socket(PF_INET, SOCK_STREAM, 0);//TCP/IP 통신을 위한 서버 소켓 생성
	
	 //2. 서버 소켓 주소 설정
	memset(&s_addr, 0, sizeof(s_addr)); //s_addr의 값을 모두 0으로 초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); // IP 주소 설정
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	
	//3. 서버 소켓 바인딩
	if(bind(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1)
	{
		//바인딩 작업 실패시 , Cannot Bind 메시지 출력후 프로그램 종료
		printf("Cannot Bidn\n");
		return -1;
	}

	//4. listen() 함수 실행

	if(listen(s_socket, 5) == -1)
	{
		printf("Listen Fail\n");
		return -1;
	}
	
	//5. 클라이언트 요청 처리
	//  요청을 허용한 후, HelloWorld 메시지를 전송함
	while(1)
	{
		len = sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr * )&c_addr, &len); 
	//클라이언트의 요청이 오면 허용(accept)해 주고, 해당 클라이언트와 통신 할 수 있도록 클라이언트 소켓(c_socket)을 반환함.
		printf("/client is connected\n");
		numClient++;
		printf("현재 접속 중인 클라이언트 수 : %d\n",numClient);
		int pid = fork();
		if(pid > 0) // 부모프로세스		
		{
			close(c_socket); //c_socket의 값도 복사되기때문에 부모프로세스에서는 c_socket을 닫아 자원회수
			continue;
		} 
		else if(pid == 0)
		{
			close(s_socket);// 자식프로세스는 accept하는 역할이 아니기 때문에 s_socket이 필요 없다.
			do_service(c_socket);
			exit(0);
		}
		else
		{
			printf("fork() failed\n");
			exit(0);
		}
		
		
	}

	close(s_socket);
	return 0;

}

void do_service(int c_socket)
{
	int n;

	while(1)
	{
		n = read(c_socket, rcvbuffer, sizeof(rcvbuffer));
		rcvbuffer[n - 1] = '\0'; //개행 문자 삭제

		if(strncasecmp(rcvbuffer,"quit",4)==0 || strncasecmp(rcvbuffer,"killserver",11)==0)
			break;
		else if(!strncasecmp(rcvbuffer,"안녕하세요",strlen("안녕하세요")))
			strcpy(buffer,"안녕하세요. 만나서 반가워요.");
		else if(!strncasecmp(rcvbuffer,"이름이 머야?",strlen("이름이 머야?")))
			strcpy(buffer,"내 이름은 김용찬이야.");
		else if(!strncasecmp(rcvbuffer,"몇 살이야?",strlen("몇 살이야?")))
			strcpy(buffer,"나는 24살이야");
		else if(!strncasecmp(rcvbuffer,"strlen ",strlen("strlen ")))
			//문자열의 길이는 XX입니다.
			sprintf(buffer,"문자열의 길이는 %d 입니다.",strlen(rcvbuffer)-7);
		else if(!strncasecmp(rcvbuffer,"strcmp ",strlen("strcmp ")))
		{
			char *token;
			char *str[3];
			int idx = 0;
			token = strtok(rcvbuffer, " ");	
			while(token != NULL)
			{
				str[idx] = token;
				printf("str[%d] = %s\n",idx,str[idx]);
				idx++;
				token = strtok(NULL, " ");
			}
			if(idx < 3)
				strcpy(buffer,"문자열 비교를 위해서는 두 문자열이 필요합니다.");
			else if(!strcmp(str[1],str[2]))
				sprintf(buffer,"%s와 %s는 같은 문자열입니다.",str[1],str[2]);
			else
				sprintf(buffer,"%s와 %s는 다른 문자열입니다.",str[1],str[2]);
		
		}
		else if(!strncasecmp(rcvbuffer,"readfile ",strlen("readfile ")))
		{
			char *token;
			char *str[10];
			int cnt=0;
			token = strtok(rcvbuffer," ");
			while(token != NULL)
			{
				str[cnt]=token;
				cnt++;
				token = strtok(NULL," ");
			}
			if(cnt < 2)
			{
				strcpy(buffer,"파일명을 입력해주세요");
			}

			else
			{
				FILE *fp = fopen(str[1],"r");
				if(fp)
				{ 
					char tempStr[BUFSIZE];
					memset(buffer,0,BUFSIZE);
					while(fgets(tempStr,BUFSIZE,(FILE *)fp))
					{
						strcat(buffer,tempStr); //여러줄의 내용을 저장하기위해 사용
					}
				}
				else // 파일이 존재하지 않는 경우
				{
					strcpy(buffer,"해당파일이 존재하지 않습니다.");
				}
				fclose(fp);
			}
			
		}

		else if(!strncasecmp(rcvbuffer,"exec ",strlen("exec ")))
		{
			char *token;
			int command;
			strtok(rcvbuffer," ");
			token = strtok(NULL,"\0");
			command = system(token);
			if(command == 0)
			{
				sprintf(buffer,"[%s] command is executed",token);	 
			}
			else
			{
				sprintf(buffer,"[%s] command is failed",token);		
			}
		}
		else
			strcpy(buffer,"무슨 말인지 모르겠습니다.");

		write(c_socket,buffer,strlen(buffer));
	

	}
	close(c_socket);
}
void sig_handler(int signo)
{
	int pid;
	int status;
	pid = wait(&status);
	printf("pid[%d] is terminated.status = %d\n",pid,status);
	numClient--;
	printf("현재 접속 중인 클라이언트 수 : %d\n",numClient);
}
