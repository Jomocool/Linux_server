#include <stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include <unistd.h>

// 打开和关闭文件
int main(int argc, char *argv[])
{
    int fd = -1;
    // 以只读的方式打开一个文件，如果文件不存在就报错
    fd = open("txt",O_RDONLY);
    if (-1 == fd)
    {
        perror("open");
        return 1;
    }

    printf("fd = %d\n",fd);

    // 关闭文件
    close(fd);
    return 0;
}
