#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
//******************************************************************************************************************************
//一、本节主要是学习条件变量
//1。条件变量是配合互斥锁一起出现的
//2.目的是让线程解锁，然后进入睡眠状态（没必要一直拿锁解锁，让别的真需要拿锁的先拿）
//3.线程的唤醒，可以唤醒一条，也可以同时唤醒全部线程
//二、主要学习的函数
//1.pthread_cond_init(&cond, NULL);//初始化条件变量
//2.pthread_cond_destroy(&cond);//销毁条件变量
//3.pthread_cond_wait(&cond, &mutex);//解锁，睡觉觉,等待被唤醒 ，如果被唤醒再去拿锁
//4.pthread_cond_signal(&cond);////唤醒一条线程(至少唤醒一个等待的线程，但如果有多个线程在等待，具体唤醒哪个线程是不确定的，取决于线程调度策略。)
//5.pthread_cond_broadcast(&cond);//唤醒所有因指定条件变量睡眠的线程(同时唤醒，但是不意味着同时执行，要看谁先获取到互斥锁)
//三、什么时候用条件变量
//1.想让线程等待某个条件符合时才运行，不符合时去睡觉
//********************************************************************************************************************************
int count=1000;

pthread_mutex_t mutex;
pthread_cond_t cond;

void *new_thread(void *arg)
{
	int retval;

	while(1)
	{

		pthread_mutex_lock(&mutex);
		
		printf("%s线程拿锁成功\n", (char *)arg);
		
		while(count <100)
		{
			printf("线程%s开始睡眠\n", (char *)arg);
			
			//解锁，睡觉觉,等待被唤醒 ，如果被唤醒再去拿锁
			pthread_cond_wait(&cond, &mutex);
			
			printf("线程%s醒来啦\n", (char *)arg);
		}


		count -= 100;
		
		sleep(1);
		
		printf("%s线程还锁成功,余额=%d\n", (char *)arg, count);
		pthread_mutex_unlock(&mutex);
		
	
	}
	
	return NULL;
}

void sighand(int signum)
{
	pthread_mutex_lock(&mutex);
	printf("充值\n");
	count+=1000;
	pthread_mutex_unlock(&mutex);
	
	pthread_cond_signal(&cond);//唤醒一条线程(至少唤醒一个等待的线程，但如果有多个线程在等待，具体唤醒哪个线程是不确定的，取决于线程调度策略。)
	//pthread_cond_broadcast(&cond);//唤醒所有因指定条件变量睡眠的线程(同时唤醒，但是不意味着同时执行，要看谁先获取到互斥锁)
}


int main(void)
{
	pthread_t tid[3];
	int retval;
	
	pthread_mutex_init(&mutex, NULL);//初始化互斥锁
	pthread_cond_init(&cond, NULL);//初始化条件变量
	

	retval = pthread_create(tid, NULL, new_thread, "1");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程失败:%s\n", strerror(retval));
		return -1;
	}
	
	retval = pthread_create(tid+1, NULL, new_thread, "2");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程失败:%s\n", strerror(retval));
		return -1;
	}
	
	retval = pthread_create(tid+2, NULL, new_thread, "3");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程失败:%s\n", strerror(retval));
		return -1;
	}
	
	signal(SIGINT, sighand);
	
	
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);
	
	pthread_cond_destroy(&cond);//销毁条件变量
	pthread_mutex_destroy(&mutex);//销毁互斥锁
	
	printf("count=%d\n", count);


	return 0;
}
