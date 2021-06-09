#ifndef TINYFS_H_
#define TINYFS_H_
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define ROOT_DIR (root_dir_ptr)

enum tinyfs_error_code
{
    TINYFS_NO_ERROR,
    TINYFS_RECORD_DATA_CORRUPTED,
    TINYFS_INSUFFICIENT_NODE_BUF,
    TINYFS_INSUFFICIENT_RECORD_DATA_BUF,
    TINYFS_DIR_IDX_OVERFLOW,
    TINYFS_PARENT_DIR_NOT_FOUND,
    TINYFS_DIR_NOT_EXISTED,
    TINYFS_DIR_NOT_EMPTY,
    TINYFS_DIR_KEY_ALREADY_EXISTED,
    TINYFS_RECORD_KEY_ALREADY_EXISTED,
    TINYFS_RECORD_KEY_NOT_FOUND,
    TINYFS_TMP_BUF_OVERFLOW,
};

typedef void * tinyfs_dir_t;

typedef struct
{
    uint8_t *data;
    uint16_t length;
    uint16_t record_key;
    uint8_t dir_depth;
}tinyfs_hierarchy_rw_t;

typedef struct
{
    uint16_t *list;
    uint16_t num;
    uint8_t dir_depth;
}tinyfs_hierarchy_record_list_t;

typedef struct
{
    uint16_t record_key;
    uint8_t dir_depth;
}tinyfs_hierarchy_del_record_t;

extern void *root_dir_ptr;

void tinyfs_init(uint32_t base);

void tinyfs_print_dir_tree(void);

uint8_t tinyfs_mkdir(tinyfs_dir_t *dir_to_make,tinyfs_dir_t upper_dir,uint16_t dir_name);

uint8_t tinyfs_write(tinyfs_dir_t dir,uint16_t record_name,uint8_t *data,uint16_t length);

uint8_t tinyfs_read(tinyfs_dir_t dir,uint16_t record_name,uint8_t *data,uint16_t *length_ptr);

uint8_t tinyfs_del_record(tinyfs_dir_t dir,uint16_t record_name);

uint8_t tinyfs_del_dir(tinyfs_dir_t dir,bool force);

uint8_t tinyfs_hierarchy_write(tinyfs_hierarchy_rw_t *param,...);

uint8_t tinyfs_hierarchy_read(tinyfs_hierarchy_rw_t *param,...);

uint8_t tinyfs_record_list_get(tinyfs_dir_t dir,uint16_t *num,uint16_t *list);

uint8_t tinyfs_hierarchy_record_list_get(tinyfs_hierarchy_record_list_t *param,...);

uint8_t tinyfs_hierarchy_del_record(tinyfs_hierarchy_del_record_t *param,...);

uint16_t tinyfs_write_through(void);

bool tinyfs_record_exist(tinyfs_dir_t dir,uint16_t record_name);

void tinyfs_earse_data(void);

uint8_t tinyfs_get_child_dir_by_name(tinyfs_dir_t parent, uint16_t dir_name, tinyfs_dir_t *ptr);

uint16_t tinyfs_get_dir_id_max(void);

#endif
