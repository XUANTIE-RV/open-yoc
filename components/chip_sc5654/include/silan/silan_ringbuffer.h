#ifndef __SILAN_RINGBUFFER_H__
#define __SILAN_RINGBUFFER_H__

#include "silan_types.h"

#define RINGBUFFER_HANDLE_NUM		8

typedef struct RINGBUFFER_handle
{
	int8_t		*start;
	uint32_t	size;
	int8_t		*limit;
	int8_t		*rd;
	int8_t		*wr;
}
RINGBUFFER_HANDLE, *PRINGBUFFER_HANDLE;

void silan_ringbuffer_init(void);
RINGBUFFER_HANDLE *silan_ringbuffer_alloc(int8_t *buf, uint32_t size);
int32_t silan_ringbuffer_fullness(RINGBUFFER_HANDLE *rb_handle);
int32_t silan_ringbuffer_space(RINGBUFFER_HANDLE *rb_handle);
int32_t silan_ringbuffer_write(RINGBUFFER_HANDLE *rb_handle, int8_t *data, uint32_t size);
int32_t silan_ringbuffer_read(RINGBUFFER_HANDLE *rb_handle, int8_t *data, uint32_t size);

#endif
