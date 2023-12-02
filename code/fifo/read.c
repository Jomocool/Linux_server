#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 128

// 读管道
int main(void)
{
    int fd = -1;
    char buf[SIZE];
    int ret = -1;

    // 1.以只读的方式打开一个管道文件
    fd = open("fifo", O_RDONLY);
    if (-1 == fd)
    {
        perror("open");
        return 1;
    }
    printf("以只读的方式打开一个管道...OK\n");

    // 2.循环读管道
    while (1)
    {
        memset(buf, 0, SIZE);
        ret = read(fd, buf, SIZE); // 不需要sleep，因为如果没有数据可读就会阻塞

        // 如果写端是关闭的，则ret = 0
        if (ret <= 0)
        {
            perror("read");
            break;
        }
        printf("buf: %s\n", buf);
    }

    // 3.关闭文件
    close(fd);

    return 0;
}