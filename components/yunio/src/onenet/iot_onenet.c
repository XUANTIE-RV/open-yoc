/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <aos/kernel.h>
#include <aos/log.h>
#include <aos/debug.h>
#include <yoc/iot.h>
#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include <yoc/udata.h>

#include "iot_onenet_api.h"

#define TAG "iotonet"

static iot_t         *g_iot_onenet = NULL;
static iot_channel_t *iot_onenet_channel = NULL;
static aos_timer_t g_timer_conn_timeout;

static void conn_timeout_entry(void *timer, void *arg);

int onenet_channel_open(iot_channel_t *ch)
{
    int ret;

    aos_check_return_einval(ch);


    if (iot_onenet_channel != NULL) {
        return -1;
    }

    ret = iot_onenet_miplcreate();

    if (ret < 0) {
        return -1;
    }

    iot_onenet_channel = ch;

    return 0;
}

static int get_obj_ids(uData *udata, int *ids, int count)
{
    uData *node;
    int    id_count = 0;
    int    id = 0, i = 0;

    slist_for_each_entry(&udata->head, node, uData, head)
    {
        if (id_count >= count) {
            return count;
        }

        if (node->key.type == TYPE_STR) {
            id = atoi(node->key.v_str);

            /* check exist */
            for (i = 0; i < id_count; i++) {
                if (ids[i] == id) {
                    break;
                }
            }

            if (i >= id_count) {
                /* add new id */
                ids[i] = id;
                id_count++;
            }
        } /* if */
    }     /* slist */

    return id_count;
}

static int get_obj_ins_count(uData *udata, int obj_id)
{
    int id;
    int ins;
    int i;

    char arr_ins[10];
    int  ins_count = 0;

    uData *node;
    slist_for_each_entry(&udata->head, node, uData, head)
    {
        if (ins_count >= sizeof(arr_ins)) {
            return sizeof(arr_ins);
        }

        if (node->key.type == TYPE_STR) {
            id  = atoi(node->key.v_str);
            ins = atoi(strchr(node->key.v_str, '/') + 1);

            if (id == obj_id) {
                /* check exist */
                for (i = 0; i < ins_count; i++) {
                    if (arr_ins[i] == ins) {
                        break;
                    }
                }

                if (i >= ins_count) {
                    /* add new ins */
                    arr_ins[i] = ins;
                    ins_count++;
                }
            }
        }
    } /* slist */
    return ins_count;
}

static int get_obj_att_count(uData *udata, int obj_id, int *arr_att, int count)
{
    int id;
    int ins;
    int i;
    int att;

    int  tmp_atts[32];
    int  att_max_count = sizeof(tmp_atts) / sizeof(int);
    int *ptr_att       = tmp_atts;

    int att_count = 0;

    if (arr_att != NULL && count > 0) {
        ptr_att       = arr_att;
        att_max_count = count;
    }

    uData *node;
    slist_for_each_entry(&udata->head, node, uData, head)
    {
        if (att_count >= att_max_count) {
            return att_max_count;
        }

        if (node->key.type == TYPE_STR) {
            id  = atoi(node->key.v_str);
            ins = atoi(strchr(node->key.v_str, '/') + 1);
            att = atoi(strrchr(node->key.v_str, '/') + 1);

            if (id == obj_id && ins == 0) {
                /* check exist */
                for (i = 0; i < att_count; i++) {
                    if (ptr_att[i] == att) {
                        break;
                    }
                }

                if (i >= att_count) {
                    /* add new ins */
                    ptr_att[i] = att;
                    att_count++;
                }
            }
        }
    } /* slist */

    return att_count;
}

int onenet_channel_start(iot_channel_t *ch)
{
    int ret, i;

    aos_check_return_einval(ch);


    if (ch->uData == NULL) {
        return -EINVAL;
    }

    /* uData to Objs */
    int obj_ids[10];
    int obj_count;

    aos_mutex_lock(&ch->ch_mutex, AOS_WAIT_FOREVER);
    obj_count = get_obj_ids(ch->uData, obj_ids, sizeof(obj_ids) / sizeof(int));
    aos_mutex_unlock(&ch->ch_mutex);

    /* get ins count */
    for (i = 0; i < obj_count; i++) {
        struct onenet_addobj obj;
        memset(obj.ins_map, '1', sizeof(obj.ins_map));

        obj.ref                    = 0;
        obj.obj_id                 = obj_ids[i];
        obj.ins_count              = get_obj_ins_count(ch->uData, obj_ids[i]);
        obj.ins_map[obj.ins_count] = '\0';
        obj.attr_count             = get_obj_att_count(ch->uData, obj_ids[i], NULL, 0);
        obj.action_count           = 0;

        LOGI(TAG, "obj=%d,%d,%s,%d,%d", obj.obj_id, obj.ins_count, obj.ins_map, obj.attr_count,
             obj.action_count);

        ret = iot_onenet_mipladdobj(&obj);

        if (ret < 0) {
            LOGE(TAG, "add obj %d", obj.obj_id);
            return ret;
        }
    }

    int32_t lieftime = ((iot_onenet_config_t *)ch->iot->user_cfg)->lifetime;

    ret = iot_onenet_miplopen(lieftime);

    if (ret < 0) {
        return ret;
    }

    return 0;
}

static int udata2notify(uint32_t mid, uData *node, struct onenet_notify_read *notify_data)
{
    int16_t obj_id, ins_id, res_id;

    if (node == NULL || notify_data == NULL) {
        return -1;
    }

    if (node->key.type != TYPE_STR) {
        return -1;
    }

    obj_id = atoi(node->key.v_str);
    ins_id = atoi(strchr(node->key.v_str, '/') + 1);
    res_id = atoi(strrchr(node->key.v_str, '/') + 1);

    notify_data->ref    = 0;
    notify_data->msg_id = mid;
    notify_data->obj_id = obj_id;
    notify_data->ins_id = ins_id;
    notify_data->res_id = res_id;

    if (node->value.type == TYPE_STR) {
        notify_data->value_type = cis_data_type_string;
        notify_data->len        = strlen(node->value.v_str);
        notify_data->value      = node->value.v_str;

        LOGD(TAG, "set %d/%d/%d=%s", obj_id, ins_id, res_id, node->value.v_str);
    } else if (node->value.type == TYPE_INT) {
        notify_data->value_type = cis_data_type_integer;
        notify_data->len        = 4;
        notify_data->value      = (char *)&node->value.v_int;

        LOGD(TAG, "set %d/%d/%d=%d", obj_id, ins_id, res_id, node->value.v_int);
    } else if (node->value.type == TYPE_FLOAT) {
        notify_data->value_type = cis_data_type_float;
        notify_data->len        = 4;
        notify_data->value      = (char *)&node->value.v_float;

        LOGD(TAG, "set %d/%d/%d=%f", obj_id, ins_id, res_id, node->value.v_float);
    } else if (node->value.type == TYPE_BOOL) {
        notify_data->value_type = cis_data_type_bool;
        notify_data->len        = 1;
        notify_data->value      = (char *)&node->value.v_bool;

        LOGD(TAG, "set %d/%d/%d=%db", obj_id, ins_id, res_id, node->value.v_bool);
    } else {
        return -1;
    }

    /* no ack */
    notify_data->ackid = 1;

    /* default is continue packet */
    notify_data->last = 0;

    return 0;
}

int onenet_channel_send(iot_channel_t *ch)
{
    int      ret = 0;
    int16_t  obj_id, ins_id, res_id;
    uint32_t mid;

    uData *                   node;
    struct onenet_notify_read notify_data;

    aos_mutex_lock(&ch->ch_mutex, AOS_WAIT_FOREVER);

    slist_for_each_entry(&ch->uData->head, node, uData, head)
    {
        if (node->key.type == TYPE_STR && node->value.updated) {
            obj_id = atoi(node->key.v_str);
            ins_id = atoi(strchr(node->key.v_str, '/') + 1);
            res_id = atoi(strrchr(node->key.v_str, '/') + 1);

            if (iot_onenet_get_notify_mid(obj_id, ins_id, &mid) == 0) {
                if (udata2notify(mid, node, &notify_data) == 0) {
                    notify_data.last = 1;

                    if (iot_onenet_miplnotify(&notify_data) == 0) {
                        ;
                    } else {
                        ret = -1;
                        LOGW(TAG, "%d/%d/%d notify", obj_id, ins_id, res_id);
                    }
                } else {
                    ret = -1;
                    LOGW(TAG, "%d/%d/%d udata2notify", obj_id, ins_id, res_id);
                }
            } else {
                ret = -1;
                LOGW(TAG, "%d/%d/%d notify denied", obj_id, ins_id, res_id);
            }
        }
    } /* slist */

    yoc_udata_clear_flag_all(ch->uData);

    aos_mutex_unlock(&ch->ch_mutex);

    return ret;
}

int onenet_channel_recv(iot_channel_t *ch)
{
    //TODO: cloud data convert to uData
    return 0;
}

void onenet_channel_close(iot_channel_t *ch)
{
    iot_onenet_miplclose();

    iot_onenet_mipldel();

    iot_onenet_channel = NULL;

    return;
}

int onenet_destroy(iot_t *iot)
{
    if (g_iot_onenet != iot) {
        return -EBADFD;
    }

    if (iot == NULL) {
        return -EINVAL;
    }

    if (iot->user_cfg) {
        aos_free(iot->user_cfg);
    }
    
    aos_free(g_iot_onenet);
    g_iot_onenet = NULL;

    return 0;
}


iot_t *iot_new_onenet(iot_onenet_config_t *config)
{
    if (g_iot_onenet != NULL) {
        return NULL;
    }

    g_iot_onenet = aos_malloc(sizeof(iot_t));
    if (g_iot_onenet == NULL) {
        return NULL;
    }

    g_iot_onenet->user_cfg = aos_malloc(sizeof(iot_onenet_config_t));

    if (g_iot_onenet->user_cfg == NULL) {
        return NULL;
    }

    memcpy(g_iot_onenet->user_cfg, config, sizeof(iot_onenet_config_t));

    g_iot_onenet->channel_open  = onenet_channel_open;
    g_iot_onenet->channel_start = onenet_channel_start;
    g_iot_onenet->channel_send  = onenet_channel_send;
    g_iot_onenet->channel_recv  = onenet_channel_recv;
    g_iot_onenet->channel_close = onenet_channel_close;
    g_iot_onenet->destroy       = onenet_destroy;

    return g_iot_onenet;
}

/*
*  call back process
*/
int iot_onenet_on_discover_cb(uint32_t mid, uint16_t obj_id)
{
    int ret;
    int arr_att[32];
    int count = 0;

    iot_channel_t *ch = iot_onenet_channel;

    aos_check_return_einval(ch);


    if (ch->uData == NULL) {
        return -EINVAL;
    }

    LOGD(TAG, "dis cb %d,%d\n", mid, obj_id);


    aos_mutex_lock(&ch->ch_mutex, AOS_WAIT_FOREVER);
    count = get_obj_att_count(ch->uData, obj_id, arr_att, sizeof(arr_att) / sizeof(int));
    aos_mutex_unlock(&ch->ch_mutex);

    ret = iot_onenet_mipldiscoverresp(mid, 1, arr_att, count);

    if (ret < 0) {
        LOGE(TAG, "dis cb");
        return ret;
    }

    if (g_timer_conn_timeout.hdl) {
        aos_timer_stop(&g_timer_conn_timeout);
        aos_timer_free(&g_timer_conn_timeout);
        event_publish(EVENT_IOT_CONNECT_SUCCESS, NULL);
    }

    return 0;
}

int iot_onenet_on_read_cb(uint32_t mid, uint16_t obj_id, uint16_t ins_id, uint16_t res_id)
{
    iot_channel_t *ch = iot_onenet_channel;

    aos_check_return_einval(ch);

    if (ch->uData == NULL) {
        return -EINVAL;
    }

    LOGD(TAG, "read cb 0x%x %d/%d/%d\n", mid, obj_id, ins_id, res_id);

    aos_mutex_lock(&ch->ch_mutex, AOS_WAIT_FOREVER);

    /* find uri, set key update flag */
    uData * node;
    int16_t node_obj_id, node_ins_id, node_res_id;
    slist_for_each_entry(&ch->uData->head, node, uData, head)
    {
        node_obj_id = atoi(node->key.v_str);
        node_ins_id = atoi(strchr(node->key.v_str, '/') + 1);
        node_res_id = atoi(strrchr(node->key.v_str, '/') + 1);

        if (node_obj_id == obj_id && node_ins_id == ins_id) {
            if (res_id == 0xffff || node_res_id == res_id) {
                node->value.updated = 1;
            }
        }
    }

    /* user update data from hardware */
    ch->ch_get(ch->uData, ch->ch_cfg_arg);

    /* read rsp */
    struct onenet_notify_read notify_data;
    slist_for_each_entry(&ch->uData->head, node, uData, head)
    {
        node_obj_id = atoi(node->key.v_str);
        node_ins_id = atoi(strchr(node->key.v_str, '/') + 1);
        node_res_id = atoi(strrchr(node->key.v_str, '/') + 1);

        if (node_obj_id == obj_id && node_ins_id == ins_id) {
            if (res_id == -1 || node_res_id == res_id) {
                if (udata2notify(mid, node, &notify_data) == 0) {
                    if (iot_onenet_miplreadrsp(&notify_data) == 0) {
                        ;
                    } else {
                        LOGW(TAG, "%d/%d/%d readrsp", obj_id, ins_id, res_id);
                    }
                } else {
                    LOGW(TAG, "%d/%d/%d udata2read", obj_id, ins_id, res_id);
                }
            }
        }
    }

    /* clear key update flag */
    yoc_udata_clear_flag_all(ch->uData);

    aos_mutex_unlock(&ch->ch_mutex);

    notify_data.last = 1;
    iot_onenet_miplreadrsp(&notify_data);

    return 0;
}

int iot_onenet_on_write_cb(uint16_t obj_id, uint16_t ins_id, const cis_data_t *res_data, int count)
{
    int            i;
    iot_channel_t *ch = iot_onenet_channel;

    aos_check_return_einval(ch);

    if (ch->uData == NULL) {
        return -1;
    }

    aos_mutex_lock(&ch->ch_mutex, AOS_WAIT_FOREVER);

    /* update udata form onenet */
    for (i = 0; i < count; i++) {
        const cis_data_t *res = &res_data[i];
        char              str_uri[32];

        sprintf(str_uri, "%d/%d/%d", obj_id, ins_id, res->id);

        /* onenet write just support type opaque, check type by udata */
        int    int_value;
        char * str_value;
        uData *item = yoc_udata_get(ch->uData, value_s(str_uri));

        if (item == NULL) {
            LOGE(TAG, "unknown key %s", str_uri);
            continue;
        }

        switch (item->value.type) {
            case TYPE_STR:
                str_value = aos_zalloc(res->asBuffer.length + 1);

                if (str_value) {
                    memcpy(str_value, res->asBuffer.buffer, res->asBuffer.length);
                    yoc_udata_set(ch->uData, value_s(str_uri), value_s(str_value), 1);
                    LOGD(TAG, "write cb set %s=%s", str_uri, str_value);
                } else {
                    LOGE(TAG, "mem");
                }

                break;

            case TYPE_INT:
                if (res->asBuffer.length == 1) {
                    int_value = res->asBuffer.buffer[0];
                } else if ((res->asBuffer.length == 2)) {
                    int_value = res->asBuffer.buffer[0] << 8 | res->asBuffer.buffer[1];
                } else if ((res->asBuffer.length == 4)) {
                    int_value = res->asBuffer.buffer[0] << 24 | res->asBuffer.buffer[1] << 16 |
                                res->asBuffer.buffer[2] << 8 | res->asBuffer.buffer[3];
                } else {
                    int_value = 0;
                    LOGE(TAG, "type err %d", res->asBuffer.length);
                }

                yoc_udata_set(ch->uData, value_s(str_uri), value_i(int_value), 1);
                LOGD(TAG, "write cb seti %s=%s", str_uri, res->asBuffer.buffer);
                break;

            case TYPE_BOOL:
                int_value = res->asBuffer.buffer[0];
                yoc_udata_set(ch->uData, value_s(str_uri), value_b(int_value), 1);
                LOGD(TAG, "write cb setb %s=%s", str_uri, res->asBuffer.buffer);
                break;
            default:;
        }
    }

    /* user update to hardware */
    if (ch && ch->ch_set) {
        ch->ch_set(ch->uData, ch->ch_cfg_arg);
    }

    /* clear update flag */
    yoc_udata_clear_flag_all(ch->uData);


    aos_mutex_unlock(&ch->ch_mutex);

    return 0;
}

int iot_onenet_on_exec_cb(uint16_t obj_id, uint16_t ins_id, uint16_t res_id, const uint8_t *data,
                          uint32_t len)
{
    iot_channel_t *ch = iot_onenet_channel;
    uData *node;
    char * str_value;
    char str_uri[32];

    aos_check_return_einval(ch);

    if (ch->uData == NULL || ch->ch_set == NULL) {
        return -1;
    }

    LOGD(TAG, "exec cb %u/%d/%d\n", obj_id, ins_id, res_id);

    sprintf(str_uri, "%d/%d/%d", obj_id, ins_id, res_id);

    aos_mutex_lock(&ch->ch_mutex, AOS_WAIT_FOREVER);

    node = yoc_udata_get(ch->uData, value_s(str_uri));

    switch (node->value.type) {
        case TYPE_INT:
            str_value = aos_zalloc(len + 1);

            if (str_value) {
                memcpy(str_value, data, len);
                yoc_udata_set(ch->uData, value_s(str_uri), value_i(strtol((char *)str_value, NULL, 10)), 1);
                aos_free(str_value);
            } else {
                LOGE(TAG, "mem");
            }
            break;

        case TYPE_STR:
            str_value = aos_zalloc(len + 1);

            if (str_value) {
                memcpy(str_value, data, len);
                yoc_udata_set(ch->uData, value_s(str_uri), value_s(data), 1);
            } else {
                LOGE(TAG, "mem");
            }
            break;

        case TYPE_BOOL:
            str_value = aos_zalloc(len + 1);

            if (str_value) {
                memcpy(str_value, data, len);
                yoc_udata_set(ch->uData, value_s(str_uri), value_b(strtol((char *)str_value, NULL, 10)), 1);
                aos_free(str_value);
            } else {
                LOGE(TAG, "mem");
            }
            break;

        case TYPE_FLOAT:
            str_value = aos_zalloc(len + 1);

            if (str_value) {
                memcpy(str_value, data, len);
                yoc_udata_set(ch->uData, value_s(str_uri), value_f(strtol((char *)str_value, NULL, 10)), 1);
                aos_free(str_value);
            } else {
                LOGE(TAG, "mem");
            }
            break;

        default:
            LOGE(TAG, "unsupport uData type");
    }

    /* user action */
    if (ch && ch->ch_set) {
        ch->ch_set(ch->uData, ch->ch_cfg_arg);
    }

    /* clear update flag */
    yoc_udata_set_flag(ch->uData, value_s(str_uri), 0);

    aos_mutex_unlock(&ch->ch_mutex);
    
    return 0;
}

static void conn_timeout_entry(void *timer, void *arg)
{
    onenet_channel_close(NULL);
    event_publish(EVENT_IOT_CONNECT_FAILED, NULL);
}

int iot_onenet_on_event_cb(cis_evt_t eid)
{
    int ret      = 0;

    aos_check_param(g_iot_onenet);

    iot_channel_t *ch = iot_onenet_channel;

    switch (eid) {
        case CIS_EVENT_CONNECT_SUCCESS:
            /* udp always success */
            ret = -1;
            break;
        case CIS_EVENT_NOTIFY_SUCCESS:
            /* notify forece not support ack, cis_core.c:894 */
            ret = -1;
            break;

        case CIS_EVENT_REG_SUCCESS:
            //event_publish(EVENT_IOT_CONNECT_SUCCESS, (void*)ch);
            /* reg success, onenet will call obs cb & disc cb */
            
            /* Timer */
            aos_timer_new_ext(&g_timer_conn_timeout, conn_timeout_entry, NULL, 10000, 0, 0);
            aos_timer_start(&g_timer_conn_timeout);
            break;

        case CIS_EVENT_NOTIFY_FAILED:
            event_publish(EVENT_IOT_PUSH_FAILED, (void*)ch);
            break;

        case CIS_EVENT_CONNECT_FAILED:
            event_publish(EVENT_IOT_CONNECT_FAILED, (void*)ch);
            break;
        case CIS_EVENT_UPDATE_FAILED:
        case CIS_EVENT_UPDATE_TIMEOUT:
        case CIS_EVENT_REG_FAILED:
        case CIS_EVENT_REG_TIMEOUT:
        case CIS_EVENT_LIFETIME_TIMEOUT:
            event_publish(EVENT_IOT_DISCONNECTED, (void*)ch);
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}
