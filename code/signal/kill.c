#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

// 父进程杀死子进程
int main(void)
{
    pid_t pid = -1;

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
        while (1)
        {
            printf("child process do work...\n");
            sleep(1);
        }

        // 进程退出
        exit(0);
    }
    // 父进程
    else
    {
        sleep(3);
        printf("子进程该退出了...\n");
        kill(pid, SIGKILL);
        printf("父进程完成任务，该结束了...\n");
    }

    return 0;
}