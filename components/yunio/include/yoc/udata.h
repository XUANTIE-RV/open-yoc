/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef UDATA_H
#define UDATA_H

#include <stdlib.h>

#include <aos/list.h>

enum {
    TYPE_UNSET = 0,
    TYPE_INT,
    TYPE_STR,
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_OBJ
};

typedef struct _udata uData;
typedef struct _value Value;

struct _value {
    int type;
    int updated;
    union {
        int v_int;
        unsigned char v_bool;
        float v_float;
        char *v_str;
        uData *v_obj;
    };
};

void value_init(Value *v);
void value_uninit(Value *v1);

int value_cmp(const Value *v1, const Value *v2);
int value_cmp_str(const Value *v1, char *v_str);
int value_cmp_int(const Value *v1, int v_int);

int value_copy(Value *v1, const Value *v2);
int value_set(Value *v, int type, ...);

Value value(int type, ...);

#define value_i(v) value(TYPE_INT, v)
#define value_s(v) value(TYPE_STR, v)
#define value_b(v) value(TYPE_BOOL, v)
#define value_f(v) value(TYPE_FLOAT, (double)v)

struct _udata {
    slist_t head;
    Value key;
    Value value;
};

typedef int (*josn_node_cb_t)(const char *json, const char *key, const char *val, int type, void *arg);

int yoc_json_search(const char *json, josn_node_cb_t iter_node_cb, void *arg);

uData *yoc_udata_new();
void yoc_udata_free(uData *data);

uData *yoc_udata_get(uData *data, Value key);
int    yoc_udata_set(uData *data, Value key, Value value, int force_set_update_flag);
void yoc_udata_clear_flag_all(uData *data);
int yoc_udata_set_flag(uData *udata, Value key, int flag);

int yoc_data_print(uData *data);
int yoc_udata_to_json(uData *data, char *buffer, size_t len, int check_update);
int yoc_udata_from_json(uData *data, char *json);

#endif
