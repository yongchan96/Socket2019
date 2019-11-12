#include<stdio.h>

int main()
{
	int a = 10;
	int b = 100;
	int pid;

	a += 10;
	sleep(10);

	pid = fork();	
	// fork() 함수는 현재 구동 중인 프로세스의 복제본을 생성
	//	현재 프로세스 = 부모 프로세스
	//	fork()에 의해 생성된 프로세스 = 자식 프로세스
	//	리턴 값(pid) = 0, 자식프로세스
	//	리턴값 = 자식 프로세스의 pid값 if 부모프로세스
	//	만약 fork() 함수 실패시 -1 값을 리턴

	if(pid > 0) // 부모프로세스
	{
		printf("부모 프로세스\n");
		a += 10;
		printf("[Parent] a = %d, b = %d\n",a,b);
		sleep(20);
	}
	else if(pid == 0) // 자식프로세스
	{
		printf("자식 프로세스\n");
		b *= 10;
		printf("[Child] a = %d, b = %d\n",a,b);
		sleep(20);
	}
	else
	{
		printf("fork() 함수가 실패하였습니다.\n");
	}
	return 0;
}
