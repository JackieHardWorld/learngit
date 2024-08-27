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





int main(void)
{
    // 使用 ftok 函数生成一个唯一的关键字，用于后续的 IPC 操作
    key_t key;
    key = ftok(".", 1); // '.' 表示当前目录，1 是一个项目标识符

    // 创建或打开一个信号量，并赋予初始值 0
    sem_t *mysem;
    mysem = sem_open("mysem", O_CREAT, 0664, 0); // "mysem" 是信号量名称，O_CREAT 表示如果信号量不存在则创建，0664 是权限，0 是初始值

    // 初始化共享内存
    int shm_id;
    shm_id = shmget(key, 4096, IPC_CREAT | 0664); // 使用前面生成的 key，创建大小为 4096 字节的共享内存，IPC_CREAT 表示如果共享内存不存在则创建

    // 将共享内存映射到当前进程的地址空间
    char *addr;
    addr = shmat(shm_id, NULL, 0); // NULL 表示让系统选择映射地址，0 表示读写权限

    while(1)
    {
        // 提示用户输入数据
        printf("请输入数据:\n");
        scanf("%s", addr); // 读取用户输入的数据到共享内存
        sem_post(mysem); // 执行 V 操作，即增加信号量的值，这可以唤醒等待该信号量的其他进程

        // 注意：此处没有退出循环的机制，程序将无限循环
    }

    // 取消共享内存映射
    shmdt(addr);

    // 关闭信号量
    sem_close(mysem);

    return 0;
}
