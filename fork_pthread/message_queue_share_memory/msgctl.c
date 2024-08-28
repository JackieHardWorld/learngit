#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//******************************************************************************************************************************
//一、本节主要是学习获取消息队列的一些参数
//1.要注意定义一个struct msqid_ds ds;结构体
//2.主要获取消息队列的容量，当前消息数，当前消息总大小这几个参数
//二、主要学习的函数 
//1.struct msqid_ds ds; msgctl(msg_id, IPC_STAT, &ds);//获取消息队列的状态
//2.msgctl(msg_id, IPC_SET, &ds);//设置消息队列的状态
//3.printf("消息队列的容量:%ld, 当前消息数%ld，当前消息总大小:%ld\n", ds.msg_qbytes, ds.msg_qnum, ds.msg_cbytes);//打印当前信息参数的结构体成员

//********************************************************************************************************************************




struct msgbuf {
               long mtype;       /* message type, must be > 0 */
               char mtext[100];    /* message data */
           };


int main(void)
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
	
	
	struct msgbuf buf;
	
	buf.mtype = 250;
	
	struct msqid_ds ds;
	

	while(1)
	{
		msgctl(msg_id, IPC_STAT, &ds);//获取消息队列的状态
		ds.msg_qbytes = 1000;
		
		msgctl(msg_id, IPC_SET, &ds);//获取消息队列的状态
	
		scanf("%s", buf.mtext);
		if(strcmp(buf.mtext, "exit") == 0)
			break;
	
		msgsnd(msg_id, &buf, strlen(buf.mtext), 0);
		
		msgctl(msg_id, IPC_STAT, &ds);//获取消息队列的状态

		printf("消息队列的容量:%ld, 当前消息数%ld，当前消息总大小:%ld\n", ds.msg_qbytes, ds.msg_qnum, ds.msg_cbytes);
	}
	
	
	msgctl(msg_id, IPC_RMID, NULL);//删除消息队列
	

	return 0;
}
