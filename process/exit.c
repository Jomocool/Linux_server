#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// 结束进程
int main(int argc, char *argv[])
{
    printf("hello world");

    // 等价于return 0
    // exit(0); 正常输出hello world
    _exit(0); // 无法正常输出hello world，因为直接退出，不做任何清理工作
}
