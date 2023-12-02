#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 全局变量 数据段
int num = 100;

// 父子进程地址空间
int main(int argc, char *argv[])
{
    int var = 88;
    pid_t pid = -1;
    int *p =NULL;
    
    // 在堆区分配内存空间
    p = malloc(sizeof(int));
    if(NULL == p)
    {
        printf("malloc failed...\n");
        return 1;    
    }
    memset(p,0,sizeof(int));

    *p = 200;

    // 创建一个子进程
    pid = fork();
    if(-1 == pid)
    {
        perror("fork");
        return 1;
    }

    if (0==pid)
    {
        // 子进程
        sleep(2);
        printf("子进程睡醒之后 *p = %d num = %d  var = %d\n",*p,num,var);
        free(p);
        p=NULL;
    }
    else
    {
        // 父进程
        printf("父进程之前 *p = %d num = %d  var = %d\n",*p,num,var);
        var++;
        num++;
        (*p)++;
        printf("父进程之后 *p = %d num = %d  var = %d\n",*p,num,var);
        free(p);
        p=NULL;
    }

    return 0;
}
