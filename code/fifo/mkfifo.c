#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// 通过mkfifo函数创建一个管道文件
int main(void)
{
    int ret = -1;

    // 可以用access函数判断文件是否已经存在
    // 创建一个命名管道，管道的名字：fifo
    ret = mkfifo("./fifo1", 0644);
    if (-1 == ret)
    {
        perror("mkfifo");
        return 1;
    }

    printf("创建一个命名管道ok...\n");

    return 0;
}