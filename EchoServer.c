#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>

#define PORT 10000

char buffer[100] = "Hi, I'm server.\n";
//sizeof 배열의 크기 = 100
//strlen 배열에 저장된 문자열의 길이 = 15
char rcvbuffer[100];
int main()
{
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;
	
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
		printf("클라이언트 접속 허용\n");

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
				char *filename;
				FILE *fp;
				token = strtok(rcvbuffer," ");
				filename=strtok(NULL,"\n ");
				fp = fopen(filename,"r");
				while(fgets(buffer,255,(FILE *)fp))
				{
					printf("%s\n",buffer);
					write(c_socket,buffer,strlen(buffer));
				}
				fclose(fp);
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
		if(strncasecmp(rcvbuffer,"kill server",11)==0)
		break;
		close(c_socket);
	}

	close(s_socket);
	return 0;


}
