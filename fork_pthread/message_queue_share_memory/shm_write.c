#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

//***是的，读写操作所使用的 key 必须相同，
//***否则读写操作将针对不同的共享内存段进行，无法正确进行数据的共享。

int main(void)
{
    key_t key;

    // 使用当前目录和整数1生成一个唯一的IPC键值
    // 这个键值将用于标识共享内存段
    key = ftok(".", 1); // 获取IPC的key，"."表示当前目录，1是项目ID
                        
    int shm_id;
    // 使用生成的键值创建一个共享内存段，大小为4096字节
    // IPC_CREAT表示如果共享内存不存在则创建，0664是权限设置
    shm_id = shmget(key, 4096, IPC_CREAT | 0664); // 获取共享内存ID

    char *addr;
    
    // 锁定共享内存段，防止其被交换到磁盘上
    // 此操作将确保共享内存驻留在物理内存中
    shmctl(shm_id, SHM_LOCK, NULL); // 锁定共享内存段

    // 将共享内存段映射到进程的地址空间
    // NULL表示由系统决定映射到哪个地址，0表示以读写模式映射
    addr = shmat(shm_id, NULL, 0); // 映射共享内存

    // 读取用户输入，并将输入内容存储到共享内存段中
    // 注意：addr 是一个指向共享内存的指针
    scanf("%s", addr); // 将用户输入的字符串存储到共享内存中

    // 将共享内存段从当前进程的地址空间中分离
    shmdt(addr); // 取消内存映射

    // 解锁共享内存段，允许它被交换到磁盘上
    shmctl(shm_id, SHM_UNLOCK, NULL); // 解锁共享内存段

    return 0;
}
