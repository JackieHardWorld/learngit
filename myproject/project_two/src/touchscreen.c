#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "touchscreen.h"

int init_touchscreen_device(void)
{
    int ts_fd;

    ts_fd = open("/dev/input/event0", O_RDONLY);
    if(ts_fd == -1)
    {
        perror("打开触摸屏失败");
        return -1;
    }

    return ts_fd;
}

int destroy_touchscreen_device(int ts_fd)
{
    return close(ts_fd);
}

#define TOUCH_COORDINATE_STATUS     0 //坐标
#define TOUCH_PRESS_STATUS          1 //按下
#define TOUCH_RELEASE_STATUS        2 //释放
#define TOUCH_ERROR_STATUS          -1 //错误

int up_slide_flag = 0;
int down_slide_flag = 0;
int left_slide_flag = 0;
int right_slide_flag = 0;


int get_touchscreen_status(int ts_fd, int *x_addr, int *y_addr)
{
    
    struct input_event ts_data;
    ssize_t rd_size;
    static int x, y;
    int status;
    
    while(1)
    {
    
        rd_size = read(ts_fd, &ts_data, sizeof(ts_data));//阻塞功能
        if(rd_size == -1)
        {
            perror("获取触摸屏数据失败");
            status = TOUCH_ERROR_STATUS;
            break;
        }
        
        if(ts_data.type == EV_ABS)//判断他是否是坐标事件
        {
            if(ts_data.code == ABS_X)//判断数据是否是x坐标
            {
                x = ts_data.value;
            }
            else if(ts_data.code == ABS_Y)//判断数据是否是y坐标
            {
                y =  ts_data.value;
            }
            status = TOUCH_COORDINATE_STATUS;
        }
        else if(ts_data.type == EV_KEY)//按键事件
        {
            if(ts_data.value == 1)
            {
                status = TOUCH_PRESS_STATUS;
            }
            else
            {
                status = TOUCH_RELEASE_STATUS;
            }
        }
        else//结束包
        {
            *x_addr = x*800/1024.0;
            *y_addr = y*480/600.0;
            //printf("获取结束\n");
            break;
        }

    }

    return status;
}

int silde_state_function(int status, int *x, int *y)
{
   static int start_x, start_y;
    switch(status)
    {
        case TOUCH_COORDINATE_STATUS:
            //printf("x=%d, y=%d\n", x, y);
            break;
        case TOUCH_PRESS_STATUS:
            printf("触摸屏被按下x=%d, y=%d\n", *x, *y);
            start_x = *x;
            start_y = *y;
            break;
        case TOUCH_RELEASE_STATUS:
            printf("触摸屏被松开x=%d, y=%d\n", *x, *y);

            if(abs((*x)-start_x) > abs((*y)-start_y))
            {
                if((*x)-start_x > 20)
                {
                    right_slide_flag =1; //右滑动
                    printf("右滑动\n");
                }
                else if(start_x-(*x)>20)
                {
                    left_slide_flag =1; //左滑动
                    printf("左滑动\n");
                }
            }
            else
            {
                if((*y)-start_y > 20)
                {

                    down_slide_flag =1; //下滑动
                    printf("下滑动\n");
                }
                else if(start_y-(*y)>20)
                {

                    up_slide_flag =1; //上滑动
                    printf("上滑动\n");
                }
            }

            break;
        default:
            fprintf(stderr, "触摸屏bbq啦\n");
            break;
    }


    if(up_slide_flag == 1)
    {
        printf("^^^^^^^^^^^^^^\n");
        up_slide_flag = 0;
        return 1;
    }
    else if(down_slide_flag == 1)
    {
        printf("VVVVVVVVVVVVVVV\n");
        down_slide_flag = 0;
        return 2;
    }
    else if(left_slide_flag == 1)
    {
        printf("<<<<<<<<<<<<<\n");
        left_slide_flag = 0;
        return 3;
    }
    else if(right_slide_flag == 1)
    {
        right_slide_flag = 0;
        printf(">>>>>>>>>>>>>\n");
        return 4;
    }
    else
    {
        return 0;
    }
}

