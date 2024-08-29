#include <stdio.h>
// 添加字库的头文件
#include "font.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

// 延时函数头文件
#include <unistd.h>
// 添加输入模型头文件
#include <linux/input.h>
// 添加串口头文件
#include <termios.h>
#include <string.h>

// 线程的头文件
#include <pthread.h>




int money_add_flag = 0;//充值标志位
int money_sub_flag = 0;//消费标志位
// 设置一个卡结构体
struct card
{
    char rfid[1024]; // 卡号
    int money;       // 当前卡的金额
};

struct card arry[1024]; // 卡管理数组
int card_num = 0;       // 当前卡的数量

// 当前操作的卡号
char rfid[20];

int keyboard_figure[8];// 键盘输入的数字

// 全局变量，所有函数都可以访问该变量
unsigned int *lcd = NULL;

// 全局变量，所有函数都可以操作字库
font *f;

void clear(int color)
{
    for (int i = 0; i < 800 * 480; i++)
    {
        lcd[i] = color;
    }
}

// 初始化LCD设备
int mmap_lcd()
{
    // 1.打开LCD设备
    int fd = open("/dev/fb0", O_RDWR);
    if (fd == -1)
    {
        perror("Failed to open LCD device");
        return -1;
    }
    else
    {
        printf("LCD device opened successfully\n");
    }

    // 2.映射LCD设备到内存 ,并获取二维指针
    lcd = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (lcd == MAP_FAILED)
    {
        perror("Failed to mmap LCD device");
        return -1;
    }
    else
    {
        printf("LCD device mmaped successfully\n");
    }

    close(fd);
}

// 初始化字库
int init_font(int size)
{
    // 1.加载字库文件
    f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
    if (f == NULL)
    {
        printf("load font fail:\n");
        return -1;
    }

    // 2.设置字体的大小
    fontSetSize(f, size);
}

int bmp0487()
{
    // 1.打开LCD设备
    int fd = open("/dev/fb0", O_RDWR);
    if (fd == -1)
    {
        perror("Failed to open LCD device");
        return -1;
    }
    else
    {
        printf("LCD device opened successfully\n");
    }

    // 2.映射LCD设备到内存 ,并获取二维指针
    int(*p)[800] = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        perror("Failed to mmap LCD device");
        return -1;
    }
    else
    {
        printf("LCD device mmaped successfully\n");
    }

    // 3.打开图片文件
    int fd2 = open("0487.bmp", O_RDONLY);
    if (fd2 == -1)
    {
        perror("Failed to open bmp file");
        return -1;
    }
    else
    {
        printf("bmp file opened successfully\n");
    }

    // 4.读取图片的头数据
    char head[54];
    read(fd2, head, 54);

    // 5.解析图片的宽度和高度
    int width = *(int *)&head[18];
    int height = *(int *)&head[22];

    printf("width:%d, height:%d\n", width, height);

    // 6.读取图片的像素数据
    char rgb[800 * 480 * 3];
    read(fd2, rgb, 800 * 480 * 3);

    // 7.把24位的像素数据转换为32位
    char rgb32[800 * 480 * 4];
    for (int i = 0; i < 800 * 480; i++)
    {
        rgb32[0 + i * 4] = rgb[0 + i * 3];
        rgb32[1 + i * 4] = rgb[1 + i * 3];
        rgb32[2 + i * 4] = rgb[2 + i * 3];
        rgb32[3 + i * 4] = 0;
    }

    // 8.把像素数据转换为与映射地址匹配的类型
    int(*color)[800] = (void *)rgb32;

    // 9.把颜色放入对应的映射地址
    for (int y = 0; y < 480; y++)
    {
        for (int x = 0; x < 800; x++)
        {
            p[y][x] = color[480-y][x]; // 把对应的坐标颜色点，放入对应的位置
        }
    }
}

int show_text(int x, int y, int w, int h, char *text)
{
    // 3.创建一块画板
    bitmap *bm = createBitmap(w, h, 4);
    if (bm == NULL)
    {
        printf("create bitmap fail\n");
        return -1;
    }
    //3.可以指定输出框的颜色
    bm = createBitmapWithInit(w, h, 4,0x0000ff);  //蓝色

    // 4.把文字输出到画板中               //R G  B   A
    fontPrint(f, bm, 0, 0, text, getColor(178, 231, 248, 00), 500);

    // 5.把画板输出到LCD映射地址上
    show_font_to_lcd(lcd, x, y, bm);

    // 销毁画板
    destroyBitmap(bm);
}


// 获取触摸屏的坐标信息
int get_touch(int *x, int *y)
{
    // 1.打开触摸屏设备文件
    int fd = open("/dev/input/event0", O_RDWR);
    if (fd < 0)
    {
        printf("open touch failed!\n");
        return -1;
    }
    else
    {
        printf("open touch success!\n");
    }

    while (1) // 不断读取输入信息
    {
        // 2.读取触摸屏输入信息到 输入模型结构体中
        struct input_event ev;
        read(fd, &ev, sizeof(ev));

        // 3.处理输入模型的信息，判断哪个设备输入
        if (ev.type == EV_ABS)
        {
            // printf("touch input\n");

            // 4.判断是否为X轴坐标输入
            if (ev.code == ABS_X)
            {
                 *x = ev.value * 800 / 1020; // 校准屏坐标与屏幕分辨率
            }

            // 5.判断是否为Y轴坐标输入
            if (ev.code == ABS_Y)
            {
                *y = ev.value * 480 / 600;
            }
        }

        // 判断用户手指是否松开
        if (ev.type == EV_KEY && ev.code == BTN_TOUCH)
        {
            if (ev.value == 0) // 松开
            {
                // printf("Touch released\n");
                break;
            }

            if (ev.value == 1) // 按下
            {
                // printf("Touch pressed\n");
            }
        }
    }

    // 关闭触摸屏
    close(fd);
}



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

int read_card()
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


            sprintf(rfid, "%x%x%x%x", id[7], id[6], id[5], id[4]);

            printf("卡号:%s\n", rfid);
            
            return 0;
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


int out_put()
{
    int x, y;
    static int  cnt =0;
    get_touch(&x, &y);
            //键盘第一列
    if(x> 460 && x<540 && y>170 && y<250)//数字1
    {
        printf("数字1\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "1");
        //strcat(keyboard_figure, "1");
        keyboard_figure[cnt]=1;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    if(x> 460 && x<540 && y>250 && y<320)//数字4
    {
        printf("数字4\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "4");
        // strcat(keyboard_figure, "4");
        keyboard_figure[cnt]=4;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    if(x> 460 && x<540 && y>320 && y<390)//数字7
    {
        printf("数字7\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "7");
        // strcat(keyboard_figure, "7");
        keyboard_figure[cnt]=7;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    if(x> 460 && x<540 && y>390 && y<480)//数字0
    {
        printf("数字0\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "0");
        // strcat(keyboard_figure, "0");
        keyboard_figure[cnt]=0;
        cnt++;
        cnt%=7;
         printf("cnt%d\n",cnt);
        //显示 

    }
    //键盘第二列
    if(x> 540 && x<610 && y>170 && y<250)//数字2
    {
        printf("数字2\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "2");
        // strcat(keyboard_figure, "2");
        keyboard_figure[cnt]=2;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    if(x> 540 && x<610 && y>250 && y<320)//数字5
    {
        printf("数字5\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "5");
        // strcat(keyboard_figure, "5");
        keyboard_figure[cnt]=5;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    if(x> 540 && x<610 && y>320 && y<390)//数字8
    {
        printf("数字8\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "8");
        // strcat(keyboard_figure, "8");
        keyboard_figure[cnt]=8;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    if(x> 540 && x<610 && y>390 && y<480)//符号.
    {
        printf("符号.\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, ".");
        // strcat(keyboard_figure, ".");
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    //键盘第三列
    if(x> 610 && x<690 && y>170 && y<250)//数字3
    {
        printf("数字3\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "3");
        // strcat(keyboard_figure, "3");
        keyboard_figure[cnt]=3;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    if(x> 610 && x<690 && y>250 && y<320)//数字6
    {
        printf("数字6\n");

        show_text(450+cnt*50, 85, 350-cnt*50, 50, "6");
        // strcat(keyboard_figure, "6");
        keyboard_figure[cnt]=6;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    if(x> 610 && x<690 && y>320 && y<390)//数字9
    {
        printf("数字9\n");
        show_text(450+cnt*50, 85, 350-cnt*50, 50, "9");
        // strcat(keyboard_figure, "9");
        keyboard_figure[cnt]=9;
        cnt++;
        cnt%=7;
        printf("cnt%d\n",cnt);
        //显示 

    }
    //键盘第四列
     if(x> 690 && x< 800 && y>170 && y<250)//退出
    {
        show_text(10, 100, 410, 60, "退卡成功");
        // 清空数组
        memset(rfid, 0, 20 * sizeof(char));
        show_text(10, 400, 285, 50, " ");
        // clear(0x000000);
        // return 0;
        printf("退出\n");

    } 
    if(x> 690 && x< 800 && y>250 && y<320)//删除
    {
        printf("删除\n");
        show_text(450, 85, 350, 50, " ");
        // 清空数组
        memset(keyboard_figure, 0, 8 * sizeof(int));
        cnt = 0;

        

    }
    if(x> 690 && x< 800 && y>320 && y<390)//退格
    {
        printf("退格\n");
        cnt--;
        if(cnt<0)
        {
            cnt = 0;
        }
        show_text(450+cnt*50, 85, 350-cnt*50, 50, " ");
        keyboard_figure[cnt] = 0;
    }
    if(x> 610 && x< 800 && y>390 && y<480)//确定
    {
        if(money_add_flag == 1)
        {
            int i = 0;      
            for (i = 0; i < card_num; i++)// 遍历数组看看当前的卡号是否已经存在
            {
                if (strcmp(arry[i].rfid, rfid) == 0) // 卡号存在
                {
                     int add =0;//充值金额
                    int sum =0;
                    char temp[10]; // 用于转换成字符串的数组
                    // add=(int)(keyboard_figure[0]-'0')*10+(int)(keyboard_figure[1]-'0');//将键盘上获取的俩数字（充值金额）转换成整数
                    int arr_size = sizeof(keyboard_figure) / sizeof(keyboard_figure[0]);

                    for (int j = 0; j < cnt; j++)
                    {
                        
                        
                            sum = sum * 10 + keyboard_figure[j];
                        
                        
                    }
                    

                    add += sum;
                    if(add>0 && add<10000)
                    {
                        arry[i].money+=add;
                        sprintf(temp,"%d",add);// 将整数转换成字符串（将充值金额转换为字符串输出）
                    
                        memset(keyboard_figure, 0, 8 * sizeof(int));// 清空数组

                        show_text(10, 100, 410, 60,strcat(temp,"元充值成功"));
                        printf("充值成功，总金额：%d\n",arry[card_num].money);
                        show_text(450, 85, 350, 50, " "); //清空键盘输入
                        cnt = 0;
                        break;
                    }
                    else
                    {
                         show_text(10, 100, 410, 60,"请输入充值金额");
                         money_add_flag = 1;
                         break;

                    }
                    
                    
                }
            }

            // 当前卡号不存在
            if (strlen(rfid) > 0 && i == card_num)
            {
                show_text(10, 100, 410, 60, "卡号不存在");
            }
            money_add_flag = 0;
        }
        if(money_sub_flag == 1)
        {
            int i = 0;
            for (i = 0; i < card_num; i++)// 遍历数组看看当前的卡号是否已经存在
            {
                if (strcmp(arry[i].rfid, rfid) == 0) // 卡号存在
                {
                    int sub =0;//消费金额
                    int sum =0;
                    char temp[10];// 用于转换成字符串的数组
                    // sub=(int)(keyboard_figure[0]-'0')*10+(int)(keyboard_figure[1]-'0');//将键盘上获取的俩数字（消费金额）转换成整数
                    int arr_size = sizeof(keyboard_figure) / sizeof(keyboard_figure[0]);

                    for (int j = 0; j < cnt; j++)
                    {
                    
                        
                            sum = sum * 10 + keyboard_figure[j];
                        
                        
                    }

                    sub += sum;
                    if(sub>0 && sub<arry[i].money)
                    {
                        arry[i].money-=sub;
                        sprintf(temp,"%d",sub);// 将整数转换成字符串（将消费金额转换为字符串输出）
                        // 清空数组
                        memset(keyboard_figure, 0, 8 * sizeof(int));

                        show_text(10, 100, 410, 60,strcat(temp,"元消费成功"));
                        printf("消费成功，总金额：%d\n",arry[card_num].money);
                        show_text(450, 85, 350, 50, " ");// 清空键盘输入
                        cnt = 0;
                        break;
                    }
                    else
                    {
                        show_text(10, 100, 410, 60,"余额不足，请充值");
                        show_text(450, 85, 350, 50, " "); //清空键盘输入
                        cnt =0;
                        money_sub_flag = 1;
                        break;

                    }
                }
            }

            // 当前卡号不存在
            if (strlen(rfid) > 0 && i == card_num)
            {
                show_text(10, 100, 410, 60, "卡号不存在");
            }
            money_sub_flag = 0;

        }
        printf("确定\n");
        
    }
    if(x> 310 && x< 400 && y>380 && y<480)//读卡
    {
        printf("读卡\n");
        read_card();
        show_text(10, 400, 285, 50, rfid);//rfid设为全局变量
        show_text(10, 100, 410, 60, "读卡成功");
        printf("%s\n",rfid);
    }
    if(x> 0 && x< 110 && y>190 && y<260)//注册
    {
         // 遍历数组看看当前的卡号是否已经注册
            int i = 0;
            for (i = 0; i < card_num; i++)
            {
                if (strcmp(arry[i].rfid, rfid) == 0) // 卡号存在
                {
                     show_text(10, 100, 410, 60,"卡号已经存在,注册失败");
                    break;
                }
            }

            // 当前卡号不存在，注册成功
            if (strlen(rfid) > 0 && i == card_num)
            {
                strcpy(arry[card_num].rfid, rfid);
                arry[card_num].money = 0;
                card_num++;
                show_text(10, 100, 410, 60, "注册成功0元");
            }
        //show_text(10, 100, 410, 60, "注册成功");
    }
    if(x> 145 && x< 235 && y>190 && y<260)//充值
    {
            int i = 0;      
            for (i = 0; i < card_num; i++)// 遍历数组看看当前的卡号是否已经存在
            {
                if (strcmp(arry[i].rfid, rfid) == 0) // 卡号存在
                {
                    int add =0;//充值金额
                    int sum =0;
                    char temp[10]; // 用于转换成字符串的数组
                    // add=(int)(keyboard_figure[0]-'0')*10+(int)(keyboard_figure[1]-'0');//将键盘上获取的俩数字（充值金额）转换成整数

                    int arr_size = sizeof(keyboard_figure) / sizeof(keyboard_figure[0]);

                    for (int j = 0; j < cnt; j++)
                    {
                        
                        
                            sum = sum * 10 + keyboard_figure[j];
                        
                    }

                    add += sum;
                    if(add>0 && add<10000)
                    {
                        arry[i].money+=add;
                        sprintf(temp,"%d",add);// 将整数转换成字符串（将充值金额转换为字符串输出）
                    
                        memset(keyboard_figure, 0, 8 * sizeof(int));// 清空数组

                        show_text(10, 100, 410, 60,strcat(temp,"元充值成功"));
                        printf("充值成功，总金额：%d\n",arry[card_num].money);
                        show_text(450, 85, 350, 50, " "); //清空键盘输入
                        cnt = 0;
                        break;
                    }
                    else
                    {
                         show_text(10, 100, 410, 60,"请输入充值金额");
                         money_add_flag = 1;
                         break;

                    }
                    
                    
                }
            }

            // 当前卡号不存在
            if (strlen(rfid) > 0 && i == card_num)
            {
                show_text(10, 100, 410, 60, "卡号不存在");
            }
            
           
        
    }
    if(x> 285 && x< 400 && y>190 && y<260)//查询
    {
         // 遍历数组看看当前的卡号是否存在，存在则显示余额
            int i = 0;
            char temp[30];// 用于转换成字符串的数组
            for (i = 0; i < card_num; i++)
            {
                if (strcmp(arry[i].rfid, rfid) == 0) // 卡号存在
                {
                    sprintf(temp,"%d",arry[i].money);
                    show_text(10, 100, 410, 60,strcat(temp,"元"));
                    break;
                }
            }

            // 当前卡号不存在
            if (strlen(rfid) > 0 && i == card_num)
            {
                show_text(10, 100, 410, 60, "卡号不存在");
            }
    }
    if(x> 70 && x< 170 && y>280 && y<345)//消费
    {
        int i = 0;
        for (i = 0; i < card_num; i++)// 遍历数组看看当前的卡号是否已经存在
        {
            if (strcmp(arry[i].rfid, rfid) == 0) // 卡号存在
            {
                 int sub =0;//消费金额
                int sum =0;
                char temp[10];// 用于转换成字符串的数组
                // sub=(int)(keyboard_figure[0]-'0')*10+(int)(keyboard_figure[1]-'0');//将键盘上获取的俩数字（消费金额）转换成整数
                 int arr_size = sizeof(keyboard_figure) / sizeof(keyboard_figure[0]);

                    for (int j = 0; j < cnt; j++)
                    {
                       
                        
                            sum = sum * 10 + keyboard_figure[j];
                        
                        
                    }

                    sub += sum;
                if(sub>0 && sub<=arry[i].money)
                {
                    arry[i].money-=sub;
                    sprintf(temp,"%d",sub);// 将整数转换成字符串（将消费金额转换为字符串输出）
                    // 清空数组
                    memset(keyboard_figure, 0, 8 * sizeof(char));

                    show_text(10, 100, 410, 60,strcat(temp,"元消费成功"));
                    printf("消费成功，总金额：%d\n",arry[card_num].money);
                    show_text(450, 85, 350, 50, " ");// 清空键盘输入
                    cnt = 0;
                    break;
                }
                else
                {
                    show_text(10, 100, 410, 60,"请输入消费金额");
                    show_text(450, 85, 350, 50, " "); //清空键盘输入
                    cnt = 0;
                    money_sub_flag = 1;
                    break;

                }
            }
        }

        // 当前卡号不存在
        if (strlen(rfid) > 0 && i == card_num)
        {
            show_text(10, 100, 410, 60, "卡号不存在");
        }


        
    }
    if(x> 220 && x< 325 && y>275 && y<345)//注销
    {
        int i = 0;
        int logout_flag = 0;
        for (i = 0; i < card_num; i++)// 遍历数组看看当前的卡号是否已经存在
        {
            if (strcmp(arry[i].rfid, rfid) == 0) // 卡号存在
            {
                arry[i].money = 0;
                memset(arry[i].rfid, 0, sizeof(arry[i].rfid));
                arry[i]=arry[i+1];
                card_num--;
                show_text(10, 100, 410, 60,"注销成功");
                logout_flag = 1;
                break;
            }
        }

        // 当前卡号不存在，注销失败
        if (strlen(rfid) > 0 && i == card_num && logout_flag == 0)
        {
            show_text(10, 100, 410, 60, "卡号不存在");
        }
    }


}

void *task(void *arg) // 任务函数，必须这样定义！！
{
    
    while (1)
    {
       
        printf("读卡\n");
        read_card();
        sleep(1);
        show_text(10, 400, 285, 50, rfid);//rfid设为全局变量
        printf("%s\n",rfid);
        
    }
}


int main()
{
    // 初始化LCD设备
    mmap_lcd();

    // 初始化字库
    init_font(48);
    //显示一张图片
    bmp0487();

    // 创建一个线程执行一个循环任务
    pthread_t tid;
    pthread_create(&tid, NULL, task, NULL); // 线程创建后，就会执行

    while(1)
    {
        out_put();
        
    }


    return 0;
}
