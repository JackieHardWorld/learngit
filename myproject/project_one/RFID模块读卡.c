#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// 添加串口头文件
#include <termios.h>
#include <unistd.h>
#include <string.h>

int set_serial_uart(int ser_fd)
{
    struct termios new_cfg, old_cfg;

    /*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
    if (tcgetattr(ser_fd, &old_cfg) != 0)
    {
        perror("tcgetattr");
        return -1;
    }

    bzero(&new_cfg, sizeof(new_cfg));
    /*原始模式*/
    /* 设置字符大小*/
    new_cfg = old_cfg;
    cfmakeraw(&new_cfg); /* 配置为原始模式 */

    /*波特率为9600*/
    cfsetispeed(&new_cfg, B9600);
    cfsetospeed(&new_cfg, B9600);

    new_cfg.c_cflag |= CLOCAL | CREAD;

    /*8位数据位*/
    new_cfg.c_cflag &= ~CSIZE;
    new_cfg.c_cflag |= CS8;

    /*无奇偶校验位*/
    new_cfg.c_cflag &= ~PARENB;

    /*1位停止位*/
    new_cfg.c_cflag &= ~CSTOPB;
    /*清除串口缓冲区*/
    tcflush(ser_fd, TCIOFLUSH);

    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 1; // 接收一个字节立即返回
    tcflush(ser_fd, TCIOFLUSH);
    /*串口设置使能*/
    tcsetattr(ser_fd, TCSANOW, &new_cfg);
}

int main()
{
    // 1.打开串口设备
    int fd = open("/dev/ttySAC2", O_RDWR);
    if (fd < 0)
    {
        perror("打开串口失败\n");
        return -1;
    }

    // 2.配置串口协议
    set_serial_uart(fd);

    // 3.设计请求协议
    char requst[8] = {0};
    requst[0] = 0x07; // 帧长
    requst[1] = 0x02; // 命令类型
    requst[2] = 0x41; // 命令
    requst[3] = 0x01; // 长度
    requst[4] = 0x52; // 信息
    char BCC = 0;     // 校验和
    for (int i = 0; i < (requst[0] - 2); i++)
    {
        BCC ^= requst[i];
    }
    requst[5] = ~BCC; // 校验和
    requst[6] = 0x03; // 结束标志

    // 发起请求
    write(fd, requst, 7);

    // 读取RFID 模块返回的信息
    char ret[9] = {0};
    read(fd, ret, 9);
    if (ret[2] == 0x00)
    {
        printf("请求成功\n");

        // 防碰撞协议
        char Collision[9] = {0};
        Collision[0] = 0x08;
        Collision[1] = 0x02;
        Collision[2] = 0x42;
        Collision[3] = 0x02;
        Collision[4] = 0x93;
        Collision[5] = 0x00;
        char BCC = 0; // 校验和
        for (int i = 0; i < (Collision[0] - 2); i++)
        {
            BCC ^= Collision[i];
        }
        Collision[6] = ~BCC; // 校验和
        Collision[7] = 0x03;

        // 发送防碰撞
        write(fd, Collision, 8);

        // 读取卡号
        char id[11] = {0};
        read(fd, id, 11);

        if (id[2] == 0x0)
        {
            printf("读卡成功\n");

            char rfid[20]; // 卡号

            sprintf(rfid, "%x%x%x%x", id[7], id[6], id[5], id[4]);

            printf("卡号:%s\n", rfid);
        }
        else
        {
            printf("读卡失败\n");
        }
    }
    else
    {
        printf("请求失败\n");
    }
}