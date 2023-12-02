#include <stdio.h>

void fun(void)
{
	int i = 0;
	for (i=0; i<10;i++)
	{
		printf("func==> i = %d\n",i);
	}
}

int main(int argc, char *argv[])
{
	int i = 0;
	
	// 将传入的参数全部输出
	for (i = 0; i<argc; i++)
	{
		printf("argv[%d]: %s\n",i,argv[i]);
	}

	fun();

	printf("hello itcast\n");

	return 0;
}
