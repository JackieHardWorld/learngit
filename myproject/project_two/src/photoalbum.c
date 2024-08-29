#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <framebuffer.h>
#include "list.h"
#include "class_head.h"
#include "photoalbum.h"


void photo_album(int ts_fd)
{
    int x,y; //触摸屏坐标
    int status;//触摸屏状态标志位
    int silde_status;//滑动状态标志位
    int i = 0;//幻灯片图片索引
    char *photo_jpeg[5] = {"../jpg/doctor1.jpg", "../jpg/doctor2.jpg", "../jpg/doctor3.jpg",
                             "../jpg/doctor4.jpg", "../jpg/doctor5.jpg"};//医生头像数组
    display_jpeg_picture_to_framebuffer("../jpg/video.jpg", 0, 0, fb_addr, 1); //进入相册初始页面    
    while(1)
    {
        status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态（阻塞，等待触屏）
        silde_status = silde_state_function(status, &x, &y);//滑动状态函数
        if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
        {
            break;  //退回上个界面
        }
      
        //左半屏滑动（切换图片）
        if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status ==3)
        {
            i--;
            if(i < 0)
            {
                i = 4;
            }
            display_jpeg_picture_to_framebuffer(photo_jpeg[i], 0, 0, fb_addr, 1); //上一张
            continue;
        }
        //右半屏滑动（切换图片）
        if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 4)
        {
            i++;
            i %= 5;
            display_jpeg_picture_to_framebuffer(photo_jpeg[i], 0, 0, fb_addr, 1); //下一张    
            continue;
        }

        // //左半屏上滑（幻灯片）
        // if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 1)
        // {
        //     while(1)
        //     {
        //         status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态（阻塞，等待触屏）
        //         i--;
        //         if(i < 0)
        //         {
        //             i = 4;
        //         }
        //         usleep(500000);
        //         display_jpeg_picture_to_framebuffer(photo_jpeg[i], 0, 0, fb_addr, 1); //上一张
        //         if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
        //         {
        //             break;
        //         }
        //     }
        // }
        // //右半屏上滑（幻灯片）
        // if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 2)
        // {
        //     while(1)
        //     {
        //         status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态（阻塞，等待触屏）
        //         i++;
        //         i %= 5;
        //         usleep(500000);
        //         display_jpeg_picture_to_framebuffer(photo_jpeg[i], 0, 0, fb_addr, 1); //下一张
        //         if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
        //         {
        //             break;
        //         }
        //     }
        // }
    }
    display_jpeg_picture_to_framebuffer("../jpg/desk.jpg", 0, 0, fb_addr, 1); //进入娱乐界面    
}
