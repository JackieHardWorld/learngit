#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


//******************************************************************************************************************************
//一、本节主要是学习线程的设置分离属性
//1.pthread_attr_t attr;//诞生一个属性变量
//2.pthread_attr_init(&attr);//进行初始化
//3.pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//设置线程为完全分离
//4.pthread_attr_destroy(&attr);//销毁属性结构体

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
	

	
	return "xixi";
}


int main(void)
{
	pthread_t tid;
	int retval;
	
	pthread_attr_t attr;//诞生一个属性变量
	
	pthread_attr_init(&attr);//进行初始化
	
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//设置线程为完全分离

	retval = pthread_create(&tid, &attr, new_thread, "hello");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程失败:%s\n", strerror(retval));
		return -1;
	}
	
	pthread_attr_destroy(&attr);
	
	retval = pthread_join(tid, NULL);
	if(retval != 0)
	{
		fprintf(stderr, "接合线程失败:%s\n", strerror(retval));
		return -1;
	}
	

	return 0;
}
