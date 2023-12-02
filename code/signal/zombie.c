#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

int main(void)
{
    pid_t pid = -1;

    // 忽略子进程退出信号的信号
    // 那么子进程结束后，内核会回收，并不再给父进程发送信号
    signal(SIGCHLD, SIG_IGN);

    // 创建一个子进程
    pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        return 1;
    }

    // 子进程
    if (0 == pid)
    {
        printf("子进程休息2s...\n");
        sleep(2);
        printf("子进程退出...\n");
        exit(0);
    }
    else
    {
        // 父进程
        while (1)
        {
            printf("父进程do working...\n");
            sleep(1);
        }
    }

    return 0;
}