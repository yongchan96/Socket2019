#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>

#define PORT 10000
#define IPADDR "127.0.0.1"
#define BUFSIZE 100

int main()
{
	int n;
	int c_socket;
	char sendbuffer[100];
	struct sockaddr_in c_addr;
	
	char rcvBuffer[BUFSIZE];// 서버에서 보내준 메시지를 저장하는 변수

	//1. 클라이언트 소켓 생성
	
	c_socket = socket(PF_INET, SOCK_STREAM, 0); // 서버와 동일한 설정으로 생성

	//2. 소켓 정보 초기화

	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(IPADDR); // 접속할 IP 설정(127.0.0.1)
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(PORT);

	//3. 서버에 접속

	if(connect(c_socket, (struct sockaddr * )&c_addr, sizeof(c_addr)) == -1)
	{
		//서버 접속에 실패하면
		printf("Cannot Connect\n"); // Cannot Connect 메세지 출력
		close(c_socket); // 자원회수
		return -1; //프로세스 종료
	}
	while(1)
	{
		fgets(sendbuffer,sizeof(sendbuffer),stdin);	
		write(c_socket,sendbuffer,sizeof(sendbuffer));
		if(strncasecmp(sendbuffer,"quit",4)==0)
				break;
		n = read(c_socket,rcvBuffer,sizeof(rcvBuffer)); 
		//서버에서 보내준 메세지를 rcvBuffer에 저장하고 메세지 길이를 n에 저장
		//만약 read에 실패하면 -1을 리턴	
		if(n < 0)
		{
			printf("Read Failed\n");
			return -1;
		}
		printf("received data : %s\n" , rcvBuffer);
		printf("rcvBuffer length : %d\n",n);
	}
		close(c_socket);
	return 0;
	
}
