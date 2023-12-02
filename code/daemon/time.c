#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#define SIZE 32

int main(void)
{
    time_t t = -1;

    struct tm *pT = NULL;

    char file_name[SIZE];

    // 获取时间 秒
    t = time(NULL);
    if (-1 == t)
    {
        perror("time");
        return 1;
    }
    printf("t: %ld\n", t);

    printf("ctime: %s\n", ctime(&t));

    // 转化为时间
    pT = localtime(&t);
    if (NULL == pT)
    {
        printf("local time failed...\n");
    }

    printf("year: %d\n", pT->tm_year + 1900);
    printf("month: %d\n", pT->tm_mon + 1);
    printf("day: %d\n", pT->tm_mday);
    printf("hout: %d\n", pT->tm_hour);
    printf("min: %d\n", pT->tm_min);
    printf("sec: %d\n", pT->tm_sec);

    // 转化为文件名
    memset(file_name, 0, SIZE);
    sprintf(file_name, "%s %d%d%d%d%d%d.log", "touch", pT->tm_year + 1900, pT->tm_mon, pT->tm_mday, pT->tm_hour, pT->tm_min, pT->tm_sec);

    printf("file_name: %s\n", file_name);

    system(file_name);

    return 0;
}