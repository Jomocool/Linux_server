#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    printf("Hello itcast\n");

    // arg0：可执行文件的绝对路径或者相对路径
    // arg1：可执行文件的名字
    // arg2~arg(n-1)：可执行文件的参数
    // argn：必须是NULL
    execl("/bin/ls", "ls", "-l", "/home", NULL);

    printf("Hello world\n");

    return 0;
}