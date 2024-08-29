#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <framebuffer.h>
#include "list.h"
#include "class_head.h"
#include "musicplayer.h"



int play_music_flag = 0;//播放音乐标志位

void load_music_to_screen(void)
{
    char song_name[100] = "madplay ";//名称
    system("killall -SIGTERM madplay");  // 停止当前音乐
    bzero(song_name, 100);
    struct list_head *pos;
    music_node_t *bpos;
    int i = 0;
    list_for_each(pos, &manager->music_list->list) // 遍历链表
    {
        i++;
        bpos = list_entry(pos, music_node_t, list); // 获取节点数据
        printf("歌名：%s 歌手：%s 照片：%s\n", bpos->name, bpos->singer, bpos->photo);
        if(play_music_flag == i)
        {
            show_text(270, 120, 80, 50,"歌名：");
            show_text(270, 170, 180, 50,"歌手：");
            show_text(350, 120, 180, 50,(char*)bpos->name);
            show_text(350, 170, 180, 50,(char*)bpos->singer);
            strcat(song_name, "madplay ../music/");
            strcat(song_name, (char*)bpos->name);
            strcat(song_name, ".mp3 &");
            system(song_name);// 播放上一首歌曲
        }
        
    }
}

void music_player(int ts_fd)
{
    printf("99999/n");
    int x,y; //触摸屏坐标
    int status;//触摸屏状态标志位
    int silde_status;//滑动状态标志位
    int stop_continue_flag = 0;//停止/继续标志位
    display_jpeg_picture_to_framebuffer("../jpg/lake.jpg", 0, 0, fb_addr, 1); //进入音乐界面
    display_jpeg_picture_to_framebuffer("../jpg/bar5.jpg", 0, 400, fb_addr, 1); //显示背景图片
    system("madplay ../music/喜帖街.mp3 &");
    while(1)
    {
        status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态（阻塞，等待触屏）
        if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
        {
            system("killall -SIGTERM madplay"); //关闭所有madplay进程(首选)
            break;  //退回上个界面
        }
        if(x >= 360 && x <= 440 && y >= 400 && y <= 480 && status == 1)//播放和停止按钮
        {
            
            stop_continue_flag %= 2;
            system("killall -SIGSTOP madplay");
            if(stop_continue_flag == 1)
            {
                    system("killall -SIGCONT madplay");
            }
            stop_continue_flag++;
        }
        if (x >= 220 && x <= 340 && y >= 400 && y <= 480 && status == 1) // 上一首歌曲
        {
            play_music_flag--;
            if(play_music_flag < 1)
            {
                play_music_flag = manager->music_cur_numb;
            }
            load_music_to_screen();
    
        }

        if (x >= 500 && x <= 610 && y >= 400 && y <= 480 && status == 1) // 下一首歌曲
        {
            play_music_flag++;
            if(play_music_flag > manager->music_cur_numb)
            {
                play_music_flag = 1;
            }
            load_music_to_screen();

        }
    } 
    display_jpeg_picture_to_framebuffer("../jpg/desk.jpg", 0, 0, fb_addr, 1); //进入娱乐界面    
}