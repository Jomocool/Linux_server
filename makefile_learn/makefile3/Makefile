OBJS = add.o sub.o mul.o div.o test.o

#$@ 表示目标
#$^ 表示所有依赖
#$< 表示第一个依赖

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
