#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// 创建守护进程
int main(void)
{
    pid_t pid = -1;
    int ret = -1;
    // 1.创建子进程 父进程退出
    pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        return 1;
    }
    if (pid > 0)
    {
        // 父进程退出，此时子进程成为孤儿进程，被1号进程收养
        exit(0);
    }

    // 接下里都是子进程执行

    // 2.创建新的会话 完全脱离控制终端（运行之后，即使关闭当前会话终端，子进程也不会终止）
    pid = setsid();
    if (-1 == pid)
    {
        perror("setsid");
        return 1;
    }

    // 3.改变当前工作目录
    ret = chdir("/");
    if (-1 == ret)
    {
        perror("chdir");
        return 1;
    }

    // 4.设置权限掩码 不屏蔽任何权限
    umask(0);

    // 5.关闭文件描述符
    close(STDIN_FILENO);  // 关闭标准输入
    close(STDOUT_FILENO); // 关闭标准输出
    close(STDERR_FILENO); // 关闭标准错误输出

    // 6.执行核心的任务
    // 每隔1分钟输出当前时间到/tmp/txt.log文件中
    while (1)
    {
        system("date >> /tmp/txt.log"); // 像终端一样执行命令行
        sleep(60);
    }

    return 0;
}