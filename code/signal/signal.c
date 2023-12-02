#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// 信号处理函数1
void fun1(int signum)
{
    printf("捕捉到信号：%d\n", signum);
}

// 信号处理函数2
void fun2(int signum)
{
    printf("捕捉到信号：%d\n", signum);
}

// 信号注册函数
int main(void)
{
    // 信号注册
    // Ctrl + 'c'
    signal(SIGINT, fun1);// （异步）相当于只要来了SIGINT信号，就会执行fun1函数（信号处理函数）

    // Ctrl + '\'
    signal(SIGQUIT, fun2);

    while (1)
    {
        sleep(1);
    }

    return 0;
}