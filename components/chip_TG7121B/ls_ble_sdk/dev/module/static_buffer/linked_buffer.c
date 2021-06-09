#include <stdarg.h>
#include <string.h>
#include "linked_buffer.h"
#include "ls_dbg.h"
#define LINKED_BUF_ASSERT(...) LS_ASSERT(__VA_ARGS__)

void linked_buf_init(linked_buffer_t *ptr,uint16_t element_size,uint16_t buf_length,uint8_t *buf,uint8_t *ref_cnt)
{
    memset(ref_cnt,0,buf_length);
    co_list_init(&ptr->allocatable);
    ptr->buf = buf;
    ptr->ref_cnt = ref_cnt;
    ptr->element_size = element_size;
    ptr->buf_length = buf_length;
    uint16_t i;
    for(i=0;i<buf_length;++i)
    {
        co_list_push_back(&ptr->allocatable,(struct co_list_hdr *)&buf[i*element_size]);
    }
}

struct co_list_hdr * linked_buf_alloc(linked_buffer_t *ptr)
{
    struct co_list_hdr *hdr = co_list_pop_front(&ptr->allocatable);
    if(hdr)
    {
        uint16_t idx = linked_buf_get_elem_idx(ptr,hdr);
        ptr->ref_cnt[idx] += 1;
    }
    return hdr;
}

static bool linked_buf_hdl_sanity_check(linked_buffer_t *buf_hdl,uint8_t *ptr)
{
    if((ptr-buf_hdl->buf)%buf_hdl->element_size)
    {
        return false;
    }
    if(ptr<buf_hdl->buf || ptr>= &buf_hdl->buf[buf_hdl->buf_length*buf_hdl->element_size])
    {
        return false;
    }
    return true;

}

uint8_t linked_buf_release(linked_buffer_t *ptr,struct co_list_hdr * hdr)
{
    LINKED_BUF_ASSERT(linked_buf_hdl_sanity_check(ptr,(uint8_t *)hdr));
    uint16_t idx = linked_buf_get_elem_idx(ptr,hdr);
    LINKED_BUF_ASSERT(ptr->ref_cnt[idx]);
    co_list_push_back(&ptr->allocatable, hdr);
    return --ptr->ref_cnt[idx];
}

uint16_t linked_buf_get_elem_idx(linked_buffer_t *ptr,struct co_list_hdr *hdr)
{
    uint8_t *elem = (uint8_t *)hdr;
    LINKED_BUF_ASSERT((elem-(uint8_t *)ptr->buf)%ptr->element_size==0);
    return (elem-(uint8_t *)ptr->buf)/ptr->element_size;
}

struct co_list_hdr *linked_buf_get_elem_by_idx(linked_buffer_t *ptr,uint16_t idx)
{
    return (struct co_list_hdr *)&ptr->buf[ptr->element_size*idx];
}

uint16_t linked_buf_available_size(linked_buffer_t *ptr)
{
    return co_list_size(&ptr->allocatable);
}

bool linked_buf_is_allocatable(linked_buffer_t *ptr)
{
    return co_list_pick(&ptr->allocatable)? true : false;
}

uint8_t linked_buf_get_ref_cnt_by_idx(linked_buffer_t *ptr,uint16_t idx)
{
    return ptr->ref_cnt[idx];
}

uint8_t linked_buf_retain(linked_buffer_t *ptr,struct co_list_hdr *hdr)
{
    LINKED_BUF_ASSERT(linked_buf_hdl_sanity_check(ptr,(uint8_t *)hdr));
    uint16_t idx = linked_buf_get_elem_idx(ptr,hdr);
    return ++ptr->ref_cnt[idx];
}

bool linked_buf_contain_element(linked_buffer_t *ptr,struct co_list_hdr *hdr)
{
    bool contain = false;
    if(linked_buf_hdl_sanity_check(ptr,(uint8_t *)hdr))
    {
        uint16_t idx = linked_buf_get_elem_idx(ptr,hdr);
        contain = linked_buf_get_ref_cnt_by_idx(ptr,idx) ? true : false;
    }
    return contain;
}

uint16_t linked_buf_element_size(linked_buffer_t *ptr)
{
    return ptr->element_size;
}
