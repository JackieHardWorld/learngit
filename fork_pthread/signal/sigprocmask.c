#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_sigint_func(int signum)
{
    printf("第%d号, 进程号=%d\n", signum, getpid());

    // 在收到 SIGINT 信号后，解除对 SIGTRAP 和 SIGFPE 信号的屏蔽
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTRAP);
    sigaddset(&set, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &set, NULL); // 解除屏蔽
}

void signal_sigtrap_func(int signum)
{
    printf("第%d号, 进程号=%d\n", signum, getpid());
}

void signal_sigfpe_func(int signum)
{
    printf("第%d号, 进程号=%d\n", signum, getpid());
}

int main(void)
{
    // 注册信号处理函数
    signal(SIGINT, signal_sigint_func);  // 处理 SIGINT 信号
    signal(SIGTRAP, signal_sigtrap_func); // 处理 SIGTRAP 信号
    signal(SIGFPE, signal_sigfpe_func);  // 处理 SIGFPE 信号

    sigset_t set; // 定义信号集变量

    // 初始化信号集，将其置为空
    sigemptyset(&set);

    // 向信号集中添加 SIGTRAP 和 SIGFPE 信号
    sigaddset(&set, SIGTRAP);
    sigaddset(&set, SIGFPE);

    // 将 SIGTRAP 和 SIGFPE 信号屏蔽（阻塞）
    sigprocmask(SIG_BLOCK, &set, NULL);

    while (1)
    {
        printf("等待信号中.....\n");
        pause(); // 挂起进程，直到信号到达
    }

    return 0;
}
