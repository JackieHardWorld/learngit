#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
	pid_t pid;

	int pfd[2];//匿名管道只能作用于亲缘进程

	pipe(pfd);//创建出一条管道，其中pfd[0]是读端，pfd[1]是写端


	pid = fork();
	if(pid == 0)
	{
		char buffer[100];
		close(pfd[0]);
		
		while(1)
		{
			memset(buffer, 0, sizeof(buffer));
		
			read(pfd[0], buffer, sizeof(buffer));//如果没有内容，则会阻塞
			
			printf("子进程读取管道内容：buffer=%s\n", buffer);
		}
	
		exit(0);
	}


	char buffer[100]={0};
	int i=0;
	
	
	while(1)
	{
		close(pfd[0]);
	
		scanf("%s", buffer);
		
		
		write(pfd[1], buffer, strlen(buffer));//如果缓冲区满了，则会阻塞(缓冲区大小64k)
		
		
	}



	return 0;
}
