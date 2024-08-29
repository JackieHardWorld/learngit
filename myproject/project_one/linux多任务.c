#include <stdio.h>
// 线程的头文件
#include <pthread.h>

// 延时函数头文件
#include <unistd.h>

void *task(void *arg) // 任务函数，必须这样定义！！
{
    int j = 0;
    while (1)
    {
       
        
        printf("j=%d\n", j++);
        sleep(1); // 延时1S
    }
}

void *task1(void *arg) // 任务函数，必须这样定义！！
{
    int k = 0;
    while (1)
    {
       
        
        printf("k=%d\n", k++);
        sleep(1); // 延时1S
    }
}



int main()
{

    // 创建一个线程执行一个循环任务
    pthread_t tid;
    pthread_create(&tid, NULL, task, NULL); // 线程创建后，就会执行

     pthread_t tid1;
    pthread_create(&tid1, NULL, task1, NULL); // 线程创建后，就会执行
  
    // task 任务函数

    // main函数就执行其他循环任务
    int i = 0;
    while (1)
    {
        printf("i=%d\n", i++);
        sleep(1); // 延时1S
    }
}