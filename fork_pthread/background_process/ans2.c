#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <strings.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(void)
{

	pid_t pid;
	
	
	pid = vfork();
	if(pid == 0)
	{


		execl("./ans1", "ans1", NULL);
		
		exit(0);
	}

	mkfifo("myfifo", 0664);

	int fifo_fd = open("myfifo", O_RDWR);

	char buf;

	read(fifo_fd, &buf, 1);

	close(fifo_fd);
	

	return 0;
}
