#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <time.h>





int main(void)
{
    // 使用 ftok 函数生成一个唯一的关键字，用于后续的 IPC 操作
    key_t key;
    key = ftok(".", 1); // '.' 表示当前目录，1 是一个项目标识符

    // 创建或打开一个信号量，并赋予初始值 0
    sem_t *mysem;
    mysem = sem_open("mysem", O_CREAT, 0664, 0); // "mysem" 是信号量名称，0664 是权限，0 是初始值

    // 初始化共享内存
    int shm_id;
    shm_id = shmget(key, 4096, IPC_CREAT | 0664); // 使用前面生成的 key，创建大小为 4096 字节的共享内存
    char *addr;
    addr = shmat(shm_id, NULL, 0); // 将共享内存映射到当前进程的地址空间


	// struct timespec time;
	// clock_gettime(CLOCK_REALTIME, &time);//获取当前系统时间
	
	// time.tv_sec += 5;
	// time.tv_nsec = 0;
	// int retval;
    while(1)
    {
        //等待信号量，如果信号量的值大于 0，则减 1 并继续执行；如果为 0，则阻塞
        sem_wait(mysem);//P操作

		// retval = sem_timedwait(mysem, &time);//P操作(加时等待，超时退出)
		// if(retval == -1)
		// {
		// 	perror("等待失败:");
		// 	return -1;
		// }

        // 输出共享内存中的内容，假设这是对方进程写入的数据
        printf("对方输入的数据=%s\n", addr);
    }

    // 取消共享内存映射
    shmdt(addr);
    // 关闭信号量
    sem_close(mysem);

    return 0;
}

