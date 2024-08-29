#include <stdio.h>
#include <stdlib.h>
#include <framebuffer.h>
#include <touchscreen.h>
#include "botton.h"



void display_image_view(struct image_view *vw, unsigned int *fb_addr)
{
    int x, y;

    //显示一个图形颜色
    for(y=vw->y; y<vw->y+vw->hight; y++)
    {
        for(x=vw->x; x<vw->x+vw->width; x++)
        {
            display_point_to_framebuffer(x, y, vw->color, fb_addr);
        }
    }

}

void *bt0_handler_function(void *arg)
{
    struct pass_arg rarg = *(struct pass_arg *)arg;

    rarg.vw->color = 0x00ff0000;

    display_image_view(rarg.vw, rarg.fb_addr);

    printf("hello 红色\n");

    return NULL;
}

void *bt1_handler_function(void *arg)
{
    struct pass_arg rarg = *(struct pass_arg *)arg;

    rarg.vw->color = 0x0000ff00;

    display_image_view(rarg.vw, rarg.fb_addr);

    printf("hello 绿色\n");

    return NULL;
}

void *bt2_handler_function(void *arg)
{
    struct pass_arg rarg = *(struct pass_arg *)arg;

    rarg.vw->color = 0x000000ff;

    display_image_view(rarg.vw, rarg.fb_addr);

    printf("hello 蓝色\n");

    return NULL;
}

void *bt3_handler_function(void *arg)
{
    struct pass_arg rarg = *(struct pass_arg *)arg;

    rarg.vw->color = 0x00ffff00;

    display_image_view(rarg.vw, rarg.fb_addr);

    printf("hello 黄色\n");

    return NULL;
}

void *bt4_handler_function(void *arg)
{
    printf("hello 黑色\n");

    return NULL;
}

struct button *request_button(int x, int y, int width, int hight, unsigned int color, unsigned int pcolor, void *(handler)(void *))
{
    struct button *bt;
    bt = calloc(1, sizeof(struct button));//申请1个按键结构体内存

     //按键初始化
    bt[0].x = x;
    bt[0].y = y;
    bt[0].width = width;
    bt[0].hight = hight;
    bt[0].color = color;//记录按下的颜色
    bt[0].press_color = pcolor;//记录松开的颜色
    bt[0].handler = handler;//记录以后按倒这个按键之后做什么动作

    return bt;
}

void destroy_button(struct button *bt)
{
    free(bt);
}


struct image_view *request_image_view(int x, int y, int width, int hight, unsigned int color)
{
    struct image_view *vw;
    vw = calloc(1, sizeof(struct image_view));//申请1个图像结构体内存

    vw->x = x;
    vw->y = y;
    vw->width = width;
    vw->hight = hight;
    vw->color = color;

    return vw;
}

void display_button(struct button *bt, unsigned int *fb_addr)
{
    int x, y;

    //显示一个按键
    for(y=bt->y; y<bt->y+bt->hight; y++)
    {
        for(x=bt->x; x<bt->x+bt->width; x++)
        {
            display_point_to_framebuffer(x, y, bt->color, fb_addr);
        }
    }
}

void display_press_button(struct button *bt, unsigned int *fb_addr)
{
    int x, y;

    //显示一个按键
    for(y=bt->y; y<bt->y+bt->hight; y++)
    {
        for(x=bt->x; x<bt->x+bt->width; x++)
        {
            display_point_to_framebuffer(x, y, bt->press_color, fb_addr);
        }
    }
}




// int main(void)
// {
//     int fb_fd;
//     int ts_fd;
//     int status;
// 	int x, y;
//     int ts_x, ts_y;
//     int press_x, press_y;
//     int release_x, release_y;
// 	unsigned int *fb_addr;

//     fb_addr = init_framebuffer_device(&fb_fd);//初始化显存设备

//     ts_fd = init_touchscreen_device();//初始化触摸屏设备

//     struct button *bt[BUTTON_NUMB];


//     //申请4个按键
//     bt[0] = request_button(50, 50, 100, 50, 0x00ff0000, 0x00ffffff, bt0_handler_function);
//     bt[1] = request_button(650, 50, 100, 50, 0x0000ff00, 0x00ffffff, bt1_handler_function);
//     bt[2] = request_button(50, 380, 100, 50, 0x000000ff, 0x00ffffff, bt2_handler_function);
//     bt[3] = request_button(650, 380, 100, 50, 0x00ffff00, 0x00ffffff, bt3_handler_function);
//     //bt[4] = request_button(350, 210, 100, 50, 0x00000000, 0x00ffffff, bt4_handler_function);

//     struct image_view *vw;

//     //申请一个纯画面
//     vw = request_image_view(300, 185, 200, 100, 0x00ffffff);

//     //初始化界面
//     int i;

//     //循环显示BUTTON_NUMB个按键
//     for(i=0; i<BUTTON_NUMB; i++);
//     {
//         display_button(bt[i], fb_addr);//显示按键
//     }

//     //显示image_view
//     display_image_view(vw, fb_addr);
    
//     //监控触摸屏
//     while(1)
//     {
//         status = get_touchscreen_status(ts_fd, &ts_x, &ts_y);
//         switch(status)
//         {
//             case TOUCH_COORDINATE_STATUS:
//                 //printf("x=%d, y=%d\n", x, y);
//                 break;
//             case TOUCH_PRESS_STATUS:
//                 //printf("触摸屏被按下x=%d, y=%d\n", x, y);
//                 //记录按下的触摸屏坐标
//                 press_x = ts_x;
//                 press_y = ts_y;

//                 for(i=0; i<BUTTON_NUMB; i++)
//                 {
//                     //判断是否按倒了按键范围内
//                     if(press_x>bt[i]->x && press_x < bt[i]->x+bt[i]->width && press_y>bt[i]->y && press_y < bt[i]->y+bt[i]->hight )//判断松开的坐标是否在范围内
//                     {
//                             //显示按下来按键内容
//                             display_press_button(bt[i], fb_addr);
//                     }

//                 }
//                 break;
//             case TOUCH_RELEASE_STATUS:
//                 //printf("触摸屏被松开x=%d, y=%d\n", ts_x, ts_x);
//                 release_x = ts_x;
//                 release_y = ts_y;

//                 //循环显示4个按键
//                 for(i=0; i<BUTTON_NUMB; i++)
//                 {

//                     if(press_x>bt[i]->x && press_x < bt[i]->x+bt[i]->width && press_y>bt[i]->y && press_y < bt[i]->y+bt[i]->hight )//判断松开的位置在是否在我们的按键范围内
//                     {
//                         display_button(bt[i], fb_addr);//显示按键

//                         //判断松开来的位置是否在按键范围内容
//                         if(release_x>bt[i]->x && release_x < bt[i]->x+bt[i]->width && release_y>bt[i]->y && release_y < bt[i]->y+bt[i]->hight) //判断松开的坐标是否在范围内
//                         {
//                             struct pass_arg arg;//定义一个要传输给handler的参数

//                             arg.vw = vw;
//                             arg.fb_addr = fb_addr;

//                             bt[i]->handler(&arg);//执行响应函数
//                         }
//                     }

//                 }
                
//                 break;
//             default:
//                 fprintf(stderr, "触摸屏bbq啦\n");
//                 break;
//         }


//     }

    

//     //销毁界面

//     //释放资源
//     for(i=0; i<4; i++)
//     {
//         destroy_button(bt[i]);
//     }

//     destroy_touchscreen_device(ts_fd);//销毁触摸屏设备

//     destroy_framebuffer_device(fb_fd, fb_addr);//销毁显存设备

//     return 0;
// }