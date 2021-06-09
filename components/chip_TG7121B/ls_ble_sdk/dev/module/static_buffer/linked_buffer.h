#ifndef LINKED_BUFFER_H_
#define LINKED_BUFFER_H_
#include <stddef.h>
#include "co_list.h"
#include "common.h"

typedef struct{
    struct co_list allocatable;
    uint8_t *buf;
    uint8_t *ref_cnt;
    uint16_t element_size;
    uint16_t buf_length;
}linked_buffer_t;

#define DEF_LINKED_BUF(buf_hdl,type,size) \
    _Static_assert(offsetof(type,hdr)==0,"hdr");\
    static type _##buf_hdl##_array[(size)]; \
    static uint8_t _##buf_hdl##_ref_cnt[(size)];\
    linked_buffer_t buf_hdl

#define INIT_LINKED_BUF(buf_hdl)\
    linked_buf_init(&buf_hdl,sizeof(_##buf_hdl##_array[0]),ARRAY_LEN(_##buf_hdl##_array),(uint8_t *)_##buf_hdl##_array,_##buf_hdl##_ref_cnt)

void linked_buf_init(linked_buffer_t *ptr,uint16_t element_size,uint16_t buf_length,uint8_t *buf,uint8_t *ref_cnt);

struct co_list_hdr * linked_buf_alloc(linked_buffer_t *ptr);

uint8_t linked_buf_release(linked_buffer_t *ptr,struct co_list_hdr *hdr);

bool linked_buf_is_allocatable(linked_buffer_t *ptr);

uint16_t linked_buf_available_size(linked_buffer_t *ptr);

struct co_list_hdr *linked_buf_get_elem_by_idx(linked_buffer_t *ptr,uint16_t idx);

uint16_t linked_buf_get_elem_idx(linked_buffer_t *ptr,struct co_list_hdr *hdr);

uint8_t linked_buf_get_ref_cnt_by_idx(linked_buffer_t *ptr,uint16_t idx);

uint8_t linked_buf_retain(linked_buffer_t *ptr,struct co_list_hdr *hdr);

bool linked_buf_contain_element(linked_buffer_t *ptr,struct co_list_hdr *hdr);

#endif

