#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include <unistd.h>
#include <errno.h>
#include "doctor.h"

doctor_node_t *Init_doctor_node(doctor_node_t *data)
{
    doctor_node_t *new_node;
    new_node = malloc(sizeof(doctor_node_t));
    if(new_node == NULL)
    {
        perror("教师节点创建失败\n");
        return NULL;
    }
    if (data != NULL)
	  new_node = data;//将数据存放到节点当中

    INIT_LIST_HEAD(&new_node->list);

    return new_node;

}




void destroy_doctor_list(doctor_node_t* head)
{
    struct list_head *pos, *n;
    doctor_node_t *bpos;
    list_for_each_safe(pos, n, &head->list) 
    {
        bpos = list_entry(pos, doctor_node_t, list);
        printf("销毁了%s %s %d\n", bpos->name, bpos->position, bpos->age);
        list_del(pos);
        free(bpos);
    }

    free(head);
}



void file_write_doctor_info(FILE *fp2, class_manage_t *manager)
{
    struct list_head *pos;
    doctor_node_t *bpos;
    list_for_each(pos, &manager->doctor_list->list) // 遍历链表
    {
        bpos = list_entry(pos, doctor_node_t, list); // 获取节点数据
        fprintf(fp2, "姓名:%-10s 职称:%2s 年龄:%d\n", bpos->name, bpos->position, bpos->age);

    }
}



void file_read_doctor_info(FILE *fp2, class_manage_t *manager)
{
    doctor_node_t file_save;
    char buffer[100];
    char *pos;
    char *ret_value;
    int count = manager->doctor_cur_numb;
    int i = 0;
    fseek(fp2, 0, SEEK_SET);
    for(i = 0; i<count; i++)
    {
        ret_value = fgets(buffer, 100, fp2);
        if(ret_value == NULL)
        {
            perror("文件内容获取失败\n");
            break;
        }
        // printf("%s", buffer);
        pos = strstr(buffer, "姓名:");
        if(pos == NULL)
        {
            perror("没找到姓名，文件加载出错\n");
        }
        pos = pos + strlen("姓名:");
        sscanf(pos, "%s", file_save.name);

        pos = strstr(buffer, "职称:");
        if(pos == NULL)
        {
            perror("没找到性别，文件加载出错\n");
        }
        pos = pos + strlen("职称:");
        sscanf(pos, "%s", file_save.position);

        pos = strstr(buffer, "年龄:");
        if(pos == NULL)
        {
            perror("没找到年龄，文件加载出错\n");
        }
        pos = pos + strlen("年龄:");
        sscanf(pos, "%d", &file_save.age);

        printf("姓名:%-10s 职称:%2s 年龄:%d\n", file_save.name, file_save.position, file_save.age);
    }
}


void insert_doctor_node(class_manage_t *manager, FILE *fp2)
{

	doctor_node_t* new_node = Init_doctor_node(NULL);
	if (new_node == NULL)
	{
		printf("结点内存分配失败\n");
		return ;
	}
	printf("请输入你的姓名：");
	scanf("%s", new_node->name);
	printf("\n请输入你的职称:");
	scanf("%s", new_node->position);
	printf("\n请输入你的年龄:");
	scanf("%d", &(new_node->age));
    
    struct list_head *pos;
    doctor_node_t *bpos;
    list_for_each(pos, &manager->doctor_list->list) // 遍历链表
    {
        bpos = list_entry(pos, doctor_node_t, list); // 获取节点数据
        if(bpos->age > new_node->age)
        {
            break;
        } 

    }
    list_add_tail(&new_node->list, pos);
    // list_add_tail(&new_node->list, &manager->doctor_list->list); //后插
	manager->doctor_cur_numb++;
	printf("\n教师信息添加成功\n");

    fseek(fp2, 0, SEEK_END); // 移动到文件末尾
    fprintf(fp2, "姓名:%-10s 职称:%2s 年龄:%d\n", new_node->name, new_node->position, new_node->age);
    fflush(fp2); // 刷新文件缓冲区，确保数据写入
}


void foreach_doctor_node(class_manage_t *manager)
{
    struct list_head *pos;
    doctor_node_t *bpos;
    list_for_each(pos, &manager->doctor_list->list) // 遍历链表
    {
        bpos = list_entry(pos, doctor_node_t, list); // 获取节点数据
        printf("姓名：%s 性别：%s 年龄：%d\n", bpos->name, bpos->position, bpos->age);

    }
}


doctor_node_t *find_doctor_node(class_manage_t *manager, char *name)
{

    struct list_head *pos;
    doctor_node_t *bpos;
    list_for_each(pos, &manager->doctor_list->list) // 遍历链表
    {
        bpos = list_entry(pos, doctor_node_t, list); // 获取节点数据
        if(strcmp(bpos->name, name) == 0)
        {
            printf("找到了,你要找的教师信息如下\n");
            printf("姓名：%s 性别：%s 年龄：%d\n", bpos->name, bpos->position, bpos->age);
            return bpos;
        }
    }
    printf("没有找到\n");
    return NULL;
}


void delete_doctor_node(class_manage_t *manager, FILE *fp2)
{
    char name[20];
    printf("请输入你要删除的教师：");
	scanf("%s", name);
    doctor_node_t *bops = find_doctor_node(manager, name);
    if(bops == NULL)
    {
        printf("删除失败\n");
        return;
    }
    
    list_del(&bops->list);
    free(bops);
    manager->doctor_cur_numb--;
    printf("删除成功\n");

    // 重新写入整个链表到文件
    freopen("doctor_info.txt", "w+", fp2); // 清空文件并重新打开
    file_write_doctor_info(fp2, manager);
    fflush(fp2);
}



void list_bubble_doctor_info(class_manage_t *manager)
{
    if (manager->doctor_list == NULL || list_empty(&manager->doctor_list->list))
        return;

    struct list_head *i, *j;
    doctor_node_t *bpos, *bpos_next;
    int swapped;

    for (i = manager->doctor_list->list.next; i->next != &manager->doctor_list->list; i = i->next) 
    {
        swapped = 0;
        for (j = manager->doctor_list->list.next; j->next != &manager->doctor_list->list; j = j->next) 
        {
            bpos = list_entry(j, doctor_node_t, list);
            bpos_next = list_entry(j->next, doctor_node_t, list);

            if (bpos->age > bpos_next->age) 
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


void change_doctor_node(class_manage_t *manager, FILE *fp2)
{
    char name[20];
    printf("请输入你要修改的教师：");
	scanf("%s", name);
    doctor_node_t *new_node = find_doctor_node(manager, name);
    if(new_node == NULL)
    {
        printf("修改失败\n");
        return;
    }
    printf("请输入你的姓名：");
	scanf("%s", new_node->name);
	printf("\n请输入你的性别：");
	scanf("%s", new_node->position);
	printf("\n请输入你的年龄：");
	scanf("%d", &(new_node->age));
    // list_add_tail(&new_node->list, &manager->doctor_list->list);
	printf("\n教师信息修改成功\n");
    list_bubble_doctor_info(manager);

    // 重新写入整个链表到文件
    freopen("doctor_info.txt", "w+", fp2); // 清空文件并重新打开
    file_write_doctor_info(fp2, manager);
    fflush(fp2);
}



void file_load_doctor_info(FILE *fp2, class_manage_t *manager)
{
    doctor_node_t *file_save;
    char buffer[100];
    char *pos;
    char *ret_value;
    int i = 0;
    fseek(fp2, 0, SEEK_SET);
    while(1)
    {
        ret_value = fgets(buffer, 100, fp2);    
        if (ret_value == NULL)
        {
            if (feof(fp2))
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
    
        file_save = Init_doctor_node(NULL);
        if (file_save == NULL)
        {
            printf("结点内存分配失败\n");
            return ;
        }
        
        pos = strstr(buffer, "姓名:");
        if(pos == NULL)
        {
            perror("没找到姓名，文件加载出错\n");
        }
        pos = pos + strlen("姓名:");
        sscanf(pos, "%s", file_save->name);

        pos = strstr(buffer, "职称:");
        if(pos == NULL)
        {
            perror("没找到性别，文件加载出错\n");
        }
        pos = pos + strlen("职称:");
        sscanf(pos, "%s", file_save->position);

        pos = strstr(buffer, "年龄:");
        if(pos == NULL)
        {
            perror("没找到年龄，文件加载出错\n");
        }
        pos = pos + strlen("年龄:");
        sscanf(pos, "%d", &file_save->age);

        struct list_head *pose;
        doctor_node_t *bpos;
        list_for_each(pose, &manager->doctor_list->list) // 遍历链表
        {
            bpos = list_entry(pose, doctor_node_t, list); // 获取节点数据
            if(bpos->age > file_save->age)
            {
                break;
            } 

        }
        list_add_tail(&file_save->list, pose);
        manager->doctor_cur_numb++;

    }
}
