#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 128

// 先写后读
int main(void)
{
    int fdr = -1;
    int fdw = -1;
    char buf[SIZE];
    int ret = -1;
    // 以只写的方式打开fifo1
    fdw = open("fifo1", O_WRONLY);
    if (-1 == fdw)
    {
        perror("open");
        return 1;
    }

    printf("以只写的方式打开fifo1...OK\n");

    // 以只读的方式打开fifo2
    fdr = open("fifo2", O_RDONLY);
    if (-1 == fdr)
    {
        perror("open");
        return 1;
    }
    printf("以只读的方式打开fifo2...OK\n");

    // 循环读写
    while (1)
    {
        // 写fifo1
        memset(buf, 0, SIZE);
        fgets(buf, SIZE, stdin);
        // 去掉最后一个换行符
        if ('\n' == buf[strlen(buf) - 1])
        {
            buf[strlen(buf) - 1] = '\0';
        }
        ret = write(fdw, buf, strlen(buf));
        if (ret <= 0)
        {
            perror("write");
            break;
        }

        printf("Send to A ret: %d\n", ret);

        // 读fifo2
        memset(buf, 0, SIZE);
        ret = read(fdr, buf, SIZE);
        if (ret <= 0)
        {
            perror("read");
            break;
        }
        printf("Received from A: %s\n", buf);
    }

    // 关闭文件描述符
    close(fdr);
    close(fdw);

    return 0;
}