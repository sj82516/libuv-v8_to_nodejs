/*
 * =====================================================================================
 *
 *       Filename:  readasync.c
 *
 *    Description:  簡單的非同步讀檔案，可調整BUF_SIZE觀察讀取的內容
 *
 *        Version:  1.0
 *        Created:  2017/11/29 16時24分45秒
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

static const char* file_path = "/Users/zhengyuanjie/Desktop/Nodejs/learnuv/a.txt";
static int BUF_SIZE = 20;

typedef struct context_struct {
	uv_fs_t* open_req;
	uv_buf_t iov;
} context_t;

/*
uv_fs_t is a subclass of uv_req_t.
struct uv_fs_s {
  UV_REQ_FIELDS
  uv_fs_type fs_type;
  uv_loop_t* loop;
  uv_fs_cb cb;
  ssize_t result;
  void* ptr;
  const char* path;
  uv_stat_t statbuf;  // Stores the result of uv_fs_stat() and uv_fs_fstat()
  UV_FS_PRIVATE_FIELDS
};
*/

void read_cb(uv_fs_t* read_req){
	int r = 0;
	if(read_req->result < 0) printf("read file error");

	context_t* context = read_req->data;
	printf("iov.base: %s \n", context->iov.base);
	printf("what is result: %zu \n", context->open_req->result);

	uv_fs_t close_req;
	r = uv_fs_close(read_req -> loop, &close_req, context->open_req->result, NULL);

	uv_fs_req_cleanup(context->open_req);
	uv_fs_req_cleanup(read_req);
	uv_fs_req_cleanup(&close_req);
}

int main(){
	int r = 0;
	uv_loop_t *loop = uv_default_loop();

	uv_fs_t* open_req = malloc(sizeof(uv_fs_t));
	r = uv_fs_open(loop, open_req, file_path, O_RDONLY, S_IRUSR, NULL);
	if(r < 0) printf("uv_fs_open");

	char *buf = malloc(BUF_SIZE);
	uv_buf_t iov = uv_buf_init(buf, sizeof(char) * BUF_SIZE);
	context_t* context = malloc(sizeof(context_t));
	context->open_req = open_req;
	context->iov = iov;

	uv_fs_t* read_req = malloc(sizeof(uv_fs_t));
	read_req->data = context;
	r = uv_fs_read(loop, read_req, open_req->result, &iov, 1, 0, read_cb);
	if(r<0) printf("uv_fs_read");

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;

}
