main: main.c
	gcc -o main.o -L /Users/zhengyuanjie/Desktop/Nodejs/learnuv/libuv -I /Users/zhengyuanjie/Desktop/Nodejs/learnuv/libuv/include main.c -luv -framework CoreFoundation -framework CoreServices
