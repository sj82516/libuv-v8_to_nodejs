CC = gcc
CFLAGS = -std=c99

LDFLAGS = -L /Users/zhengyuanjie/Desktop/Nodejs/learnuv/libuv /Users/zhengyuanjie/Desktop/Nodejs/learnuv/http-parser/http_parser.o
LDLIBS =  -luv -framework CoreFoundation -framework CoreServices
LDINCLUDES = -I/Users/zhengyuanjie/Desktop/Nodejs/learnuv/libuv/include

echo_server: echo_server.c
	$(CC) echo_server.c $(LDFLAGS) $(LDINCLUDES) $(LDLIBS) -o $@


http_server: http_server.c
	$(CC) http_server.c $(LDFLAGS) $(LDINCLUDES) $(LDLIBS) -o $@

clean:
	rm -f *.o echo_server
