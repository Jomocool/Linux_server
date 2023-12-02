#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// 自己给自己发送一个信号
int main(void)
{
    int i = 1;

    while (1)
    {
        printf("do working %d\n", i);

        if (i == 4)
        {
            // 自己给自己发送一个信号
            printf("自己给自己发送信号...\n");
            raise(SIGTERM);
        }

        i++;
        sleep(1);
    }

    return 0;
}