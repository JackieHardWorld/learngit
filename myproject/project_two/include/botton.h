#ifndef _BOTTON_H
#define _BOTTON_H

#include <stdio.h>
#include <stdlib.h>
#include <framebuffer.h>
#include <touchscreen.h>
#define BUTTON_NUMB 4

//按键控件
struct button{
    int x;
    int y;
    int width;
    int hight;
    unsigned int color;//没有点击的颜色
    unsigned int press_color;//点击的颜色
    void *(*handler)(void *);//按键的响应函数指针
};

//图像控件
struct image_view{
    int x;
    int y;
    int width;
    int hight;
    unsigned int color;
    
};

//定义了一个临时用来传递参数的结构体
struct pass_arg{
    struct image_view *vw;//用来记录image_view的内存地址
    unsigned int *fb_addr;//显存地址
};

void display_image_view(struct image_view *vw, unsigned int *fb_addr);

void *bt0_handler_function(void *arg);

void *bt1_handler_function(void *arg);

void *bt2_handler_function(void *arg);
void *bt3_handler_function(void *arg);

void *bt4_handler_function(void *arg);
struct button *request_button(int x, int y, int width, int hight, unsigned int color, unsigned int pcolor, void *(handler)(void *));
void destroy_button(struct button *bt);


struct image_view *request_image_view(int x, int y, int width, int hight, unsigned int color);

void display_button(struct button *bt, unsigned int *fb_addr);

void display_press_button(struct button *bt, unsigned int *fb_addr);



#endif