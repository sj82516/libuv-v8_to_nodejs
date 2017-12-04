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
static const char* write_file_path = "/Users/zhengyuanjie/Desktop/Nodejs/learnuv/c.txt";

void open_read_cb(uv_fs_t *);
void open_write_cb(uv_fs_t *);
void read_cb(uv_fs_t *);
void write_cb(uv_fs_t *);
void close_cb(uv_fs_t *);

typedef struct context_struct {
	uv_fs_t *open_read_req;
	uv_fs_t *open_write_req;
	uv_fs_t *read_req;
	uv_fs_t *write_req;
	uv_buf_t iov;
    // 紀錄讀寫次數
	int times;
} context_t;

// 先開啟要讀的檔案，並啟動開啟寫檔案
void open_read_cb(uv_fs_t* open_req){
	int r = 0;
	if(open_req -> result < 0 ) printf("open_read_req");

	context_t* context = open_req -> data;

	uv_fs_t *open_write_req = malloc(sizeof(uv_fs_t));
	context->open_write_req = open_write_req;
	open_write_req->data = context;

	r = uv_fs_open(uv_default_loop(), open_write_req, write_file_path, O_WRONLY | O_CREAT, 0644, open_write_cb);;
}

// 接著開啟要寫的檔案，並開始讀取
void open_write_cb(uv_fs_t* open_req){
	int r = 0;
	if(open_req -> result < 0 ) printf("open_write_req");

	context_t* context = open_req -> data;

	size_t buf_len = sizeof(char) * BUF_SIZE;
	char *buf = malloc(buf_len);
	context->iov = uv_buf_init(buf, buf_len);
	context->times = 0;

	uv_fs_t *read_req = malloc(sizeof(uv_fs_t));
	uv_fs_t *write_req = malloc(sizeof(uv_fs_t));
	context->read_req = read_req;
	context->write_req = write_req;
	read_req->data = context;
	write_req->data = context;

	r = uv_fs_read(uv_default_loop(), read_req, context->open_read_req->result, &context->iov, 1, 0, read_cb);
}

// 完成讀取後，接內容寫入檔案
void read_cb(uv_fs_t* read_req){
	int r = 0;

	//uv_fs_t* close_req = mollac(sizeof(uv_fs_t));
	if(read_req->result < 0){
		printf("read_req");
		return;
	}else if(read_req->result == 0){
		printf("read finish\n");
		return;
	}

	context_t* context = read_req->data;

	printf("read result:%zu, open result:%zu \n", read_req->result, context->open_read_req->result);
	printf("time: %i :", context->times);
	printf("%s\n", context->iov.base);

	r = uv_fs_write(uv_default_loop(), context->write_req, context->open_write_req->result, &context->iov, 1, -1, write_cb);
	if(r<0) printf("read_req");
}

// 寫完下一輪繼續讀
void write_cb(uv_fs_t* write_req){
	int r = 0;

	if(write_req->result < 0){
		printf("write_req");
		return;
	}

	context_t* context = write_req->data;
	size_t buf_len = sizeof(char) * BUF_SIZE;
	char *buf = malloc(buf_len);
	context->iov = uv_buf_init(buf, buf_len);
	context->times++;
	write_req->data = context;

	r = uv_fs_read(uv_default_loop(), context->read_req, context->open_read_req->result, &context->iov, 1, -1, read_cb);
	if(r<0) printf("read_req");
}

void init(){
	int r = 0;
	uv_fs_t* open_req = malloc(sizeof(uv_fs_t));

	context_t *context = malloc(sizeof(context_t));
	context->open_read_req = open_req;
	open_req->data = context;

	r = uv_fs_open(uv_default_loop(), open_req, read_file_path, O_RDONLY, S_IRUSR, open_read_cb);
	if(r < 0) printf("uv_fs_open");
}

int main(){
	uv_loop_t *loop = uv_default_loop();

	init();

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}
