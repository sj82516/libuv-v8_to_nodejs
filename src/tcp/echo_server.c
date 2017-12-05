/*
 * =====================================================================================
 *
 *       Filename:  echo_server.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/04/2017 16:21:47
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
#include <math.h>

const static int DEFAULT_PORT = 8080;
const static char* LOCAL_HOST = "127.0.0.1";
const static int NBUFS = 1;

static uv_tcp_t tcp_server;

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;


static void close_cb(uv_handle_t * client);
static void server_close_cb(uv_handle_t*);
static void shutdown_cb(uv_shutdown_t*, int);

static void alloc_cb(uv_handle_t*, size_t, uv_buf_t*);
static void read_cb(uv_stream_t*, ssize_t, const uv_buf_t*);
static void write_cb(uv_write_t*, int);

// 每當有新的連線完成，就會觸發(completely established sockets)
static void onconnection(uv_stream_t* server, int status){
    int r = 0;
    printf("connection status");

    uv_shutdown_t* shutdown_req;

    printf("Accepting Connection");

    uv_tcp_t* client = malloc(sizeof(uv_tcp_t));

    r = uv_tcp_init(server->loop, client);

    r = uv_accept(server, (uv_stream_t*) client);
    if(r){
        shutdown_req = malloc(sizeof(uv_shutdown_t));
        r = uv_shutdown(shutdown_req, (uv_stream_t*) client, shutdown_cb);
    }

    r = uv_read_start((uv_stream_t*) client, alloc_cb, read_cb);
}

// 使用者必須自己allocate memory，用在stream連續讀取資料中如uv_read_start
static void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf){
    buf->base = malloc(size);
    buf->len = size;
}

static void shutdown_cb(uv_shutdown_t* req, int status){
    uv_close((uv_handle_t*) req-> handle, close_cb);
    free(req);
}

static void close_cb(uv_handle_t* client){
    free(client);
}

static void read_cb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf){
    int r = 0;
    uv_shutdown_t *shutdown_req;

    if(nread<0){
        if(nread!=UV_EOF){}

        free(buf->base);

        shutdown_req = malloc(sizeof(uv_shutdown_t));
        r = uv_shutdown(shutdown_req, client, shutdown_cb);
    }

    if (nread == 0) {
        free(buf->base);
        return;
    }

    // strncmp 比較兩字串前n個字元是否相同，相同則回傳0
    if(!strncmp("QUIT", buf->base, fmin(nread, 4))){
        free(buf->base);

        uv_close((uv_handle_t*) &tcp_server, NULL);
        exit(0);
    }

    write_req_t *write_req = malloc(sizeof(write_req_t));
    write_req->buf = uv_buf_init(buf->base, nread);
    r = uv_write(&write_req->req, client, &write_req->buf, NBUFS, write_cb);
}

static void write_cb(uv_write_t* req, int status){

    write_req_t *write_req = (write_req_t*) req;
    //free(write_req->buf.base);
    free(write_req);
}

int main(){
    int r = 0;
    uv_loop_t* loop = uv_default_loop();

    r = uv_tcp_init(loop, &tcp_server);

    struct sockaddr_in addr;
    r = uv_ip4_addr(LOCAL_HOST, DEFAULT_PORT, &addr);

    r = uv_tcp_bind(&tcp_server, (struct sockaddr*) &addr, AF_INET);

    r = uv_listen((uv_stream_t*) &tcp_server, SOMAXCONN, onconnection);

    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}
