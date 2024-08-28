#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

// 自定义信号处理函数，用于处理 SIGINT 信号（信号编号 2）
// signum 是接收到的信号编号
void signal_sigint_func(int signum)
{
    // 打印接收到的信号编号和当前进程的进程号
    printf("第%d号, 进程号=%d\n", signum, getpid());
    // 退出程序
    exit(0);
}

int main(void)
{
    sigset_t set; // 定义一个信号集变量

    // 初始化信号集，将其置为空
    sigemptyset(&set);

    // 向信号集中添加 SIGQUIT 信号（信号编号 3）
    sigaddset(&set, SIGQUIT);

    // 设置信号屏蔽字，将 SIGQUIT 信号阻塞（屏蔽）
    sigprocmask(SIG_BLOCK, &set, NULL);

    // 注册 SIGINT 信号（信号编号 2）的处理函数
    // 当收到 SIGINT 信号时，将调用 signal_sigint_func 函数
    signal(SIGINT, signal_sigint_func);

    // 提示等待信号
    printf("等待信号中.....\n");

    // 暂停进程，等待接收信号
    pause(); // 进程在此挂起，直到接收到信号

    return 0;
}
