#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//******************************************************************************************************************************
//一、本节主要是学习线程的取消函数
//1.线程在收到取消时，可以调用函数来实现让线程不取消，或者实现延时取消操作
//2.让线程可以不用遇到取消点（一些函数）就可以被马上取消
//3.  void *retp;   pthread_join(tid, &retp);//如果线程被取消，retp 是 PTHREAD_CANCELED;如果正常退出是线程返回值（“yyds”）
//4.如果线程的返回值不为0，则说明线程创建失败
//5.pthread_cancel(tid);函数的取消信号具有滞后性（不是一发送就没了，但没法被取消时，可以等待）
//二、主要学习的函数
//1.pthread_create(&tid, NULL, new_thread, "hello");//线程的创建函数，第二个如果不需要设置线程的属性则位NULL,第四个为线程执行函数的参数，没有可以填NULL
//2.pthread_cancel(tid);//发送取消请求
//3.pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);//让线程无法被取消
//4.pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);//让线程可以被取消
//5. pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//让线程可以不用遇到取消点（一些函数）就可以被马上取消
//6.pthread_join(tid, &retp);//线程的接合/等待函数，等待线程结束
//三、什么时候能用这个代码例子
//1.线程在收到取消时，可以调用函数来实现让线程不取消，或者实现延时取消操作
//********************************************************************************************************************************

void *new_thread(void *arg)
{
	char *str = arg;
	int i= 1;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);//让线程无法被取消

	while(i <= 10)
	{
        
		sleep(1);
		printf("现在是第%d秒：%s\n", i++, str);
        if(i > 5)
        {
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);//让线程可以被取消
            pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//让线程可以不用遇到取消点（一些函数）就可以被马上取消
        }


	}
    //pthread_exit("yyds");//与return的效果一样
	
	 return "yyds";
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
	sleep(2);//当小于5秒发送取消时，，要延时到5秒才能响应取消
	pthread_cancel(tid);//发送取消请求
	pthread_join(tid, &retp);//线程的接合/等待函数
    if (retp == PTHREAD_CANCELED) //如果线程被取消，retp 是 PTHREAD_CANCELED;如果正常退出是线程返回值（“yyds”）
    {
        printf("线程被取消\n");
    } 
    else
    {
        printf("接合成功：%s\n", (char *)retp);
    }


	return 0;
}
