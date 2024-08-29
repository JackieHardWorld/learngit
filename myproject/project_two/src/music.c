#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include <unistd.h>
#include <errno.h>
#include "music.h"

 // 在开发板中播放音乐，可直接使用开发板中的madplay进行播放
// 1、播放
// madplay  1.mp3    //播放歌曲1.mp3
// 2、暂停
// killall -SIGSTOP madplay
// 3、继续
// killall -SIGCONT madplay
// 4、退出
// killall -SIGKILL  madplay
// 在代码中实现以上命令时，可调用system()函数完成，如：
// 播放：system("madplay 1.mp3 &");
// 暂停：system("killall -SIGSTOP madplay")
// 继续：system("killall -SIGCONT madplay")

music_node_t *Init_music_node(music_node_t *data)
{
    music_node_t *new_node;
    new_node = malloc(sizeof(music_node_t));
    if(new_node == NULL)
    {
        perror("音乐节点创建失败\n");
        return NULL;
    }
    if (data != NULL)
	  new_node = data;//将数据存放到节点当中

    INIT_LIST_HEAD(&new_node->list);

    return new_node;

}



void destroy_music_list(music_node_t* head)
{
    struct list_head *pos, *n;
    music_node_t *bpos;
    list_for_each_safe(pos, n, &head->list) 
    {
        bpos = list_entry(pos, music_node_t, list);
        printf("销毁了%s %s %s\n", bpos->name, bpos->singer, bpos->photo);
        list_del(pos);
        free(bpos);
    }

    free(head);
}



void file_write_music_info(FILE *fp3, class_manage_t *manager)
{
    struct list_head *pos;
    music_node_t *bpos;
    list_for_each(pos, &manager->music_list->list) // 遍历链表
    {
        bpos = list_entry(pos, music_node_t, list); // 获取节点数据
        fprintf(fp3, "歌名:%-10s 歌手:%2s 照片:%s\n", bpos->name, bpos->singer, bpos->photo);

    }
}



void file_read_music_info(FILE *fp3, class_manage_t *manager)
{
    music_node_t file_save;
    char buffer[100];
    char *pos;
    char *ret_value;
    int count = manager->music_cur_numb;
    int i = 0;
    fseek(fp3, 0, SEEK_SET);
    for(i = 0; i<count; i++)
    {
        ret_value = fgets(buffer, 100, fp3);
        if(ret_value == NULL)
        {
            perror("文件内容获取失败\n");
            break;
        }
        // printf("%s", buffer);
        pos = strstr(buffer, "歌名:");
        if(pos == NULL)
        {
            perror("没找到歌名，文件加载出错\n");
        }
        pos = pos + strlen("歌名:");
        sscanf(pos, "%s", file_save.name);

        pos = strstr(buffer, "歌手:");
        if(pos == NULL)
        {
            perror("没找到歌手，文件加载出错\n");
        }
        pos = pos + strlen("歌手:");
        sscanf(pos, "%s", file_save.singer);

        pos = strstr(buffer, "照片:");
        if(pos == NULL)
        {
            perror("没找到照片，文件加载出错\n");
        }
        pos = pos + strlen("照片:");
        sscanf(pos, "%s", file_save.photo);

        printf("歌名:%-10s 歌手:%2s 照片:%d\n", file_save.name, file_save.singer, file_save.photo);
    }
}


void insert_music_node(class_manage_t *manager, FILE *fp3)
{

	music_node_t* new_node = Init_music_node(NULL);
	if (new_node == NULL)
	{
		printf("结点内存分配失败\n");
		return ;
	}
	printf("请输入你的歌名：");
	scanf("%s", new_node->name);
	printf("\n请输入你的歌手:");
	scanf("%s", new_node->singer);
	printf("\n请输入你的照片:");
	scanf("%s", new_node->photo);
    
    struct list_head *pos;
    music_node_t *bpos;
    list_for_each(pos, &manager->music_list->list) // 遍历链表
    {
        bpos = list_entry(pos, music_node_t, list); // 获取节点数据
        if(strcmp(bpos->name, new_node->name) > 0)
        {
            break;
        } 

    }
    list_add_tail(&new_node->list, pos);
    // list_add_tail(&new_node->list, &manager->music_list->list); //后插
	manager->music_cur_numb++;
	printf("\n音乐信息添加成功\n");

    fseek(fp3, 0, SEEK_END); // 移动到文件末尾
    fprintf(fp3, "歌名:%-10s 歌手:%2s 照片:%s\n", new_node->name, new_node->singer, new_node->photo);
    fflush(fp3); // 刷新文件缓冲区，确保数据写入
}



void foreach_music_node(class_manage_t *manager)
{
    struct list_head *pos;
    music_node_t *bpos;
    list_for_each(pos, &manager->music_list->list) // 遍历链表
    {
        bpos = list_entry(pos, music_node_t, list); // 获取节点数据
        printf("歌名：%s 歌手：%s 照片：%s\n", bpos->name, bpos->singer, bpos->photo);

    }
}


music_node_t *find_music_node(class_manage_t *manager, char *name)
{

    struct list_head *pos;
    music_node_t *bpos;
    list_for_each(pos, &manager->music_list->list) // 遍历链表
    {
        bpos = list_entry(pos, music_node_t, list); // 获取节点数据
        if(strcmp(bpos->name, name) == 0)
        {
            printf("找到了,你要找的音乐信息如下\n");
            printf("歌名：%s 歌手：%s 照片：%s\n", bpos->name, bpos->singer, bpos->photo);
            return bpos;
        }
    }
    printf("没有找到\n");
    return NULL;
}



void delete_music_node(class_manage_t *manager, FILE *fp3)
{
    char name[20];
    printf("请输入你要删除的音乐：");
	scanf("%s", name);
    music_node_t *bops = find_music_node(manager, name);
    if(bops == NULL)
    {
        printf("删除失败\n");
        return;
    }
    
    list_del(&bops->list);
    free(bops);
    manager->music_cur_numb--;
    printf("删除成功\n");

    // 重新写入整个链表到文件
    freopen("music_info.txt", "w+", fp3); // 清空文件并重新打开
    file_write_music_info(fp3, manager);
    fflush(fp3);
}


void list_bubble_music_info(class_manage_t *manager)
{
    if (manager->music_list == NULL || list_empty(&manager->music_list->list))
        return;

    struct list_head *i, *j;
    music_node_t *bpos, *bpos_next;
    int swapped;

    for (i = manager->music_list->list.next; i->next != &manager->music_list->list; i = i->next) 
    {
        swapped = 0;
        for (j = manager->music_list->list.next; j->next != &manager->music_list->list; j = j->next) 
        {
            bpos = list_entry(j, music_node_t, list);
            bpos_next = list_entry(j->next, music_node_t, list);

            if (strcmp(bpos->name, bpos_next->name)) 
            {
                list_del(&bpos->list);
                list_add(&bpos->list, &bpos_next->list);
                swapped = 1;
            }
        }
        // 如果在一次完整的遍历中没有发生交换，说明列表已经排序好，可以退出
        if (!swapped)
            break;
    }
}


void change_music_node(class_manage_t *manager, FILE *fp3)
{
    char name[20];
    printf("请输入你要修改的歌名：");
	scanf("%s", name);
    music_node_t *new_node = find_music_node(manager, name);
    if(new_node == NULL)
    {
        printf("修改失败\n");
        return;
    }
    printf("请输入你的歌名：");
	scanf("%s", new_node->name);
	printf("\n请输入你的歌手：");
	scanf("%s", new_node->singer);
	printf("\n请输入你的照片：");
	scanf("%s", new_node->photo);
    // list_add_tail(&new_node->list, &manager->music_list->list);
	printf("\n音乐信息修改成功\n");
    list_bubble_music_info(manager);

    // 重新写入整个链表到文件
    freopen("music_info.txt", "w+", fp3); // 清空文件并重新打开
    file_write_music_info(fp3, manager);
    fflush(fp3);
}




void file_load_music_info(FILE *fp3, class_manage_t *manager)
{
    music_node_t *file_save;
    char buffer[100];
    char *pos;
    char *ret_value;
    int i = 0;
    fseek(fp3, 0, SEEK_SET);
    printf("文件内容读取中\n");
    while(1)
    {
        ret_value = fgets(buffer, 100, fp3);    
        if (ret_value == NULL)
        {
            if (feof(fp3))
            {
                printf("文件内容读取完毕\n");
                break;
            }
            else
            {
                perror("读取文件时出错\n");
                break;
            }
        }
        printf("111\n");
        file_save = Init_music_node(NULL);
        if (file_save == NULL)
        {
            printf("结点内存分配失败\n");
            return ;
        }
        printf("222\n");
        pos = strstr(buffer, "歌名:");
        if(pos == NULL)
        {
            perror("没找到歌名，文件加载出错\n");
        }
        pos = pos + strlen("歌名:");
        sscanf(pos, "%s", file_save->name);
        printf("333\n");
        pos = strstr(buffer, "歌手:");
        if(pos == NULL)
        {
            perror("没找到歌手，文件加载出错\n");
        }
        pos = pos + strlen("照片:");
        sscanf(pos, "%s", file_save->singer);
        printf("444\n");
        pos = strstr(buffer, "照片:");
        if(pos == NULL)
        {
            perror("没找到照片，文件加载出错\n");
        }
        pos = pos + strlen("照片:");
        sscanf(pos, "%s", file_save->photo);
        printf("555\n");
        struct list_head *pose;
        music_node_t *bpos;
        list_for_each(pose, &manager->music_list->list) // 遍历链表
        {
            printf("888\n");
            bpos = list_entry(pose, music_node_t, list); // 获取节点数据
            printf("666\n");
            if(strcmp(bpos->name, file_save->name) > 0)
            {
                break;
            } 
            printf("777\n");

        }
        list_add_tail(&file_save->list, pose);
        manager->music_cur_numb++;
        printf("音乐信息加载成功\n");
    }
}

