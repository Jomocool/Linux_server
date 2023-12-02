#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include <unistd.h>

// 写文件
int main(int argc, char *argv[])
{
    int fd=-1;
    int ret=1;

    char*str = "hello itcast";

    // 1.以只写的方式打开一个文件
    fd=open("txt",O_WRONLY | O_CREAT,0644);
    if(-1 == fd)
    {
        perror("open");
        return 1;
    }

    printf("fd=%d\n",fd);

    // 2.写文件
    ret = write(fd,str,strlen(str));
    if (-1==ret){
        perror("write");
        return 1;
    }
    printf("write len: %d\n",ret);

    // 3.关闭文件
    close(fd);

    return 0;
}
