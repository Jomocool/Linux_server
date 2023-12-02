#目标是all，依赖文件列表是test1、test2，如果test1和test2存在，就会执行echo "hello all"
all:test1 test2
	echo "hello all"

test1:
	echo "hello test1"

test2:
	echo "hello test2"
