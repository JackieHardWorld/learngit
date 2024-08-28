#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int signal_flag = 0; // 全局变量，用于标记 SIGINT 信号的到达

// SIGINT 信号处理函数
void signal_sigint_func(int signum)
{
    signal_flag = 1; // 当接收到 SIGINT 信号时，设置标志位
}

int main(void)
{
    char buffer[100]; // 用于存储用户输入
    // 打开文件 "a.txt"，如果文件不存在则创建它，文件权限为 0664
    // 使用 O_TRUNC 选项，如果文件存在则清空文件
    int fd = open("a.txt", O_RDWR | O_CREAT | O_APPEND, 0664);
    if (fd == -1) {
        perror("文件打开失败");
        return 1;
    }

    // 注册 SIGINT 信号处理函数
    signal(SIGINT, signal_sigint_func);

    // 无限循环，等待信号
    while(1)
    {
        printf("等待信号中...\n");
        pause(); // 挂起进程，直到接收到信号

        // 如果接收到 SIGINT 信号（即 signal_flag == 1）
        if(signal_flag == 1)
        {
            memset(buffer, 0, sizeof(buffer)); // 清空 buffer
            printf("请输入一句话\n");
            fgets(buffer, sizeof(buffer), stdin); // 从标准输入读取用户输入
            write(fd, buffer, strlen(buffer)); // 将用户输入写入文件
            signal_flag = 0; // 重置标志位
        }
    }

    close(fd); // 关闭文件

    return 0;
}


