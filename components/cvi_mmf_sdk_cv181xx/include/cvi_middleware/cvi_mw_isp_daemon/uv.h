#ifndef __UV_H__
#define __UV_H__

#include <stdio.h>
#include <sys/time.h>

#define USE_UV_BLOCKING_MODE

#include <sys/socket.h>

#define UV_EOF	(-4095)
#define uv_strerror uv_err_name

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

typedef int uv_os_fd_t;
typedef void uv_handle_t;
typedef void uv_stream_t;
typedef void (*uv_connection_cb)(uv_stream_t* server, int status);

typedef struct {
	int sockFd;
} uv_tcp_t;

typedef struct uv_buf_t {
	char* base;
	size_t len;
} uv_buf_t;

typedef struct uv_loop_t {
	uv_connection_cb conn_cb;
	int bServerSocketInit;
	void *ptObject;

} uv_loop_t;

typedef enum {
  UV_RUN_DEFAULT = 0,
  UV_RUN_ONCE,
  UV_RUN_NOWAIT
} uv_run_mode;

typedef void (*uv_close_cb)(uv_handle_t* handle);
typedef void (*uv_alloc_cb)(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
typedef void (*uv_read_cb)(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
typedef void (*uv_walk_cb)(uv_handle_t* handle, void* arg);

//static void CVI_ISPD2_ES_CB_NewConnection(uv_stream_t *pUVServerHandle, int iStatus);
void *CVI_ISPD2_ES_CB_NewConnectionEx(void *args);


#define uv_loop_init(x) ({memset((x), 0, sizeof(*(x)));(x)->conn_cb = CVI_ISPD2_ES_CB_NewConnection;})
#define uv_loop_close(x) (0)
#define uv_read_stop(x)

//int uv_read_stop(uv_stream_t* stream);
int uv_accept(uv_stream_t* server, uv_stream_t* client);
int uv_read_start(uv_stream_t* stream, uv_alloc_cb alloc_cb, uv_read_cb read_cb);
int uv_fileno(const uv_handle_t* handle, uv_os_fd_t* fd);
int uv_tcp_init(uv_loop_t* loop, uv_tcp_t* tcp);
//int uv_loop_init(uv_loop_t* loop);
int uv_ip4_addr(const char* ip, int port, struct sockaddr_in* addr);
int uv_tcp_bind(uv_tcp_t* tcp, const struct sockaddr* addr, unsigned int flags);
int uv_listen(uv_stream_t* stream, int backlog, uv_connection_cb cb);
int uv_run(uv_loop_t *loop, uv_run_mode mode);
void uv_close(uv_handle_t *handle, uv_close_cb close_cb);
void uv_walk(uv_loop_t* loop, uv_walk_cb walk_cb, void* arg);
//int uv_loop_close(uv_loop_t* loop);
void uv_stop(uv_loop_t *loop);
const char* uv_err_name(int err);


#endif
