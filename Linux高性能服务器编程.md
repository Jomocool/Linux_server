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



## 5. 动态库制作和使用

![image-20231030001622934](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231030001622934.png)

1. **动态库制作：**

   步骤1：生成目标文件，此时要加编译选项：-fPIC(fpic)

   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_lib$ ls
   > add.c  add.h  div.c  div.h  mul.c  mul.h  sub.c  sub.h  test.c
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_lib$ gcc -fPIC -c *.c
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_lib$ ls
   > add.c  add.h  add.o  div.c  div.h  div.o  mul.c  mul.h  mul.o  sub.c  sub.h  sub.o  test.c  test.o

   参数：-fPIC创建与地质无关的编译程序（pic，position independent code），是为了能够在多个应用程序间共享

   步骤2：生成共享库，此时要加链接器选项：-shared（指定生成动态链接库）

   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_lib$ gcc -fPIC -c *.c
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_lib$ ls
   > add.c  add.h  add.o  div.c  div.h  div.o  mul.c  mul.h  mul.o  sub.c  sub.h  sub.o  test.c  test.o
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_lib$ gcc -shared *.o -o libaa.so
   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_lib$ ls
   > add.c  add.h  add.o  div.c  div.h  div.o  libaa.so  mul.c  mul.h  mul.o  sub.c  sub.h  sub.o  test.c  test.o

   步骤3：通过nm命令查看对应的函数

   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_lib$ nm libaa.so
   > 0000000000001199 T add
   > 0000000000004048 b completed.0
   >                  w __cxa_finalize@GLIBC_2.2.5
   > 00000000000010e0 t deregister_tm_clones
   > 00000000000011b1 T div
   > 0000000000001150 t __do_global_dtors_aux
   > 0000000000003e18 d __do_global_dtors_aux_fini_array_entry
   > 0000000000004040 d __dso_handle
   > 0000000000003e20 d _DYNAMIC
   > 00000000000012ac t _fini
   > 0000000000001190 t frame_dummy
   > 0000000000003e10 d __frame_dummy_init_array_entry
   > 0000000000002190 r __FRAME_END__
   > 0000000000004000 d _GLOBAL_OFFSET_TABLE_
   >                  w __gmon_start__
   > 0000000000002030 r __GNU_EH_FRAME_HDR
   > 0000000000001000 t _init
   >                  w _ITM_deregisterTMCloneTable
   >                  w _ITM_registerTMCloneTable
   > 00000000000011f5 T main
   > 00000000000011c8 T mul
   >                  U printf@GLIBC_2.2.5
   > 0000000000001110 t register_tm_clones
   > 00000000000011df T sub
   > 0000000000004048 d __TMC_END__

2. **动态库测试：**

   > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ gcc test.c -L. -I. -laa

   报错：

   ![image-20231030223342495](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231030223342495.png)

   原因：

   - 当系统加载可执行代码时，能够知道其所依赖的库的名字，但是还需要知道绝对路径。此时就需要系统动态载入器（dynamic linker/loader）
   - 对于elf格式的可执行程序，是由ld-linux.so*来完成的，它先后搜索elf文件的DT_RPATH段——环境变量LD_LIBRARY_PATH —— /etc/ld.so.cache文件列表 —— /lib/,/user/lib目录找到库文件后将其载入内存

3. **如何让系统找到动态库：**

   - 拷贝自己制作的共享库到/lib或者/usr/lib（不能是/lib64目录）

     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ sudo cp libaa.so /lib
     >
     > #不用指明链接库的目录了，因为会默认从/lib或/usr/lib/目录下去找同名库
     >
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ gcc test.c  -I. -laa
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ ./a.out 
     > x + y = 20
     > x - y = 10
     > x * y = 75
     > x / y = 3
     
   - 临时设置LD_LIBRARY_PATH:
   
     > export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:库路径
   
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/jomo/linux_server/gcc_learn/share_test
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ ./a.out 
     > x + y = 20
     > x - y = 10
     > x * y = 75
     > x / y = 3
   
   - 永久设置
   
     > #追加export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/jomo/linux_server/gcc_learn/share_test到文件末尾
     >
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ vim ~/.bashrc 
     >
     > #使环境变量生效
     >
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ source ~/.bashrc 
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ ./a.out 
     > x + y = 20
     > x - y = 10
     > x * y = 75
     > x / y = 3
   
   - 将其添加到/etc/ld.so.conf文件中
   
     编辑/etc/ld.so.conf文件，加入库文件所在目录的路径
   
     运行sudo ldconfig -v，该命令会重建/etc/ld.so.cache文件
   
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ sudo vim /etc/ld.so.conf
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ echo $LD_LIBRARY_PATH 
     > :/home/jomo/linux_server/gcc_learn/share_test:/home/jomo/linux_server/gcc_learn/share_test
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ unset LD_LIBRARY_PATH 
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ echo $LD_LIBRARY_PATH 
     >
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ ./a.out 
     > ./a.out: error while loading shared libraries: libaa.so: cannot open shared object file: No such file or directory
     >
     > #使生效
     >
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ sudo ldconfig -v
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ ./a.out 
     > x + y = 20
     > x - y = 10
     > x * y = 75
     > x / y = 3
   
   - 使用符号链接，但是一定要使用绝对路径
   
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ sudo ln -s /home/jomo/linux_server/gcc_learn/share_test/libaa.so /lib/libaa.so
     > jomo@jomo-virtual-machine:~/linux_server/gcc_learn/share_test$ ./a.out 
     > x + y = 20
     > x - y = 10
     > x * y = 75
     > x / y = 3



## 6. GDB调试器

**GDB简介：**

![image-20231104122445761](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104122445761.png)

**生成调试信息**

GDB主要是调试C/C++程序。要调试C/C++程序，首先在编译时，必须要把调试信息加到可执行文件中。使用编译器（cc/gcc/g++）的-g参数可以做到这一点。如：

> gcc -g hello.c -o hello
>
> g++ -g hello.cpp -o hello

如果没有-g，你将看不见程序的函数名、变量名，所代替的全是运行时的内存地址



**启动GDB**

测试程序：test.c

```c
#include <stdio.h>

void fun(void)
{
        int i = 0;
        for (i=0; i<10;i++)
        {
                printf("func==> i = %d\n",i);
        }
}

int main(int argc, char *argv[])
{
        int i = 0;

        // 将传入的参数全部输出
        for (i = 0; i<argc; i++)
        {
                printf("argv[%d]: %s\n",i,argv[i]);
        }

        fun();

        printf("hello itcast\n");

        return 0;
}
```



- 启动gdb：gdb program

  program就是你的执行文件，一般在当前目录下

  ![image-20231104125832965](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104125832965.png)

- 设置运行参数

  set args 可指定运行时参数。（如：set args 10 20 30 40 50)

  show args 可以查看设置好的运行参数

  ![image-20231104130541914](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104130541914.png)

- 启动程序

  run：程序开始执行，如果有断点，停在第一个断点处

  start：程序向下执行一行

  ![image-20231104130606975](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104130606975.png)

  ![image-20231104130631502](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104130631502.png)

- 退出gdb：quit

**显示源代码**

用list命令来打印程序的源代码。默认打印0行

- list linenum：打印第linenum行的上下文内容

- list function：显示函数名为function的函数的源程序
- list：显示当前行后面的源程序
- list -：显示当前行前面的源程序

一般是打印当前行的上5行和下5行，如果显示函数是上2行下8行，默认10行。也可以定制显示范围：

- set listsize count：设置一次显示源代码的行数
- show listsize：查看当前listsize的设置

![image-20231104131622353](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104131622353.png)

![image-20231104131729450](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104131729450.png)



**断点操作**

- 查看断点：

  - info break
  - info b
  - i break
  - i b

  ![image-20231104132259611](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104132259611.png)

  Num：断点编号

  Type：断点类型

  Disp：显示

  Enb：enable，使能

  Address：地址

  What：相关信息：（函数、文件、行号）

- 简单断点：

  break 设置断点，可以简写为b

  - b 10：设置断点，在源程序第10行
  - b func：设置断点，在func函数如果处

- 多文件设置断点

  - break filename:linenum -- 在源文件filename的linenum行设断点
  - break finename:function -- 在源文件的filen的function函数入口处设断点
  - break class::function或(function(type,type)) -- 在类class的function函数的入口处设设断点（考虑到了函数重载）
  - break namespace::class::function -- 在名称空间为namespace的类class的function函数的入口处设断点

**条件断点**

一般来说，为断点设置一个条件，使用if关键词，后面紧跟断点条件

设置一个条件断点：

> b test.c:8 if Value == 5

![image-20231104150015485](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104150015485.png)



**维护断点**

![image-20231104150130853](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104150130853.png)

![image-20231104150311172](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104150311172.png)



**调试代码**

![image-20231104150424205](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104150424205.png)



**自动显示**

![image-20231104150658031](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104150658031.png)



**查看修改变量的值**

![image-20231104151016975](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104151016975.png)



## 7. Makefile

**Makefile简介**

![image-20231104175113716](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104175113716.png)



**make主要解决的问题**

![image-20231104175455155](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231104175455155.png)



**Makefile文件命名规则**

makefile 和 Makefile都可以，推荐使用Makefile



**make工具安装**

> sudo apt install make



**Makefile语法规则**

- 一条规则：

  > 目标: 依赖文件列表
  >
  > <Tab>命令列表

  Makefile基本规则三要素：

  1. 目标：

     通常是要产生的文件名称，目标可以是可执行文件或者其它obj文件，也可以是一个动作的名称

  2. 依赖文件：

     用来输入从而产生目标的文件

     一个目标通常有几个依赖文件（可以没有）

  3. 命令：

     make执行的动作，一个规则可以含几个命令（可以没有）

     有多个命令时，每个命令占一行

  ```makefile
  #目标是all，依赖文件列表是test1、test2，如果test1和test2存在，就会执行echo "hello all"
  all:test1 test2
          echo "hello all"
  
  test1:
          echo "hello test1"
  
  test2:
          echo "hello test2"
  ```

  > jomo@jomo-virtual-machine:~/linux_server/makefile_learn$ make -f 1.mk
  > echo "hello test1"
  > hello test1
  > echo "hello test2"
  > hello test2
  > echo "hello all"
  > hello all

**make命令格式**

make是一个命令工具，它解释Makefile中的指令（规则）

> make \[-f file\]\[options\]\[targets\]

![image-20231105112821223](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105112821223.png)

![image-20231105113219778](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105113219778.png)

![image-20231105115300195](C:/Users/86135/AppData/Roaming/Typora/typora-user-images/image-20231105115300195.png)



**Makefile示例**

- 最简单的Makefile

  ```makefile
  test:add.c sub.c mul.c div.c test.c
  	gcc add.c sub.c mul.c div.c test.c -o test
  ```

  ![image-20231105115939327](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105115939327.png)

  **缺点：**效率低，修改一个文件，所有文件都要重新编译一次

- 第二个版本Makefile

  ```makefile
  #目标：test
  #依赖文件列表：add.o sub.o mul.o div.o test.o
  #命令：链接目标代码生成可执行文件
  test:add.o sub.o mul.o div.o test.o
          gcc add.o sub.o mul.o div.o test.o -o test
  
  add.o:add.c
          gcc -c add.c -o add.o
  
  sub.o:sub.c
          gcc -c sub.c -o sub.o
  
  mul.o:mul.c
          gcc -c mul.c -o mul.o
  
  div.o:div.c
          gcc -c div.c -o div.o
  
  test.0:test.c
  		gcc -c test.c -o test.o
  ```

  ![image-20231105122434843](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105122434843.png)

  只修改了add.c，因此第二次make后，只会执行add.c生成目标代码

  ![image-20231105122514204](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105122514204.png)

  

**Makefile中的变量**

Makefile使用变量类似宏定义，使用该变量，相当于内容替换，使用变量可以使Makefile易于维护，修改内容变得简单

- 自定义变量

  1. 定义变量方法：

     变量名=变量值

  2. 引用变量：

     $(变量名)或${变量名}

  3. makefile的变量名：

     - makefile可以以数字开头
     - 变量是大小写敏感的
     - 变量一般都在makefile的头部定义
     - 变量几乎可在makefile的任何地方使用 

  使用变量后的第二版Makefile

  ```makefile
  OBJS = add.o sub.o mul.o div.o test.o
  
  test:$(OBJS)
          gcc $(OBJS) -o test
  
  add.o:add.c
          gcc -c add.c -o add.o
  
  sub.o:sub.c
          gcc -c sub.c -o sub.o
  
  mul.o:mul.c
          gcc -c mul.c -o mul.o
  
  div.o:div.c
          gcc -c div.c -o div.o
  
  test.o:test.c
          gcc -c test.c -o test.o 
  
  clean:
  		rm -rf $(OBJS) test
  ```

  ![image-20231105122923505](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105122923505.png)

  

  make clean会执行Makefile中的clean目标，由于没有依赖，因此会直接执行命令，清除中间文件

  ![image-20231105123246919](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105123246919.png)

  

  另一个变量版本的Makefile

  ```makefile
  OBJS = add.o sub.o mul.o div.o test.o
  TARGET = test
  
  #这些变量不能单独使用，必须在命令中使用
  #$@ 表示目标
  #$^ 表示所有依赖
  #$< 表示第一个依赖
  
  $(TARGET):$(OBJS)
          gcc $^ -o $@
  
  add.o:add.c
          gcc -c $^ -o $@
  
  sub.o:sub.c
          gcc -c $^ -o $@
  
  mul.o:mul.c
          gcc -c $^ -o $@
  
  div.o:div.c
          gcc -c $^ -o $@
  
  test.o:test.c
          gcc -c $^ -o $@
  
  clean:
          rm -rf $(OBJS) $(TARGET)
  ```

  ![image-20231105141235489](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105141235489.png)

- 模式规则

  除了使用用户自定义变量，Makefile中也提供了一些变量（变量名大写）供用户直接使用，可以对其进行赋值

  ![image-20231105123046106](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105123046106.png)

  > %.o:%.c
  >
  > $(CC) -c $(CFLAGS) $(CPPFALGS) $< -o $@

  ```makefile
  OBJS = add.o sub.o mul.o div.o test.o
  TARGET = test
  
  #这些变量不能单独使用，必须在命令中使用
  #$@ 表示目标
  #$^ 表示所有依赖
  #$< 表示第一个依赖
  
  $(TARGET):$(OBJS)
          gcc $^ -o $@
  
  #模式匹配 所有的.o都依赖对应的.c
  #将所有的.c 生成对应的.o
  %.o:%.c
          gcc -c $^ -o $@
  
  clean:
          rm -rf $(OBJS) $(TARGET)
  ```

  ![image-20231105141920606](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105141920606.png)



**Makefile中的函数**

![image-20231105142245656](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105142245656.png)



```makefile
#获取当前目录下所有的.c文件
SRC=$(wildcard ./*.c)

#将SRC中所有的.c替换成.o，这样就可以自动获取.o，不需要一个一个输入
OBJS=$(patsubst %.c, %.o, $(SRC))

TARGET = test

#这些变量不能单独使用，必须在命令中使用
#$@ 表示目标
#$^ 表示所有依赖
#$< 表示第一个依赖

$(TARGET):$(OBJS)
        gcc $^ -o $@

#模式匹配 所有的.o都依赖对应的.c
#将所有的.c 生成对应的.o
%.o:%.c
        gcc -c $^ -o $@

clean:
        rm -rf $(OBJS) $(TARGET)
```

![image-20231105142837869](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105142837869.png)



但是clean仍有问题，如果同级目录下有一个clean文件，就会出问题，不会去执行Makefile中的clean命令了

![image-20231105143140842](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105143140842.png)



原因：

![image-20231105143333025](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105143333025.png)



最终版Makefile

```makefile
#获取当前目录下所有的.c文件
SRC=$(wildcard ./*.c)

#将SRC中所有的.c替换成.o，这样就可以自动获取.o，不需要一个一个输入
OBJS=$(patsubst %.c, %.o, $(SRC))

TARGET = test

#这些变量不能单独使用，必须在命令中使用
#$@ 表示目标
#$^ 表示所有依赖
#$< 表示第一个依赖

$(TARGET):$(OBJS)
        gcc $^ -o $@

#模式匹配 所有的.o都依赖对应的.c
#将所有的.c 生成对应的.o
%.o:%.c
        @gcc -c $^ -o $@

#声明clean为伪目标 伪目标不去判断目标文件是否存在或已经更新
#无条件执行命令
.PHONY:clean
clean:
        -rm -rf $(OBJS) $(TARGET)
```

![image-20231105143725332](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105143725332.png)



**Makefile工作原理**

1. ![image-20231105144230468](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105144230468.png)
2. ![image-20231105144344994](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231105144344994.png)

> 总结：
>
> - 分析各个目标和依赖之间的关系
> - 根据依赖关系自底向上执行命令
> - 根据修改时间比目标新，确定更新
> - 如果目标不依赖任何条件，则执行对应命令，以示更新



## 8. 系统调用

**什么是系统调用？**

![image-20231106110233740](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231106110233740.png)

![image-20231106110523264](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231106110523264.png)



**系统调用的实现**

![image-20231106110621495](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231106110621495.png)



**系统调用和库函数的区别**

![image-20231106110745665](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231106110745665.png)



**C库中IO函数工作流程**

![image-20231107163946955](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231107163946955.png)

![image-20231107164234394](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231107164234394.png)



**错误处理函数**

​	errno是记录系统的最后一次错误代码。代码是一个int型的值，在errno.h定义。查看错误代码errno是调试程序的一个重要方法

​	当Linux C api函数发生异常时，一般会将errno全局变量赋一个整数值，不同的值表示你不同的涵义，可以通过查看该值推测出错的原因



strerror(errno)：传入错误码，返回错误信息字符串

perror("fopen err")：打印错误原因的字符串



测试程序：1errno.c

```c
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
```

> jomo@jomo-virtual-machine:~/linux_server/sysfn$ gcc 1errno.c 
> jomo@jomo-virtual-machine:~/linux_server/sysfn$ ./a.out 
> fopen failed....
> errno: 2
> fopen: No such file or directory
> fopen: No such file or directory



查看错误号：

> /usr/include/asm-generic/errno-base.h
>
> /usr/include/asm-generic//errno.h



**虚拟地址空间**

![image-20231107171455415](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231107171455415.png)

![image-20231107172251836](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231107172251836.png)



**文件描述符**

![image-20231107173214157](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231107173214157.png)

![image-20231107173521486](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231107173521486.png)

![image-20231107173643451](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231107173643451.png)



**常用文件IO函数**

- **open函数**

  ```c
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  
  int open(const char *pathname, int flags);
  int open(const char *pathname, int flags, mode_t mode);
  功能：
      打开文件，如果文件不存在则可以选择创建
  参数；
      pathname: 文件的路径及文件名
      flags: 打开文件的行为标志，必选项 O_RDONLY,O_WRONLY,O_RDWR
      mode: 这个参数，只有文件不存在时有效，指新建文件时指定文件的权限
  返回值：
      成功：成功返回打开的文件描述符
      失败：-1
  ```

  ![image-20231116134348541](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231116134348541.png)

- **close函数**

  ```c
  #include <unistd.h>
  int close(int fd);
  功能：
      关闭已打开的文件
  参数；
      fd: 文件描述符，open()的返回值
  返回值：
      成功：0
      失败：-1，并设置errno
  ```
  
  需要说明的是，当一个进程终止时，内核对该进程所有尚未关闭的文件描述符调用close关闭，所以即使用户程序不调用close，在终止时内核也会自动关闭它打开的所有文件
  
  但是对于一个常年运行的程序（比如网络服务器），打开的文件描述符一定要记得关闭，否则随着打开的文件越来越多，会占用大量文件描述符和系统资源
  
  ```c
  #include <stdio.h>
  #include<string.h>
  #include<sys/types.h>
  #include<sys/stat.h>
  #include<sys/fcntl.h>
  #include <unistd.h>
  
  // 打开和关闭文件
  int main(int argc, char *argv[])
  {
      int fd = -1;
      // 以只读的方式打开一个文件，如果文件不存在就报错
      fd = open("txt",O_RDONLY);
      if (-1 == fd)
      {
          perror("open");
          return 1;
      }
  
      printf("fd = %d\n",fd);
  
      // 关闭文件
      close(fd);
      return 0;
  }
  ```
  
- **write函数**

  ```c
  #include <unistd.h>
  ssize_t write(int fd, const void *buf, size_t count);
  功能：
      把指定数目的数据写到文件（fd）
  参数：
  	fd: 文件描述符
      buf: 数据首地址
      count: 写入数据的长度
  返回值：
  	成功：实际写入的字节个数
      失败：-1
  ```

  ```c
  #include <stdio.h>
  #include<string.h>
  #include<stdlib.h>
  #include<sys/types.h>
  #include<sys/stat.h>
  #include<sys/fcntl.h>
  #include <unistd.h>
  
  // 写文件
  int main(int argc, char *argv[])
  {
      int fd=-1;
      int ret=1;
  
      char*str = "hello itcast";
  
      // 1.以只写的方式打开一个文件
      fd=open("txt",O_WRONLY | O_CREAT,0644);
      if(-1 == fd)
      {
          perror("open");
          return 1;
      }
  
      printf("fd=%d\n",fd);
  
      // 2.写文件
      ret = write(fd,str,strlen(str));
      if (-1==ret){
          perror("write");
          return 1;
      }
      printf("write len: %d\n",ret);
  
      // 3.关闭文件
      close(fd);
  
      return 0;
  }
  ```

- **read函数**

  ```c
  #include <unistd.h>
  ssize_t read(int fd, void *buf, size_t count);
  功能：
      把指定数目的数据读到内存（缓冲区）
  参数：
  	fd: 文件描述符
      buf: 内存首地址
  	count: 读取的字节个数
  返回值：
      成功：实际读取到的字节个数
  	失败：-1
  ```

  ```c
  #include <stdio.h>
  #include<string.h>
  #include<stdlib.h>
  #include<sys/types.h>
  #include<sys/stat.h>
  #include<sys/fcntl.h>
  #include <unistd.h>
  
  #define SIZE 128
  
  // 读文件
  int main(int argc, char *argv[])
  {
      int fd=-1;
      int ret=1;
  
      char buf[SIZE];
  
      // 1.以只读的方式打开一个文件
      fd=open("txt",O_RDONLY);
      if(-1 == fd)
      {
          perror("open");
          return 1;
      }
  
      printf("fd=%d\n",fd);
  
      // 2.读文件
      memset(buf,0,SIZE);
      ret = read(fd,buf,SIZE);
      if (-1==ret){
          perror("read");
          return 1;
      }
      printf("read len: %d %s\n",ret,buf);
  
      // 3.关闭文件
      close(fd);
  
      return 0;
  }
  ```

  **阻塞与非阻塞**

  ![image-20231116143111609](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231116143111609.png)

  阻塞程序示例（getchar.c)

  ```c
  #include <stdio.h>
  
  int main(int argc, char *argv[])
  {
      char ch = -1;
  
      // 从标准输入获取一个字符
      ch = getchar();// 如果不输入换行符，则会一直阻塞在标准输入中
  
      putchar(ch);
  
      return 0;
  }
  ```




## 9. 进程

![image-20231122102057103](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122102057103.png)

**进程号和相关函数**

- **getpid**

  ```c
  #include <sys/types.h>
  #include <unistd.h>
  
  pid_t getpid(void);
  功能：
      获取本进程号PID
  参数：
      无
  返回值：
      本进程号
  ```

- **getppid**

  ```c
  #include <sys/types.h>
  #include <unistd.h>
  
  pid_t getppid(void);
  功能：
      获取调用此函数的进程的父进程号PID
  参数：
      无
  返回值：
      父进程号
  ```

- **getpgid**

  ```c
  #include <sys/types.h>
  #include <unistd.h>
  
  pid_t getpgid(void);
  功能：
      获取进程组号
  参数：
      pid：进程号
  返回值：
      参数为0时返回当前进程组号，否则返回参数指定的进程的进程组号
  ```

**进程的创建**

- **fork**

  ```c
  #include <sys/types.h>
  #include <unistd.h>
  
  pid_t fork(void);
  功能：
      用于从一个已存在的进程中创建一个新进程，新进程称为子进程，原进程称为父进程
  参数：
      无
  返回值：
      成功：子进程中返回0，父进程中返回子进程ID。pid_t，为整型
      失败：返回-1
      失败的两个主要原因是：
      	1)当前的进程数已经达到了系统规定的上限，这时errno的值被设置为EAGAIN
      	2)系统内存不足时，这时errno的值被设置为ENOMEM
  ```

  示例代码：

  ```c
  #include <stdio.h>
  #include <sys/types.h>
  #include <unistd.h>
  
  // 创建一个子进程
  int main(int argc, char *argv[])
  {
      // 创建子进程
      fork();
  
      printf("hello world\n");
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/process$ gcc fork.c 
  > jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
  > hello world
  > hello world
  >
  >
  > 也可能出现以下情况：
  >
  > jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
  > hello world
  > jomo@jomo-virtual-machine:~/linux_server/process$ hello world
  > 这是由于父进程退出后，控制权重新交回给bash，而子进程还没打印完

  ![image-20231122104013839](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122104013839.png)



**父子进程关系**

![image-20231122104421078](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122104421078.png)

![image-20231122104442374](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122104442374.png)

写时拷贝，读时共享



**区分父子进程**

![image-20231122104953717](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122104953717.png)

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// 区分父子进程
int main(int argc, char *argv[])
{
    pid_t pid = -1;
    
    // 创建一个子进程
    // fork函数在子进程中返回0，在父进程中返回子进程的pid
    pid = fork();

    if(pid < 0){
        // 没有创建成功
        perror("fork");
        return 0;
    }
    if(0 == pid){
        // 子进程
        while(1){
            printf("I am son\n");
            sleep(1);    
        }
    }else if (pid>0){
        // 父进程
        while(1){
            printf("I am father\n");
            sleep(1);
        }
    }

    return 0;
}
```



> jomo@jomo-virtual-machine:~/linux_server/process$ gcc fork.c 
> jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
> I am father
> I am son
> I am father
> I am son
> I am father
> I am son
> I am father
> I am son
> I am son
> I am father
> I am son
> I am father
> I am son
> I am father
> I am father
> I am son
>
> .......

![image-20231122110143605](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122110143605.png)



**父子进程地址空间**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 父子进程地址空间
int main(int argc, char *argv[])
{
    int var = 88;
    pid_t pid = -1;
    
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
        printf("子进程睡醒之后 var = %d\n",var);// 88
    }
    else
    {
        // 父进程
        printf("父进程之前 var = %d\n",var); //88
        var++;
        printf("父进程之后 var = %d\n",var);// 89
    }

    return 0;
}
```



> jomo@jomo-virtual-machine:~/linux_server/process$ gcc fork.c 
> jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
> 父进程之前 var = 88
> 父进程之后 var = 89
> jomo@jomo-virtual-machine:~/linux_server/process$ 子进程睡醒之后 var = 88





```c
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
    }
    else
    {
        // 父进程
        printf("父进程之前 *p = %d num = %d  var = %d\n",*p,num,var);
        var++;
        num++;
        (*p)++;
        printf("父进程之后 *p = %d num = %d  var = %d\n",*p,num,var);
    }

    return 0;
}
```



> jomo@jomo-virtual-machine:~/linux_server/process$ gcc fork.c 
> jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
> 父进程之前 *p = 200 num = 100  var = 88
> 父进程之后 *p = 201 num = 101  var = 89
> jomo@jomo-virtual-machine:~/linux_server/process$ 子进程睡醒之后 *p = 200 num = 100  var = 88



查看内存泄漏：



> jomo@jomo-virtual-machine:~/linux_server/process$ valgrind ./a.out 
> ==13379== Memcheck, a memory error detector
> ==13379== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
> ==13379== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
> ==13379== Command: ./a.out
> ==13379== 
> 父进程之前 *p = 200 num = 100  var = 88
> 父进程之后 p = 201 num = 101  var = 89
> **==13379==** 
> ***==13379== HEAP SUMMARY:**
> ***==13379==     in use at exit: 4 bytes in 1 blocks***
> ***==13379==   total heap usage: 2 allocs, 1 frees, 1,028 bytes allocated***
> ***==13379==*** 
> ***==13379== LEAK SUMMARY:***
> ***==13379==    definitely lost: 4 bytes in 1 blocks***
> ***==13379==    indirectly lost: 0 bytes in 0 blocks***
> ***==13379==      possibly lost: 0 bytes in 0 blocks***
> ***==13379==    still reachable: 0 bytes in 0 blocks***
> ***==13379==         suppressed: 0 bytes in 0 blocks***
> ***==13379== Rerun with --leak-check=full to see details of leaked memory***
> ***==13379==*** 
> ==13379== For lists of detected and suppressed errors, rerun with: -s
> ==13379== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
> jomo@jomo-virtual-machine:~/linux_server/process$ 子进程睡醒之后 *p = 200 num = 100  var = 88
> ******==13380==*** 
> ***==13380== HEAP SUMMARY:***
> ***==13380==     in use at exit: 4 bytes in 1 blocks***
> ***==13380==   total heap usage: 2 allocs, 1 frees, 1,028 bytes allocated***
> ***==13380==*** 
> ***==13380== LEAK SUMMARY:***
> ***==13380==    definitely lost: 4 bytes in 1 blocks***
> ***==13380==    indirectly lost: 0 bytes in 0 blocks***
> ***==13380==      possibly lost: 0 bytes in 0 blocks***
> ***==13380==    still reachable: 0 bytes in 0 blocks***
> ***==13380==         suppressed: 0 bytes in 0 blocks***
> ***==13380== Rerun with --leak-check=full to see details of leaked memory***
> ==13380==*** 
> ==13380== For lists of detected and suppressed errors, rerun with: -s
> ==13380== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

原因：

没有释放指针p，注意父子进程都要释放



修改后：

```c
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
```

> jomo@jomo-virtual-machine:~/linux_server/process$ valgrind ./a.out 
>
> ==3430== Memcheck, a memory error detector
> ==3430== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
> ==3430== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
> ==3430== Command: ./a.out
> ==3430== 
> 父进程之前 *p = 200 num = 100  var = 88
> 父进程之后 *p = 201 num = 101  var = 89
> ******==3430==*** 
> ***==3430== HEAP SUMMARY:***
> ***==3430==     in use at exit: 0 bytes in 0 blocks***
> ***==3430==   total heap usage: 2 allocs, 2 frees, 1,028 bytes allocated***
> ***==3430==*** 
> ***==3430== All heap blocks were freed -- no leaks are possible***
> ==3430==*** 
> ==3430== For lists of detected and suppressed errors, rerun with: -s
> ==3430== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
> jomo@jomo-virtual-machine:~/linux_server/process$ 子进程睡醒之后 *p = 200 num = 100  var = 88
> ******==3431==*** 
> ***==3431== HEAP SUMMARY:***
> ***==3431==     in use at exit: 0 bytes in 0 blocks***
> ***==3431==   total heap usage: 2 allocs, 2 frees, 1,028 bytes allocated***
> ***==3431==*** 
> ***==3431== All heap blocks were freed -- no leaks are possible***
> ==3431==*** 
> ==3431== For lists of detected and suppressed errors, rerun with: -s
> ==3431== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)



**GDB调试多进程**

![image-20231122221202636](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122221202636.png)



**进程退出函数**

相关函数：

```c
#include <stdlib.h>
void exit(int status);

#include <unistd.h>
void _exit(int status);
功能：
    结束调用此函数的进程
参数：
    status：返回给父进程的参数（低8位有效），至于这个参数是多少根据需要来填写
返回值：
    无
```

![image-20231122221524235](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122221524235.png)

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// 结束进程
int main(int argc, char *argv[])
{
    printf("hello world");

    // 等价于return 0
    // exit(0); 正常输出hello world
    // _exit(0); // 无法正常输出hello world，因为直接退出，不做任何清理工作
    _Exit(0); // 等价于_exit(0)
}
```



**等待子进程退出函数**

![image-20231122222125838](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122222125838.png)

- wait：

  ```c
  #include <sys/types.h>
  #include <sys/wait.h>
  
  pid_t wait(int *wstatus);
  功能：
      等待任意一个子进程结束，如果任意一个子进程结束了，此函数会回收该子进程的资源
  参数：
  	status：进程退出时的状态信息
  返回值：
      成功：已经结束子进程的进程号
      失败：-1
  ```

  ![image-20231122223558084](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122223558084.png)

  ![image-20231122224713369](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122224713369.png)

  ```c
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
          // jomo@jomo-virtual-machine:~$ kill -9 3638
          printf("子进程被信号%d杀死了...\n",WTERMSIG(status));
      }
      else if(WSTOPSIG(status))
      {	
          // 向指定进程发送暂停信号
          // jomo@jomo-virtual-machine:~$ kill -19 3640
          // 唤醒指定进程
          // jomo@jomo-virtual-machine:~$ kill -19 3640
          printf("子进程被信号%d暂停了...\n",WSTOPSIG(status));
      }
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/process$ gcc wait.c 
  > jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
  > 父进程等待子进程退出，回收其资源
  > child process 3588 do 1
  > child process 3588 do 2
  > child process 3588 do 3
  > child process 3588 do 4
  > child process 3588 do 5
  > child process 3588 do 6
  > child process 3588 do 7
  > child process 3588 do 8
  > child process 3588 do 9
  > child process 3588 do 10
  > 父进程回收了子进程资源...
  > 子进程正常退出状态码：10

  

  > jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
  >
  > 在另一终端输入：jomo@jomo-virtual-machine:~$ kill -9 3638
  >
  > 父进程等待子进程退出，回收其资源
  > child process 3638 do 1
  > child process 3638 do 2
  > child process 3638 do 3
  > child process 3638 do 4
  > child process 3638 do 5
  > 父进程回收了子进程资源...
  > 子进程被信号9杀死了...

- waitpid：

  ![image-20231122230017982](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122230017982.png)

  ```c
  ret = waitpid(-1,&status,0);//等价于wait：阻塞等待任意子进程退出
  ret = waitpid(-1,&status,WNOHANG);//非阻塞等待任意子进程退出
  ```



**孤儿进程**

![image-20231122230602136](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122230602136.png)



**僵尸进程**

![](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231122230719126.png)

如果子进程先于父进程退出，那么在父进程退出之前，子进程会一直处于僵尸状态

进程退出后，内核会为其释放资源，例如占用的内存与文件描述符等，但仍会为其保留一定的信息，主要是PCB的信息（包括进程号、退出状态、运行时间等），而调用wait或waitpid函数（由别的进程调用）可以彻底清除该进程的遗留信息，可以把进程号让出来，避免无进程号可用



子进程先于父进程退出，且父进程也没有调用wait或waitpid函数：

- 父进程未退出：僵尸进程可能会一直存在，占用系统资源。
- 父进程退出后：在这种情况下，虽然父进程没有主动调用`wait`或`waitpid`，但当父进程退出时，操作系统会自动将所有属于父进程的子进程交由init进程（通常是PID为1的进程）来收养。init进程会定期调用`wait`或`waitpid`来获取已经退出的子进程的退出状态，并释放相关的资源。



**进程替换**

![image-20231126113236730](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126113236730.png)

![image-20231126113722324](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126113722324.png)

![image-20231126113811421](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126113811421.png)

- **execlp**

  ```c
  int execlp(const char *file, const char *arg, .../* (char  *) NULL */);
  ```

  

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  
  int main(void)
  {
      printf("Hello itcast\n");
  
      // arg0 arg1 arg2 ... argn
      // arg0一般是可执行文件名 argn必须是NULL
      // 等价于执行ls -l /home
      execlp("ls", "ls", "-l", "/home", NULL);
  
      printf("Hello world\n");
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/process$ gcc execlp.c 
  > jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
  > Hello itcast
  > 总计 4
  > drwxr-x--- 32 jomo jomo 4096 11月 26 11:40 jomo

  最后一个hello的打印语句并没有被执行，原因如下：

  ![image-20231126115058501](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126115058501.png)

- **execl**

  ```c
  int execl(const char *pathname, const char *arg, .../* (char  *) NULL */);
  ```

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  
  int main(void)
  {
      printf("Hello itcast\n");
  
      // arg0：可执行文件的绝对路径或者相对路径
      // arg1：可执行文件的名字
      // arg2~arg(n-1)：可执行文件的参数
      // argn：必须是NULL
      execl("/bin/ls", "ls", "-l", "/home", NULL);
  
      printf("Hello world\n");
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/process$ gcc execl.c
  > jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
  > Hello itcast
  > 总计 4
  > drwxr-x--- 32 jomo jomo 4096 11月 26 11:58 jomo



**进程间通信**

![image-20231126121239015](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126121239015.png)

![image-20231126121327042](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126121327042.png)



**无名管道**

![image-20231126142541551](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126142541551.png)

![image-20231126142803457](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126142803457.png)



**pipe**

```c
#include <unistd.h>

int pipe(int pipefd[2]);
功能：创建无名管道

参数：
    pipefd：为int型数组的首地址，其存放了管道的文件描述符pipefd[0]、pipefd[1]
	
    当一个管道建立时，它会创建两个文件描述符fd[0]和fd[1]。其中fd[0]固定用于读管道，而fd[1]固定用于写管道。一般文件I/O的函数都可以用来操作管道(lseek())除外

返回值：
    成功：0
    失败：-1
```

![image-20231126174056155](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231126174056155.png)

```c
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
```

> jomo@jomo-virtual-machine:~/linux_server/process$ gcc pipe.c 
> jomo@jomo-virtual-machine:~/linux_server/process$ ./a.out 
> parent process write len :10
> chilid process read buf: ABCDEFGHIJ

由于无名管道是阻塞型I/O，因此子进程执行read时，如果父进程还未写入管道（管道还没有数据），则子进程会一直阻塞直至父进程写完（管道有数据了）



管道的读写特性：

1. 写端没有关闭
   - 管道中没有数据：读管道进程会阻塞
   - 管道中有数据：读管道进程会将数据读出，下一次读没有数据就会阻塞
2. 所有写端关闭：读管道进程读取全部内容，最后返回0
3. 读端没有关闭：如果管道被写满了，写管道进程会被阻塞
4. 所有读端被关闭：写管道进程会收到一个信号SIGPIPE，然后异常终止



**查看管道缓冲区**

**fpathconf**

```c
#include <unistd.h>

long fpathconf(int fd, int name);
功能：该函数可以通过name参数查看不同的属性值
参数：
    fd：文件描述符
    name：
		_PC_PIPE_BUF，查看管道缓冲区大小
    	_PC_NAME_MAX，文件名名字字节数的上限
返回值：
    成功：根据namne返回的值的意义也不同
    失败：-1
```



**设置为非阻塞的方法**

```c
// 获取原来的flag
int flag = fcntl(fd[0],F_GETFL);
// 设置新的flag
flag |= O_NONBLOCK
fcntl(fd[0],F_SETFL,flag);
```

结论：如果写端没有关闭，读端设置为非阻塞，如果没有数据，直接返回-1
