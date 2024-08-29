#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <framebuffer.h>
#include "touchscreen.h"
#include "videoplayer.h"




// 在开发板中播放视频，可直接使用开发板中的mplayer进行播放，注意：开发板中没有mplayer，需要下载到/usr/bin
// 1、播放
// 播放视频，输入以下命令：
// mplayer  Faded3.avi  -x  800  -y  480  -Zoom  -geometry 0:0  -loop 0  -quiet  
// 说明：
// mplayer                                 播放视频命令
// Faded3.avi                              视频文件  
// -x  400  -y  240  -Zoom：               指定大小（-Zoom：允许视频缩放）
// -geometry 0:0：                         指定位置
// -loop 0：                               循环播放
// -quiet：                                不打印多余的信息
                                   
// 2、暂停
// killall -SIGSTOP mplayer
// 3、继续
// killall -SIGCONT mplayer
// 4、退出
// killall -SIGKILLmadplay
// 在代码中实现以上命令时，可调用system()函数完成，如：
// 播放：system("mplayer  Faded3.avi  -x  800  -y  480  -Zoom  -geometry 0:0  -loop 0  -quiet &");  //& 程序在后台执行
// 暂停：system("killall -SIGSTOP mplayer ")
// 继续：system("killall -SIGCONT mplayer ")

 void doctor_video(int ts_fd)//医生视频
 {

    int x,y; //触摸屏坐标
    int status;//触摸屏状态标志位
    int silde_status;//滑动状态标志位
    int stop_continue_flag = 0;//停止/继续标志位
    int play_flag = -1;//播放标志位


    FILE *mplayer_pipe1, *mplayer_pipe2, *mplayer_pipe3, *mplayer_pipe4;//mplayer管道


start:

    // 删除已有的命名管道文件
    system("rm -f /tmp/mplayer-control1 /tmp/mplayer-control2 /tmp/mplayer-control3 /tmp/mplayer-control4");

    // 创建新的命名管道
    system("mkfifo /tmp/mplayer-control1 /tmp/mplayer-control2 /tmp/mplayer-control3 /tmp/mplayer-control4");

    display_jpeg_picture_to_framebuffer("../jpg/video.jpg", 0, 0, fb_addr, 1); //显示背景图片
    system("mkfifo /tmp/mplayer-control1 /tmp/mplayer-control2 /tmp/mplayer-control3 /tmp/mplayer-control4");
    system("mplayer -slave -input file=/tmp/mplayer-control1 ../video/1111.avi -x 200 -y 140 -Zoom -geometry 0:0 -loop 0 -quiet &");
    system("mplayer -slave -input file=/tmp/mplayer-control2 ../video/2222.avi -x 200 -y 140 -Zoom -geometry 200:0 -loop 0 -quiet &");
    system("mplayer -slave -input file=/tmp/mplayer-control3 ../video/3333.avi -x 200 -y 140 -Zoom -geometry 0:140 -loop 0 -quiet &");
    system("mplayer -slave -input file=/tmp/mplayer-control4 ../video/4444.avi -x 200 -y 140 -Zoom -geometry 200:140 -loop 0 -quiet &");

    mplayer_pipe1 = fopen("/tmp/mplayer-control1", "w");
    if (mplayer_pipe1 == NULL) {
    perror("Failed to open /tmp/mplayer-control1");
    return;
}

    mplayer_pipe2 = fopen("/tmp/mplayer-control2", "w");
    if (mplayer_pipe2 == NULL) {
    perror("Failed to open /tmp/mplayer-control1");
    return;
}

    mplayer_pipe3 = fopen("/tmp/mplayer-control3", "w");
    if (mplayer_pipe3 == NULL) {
    perror("Failed to open /tmp/mplayer-control1");
    return;
}

    mplayer_pipe4 = fopen("/tmp/mplayer-control4", "w");
    if (mplayer_pipe4 == NULL) {
    perror("Failed to open /tmp/mplayer-control1");
    return;
}


    while(1)
    {
        
        status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态（阻塞，等待触屏）


        if((x >= 0 && x <= 200 && y >= 0 && y <= 140 && status == 1) || play_flag == 0)
        {
            display_jpeg_picture_to_framebuffer("../jpg/bar5.jpg", 0, 400, fb_addr, 1); //显示背景图片
            play_flag++;
            system("killall -SIGTERM mplayer");
            system("mplayer -slave -input file=/tmp/mplayer-control1 ../video/1111.avi -x 800 -y 400 -Zoom -geometry 0:0 -loop 0 -quiet &");
            mplayer_pipe1 = fopen("/tmp/mplayer-control1", "w");
            // play_flag++;
            // system("killall -SIGTERM mplayer");
            // system("mplayer  111.avi  -x  800  -y  480  -Zoom  -geometry 0:0  -loop 0  -quiet  -osdlevel 3  &");  //& 程序在后台执行
            while(1)
            {   status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态
                silde_status = silde_state_function(status, &x, &y);//滑动状态函数

                printf("silde_status = %d\n", silde_status);
                if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
                {
                    system("killall -SIGTERM mplayer"); //关闭所有mplayer进程(首选)
                    play_flag = -1;
                    break;  //退回上个界面（借助goto）
                }
                if(x >= 360 && x <= 440 && y >= 400 && y <= 480 && status == 1)//暂停和播放
                {
                    
                    stop_continue_flag %= 2;
                    system("killall -SIGSTOP mplayer ");
                    if(stop_continue_flag == 1)
                    {
                         system("killall -SIGCONT mplayer ");
                    }
                    stop_continue_flag++;
                }
                if(x >= 220 && x <= 340 && y >= 400 && y <= 480 && status == 1)//上一个视频
                {
                    play_flag =3;
                    break;
                }
                if(x >= 500 && x <= 610 && y >= 400 && y <= 480 && status == 1)//下一个视频
                {
                    play_flag =1;
                    break;
                }
                 
                if(x >= 700 && x <= 800 && y >= 400 && y <= 480 && status == 1)// 快进
                {
                    fprintf(mplayer_pipe1, "seek 2 0\n");  // 快进2秒
                    fflush(mplayer_pipe1);
                    continue;
                }
                
                if(x >= 0 && x <= 100 && y >= 400 && y <= 480 && status == 1)// 后退
                {
                    fprintf(mplayer_pipe1, "seek -2 0\n");  // 后退2秒
                    fflush(mplayer_pipe1);
                    continue;
                }
                  //音量增加
                if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 1)
                {
                    printf("音量增加\n");
                    fprintf(mplayer_pipe1,"volume +20\n");
                    fflush(mplayer_pipe1);
                    continue;
                }
                
                //音量减少
                if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 2)
                {
                    printf("音量减少\n");
                    fprintf(mplayer_pipe1,"volume -20\n");
                    fflush(mplayer_pipe1);
                    continue;
                }
                //亮度增加
                if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 1)
                {
                    printf("亮度增加\n");
                    fprintf(mplayer_pipe1,"brightness +20 0\n");
                    fflush(mplayer_pipe1);
                    continue;
                }
                
                //亮度减少
                if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 2)
                {
                    printf("亮度减少\n");
                    fprintf(mplayer_pipe1,"brightness -20 0\n");
                    fflush(mplayer_pipe1);
                    continue;
                }
                //进度条左滑
                if(x >= 0 && x <= 800 && y >= 300 && y <= 400 && silde_status == 3)
                {
                    printf("进度条左滑\n");
                    int seek_time = (x * 6) / 800; 
                    fprintf(mplayer_pipe1,"seek -%d 0\n",seek_time);
                    fflush(mplayer_pipe1);
                }
                //进度条右滑                
                if(x >= 0 && x <= 800 && y >= 300 && y <= 400 && silde_status == 4)
                {
                    printf("进度条右滑\n");
                    int seek_time = (x * 6) / 800; 
                    fprintf(mplayer_pipe1,"seek %d 0\n",seek_time);
                    fflush(mplayer_pipe1);
                }

            }
            goto start;
            continue;
        }
        if((x >= 200 && x <= 400 && y >= 0 && y <= 140 && status == 1) || play_flag == 1)
        {
            display_jpeg_picture_to_framebuffer("../jpg/bar5.jpg", 0, 400, fb_addr, 1); //显示背景图片
            system("killall -SIGTERM mplayer");
            system("mplayer -slave -input file=/tmp/mplayer-control2 ../video/2222.avi -x 800 -y 400 -Zoom -geometry 0:0 -loop 0 -quiet &");
            mplayer_pipe2 = fopen("/tmp/mplayer-control2", "w");
            // system("killall -SIGTERM mplayer");
            // system("mplayer  222.avi  -x  800  -y  480  -Zoom  -geometry 0:0  -loop 0  -quiet   &");  //& 程序在后台执行
            while(1)
            {   status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态
                silde_status = silde_state_function(status, &x, &y);//滑动状态函数
                if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
                {
                    system("killall -SIGTERM mplayer"); //关闭所有mplayer进程(首选)
                    play_flag = -1;
                    break;
                }
                if(x >= 360 && x <= 440 && y >= 400 && y <= 480 && status == 1)
                {
                    
                    stop_continue_flag %= 2;
                    system("killall -SIGSTOP mplayer ");
                    if(stop_continue_flag == 1)
                    {
                         system("killall -SIGCONT mplayer ");
                    }
                    stop_continue_flag++;
                }
                if(x >= 220 && x <= 340 && y >= 400 && y <= 480 && status == 1)
                {
                    play_flag =0;
                    break;
                }
                if(x >= 500 && x <= 610 && y >= 400 && y <= 480 && status == 1)
                {
                    play_flag =2;
                    break;
                }
                // 快进
                if(x >= 700 && x <= 800 && y >= 400 && y <= 480 && status == 1)
                {
                    printf("快进2\n");
                    fprintf(mplayer_pipe2, "seek 2 0\n");  // 快进2秒
                    fflush(mplayer_pipe2);
                    printf("快进22222222222\n");
                    continue;
                }
                // 后退
                if(x >= 0 && x <= 100 && y >= 400 && y <= 480 && status == 1)
                {
                    printf("后退2\n");
                    fprintf(mplayer_pipe2, "seek -2 0\n");  // 后退2秒
                    fflush(mplayer_pipe2);
                    printf("后退22222222222\n");
                    continue;
                }
                //音量增加
                if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 1)
                {
                    printf("音量增加\n");
                    fprintf(mplayer_pipe2,"volume +200\n");
                    fflush(mplayer_pipe2);
                    continue;
                }
                
                //音量减少
                if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 2)
                {
                    printf("音量减少\n");
                    fprintf(mplayer_pipe2,"volume -200\n");
                    fflush(mplayer_pipe2);
                    continue;
                }
                //亮度增加
                if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 1)
                {
                    printf("亮度增加\n");
                    fprintf(mplayer_pipe2,"brightness +20 0\n");
                    fflush(mplayer_pipe2);
                    continue;
                }
                
                //亮度减少
                if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 2)
                {
                    printf("亮度减少\n");
                    fprintf(mplayer_pipe2,"brightness -20 0\n");
                    fflush(mplayer_pipe2);
                    continue;
                }
                //进度条左滑
                if(x >= 0 && x <= 800 && y >= 300 && y <= 400 && silde_status == 3)
                {
                    printf("进度条左滑\n");
                    int seek_time = (x * 6) / 800; 
                    fprintf(mplayer_pipe2,"seek -%d 0\n",seek_time);
                    fflush(mplayer_pipe2);
                }
                //进度条右滑                
                if(x >= 0 && x <= 800 && y >= 300 && y <= 400 && silde_status == 4)
                {
                    printf("进度条右滑\n");
                    int seek_time = (x * 6) / 800; 
                    fprintf(mplayer_pipe2,"seek %d 0\n",seek_time);
                    fflush(mplayer_pipe2);
                }
            }
            system("killall -SIGTERM mplayer");
            goto start;
            continue;
        }
        if((x >= 0 && x <= 200 && y >= 140 && y <= 280 && status == 1) || play_flag == 2)
        {
            display_jpeg_picture_to_framebuffer("../jpg/bar5.jpg", 0, 400, fb_addr, 1); //显示背景图片
            system("killall -SIGTERM mplayer");
            system("mplayer -slave -input file=/tmp/mplayer-control3 ../video/3333.avi -x 800 -y 400 -Zoom -geometry 0:0 -loop 0 -quiet &");
            mplayer_pipe3 = fopen("/tmp/mplayer-control3", "w");
            // system("killall -SIGTERM mplayer");
            // system("mplayer  3333.avi  -x  800  -y  480  -Zoom  -geometry 0:0  -loop 0  -quiet   &");  //& 程序在后台执行
            while(1)
            {   status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态
                silde_status = silde_state_function(status, &x, &y);//滑动状态函数
                if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
                {
                    system("killall -SIGTERM mplayer"); //关闭所有mplayer进程(首选)
                    play_flag = -1;
                    break;
                }
                if(x >= 360 && x <= 440 && y >= 400 && y <= 480 && status == 1)
                {
                    
                    stop_continue_flag %= 2;
                    system("killall -SIGSTOP mplayer ");
                    if(stop_continue_flag == 1)
                    {
                         system("killall -SIGCONT mplayer ");
                    }
                    stop_continue_flag++;
                }
                if(x >= 220 && x <= 340 && y >= 400 && y <= 480 && status == 1)
                {
                    play_flag =1;
                    break;
                }
                 if(x >= 500 && x <= 610 && y >= 400 && y <= 480 && status == 1)
                {
                    play_flag =3;
                    break;
                }
                // 快进
                if(x >= 700 && x <= 800 && y >= 400 && y <= 480 && status == 1)
                {
                    fprintf(mplayer_pipe3, "seek 2 0\n");  // 快进2秒
                    fflush(mplayer_pipe3);
                    continue;
                }
                // 后退
                if(x >= 0 && x <= 100 && y >= 400 && y <= 480 && status == 1)
                {
                    fprintf(mplayer_pipe3, "seek -2 0\n");  // 后退2秒
                    fflush(mplayer_pipe3);
                    continue;
                }
                //音量增加
                if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 1)
                {
                    printf("音量增加\n");
                    fprintf(mplayer_pipe3,"volume +200\n");
                    fflush(mplayer_pipe3);
                    continue;
                }
                
                //音量减少
                if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 2)
                {
                    printf("音量减少\n");
                    fprintf(mplayer_pipe3,"volume -200\n");
                    fflush(mplayer_pipe3);
                    continue;
                }
                //亮度增加
                if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 1)
                {
                    printf("亮度增加\n");
                    fprintf(mplayer_pipe3,"brightness +20 0\n");
                    fflush(mplayer_pipe3);
                    continue;
                }
                
                //亮度减少
                if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 2)
                {
                    printf("亮度减少\n");
                    fprintf(mplayer_pipe3,"brightness -20 0\n");
                    fflush(mplayer_pipe3);
                    continue;
                }
                //进度条左滑
                if(x >= 0 && x <= 800 && y >= 300 && y <= 400 && silde_status == 3)
                {
                    printf("进度条左滑\n");
                    int seek_time = (x * 6) / 800; 
                    fprintf(mplayer_pipe3,"seek -%d 0\n",seek_time);
                    fflush(mplayer_pipe3);
                }
                //进度条右滑                
                if(x >= 0 && x <= 800 && y >= 300 && y <= 400 && silde_status == 4)
                {
                    printf("进度条右滑\n");
                    int seek_time = (x * 6) / 800; 
                    fprintf(mplayer_pipe3,"seek %d 0\n",seek_time);
                    fflush(mplayer_pipe3);
                }

            }
            system("killall -SIGTERM mplayer");
            goto start;
            continue;
        }
        if((x >= 200 && x <= 400 && y >= 140 && y <= 280 && status == 1) || play_flag == 3)
        {
            display_jpeg_picture_to_framebuffer("../jpg/bar5.jpg", 0, 400, fb_addr, 1); //显示背景图片
            system("killall -SIGTERM mplayer");
            system("mplayer -slave -input file=/tmp/mplayer-control4 ../video/4444.avi -x 800 -y 400 -Zoom -geometry 0:0 -loop 0 -quiet &");
            mplayer_pipe4 = fopen("/tmp/mplayer-control4", "w");
            // system("killall -SIGTERM mplayer");
            // system("mplayer  4444.avi  -x  800  -y  480  -Zoom  -geometry 0:0  -loop 0  -quiet   &");  //& 程序在后台执行
            while(1)
            {   status = get_touchscreen_status(ts_fd, &x, &y);//获取触摸屏坐标和状态
                silde_status = silde_state_function(status, &x, &y);//滑动状态函数
                if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
                {
                    system("killall -SIGTERM mplayer"); //关闭所有mplayer进程(首选)
                    play_flag = -1;
                    break;
                }
                if(x >= 360 && x <= 440 && y >= 400 && y <= 480 && status == 1)
                {
                    
                    stop_continue_flag %= 2;
                    system("killall -SIGSTOP mplayer ");
                    if(stop_continue_flag == 1)
                    {
                         system("killall -SIGCONT mplayer ");
                    }
                    stop_continue_flag++;
                }
                if(x >= 220 && x <= 340 && y >= 400 && y <= 480 && status == 1)
                {
                    play_flag =2;
                    break;
                }
                if(x >= 500 && x <= 610 && y >= 400 && y <= 480 && status == 1)
                {
                    play_flag =0;
                    break;
                }
                // 快进
                if(x >= 700 && x <= 800 && y >= 400 && y <= 480 && status == 1)
                {
                    fprintf(mplayer_pipe4, "seek 2 0\n");  // 快进2秒
                    fflush(mplayer_pipe4);
                    continue;
                }
                // 后退
                if(x >= 0 && x <= 100 && y >= 400 && y <= 480 && status == 1)
                {
                    fprintf(mplayer_pipe4, "seek -2 0\n");  // 后退2秒
                    fflush(mplayer_pipe4);
                    continue;
                }
                 //音量增加
                if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 1)
                {
                    printf("音量增加\n");
                    fprintf(mplayer_pipe4,"volume +200\n");
                    fflush(mplayer_pipe4);
                    continue;
                }
                
                //音量减少
                if(x >= 0 && x <= 400 && y >= 0 && y <= 480 && silde_status == 2)
                {
                    printf("音量减少\n");
                    fprintf(mplayer_pipe4,"volume -200\n");
                    fflush(mplayer_pipe4);
                    continue;
                }
                //亮度增加
                if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 1)
                {
                    printf("亮度增加\n");
                    fprintf(mplayer_pipe4,"brightness +20 0\n");
                    fflush(mplayer_pipe4);
                    continue;
                }
                
                //亮度减少
                if(x >= 400 && x <= 800 && y >= 0 && y <= 480 && silde_status == 2)
                {
                    printf("亮度减少\n");
                    fprintf(mplayer_pipe4,"brightness -20 0\n");
                    fflush(mplayer_pipe4);
                    continue;
                }
                //进度条左滑
                if(x >= 0 && x <= 800 && y >= 300 && y <= 400 && silde_status == 3)
                {
                    printf("进度条左滑\n");
                    int seek_time = (x * 6) / 800; 
                    fprintf(mplayer_pipe4,"seek -%d 0\n",seek_time);
                    fflush(mplayer_pipe4);
                }
                //进度条右滑                
                if(x >= 0 && x <= 800 && y >= 300 && y <= 400 && silde_status == 4)
                {
                    printf("进度条右滑\n");
                    int seek_time = (x * 6) / 800; 
                    fprintf(mplayer_pipe4,"seek %d 0\n",seek_time);
                    fflush(mplayer_pipe4);
                }
                
            }
            system("killall -SIGTERM mplayer");
            goto start;
            continue;
        }
        if(x >= 700 && x <= 800 && y >= 0 && y <= 80 && status == 1)
        {
            system("killall -SIGTERM mplayer"); //关闭所有mplayer进程(首选)
        //    system("killall -SIGKILL madplay");//未能关闭所有mplayer进程(次选)
            break;
        }
    }

    // 程序结束时清理管道资源
    fclose(mplayer_pipe1);
    fclose(mplayer_pipe2);
    fclose(mplayer_pipe3);
    fclose(mplayer_pipe4);
    system("rm -f /tmp/mplayer-control1 /tmp/mplayer-control2 /tmp/mplayer-control3 /tmp/mplayer-control4");
    display_jpeg_picture_to_framebuffer("desk.jpg", 0, 0, fb_addr, 1); //进入娱乐界面

 }
