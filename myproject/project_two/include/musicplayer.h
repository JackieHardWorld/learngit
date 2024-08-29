#ifndef _MUSICPLAYER_H
#define _MUSICPLAYER_H

int play_music_flag;//播放音乐标志位
extern int ts_fd;
extern unsigned int *fb_addr;//操作LCD映射地址
extern class_manage_t *manager;// 全局变量，所有函数都可以操作链表

extern void load_music_to_screen(void);
extern void music_player(int ts_fd);

#endif