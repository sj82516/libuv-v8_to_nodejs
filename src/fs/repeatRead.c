/*
 * =====================================================================================
 *
 *       Filename:  readallasync.c
 *
 *    Description:  使用libuv分次讀取檔案
 *
 *        Version:  1.0
 *        Created:  2017/11/29 17時02分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YuanChieh (), sj82516@gmail.com
 *   Organization:  Taiwan
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <uv.h>

#define BUF_SIZE 5
static const char* read_file_path = "/Users/zhengyuanjie/Desktop/Nodejs/learnuv/a.txt";

void open_cb(uv_fs_t *);
void read_cb(uv_fs_t *);
void close_cb(uv_fs_t *);

typedef struct context_struct {
	uv_fs_t *read_open_req;
	uv_fs_t *read_req;
	uv_buf_t iov;
    // 紀錄讀寫次數
	int times;
} context_t;

void open_cb(uv_fs_t* open_req){
	int r = 0;
	if(open_req -> result < 0 ) printf("open_req");

	context_t* context = open_req -> data;

	size_t buf_len = sizeof(char) * BUF_SIZE;
	char *buf = malloc(buf_len);
	context->iov = uv_buf_init(buf, buf_len);
	context->times = 0;

	uv_fs_t *read_req = malloc(sizeof(uv_fs_t));
	context->read_req = read_req;
	read_req->data = context;

	r = uv_fs_read(uv_default_loop(), read_req, open_req->result, &context->iov, 1, -1, read_cb);
}

void read_cb(uv_fs_t* read_req){
	int r = 0;

	context_t* context = read_req->data;
	uv_fs_t* close_req = malloc(sizeof(uv_fs_t));
	close_req->data = context;

	if(read_req->result < 0){
		printf("read_req");
		r = uv_fs_close(uv_default_loop(), close_req, context->read_open_req->result, close_cb);
		return;
	}else if(read_req->result == 0){
		printf("read finish\n");
		r = uv_fs_close(uv_default_loop(), close_req, context->read_open_req->result, close_cb);
		return;
	}

	printf("read result:%zu, open result:%zu \n", read_req->result, context->read_open_req->result);
	printf("time: %i :", context->times);
	printf("%s\n", context->iov.base);

	size_t buf_len = sizeof(char) * BUF_SIZE;
	char *buf = malloc(buf_len);
	context->iov = uv_buf_init(buf, buf_len);
	context->times++;
	read_req->data = context;

	r = uv_fs_read(uv_default_loop(), read_req, context->read_open_req->result, &context->iov, 1, -1, read_cb);
	if(r<0) printf("read_req");
}

void close_cb(uv_fs_t* close_req){
	if(close_req->result < 0 ) printf("close error \n");

	context_t* context = close_req->data;

	uv_fs_req_cleanup(context->read_open_req);
	uv_fs_req_cleanup(context->read_req);
	uv_fs_req_cleanup(close_req);
}

void init(){
	int r = 0;
	uv_fs_t* open_req = malloc(sizeof(uv_fs_t));

	context_t *context = malloc(sizeof(context_t));
	context->read_open_req = open_req;
	open_req->data = context;

	r = uv_fs_open(uv_default_loop(), open_req, read_file_path, O_RDONLY, S_IRUSR, open_cb);
	if(r < 0) printf("uv_fs_open");
}

int main(){
	uv_loop_t *loop = uv_default_loop();

	init();

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}
