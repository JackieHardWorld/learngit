#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

//./kill 信号数  进程号
int main(int argc, const char* argv[])
{
    // 检查参数是否足够
    if (argc != 3) {
        fprintf(stderr, "用法: %s <信号数> <进程号>\n", argv[0]);
        return -1;
    }

    // 将命令行参数argv[1]转换为整数，代表信号数
    int signal_num = atoi(argv[1]);

    // 将命令行参数argv[2]转换为整数，代表进程号
    pid_t pid = atoi(argv[2]);

    // 发送信号给指定进程
    if (kill(pid, signal_num) == -1) {
        perror("发送信号失败");
        return -1;
    }

    printf("成功发送信号 %d 给进程 %d\n", signal_num, pid);

    return 0;
}
