#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//******************************************************************************************************************************
//一、本节主要是学习线程的的等待/接合函数
//1.return ”xixi“  与pthread_exit（"xixi"）等价
//2.如果线程的返回值不为0，则说明线程创建失败
//3.void *retp;   pthread_join(tid, &retp);//如果线程被取消，retp 是 PTHREAD_CANCELED;如果正常退出是线程返回值（“xixi”）
//4.retp类型是void*,如果需要打印要用char*强转

//二、主要学习的函数
//1.pthread_create(&tid, NULL, new_thread, "hello");//线程的创建函数，第二个如果不需要设置线程的属性则位NULL,第四个为线程执行函数的参数，没有可以填NULL
//2.pthread_exit("xixi");//退出线程
//3.pthread_join(tid, &retp);//线程的接合/等待函数
//********************************************************************************************************************************

void *new_thread(void *arg)
{
	char *str = arg;

	int i=5;

	while(i--)
	{
		sleep(1);
		printf("我是新线程：%s\n", str);
	}
	
	pthread_exit("xixi");//退出线程
	
	return "xixi";
}


int main(void)
{
	pthread_t tid;
	int retval;

	retval = pthread_create(&tid, NULL, new_thread, "hello");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程失败:%s\n", strerror(retval));
		return -1;
	}
	
	void *retp;
	
	//pthread_exit(NULL);
	
	pthread_join(tid, &retp);//线程的接合/等待函数
	//pthread_join(tid, NULL);
	printf("接合成功：%s\n", (char *)retp);

	return 0;
}
