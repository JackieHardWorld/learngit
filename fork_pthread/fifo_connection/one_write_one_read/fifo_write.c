#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(void)
{

	if(access("myfifo", F_OK) != 0)//判断文件是否存在
	{	
		mkfifo("myfifo", 0664);//创建一个有名管道文件
	}
	
	int fd;
	char buffer[100];
	
	fd = open("myfifo", O_RDWR);
	
	while(1)
	{
		scanf("%s", buffer);
	
		write(fd, buffer, strlen(buffer));
	}

	close(fd);

	return 0;
}
