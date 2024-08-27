#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>


// 定义信号量操作函数
int sem_operation(int sem_id, int label, int op) {
    struct sembuf buf;
    buf.sem_num = label; // 指定要操作的信号量下标
    buf.sem_op = op;     // 指定操作类型，正数表示V操作（增加），负数表示P操作（减少）
    buf.sem_flg = 0;     // 设置信号量操作标志，0表示默认操作

    // 执行信号量操作，1表示操作数组中元素的个数
    return semop(sem_id, &buf, 1);
}

int main(void) {
    key_t key;

    // 使用 ftok 生成唯一键
    key = ftok(".", 1); // 路径名，项目标识符

    // 初始化信号量
    int sem_id;
    sem_id = semget(key, 2, IPC_CREAT | 0664); // 创建信号量集，2表示信号量的数量

    // 设置信号量的初始值
    semctl(sem_id, 0, SETVAL, 1); // 设置第0个信号量（空间资源）的初始值为1
    semctl(sem_id, 1, SETVAL, 0); // 设置第1个信号量（数据资源）的初始值为0

    // 初始化共享内存
    int shm_id;
    shm_id = shmget(key, 4096, IPC_CREAT | 0664); // 创建共享内存，大小为4096字节
    char *addr;
    addr = shmat(shm_id, NULL, 0); // 将共享内存映射到当前进程的地址空间

    while(1) {
        // 进行空间资源P操作（减少信号量）
        sem_operation(sem_id, 0, -1);

        printf("请输入数据:\n");
        scanf("%s", addr); // 读取用户输入的数据到共享内存

        // 进行数据资源V操作（增加信号量）
        sem_operation(sem_id, 1, 1);
    }

    // 取消共享内存映射
    shmdt(addr);

    return 0;
}

