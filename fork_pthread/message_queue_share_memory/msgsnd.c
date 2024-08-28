#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//***是的，读写操作所使用的 key 必须相同，
//***否则读写操作将针对不同的共享内存段进行，无法正确进行数据的共享。
//***消息队列相当与管道的plus version ,send message and receive message all have numberal logo
//***要注意他有一个信息结构体
//***msgget(key, IPC_CREAT|0664);//获取消息队列这个对象
//***msgsnd(msg_id, &buf, strlen(buf.mtext), 0);//消息队列发送函数，最后一个参数默认填0
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

	while(1)
	{
		scanf("%s", buf.mtext);
	
		msgsnd(msg_id, &buf, strlen(buf.mtext), 0);
	}

	return 0;
}
