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


int daemon_init(void)
{
	int max_fd;
	int i;
	pid_t pid;
	
	/*1，先忽略终端的挂断信号对这个程序的影响*/
	signal(SIGHUP, SIG_IGN);

	/*2，新建一个子进程，让父进程退出，这个时候子进程就不会接收到控制终端的信号跟内容*/
	pid = fork();
	if(pid > 0)
		exit(0);

	/*3，新建一个会话，脱离开原本的会话（也就是这个控制终端）*/
	setsid();

	/*下面的动作都是为了让我们的程序更加的纯净*/

	/*4，再让这个子进程脱离开原本的进程组*/
	//setpgrp();
	
	/*5，新建多一个子进程，脱离开会话管理的权限*/
	pid = fork();
	if(pid > 0)
		exit(0);


	/*6，关闭掉原本的所有文件描述符*/
	max_fd = sysconf(_SC_OPEN_MAX);
	for(i=0; i<max_fd; i++)
		close(i);

	/*7，改变工作路径到根目录*/
	//chdir("/");

	/*8，改变原有的掩码，成为没有任何权限影响的0*/
	umask(0);
	

	return 0;
}



int main(void)
{

	daemon_init();//让程序编程守护进程
	
	int fd;
	ssize_t rd_size;
	char buffer[11]={0};

	fd = open("a.txt", O_RDONLY);


	openlog("靓仔", LOG_CONS|LOG_PID, LOG_DAEMON);
	
	while(1)
	{
		sleep(2);

		memset(buffer, 0, sizeof(buffer));

		rd_size = read(fd, buffer, 10);
		if(rd_size == 0)
			break;

		syslog(LOG_INFO, "%s", buffer);
	}

	closelog();

	close(fd);

	mkfifo("myfifo", 0664);

	int fifo_fd = open("myfifo", O_RDWR);

	write(fifo_fd, "o", 1);

	close(fifo_fd);

	return 0;
}
