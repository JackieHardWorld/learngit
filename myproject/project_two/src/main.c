#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <framebuffer.h>
#include <touchscreen.h>
#include <font.h>

#include "patient.h"
#include "doctor.h"
#include "music.h"
#include "list.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#include <sys/time.h>
#include <time.h>
// 线程的头文件
#include <pthread.h>
#include "musicplayer.h"
#include "photoalbum.h"
#include "videoplayer.h"




unsigned int *fb_addr = NULL;// 全局变量，所有函数都可以操作LCD映射地址
font *f = NULL;// 全局变量，所有函数都可以操作字库
class_manage_t *manager = NULL;// 全局变量，所有函数都可以操作链表
// int play_music_flag = 0;//播放音乐标志位




class_manage_t* Init_class_manage(void)
{
    class_manage_t *manager;
    manager = malloc(sizeof(class_manage_t));
    if(manager == NULL)
    {
        perror("malloc error");
        return NULL;
    }
    manager->doctor_list = Init_doctor_node(NULL);
    manager->patient_list = Init_patient_node(NULL);
    manager->music_list = Init_music_node(NULL);
    manager->patient_cur_numb = 0;
    manager->doctor_cur_numb = 0;
    manager->music_cur_numb = 0;
    return manager;

}

void destroy_class_manage(class_manage_t *manager)
{

    destroy_patient_list(manager->patient_list);
    destroy_doctor_list(manager->doctor_list);
    destroy_music_list(manager->music_list);
     free(manager); // 释放链表头节点内存

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



int show_text(int x, int y, int w, int h, char *text)
{
    // 1. 创建并初始化一块画板
    bitmap *bm = createBitmapWithInit(w, h, 4, 0xffffffff);  // 创建并初始化为白色背景
    if (bm == NULL)
    {
        printf("create bitmap fail\n");
        return -1;
    }
    printf("create bitmap success\n");

    // 2. 把文字输出到画板中
    fontPrint(f, bm, 0, 0, text, getColor(178, 231, 248, 00), 00);
    printf("text printed on bitmap\n");

    // 3. 把画板输出到LCD映射地址上
    show_font_to_lcd(fb_addr, x, y, bm);
    printf("show font success\n");

    // 4. 销毁画板，释放内存
    destroyBitmap(bm);

    return 0;
}


void load_patient_to_screen(void)
{
    struct list_head *pos;
    patient_node_t *bpos;
    int i = 0;
    int j = 0;
    list_for_each(pos, &manager->patient_list->list) // 遍历链表
    {
        i++;
        bpos = list_entry(pos, patient_node_t, list); // 获取节点数据
        printf("姓名：%s 性别：%s 年龄：%d\n", bpos->name, bpos->gender, bpos->age);
        if(i == 1)
        {
            show_text(350, 120, 180, 50,(char*)bpos->name);
        }
        
        else if(i == 2)
        {
            show_text(350,220, 180, 50,(char*)bpos->name);
        }
        else if(i <= 8)
        {
            show_text(630,160+j, 100, 35,(char*)bpos->name);
            j = j + 48;
        }

    }
}


void unload_patient_to_screen(FILE *fp)
{
    struct list_head *pos;
    patient_node_t *bpos;
    pos = (&manager->patient_list->list)->next;
    bpos = list_entry(pos, patient_node_t, list);
    if(pos == (&manager->patient_list->list))
    {
        printf("删除失败\n");
        return;
    }
    printf("删除姓名：%s 性别：%s 年龄：%d\n", bpos->name, bpos->gender, bpos->age);
    list_del(&bpos->list);
    free(bpos);
    manager->patient_cur_numb--;
    printf("删除成功\n");

    // 重新写入整个链表到文件
    freopen("patient_info.txt", "w+", fp); // 清空文件并重新打开
    file_write_patient_info(fp, manager);
    fflush(fp);

    int i = 0;
    int j = 0;
    list_for_each(pos, &manager->patient_list->list) // 遍历链表
    {
        i++;
        bpos = list_entry(pos, patient_node_t, list); // 获取节点数据
        printf("姓名：%s 性别：%s 年龄：%d\n", bpos->name, bpos->gender, bpos->age);
        if(i == 1)
        {
            show_text(350, 120, 180, 50,(char*)bpos->name);
        }
        
        else if(i == 2)
        {
            show_text(350,220, 180, 50,(char*)bpos->name);
        }
        else if(i <= 8)
        {
            show_text(630,160+j, 100, 35,(char*)bpos->name);
            j = j + 48;
        }

    }
    if(i == 1)
    {
        show_text(350,220, 180, 50,"  ");
    }
    if(i < 8)
    {
        show_text(630,160+j, 100, 35,"  ");
    }

    
}




void *my_time(void *arg) // 任务函数，必须这样定义！！
{
    struct timeval tv;
    struct tm *tm;
    char buffer[30];
    bitmap bm;
	bm.height = 480;
	bm.width = 800;
	bm.byteperpixel = 4;
	bm.map = (void *)fb_addr;//指定画板是显存地址
    
    while (1)
    {
        // 获取当前时间
        gettimeofday(&tv, NULL);

        // 将秒数转换为本地时间
        tm = localtime(&tv.tv_sec);

        // 格式化时间并输出
        strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", tm);
        printf("Current time: %s.%06ld\n", buffer, tv.tv_usec);
        show_text(515, 10, 280, 50,buffer);
        usleep(1000000);
        // //将字体写到点阵图上
	    // fontPrint(f,&bm, 515, 10,buffer,getColor(0,0,0xff,0),0);//fontPrint(字体资源， 显存资源， x坐标，y坐标，显示的字符串, 字体颜色, 默认写0);

        
    }
}


 void desktop(pthread_t *tid,int ts_fd)
 {   
    int x,y; //触摸屏坐标
    int status;//触摸屏状态标志位
    // 停止时间线程
    pthread_cancel(*tid);
    display_jpeg_picture_to_framebuffer("../jpg/desk.jpg", 0, 0, fb_addr, 1); //进入娱乐界面
    while(1)
    {
        status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态
        if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
        {
            break;//退出娱乐界面
        }
        if(x >= 100 && x <= 260 && y >= 100 && y <= 300 && status == 1)
        {
            music_player(ts_fd);//播放音乐
           
        }
        if(x >= 325 && x <= 500 && y >= 100 && y <= 300 && status == 1)
        {
           doctor_video(ts_fd);//播放视频
        }
        if(x >= 550 && x <= 710 && y >= 100 && y <= 300 && status == 1)
        {
            photo_album(ts_fd);//进入相册
        }
    }
    display_jpeg_picture_to_framebuffer("../jpg/doctor1.jpg", 0, 0, fb_addr, 1); //重新加载医生界面
    load_patient_to_screen(); //加载患者信息到屏幕
    pthread_create(tid, NULL, my_time, NULL); //重新启动时间线程

 }


int main(void)
{
   
    int fb_fd;//显存设备文件描述符
    int ts_fd;//触摸屏设备文件描述符
    ts_fd  = init_touchscreen_device();
    fb_addr = init_framebuffer_device(&fb_fd);//初始化显存设备
    int status; //触摸屏状态标志位
    int silde_status = 0;//滑动状态标志位
    int x, y; //触摸屏坐标信息
    init_font(40);//初始化字体大小
    manager = Init_class_manage();//初始化
	

    display_jpeg_picture_to_framebuffer("../jpg/doctor1.jpg", 0, 0, fb_addr, 1);//刷界面图片

    int cmd;
    FILE *fp = fopen("patient_info.txt", "r+");//打开患者文件
    if(fp == NULL)
    {
        fprintf(stderr,"打开文件失败:%s\n", strerror(errno));
    }
    FILE *fp2 = fopen("doctor_info.txt", "r+");//打开医生文件
    if(fp2 == NULL)
    {
        fprintf(stderr,"打开文件失败:%s\n", strerror(errno));
    }
    FILE *fp3 = fopen("music_info.txt", "r+");//打开音乐文件
    if(fp3 == NULL)
    {
        fprintf(stderr,"打开文件失败:%s\n", strerror(errno));
    }
    
    file_load_patient_info(fp,manager); //导入患者文件信息
    file_load_doctor_info(fp2,manager); //导入医生文件信息
    file_load_music_info(fp3,manager); //导入音乐文件信息
    load_patient_to_screen(); //加载患者信息到屏幕
    char *doctor_jpeg[5] = {"../jpg/doctor1.jpg", "../jpg/doctor2.jpg", "../jpg/doctor3.jpg", "../jpg/doctor4.jpg", "../jpg/doctor5.jpg"};//医生头像数组
    char dotctor_jpeg_index = 0;//医生头像数组下标

    // 创建一个线程执行一个循环任务
    pthread_t tid;
    pthread_create(&tid, NULL, my_time, NULL); //创建时间线程


    while(1)
    {
        printf("-------------------------------------------------------------------------------------------\n");

        char *patient_name = read_patient_head_node(manager);//读取患者头节点信息
        if(patient_name == NULL)
        {
           show_text(350, 120, 180, 50,"暂时没有患者");
            printf("没有患者信息\n");
        }
        
        status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态
        silde_status = silde_state_function(status, &x, &y);//滑动状态函数
        if(x>=0 && x>=100 && y>=90 && y<=135)
        {
            // display_jpeg_picture_to_framebuffer("1.jpg", 0, 0, fb_addr, 1);
            // insert_patient_node(manager, fp);//挂号
        }
        if(x>=0 && x<=100 && y>=145 && y<=190 &&status == TOUCH_RELEASE_STATUS)
        {
            //切换医生
            display_jpeg_picture_to_framebuffer(doctor_jpeg[dotctor_jpeg_index], 0, 0, fb_addr, 1);
            load_patient_to_screen(); //加载患者信息到屏幕
            dotctor_jpeg_index++;
            dotctor_jpeg_index %= 5;
            printf("切换医生\n");

        }
        if(x>=450 && x<=550 && y>=100 && y<=180 &&status == TOUCH_RELEASE_STATUS)
        {
            unload_patient_to_screen(fp); //去掉患者信息
            
        }
        if(x>=0 && x<=100 && y>=195 && y<=240 &&status == TOUCH_RELEASE_STATUS)
        {
            desktop(&tid,ts_fd);
            printf("进入桌面\n");
        }
        
    }

    //关闭字体，关闭画板
	fontUnload(f);

    printf("finish %p\n", fb_addr);
    destroy_touchscreen_device(ts_fd);
    destroy_framebuffer_device(fb_fd, fb_addr);//销毁显存设备

}











