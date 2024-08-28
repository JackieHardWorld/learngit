#include "thread_pool.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>



#define PAGE_SIZE	4096
#define NMEMB	256

typedef struct filename
{
	char *src_dir;
	char *dest_dir;
	thread_pool pool;

}filename_t;


void *mytask1(void *arg);

void *mytask2(void *arg);

long get_file_size(const char *pathname)
{
    struct stat st;
    int retval;

    retval = stat(pathname, &st);
    if(retval == -1)
    {
        perror("获取文件属性失败");
        return -1;
    }

	return st.st_size;
}

int create_dir(const char *dirname)
{
    int retval;
    struct stat st;

    if(access(dirname, F_OK) != 0)//如果文件夹不存在则创建
    {
        mkdir(dirname, 0755);
    }
    else
    {
        retval = stat(dirname, &st);//获取文件属性
        if(retval == -1)
        {
            perror("获取文件属性失败");
            return -1;
        }

        if(!S_ISDIR(st.st_mode))//如果这个文件不是一个文件夹，则不能复制东西过去了
        {
            fprintf(stderr, "目标文件夹%s：他是一个已经存在的文件，并非文件夹，复制失败\n", dirname);
            return -1;
        }
    }

    return 0;
}


int copy_file(const char *src_name, const char *dest_name)
{
	FILE *src_fp, *dest_fp;
	void *buffer;

	// 确保目标目录存在
    char dest_dir[512];
    strncpy(dest_dir, dest_name, sizeof(dest_dir));
    char *last_slash = strrchr(dest_dir, '/');
    if (last_slash != NULL)
    {
        *last_slash = '\0'; // 去除文件名，留下目录路径
        if (create_dir(dest_dir) == -1)
        {
			printf("创建目标目录失败\n");
            return -1; // 创建目标目录失败
        }
    }

	src_fp = fopen(src_name, "r");
	if(src_fp == NULL)
	{
		perror("打开源文件失败");
		goto open_src_file_err;
	}

	dest_fp = fopen(dest_name, "w");
	if(dest_fp == NULL)
	{
		perror("打开目标文件失败666");
		goto open_dest_file_err;
	}
	
	buffer = malloc(PAGE_SIZE*NMEMB);
	if(buffer == NULL)
	{
		perror("申请内存失败");
		goto request_mem_err;
	}
	
	size_t rd_items, wr_items;
	long src_file_size, cur_copy_size=0;

	src_file_size = get_file_size(src_name);

	printf("\n");

	while(1)
	{
        
		printf("\033[1A正在复制%s文件到%s，进度：%ld%%\n", src_name, dest_name, (long)(cur_copy_size/(double)src_file_size*100));
        

		rd_items = fread(buffer, PAGE_SIZE, NMEMB, src_fp);
		if(rd_items < NMEMB)
		{
			//判断读取到文件末尾
			if(feof(src_fp))
			{
				wr_items = fwrite(buffer, src_file_size-cur_copy_size, 1, dest_fp);
				if(wr_items < 1)
					goto opt_file_err;

                cur_copy_size += src_file_size-cur_copy_size;
                printf("\033[1A正在复制%s文件到%s，进度：%ld%%\n", src_name, dest_name, (long)(cur_copy_size/(double)src_file_size*100));
				break;
			}
			
			goto opt_file_err;
		}
		
		wr_items = fwrite(buffer, PAGE_SIZE, NMEMB, dest_fp);
		if(ferror(dest_fp))
		{
			perror("写入失败");
			goto opt_file_err;
		}

		cur_copy_size += PAGE_SIZE*NMEMB;
	}
	
	free(buffer);

	fclose(dest_fp);

	fclose(src_fp);
	
	return 0;

opt_file_err:
	
	free(buffer);
request_mem_err:
	fclose(dest_fp);
	unlink(dest_name);//删除掉目标文件
open_dest_file_err:
	fclose(src_fp);

open_src_file_err:
	return -1;
}

int copy_dir(filename_t *p)
{
	DIR *dp;
	struct dirent *dirstruct;
	char src_pathname[512];
    char dest_pathname[512];
    int retval;

	//打开src_dir文件夹
	dp = opendir(p->src_dir);
	if(dp == NULL)
	{
		fprintf(stderr, "打开%s目录失败:%s\n", p->src_dir, strerror(errno));
		return -1;
	}
	
	while(1)
	{
	
		dirstruct = readdir(dp);//每次读取得到文件夹当中的一个文件的信息
		if(dirstruct == NULL)//读取结束
			break;

		//组合路径，因为你现在读到的文件名字是纯名字，不包含路径的，我们需要把这个文件在哪个路径之下组合进去
		snprintf(src_pathname, 512, "%s/%s", p->src_dir, dirstruct->d_name);
        snprintf(dest_pathname, 512, "%s/%s", p->dest_dir, dirstruct->d_name);

		//printf("名字：%s\n", src_pathname);

		//display_file_info(src_pathname);//以列表的形式打印文件信息

		//如果发现该文件是文件夹，并且不是.  跟..文件，那就递归的进去这个文件夹里面查看文件的内容
		if(dirstruct->d_type == DT_DIR && strcmp(dirstruct->d_name, ".") !=0 && strcmp(dirstruct->d_name, "..") !=0)
		{
            retval = create_dir(dest_pathname);//创建目标文件夹
            if(retval == -1)
            {
                fprintf(stderr, "在复制文件夹的过程中,创建目标文件夹失败\n");
                    goto copy_dir_err;
            }

			printf("进入文件夹：%s\n", src_pathname);

			//再进去这个文件夹里面去复制文件
            filename_t *sub_dir_task = malloc(sizeof(filename_t));
            sub_dir_task->src_dir = strdup(src_pathname);
            sub_dir_task->dest_dir = strdup(dest_pathname);
            sub_dir_task->pool = p->pool;
			add_task(&p->pool, mytask2, (void*)sub_dir_task);

			printf("退出文件夹：%s\n", src_pathname);
		}

        if(dirstruct->d_type == DT_REG)//如果读取到的文件是一个普通文件
        {
            //复制文件
            filename_t *file_task = malloc(sizeof(filename_t));
            file_task->src_dir = strdup(src_pathname);
            file_task->dest_dir = strdup(dest_pathname);
            file_task->pool = p->pool;
            add_task(&p->pool, mytask1, (void*)file_task);
        }

	}
	
	closedir(dp);

	return 0;

copy_dir_err:
    closedir(dp);
    return -1;
}

void *mytask1(void *arg)
{
    filename_t *p = (filename_t*)arg;
    copy_file(p->src_dir, p->dest_dir);

    // 释放内存
    free(p->src_dir);
    free(p->dest_dir);
    free(p);

    return NULL;
}

void *mytask2(void *arg)
{
    filename_t *p = (filename_t*)arg;
    copy_dir(p);  // 调用复制目录的函数

    free(p->src_dir);
    free(p->dest_dir);
    free(p);

    return NULL;
}


int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "用法: %s <源目录> <目标目录>\n", argv[0]);
        return -1;
    }

    // 分配并复制路径
    filename_t *p = malloc(sizeof(filename_t));
    p->src_dir = strdup(argv[1]);
    p->dest_dir = strdup(argv[2]);

    // 初始化线程池
    init_pool(&p->pool, 2);

    // 将第一个任务添加到线程池
    add_task(&p->pool, mytask2, (void*)p);

    // 销毁线程池并等待所有任务完成
    destroy_pool(&p->pool);

    return 0;
}






