#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>


//******************************************************************************************************************************
//一、本节主要是学习线程的各种属性的设置
//1.一般设置几个主要的属性：分离、栈大小、优先级、是否继承调度策略（不继承才能设置调度策略）、调度策略
//2.非实时线程（0）与实时线程（1~99数字越大，优先级越高），不管实时线程的优先级多高，非实时线程永远都有享有cpu分配的一点资源
// 当优先级不等时，低的不会被分配一点资源；当优先级相等时，看你设置的调度策略时抢占还是轮询，抢占即是一直霸占着了。（第二点是争对单核cpu来说的）

//二、主要学习的函数
//1.pthread_attr_t attr1; pthread_attr_init(&attr1);//线程属性变量的初始化
//2.pthread_attr_destroy(&attr1);//线程属性变量的销毁
//3.pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);//设置线程是否继承创建者的调度策略 PTHREAD_EXPLICIT_SCHED
//4.pthread_attr_setschedpolicy(&attr1, SCHED_RR);//设置线程的调度策略：SCHED_FIFO：抢占性调度; SCHED_RR：轮寻式调度；SCHED_OTHER：非实时线程调度策略
//5.struct sched_param param1;  param1.sched_priority = 10; pthread_attr_setschedparam(&attr1, &param1);//设置线程静态优先级
//6.pthread_join(tid, &retp);//线程的接合/等待函数，等待线程结束

//********************************************************************************************************************************

void *start_routine(void *arg)
{
	
	int i, j;

	while(1)
	{

		fprintf(stderr, "%c ", *(char *)arg);

		for(i=0; i<100000; i++)
			for(j=0; j<1000; j++);
	}

	pthread_exit(NULL);
}


int main(void)
{

	pthread_t tid1, tid2, tid3;
	pthread_attr_t attr1, attr2;
	struct sched_param param1, param2;

	/* 线程属性变量的初始化 */
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);


	/* 设置线程是否继承创建者的调度策略 PTHREAD_EXPLICIT_SCHED：不继承才能设置线程的调度策略*/
	errno = pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
	if(errno != 0)
	{
		perror("setinherit failed\n");
		return -1;
	}

	/* 设置线程是否继承创建者的调度策略 PTHREAD_EXPLICIT_SCHED：不继承才能设置线程的调度策略*/
	errno = pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
	if(errno != 0)
	{
		perror("setinherit failed\n");
		return -1;
	}

	/* 设置线程的调度策略：SCHED_FIFO：抢占性调度; SCHED_RR：轮寻式调度；SCHED_OTHER：非实时线程调度策略*/
	errno = pthread_attr_setschedpolicy(&attr1, SCHED_RR);
	if(errno != 0)
	{
		perror("setpolicy failed\n");
		return -1;
	}

	errno = pthread_attr_setschedpolicy(&attr2, SCHED_FIFO);
	if(errno != 0)
	{
		perror("setpolicy failed\n");
		return -1;
	}

	
	param1.sched_priority = 10;
	param2.sched_priority = 10;
	
	//查看抢占性调度策略的最小跟最大静态优先级的值是多少
	printf("min=%d, max=%d\n", sched_get_priority_min(SCHED_FIFO), sched_get_priority_max(SCHED_FIFO));


	/* 设置线程静态优先级 */
	errno = pthread_attr_setschedparam(&attr1, &param1);
	if(errno != 0)
	{
		perror("setparam failed\n");
		return -1;
	}

	errno = pthread_attr_setschedparam(&attr2, &param2);
	if(errno != 0)
	{
		perror("setparam failed\n");
		return -1;
	}

	errno = pthread_create(&tid1, &attr1, start_routine, (void *)"1");
	if(errno != 0)
	{
		perror("create thread 1 failed\n");
		return -1;
	}

	errno = pthread_create(&tid2, &attr2, start_routine, (void *)"2");
	if(errno != 0)
	{
		perror("create thread 2 failed\n");
		return -1;
	}

	errno = pthread_create(&tid3, NULL, start_routine, (void *)"3");
	if(errno != 0)
	{
		perror("create thread 3 failed\n");
		return -1;
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);

	pthread_attr_destroy(&attr1);
	pthread_attr_destroy(&attr2);

	return 0;
}
