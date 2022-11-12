# 指定编译器
CC=gcc

# -Wall 输出所有警告信息
# -lm 链接数学库
# -lpthread 连接线程库文件
CFLAGS:=-Wall -lpthread -lm -g

objects = main.o fun2.o fun1.o
target:$(objects)
	$(CC) -o target $(objects) $(CFLAGS)
main.o:main.c
	$(CC) -c main.c $(CFLAGS)
fun2.o:fun2.c
	$(CC) -c fun2.c $(CFLAGS)
fun1.o:fun1.c
	$(CC) -c fun1.c $(CFLAGS)

# .PHONY后面目标文件下的指令将被强制执行，即使该文件被手动修改过是最新的
.PHONY:clean
clean:
	rm *.o
