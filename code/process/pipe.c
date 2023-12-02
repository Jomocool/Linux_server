#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 64

// 用于创建无名管道，父子进程使用无名管道进行通信
// 父进程写管道 子进程读管道
int main(void)
{
    int fd[2];
    int ret = -1;
    char buf[SIZE];
    pid_t pid = -1;

    // 1.创建一个无名管道
    ret = pipe(fd);
    if (-1 == ret)
    {
        perror("pipe");
        return 1;
    }

    // 一定要在创建子进程前创建无名管道，这样父子进程共享的才是同一个管道
    // 2.创建子进程
    pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        return 1;
    }

    // 子进程 读
    if (0 == pid)
    {
        // 关闭写端
        close(fd[1]);

        memset(buf, 0, SIZE);
        ret = read(fd[0], buf, SIZE);
        if (ret < 0)
        {
            perror("read");
            exit(-1);
        }

        printf("chilid process read buf: %s\n", buf);

        // 关闭读端
        close(fd[0]);
        // 进程退出
        exit(0);
    }
    // 父进程 写
    else
    {
        // 关闭读端
        close(fd[0]);

        ret = write(fd[1], "ABCDEFGHIJ", 10);
        if (ret < 0)
        {
            perror("write");
            return 1;
        }

        printf("parent process write len :%d\n", 10);

        // 关闭写端
        close(fd[1]);
    }

    return 0;
}