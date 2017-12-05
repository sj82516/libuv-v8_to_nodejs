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

static uv_fs_t open_req;
static uv_buf_t iov;

static const char* file_path = "/Users/zhengyuanjie/Desktop/Nodejs/learnuv/a.txt";
static int BUF_SIZE = 5;

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

int main(){
	int r = 0;
	uv_loop_t *loop = uv_default_loop();

    // 讀取檔案
	r = uv_fs_open(loop, &open_req, file_path, O_RDONLY, S_IRUSR, NULL);
	if(r < 0) printf("uv_fs_open error");
    printf("what is result: %zu\n", open_req.result);

    // 創建 buffer，用以存放讀取檔案的內容
	char buf[BUF_SIZE];
	iov = uv_buf_init(buf, sizeof(buf));

	uv_fs_t read_req;
	r = uv_fs_read(loop, &read_req, open_req.result, &iov, 100, 0, NULL);
    if(r < 0) printf("uv_fs_read error");
	if(read_req.result < 0) printf("read file error");
    // iov.base是讀取的內容，iov.len是指讀取的內容長度
	printf("iov.base: %s %zu", iov.base, iov.len);

    // 關閉檔案
	uv_fs_t close_req;
	r = uv_fs_close(loop, &close_req, open_req.result, NULL);
    if(r < 0) printf("uv_fs_read error");

    // 清除
	uv_fs_req_cleanup(&open_req);
	uv_fs_req_cleanup(&read_req);
	uv_fs_req_cleanup(&close_req);

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}
