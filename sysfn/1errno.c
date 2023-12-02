#include <stdio.h>
#include<errno.h>
#include<string.h>

// errno是一个全局变量，在errno.h头文件中有定义
// errno是保存系统最近出错错误码
int main(int argc, char *argv[])
{
    // 打开文件（只读）
    FILE *fp = fopen("txt","r");
    if (NULL == fp)
    {
        printf("fopen failed....\n");
        printf("errno: %d\n",errno);
        // 根据errno的值，解析出错原因
        printf("fopen: %s\n",strerror(errno));

        // 根据errno值输出错误信息
        // 提示字符串：出错原因
        perror("fopen");

        return 1;
    }

    return 0;
}
