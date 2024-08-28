#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void func1(void)
{
	printf("跑路啦1\n");
}

void func2(void)
{
	printf("跑路啦2\n");
}

void func3(void)
{
	printf("跑路啦3\n");
}

void my_on_exit(int status, void *arg)
{
	printf("status=%d, arg=%s\n", status, (char *)arg);
}

int main(void)
{

	atexit(func1);//注册，当进程退出的时候要去运行什么内容
	atexit(func2);
	atexit(func3);

	on_exit(my_on_exit, "我爱你");

	printf("嘻嘻");

	_exit(0);

	return 0;//仅代表main函数结束,去执行一段特殊代码，然后再清空缓冲区
}
