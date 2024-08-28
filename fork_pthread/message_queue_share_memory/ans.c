#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//******************************************************************************************************************************
//一、本节主要是学习获取消息队列的A->B,B->C,C->A,闭环发接数据
//1.要注意定义一个struct msqid_ds ds;结构体
//2.主体设计为：主进程死循环发送数据，创建子进程死循环来接收数据。
//3.巧妙利用linux的main函数传参
//4.开启三个终端，同时执行此函数

//********************************************************************************************************************************

struct msgbuf {
               long mtype;       /* message type, must be > 0 */
               char mtext[100];    /* message data */
           };

//./ans  A  B
int main(int argc, const char *argv[])
{

	//获取IPC的key
	key_t key;
	
	key = ftok(".", 1);
	if(key == -1)
	{
		perror("获取IPC的key失败");
		return -1;
	}

	int msg_id;
	
	msg_id = msgget(key, IPC_CREAT|0664);//获取消息队列这个对象
	if(msg_id == -1)
	{
		perror("获取消息队列失败");
		return -1;
	}
	
	struct msgbuf buf;//创建消息结构体对象
	
	if(fork() == 0)//子进程
	{
		ssize_t rcv_size;

		while(1)
		{
			rcv_size = msgrcv(msg_id, &buf, sizeof(buf), argv[1][0], 0);//为什么是argv[1][0]，因为argv[0]已经给（执行程序了）./ans
			
			printf("接收到%ld消息%ld个字节的数据:%s\n", buf.mtype, rcv_size, buf.mtext);
		}
	
		exit(0);
	}
	
	
	
	
	buf.mtype = argv[2][0];//argv[2][0]为char类型，但是buf.mtype为long类型（这里应该发生了整形提升）

	while(1)
	{
		scanf("%s", buf.mtext);
	
		msgsnd(msg_id, &buf, strlen(buf.mtext), 0);
	}

	return 0;
}
