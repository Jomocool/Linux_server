# Linux高性能服务器编程

## Linux编程

### 1. GCC编译器简介

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



### 2. GCC工作流程和常用选项

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



### 3. 静态链接和动态链接

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



### 4. 静态库和动态库简介

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



### 5. 动态库制作和使用

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



### 6. GDB调试器

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



### 7. Makefile

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



### 8. 系统调用

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
#include <errno.h>
#include <string.h>

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




### 9. 进程

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



**有名管道**

![image-20231129114932531](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129114932531.png)

- 通过命令创建命名管道

  > jomo@jomo-virtual-machine:~/linux_server/process$ mkfifo fifo
  > jomo@jomo-virtual-machine:~/linux_server/process$ ls -l fifo
  > prw-rw-r-- 1 jomo jomo 0 11月 29 11:51 fifo

- 通过函数创建命名管道

  ```c
  #include <sys/types.h>
  #include <sys/stat.h>
  
  int mkfifo(const char* pathname,mode_t mode);
  功能：
      命名管道的创建
  参数：
      pathname：普通的路径名，也就是创建后FIFO的名字
      mode：文件权限，与打开普通文件的open函数中的mode参数相同。（0666）
  返回值：
      成功：0 状态码
      失败：如果文件已经存在，则会出错且返回-1
  ```

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  
  // 通过mkfifo函数创建一个管道文件
  int main(void)
  {
      int ret = -1;
  
      // 可以用access函数判断文件是否已经存在
      // 创建一个命名管道，管道的名字：fifo
      ret = mkfifo("./fifo", 0644);
      if (-1 == ret)
      {
          perror("mkfifo");
          return 1;
      }
  
      printf("创建一个命名管道ok...\n");
  
      return 0;
  }
  ```

- 命名管道读写操作

  ![image-20231129145330168](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129145330168.png)

  write.c

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <fcntl.h>
  
  #define SIZE 128
  
  // 写命名管道
  int main(void)
  {
      int fd = -1;
      int i = 0;
      int ret = -1;
      char buf[128];
  
      // 1.以只写的方式打开一个管道文件
      fd = open("fifo", O_WRONLY);
      if (-1 == fd)
      {
          perror("open");
          return 1;
      }
      printf("以只写的方式打开一个命名管道...OK\n");
  
      // 2.写管道
      while (1)
      {
          memset(buf, 0, SIZE);
          sprintf(buf, "Hello world %d", i++);
          ret = write(fd, buf, strlen(buf));
          if (ret <= 0)
          {
              perror("write");
              break;
          }
          printf("write fifo %d\n", ret);
          sleep(1);
      }
  
      // 3.关闭文件
      close(fd);
  
      return 0;
  }
  ```

  read.c

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <fcntl.h>
  
  #define SIZE 128
  
  // 读管道
  int main(void)
  {
      int fd = -1;
      char buf[SIZE];
      int ret = -1;
  
      // 1.以只读的方式打开一个管道文件
      fd = open("fifo", O_RDONLY);
      if (-1 == fd)
      {
          perror("open");
          return 1;
      }
      printf("以只读的方式打开一个管道...OK\n");
  
      // 2.循环读管道
      while (1)
      {
          memset(buf, 0, SIZE);
          ret = read(fd, buf, SIZE); // 不需要sleep，因为如果没有数据可读就会阻塞
  
          // 如果写端是关闭的，则ret = 0
          if (ret <= 0)
          {
              perror("read");
              break;
          }
          printf("buf: %s\n", buf);
      }
  
      // 3.关闭文件
      close(fd);
  
      return 0;
  }
  ```

  

  在write.c执行前，read.c会一直阻塞：

  ![image-20231129154128914](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129154128914.png)

  ![image-20231129154159038](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129154159038.png)

  ![image-20231129154219562](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129154219562.png)

- 命名管道注意事项

  1. 一个以只读的方式打开一个管道的进程会阻塞直至另一个进程以只写的`打开`该管道
  2. 一个以只写的方式打开一个管道的进程会阻塞直至另一个进程以只读的方式`打开`该管道

  ![image-20231129160010565](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129160010565.png)



**使用命名管道写一个简易的聊天程序**

talkA.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 128

// 先读后写
int main(void)
{
    int fdr = -1;
    int fdw = -1;
    char buf[SIZE];
    int ret = -1;
    // 以只读的方式打开fifo1
    fdr = open("fifo1", O_RDONLY);
    if (-1 == fdr)
    {
        perror("open");
        return 1;
    }
    printf("以只读的方式打开fifo1...OK\n");

    // 以只写的方式打开fifo2
    fdw = open("fifo2", O_WRONLY);
    if (-1 == fdw)
    {
        perror("open");
        return 1;
    }
    printf("以只写的方式打开fifo2...OK\n");

    // 循环读写
    while (1)
    {
        // 读fifo1
        memset(buf, 0, SIZE);
        ret = read(fdr, buf, SIZE);
        if (ret <= 0)
        {
            perror("read");
            break;
        }
        printf("Received from B: %s\n", buf);

        // 写fifo2
        memset(buf, 0, SIZE);
        fgets(buf, SIZE, stdin);
        // 去掉最后一个换行符
        if ('\n' == buf[strlen(buf) - 1])
        {
            buf[strlen(buf) - 1] = '\0';
        }
        ret = write(fdw, buf, strlen(buf));
        if (ret <= 0)
        {
            perror("write");
            break;
        }

        printf("Send to B ret: %d\n", ret);
    }

    // 关闭文件描述符
    close(fdr);
    close(fdw);

    return 0;
}
```

talkB.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 128

// 先写后读
int main(void)
{
    int fdr = -1;
    int fdw = -1;
    char buf[SIZE];
    int ret = -1;
    // 以只写的方式打开fifo1
    fdw = open("fifo1", O_WRONLY);
    if (-1 == fdw)
    {
        perror("open");
        return 1;
    }

    printf("以只写的方式打开fifo1...OK\n");

    // 以只读的方式打开fifo2
    fdr = open("fifo2", O_RDONLY);
    if (-1 == fdr)
    {
        perror("open");
        return 1;
    }
    printf("以只读的方式打开fifo2...OK\n");

    // 循环读写
    while (1)
    {
        // 写fifo1
        memset(buf, 0, SIZE);
        fgets(buf, SIZE, stdin);
        // 去掉最后一个换行符
        if ('\n' == buf[strlen(buf) - 1])
        {
            buf[strlen(buf) - 1] = '\0';
        }
        ret = write(fdw, buf, strlen(buf));
        if (ret <= 0)
        {
            perror("write");
            break;
        }

        printf("Send to A ret: %d\n", ret);

        // 读fifo2
        memset(buf, 0, SIZE);
        ret = read(fdr, buf, SIZE);
        if (ret <= 0)
        {
            perror("read");
            break;
        }
        printf("Received from A: %s\n", buf);
    }

    // 关闭文件描述符
    close(fdr);
    close(fdw);

    return 0;
}
```

![image-20231129165225965](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129165225965.png)



![image-20231129165630862](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129165630862.png)



![image-20231129170431979](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129170431979.png)





**共享存储映射**

![image-20231129174313731](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129174313731.png)

- **存储映射函数**

  - mmap函数

    ![image-20231129174651049](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129174651049.png)

    ![image-20231129174949419](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129174949419.png)

  - munmap函数

    ![image-20231129175020981](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129175020981.png)

    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/mman.h>
    
    // 存储映射
    int main(void)
    {
        int fd = -1;
        int ret = -1;
        void *addr = NULL;
    
        // 1. 以读写的方式打开一个文件
        fd = open("txt", O_RDWR);
        if (-1 == fd)
        {
            perror("open");
            return 1;
        }
    
        // 2. 将文件映射到内存
        addr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED)
        {
            perror("mmap");
            return 1;
        }
    
        printf("文件存储映射OK...\n");
    
        // 3. 关闭文件
        close(fd);
    
        // 4. 写文件（不需要再通过write了，直接操作内存地址）
        memcpy(addr, "1234567890", 10);
    
        // 5. 断开存储映射
        munmap(addr, 1024);
    
        return 0;
    }
    ```

    ![image-20231129180324624](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129180324624.png)

    ![image-20231129180353395](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129180353395.png)

  - **借助共享映射区实现进程间的通信**

    - 有血缘的进程之间：借助共享映射区（即映射之后的内存地址指针）
    - 没有血缘的进程之间：借助文件

  - **匿名映射实现父子进程间通信**

    不需要依赖任何文件

    ![image-20231129182236021](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231129182236021.png)




### 10. 信号

**信号的概念**

![image-20231130162330108](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130162330108.png)



**信号的特点**

- 简单
- 不能携带大量信息
- 满足某个特定的条件才发送



信号可以直接进行用户空间进程和内核空间进程的交互，内核进程可以利用它来通知用户空间进程发生了哪些系统事件。

一个完整的信号周期包括三个部分：信号的产生，信号在进程中的注册、信号在进程中的注销，执行信号处理函数

![image-20231130163823539](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130163823539.png)

注意：这里的信号的产生，注册，注销是信号的内部机制，而不是信号的函数实现



**信号的编号**

![image-20231130175827136](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130175827136.png)

不存在编号为0的信号，其中1-31号信号称之为常规信号（也叫普通信号或标准信号），34-64称之为实时信号，驱动编程与硬件相关。名字上区别不大，而前32个名字各不相同



**信号四要素**

每个信号必备的4要素

1. 编号
2. 名称
3. 事件
4. 默认处理动作



Action为默认动作：

- Term：终止进程
- Ign：忽略信号（默认即使对该种信息忽略操作）
- Core：终止进程，生成Core文件（查验死亡原因，用于gdb调试）
- Stop：停止（暂停）进程
- Cont：继续运行进程

![image-20231130183023393](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130183023393.png)



**信号的状态**

1. **产生**

   ![image-20231130183157232](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130183157232.png)

2. **未决状态：没有被处理**

3. **递达状态：信号被处理了**



**阻塞信号集和未决信号集**

![image-20231130193035848](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130193035848.png)

阻塞信号：不一定产生了，无论产不产生，都不处理

味觉信号：一定产生了，但还未被处理



**信号产生函数**

- kill函数

  ![image-20231130194126302](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130194126302.png)

  ```c
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
          kill(pid, SIGKILL);// 也可以杀死自己，kill(getpid(),SIGKILL);
          printf("父进程完成任务，该结束了...\n");
      }
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/signal$ gcc kill.c 
  > jomo@jomo-virtual-machine:~/linux_server/signal$ ./a.out 
  > child process do work...
  > child process do work...
  > child process do work...
  > 子进程该退出了...
  > 父进程完成任务，该结束了...

- raise函数

  ![image-20231130195208725](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130195208725.png)

  > jomo@jomo-virtual-machine:~/linux_server/signal$ gcc raise.c 
  > jomo@jomo-virtual-machine:~/linux_server/signal$ ./a.out 
  > do working 1
  > do working 2
  > do working 3
  > do working 4
  > 自己给自己发送信号...
  > 已终止

- abort函数

  ![image-20231130225622889](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130225622889.png)

- alarm函数（闹钟）

  ![image-20231130225843881](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130225843881.png)

  进程的定时器超时后，进程默认终止

  新设置的闹钟的alarm函数返回值是上一个闹钟剩余的时间，且新闹钟会覆盖旧闹钟，如下：

  ```c
  int ret = 0;
  ret = alarm(5);// ret = 0
  sleep(2);
  ret = alarm(4);// ret = 3
  // 在上一行设置完新定时器后，4秒后进程终止
  while(1);
  ```

- setitimer函数（定时器）

  ![image-20231130230819802](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130230819802.png)

  ![image-20231130231422258](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231130231422258.png)

  虽然设置了周期，但在第一次定时器超时后，进程收到内核发出的信号`SIGALRM`，所以还是默认终止了



**信号集**

- 概述

  在PCB中有两个非常重要的信号集。一个称之为“阻塞信号集”，另一个称为“阻塞信号集”

  这两个信号集都是内核使用`位图机制`来实现的。但操作系统不允许我们直接对其进行位操作。而需自定义另外一个集合，借助信号集操作函数来对PCB中的这两个信号集进行修改

  ![image-20231201135325634](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201135325634.png)

  阻塞信号集可以读写

  未决信号集只能读不能写

- 自定义信号集函数

  ![image-20231201140203370](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201140203370.png)

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <signal.h>
  #include <bits/types/sigset_t.h>
  
  // 显示信号集
  void show_set(sigset_t *s)
  {
      int i = 0;
      for (i = 1; i < 32; i++)
      {
          if (sigismember(s, i))
          {
              printf("1");
          }
          else
          {
              printf("0");
          }
      }
      putchar('\n');
  }
  
  // 信号集处理函数
  int main(void)
  {
      // 信号集集合
      sigset_t set;
  
      // 清空集合
      sigemptyset(&set);
      show_set(&set);
  
      // 将所有的信号加入到set集合中
      sigfillset(&set);
      show_set(&set);
  
      // 将信号2和3从信号集中移除
      sigdelset(&set, SIGINT);
      sigdelset(&set, SIGQUIT);
      show_set(&set);
  
      // 将信号2添加到集合中
      sigaddset(&set, SIGINT);
      show_set(&set);
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/signal$ gcc sigset.c 
  > jomo@jomo-virtual-machine:~/linux_server/signal$ ./a.out 
  > 0000000000000000000000000000000
  > 1111111111111111111111111111111
  > 1001111111111111111111111111111
  > 1101111111111111111111111111111

- sigprocmask函数

  ![image-20231201151013903](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201151013903.png)

  ![image-20231201151121256](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201151121256.png)

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <signal.h>
  #include <bits/types/sigset_t.h>
  #include <bits/sigaction.h>
  
  // 信号处理函数1
  void fun1(int signum)
  {
      printf("捕捉到信号：%d\n", signum);
  }
  
  // 信号处理函数2
  void fun2(int signum)
  {
      printf("捕捉到信号：%d\n", signum);
  }
  
  // 信号注册函数
  int main(void)
  {
      int ret = -1;
  
      // 信号集
      sigset_t set;
      sigset_t oldset;
  
      // 信号注册
      // Ctrl + 'c'
      signal(SIGINT, fun1); // （异步）相当于只要来了SIGINT信号，就会执行fun1函数（信号处理函数）
  
      // Ctrl + '\'
      signal(SIGQUIT, fun2);
  
      printf("按下任意键 阻塞信号2\n");
      getchar();
  
      sigemptyset(&oldset);
      sigemptyset(&set);
      sigaddset(&set, SIGINT);
  
      // 设置屏蔽编号为2的信号
      ret = sigprocmask(SIG_BLOCK, &set, &oldset);
      if (-1 == ret)
      {
          perror("sigprocmask");
          return 1;
      }
  
      printf("设置屏蔽编号为2的信号成功...\n");
  
      printf("按下任意键解除编号为2的信号的阻塞...\n");
      getchar();
  
      // 将信号屏蔽集设置为原来的集合
      ret = sigprocmask(SIG_SETMASK, &oldset, NULL);
      if (-1 == ret)
      {
          perror("sigprocmask");
          return 1;
      }
      printf("除屏蔽编号为2的信号成功...\n");
  
      printf("按下任意键 退出...\n");
      getchar();
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/signal$ ./a.out 
  > 按下任意键 阻塞信号2
  > ^C捕捉到信号：2
  > ^C捕捉到信号：2
  > ^C捕捉到信号：2
  > ^C捕捉到信号：2
  > ^C捕捉到信号：2
  >
  > 设置屏蔽编号为2的信号成功...
  > 按下任意键解除编号为2的信号的阻塞...
  > ^C^C^C^C^C^C^C^C^C^C^C^C
  > 捕捉到信号：2
  > 除屏蔽编号为2的信号成功...
  > 按下任意键 退出...
  > ^C捕捉到信号：2
  > ^C捕捉到信号：2
  > ^C捕捉到信号：2
  > ^C捕捉到信号：2
  >
  > jomo@jomo-virtual-machine:~/linux_server/signal$ 

- sigpending函数

  ![image-20231201153059258](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201153059258.png)





**信号捕捉**

- 信号处理方式

  ![image-20231201162005230](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201162005230.png)

  ![image-20231201163847237](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201163847237.png)

- signal函数

  ![image-20231201132550834](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201132550834.png)

  sighandler_t是一个函数指针（即指向一个函数，该函数返回类型是void，参数列表只有一个int类型的）

  ![image-20231201132926239](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201132926239.png)

  该函数由ANSI定义，由于历史原因在不同版本的Unix和不同版本的Linux中可能有不同的行为。因此应该尽量避免使用它，取而代之使用的是sigaction函数

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <signal.h>
  
  // 信号处理函数1
  void fun1(int signum)
  {
      printf("捕捉到信号：%d\n", signum);
  }
  
  // 信号处理函数2
  void fun2(int signum)
  {
      printf("捕捉到信号：%d\n", signum);
  }
  
  // 信号注册函数
  int main(void)
  {
      // 信号注册
      // Ctrl + 'c'
      // 把Ctrl + 'c'的信号处理函数改为fun1（本来是默认终止进程）
      signal(SIGINT, fun1);// （异步）相当于只要来了SIGINT信号，就会执行fun1函数（信号处理函数）
  
      // Ctrl + '\'
      signal(SIGQUIT, fun2);
  
      while (1)
      {
          sleep(1);
      }
  
      return 0;
  }
  ```

  ![](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201133830674.png)

- sigaction函数

  ![image-20231201164059536](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201164059536.png)

  ![image-20231201164449985](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201164449985.png)

  ![image-20231201164503475](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201164503475.png)

  ![image-20231201165038358](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201165038358.png)

- sigqueue函数

  ![image-20231201165139790](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201165139790.png)



**不可重入、可重入函数**

![image-20231201165323016](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201165323016.png)

注意：信号处理函数应该为可重入函数



**SIGCHLD信号**

- SIGCHLD信号产生的条件

  1. 子进程终止时
  2. 子进程接收SIGSTOP信号停止时
  3. 子进程处在停止态，接受到SIGCONT后唤醒时

- 如何避免僵尸进程

  ![image-20231201170344870](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201170344870.png)

  ```c
  // SIGCHLD信号处理函数
  void sig_child(int signo)
  {
      pid_t pid;
      
      // 处理僵尸进程，-1 代表等待任意一个子进程，WNOHANG代表不阻塞
      while((pid = waitpid(-1, NULL, WNOHANG)) > 0)
      {
          printf("child %d terminated.\n", pid);
      }
  }
  ```

  ![image-20231201170643623](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231201170643623.png)

  ```c
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
  ```

  

### 11. 守护进程

**终端的概念**

![image-20231202110704431](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202110704431.png)

![image-20231202111143273](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202111143273.png)

所以我们实际上只是在控制一个Shell进程而已



**进程组概念**

- 进程组概述

  ![image-20231202111528288](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202111528288.png)

- 相关函数

  ![image-20231202112144489](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202112144489.png)



**会话**

- 会话概念

  ![image-20231202113752811](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202113752811.png)

- 创建会话注意事项

  ![image-20231202113906814](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202113906814.png)

- API函数

  - getsid

    ![image-20231202115348994](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202115348994.png)

  - setsid

    ![image-20231202115612288](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202115612288.png)



**守护进程（重点）**

- 守护进程介绍

  ![image-20231202124729923](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231202124729923.png)

- 守护进程模型

  1. 创建子进程，父进程必须退出（父进程如果不退出，在设置会话时会报错）
     - 所有工作在子进程中进行形式上脱离了控制终端
  2. 在子进程中创建新会话（必须）
     - setsid()
     - 使子进程完全独立出来，脱离控制（新会话没有控制终端）
     - 此时子进程是会话会长也是进程组组长
  3. 改变当前工作目录为根目录（非必须）（因为无论如何，根目录都存在）
     - chdir()
     - 防止占用可卸载的文件系统
     - 也可以换成其它路径
  4. 重设文件掩码（非必须）
     - umash()
     - 防止继承的文件创建屏蔽字拒绝某些权限
     - 增加守护进程灵活性
  5. 关闭文件描述符（非必须）
     - 继承的打开文件不会用到，浪费系统资源，无法卸载
  6. 开始执行守护进程核心工作（必须）
     - 守护进程退出处理程序模型

  ```c
  #include <stab.h>
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
  ```

  > jomo@jomo-virtual-machine:~/Linux_server/code/daemon$ ./a.out 
  > jomo@jomo-virtual-machine:~/Linux_server/code/daemon$ cat /tmp/txt.log 
  > 2023年 12月 02日 星期六 16:59:47 CST
  > jomo@jomo-virtual-machine:~/Linux_server/code/daemon$ tail -f /tmp/txt.log #以动态的方式查看文件
  > 2023年 12月 02日 星期六 16:59:47 CST
  > 2023年 12月 02日 星期六 17:00:47 CST
  > 2023年 12月 02日 星期六 17:01:47 CST
  > 2023年 12月 02日 星期六 17:02:47 CST



**获取当前系统时间**

```c
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
```

> jomo@jomo-virtual-machine:~/Linux_server/code/daemon$ gcc time.c 
> jomo@jomo-virtual-machine:~/Linux_server/code/daemon$ ./a.out 
> t: 1701509442
> ctime: Sat Dec  2 17:30:42 2023
>
> year: 2023
> month: 12
> day: 2
> hout: 17
> min: 30
> sec: 42
> file_name: touch 2023112173042.log
> jomo@jomo-virtual-machine:~/Linux_server/code/daemon$ ls
> 2023112173042.log  a.out  daemon.c  time.c
> jomo@jomo-virtual-machine:~/Linux_server/code/daemon$ 



### 12. 线程

**线程概念**

![image-20231203155655741](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203155655741.png)

![image-20231203155812821](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203155812821.png)

![image-20231203155852975](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203155852975.png)

`进程是操作系统分配资源的最小单位`

`线程是操作系统调度的最小单位`



**线程函数列表安装**

命令：

> sudo apt-get install manpages-posix-dev
>
> 【说明】manpages-posix-dev包含POSIX的header files和library calls的用法

查看：

> man -k pthread



**NPTL**

![image-20231203160517714](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203160517714.png)

![image-20231203160804969](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203160804969.png)



**线程的特点**

![image-20231203160837633](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203160837633.png)

![image-20231203161000672](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203161000672.png)

查看指定进程的LWP号：

> ps -Lf pid

![image-20231203161124649](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203161124649.png)

复制：深拷贝

共享：浅拷贝



**线程共享资源**

- 文件描述符表
- 每种信号的处理方式
- 当前工作目录
- 用户ID和组ID
- 内存地址空间(.text(代码段)/.data(已初始化数据段)/.bss(未初始化数据段)/heap(堆)/共享库)



**线程非共享资源**

- 线程id
- 处理器现场和栈指针（内核栈）
- 独立的栈空间（用户空间栈）
- errno变量
- 信号屏蔽字
- 调度优先级

- 线程优缺点

  - **优点：**

    1. 提高程序并发性
    2. 开销小
    3. 数据通信、共享数据方便

  - **缺点：**

    1. 库函数，不稳定
    2. 调试、编写困难、gdb不支持
    3. 对信号支持不好

    优点相对突出，缺点均不是硬伤。Linux下由于实现方法导致进程、线程差别不是很大



**线程常用操作**

- 线程号

  ![image-20231203162558273](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203162558273.png)

  

  pthread_self函数：

  ```c
  #include <pthread.h>
  
  pthread_t pthread_self(void);
  功能：
      获取线程号
  参数：
      无
  返回值：
      调用线程的线程ID
  ```

  pthread_equal函数：

  ```c
  int pthread_equal(pthread_t t1,pthread_t t2);
  功能：
      判断线程号t1和t2是否相等，为了方便移植，尽量使用函数来比较线程ID
  参数：
      t1，t2：待判断的线程号
  返回值：
      相等：非0
      不相等：0
  ```

  测试程序：

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  
  #include <pthread.h>
  
  // 指定链接线程库
  // jomo@jomo-virtual-machine:~/linux_server/thread$ gcc pthread_self.c -pthread
  // 线程常用函数
  int main(void)
  {
      pthread_t tid = 0;
  
      // 由于每个进程默认有一个线程，这里我们没有创建线程，所以当前线程就是默认的那个线程
      // 获取当前线程的线程号
      tid = pthread_self();
  
      printf("tid: %lu\n", tid);
  
      // 比较两个线程ID是否相同
      if (pthread_equal(tid, pthread_self()))
      {
          printf("两个线程ID相同...\n");
      }
      else
      {
          printf("两个线程ID不相同...\n");
      }
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/thread$ gcc pthread_self.c -pthread 
  > jomo@jomo-virtual-machine:~/linux_server/thread$ ./a.out 
  > tid: 140020080564032
  > 两个线程ID相同...
  > jomo@jomo-virtual-machine:~/linux_server/thread$ 

- 线程的创建

  pthread_create函数：

  ![image-20231203163902181](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203163902181.png)

  ![image-20231203164104150](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203164104150.png)

  测试程序：

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  
  #include <pthread.h>
  
  // 线程1调度之后执行的函数
  void *fun(void *arg)
  {
      printf("新的线程执行任务 tid: %lu\n", pthread_self());
  
      return NULL;
  }
  
  // 线程2调度之后执行的函数
  void *fun1(void *arg)
  {
      // int var = (int)arg; 字节大小不同，指针在64位机器上站8个字节，而int占4个字节
      // 传入0x3是一个地址，但是要把它当作整型取出，如果直接解引用，会引起段错误（即访问到不属于当前进程的内存地址了）
      int var = (int)(long)arg; // 先强转为8字节的long，再强转为4字节的int
  
      printf("线程2 var = %d\n", var);
  
      return NULL;
  }
  
  int main(void)
  {
      int ret = -1;
      pthread_t tid1 = -1;
      pthread_t tid2 = -1;
  
      // 创建一个线程
      ret = pthread_create(&tid1, NULL, fun, NULL);
      if (0 != ret)
      {
          printf("pthread_create failed...\n");
          return 1;
      }
  
      // 创建一个线程
      ret = pthread_create(&tid2, NULL, fun1, (void *)0x3);
      if (0 != ret)
      {
          printf("pthread_create failed...\n");
          return 1;
      }
  
      printf("main thread... tid1: %lu\n", pthread_self());
  
      printf("按下任意键主线程退出...\n");
      getchar();
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/thread$ gcc pthread_create.c -pthread 
  > jomo@jomo-virtual-machine:~/linux_server/thread$ ./a.out 
  > main thread... tid1: 140072367859520
  > 按下任意键主线程退出...
  > 新的线程执行任务 tid: 140072364013120
  > 线程2 var = 3
  >
  > jomo@jomo-virtual-machine:~/linux_server/thread$ 

- 多线程共享资源验证

  ```c
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  
  int num = 99; // 用于验证共享data segment
  
  void *fun(void *arg)
  {
      int *pn = (int *)arg;
      printf("thread: num=%d *p=%d\n", num, *pn);
      printf("thread: execute\"num++ and *pn++\"\n");
      num++;
      (*pn)++;
      printf("thread: num=%d *p=%d\n", num, *pn);
  }
  
  int main(void)
  {
      int ret = -1;
      pthread_t tid = -1;
  
      // 用于验证共享堆
      int *p = malloc(sizeof(int));
      memset(p, 0, sizeof(int));
      *p = 99;
  
      // 创建线程
      ret = pthread_create(&tid, NULL, fun, (void *)p);
      if (0 != ret)
      {
          printf("pthread_create failed...\n");
          return 1;
      }
  
      sleep(5);
  
      printf("main thread: num=%d *p=%d\n", num, *p);
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/thread$ ./a.out 
  > thread: num=99 *p=99
  > thread: execute"num++ and *pn++"
  > thread: num=100 *p=100
  > main thread: num=100 *p=100
  > jomo@jomo-virtual-machine:~/linux_server/thread$ 

  在新创建的线程中修改num和*p的值，主线程对应的值也被修改了，说明线程之间是共享资源的（除了栈）

- 线程资源回收

  pthread_join函数：

  ![image-20231203183412542](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203183412542.png)

  测试程序：

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  
  // 线程处理函数
  void *fun(void *arg)
  {
      int i = 0;
  
      for (i = 0; i < 5; i++)
      {
          printf("fun thread do working %d\n", i);
          sleep(1);
      }
  
      return (void *)0x3;
  }
  
  // 回收线程的资源
  int main(void)
  {
      int ret = -1;
      void *retp = NULL;
      pthread_t tid = -1;
  
      // 创建一个线程
      ret = pthread_create(&tid, NULL, fun, NULL);
      if (0 != ret)
      {
          printf("pthread_create failed...\n");
          return 1;
      }
  
      printf("main thread running...\n");
  
      // 等待线程结束 会阻塞
      ret = pthread_join(tid, &retp);
      if (0 != ret)
      {
          printf("pthread_join failed...\n");
          return 1;
      }
  
      printf("retp: %p\n", retp);
      printf("main thread exit.\n");
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/thread$ gcc pthread_join.c -pthread 
  > jomo@jomo-virtual-machine:~/linux_server/thread$ ./a.out 
  > main thread running...
  > fun thread do working 0
  > fun thread do working 1
  > fun thread do working 2
  > fun thread do working 3
  > fun thread do working 4
  > retp: 0x3
  > main thread exit.

  实现一个双线程打印机：

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  
  void *fun1(void *arg)
  {
      int i = 0;
      for (i = 'A'; i <= 'Z'; i++)
      {
          putchar(i);
          fflush(stdout); // 刷新标准输出缓存区，让缓冲区中的信息显示到终端上
          usleep(100000); // 100ms
      }
  
      return NULL;
  }
  
  void *fun2(void *arg)
  {
      int i = 0;
      for (i = 'a'; i <= 'z'; i++)
      {
          putchar(i);
          fflush(stdout);
          usleep(100000); // 100ms
      }
  
      return NULL;
  }
  
  // 模拟输出字符
  int main(void)
  {
      pthread_t tid1, tid2;
  
      // 创建两个线程
      pthread_create(&tid1, NULL, fun1, NULL);
      pthread_create(&tid2, NULL, fun2, NULL);
  
      // 等待两个线程结束
      pthread_join(tid1, NULL);
      pthread_join(tid2, NULL);
  
      printf("\nmain thread exit...\n");
  
      return 0;
  }
  ```

  > omo@jomo-virtual-machine:~/linux_server/thread$ gcc printer.c -pthread 
  > jomo@jomo-virtual-machine:~/linux_server/thread$ ./a.out 
  > AabBCcdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ
  > main thread exit...
  > jomo@jomo-virtual-machine:~/linux_server/thread$ 

- 线程分离

  ![image-20231203185732338](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203185732338.png)

  pthread_detach函数：

  ![image-20231203185841354](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203185841354.png)

  但是如果主线程退出了，其余线程都无法继续执行了，因为所有子线程都是共用进程的资源（主线程退出，进程也退出了）

- 线程退出

  ![image-20231203190341897](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203190341897.png)

  pthread_exit函数：

  ![image-20231203190521521](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203190521521.png)

- 线程取消

  只能取消同一个进程中的线程

  pthread_cancel函数：

  ![image-20231203190625056](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203190625056.png)



**线程属性**

- 概述

  ![image-20231203191103855](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191103855.png)

  ![image-20231203191138983](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191138983.png)

- 线程属性初始化和销毁

  ![image-20231203191316122](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191316122.png)

- 线程分离状态设置

  ![image-20231203191418908](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191418908.png)

  ![image-20231203191456823](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191456823.png)

- 线程栈地址设置

  ![image-20231203191625297](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191625297.png)

  ![image-20231203191649020](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191649020.png)

- 线程栈大小设置

  ![image-20231203191719672](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191719672.png)

- 线程使用注意事项

  ![image-20231203191745169](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231203191745169.png)



### 13. 锁

**互斥锁**

- 同步与互斥

  ![image-20231207140706622](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207140706622.png)

- 为什么需要互斥锁

  ![image-20231207141302612](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207141302612.png)

- 互斥锁Mutex

  ![](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207141621048.png)

- pthread_mutex_init函数

  ![image-20231207150846012](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207150846012.png)

  restrict，C语言中的一种类型限定符（Type Qualifiers），用于告诉编译器，对象已经被指针所引用，不能通过该指针外所有其他直接或间接的方式修改该对象的内容

- pthread_mutex_destroy函数

  ![image-20231207151217124](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207151217124.png)

- pthread_mutex_lock函数

  ![image-20231207151542498](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207151542498.png)

- pthread_mutex_unlock函数

  ![image-20231207151551769](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207151551769.png)

- 测试程序

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  
  // 全局变量作用域是从声明那行直至文件末端，比如mutex作用域从第11行开始直至最后一行
  
  // 互斥锁变量
  pthread_mutex_t mutex;
  
  void *fun1(void *arg)
  {
      int i = 0;
  
      // 加锁
      pthread_mutex_lock(&mutex);
  
      // 临界区代码：
      for (i = 'A'; i <= 'Z'; i++)
      {
          putchar(i);
          fflush(stdout); // 刷新标准输出缓存区，让缓冲区中的信息显示到终端上
          usleep(100000); // 100ms
      }
  
      // 解锁
      pthread_mutex_unlock(&mutex);
  
      return NULL;
  }
  
  void *fun2(void *arg)
  {
      int i = 0;
  
      // 加锁
      pthread_mutex_lock(&mutex);
  
      // 临界区代码：
      for (i = 'a'; i <= 'z'; i++)
      {
          putchar(i);
          fflush(stdout);
          usleep(100000); // 100ms
      }
  
      // 解锁
      pthread_mutex_unlock(&mutex);
  
      return NULL;
  }
  
  int num;
  
  // 模拟输出字符(互斥)
  int main(void)
  {
      int ret = -1;
      pthread_t tid1, tid2;
  
      // 初始化一个互斥量，即互斥锁
      ret = pthread_mutex_init(&mutex, NULL);
      if (0 != ret)
      {
          printf("pthread_mutex_init failed...\n");
          return 1;
      }
      printf("初始化一个互斥锁成功...\n");
  
      // 创建两个线程
      pthread_create(&tid1, NULL, fun1, NULL);
      pthread_create(&tid2, NULL, fun2, NULL);
  
      // 等待两个线程结束
      pthread_join(tid1, NULL);
      pthread_join(tid2, NULL);
  
      printf("\nmain thread exit...\n");
  
      // 销毁互斥锁
      pthread_mutex_destroy(&mutex);
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/lock$ gcc printer_mutex.c 
  > jomo@jomo-virtual-machine:~/linux_server/lock$ ./a.out 
  > 初始化一个互斥锁成功...
  > ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz
  > main thread exit...
  > jomo@jomo-virtual-machine:~/linux_server/lock$ 

- 死锁

  ![image-20231207155255813](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207155255813.png)

  ![image-20231207155429131](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207155429131.png)

  ![image-20231207155516615](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207155516615.png)

  ![image-20231207155612226](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207155612226.png)



**读写锁**

- 读写锁概述

  ![image-20231207160146395](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207160146395.png)

- pthread_rwlock_init函数

  ![image-20231207171100576](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207171100576.png)

- pthread_rwlock_destroy函数

  ![image-20231207171141496](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207171141496.png)

- pthread_rwlock_rdlock函数

  ![image-20231207171203134](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207171203134.png)

- pthread_rwlock_wrlock函数

  ![image-20231207171341462](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207171341462.png)

- pthread_rwlock_unlock函数

  ![image-20231207171806662](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231207171806662.png)

  可以解开读锁或写锁的，因为读锁或写锁肯定不会同时存在，即读写锁必然只会处于一种状态读锁或写锁

- 测试程序

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  #include <bits/pthreadtypes.h>
  
  // 读写锁变量
  pthread_rwlock_t rwlock;
  
  // 全局变量
  int num = 0;
  
  // 读线程
  void *fun_read(void *arg)
  {
      // 获取线程的编号
      int index = (int)(long)arg;
  
      while (1)
      {
          // 加读写锁读锁
          pthread_rwlock_rdlock(&rwlock);
  
          printf("读线程%d 读取num的值 %d\n", index, num);
  
          // 解锁
          pthread_rwlock_unlock(&rwlock);
  
          // 随机睡眠1~3秒
          sleep(random() % 3 + 1);
      }
  
      return NULL;
  }
  
  void *fun_write(void *arg)
  {
      // 获取线程的编号
      int index = (int)(long)arg;
  
      while (1)
      {
          // 加读写锁写锁
          pthread_rwlock_wrlock(&rwlock);
  
          num++;
          printf("写线程%d 修改num的值 %d\n", index, num);
  
          // 随机睡眠1~3秒
          sleep(random() % 3 + 1);
  
          // 解锁
          pthread_rwlock_unlock(&rwlock);
      }
  
      return NULL;
  }
  
  // 写线程
  
  int main(void)
  {
      int i = 0;
      int ret = -1;
      pthread_t tid[8];
  
      // 初始化读写锁
      ret = pthread_rwlock_init(&rwlock, NULL);
      if (0 != ret)
      {
          printf("pthread_rwlock_init failed...\n");
          return 1;
      }
  
      // 设置随机种子
      srandom(getpid());
  
      // 创建8个线程
      for (i = 0; i < 8; i++)
      {
          if (i < 5)
          {
              // 创建读线程
              pthread_create(&tid[i], NULL, fun_read, (void *)(long)i);
          }
          else
          {
              // 创建写线程
              pthread_create(&tid[i], NULL, fun_write, (void *)(long)i);
          }
      }
  
      // 回收8个线程资源
      for (i = 0; i < 8; i++)
      {
          pthread_join(tid[i], NULL);
      }
  
      // 销毁读写锁
      pthread_rwlock_destroy(&rwlock);
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/lock$ ./a.out 
  > 读线程0 读取num的值 0
  > 读线程2 读取num的值 0
  > 读线程1 读取num的值 0
  > 读线程3 读取num的值 0
  > 读线程4 读取num的值 0
  > 写线程6 修改num的值 1
  > 写线程6 修改num的值 2
  > 读线程4 读取num的值 2
  > 读线程2 读取num的值 2
  > 读线程3 读取num的值 2
  > 读线程0 读取num的值 2
  > 读线程1 读取num的值 2
  > 写线程6 修改num的值 3
  > 读线程1 读取num的值 3
  > 写线程6 修改num的值 4

  写比读的优先级更高，假设现在有一个线程拿到了写锁，然后其它线程申请了写锁和读锁，最终申请写锁那个线程会获取到写锁，因为写的优先级更高




### 14. 条件变量

- 条件变量概述

  ![image-20231208153055247](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208153055247.png)

- pthread_cond_init函数

  ![image-20231208153250415](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208153250415.png)

- pthread_cond_destroy函数

  ![image-20231208153344832](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208153344832.png)

- pthread_cond_wait函数

  ![image-20231208153531512](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208153531512.png)

  ![image-20231208153557427](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208153557427.png)

  ![image-20231208153649628](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208153649628.png)

- pthread_cond_signal函数

  ![image-20231208153851373](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208153851373.png)

- 测试程序

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  
  int flag = 0;
  
  // 互斥量
  pthread_mutex_t mutex;
  // 条件变量
  pthread_cond_t cond;
  
  // 改变条件的线程
  void *fun1(void *arg)
  {
      while (1)
      {
          // 加锁
          pthread_mutex_lock(&mutex);
          flag = 1;
  
          // 先解锁再唤醒其它线程，否则可能会出现其它线程阻塞在获取锁
          // 解锁
          pthread_mutex_unlock(&mutex);
  
          // 唤醒因为条件而阻塞线程
          pthread_cond_signal(&cond);
  
          sleep(2);
      }
  
      return NULL;
  }
  
  // 等待条件的线程
  void *fun2(void *arg)
  {
      while (1)
      {
          // 加锁
          pthread_mutex_lock(&mutex);
  
          // 表示条件不满足
          if (0 == flag)
          {
              // 等待条件不满足，会阻塞并释放mutex
              // 被唤醒之后，解放阻塞并重新申请mutex
              pthread_cond_wait(&cond, &mutex);
          }
          printf("线程二因为条件满足 开始运行...\n");
          flag = 0;
  
          // 解锁
          pthread_mutex_unlock(&mutex);
      }
  
      return NULL;
  }
  
  // 条件变量的应用
  int main(void)
  {
      int ret = -1;
      pthread_t tid1, tid2;
  
      // 初始化条件变量
      ret = pthread_cond_init(&cond, NULL);
      if (0 != ret)
      {
          printf("pthread_cond_init failed..\n");
          return 1;
      }
  
      // 初始化互斥量
      ret = pthread_mutex_init(&mutex, NULL);
      if (0 != ret)
      {
          printf("pthread_mutex_init failed..\n");
          return 1;
      }
  
      // 创建两个线程
      pthread_create(&tid1, NULL, fun1, NULL);
      pthread_create(&tid2, NULL, fun2, NULL);
  
      // 回收线程资源
      ret = pthread_join(tid1, NULL);
      if (0 != ret)
      {
          printf("pthread_join failed...\n");
          return 1;
      }
  
      ret = pthread_join(tid2, NULL);
      if (0 != ret)
      {
          printf("pthread_join failed...\n");
          return 1;
      }
  
      // 销毁互斥量
      pthread_mutex_destroy(&mutex);
  
      // 销毁条件变量
      pthread_cond_destroy(&cond);
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/cond$ gcc pthread_cond_init.c 
  > jomo@jomo-virtual-machine:~/linux_server/cond$ ./a.out 
  > 线程二因为条件满足 开始运行...
  > 线程二因为条件满足 开始运行...
  > 线程二因为条件满足 开始运行...
  > 线程二因为条件满足 开始运行...

- 生产者和消费者条件变量模型

  ![image-20231208160933656](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208160933656.png)

  生产者消费者模型代码：

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  
  typedef struct _node_t
  {
      int data;
      struct _node_t *next; // 这里必须使用原始结构名声明，因为此时还不知道别名node_t
  } node_t;
  
  node_t *head = NULL;
  
  // 互斥量
  pthread_mutex_t mutex;
  // 条件变量
  pthread_cond_t cond;
  
  // 生产者线程
  void *producer(void *arg)
  {
      // 循环生产产品
      while (1)
      {
          // 加锁
          pthread_mutex_lock(&mutex);
  
          // 分配节点空间
          node_t *new = malloc(sizeof(node_t));
          if (NULL == new)
          {
              printf("malloc failed...\n");
              break;
          }
          memset(new, 0, sizeof(node_t));
          new->data = random() % 100 + 1;
          new->next = NULL;
          printf("生产者生产产品 %d\n", new->data);
  
          // 头插法
          new->next = head;
          head = new;
  
          // 解锁
          pthread_mutex_unlock(&mutex);
  
          // 唤醒因为条件变量而阻塞的线程
          pthread_cond_signal(&cond);
  
          // 随机睡眠
          sleep(random() % 3 + 1);
      }
  
      pthread_exit(NULL);
  }
  
  // 消费者线程
  void *customer(void *arg)
  {
      node_t *tmp = NULL;
      // 循环消费
      while (1)
      {
          // 加锁
          pthread_mutex_lock(&mutex);
  
          if (NULL == head)
          {
              // 等待...
              pthread_cond_wait(&cond, &mutex);
          }
          else
          {
              // 消费第一个节点
              tmp = head;
              head = head->next;
              printf("消费者消费 %d\n", tmp->data);
              free(tmp);
          }
  
          // 解锁
          pthread_mutex_unlock(&mutex);
  
          // 睡眠1~3秒
          sleep(random() % 3 + 1);
      }
  
      pthread_exit(NULL);
  }
  
  // 生产者和消费者模型 条件变量的模型
  int main(void)
  {
      int ret = -1;
  
      pthread_t tid1 = -1, tid2 = -1;
  
      // 设置随机种子
      srandom(getpid());
  
      // 初始化条件变量
      ret = pthread_cond_init(&cond, NULL);
      if (0 != ret)
      {
          printf("pthread_cond_init failed...\n");
          return 1;
      }
  
      // 初始化互斥量
      ret = pthread_mutex_init(&mutex, NULL);
      if (0 != ret)
      {
          printf("pthread_mutexinit failed...\n");
          return 1;
      }
  
      // 创建两个线程
      // 生产者线程
      pthread_create(&tid1, NULL, producer, NULL);
      // 消费者线程
      pthread_create(&tid2, NULL, customer, NULL);
  
      // 等待两个线程结束
      pthread_join(tid1, NULL);
      pthread_join(tid2, NULL);
  
      // 销毁
      pthread_mutex_destroy(&mutex);
      pthread_cond_destroy(&cond);
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/cond$ gcc prodecer_cond.c 
  > jomo@jomo-virtual-machine:~/linux_server/cond$ ./a.out 
  > 生产者生产产品 72
  > 消费者消费 72
  > 生产者生产产品 38
  > 消费者消费 38
  > 生产者生产产品 8
  > 生产者生产产品 39
  > 消费者消费 39
  > 生产者生产产品 79
  > 消费者消费 79
  > 生产者生产产品 6
  > 消费者消费 6
  > 消费者消费 8
  > 生产者生产产品 12
  > 消费者消费 12
  > 生产者生产产品 16
  > 消费者消费 16
  > ^C
  > jomo@jomo-virtual-machine:~/linux_server/cond$ 

- 条件变量优缺点

  ![image-20231208170201167](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170201167.png)



### 15. 信号量

- 信号量概述

  ![image-20231208170319180](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170319180.png)

  ![image-20231208170420582](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170420582.png)

  ![image-20231208170448708](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170448708.png)

- sem_init函数

  ![image-20231208170513704](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170513704.png)

- sem_destroy函数

  ![image-20231208170555384](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170555384.png)

- 信号量P操作（减1）

  ![image-20231208170622644](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170622644.png)

  ![image-20231208170701475](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170701475.png)

- 信号量V操作（加1）

  ![image-20231208170727479](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170727479.png)

- 获取信号量的值

  ![image-20231208170914736](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208170914736.png)

- 信号量用于互斥

  测试程序：

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  #include <semaphore.h>
  
  // 信号量变量
  sem_t sem;
  
  void *fun1(void *arg)
  {
      int i = 0;
  
      // 申请资源 将可用资源减1
      sem_wait(&sem);
  
      // 临界区代码：
      for (i = 'A'; i <= 'Z'; i++)
      {
          putchar(i);
          fflush(stdout); // 刷新标准输出缓存区，让缓冲区中的信息显示到终端上
          usleep(100000); // 100ms
      }
  
      // 释放资源 将可用资源加1
      sem_post(&sem);
  
      return NULL;
  }
  
  void *fun2(void *arg)
  {
      int i = 0;
  
      // 申请资源 将可用资源减1
      sem_wait(&sem);
  
      // 临界区代码：
      for (i = 'a'; i <= 'z'; i++)
      {
          putchar(i);
          fflush(stdout);
          usleep(100000); // 100ms
      }
  
      // 释放资源 将可用资源加1
      sem_post(&sem);
  
      return NULL;
  }
  
  int num;
  
  // 模拟输出字符(互斥)
  int main(void)
  {
      int ret = -1;
      pthread_t tid1, tid2;
  
      // 初始化一个信号量
      ret = sem_init(&sem, 0, 1);
      if (0 != ret)
      {
          printf("sem_init failed...\n");
          return 1;
      }
      printf("初始化一个信号量成功...\n");
  
      // 创建两个线程
      pthread_create(&tid1, NULL, fun1, NULL);
      pthread_create(&tid2, NULL, fun2, NULL);
  
      // 等待两个线程结束
      pthread_join(tid1, NULL);
      pthread_join(tid2, NULL);
  
      printf("\nmain thread exit...\n");
  
      // 销毁信号量
      sem_destroy(&sem);
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/semaphore$ gcc printer_sem.c  -pthread
  > jomo@jomo-virtual-machine:~/linux_server/semaphore$ ./a.out 
  > 初始化一个信号量成功...
  > ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz
  > main thread exit...
  > jomo@jomo-virtual-machine:~/linux_server/semaphore$

- 信号量用于生产者和消费者模型

  ```c
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  
  #include <pthread.h>
  #include <semaphore.h>
  
  typedef struct _node_t
  {
      int data;
      struct _node_t *next; // 这里必须使用原始结构名声明，因为此时还不知道别名node_t
  } node_t;
  
  node_t *head = NULL;
  
  // 信号量1 (生产者资源: 存放商品容器的个数（容器放满了就不能继续生产了）)
  sem_t sem1;
  
  // 信号量2 (产品资源：可以消费的产品个数)
  sem_t sem2;
  
  // 生产者线程
  void *producer(void *arg)
  {
      // 循环生产产品
      while (1)
      {
          // 申请生产者资源，生产者资源减1
          sem_wait(&sem1);
  
          // 分配节点空间
          node_t *new = malloc(sizeof(node_t));
          if (NULL == new)
          {
              printf("malloc failed...\n");
              break;
          }
          memset(new, 0, sizeof(node_t));
          new->data = random() % 100 + 1;
          new->next = NULL;
          printf("生产者生产产品 %d\n", new->data);
  
          // 头插法
          new->next = head;
          head = new;
  
          // 产品资源加1
          sem_post(&sem2);
  
          // 随机睡眠
          sleep(random() % 3 + 1);
      }
  
      pthread_exit(NULL);
  }
  
  // 消费者线程
  void *customer(void *arg)
  {
      node_t *tmp = NULL;
      // 循环消费
      while (1)
      {
          // 申请产品资源 产品资源减1
          sem_wait(&sem2);
  
          // 消费第一个节点
          tmp = head;
          head = head->next;
          printf("消费者消费 %d\n", tmp->data);
          free(tmp);
  
          // 释放生产者资源
          sem_post(&sem1);
  
          // 睡眠1~3秒
          sleep(random() % 3 + 1);
      }
  
      pthread_exit(NULL);
  }
  
  // 生产者和消费者模型 条件变量的模型
  int main(void)
  {
      int ret = -1;
  
      pthread_t tid1 = -1, tid2 = -1;
  
      // 设置随机种子
      srandom(getpid());
  
      // 初始化信号量1，一开始没有产品，因此只有生产者有资源，产品资源等于0
      ret = sem_init(&sem1, 0, 4);
      if (0 != ret)
      {
          printf("sem_init failed...\n");
          return 1;
      }
  
      // 初始化信号量2
      ret = sem_init(&sem2, 0, 0);
      if (0 != ret)
      {
          printf("sem_init failed...\n");
          return 1;
      }
  
      // 创建两个线程
      // 生产者线程
      pthread_create(&tid1, NULL, producer, NULL);
      // 消费者线程
      pthread_create(&tid2, NULL, customer, NULL);
  
      // 等待两个线程结束
      pthread_join(tid1, NULL);
      pthread_join(tid2, NULL);
  
      // 销毁
      sem_destroy(&sem1);
      sem_destroy(&sem2);
  
      return 0;
  }
  ```

  > jomo@jomo-virtual-machine:~/linux_server/semaphore$ gcc producer_sem.c -pthread 
  > jomo@jomo-virtual-machine:~/linux_server/semaphore$ ./a.out 
  > 生产者生产产品 66
  > 消费者消费 66
  > 生产者生产产品 41
  > 消费者消费 41
  > 生产者生产产品 48
  > 消费者消费 48
  > 生产者生产产品 66
  > 消费者消费 66
  > 生产者生产产品 90
  > 消费者消费 90
  > 生产者生产产品 5
  > 生产者生产产品 81
  > 生产者生产产品 80
  > 消费者消费 80
  > 消费者消费 81
  > 生产者生产产品 41
  > 消费者消费 41
  > ^C
  > jomo@jomo-virtual-machine:~/linux_server/semaphore$ 

- 哲学家就餐问题

  ![image-20231208180140397](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231208180140397.png)



## 网络编程

### 1. 网络基础

**地址**

- 网卡

  ![image-20231209143708377](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209143708377.png)

- MAC地址

  网卡的id，理论上该id是全球唯一。

  一般用来标识主机的id，这个id是物理地址，不会改变的

  6字节，48位

- IP地址

  标识主机的id，这个id是虚拟地址，是会改变的

  一个IP将其分为子网id和主机id

  子网id和主机id需要和子网掩码一起工作

  ipv4:：4字节，32位

  ipv6：16字节，128位

  ![image-20231209144614520](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209144614520.png)

  ![image-20231209145025380](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209145025380.png)

  A：广域网

  B：城域网

  C：局域网

  

  Linux下设置ip命令

  > ifocnfig ens 33 \<ip> netmask \<netmask>



**桥接和NAT**

- 桥接模式

  虚拟机可以和同一局域网下的其它主机通信

- NAT模式

  虚拟机和同一局域网下的通信只能通过宿主机来完成



**端口**

用于标识应用程序（进程），不用进程号来标识是因为进程号在每次开机之后不一样，而端口是可重用的

prot：2字节 0-65535

0-1023：知名端口（不要用）

自定义端口：1024-65535



**OSI七层模型**

![image-20231209151324007](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209151324007.png)

- 物理层

  定义物理设备标准

- 数据链路层

  负责收发数据（通过MAC地址区分给哪个主机）

- 网络层

  给两台主机提供路径选择（通过IP地址区分给哪个主机）

- 传输层

  区分数据递送到哪一个应用程序（通过port区分给哪个应用程序）

- 会话层

  建立连接

- 表示层

  解码

- 应用层



**TCP/IP模型**

![image-20231209151942589](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209151942589.png)



**协议**

- 应用层协议

  - FTP：文件传输协议
  - HTTP：超文本传输协议
  - NFS：网络文本系统

- 传输层协议

  - **TCP：传输控制协议**

    ![image-20231209152934181](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209152934181.png)

    ![image-20231209153114059](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209153114059.png)

    ![image-20231209153323779](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209153323779.png)

  - **UDP：用户数据报协议**

    ![image-20231209152647230](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209152647230.png)

- 网络层协议

  - **IP：因特网互联协议**

    ![image-20231209153343715](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209153343715.png)

    ![image-20231209153604584](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209153604584.png)

    ![image-20231209153620549](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209153620549.png)

  - ICMP：因特网控制报文协议 ping

  - IGMP：因特网组管理协议

- 链路层协议

  ![](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209153727079.png)

  - **ARP：地址解析协议 通过IP地址找MAC地址**

    ![image-20231209160225146](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209160225146.png)

    ![image-20231209160333761](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209160333761.png)

  - RARP：反向地址解析协议 通过MAC地址找IP地址

**网络通信过程**

![image-20231209155740338](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209155740338.png)



**网络设计模式**

- B/S：browser/server

  服务器计算，性能较低，客户端安全，开发周期短

- C/S：client/server

  本地计算，性能更好，客户端容易篡改数据，开发周期较长



### 2. Socket编程

**Socket套接字**

解决不同主机间进程通信的方法

![image-20231209212504837](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209212504837.png)

![image-20231209212714502](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209212714502.png)



**网络字节序**

小端：低位存于低地址

大端：高位存于低地址

![image-20231209213253176](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209213253176.png)



**字节序转换**

可调用库函数转换网络字节序和主机字节序

```c
#include <arpa/inet.h>

// 主机字节序转网络字节序
uint32_t htonl(uint32_t hostlong);// 4字节，转IP地址
uint16_t htons(uint16_t hostshort);// 2字节，转端口

//网络字节序转主机字节序
uint32_t ntohl(uint32_t netlong);// 4字节
uint16_t ntohs(uint16_t netshort);// 2字节
```

测试程序：

```c
#include <stdio.h>

#include <arpa/inet.h>

int main(void)
{
    char buf[4] = {192, 168, 1, 2};
    int num = *(int *)buf;
    int sum = htonl(num);
    unsigned char *p = (unsigned char *)&sum;
    printf("%d.%d.%d.%d\n", *p, *(p + 1), *(p + 2), *(p + 3));

    unsigned short a = 0x0102;
    unsigned short b = htons(a);

    printf("%x\n", b);

    return 0;
}
```

> jomo@jomo-virtual-machine:~/linux_server/socket$ gcc htonl.c 
> jomo@jomo-virtual-machine:~/linux_server/socket$ ./a.out 
> 2.1.168.192
> 201
> jomo@jomo-virtual-machine:~/linux_server/socket$ 





**IP地址转换**

![image-20231209220149817](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231209220149817.png)

```c
#include <arpa/inet.h>
int inet_pton(int af, const char *src, void *dst);

功能：将点分十进制串转成32位网络大端的数据("192.168.1.2" ==> )
参数：
    af：
    	AF_INET  IPV4
    	AF_INET6 IPV6
    src:点分十进制串的首地址
    dst:32位网络数据的地址
返回值：
      1:成功
      0:主机地址无效
      -1:af不包含一个有效的地址，并且errno会被设置为EAFNOSUPPORT
```

```c
#include <arpa/inet.h>
const char *inet_ntop(int af, const void *src, char *dst, socketlen_t size) ;

功能：将32位大端的网络数据转成点分十进制
参数：
    af：
    	AF_INET  IPV4
    	AF_INET6 IPV6
    src:32位网络数据的地址
    dst:点分十进制串的首地址
    size:存储点分十进制串数组的大小(最大16字节即可，"255.255.255\0"(16字节))
返回值：
      存储点分十进制串数组首地址
```

测试程序：

```c
#include <stdio.h>
#include <arpa/inet.h>

int main(void)
{
    char buf[] = "192.168.1.4"; // 点分十进制串
    unsigned int num = 0;
    inet_pton(AF_INET, buf, &num);
    unsigned char *p = (unsigned char *)&num;
    printf("%d %d %d %d\n", *p, *(p + 1), *(p + 2), *(p + 3));

    char ip[INET_ADDRSTRLEN] = "";
    const char *ip_str = inet_ntop(AF_INET, &num, ip, INET_ADDRSTRLEN);
    printf("%s\n", ip_str);

    return 0;
}
```

> jomo@jomo-virtual-machine:~/linux_server/socket$ gcc inet_pton.c 
> jomo@jomo-virtual-machine:~/linux_server/socket$ ./a.out 
> 192 168 1 4
> 192.168.1.4
> jomo@jomo-virtual-machine:~/linux_server/socket$ 



**网络通信三大问题**

协议、IP、端口，将这三个封装到sockaddr结构体中



IPV4套接字结构体

```c
 struct sockaddr_in {
     sa_family_t    sin_family; /* address family: AF_INET */
     in_port_t      sin_port;   /* port in network byte order */
     struct in_addr sin_addr;   /* internet address */
 };

/* Internet address. */
struct in_addr {
    uint32_t       s_addr;     /* address in network byte order */
};
```

IPV6套接字结构体

```c
struct sockaddr_in6 {
    sa_family_t     sin6_family;   /* AF_INET6 */
    in_port_t       sin6_port;     /* port number */
    uint32_t        sin6_flowinfo; /* IPv6 flow information */
    struct in6_addr sin6_addr;     /* IPv6 address */
    uint32_t        sin6_scope_id; /* Scope ID (new in 2.4) */
};

struct in6_addr {
    unsigned char   s6_addr[16];   /* IPv6 address */
};
```

通用套接字结构体

```c
struct sockaddr{
    sa_family_t sa_family;	/*address family,AF_xxx*/
    char sa_data[4];		/*14 bytes of protocol address*/
}
```

调用send函数时，第二个参数需要把IPV4/6套接字结构体转成通用套接字结构体（把自己结构体的协议成员变量传给赋给通用套接字结构体的协议成员变量），这么做的目的是IPV4/6可以共用一个send函数，不需要为它们各写一个



**TCP客户端**

TCP协议特点：出错重传，每次发送数据对方收到后会回复ACK，可靠传输



TCP是抽象打电话的模型：

建立连接 使用连接 关闭连接

![img](https://img-blog.csdnimg.cn/20200616161409442.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L01yX1hKQw==,size_16,color_FFFFFF,t_70)

- 创建套接字

  ```c
  #include <sys/types.h>          /* See NOTES */
  #include <sys/socket.h>
  
  int socket(int domain, int type, int protocol);
  功能：
      创建套接字
  参数：
      domain:AF_INET
  	type:SOCK_STREAM 流式套接字，用于TCP通信
  	protocol:0
  返回值：
       成功返回文件描述符
       失败返回-1
  ```

- 连接服务器

  ```c
  #include <sys/types.h>          /* See NOTES */
  #include <sys/socket.h>
  
  int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  功能：
      连接服务器
  参数：
      sockfd:socket套接字
  	addr:IPV4套接字结构体的地址
  	addrlen:IPV4套接字结构体的长度
  返回值：
       成功返回0
       失败返回-1
  ```

客户端代码：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    int sock_fd = -1;
    // 创建套接字
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080); // 端口转大端
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr));

    // 读写数据
    char buf[1024] = "";
    while (1)
    {
        bzero(buf, sizeof(buf));
        int n = read(STDIN_FILENO, buf, sizeof(buf));
        write(sock_fd, buf, n); // 发送数据给服务器
    }

    // 关闭连接
    close(sock_fd);

    return 0;
}
```



**TCP服务器通信流程**

![image-20231210222715850](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231210222715850.png)



![image-20231210222622492](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231210222622492.png)

- bind

  ![image-20231210223019869](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231210223019869.png)

- listen

  ![image-20231210223132187](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231210223132187.png)

- accept

  ![image-20231210223824600](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231210223824600.png)



**TCP服务器通信步骤**

![image-20231210223653744](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231210223653744.png)



**TCP服务器代码**

客户端代码：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    int sock_fd = -1;
    // 创建套接字
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080); // 端口转大端
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr));

    // 读写数据
    char buf[1024] = "";
    while (1)
    {
        bzero(buf, sizeof(buf));
        int n = read(STDIN_FILENO, buf, sizeof(buf));
        write(sock_fd, buf, n); // 发送数据给服务器
    }

    // 关闭连接
    close(sock_fd);

    return 0;
}
```

服务器代码：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    // 创建监听套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    // 绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    // addr.sin_addr.s_addr = INADDR_ANY; // 绑定的是通配地址（本机所有IP）
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0){
        perror("");
        0;
    }

    // 监听
    listen(lfd, 128); // 已完成连接队列和未完成连接队列的连接和最大值为128

    // 提取
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len); // 获取连接套接字

    // 读写
    char buf[1024] = "";
    while (1)
    {
        bzero(buf, sizeof(buf));
        read(cfd, buf, sizeof(buf)); // 如果read返回值是0，代表客户端关闭了
        printf("Server received: %s", buf);
    }

    // 关闭
    close(lfd);
    close(cfd);

    return 0;
}
```

![image-20231210232938895](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231210232938895.png)

上面的代码不稳定，因为并没有接收返回值并对其进行判断



**包裹函数**

![image-20231211140601678](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231211140601678.png)



![image-20231211140652942](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231211140652942.png)



![image-20231211140856762](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231211140856762.png)



![image-20231211140929249](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231211140929249.png)



![image-20231211140936389](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231211140936389.png)



函数名只是把原先的真正调用的函数名首字母大写而已，这样man Bind也可以查到bind，不区分大小写



黏包：

无法区分数据包的顺序



![image-20231211141236352](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231211141236352.png)



![image-20231211141258141](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231211141258141.png)



![image-20231211141444864](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231211141444864.png)



### 3. TCP

**TCP通信时序**

- **三次握手**：TCP建立连接的过程，只能由客户端发起连接

  ![image-20231215231403420](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231215231403420.png)

  确认序列号ack：既是确认收到对方的报文，也是期望下一次对方的序列号seq

  ack=对方发送过来的序列号seq+标志位长度SYN(如果待确认的报文是SYN报文的话，长度是1，否则是0)+标志位长度FIN+数据长度

- **四次挥手**：TCP断开连接的过程，客户端和服务端都可以发起断开

  ![image-20231215234320749](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231215234320749.png)

  只有主动方会等待关闭



### 4. 滑动窗口

**MSS**

最大报文长度：一般出现在三次握手的前两次，用来告知对方发送数据的最大长度



**MTU**

最大传输单元 和网卡有关



滑动窗口可以用来协商双方一次发送/接收的字节数，每一次读取数据后，回复的ACK报文中会携带当前缓冲区大小，用来告知对方



### 5. 多进程并发服务器

流程：

1. 创建套接字

2. 绑定

3. 监听

4. 提取：

   while(1){

   ​	提取连接

   ​	fork创建子进程

   ​	子进程中关闭lfd（监听套接字），服务客户端

   ​	父进程中关闭cfd（连接套接字），回收子进程资源

   }

5. 关闭

```c
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "wrap.h"

#include <signal.h>
#include <sys/wait.h>
#

void freeprocess(int signum)
{
    pid_t pid;
    while (1)
    {
        pid = waitpid(-1, NULL, WNOHANG);
        if (pid <= 0)
        {
            // 小于0 => 子进程全部退出了
            // 等于0 => 没有进程退出
            break;
        }
        else
        {
            printf("child pid = %d\n", pid);
        }
    }
}

int main(void)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    // 创建监听套接字，绑定所有本机IP的8000端口
    int lfd = tcp4bind(8000, NULL);

    // 监听
    Listen(lfd, 128);

    // 循环提取
    // 回射
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    while (1)
    {
        char ip[16] = "";
        // 提取连接
        int cfd = Accept(lfd, (struct sockaddr *)&cliaddr, &len);
        printf("new client ip = %s port=%d\n", inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, 16), ntohs(cliaddr.sin_port));

        // fork创建子进程
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            perror("");
            exit(0);
        }
        else if (pid == 0)
        {
            while (1)
            {
                // 子进程
                // 关闭lfd
                close(lfd);
                char buf[1024] = "";
                int n = read(cfd, buf, sizeof(buf));
                if (n < 0)
                {
                    perror("");
                    close(cfd);
                    exit(0);
                }
                else if (n == 0) // 对方关闭
                {
                    printf("client close\n");
                    close(cfd);
                    break;
                }
                else
                {
                    printf("%s\n", buf);
                    write(cfd, buf, n);
                }
            }
        }
        else
        {
            // 父进程
            close(cfd);
            // 回收
            // 注册信号回调
            // 注意考虑子进程在信号注册前挂掉的情况（先阻塞，注册完后再处理）
            signal(SIGCHLD, freeprocess);

            // 注册完后，解除阻塞
            sigprocmask(SIG_UNBLOCK, &set, NULL);
        }
    }

    // 关闭

    return 0;
}
```



### 6. 多线程并发服务器

```c
#include <stdio.h>
#include <pthread.h>
#include "wrap.h"

typedef struct c_info
{
    int cfd;
    struct sockaddr_in cliaddr;
} CINFO;

void *client_fun(void *arg)
{
    CINFO *info = (CINFO *)arg;
    char ip[16] = "";
    printf("new client ip=%s port=%d\n", inet_ntop(AF_INET, &(info->cliaddr.sin_addr.s_addr), ip, 16), ntohs(info->cliaddr.sin_port));

    while (1)
    {
        char buf[1024] = "";
        int count = 0;
        count = read(info->cfd, buf, sizeof(buf));
        if (count < 0)
        {
            perror("");
            break;
        }
        else if (count == 0)
        {
            printf("client close\n");
            break;
        }
        else
        {
            printf("%s\n", buf);
            write(info->cfd, buf, count);
        }
    }
    close(info->cfd);
    free(info);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("argc<2???\n  ./a.out 8000 \n");
        return 0;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // 设置线程退出自动释放资源

    short port = atoi(argv[1]);
    int lfd = tcp4bind(port, NULL); // 创建套接字，绑定

    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    Listen(lfd, 128);
    CINFO *info;
    while (1)
    {
        int cfd = Accept(lfd, (struct sockaddr *)&cliaddr, &len);
        pthread_t pthid;
        // 新来一个客户端就新开辟堆空间存储客户端信息，防止多个线程共用同一个info，导致覆盖问题
        info = malloc(sizeof(CINFO));
        info->cfd = cfd;
        info->cliaddr = cliaddr;
        pthread_create(&pthid, &attr, client_fun, info);
    }

    return 0;
}
```

### 7. TCP状态转换

![TCP状态转换图](https://img-blog.csdn.net/20160423144456154)



![image-20231223125029731](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231223125029731.png)



**半关闭**

![image-20231223130019478](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231223130019478.png)



### 8. 心跳包

如果对方异常断开，本机检测不到，一直等待，浪费资源

需要设置TCP的保持连接，作用就是每隔一定的时间间隔发送探测分节，如果连续发送多个还未收到对方的回复，就将此连接断开



![image-20231224113326608](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224113326608.png)

![image-20231224113252674](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224113252674.png)



![image-20231224113101261](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224113101261.png)

![image-20231224113146559](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224113146559.png)



**乒乓包：**携带比较多的数据的心跳包



### 9. 端口复用

端口重新启用

注意：程序中设置某个端口重新使用，在这之前的其他网络程序将不能使用这个端口



![image-20231224114156895](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224114156895.png)



### 10.多路IO转接技术

**高并发服务器：**

1. 阻塞等待：每个进/线程都在等待客户端发送消息，消耗资源

2. 非阻塞忙轮询：

   消耗CPU

   ![image-20231224115101372](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224115101372.png)

3. 多路IO转接（多路IO复用）：

   ![image-20231224115722972](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224115722972.png)

   内核监听多个文件描述符的属性（读写缓冲区）变化

   如果某个文件描述符的读缓冲区变化了，这个时候就可以读了，将这个事件告知应用层![image-20231224115420179](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224115420179.png)



### 11. selectAPI

![image-20231224120314420](https://md-jomo.oss-cn-guangzhou.aliyuncs.com/IMG/image-20231224120314420.png)

**注意：**变化的文件描述符会存在监听的集合中，未变化的文件描述符会被删除，所以下次监听的时候，需要把那些被删除的文件描述符重新加入集合中
