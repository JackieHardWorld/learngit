#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//******************************************************************************************************************************
//一、本节主要是学习读写锁
//1。读写锁的初始化和销毁
//2.读写锁的加锁与解锁（加锁分读锁与写锁）
//3.读锁可以同时多条线程去读，写锁不可以同时多条线程去写
//二、主要学习的函数
//1.pthread_rwlock_init(&rwlock, NULL);//初始化读写锁
//2.pthread_rwlock_destroy(&rwlock);//销毁读写锁
//3.pthread_rwlock_rdlock(&rwlock);//加读锁
//4.pthread_rwlock_wrlock(&rwlock);//加写锁
//5.pthread_rwlock_unlock(&rwlock);//解锁（读锁与写锁都可以解）
//三、什么时候用读写锁
//1.需要去多条线程去同时读一块共享数据或者文件（要注意文件的光标也是属于共享的，读操作的时候一定要千万小心）
//********************************************************************************************************************************

int count=100;

pthread_rwlock_t rwlock;

void *read_thread(void *arg)
{

	int i=5;
	pthread_rwlock_rdlock(&rwlock);//加读锁

	while(i--)
	{
		sleep(1);
		printf("读线程：%s, count=%d\n", (char *)arg, count);
	}
	
	pthread_rwlock_unlock(&rwlock);//解锁
	
	return "xixi";
}

void *write_thread(void *arg)
{
	int i=5;
	pthread_rwlock_wrlock(&rwlock);//加写锁

	while(i--)
	{
		sleep(1);
		printf("写线程：%s, count=%d\n", (char *)arg, count+=100);
	}
	
	pthread_rwlock_unlock(&rwlock);//解锁
}

int main(void)
{
	pthread_t tid[2];
	int retval;
	
	pthread_rwlock_init(&rwlock, NULL);//初始化读写锁

	retval = pthread_create(tid, NULL, write_thread, "第一条");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程失败:%s\n", strerror(retval));
		return -1;
	}
	
	retval = pthread_create(tid+1, NULL, write_thread, "第二条");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程失败:%s\n", strerror(retval));
		return -1;
	}
	
	pthread_join(tid[0], NULL);
	
	pthread_join(tid[1], NULL);
	
	pthread_rwlock_destroy(&rwlock);//销毁读写锁

	return 0;
}
