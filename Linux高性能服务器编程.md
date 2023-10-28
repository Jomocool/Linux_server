## Linux高性能服务器编程

## 1. GCC编译器简洁

![image-20231028131250285](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231028131250285.png)

c语言编译成可执行文件的步骤：

![image-20231028131628943](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231028131628943.png)



编写一个c程序：

测试程序（hello.c）如下：

```c
#include<stdio.h>
int main(void)
{
        printf("Hello world!\n");
        return 0;
}
```

> #1.生成预处理文件
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc -E hello.c -o hello.i
> #2.生成汇编文件
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc -S hello.i -o hello.s
> #3.生成目标代码
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc -c hello.s -o hello.o
> #4.生成可执行文件
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc hello.o -o hello
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ./hello
> Hello world!
>
> 一步到位：一条指令完成四步操作：预处理、汇编、二进制文件、链接生成可执行文件
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc hello.c -o hello
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ls
> hello  hello.c  hello.i  hello.o  hello.s
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ./hello
> Hello world!



## 2. GCC工作流程和常用选项

| 选项           | 作用                        |
| -------------- | --------------------------- |
| -o file        | 指定生成的输出文件名为file  |
| -E             | 只进行预处理                |
| -S             | 只进行预处理和编译          |
| -c             | 只进行预处理、编译和汇编    |
| -v / --version | 查看gcc版本号               |
| -g             | 包含调试信息                |
| -On    n=0~3   | 编译优化，n越大编译级别越高 |
| -Wall          | 提示更多警告信息            |
| -D             | 编译时定义宏                |



**测试 gcc -Wall:**

测试程序（warn.c）如下：

```c
#include<stdio.h>

int main(void)
{
        int a=3;//用不到的值

        printf("Hello warn\n");
        return 0;
}
```

> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc warn.c 
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ./a.out 
> Hello warn
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc -Wall warn.c 
> warn.c: In function ‘main’:
> warn.c:5:13: warning: unused variable ‘a’ [-Wunused-variable]
>     5 |         int a=3;//用不到的值
>       |  
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc -Wall -Werror warn.c 
> warn.c: In function ‘main’:
> warn.c:5:13: error: unused variable ‘a’ [-Werror=unused-variable]
>     5 |         int a=3;//用不到的值
>       |             ^
> cc1: all warnings being treated as errors

**测试gcc file -D:**

测试程序（warn.c)如下：

```c
#include<stdio.h>

int main(void)
{
        int a=3;//用不到的值

#ifdef DEBUG //如果定义宏DEBUG，则打印
        printf("Hello warn\n");
#endif
        printf("helllo world\n");

        return 0;
}
```

> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc warn.c 
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ./a.out 
> helllo world
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc warn.c -DDEBUG
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ./a.out 
> Hello warn
> helllo world



## 3. 静态链接和动态链接

链接分为两种：**静态链接**、**动态链接**

1. 静态链接：由链接器在链接时将库的内容加入到可执行程序中

   优点：

   - 对运行环境的依赖性较小，具有较好的兼容性

   缺点：

   - 生成的程序比较大，需要更多的系统资源，在装入内存时会消耗更多的时间
   - 库函数有了更新，必须重新编译应用程序

2. 动态链接：链接器在联结时仅仅建立与所需库函数的之间的链接关系，在程序运行时才将所需要资源调入可执行程序

   优点：

   - 在需要的时候才会调入对应的资源函数
   - 简化程序的升级；有着较小的程序体积
   - 实现进程之间的资源共享（避免重复拷贝）

   缺点：

   - 依赖动态库，不能独立运行
   - 动态库依赖版本问题严重

3. 静态、动态编译对比

   测试程序（hello.c）如下：

   ```c
   #include<stdio.h>
   
   int main(void)
   {
           printf("Hello world!\n");
           return 0;
   }
   ```

   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc hello.c -o hello_share
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ gcc -static hello.c -o hello_static
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ls
   > hello.c  hello_share  hello_static
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ./hello_static 
   > Hello world!
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ./hello_share 
   > Hello world!
   >
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn$ ls -lh hello*
   > -rw-rw-r-- 1 jomo jomo   76 10月 28 13:23 hello.c
   > -rwxrwxr-x 1 jomo jomo  16K 10月 28 20:30 hello_share
   > -rwxrwxr-x 1 jomo jomo 880K 10月 28 20:30 hello_static



## 4. 静态库和动态库简介

#include <file_name> 和 #include"file_name"的区别：

- <file_name>: 直接去标准库中搜索
- "file_name": 先在当前目录搜索，再去标准库搜索



add.h

```c
#ifndef __ADD_H__
#define __ADD_H__

int add(int x,int y);

#endif /*__ADD_H__*/
```

add.c

```c
#include "add.h"
int add(int x,int y)
{
        return x+y;
}
```

sub.h

```c
#ifndef __SUB_H__
#define __SUB_H__

int sub(int x,int y);

#endif /*__SUB_H__*/
```

sub.c

```c
#include "sub.h"
int sub(int x,int y)
{
        return x-y;
}
```

mul.h

```c
#ifndef __MUL_H__
#define __MUL_H__

int mul(int x,int y);

#endif /*__MUL_H__*/
```

mul.c

```c
#include "mul.h"
int mul(int x,int y)
{
        return x*y;
}
```

div.h

```c
#ifndef __DIV_H__
#define __DIV_H__

int div(int x,int y);

#endif /*__DIV_H__*/
```

div.c

```c
#include "div.h"
int div(int x,int y)
{
        return x/y;
}
```

test.c

```c
#include<stdio.h>
#include "add.h"
#include "sub.h"
#include "mul.h"
#include "div.h"

int main(void)
{
        int x=15;
        int y=5;
        printf("x + y = %d\n", add(x,y));
        printf("x - y = %d\n", sub(x,y));
        printf("x * y = %d\n", mul(x,y));
        printf("x / y = %d\n", div(x,y));
        return 0;
}
```

> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ gcc *.c
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ ./a.out 
> x + y = 20
> x - y = 10
> x * y = 75
> x / y = 3



**静态库的制作：**

![image-20231028205844934](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231028205844934.png)

![image-20231028205955383](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231028205955383.png)

步骤1：将c源文件生成对应的.o文件

> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ gcc -c add.c -o add.o
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ gcc -c sub.c -o sub.o
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ gcc -c mul.c -o mul.o
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ gcc -c div.c -o div.o
>
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ ls -l *.o
> -rw-rw-r-- 1 jomo jomo 1232 10月 28 21:01 add.o
> -rw-rw-r-- 1 jomo jomo 1232 10月 28 21:01 div.o
> -rw-rw-r-- 1 jomo jomo 1232 10月 28 21:01 mul.o
> -rw-rw-r-- 1 jomo jomo 1232 10月 28 21:01 sub.o

步骤2：使用打包工具ar将准备好的.o文件打包为.a文件libtest.a

> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ ar -rcs libtest.a add.o sub.o mul.o div.o
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_lib$ ls
> add.c  add.h  add.o  div.c  div.h  div.o  libtest.a  mul.c  mul.h  mul.o  sub.c  sub.h  sub.o  test.c

**在使用ar工具时需要添加参数：rcs**

- r：更新
- c：创建
- s：建立索引



**静态库使用：**

静态库制作完成之后，需要将.a文件和头文件一起发布给用户

假设测试文件是test.c，静态库文件为libtest.a，头文件是*.h

编译命令：

> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_test$ ls
> add.h  div.h  libtest.a  mul.h  sub.h  test.c
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_test$ gcc test.c -I./ -L./ -ltest
> jomo@jomo-virtual-machine:~/linux_server/gcc_learn/static_test$ ./a.out
> x + y = 20
> x - y = 10
> x * y = 75
> x / y = 3

参数说明：

- -I(大写i): 指定头文件所在目录

- -L: 表示要连接的库所在目录（.a文件）
- -l(小写L): 指定链接时需要的库，去掉前缀和后缀 

![image-20231028223943016](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231028223943016.png)
