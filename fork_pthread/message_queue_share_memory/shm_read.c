#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

//***是的，读写操作所使用的 key 必须相同，
//***否则读写操作将针对不同的共享内存段进行，无法正确进行数据的共享。

int main(void)
{
    key_t key;
    
    // 生成一个唯一的IPC键值，"."表示当前目录，1是项目ID
    key = ftok(".", 1);
    
    int shm_id;
    
    // 根据生成的键值获取或创建一个共享内存段，大小为4096字节，权限为0664
    shm_id = shmget(key, 4096, IPC_CREAT | 0664);
    
    char *addr;
    
    struct shmid_ds ds; // 用于存储共享内存段的信息
    
    // 获取共享内存段的信息，并将其存储在ds结构中
    shmctl(shm_id, IPC_STAT, &ds);
    
    int ctlval;
    
    // 检查共享内存段是否已经被锁定
    if (ds.shm_perm.mode & SHM_LOCKED) // SHM_LOCKED 表示共享内存被锁定
    {
        printf("共享内存已经被锁定\n");
        return -1; // 如果共享内存被锁定，程序退出
    }
    
    // 锁定共享内存段，防止它被交换到磁盘上
    ctlval = shmctl(shm_id, SHM_LOCK, NULL);
    
    // 输出锁定操作的返回值和共享内存的大小
    printf("ctrlval=%d, 共享内存的大小为:%ld\n", ctlval, ds.shm_segsz);

    // 将共享内存段映射到当前进程的地址空间
    addr = shmat(shm_id, NULL, 0);
    
    // 输出共享内存中的内容
    printf("共享内存的内容为:%s", addr);

    // 将共享内存段从当前进程的地址空间中分离
    shmdt(addr);
    
    // 解锁共享内存段，允许它被交换到磁盘上
    ctlval = shmctl(shm_id, SHM_UNLOCK, NULL);
    
    // 删除共享内存段
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
