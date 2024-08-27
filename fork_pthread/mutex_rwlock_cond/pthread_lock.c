#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
//******************************************************************************************************************************
//一、本节主要是学习互斥锁
//1。互斥锁的初始化和销毁
//2.互斥锁的拿锁与还锁
//3.拿锁的阻塞函数与不阻塞函数
//二、主要学习的函数
//1.pthread_mutex_init(&mutex, NULL);//初始化互斥锁
//2.pthread_mutex_destroy(&mutex);//销毁互斥锁
//3.pthread_mutex_lock(&mutex);//当上个拿锁的没有解锁，其他线程会阻塞等待，
//4.pthread_mutex_trylock(&mutex);//当上个拿锁的没有解锁，其他线程会继续往下执行；
//5.pthread_mutex_unlock(&mutex);//解锁
//三、什么时候用互斥锁
//1.操作共享资源，要防止同时去更改共有资源
//四、有关本程序按下ctrl+C信号的现象的一些解释
//1.第一次按下ctrl+C，main这条主线程被拿锁卡住了，因为锁被三条线程中的一条拿着
//2.第二次按下ctrl+C,如果正好是正在拿锁的线程去执行这个指令，那么程序陷入阻塞；如不是，则可能是没拿锁的另外两条线程之一去执行了这个指令，那么
//该线程陷入阻塞，但是对拿锁的线程没影响，所以程序不会阻塞。（以此类推，反正第四次按下ctrl+C程序肯定陷入阻塞）
//********************************************************************************************************************************

int count=1000;

pthread_mutex_t mutex;

void *new_thread(void *arg)
{
	int retval;

    while(1)
    {
        pthread_mutex_lock(&mutex);//当上个拿锁的没有解锁，其他线程会阻塞等待，
        //pthread_mutex_trylock(&mutex);//当上个拿锁的没有解锁，其他线程会继续往下执行；
        
        printf("线程%s拿锁成功\n", (char*)arg);
        
        if(count >= 100)
        {
            sleep(1);
            count -=100;
        }
        else
        {
            printf("线程%s：老登请充钱\n", (char*)arg);
        }
        
        printf("线程%s还锁成功\n",(char*)arg);
        pthread_mutex_unlock(&mutex);
        // printf("线程%s:count=%d\n", (char*)arg, count);
        sleep(1);
    }
	return NULL;
}

void signal_sigint_func(int signum)
{
    
  	pthread_mutex_lock(&mutex);//上锁操作拥有阻塞特性
	printf("father拿锁成功\n");
	
		sleep(3);
		count +=1000;
	
	printf("father还锁成功\n");
	pthread_mutex_unlock(&mutex);//解锁
    printf("father:count=%d\n", count);

    
}


int main(void)
{
	pthread_t tid[3];

	int retval;
	signal(SIGINT, signal_sigint_func);
	pthread_mutex_init(&mutex, NULL);//初始化互斥锁
	

	retval = pthread_create(tid, NULL, new_thread, "0");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程0失败:%s\n", strerror(retval));
		return -1;
	}

	retval = pthread_create(tid+1, NULL, new_thread, "1");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程1失败:%s\n", strerror(retval));
		return -1;
	}

    retval = pthread_create(tid+2, NULL, new_thread, "2");
	if(retval != 0)
	{
		fprintf(stderr, "创建线程2失败:%s\n", strerror(retval));
		return -1;
	}
	
	pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
	
	pthread_mutex_destroy(&mutex);//销毁互斥锁
	


	return 0;
}




