#include "thread_pool.h"

void *mytask1(void *arg)
{
	long n = (long)arg;

	printf("线程号为[%u][%s] ==> 要等待 %ld 秒才能完成任务...\n",
		(unsigned)pthread_self(), __FUNCTION__, n);

	sleep(n);

	printf("线程号为[%u][%s] ==> 完成任务!\n",
		(unsigned)pthread_self(), __FUNCTION__);

	return NULL;
}

void *mytask2(void *arg)
{
	long n = (long)arg;

	printf("线程号为[%u][%s] ==> 要等待 %ld 秒才能完成任务...\n",
		(unsigned)pthread_self(), __FUNCTION__, n);

	sleep(n);

	printf("线程号为[%u][%s] ==> 完成任务!\n",
		(unsigned)pthread_self(), __FUNCTION__);

	return NULL;
}

void *mytask3(void *arg)
{
	long n = (long)arg;

	printf("线程号为[%u][%s] ==> 要等待 %ld 秒才能完成任务...\n",
		(unsigned)pthread_self(), __FUNCTION__, n);

	sleep(n);

	printf("线程号为[%u][%s] ==> 完成任务!\n",
		(unsigned)pthread_self(), __FUNCTION__);

	return NULL;
}

void *mytask4(void *arg)
{
	while(1)
	{
		sleep(1);
		printf("要完蛋了\n");
	}

	return NULL;
}

void *count_time(void *arg)
{
	int n=0;

	while(1)
	{
		printf("现在是第 %d 秒\n", n++);
		sleep(1);
	}
}

int main(void)
{
	pthread_t a;
	pthread_create(&a, NULL, count_time, NULL);//只是用来创建一天用于计时的线程，跟线程池一点关系都没有（进程（主程序）退出了它自然就没了）

	// 1, initialize the pool
	thread_pool pool;
	init_pool(&pool, 2);
	
	// 2, throw tasks
	printf("throwing 3 tasks...\n");
	add_task(&pool, mytask1, (void *)(rand()%10L));
	add_task(&pool, mytask2, (void *)(rand()%10L));
	add_task(&pool, mytask3, (void *)(rand()%10L));
	//add_task(&pool, mytask4, NULL);//死循坏线程


	// 7, destroy the pool
	destroy_pool(&pool);
	
	
	return 0;
}
