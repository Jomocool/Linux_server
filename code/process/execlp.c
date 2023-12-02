#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    printf("Hello itcast\n");

    // arg0 arg1 arg2 ... argn
    // arg0一般是可执行文件名 argn必须是NULL
    // 等价于执行ls -l /home
    execlp("ls", "ls", "-l", "/home", NULL);

    printf("Hello world\n");

    return 0;
}