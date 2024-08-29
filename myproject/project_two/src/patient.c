#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include <unistd.h>
#include <errno.h>
#include "patient.h"


patient_node_t *Init_patient_node(patient_node_t *data)
{
    patient_node_t *new_node;
    new_node = malloc(sizeof(patient_node_t));
    if(new_node == NULL)
    {
        perror("学生节点创建失败\n");
        return NULL;
    }
    if (data != NULL)
	  new_node = data;//将数据存放到节点当中

    INIT_LIST_HEAD(&new_node->list);

    return new_node;

}



void destroy_patient_list(patient_node_t* head)
 {
    struct list_head *pos, *n;
    patient_node_t *bpos;
    list_for_each_safe(pos, n, &head->list) 
    {
        bpos = list_entry(pos, patient_node_t, list);
        printf("销毁了%s %s %d\n", bpos->name, bpos->gender, bpos->age);
        list_del(pos);
        free(bpos);
    }
    

    free(head);
}




void file_write_patient_info(FILE *fp, class_manage_t *manager)
{
    
    struct list_head *pos;
    patient_node_t *bpos;
    list_for_each(pos, &manager->patient_list->list) // 遍历链表
    {
        bpos = list_entry(pos, patient_node_t, list); // 获取节点数据
        fprintf(fp, "姓名:%-10s 性别:%2s 年龄:%d\n", bpos->name, bpos->gender, bpos->age);

    }
}


void file_read_patient_info(FILE *fp, class_manage_t *manager)
{
    patient_node_t file_save;
    char buffer[100];
    char *pos;
    char *ret_value;
    int count = manager->patient_cur_numb;
    int i = 0;
    fseek(fp, 0, SEEK_SET);
    while(1)
    {
        ret_value = fgets(buffer, 100, fp);
        if(ret_value == NULL)
        {
            printf("文件内容获取完毕\n");
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

        pos = strstr(buffer, "性别:");
        if(pos == NULL)
        {
            perror("没找到性别，文件加载出错\n");
        }
        pos = pos + strlen("性别:");
        sscanf(pos, "%s", file_save.gender);

        pos = strstr(buffer, "年龄:");
        if(pos == NULL)
        {
            perror("没找到年龄，文件加载出错\n");
        }
        pos = pos + strlen("年龄:");
        sscanf(pos, "%d", &file_save.age);

        printf("姓名:%-10s 性别:%2s 年龄:%d\n", file_save.name, file_save.gender, file_save.age);
        
    }

}



void insert_patient_node(class_manage_t *manager, FILE *fp)
{

	patient_node_t* new_node = Init_patient_node(NULL);
	if (new_node == NULL)
	{
		printf("结点内存分配失败\n");
		return ;
	}
	printf("请输入你的姓名：");
	scanf("%s", new_node->name);
	printf("\n请输入你的性别：");
	scanf("%s", new_node->gender);
	printf("\n请输入你的年龄：");
	scanf("%d", &(new_node->age));

    struct list_head *pos;
    patient_node_t *bpos;
    list_for_each(pos, &manager->patient_list->list) // 遍历链表
    {
        bpos = list_entry(pos, patient_node_t, list); // 获取节点数据
        if(bpos->age > new_node->age) 
        {
            break;
        }

    }
    list_add_tail(&new_node->list, pos);
	manager->patient_cur_numb++;
	printf("\n学生信息添加成功\n");

    fseek(fp, 0, SEEK_END); // 移动到文件末尾
    fprintf(fp, "姓名:%-10s 性别:%2s 年龄:%d\n", new_node->name, new_node->gender, new_node->age);
    fflush(fp); // 刷新文件缓冲区，确保数据写入

}


void foreach_patient_node(class_manage_t *manager)
{
    struct list_head *pos;
    patient_node_t *bpos;
    list_for_each(pos, &manager->patient_list->list) // 遍历链表
    {
        bpos = list_entry(pos, patient_node_t, list); // 获取节点数据
        printf("姓名：%s 性别：%s 年龄：%d\n", bpos->name, bpos->gender, bpos->age);

    }
}




patient_node_t *find_patient_node(class_manage_t *manager, char *name)
{
    struct list_head *pos;
    patient_node_t *bpos;
    list_for_each(pos, &manager->patient_list->list) // 遍历链表
    {
        bpos = list_entry(pos, patient_node_t, list); // 获取节点数据
        if(strcmp(bpos->name, name) == 0)
        {
            printf("找到了,你要找的学生信息如下\n");
            printf("姓名：%s 性别：%s 年龄：%d\n", bpos->name, bpos->gender, bpos->age);
            return bpos;
        }
    }
    return NULL;
}


void delete_patient_node(class_manage_t *manager, FILE *fp)
{   
    char name[20];
    printf("请输入你要删除的学生：");
	scanf("%s", name);
    patient_node_t *bops = find_patient_node(manager, name);
    if(bops == NULL)
    {
        printf("删除失败\n");
        return;
    }
    list_del(&bops->list);
    free(bops);
    manager->patient_cur_numb--;
    printf("删除成功\n");

    // 重新写入整个链表到文件
    freopen("patient_info.txt", "w+", fp); // 清空文件并重新打开
    file_write_patient_info(fp, manager);
    fflush(fp);

}


void list_bubble_patient_info(class_manage_t *manager)
{
    if (manager->patient_list == NULL || list_empty(&manager->patient_list->list))
        return;

    struct list_head *i, *j;
    patient_node_t *bpos, *bpos_next;
    int swapped;

    for (i = manager->patient_list->list.next; i->next != &manager->patient_list->list; i = i->next) 
    {
        swapped = 0;
        for (j = manager->patient_list->list.next; j->next != &manager->patient_list->list; j = j->next) 
        {
            bpos = list_entry(j, patient_node_t, list);
            bpos_next = list_entry(j->next, patient_node_t, list);

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


void change_patient_node(class_manage_t *manager, FILE *fp)
{
    char name[20];
    printf("请输入你要修改的学生：");
	scanf("%s", name);
    patient_node_t *new_node = find_patient_node(manager, name);
    if(new_node == NULL)
    {
        printf("修改失败\n");
        return;
    }
    printf("请输入你的姓名：");
	scanf("%s", new_node->name);
	printf("\n请输入你的性别：");
	scanf("%s", new_node->gender);
	printf("\n请输入你的年龄：");
	scanf("%d", &(new_node->age));
    // list_add_tail(&new_node->list, &manager->patient_list->list);
    list_bubble_patient_info(manager);
	printf("\n学生信息修改成功\n");
    

    // 重新写入整个链表到文件
    freopen("patient_info.txt", "w+", fp); // 清空文件并重新打开
    file_write_patient_info(fp, manager);
    fflush(fp);
}


void file_load_patient_info(FILE *fp, class_manage_t *manager)
{

    patient_node_t* file_save;
    char buffer[100];
    char *pos;
    char *ret_value;
    int i = 0;
    fseek(fp, 0, SEEK_SET);
    while(1)
    {
        ret_value = fgets(buffer, 100, fp);
        // printf("%s",buffer);
        if(ret_value == NULL)
        {
            if (ret_value == NULL)
            {
                if (feof(fp))
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
        }
        file_save = malloc(sizeof(patient_node_t));
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

        pos = strstr(buffer, "性别:");
        if(pos == NULL)
        {
            perror("没找到性别，文件加载出错\n");
        }
        pos = pos + strlen("性别:");
        sscanf(pos, "%s", file_save->gender);

        pos = strstr(buffer, "年龄:");
        if(pos == NULL)
        {
            perror("没找到年龄，文件加载出错\n");
        }
        pos = pos + strlen("年龄:");
        sscanf(pos, "%d", &file_save->age);



        struct list_head *pose;
        patient_node_t *bpos;
        list_for_each(pose, &manager->patient_list->list) // 遍历链表
        {
            bpos = list_entry(pose, patient_node_t, list); // 获取节点数据
            if(bpos->age > file_save->age) 
            {
                break;
            }

        }
        list_add_tail(&file_save->list, pose);
        manager->patient_cur_numb++;
        
    }

}


char *read_patient_head_node(class_manage_t *manager)
{
    struct list_head *pos;
    patient_node_t *bpos;
    pos = (&manager->patient_list->list)->next; // 获取第一个节点 
    if(pos == &manager->patient_list->list)
    {
        printf("链表为空\n");
        return NULL;
    }
    bpos = list_entry(pos, patient_node_t, list); // 获取节点数据
   // printf("姓名：%s 性别：%s 年龄：%d\n", bpos->name, bpos->gender, bpos->age);
    //printf("患者姓名:%s\n", (char*) bpos->name);
    return (char *)bpos->name;

}







