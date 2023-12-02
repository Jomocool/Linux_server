#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// 等待子进程退出
int main(int argc, char *argv[])
{
    int status = 0;
    int i = 0;
    pid_t pid = -1;
    int ret = -1;

    //创建子进程
    pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        return 1;
    }

    //子进程
    if (0 == pid){
        for (i=0;i<10;i++){
            printf("child process %d do %d\n",getpid(),i+1);
            sleep(1);
        }

        //子进程终止
        exit(10);
    }

    //父进程执行
    printf("父进程等待子进程退出，回收其资源\n");
    ret = wait(&status);
    if(-1 == ret){
        perror("wait");
        return 1;
    }

    printf("父进程回收了子进程资源...\n");

    //属于正常退出
    if (WIFEXITED(status))
    {
        printf("子进程正常退出状态码：%d\n",WEXITSTATUS(status));
    }
    else if(WIFSIGNALED(status))
    {
        printf("子进程被信号%d杀死了...\n",WTERMSIG(status));
    }
    else if(WSTOPSIG(status))
    {
        printf("子进程被信号%d暂停了...\n",WSTOPSIG(status));
    }

    return 0;
}
