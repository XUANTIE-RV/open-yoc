/*
 * buffer.h
 */

#ifndef __SILAN_BUFFER_H__
#define __SILAN_BUFFER_H__

typedef struct buf_s {
	volatile char *base;
	volatile char *lmt;
	volatile char *rd;
	volatile char *wt;
	volatile int  bytes;
}buf_t;

int silan_buf_init(buf_t *buf, char *base, int bytes);

#define silan_buf_try_read(buf, read_p, bytes)      silan_buf_pre_read(buf, read_p, 0, bytes)
int silan_buf_pre_read(buf_t *buf, char **read_p, int offset, int bytes);
int silan_buf_read(buf_t *buf, int bytes);

#define silan_buf_try_write(buf, write_p, bytes)    silan_buf_pre_write(buf, write_p, 0, bytes)
int silan_buf_pre_write(buf_t *buf, char **write_p, int offset, int bytes);
int silan_buf_write(buf_t *buf, int bytes);

int silan_buf_get_fullness(buf_t *buf);

#endif
