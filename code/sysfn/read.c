#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include <unistd.h>

#define SIZE 128

// 读文件
int main(int argc, char *argv[])
{
    int fd=-1;
    int ret=1;

    char buf[SIZE];

    // 1.以只读的方式打开一个文件
    fd=open("txt",O_RDONLY);
    if(-1 == fd)
    {
        perror("open");
        return 1;
    }

    printf("fd=%d\n",fd);

    // 2.读文件
    memset(buf,0,SIZE);
    ret = read(fd,buf,SIZE);
    if (-1==ret){
        perror("read");
        return 1;
    }
    printf("read len: %d %s\n",ret,buf);

    // 3.关闭文件
    close(fd);

    return 0;
}
