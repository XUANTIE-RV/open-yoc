#ifndef FIFO_H_
#define FIFO_H_
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

struct fifo_env{
    void *buf;
    uint16_t rd_idx;
    uint16_t wr_idx;
    uint16_t length;
};

#define DEF_FIFO(name,length,type) \
    type _##name##_fifo_array[(length)];\
    struct fifo_env name;

#define INIT_FIFO(name) fifo_init(&name,_##name##_fifo_array,ARRAY_LEN(_##name##_fifo_array))

void fifo_init(struct fifo_env *ptr,void *buf,uint16_t length);

bool fifo_full(struct fifo_env *ptr);

bool fifo_empty(struct fifo_env *ptr);

void fifo_flush(struct fifo_env *ptr);

uint16_t fifo_element_amount(struct fifo_env *ptr);

bool dword_fifo_put(struct fifo_env *ptr,void *data);

bool dword_fifo_get(struct fifo_env *ptr,void *data);

#endif
