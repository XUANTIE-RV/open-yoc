#define MODULE_NAME		"RB"

#include "silan_ringbuffer.h"
#include "silan_printf.h"
#include "silan_errno.h"
#include "string.h"

RINGBUFFER_HANDLE __ringbuffer_handle[RINGBUFFER_HANDLE_NUM];

uint32_t __ringbuffer_index = 0;

int32_t silan_ringbuffer_fullness(RINGBUFFER_HANDLE *rb_handle)
{
    int32_t val;

	val = rb_handle->wr - rb_handle->rd;
	if (val < 0)
		val += rb_handle->size;

    if (val < 0 || val >= rb_handle->size)
        SL_LOG("fullness %d(w:%p, r:%p)\n", val, rb_handle->wr, rb_handle->rd);

    return val;
}

int32_t silan_ringbuffer_space(RINGBUFFER_HANDLE *rb_handle)
{
	return (rb_handle->size - silan_ringbuffer_fullness(rb_handle));
}

#if 0
int8_t *silan_ringbuffer_get_rd(RINGBUFFER_HANDLE *rb_handle)
{
	return rb_handle->rd;
}

int8_t *silan_ringbuffer_get_wr(RINGBUFFER_HANDLE *rb_handle)
{
	return rb_handle->wr;
}
#endif

static inline int32_t silan_ringbuffer_to_end(RINGBUFFER_HANDLE *rb_handle)
{
	return (rb_handle->limit - rb_handle->wr);
}

static inline int32_t silan_ringbuffer_distance(RINGBUFFER_HANDLE *rb_handle)
{
	return (rb_handle->limit - rb_handle->rd);
}

int32_t silan_ringbuffer_write(RINGBUFFER_HANDLE *rb_handle, int8_t *data, uint32_t size)
{
	int32_t space, to_end, remain;
	int8_t *wr = rb_handle->wr;
	int8_t *src = data;

	space = silan_ringbuffer_space(rb_handle);
	to_end = silan_ringbuffer_to_end(rb_handle);
	if(size > space) {
		SL_LOG("Write Not Enough Space");
		return -ENOSPC;
	}
	
	if(size > to_end) {
		memcpy(wr, src, to_end);
		wr = rb_handle->start;
		src += to_end;
		remain = size - to_end;
		memcpy(wr, src, remain);
		wr = rb_handle->start + remain;
	}
	else {
		memcpy(wr, src, size);
		wr += size;
	}
	
	if(wr >= rb_handle->limit)
		rb_handle->wr = wr - rb_handle->size;
	else
		rb_handle->wr = wr;

	return size;
}

int32_t silan_ringbuffer_read(RINGBUFFER_HANDLE *rb_handle, int8_t *data, uint32_t size)
{
	int32_t fullness, distance, remain;
	int8_t *rd = rb_handle->rd;
	int8_t *dst = data;

	fullness = silan_ringbuffer_fullness(rb_handle);
	distance = silan_ringbuffer_distance(rb_handle);
	if(size > fullness) {
		SL_LOG("Read Not Enough Space");
		return -ENOSPC;
	}

	if(size > distance) {
		memcpy(dst, rd, distance);
		dst += distance;
		rd = rb_handle->start;
		remain = size - distance;
		memcpy(dst, rd, remain);
		rd += remain;
	}
	else {
		memcpy(dst, rd, size);
		rd += size;
	}

	if(rd >= rb_handle->limit)
		rb_handle->rd = rd - rb_handle->size;
	else
		rb_handle->rd = rd;
	
	return size;
}

RINGBUFFER_HANDLE *silan_ringbuffer_alloc(int8_t *buf, uint32_t size)
{
	RINGBUFFER_HANDLE *rb_handle;
	if(__ringbuffer_index > (RINGBUFFER_HANDLE_NUM-1)) {
		SL_LOG("RB Alloc Fail");
		return NULL;
	}

	rb_handle = &__ringbuffer_handle[__ringbuffer_index];
	rb_handle->start = buf;
	rb_handle->size = size;
	rb_handle->limit = buf + size;
	rb_handle->rd = buf;
	rb_handle->wr = buf;

	__ringbuffer_index++;
	return rb_handle;
}

void silan_ringbuffer_debug(RINGBUFFER_HANDLE *rb_handle, int8_t flag)
{
	int32_t size;
	if(!flag)
		size = silan_ringbuffer_fullness(rb_handle);
	else
		size = silan_ringbuffer_space(rb_handle);
	
	if(size < 200) {
		if(!flag)
			SL_LOG("fullness: %d", size);
		else
			SL_LOG("space: %d", size);
	}
}

void silan_ringbuffer_init(void)
{
	memset(__ringbuffer_handle, 0, sizeof(__ringbuffer_handle));
}

