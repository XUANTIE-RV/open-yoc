/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <aos/kernel.h>
#include <aos/debug.h>

#include <yoc/udata.h>

void value_init(Value *v)
{
    memset(v, 0, sizeof(Value));
}

void value_uninit(Value *v)
{
    if (v->type == TYPE_STR && v->v_str) {
        aos_free(v->v_str);
    }

    memset(v, 0, sizeof(Value));
}

int value_cmp(const Value *v1, const Value *v2)
{
    if (v1->type == v2->type) {
        switch (v1->type) {
            case TYPE_BOOL:
                return v1->v_bool - v2->v_bool;

            case TYPE_INT:
                return v1->v_int - v2->v_int;

            case TYPE_FLOAT:
                return v1->v_float - v2->v_float;

            case TYPE_STR:
                return strcmp(v1->v_str, v2->v_str);
        }
    }

    return -1;
}

int value_cmp_str(const Value *v1, char *v_str)
{
    int ret;

    Value v2;

    ret = value_set(&v2, TYPE_STR, v_str);

    if (ret == 0) {
        ret = value_cmp(v1, &v2);
        value_uninit(&v2);
        return ret;
    }

    return -1;
}

int value_cmp_int(const Value *v1, int v_int)
{
    int ret;

    Value v2;

    ret = value_set(&v2, TYPE_INT, v_int);

    if (ret == 0) {
        ret = value_cmp(v1, &v2);
        value_uninit(&v2);
        return ret;
    }

    return -1;
}

int value_copy(Value *v1, const Value *v2)
{
    value_uninit(v1);

    v1->type = v2->type;

    switch (v2->type) {
        case TYPE_BOOL:
            v1->v_bool = v2->v_bool;
            break;

        case TYPE_INT:
            v1->v_int = v2->v_int;
            break;

        case TYPE_STR:
            v1->v_str = strdup(v2->v_str);
            break;

        case TYPE_FLOAT:
            v1->v_float = v2->v_float;
            break;

        case TYPE_OBJ:
            //TODO:
            break;

        default:
            return -1;
    }

    return 0;
}

static int value_setv(Value *v, int type, va_list args)
{
    v->type = type;

    switch (type) {
        case TYPE_BOOL:
            v->v_bool = va_arg(args, int);
            return 0;

        case TYPE_INT:
            v->v_int = va_arg(args, int);
            return 0;

        case TYPE_STR:
            v->v_str = strdup(va_arg(args, char *));
            return 0;

        case TYPE_FLOAT:
            /* va_arg(args, float) is not support */
            v->v_float = va_arg(args, double);
            return 0;

        case TYPE_OBJ:
            //TODO:
            return -1;
    }

    return 0;
}

Value value(int type, ...)
{
    Value   v;
    va_list args;

    memset(&v, 0, sizeof(Value));
    va_start(args, type);
    value_setv(&v, type, args);
    va_end(args);

    return v;
}

int value_set(Value *v, int type, ...)
{
    int     ret;
    va_list args;

    Value v_new;
    memset(&v_new, 0, sizeof(Value));

    va_start(args, type);
    ret = value_setv(&v_new, type, args);
    va_end(args);

    if (value_cmp(v, &v_new) == 0) {
        value_uninit(&v_new);
    } else {
        value_uninit(v);
        *v         = v_new;
        v->updated = 1;
    }

    return ret;
}

static uData *_udata_find(uData *data, Value key)
{
    uData *node;
    slist_for_each_entry(&data->head, node, uData, head)
    {
        if (value_cmp(&node->key, &key) == 0) {
            return node;
        }
    }

    return NULL;
}

uData *yoc_udata_new()
{
    uData *data = aos_zalloc(sizeof(uData));
    //value_init(&data->key);
    //value_init(&data->value);
    //slist_init(&data->head);

    return data;
}

void yoc_udata_free(uData *data)
{
    uData *  node;
    slist_t *tmp;
    slist_for_each_entry_safe(&data->head, tmp, node, uData, head)
    {
        //yoc_udata_free(node);
        value_uninit(&node->key);
        value_uninit(&node->value);
        slist_del(&node->head, &data->head);
        aos_free(node);
    }

    value_uninit(&data->key);
    value_uninit(&data->value);
    aos_free(data);

    //while (!slist_empty(&data->head)) {
    //    node = pdev->bufs.next;
    //    slist_del(n);
    //}
}

uData *yoc_udata_get(uData *data, Value key)
{
    uData *node = NULL;
    slist_for_each_entry(&data->head, node, uData, head)
    {
        if (value_cmp(&node->key, &key) == 0) {
            value_uninit(&key);
            return node;
        }
    }

    value_uninit(&key);

    return NULL;
}

int yoc_udata_set(uData *data, Value key, Value value, int force_set_update_flag)
{
    aos_check_param(data);

    uData *d = _udata_find(data, key);

    if (d == NULL) {
        d = yoc_udata_new();

        if (d) {
            d->key   = key;
            d->value = value;
            /* new node not set update flag*/
            d->value.updated = 0;
            slist_add_tail(&d->head, &data->head);
        }
    } else {
        if (value_cmp(&value, &d->value) != 0) {
            value.updated = 1;
        }

        value_uninit(&key);
        value_uninit(&d->value);
        d->value = value;
    }

    /* force set update */
    if (d && force_set_update_flag) {
        d->value.updated = 1;
    }

    return d ? 0 : -1;
}

void yoc_udata_clear_flag_all(uData *data)
{
    uData *node;
    slist_for_each_entry(&data->head, node, uData, head)
    {
        node->value.updated = 0;
        node->key.updated   = 0;
    }
}

int yoc_udata_set_flag(uData *udata, Value key, int flag)
{
    uData *node;
    node = yoc_udata_get(udata, key);

    if (node) {
        node->value.updated = flag;
        return 0;
    }

    return -1;
}

int yoc_udata_to_json(uData *data, char *buffer, size_t len, int check_update)
{
    uData *node;

    int   ret;
    char *ptr = buffer;

    ret = snprintf(ptr, len, "%c", '{');

    if (ret >= len) {
        return -1;
    }

    ptr += ret;
    len -= ret;

    slist_for_each_entry(&data->head, node, uData, head)
    {
        if (check_update && !node->value.updated) {
            continue;
        }

        switch (node->key.type) {
            case TYPE_INT:
                ret = snprintf(ptr, len, "\"%d\":", node->key.v_int);
                break;

            case TYPE_STR:
                ret = snprintf(ptr, len, "\"%s\":", node->key.v_str);
                break;

            default:
                ret = 0;
                break;
        }

        if (ret >= len) {
            return -1;
        }

        ptr += ret;
        len -= ret;

        switch (node->value.type) {
            case TYPE_BOOL:
                ret = snprintf(ptr, len, "%s,", node->value.v_bool ? "true" : "false");
                break;

            case TYPE_INT:
                ret = snprintf(ptr, len, "%d,", node->value.v_int);
                break;

            case TYPE_STR:
                ret = snprintf(ptr, len, "\"%s\",", node->value.v_str);
                break;
            case TYPE_FLOAT:
                ret = snprintf(ptr, len, "\"%d\",", (int)node->value.v_float);
                break;
            default:
                snprintf(ptr, len, "\"%s\",", node->value.v_str);
                ret = 0;
                break;
        }

        if (ret >= len) {
            return -1;
        }

        ptr += ret;
        len -= ret;
    }

    /* empty */
    if (*(ptr - 1) == '{') {
        buffer[0] = '\0';
        return 0;
    }

    *(ptr - 1) = '}';

    return strlen(buffer);
}

static int josn_node_cb(const char *json, const char *key, const char *val, int type, void *arg)
{
    uData *data = (uData *)arg;

    switch (type) {
        case TYPE_INT:
            yoc_udata_set(data, value_s(key), value_i(atoi(val)), 0);
            break;
        case TYPE_STR:
            yoc_udata_set(data, value_s(key), value_s(val), 0);
            break;
        default:;
    }
    return 0;
}

int yoc_udata_from_json(uData *data, char *json)
{
    return yoc_json_search(json, josn_node_cb, data);
}

int yoc_json_search(const char *json, josn_node_cb_t iter_node_cb, void *arg)
{
    char *ptr    = (char *)json;
    char  status = '\0';
    char *key_s  = NULL;
    char *key_e  = NULL;
    char *val_s  = NULL;
    char *val_e  = NULL;

    char *key_out = NULL;
    char *val_out = NULL;

    while (1) {
        if (ptr == NULL) {
            break;
        }
        if (*ptr == '\0') {
            break;
        }
        //printf("%c\n", *ptr);

        switch (*ptr) {
            case ':':
                if (key_s != NULL && key_e == NULL) {
                    key_e = ptr;
                }
                status = ':';
                break;
            case '}':
            case '{':
            case ',':
                if (status == ':') {
                    if (key_s != NULL && key_e != NULL && val_s != NULL && val_e == NULL) {
                        val_e = ptr;

                        while (key_e > key_s && (*(key_e - 1) == ' ' || *(key_e - 1) == '\"')) {
                            key_e--;
                        }

                        if (key_s != NULL && key_e != NULL && key_e > key_s) {
                            key_out = aos_malloc(key_e - key_s + 1);
                            if (key_out) {
                                memcpy(key_out, key_s, key_e - key_s);
                                key_out[key_e - key_s] = '\0';
                            }
                        }

                        while (val_e > val_s && *(val_e - 1) == ' ') {
                            val_e--;
                        }
                        if (val_s != NULL && val_e != NULL && val_e > val_s) {
                            val_out = aos_malloc(val_e - val_s + 1);
                            if (val_out) {
                                memcpy(val_out, val_s, val_e - val_s);
                                val_out[val_e - val_s] = '\0';
                            }
                        }

                        if (key_out && val_out) {
                            if (val_out[0] == '\"') {
                                char *ch = strchr(val_out + 1, '\"');
                                if (ch) {
                                    *ch = '\0';
                                }

                                iter_node_cb(json, key_out, &val_out[1], TYPE_STR, arg);
                            } else {
                                iter_node_cb(json, key_out, val_out, TYPE_INT, arg);
                            }
                        }

                        if (key_out) {
                            aos_free(key_out);
                        }
                        if (val_out) {
                            aos_free(val_out);
                        }

                        //todo callback
                        status = '\0';
                        key_s = key_e = NULL;
                        val_s = val_e = NULL;
                    }
                } else {
                    status = '\0';
                    key_s = key_e = NULL;
                    val_s = val_e = NULL;
                }
                status = ',';
                break;
            default:
                if (status == ',') {
                    if (key_s == NULL && val_s == NULL && *ptr != ' ' && *ptr != '\"') {
                        key_s = ptr;
                    }
                } else if (status == ':') {
                    if (key_s != NULL && key_e != NULL && val_s == NULL && val_e == NULL &&
                        *ptr != ' ') {
                        val_s = ptr;
                    }
                } else {
                }
        }
        ptr++;
    }

    return 0;
}
