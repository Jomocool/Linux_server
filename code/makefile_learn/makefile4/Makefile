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
