#define LOG_TAG  "TINYFS"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "compile_flag.h"
#include "tinyfs_config.h"
#include "tinyfs_write_cache.h"
#include "tinyfs.h"
#include "tinyfs_nvm.h"
#include "linked_buffer.h"
#include "log.h"
#include "common.h"
#include "ls_dbg.h"
#include "crc16.h"
#include "co_math.h"

#define TINYFS_ASSERT(...) LS_ASSERT(__VA_ARGS__)

#define INVALID_NODE_IDX (0xffff)
#define INVALID_NODE_OFFSET (0)
#define INVALID_SECTION (0xffff)

#define TINYFS_CRC_INIT_VAL (0x5555)
#define TINYFS_CRC_LENGTH 2

#define RECORD_DATA_COPY_BUF_SIZE 64
#define BUFFERING_SECTION_NUM 1
#if (BUFFERING_SECTION_NUM>=TINYFS_SECTION_NUM)
#error BUFFERING_SECTION_NUM must be less than TINYFS_SECTION_NUM
#endif
#define crc16calc (crc16ccitt)
#define NODE_METADATA_LENGTH_MAX sizeof(record_add_t)
enum node_enum
{
    RECORD_ADD,
    DIR_ADD,
    RECORD_REMOVE,
    DIR_REMOVE,
    NODE_TYPE_MAX,
};

enum node_write_type
{
    PROGRAM_START,
    PROGRAM_CONTINUE,
};

typedef struct __attribute__((packed))
{
    enum node_enum type;
    uint16_t name;
    uint16_t parent_id;
    uint16_t data_length;
    uint16_t crc16;
} record_add_t;
#define RECORD_ADD_LENGTH (sizeof(record_add_t))

typedef struct __attribute__((packed))
{
    enum node_enum type;
    uint16_t name;
    uint16_t parent_id;
    uint16_t id;
    uint16_t crc16;
} dir_add_t;
#define DIR_ADD_LENGTH (sizeof(dir_add_t))

typedef struct __attribute__((packed))
{
    enum node_enum type;
    uint16_t name;
    uint16_t parent_id;
    uint16_t crc16;
} record_remove_t;
#define RECORD_REMOVE_LENGTH (sizeof(record_remove_t))

typedef struct __attribute__((packed))
{
    enum node_enum type;
    uint16_t id;
    uint16_t crc16;
} dir_remove_t;
#define DIR_REMOVE_LENGTH (sizeof(dir_remove_t))

typedef struct __attribute__((packed))
{
    uint16_t loop_count;
    uint16_t node_offset;
    uint16_t crc16;
} section_head_t;

typedef struct
{
    uint16_t section;
    uint16_t offset;
}storage_addr_t;

typedef uint16_t node_idx_t;

typedef struct{
    node_idx_t head;
    node_idx_t tail;
}node_list_t;

typedef struct{
    node_list_t list;
    uint16_t size;
}tinyfs_list_t;

typedef struct
{
    struct co_list_hdr hdr;
    storage_addr_t addr;
    node_idx_t child;
    node_idx_t adjacent;
    node_idx_t parent_dir;
    node_idx_t next_sibling;
    uint16_t name;
    union
    {
        uint16_t id;
        uint16_t data_length;
    }u;
}tinyfs_node_t;

typedef struct
{
    uint16_t loop_count;
    uint16_t node_offset;
}section_head_data_t;

static struct
{
    uint32_t base;
    uint32_t free_bytes;
    uint16_t tail_available_offset;
    uint16_t head_section;
    uint16_t tail_section;
    uint16_t tail_count;
    uint16_t dir_id_max;
} tinyfs_env;

static tinyfs_list_t tinyfs_list[TINYFS_SECTION_NUM];
DEF_LINKED_BUF(tinyfs_node,tinyfs_node_t,TINYFS_NODE_MAX + 1);
static const uint8_t node_metadata_length[NODE_TYPE_MAX] = {
    [RECORD_ADD] = RECORD_ADD_LENGTH,
    [DIR_ADD] = DIR_ADD_LENGTH,
    [RECORD_REMOVE] = RECORD_REMOVE_LENGTH,
    [DIR_REMOVE] = DIR_REMOVE_LENGTH,
};

static uint16_t data_write_through(void);

void *root_dir_ptr;
#define root_dir ((tinyfs_node_t *)root_dir_ptr)
#define GET_FLASH_ADDR(section, offset) (tinyfs_env.base + (section)*TINYFS_SECTION_SIZE + (offset))


#if (RTOS_USED)

static void tinyfs_mutex_create(){}
static void tinyfs_mutex_lock(){}
static void tinyfs_mutex_unlock(){}
#else
static void tinyfs_mutex_create(){}
static void tinyfs_mutex_lock(){}
static void tinyfs_mutex_unlock(){}

#endif

static node_idx_t node_idx(tinyfs_node_t *ptr)
{
    if(ptr)
    {
        return linked_buf_get_elem_idx(&tinyfs_node, &ptr->hdr);
    }else
    {
        return INVALID_NODE_IDX;
    }
}

static tinyfs_node_t *node_ptr(node_idx_t idx)
{
    if(idx == INVALID_NODE_IDX)
    {
        return NULL;
    }else
    {
        return (tinyfs_node_t *)linked_buf_get_elem_by_idx(&tinyfs_node,idx);
    }
}

static uint16_t get_next_section(uint16_t current)
{
    return current + 1 == TINYFS_SECTION_NUM ? 0 : current + 1;
}

static uint16_t get_prev_section(uint16_t current)
{
    return current ? current - 1 : TINYFS_SECTION_NUM - 1;
}

static void tinyfs_section_erase(uint16_t section_num)
{
    uint8_t i;
    uint16_t offset = 0;
    for (i = 0; i < TINYFS_SECTION_SIZE / 0x1000; ++i)
    {
        nvm_sector_ease(GET_FLASH_ADDR(section_num, offset));
        offset += 0x1000;
    }
}

static bool section_head_read(uint16_t section_num, section_head_t *section_head)
{
    tinyfs_nvm_read_with_cache(GET_FLASH_ADDR(section_num, 0), sizeof(section_head_t),(uint8_t *) section_head);
    return crc16calc(TINYFS_CRC_INIT_VAL, section_head, offsetof(section_head_t, crc16)) == section_head->crc16;
}

static void section_head_read_all(section_head_data_t *section_head_data,uint8_t *valid_mask)
{
    uint16_t i;
    for(i=0;i<TINYFS_SECTION_NUM;++i)
    {
        section_head_t head;
        if(section_head_read(i,&head))
        {
            valid_mask[i/8] |= 1<< i%8;
            section_head_data[i].loop_count = head.loop_count;
            section_head_data[i].node_offset = head.node_offset;
        }
    }
}

static uint16_t get_next_valid_section(uint8_t *valid_mask,uint16_t curr)
{
    do
    {
        curr = get_next_section(curr);
    }while((valid_mask[curr/8]&1<<curr%8)==0);
    return curr;
}

static uint16_t get_first_valid_section(uint8_t *valid_mask)
{
    uint16_t i;
    for(i=0;i<CEILING(TINYFS_SECTION_NUM,8);++i)
    {
        uint32_t trailing_zeros = co_ctz(valid_mask[i]);
        if(trailing_zeros!=32)
        {
            return trailing_zeros + i*8;
        }
    }
    TINYFS_ASSERT(0);
    return 0;
}

static void find_head_tail_section(section_head_data_t *section_head_data,uint8_t *valid_mask,
    uint16_t *head, uint16_t *tail,uint16_t *tail_count)
{
    uint16_t prev = get_first_valid_section(valid_mask);
    uint16_t curr = get_next_valid_section(valid_mask,prev);
    while(section_head_data[prev].loop_count + 1 == section_head_data[curr].loop_count)
    {
        prev = curr;
        curr = get_next_valid_section(valid_mask,curr);
    }
    int16_t diff = section_head_data[prev].loop_count - section_head_data[curr].loop_count;
    if(diff>0)
    {
        *head = curr;
        *tail = prev;
    }else
    {
        *head = prev;
        *tail = curr;
    }
    *tail_count = section_head_data[*tail].loop_count;
}

static void nvm_read_node(uint16_t section,uint16_t offset,uint16_t length,uint8_t *buf)
{
    if(offset+length<=TINYFS_SECTION_SIZE)
    {
        tinyfs_nvm_read_with_cache(GET_FLASH_ADDR(section,offset), length, buf); 
    }else
    {
        uint16_t length0 = TINYFS_SECTION_SIZE-offset;
        uint16_t length1 = length-length0;
        tinyfs_nvm_read_with_cache(GET_FLASH_ADDR(section,offset),length0,buf);
        tinyfs_nvm_read_with_cache(GET_FLASH_ADDR(get_next_section(section),sizeof(section_head_t)),
            length1,&buf[length0]);
    }
}

static bool node_metadata_read(uint16_t section, uint16_t offset, uint8_t *buf)
{
    nvm_read_node(section,offset,NODE_METADATA_LENGTH_MAX,buf);
    bool crc_match;
    uint8_t type = buf[0];
    if(type<NODE_TYPE_MAX)
    {
        uint16_t crc;
        uint8_t payload_length = node_metadata_length[type]-TINYFS_CRC_LENGTH;
        memcpy(&crc,&buf[payload_length],TINYFS_CRC_LENGTH);
        crc_match = crc16calc(TINYFS_CRC_INIT_VAL,buf,payload_length) == crc;
    }else
    {
        crc_match = false;
    }
    return crc_match;
}

static tinyfs_node_t *node_alloc()
{
    return (tinyfs_node_t *)linked_buf_alloc(&tinyfs_node);
}

static void node_free(tinyfs_node_t *ptr)
{
    linked_buf_release(&tinyfs_node,&ptr->hdr);
}

static uint16_t node_total_length(const tinyfs_node_t *const ptr)
{
    return ptr->child?sizeof(dir_add_t):sizeof(record_add_t) + ptr->u.data_length + TINYFS_CRC_LENGTH;
}

static void tinyfs_list_push(tinyfs_list_t *env,tinyfs_node_t *ptr)
{
    node_idx_t idx = node_idx(ptr);
    if(env->list.head==INVALID_NODE_IDX)
    {
        env->list.head = idx;
    }
    ptr->adjacent = INVALID_NODE_IDX;
    if(env->list.tail != INVALID_NODE_IDX)
    {
        node_ptr(env->list.tail)->adjacent = idx;
    }
    env->list.tail = idx;
    env->size += node_total_length(ptr);
}

static bool tinyfs_list_del(tinyfs_list_t *env,tinyfs_node_t *ptr)
{
    if(node_ptr(env->list.head)==ptr)
    {
        if(env->list.head == env->list.tail)
        {
            env->list.tail = INVALID_NODE_IDX;
            env->list.head = INVALID_NODE_IDX;
        }else
        {
            env->list.head = ptr->adjacent;
        }
        env->size -= node_total_length(ptr);
        return true;
    }
    node_idx_t w = env->list.head,p;
    while(w!=INVALID_NODE_IDX)
    {
        p = w;
        w = node_ptr(w)->adjacent;
        tinyfs_node_t *w_ptr = node_ptr(w);
        if(w_ptr==ptr)
        {
            node_ptr(p)->adjacent = w_ptr->adjacent;
            if(w==env->list.tail)
            {
                env->list.tail = p;
            }
            env->size -= node_total_length(ptr);
            return true;
        }
    }
    return false;
}

static tinyfs_node_t *node_list_search(node_list_t *list,bool (*compare)(void *,va_list *),va_list *param_list)
{
    node_idx_t idx;
    tinyfs_node_t *ptr;
    for(idx = list->head;idx!=INVALID_NODE_IDX;idx=ptr->adjacent)
    {
        ptr = node_ptr(idx);
        va_list param = *param_list;
        if(compare(ptr,&param))
        {
            return ptr;
        }
    }
    return NULL;
}

static bool record_compare(void *item,va_list *param)
{
    tinyfs_node_t *ptr = item;
    uint16_t parent_id = va_arg(*param,uint32_t);
    uint16_t name = va_arg(*param,uint32_t);
    return ptr->child == 0 && ptr->parent_dir == parent_id && ptr->name == name;
}

tinyfs_node_t *node_search(bool (*compare)(void *,va_list *),...)
{
    uint16_t i;
    for(i=0;i<TINYFS_SECTION_NUM;++i)
    {
        va_list param_list;
        va_start(param_list,compare);
        tinyfs_node_t *ptr = node_list_search(&tinyfs_list[i].list,compare,&param_list);
        va_end(param_list);
        if(ptr)
        {
            return ptr;
        }
    }
    return NULL;
}

static uint16_t record_add_parse(uint16_t section,uint16_t offset,void *buf)
{
    record_add_t *node = buf;
    tinyfs_node_t *ptr = node_search(record_compare,node->parent_id,node->name);
    if(ptr)
    {
        bool del = tinyfs_list_del(&tinyfs_list[ptr->addr.section],ptr);
        TINYFS_ASSERT(del);
    }else
    {
        ptr = node_alloc();
        TINYFS_ASSERT(ptr);
        ptr->name = node->name;
        ptr->parent_dir = node->parent_id;
        ptr->child = 0;
        ptr->next_sibling = INVALID_NODE_IDX;
    }
    ptr->addr.section = section;
    ptr->addr.offset = offset;
    ptr->u.data_length = node->data_length;
    tinyfs_list_push(&tinyfs_list[section],ptr);
    return sizeof(record_add_t) + node->data_length + TINYFS_CRC_LENGTH;
}

static bool dir_compare(void *item,va_list *param)
{
    tinyfs_node_t *ptr = item;
    uint16_t id = va_arg(*param,uint32_t);
    return ptr->child && ptr->u.id == id;
}

static uint16_t dir_add_parse(uint16_t section,uint16_t offset,void *buf)
{
    dir_add_t *node = buf;
    tinyfs_node_t *ptr = node_search(dir_compare,node->id);
    if(ptr)
    {
        bool del = tinyfs_list_del(&tinyfs_list[ptr->addr.section],ptr);
        TINYFS_ASSERT(del);
    }else
    {
        ptr = node_alloc();
        TINYFS_ASSERT(ptr);
        ptr->u.id = node->id;
        ptr->child = INVALID_NODE_IDX;
        ptr->next_sibling = INVALID_NODE_IDX;
    }
    ptr->addr.section = section;
    ptr->addr.offset = offset;
    ptr->name = node->name;
    ptr->parent_dir = node->parent_id;
    if(tinyfs_env.dir_id_max<node->id)
    {
        tinyfs_env.dir_id_max = node->id;
    }
    tinyfs_list_push(&tinyfs_list[section],ptr);
    return sizeof(dir_add_t);
}

static uint16_t record_remove_parse(uint16_t section,uint16_t offset,void *buf)
{
    record_remove_t *node = buf;
    tinyfs_node_t *ptr = node_search(record_compare,node->parent_id,node->name);
    if(ptr)
    {
        tinyfs_list_del(&tinyfs_list[ptr->addr.section],ptr);
        node_free(ptr);
    }else
    {
        LOG_W("record remove parse warning:flash addr:%x",offset);
    }
    return sizeof(record_remove_t);
}

static uint16_t dir_remove_parse(uint16_t section,uint16_t offset,void *buf)
{
    dir_remove_t *node = buf;
    tinyfs_node_t *ptr = node_search(dir_compare,node->id);
    if(ptr)
    {
        tinyfs_list_del(&tinyfs_list[ptr->addr.section],ptr);
        node_free(ptr);
    }else
    {
        LOG_W("dir remove parse warning:flash addr:%x",offset);
    }
    return sizeof(dir_remove_t);
}

static void section_offset_calc(uint16_t *section,uint16_t *offset,uint16_t length)
{
    if(*offset+length>=TINYFS_SECTION_SIZE)
    {
        *section = get_next_section(*section);
        *offset = *offset + length - TINYFS_SECTION_SIZE + sizeof(section_head_t);
    }else
    {
        *offset = *offset + length;
    }
}

static bool node_data_read(uint16_t section,uint16_t offset,uint16_t length,uint8_t *buf)
{
    section_offset_calc(&section,&offset,RECORD_ADD_LENGTH);
    nvm_read_node(section,offset,length,buf);
    section_offset_calc(&section,&offset,length);
    uint16_t crc;
    nvm_read_node(section,offset,TINYFS_CRC_LENGTH,(uint8_t *)&crc);
    return crc == crc16calc(TINYFS_CRC_INIT_VAL,buf,length);
}

static uint16_t (*const node_metadata_parse[NODE_TYPE_MAX])(uint16_t,uint16_t,void *) = {
    [RECORD_ADD] = record_add_parse,
    [DIR_ADD] = dir_add_parse,
    [RECORD_REMOVE] = record_remove_parse,
    [DIR_REMOVE] = dir_remove_parse,
};

static bool nvm_page_empty(uint16_t section,uint16_t offset)
{
    TINYFS_ASSERT(offset%TINYFS_WRITE_CACHE_SIZE==0);
    uint32_t page_buf[TINYFS_WRITE_CACHE_SIZE/sizeof(uint32_t)];
    tinyfs_nvm_read_with_cache(GET_FLASH_ADDR(section, offset),TINYFS_WRITE_CACHE_SIZE,(uint8_t *)page_buf);
    uint8_t i;
    for(i=0;i<TINYFS_WRITE_CACHE_SIZE/sizeof(uint32_t);++i)
    {
        if(page_buf[i]!=~0)
        {
            return false;
        }
    }
    return true;
}

static void nodes_info_load(section_head_data_t *section_head_data,uint8_t *valid_mask)
{
    uint16_t i = tinyfs_env.tail_section;
    uint16_t offset = 0;
    do
    {
        i = get_next_valid_section(valid_mask,i);
        offset = section_head_data[i].node_offset;
        while (offset > 5 && offset < TINYFS_SECTION_SIZE)
        {
            uint8_t buf[NODE_METADATA_LENGTH_MAX];
            if(node_metadata_read(i,offset,buf))
            {
                uint8_t type = buf[0];
                uint16_t node_total_length = node_metadata_parse[type](i,offset,buf);
                offset += node_total_length;
            }else
            {
                if(offset%TINYFS_WRITE_CACHE_SIZE==0 && nvm_page_empty(i,offset))
                {
                    break;
                }
                offset =  (offset+TINYFS_WRITE_CACHE_SIZE) &~(TINYFS_WRITE_CACHE_SIZE-1);
            }
        }
    }while(i!=tinyfs_env.tail_section);
    tinyfs_env.tail_available_offset = offset;
    tinyfs_env.free_bytes = TINYFS_SECTION_SIZE - tinyfs_env.tail_available_offset;
    if(tinyfs_env.tail_section>=tinyfs_env.head_section)
    {
        tinyfs_env.free_bytes += (TINYFS_SECTION_SIZE - sizeof(section_head_t))*(TINYFS_SECTION_NUM -1 - tinyfs_env.tail_section + tinyfs_env.head_section);
    }else
    {
        tinyfs_env.free_bytes += (TINYFS_SECTION_SIZE - sizeof(section_head_t))*(tinyfs_env.head_section - tinyfs_env.tail_section - 1);
    }

}

static void root_dir_init(tinyfs_node_t *root)
{
    root->child = INVALID_NODE_IDX;
    root->adjacent = INVALID_NODE_IDX;
    root->parent_dir = INVALID_NODE_IDX;
    root->next_sibling = INVALID_NODE_IDX;
    root->name = 0;
    root->addr.section = INVALID_SECTION;
    root->addr.offset = INVALID_NODE_OFFSET;
    root->u.id = 0;
}

static void node_list_init(node_list_t *list)
{
    list->head = INVALID_NODE_IDX;
    list->tail = INVALID_NODE_IDX;
}

static bool insert_check(const tinyfs_node_t *already_in_list, const tinyfs_node_t *to_insert)
{
    if(already_in_list == NULL)
    {
        return true;
    }
    if (already_in_list->parent_dir > to_insert->parent_dir)
    {
        return true;
    }else if (already_in_list->parent_dir < to_insert->parent_dir)
    {
        return false;
    }else
    {
        if(already_in_list->child&&to_insert->child)
        {
            TINYFS_ASSERT(already_in_list->u.id!=to_insert->u.id);
            if (already_in_list->u.id > to_insert->u.id)
            {
                return true;
            }else
            {
                return false;
            }
        }else
        {
            if(already_in_list->child==0)
            {
                return true;
            }else
            {
                return false;
            }
        }
    }
}

static tinyfs_node_t *sorted_list_insert(node_idx_t insert_node_idx)
{
    tinyfs_node_t *insert_node_base = node_ptr(insert_node_idx);
    tinyfs_node_t *curr = root_dir;
    while(1)
    {
        tinyfs_node_t *next = node_ptr(curr->next_sibling);
        if(insert_check(next,insert_node_base))
        {
            insert_node_base->next_sibling = curr->next_sibling;
            curr->next_sibling = insert_node_idx;
            break;
        }
        curr=next;
    }
    return insert_node_base;
}

static void dir_tree_link(tinyfs_node_t *const parent,tinyfs_node_t *const start)
{
    TINYFS_ASSERT(parent->child);
    tinyfs_node_t *first_child = start;
    while(first_child&&first_child->parent_dir!=parent->u.id)
    {
        first_child = node_ptr(first_child->next_sibling);
    }
    tinyfs_node_t *w = first_child;
    if(first_child&&first_child->parent_dir==parent->u.id)
    {
        parent->child = node_idx(first_child);
        while(w&&w->parent_dir==parent->u.id)
        {
            w=node_ptr(w->next_sibling);
        }
    }
    if(w)
    {
        tinyfs_node_t *p = first_child;
        while(p!=w)
        {
            if(p->child)
            {
                dir_tree_link(p,w);
            }
            p = node_ptr(p->next_sibling);
        }
    }
}

static void sibling_sort()
{
    uint16_t i;
    for(i=0;i<TINYFS_SECTION_NUM;++i)
    {
        uint16_t idx;
        tinyfs_node_t *ptr;
        for(idx= tinyfs_list[i].list.head;idx!=INVALID_NODE_IDX;idx=ptr->adjacent)
        {
            ptr = sorted_list_insert(idx);
        }
    }
}

static void node_sibling_fix()
{
    uint16_t idx=node_idx(root_dir);
    tinyfs_node_t *ptr;
    while(idx!=INVALID_NODE_IDX)
    {
        ptr = node_ptr(idx);
        idx=ptr->next_sibling;
        tinyfs_node_t *next = node_ptr(idx);
        if(next&&next->parent_dir!=ptr->parent_dir)
        {
            ptr->next_sibling = INVALID_NODE_IDX;
        }
    }
}

static void node_parent_fix(tinyfs_node_t *parent)
{
    tinyfs_node_t *ptr = node_ptr(parent->child);
    while(ptr && ptr->parent_dir == parent->u.id)
    {
        if(ptr->child)
        {
            node_parent_fix(ptr);
        }
        ptr->parent_dir = node_idx(parent);
        ptr = node_ptr(ptr->next_sibling);
    }
}

static void dir_tree_build()
{
    sibling_sort();
    dir_tree_link(root_dir,node_ptr(root_dir->next_sibling));
    node_sibling_fix();
    node_parent_fix(root_dir);
}

static void tinyfs_list_init(tinyfs_list_t *env,uint16_t len)
{
    uint16_t i;
    for(i=0;i<len;++i)
    {
        node_list_init(&env[i].list);
        env->size = 0;
    }
}

static bool all_section_empty(uint8_t *valid_mask,uint32_t size)
{
    uint16_t i;
    for(i=0;i<size;++i)
    {
        if(valid_mask[i])
        {
            return false;
        }
    }
    return true;
}

void tinyfs_init(uint32_t base)
{
    TINYFS_ASSERT(base % TINYFS_SECTION_SIZE == 0);
    INIT_LINKED_BUF(tinyfs_node);
    tinyfs_env.base = base;
    root_dir_ptr = node_alloc();
    root_dir_init(root_dir);
    tinyfs_list_init(tinyfs_list,TINYFS_SECTION_NUM);
    section_head_data_t head_data[TINYFS_SECTION_NUM];
    uint8_t valid_section[CEILING(TINYFS_SECTION_NUM, 8)] = {0};
    section_head_read_all(head_data,valid_section);
    if (all_section_empty(valid_section,sizeof(valid_section))==false)
    {
        find_head_tail_section(head_data,valid_section,
            &tinyfs_env.head_section, &tinyfs_env.tail_section,&tinyfs_env.tail_count);
        nodes_info_load(head_data,valid_section);
        dir_tree_build();
    }else
    {
        tinyfs_env.head_section = 0;
        tinyfs_env.tail_section = get_prev_section(0);
        tinyfs_env.tail_available_offset = TINYFS_SECTION_SIZE;
        tinyfs_env.tail_count = 0;
        tinyfs_env.dir_id_max = 0;
        tinyfs_env.free_bytes = TINYFS_SECTION_NUM * (TINYFS_SECTION_SIZE - sizeof(section_head_t));
    }
    tinyfs_mutex_create();
}


static void section_head_write(uint16_t section,uint16_t loop_count,uint16_t node_offset)
{
    section_head_t section_head;
    section_head.loop_count = loop_count;
    section_head.node_offset = node_offset;
    section_head.crc16 = crc16calc(TINYFS_CRC_INIT_VAL,&section_head,sizeof(section_head_t)-TINYFS_CRC_LENGTH);
    tinyfs_nvm_program(GET_FLASH_ADDR(section,0),sizeof(section_head_t),(uint8_t *)&section_head);
}

static storage_addr_t node_write(uint8_t *buf,uint16_t length,enum node_write_type type,uint16_t subsequent_length)
{
    tinyfs_env.free_bytes -= length;
    storage_addr_t addr;
    if(tinyfs_env.tail_available_offset == TINYFS_SECTION_SIZE)
    {
        addr.offset = sizeof(section_head_t);
        addr.section = get_next_section(tinyfs_env.tail_section);
    }else
    {
        addr.offset = tinyfs_env.tail_available_offset;
        addr.section = tinyfs_env.tail_section;
    }
    while(length)
    {
        if(tinyfs_env.tail_available_offset == TINYFS_SECTION_SIZE)
        {
           uint16_t node_offset;
           if(type == PROGRAM_START)
           {
               node_offset = sizeof(section_head_t);
           }else
           {
               if(length+subsequent_length+sizeof(section_head_t)>=TINYFS_SECTION_SIZE)
               {
                   node_offset = INVALID_NODE_OFFSET;
               }else
               {
                   node_offset = length+subsequent_length+sizeof(section_head_t);
               }
           }
           tinyfs_env.tail_section=get_next_section(tinyfs_env.tail_section);
           section_head_write(tinyfs_env.tail_section,++tinyfs_env.tail_count,node_offset);
           tinyfs_env.tail_available_offset = sizeof(section_head_t);
        }
        if(length+tinyfs_env.tail_available_offset>TINYFS_SECTION_SIZE)
        {
            tinyfs_nvm_program(GET_FLASH_ADDR(tinyfs_env.tail_section,tinyfs_env.tail_available_offset),
                TINYFS_SECTION_SIZE-tinyfs_env.tail_available_offset,buf);
            buf += TINYFS_SECTION_SIZE - tinyfs_env.tail_available_offset;
            length -= TINYFS_SECTION_SIZE - tinyfs_env.tail_available_offset;
            tinyfs_env.tail_available_offset = TINYFS_SECTION_SIZE;
        }else
        {
            tinyfs_nvm_program(GET_FLASH_ADDR(tinyfs_env.tail_section,tinyfs_env.tail_available_offset),
                length,buf);
            tinyfs_env.tail_available_offset += length;
            length = 0;
        }
        type = PROGRAM_CONTINUE;
    }
    return addr;
}

static storage_addr_t record_add_write(uint16_t name,uint16_t parent_id,uint16_t data_length)
{
    record_add_t node;
    node.type = RECORD_ADD;
    node.name = name;
    node.parent_id = parent_id;
    node.data_length = data_length;
    node.crc16 = crc16calc(TINYFS_CRC_INIT_VAL,&node,sizeof(record_add_t)-TINYFS_CRC_LENGTH);
    return node_write((uint8_t *)&node,sizeof(node),PROGRAM_START,data_length + TINYFS_CRC_LENGTH);
}

static storage_addr_t record_remove_write(uint16_t name,uint16_t parent_id)
{
    record_remove_t node;
    node.type = RECORD_REMOVE;
    node.name = name;
    node.parent_id = parent_id;
    node.crc16 = crc16calc(TINYFS_CRC_INIT_VAL,&node,sizeof(record_remove_t)-TINYFS_CRC_LENGTH);
    return node_write((uint8_t *)&node,sizeof(node),PROGRAM_START,0);
}

static storage_addr_t dir_remove_write(uint16_t id)
{
    dir_remove_t node;
    node.type = DIR_REMOVE;
    node.id = id;
    node.crc16 = crc16calc(TINYFS_CRC_INIT_VAL,&node,sizeof(dir_remove_t)-TINYFS_CRC_LENGTH);
    return node_write((uint8_t *)&node,sizeof(node),PROGRAM_START,0);
}

static storage_addr_t dir_add_write(uint16_t name,uint16_t id,uint16_t parent_id)
{
    dir_add_t node;
    node.type = DIR_ADD;
    node.name = name;
    node.id = id;
    node.parent_id = parent_id;
    node.crc16 = crc16calc(TINYFS_CRC_INIT_VAL,&node,sizeof(dir_add_t)-TINYFS_CRC_LENGTH);
    return node_write((uint8_t *)&node,sizeof(node),PROGRAM_START,0);

}

static void record_data_copy(storage_addr_t src,uint16_t length)
{
   
    uint16_t buf_size = length>RECORD_DATA_COPY_BUF_SIZE ? RECORD_DATA_COPY_BUF_SIZE : length;
    uint8_t buf[buf_size];
    section_offset_calc(&src.section, &src.offset, RECORD_ADD_LENGTH);
    while(length)
    {
        uint16_t copy_length = length>buf_size?buf_size:length;
        nvm_read_node(src.section, src.offset,copy_length,buf);
        section_offset_calc(&src.section, &src.offset, copy_length);
        length -= copy_length;
        node_write(buf, copy_length, PROGRAM_CONTINUE,length);
    }
}

static void node_copy(tinyfs_node_t *ptr)
{
    tinyfs_list_del(&tinyfs_list[ptr->addr.section],ptr);
    if(ptr->child)
    {
        ptr->addr = dir_add_write(ptr->name,ptr->u.id,node_ptr(ptr->parent_dir)->u.id);
    }else
    {
        storage_addr_t dst = record_add_write(ptr->name,node_ptr(ptr->parent_dir)->u.id,ptr->u.data_length);
        record_data_copy(ptr->addr,ptr->u.data_length+TINYFS_CRC_LENGTH);
        ptr->addr = dst;
    }
    tinyfs_list_push(&tinyfs_list[ptr->addr.section],ptr);
}

static void node_list_copy(node_list_t *list)
{
    while(list->head!=INVALID_NODE_IDX)
    {
        tinyfs_node_t *ptr = node_ptr(list->head);
        node_copy(ptr);
    }
}

static void section_copy(uint16_t section)
{
    node_list_copy(&tinyfs_list[section].list);
}

static void section_garbage_collection(uint16_t section)
{
    section_copy(section);
    data_write_through();
    tinyfs_section_erase(section);
    tinyfs_env.free_bytes += TINYFS_SECTION_SIZE - sizeof(section_head_t);
}

static bool section_wipe_check(uint16_t size,uint16_t *num)
{
    int32_t released_data_size = 0;
    uint16_t section = tinyfs_env.head_section;
    uint16_t i;
    for(i=0;i<TINYFS_SECTION_NUM;++i)
    {
        released_data_size += TINYFS_SECTION_SIZE - sizeof(section_head_t) - tinyfs_list[section].size;
        if(released_data_size > size)
        {
            *num = i+1;
            return true;
        }
        section=get_next_section(section);
    }
    return false;
}

static void garbage_collect_try(uint16_t size)
{
    uint16_t num = 0;
    bool wipe = section_wipe_check(size, &num);
    TINYFS_ASSERT(wipe);
    uint16_t i=0;
    while(tinyfs_env.free_bytes <= size + tinyfs_list[tinyfs_env.head_section].size
        + BUFFERING_SECTION_NUM*(TINYFS_SECTION_SIZE-sizeof(section_head_t)) 
        && tinyfs_env.head_section != tinyfs_env.tail_section)
    {
        TINYFS_ASSERT(i<=num);
        section_garbage_collection(tinyfs_env.head_section);
        tinyfs_env.head_section = get_next_section(tinyfs_env.head_section);
        ++i;
    }
}

static bool if_dir_exist(tinyfs_node_t *dir)
{
    bool exist = false;
    if(linked_buf_contain_element(&tinyfs_node,&dir->hdr))
    {
        if(dir->child)
        {
            exist = true;
        }
    }
    return exist;
}

static tinyfs_node_t *get_record_by_name(tinyfs_node_t *parent, uint16_t record_name, tinyfs_node_t **prev_node)
{
    node_idx_t idx = parent->child;
    if (prev_node)
    {
        *prev_node = NULL;
    }
    while (idx != INVALID_NODE_IDX)
    {
        tinyfs_node_t *ptr = node_ptr(idx);
        if (ptr->child == 0 && ptr->name == record_name)
        {
            return ptr;
        }
        if (prev_node)
        {
            *prev_node = ptr;
        }
        idx = ptr->next_sibling;
    }
    return NULL;
}

static uint8_t do_tinyfs_read(tinyfs_node_t *parent, uint16_t record_name, uint8_t *data, uint16_t *length_ptr)
{
    tinyfs_node_t *record = get_record_by_name(parent, record_name, NULL);
    if (record)
    {
        if (record->u.data_length <= *length_ptr)
        {
            if(node_data_read(record->addr.section, record->addr.offset, record->u.data_length, data))
            {
                *length_ptr = record->u.data_length;
                return TINYFS_NO_ERROR;
            }else
            {
                return TINYFS_RECORD_DATA_CORRUPTED;
            }
        }
        else
        {
            return TINYFS_INSUFFICIENT_RECORD_DATA_BUF;
        }
    }
    else
    {
        return TINYFS_RECORD_KEY_NOT_FOUND;
    }
}


uint8_t tinyfs_read(tinyfs_dir_t dir, uint16_t record_name, uint8_t *data, uint16_t *length_ptr)
{
    uint8_t error;
    TINYFS_ASSERT(dir);
    tinyfs_mutex_lock();
    if (if_dir_exist(dir))
    {
        error = do_tinyfs_read(dir,record_name, data, length_ptr);
    }else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
    tinyfs_mutex_unlock();
    return error;
}

bool tinyfs_record_exist(tinyfs_dir_t dir,uint16_t record_name)
{
    bool exist = false;
    tinyfs_mutex_lock();
    if(if_dir_exist(dir))
    {
        exist = get_record_by_name(dir, record_name, NULL) ? true : false;
    }
    tinyfs_mutex_unlock();
    return exist;
}

static tinyfs_node_t *get_child_dir_by_name(tinyfs_node_t *parent, uint16_t dir_name)
{
    uint16_t idx;
    tinyfs_node_t *ptr;
    for (idx = parent->child; idx != INVALID_NODE_IDX; idx = ptr->next_sibling)
    {
        ptr = node_ptr(idx);
        if (ptr->child && ptr->name  == dir_name)
        {
            return ptr;
        }
    }
    return NULL;
}

uint8_t tinyfs_get_child_dir_by_name(tinyfs_dir_t parent, uint16_t dir_name, tinyfs_dir_t *ptr)
{
    uint8_t err = TINYFS_DIR_NOT_EXISTED;

    tinyfs_mutex_lock();

    *ptr = (tinyfs_dir_t)get_child_dir_by_name(parent, dir_name);

    if(*ptr) {
        err = TINYFS_NO_ERROR;
    }

    tinyfs_mutex_unlock();

    return err;
}

tinyfs_node_t *hierarchy_dir_get(va_list *key_list, uint8_t depth)
{
    uint8_t i;
    tinyfs_node_t *dir = root_dir;
    for (i = 0; i < depth; ++i)
    {
        uint16_t dir_name = va_arg(*key_list, uint32_t);
        dir = get_child_dir_by_name(dir, dir_name);
        if (dir == NULL)
        {
            break;
        }
    }
    return dir;
}

uint8_t tinyfs_hierarchy_read(tinyfs_hierarchy_rw_t *param, ...)
{
    uint8_t error;
    tinyfs_node_t *dir;
    va_list key_list;
    tinyfs_mutex_lock();
    va_start(key_list, param);
    dir = hierarchy_dir_get(&key_list, param->dir_depth);
    va_end(key_list);
    if (dir)
    {
        error = do_tinyfs_read(dir, param->record_key, param->data, &param->length);
    }
    else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
    tinyfs_mutex_unlock();
    return error;
}

static tinyfs_node_t *get_parent_dir(tinyfs_node_t *dir)
{
    return node_ptr(dir->parent_dir);
}

uint8_t tinyfs_mkdir(tinyfs_dir_t *dir_to_make, tinyfs_dir_t upper_dir, uint16_t dir_name)
{
    uint8_t error;
    TINYFS_ASSERT(upper_dir);
    tinyfs_mutex_lock();
    if (if_dir_exist(upper_dir))
    {
        tinyfs_node_t *child = get_child_dir_by_name(upper_dir, dir_name);
        if (child)
        {
            *dir_to_make = child;
            error = TINYFS_DIR_KEY_ALREADY_EXISTED;
            goto ret;
        }
        if (tinyfs_env.dir_id_max == 0xffff)
        {
            error = TINYFS_DIR_IDX_OVERFLOW;
            goto ret;
        }
        tinyfs_node_t *new_dir_node = node_alloc();
        if (new_dir_node == NULL)
        {
            error = TINYFS_INSUFFICIENT_NODE_BUF;
        }
        else
        {
            tinyfs_node_t *parent = upper_dir;
            new_dir_node->u.id = ++tinyfs_env.dir_id_max;
            new_dir_node->name = dir_name;
            new_dir_node->next_sibling = parent->child;
            parent->child = node_idx(new_dir_node);
            new_dir_node->child = INVALID_NODE_IDX;
            new_dir_node->parent_dir = node_idx(parent);
            garbage_collect_try(sizeof(dir_add_t));
            new_dir_node->addr = dir_add_write(dir_name, new_dir_node->u.id,parent->u.id);
            tinyfs_list_push(&tinyfs_list[new_dir_node->addr.section],new_dir_node);
            *dir_to_make = new_dir_node;
            error = TINYFS_NO_ERROR;
        }
    }
    else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
ret:
    tinyfs_mutex_unlock();
    return error;
}

static void record_data_write(uint8_t *buf,uint16_t length)
{
    node_write(buf,length,PROGRAM_CONTINUE,TINYFS_CRC_LENGTH);
    uint16_t crc16 = crc16calc(TINYFS_CRC_INIT_VAL,buf,length);
    node_write((uint8_t *)&crc16,sizeof(crc16),PROGRAM_CONTINUE,0);
}

static void record_write(tinyfs_node_t *record,uint8_t *data,uint16_t length,uint16_t parent_id)
{
    garbage_collect_try(length + TINYFS_CRC_LENGTH + sizeof(record_add_t));
    record->u.data_length = length;
    record->addr = record_add_write(record->name,parent_id, length);
    record_data_write(data,length);
    tinyfs_list_push(&tinyfs_list[record->addr.section],record);
}

static uint8_t do_tinyfs_write(tinyfs_node_t *parent, uint16_t record_name, uint8_t *data, uint16_t length)
{
    tinyfs_node_t *record = get_record_by_name(parent, record_name, NULL);
    if (record)
    {
        bool del = tinyfs_list_del(&tinyfs_list[record->addr.section],record);
        TINYFS_ASSERT(del);
        record_write(record,data,length,parent->u.id);
        return TINYFS_NO_ERROR;
    }
    else
    {
        record = node_alloc();
        if (record)
        {
            record->child = 0;
            record->parent_dir = node_idx(parent);
            record->name = record_name;
            record->next_sibling = parent->child;
            parent->child = node_idx(record);
            record_write(record,data,length,parent->u.id);
            return TINYFS_NO_ERROR;
        }
        else
        {
            return TINYFS_INSUFFICIENT_NODE_BUF;
        }
    }
}

uint8_t tinyfs_write(tinyfs_dir_t dir, uint16_t record_name, uint8_t *data, uint16_t length)
{
    uint8_t error;
    TINYFS_ASSERT(dir);
    TINYFS_ASSERT(data);
    TINYFS_ASSERT(length);
    tinyfs_mutex_lock();
    if (if_dir_exist(dir))
    {
        error = do_tinyfs_write(dir, record_name, data, length);
    }else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
    tinyfs_mutex_unlock();
    return error;
}

static void node_del(tinyfs_node_t *parent,tinyfs_node_t *node_to_del,tinyfs_node_t *prev)
{
    if(node_to_del->child)
    {
        garbage_collect_try(sizeof(dir_remove_t));
        dir_remove_write(node_to_del->u.id);
    }else
    {
        garbage_collect_try(sizeof(record_remove_t));
        record_remove_write(node_to_del->name, parent->u.id);
    }
    if(prev)
    {
        prev->next_sibling = node_to_del->next_sibling;
    }else
    {
        parent->child = node_to_del->next_sibling;
    }
    tinyfs_list_del(&tinyfs_list[node_to_del->addr.section],node_to_del);
    node_free(node_to_del);
}

static uint8_t do_tinyfs_del_record(tinyfs_node_t *dir, uint16_t record_name)
{
    tinyfs_node_t *prev_node;
    tinyfs_node_t *record = get_record_by_name(dir, record_name, &prev_node);
    if (record)
    {
        node_del(dir,record,prev_node);
        return TINYFS_NO_ERROR;
    }else
    {
        return TINYFS_RECORD_KEY_NOT_FOUND;
    }
}

uint8_t tinyfs_del_record(tinyfs_dir_t dir, uint16_t record_name)
{
    uint8_t error;
    TINYFS_ASSERT(dir);
    tinyfs_mutex_lock();
    if (if_dir_exist(dir))
    {
        error = do_tinyfs_del_record(dir, record_name);
    }else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
    tinyfs_mutex_unlock();
    return error;
}

static void node_del_recursively(tinyfs_node_t *dir)
{
    node_idx_t idx = dir->child;
    while (idx != INVALID_NODE_IDX)
    {
        tinyfs_node_t *ptr = node_ptr(idx);
        if(ptr->child)
        {
            node_del_recursively(ptr);
        }
        idx = ptr->next_sibling;
        node_del(dir,ptr,NULL);
    }
}

static tinyfs_node_t *get_prev_node(tinyfs_node_t *parent, tinyfs_node_t *dir)
{
    uint16_t idx;
    tinyfs_node_t *prev = NULL;
    tinyfs_node_t *ptr;
    for (idx = parent->child; idx != INVALID_NODE_IDX; idx = ptr->next_sibling)
    {
        ptr = node_ptr(idx);
        if (ptr == dir)
        {
            return prev;
        }
        prev = ptr;
    }
    return NULL;
}

static bool if_dir_empty(tinyfs_node_t *dir)
{
    return dir->child == INVALID_NODE_IDX  ? true : false;
}

uint8_t tinyfs_del_dir(tinyfs_dir_t dir, bool force)
{
    uint8_t error;
    TINYFS_ASSERT(dir);
    tinyfs_mutex_lock();
    if (if_dir_exist(dir))
    {
        tinyfs_node_t *parent = get_parent_dir(dir);
        tinyfs_node_t *prev = get_prev_node(parent, dir);
        if (force)
        {
            node_del_recursively(dir);
            node_del(parent,dir,prev);
            error =  TINYFS_NO_ERROR;
        }else
        {
            if (if_dir_empty(dir) == false)
            {
                error =  TINYFS_DIR_NOT_EMPTY;
            }else
            {
                node_del(parent,dir,prev);
                error = TINYFS_NO_ERROR;
            }
        }
    }else
    {
        error = TINYFS_DIR_NOT_EXISTED;
    }

    tinyfs_mutex_unlock();
    return error;
}



uint8_t tinyfs_hierarchy_write(tinyfs_hierarchy_rw_t *param, ...)
{
    uint8_t error;
    tinyfs_node_t *dir;
    va_list key_list;
    tinyfs_mutex_lock();
    va_start(key_list, param);
    dir = hierarchy_dir_get(&key_list, param->dir_depth);
    va_end(key_list);
    if (dir)
    {
        error = do_tinyfs_write(dir, param->record_key, param->data, param->length);
    }else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
    tinyfs_mutex_unlock();
    return error;
}

static uint8_t do_tinyfs_record_list_get(tinyfs_node_t *dir, uint16_t *num, uint16_t *list)
{
    uint16_t i = 0;
    uint16_t idx;
    tinyfs_node_t *parent = dir;
    tinyfs_node_t *ptr;
    for (idx = parent->child; idx != INVALID_NODE_IDX; idx = ptr->next_sibling)
    {
        ptr = node_ptr(idx);
        if (i == *num)
        {
            return TINYFS_TMP_BUF_OVERFLOW;
        }
        if(ptr->child==0)
        {
            list[i++] = ptr->name;
        }
    }
    *num = i;
    return TINYFS_NO_ERROR;
}

uint8_t tinyfs_record_list_get(tinyfs_dir_t dir, uint16_t *num, uint16_t *list)
{
    uint8_t error;
    TINYFS_ASSERT(dir);
    tinyfs_mutex_lock();
    if (if_dir_exist(dir))
    {
        error = do_tinyfs_record_list_get(dir, num, list);
    }else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
    tinyfs_mutex_unlock();
    return error;
}

uint8_t tinyfs_hierarchy_record_list_get(tinyfs_hierarchy_record_list_t *param, ...)
{
    uint8_t error;
    tinyfs_node_t *dir;
    va_list key_list;
    tinyfs_mutex_lock();
    va_start(key_list, param);
    dir = hierarchy_dir_get(&key_list, param->dir_depth);
    va_end(key_list);
    if (dir)
    {
        error = do_tinyfs_record_list_get(dir, &param->num, param->list);
    }else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
    tinyfs_mutex_unlock();
    return error;
}

uint8_t tinyfs_hierarchy_del_record(tinyfs_hierarchy_del_record_t *param, ...)
{
    uint8_t error;
    tinyfs_node_t *dir;
    va_list key_list;
    tinyfs_mutex_lock();
    va_start(key_list, param);
    dir = hierarchy_dir_get(&key_list, param->dir_depth);
    va_end(key_list);
    if (dir)
    {
        error = do_tinyfs_del_record(dir, param->record_key);
    }else
    {
        error = TINYFS_PARENT_DIR_NOT_FOUND;
    }
    tinyfs_mutex_unlock();
    return error;
}

uint16_t tinyfs_write_through()
{
    tinyfs_mutex_lock();
    uint16_t padding_length = 0;
    padding_length = data_write_through();
    tinyfs_mutex_unlock();
    return padding_length;
}

static uint16_t data_write_through()
{
    uint16_t padding_length = 0;
    if(tinyfs_env.tail_available_offset % TINYFS_WRITE_CACHE_SIZE)
    {
        padding_length = TINYFS_WRITE_CACHE_SIZE - tinyfs_env.tail_available_offset % TINYFS_WRITE_CACHE_SIZE;
        tinyfs_env.free_bytes -= padding_length;
        tinyfs_env.tail_available_offset =  (tinyfs_env.tail_available_offset + TINYFS_WRITE_CACHE_SIZE) & ~(TINYFS_WRITE_CACHE_SIZE - 1);

    }
    tinyfs_nvm_write_through();
    return padding_length;
}

void print_indent(uint8_t depth, uint32_t indent_cnt, uint8_t is_dir)
{
    uint8_t i;
    if(depth > 0)
    {
        for(i = 0; i < depth-1; i++)
        {
            if((indent_cnt << (31 - i)) & 0x80000000)
            {
                LOG_RAW("| ");
            }
            else
            {
                LOG_RAW("  ");
            }
        }
        if(is_dir == 0)
        {
            LOG_RAW("|+");
        }
        else
        {
            LOG_RAW("|-");
        }
    }
}

void traverse_tree(tinyfs_node_t* p_node, uint8_t depth, uint32_t indent_cnt)
{
    tinyfs_node_t* p_node_bkup = NULL;
    uint32_t indent_cnt_bkup = indent_cnt;
    if(p_node)
    {
        if(p_node->child != INVALID_NODE_IDX && p_node->child != 0) 
        {
            tinyfs_node_t* p_node_child = (tinyfs_node_t*)&tinyfs_node.buf[p_node->child*tinyfs_node.element_size];
            if(p_node_child->next_sibling == INVALID_NODE_IDX || p_node_child->next_sibling == 0)
            {
                indent_cnt &= ~(1<<depth);
            }
        }
        print_indent(depth, indent_cnt, p_node->child);
        LOG_RAW("%d(%d)\r\n", p_node->name, node_idx(p_node));
        p_node_bkup = p_node;
        p_node = ((p_node->child == INVALID_NODE_IDX)||(p_node->child == 0))? NULL:(tinyfs_node_t*)&tinyfs_node.buf[p_node->child*tinyfs_node.element_size];
        traverse_tree(p_node, depth+1, indent_cnt);
        p_node = p_node_bkup;
        indent_cnt = indent_cnt_bkup;
        if(p_node->next_sibling != INVALID_NODE_IDX )  
        {
            tinyfs_node_t* p_node_next_sibling = (tinyfs_node_t*)&tinyfs_node.buf[p_node->next_sibling*tinyfs_node.element_size];
            if(p_node_next_sibling->next_sibling == INVALID_NODE_IDX || p_node_next_sibling->next_sibling == 0)
            {
                indent_cnt &= ~(1 << (depth -1));
            }
        }
        p_node = p_node->next_sibling == INVALID_NODE_IDX? NULL:(tinyfs_node_t*)&tinyfs_node.buf[p_node->next_sibling*tinyfs_node.element_size];
        traverse_tree(p_node, depth, indent_cnt);
    }
}


void tinyfs_earse_data(void)
{
    uint16_t i;
    for(i=0;i<TINYFS_SECTION_NUM;++i)
    {
        tinyfs_section_erase(i);
    }
}

uint16_t tinyfs_get_dir_id_max(void)
{
    return tinyfs_env.dir_id_max;
}

void tinyfs_print_dir_tree(void)
{
    
    LOG_I("directory tree print:(-dir(idx) +record(idx))");
    traverse_tree((tinyfs_node_t*)&tinyfs_node.buf[0], 0, 0xffffffff);
    LOG_RAW("\r\n");
}


