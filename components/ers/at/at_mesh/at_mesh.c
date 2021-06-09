/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifdef CONFIG_BT_MESH

#include <stdio.h>
#include <ctype.h>
#include "../at_internal.h"
#include <yoc/atserver.h>
#include "at_mesh.h"
#include <api/mesh.h>
#include <aos/kv.h>
#include <aos/ble.h>
#include <mesh_node.h>
#include <yoc/partition.h>
#include <errno.h>
#include <common/log.h>

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "mesh_provisioner.h"
#include "provisioner_main.h"
#include "provisioner_prov.h"
#endif

#ifdef CONFIG_BT_MESH_SIG_MODEL
#include "mesh_model/mesh_model.h"
#endif

#define TAG "at_mesh_cmd"
#define TIMEOUT 402
#define TIMEOUT1 400
#define VND_CID 0x01A8

#define AT_BACK_RET_OK_HEX(cmd ,val)  atserver_send("\r\n%s:%04x\r\nOK\r\n", cmd + 2, val)
#define AT_BACK_RET_OK_INT3(cmd,val1, val2,val3)           atserver_send("\r\n%s:%d,%d,%d\r\nOK\r\n", cmd+2, val1, val2, val3)
#define AT_BACK_TEST_RET_OK(cmd, par1)             atserver_send("\r\n%s=%s\r\nOK\r\n", cmd + 2, par1)
#define BT_MESH_ADDR_IS_UNICAST(addr) ((addr) && (addr) < 0x8000)
#define BT_MESH_ADDR_IS_GROUP(addr) ((addr) >= 0xc000 && (addr) <= 0xff00)



#define ERR_CHAR 0XFF
#define TTL_DEF 7
#define CTL_TEMP_MIN 800
#define CTL_TEMP_MAX 20000



typedef struct {
    oob_type_en oob_type;
    uint8_t oob_length;
} oob_input_info;

typedef struct {
    mesh_node_t node;
    long long time_found;
} report_node;

typedef struct {
    uint8_t front;
    uint8_t rear;
    uint8_t length;
    report_node dev[10];
} node_data_queue;

typedef struct _at_mesh {
    uint8_t init_flag: 1;
    uint8_t role: 1;
    uint8_t show_unprovison_dev: 1;
    uint8_t auto_config_flag: 1;
    uint8_t manu_rst_flag: 1;
    uint8_t get_pub_flag: 1;
    uint8_t provisioner_enabled: 1;
    k_timer_t node_report_timer;
    uint32_t node_report_timeout;
    k_timer_t auto_config_timer;
    uint16_t auto_config_addr;
    uint32_t auto_config_timeout;
    uint32_t auto_config_retry;
    model_auto_config_t auto_config;
    oob_input_info oob_info;
    node_data_queue dev_queue;
    at_mesh_cb app_cb;
} at_mesh;

static at_mesh g_mesh_server;

static inline int bt_addr_val_to_str(const uint8_t addr[6], char *str, size_t len)
{

    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

static bool is_node_data_queue_empty(node_data_queue *queue)
{
    return queue->front == queue->rear;
}

static bool is_node_data_queue_full(node_data_queue *queue)
{
    if ((queue->rear + 1) % 10 == queue->front) {
        return true;
    } else {
        return false;
    }
}

static report_node *get_node_data(node_data_queue *queue)
{
    if (is_node_data_queue_empty(queue)) {
        return NULL;
    } else {
        report_node *data = &queue->dev[queue->front];
        queue->front = (queue->front + 1) % 10;
        return data;
    }
}

static mesh_node_t *found_node(node_data_queue *queue, mesh_node_t *node)
{
    if (is_node_data_queue_empty(queue)) {
        return NULL;
    } else {

        for (uint8_t i = queue->front; i < queue->rear ; i++) {
            if (!memcmp(queue->dev[i].node.uuid, node->uuid, 16) && !memcmp(queue->dev[i].node.dev_addr, node->dev_addr, 6) &&  \
                queue->dev[i].node.addr_type == node->addr_type && queue->dev[i].node.bearer == node->bearer && queue->dev[i].node.oob_info == node->oob_info) {
                return &queue->dev[i].node;
            }
        }

        return NULL;
    }
}

static int put_node_data(node_data_queue *queue, mesh_node_t *node)
{
    if (!queue || !node) {
        return -1;
    }

    if (is_node_data_queue_full(queue)) {
        return -1;
    }

    for (int i = queue->front; i < queue->rear ; i++) {
        if (!memcmp(queue->dev[i].node.uuid, node->uuid, 16) && !memcmp(queue->dev[i].node.dev_addr, node->dev_addr, 6) &&  \
            queue->dev[i].node.addr_type == node->addr_type && queue->dev[i].node.bearer == node->bearer && queue->dev[i].node.oob_info == node->oob_info) {
            return 0;
        }
    }

    memcpy(queue->dev[queue->rear].node.uuid, node->uuid, 16);
    memcpy(queue->dev[queue->rear].node.dev_addr, node->dev_addr, 6);
    queue->dev[queue->rear].node.addr_type = node->addr_type;
    queue->dev[queue->rear].node.bearer = node->bearer;
    queue->dev[queue->rear].node.oob_info = node->oob_info;
    queue->dev[queue->rear].time_found = aos_now_ms();
    queue->rear = (queue->rear + 1) % 10;
    return 0;
}


void node_report_timeout(void *timer, void *args)
{
    if (!g_mesh_server.show_unprovison_dev) {
        return;
    }

    report_node *report_node = NULL;
    int index = 0;
    int16_t ret;
    bt_addr_le_t addr = {0};
    k_timer_start(&g_mesh_server.node_report_timer, DEF_DEV_REPORT_TIMEOUT);
    long long time_now = aos_now_ms();
    report_node = get_node_data(&g_mesh_server.dev_queue);

    if (!report_node) {
        return;
    }

    if (report_node->time_found > time_now || time_now - report_node->time_found > DEF_REPORT_DEV_SURVIVE_TIME) {
        LOGD(TAG, "drop the node survive %d ms", time_now - report_node->time_found);
        return;
    }

    if (0 != report_node->node.prim_unicast) {
        LOGD(TAG, "drop the node %04x has ben proved", report_node->node.prim_unicast);
        return;
    }

    memcpy(addr.a.val, report_node->node.dev_addr, sizeof(addr.a.val));
    addr.type = report_node->node.addr_type;
    extern int provisioner_dev_find(const bt_addr_le_t *addr, const u8_t uuid [ 16 ], int *index);
    ret = provisioner_dev_find(&addr, report_node->node.uuid, &index);

    if (!ret) {
        LOGD(TAG, "drop the node %04x has been added", report_node->node.prim_unicast);
        return;
    }

    uint8_t dev_addr_str[20];
    bt_addr_val_to_str(report_node->node.dev_addr, (char *)dev_addr_str, sizeof(dev_addr_str));
    atserver_send("\r\n+MESHDEV:%s,%02x,"
                  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x,"
                  "%02x,%02x\r\n", dev_addr_str, report_node->node.addr_type, \
                  report_node->node.uuid[0], report_node->node.uuid[1], report_node->node.uuid[2], report_node->node.uuid[3], report_node->node.uuid[4], report_node->node.uuid[5], report_node->node.uuid[6], report_node->node.uuid[7],
                  report_node->node.uuid[8], report_node->node.uuid[9], report_node->node.uuid[10], report_node->node.uuid[11], report_node->node.uuid[12], report_node->node.uuid[13], report_node->node.uuid[14], report_node->node.uuid[15],
                  report_node->node.oob_info, report_node->node.bearer);

}


void auto_config_timeout(void *timer, void *args)
{
    int ret = 0;
    k_timer_stop(&g_mesh_server.auto_config_timer);

    if (g_mesh_server.auto_config_retry) {
        ret = ble_mesh_node_model_autoconfig(0, 0, g_mesh_server.auto_config_addr, g_mesh_server.auto_config);

        if (ret) {
            LOGE(TAG, "send auto config faild");
        }

        k_timer_start(&g_mesh_server.auto_config_timer, g_mesh_server.auto_config_timeout);
        g_mesh_server.auto_config_retry--;
    }
}



void at_provisioner_cb(mesh_provisioner_event_en event, void *p_arg)
{
    switch (event) {
        // provisioner message
        case BT_MESH_EVENT_RECV_UNPROV_DEV_ADV: {
            if (g_mesh_server.show_unprovison_dev && p_arg) {
                mesh_node_t *node = (mesh_node_t *)p_arg;
                put_node_data(&g_mesh_server.dev_queue, node);
            }

        }
        break;

        case BT_MESH_EVENT_FOUND_DEV_TIMEOUT: {
            atserver_send("\r\n+MESHFOUNDDEVTIMEOUT\r\n");
        }
        break;

        case BT_MESH_EVENT_PROV_FAILD: {
            uint8_t reason = *(uint8_t *)p_arg;
            atserver_send("\r\n+MESHPROVFAILD:%x\r\n", reason);
        }
        break;

        case BT_MESH_EVENT_OOB_INPUT_NUM: {
            if (p_arg) {
                uint8_t size;
                size = *(uint8_t *)p_arg;
                atserver_send("\r\n+MESHOOBINPUT:%02x,%02x\r\n", OOB_NUM, size);
                g_mesh_server.oob_info.oob_type = OOB_NUM;
                g_mesh_server.oob_info.oob_length = size;
            }
        }
        break;

        case BT_MESH_EVENT_OOB_INPUT_STATIC_OOB: {
            atserver_send("\r\n+MESHOOBINPUT:%02x,16\r\n", OOB_STATIC);
            g_mesh_server.oob_info.oob_type = OOB_STATIC;
            g_mesh_server.oob_info.oob_length = 16;

        }
        break;

        case BT_MESH_EVENT_OOB_INPUT_STRING: {
            if (p_arg) {
                uint8_t size;
                size = *(uint8_t *)p_arg;
                atserver_send("\r\n+MESHOOBINPUT:%02x,%02x\r\n", OOB_STR, size);
                g_mesh_server.oob_info.oob_type = OOB_STR;
                g_mesh_server.oob_info.oob_length = size;
            }
        }
        break;

        case BT_MESH_EVENT_PROV_COMP: {
            if (p_arg) {
                mesh_node_t *node = (mesh_node_t *)p_arg;

                atserver_send("\r\n+MESHNODEADD:%04x,%02x,%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n" \
                              , node->prim_unicast, node->element_num, \
                              node->uuid[0], node->uuid[1], node->uuid[2], node->uuid[3], node->uuid[4], node->uuid[5], node->uuid[6], node->uuid[7], \
                              node->uuid[8], node->uuid[9], node->uuid[10], node->uuid[11], node->uuid[12], node->uuid[13], node->uuid[14], node->uuid[15]);

                mesh_node_t *prov_node = NULL;
                prov_node = found_node(&g_mesh_server.dev_queue, node);

                if (prov_node) {
                    prov_node->prim_unicast = node->prim_unicast;
                }
            }
        }
        break;

        default:
            break;
    }

    if (g_mesh_server.app_cb) {
        g_mesh_server.app_cb(event, p_arg);
    }

}



static int print_comp_data(uint16_t addr, struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(comp, 50);

    uint16_t copy_length;


    copy_length = net_buf_simple_tailroom(&comp) < buf->len ? net_buf_simple_tailroom(&comp) :  buf->len;
    net_buf_simple_add_mem(&comp, buf->data, copy_length);

    //addr CID PID VID CRPL Features
    atserver_send("\r\n+MESHCOMPDATA:%04x,%04x,%04x,%04x,%04x,%04x", addr, net_buf_simple_pull_le16(&comp), net_buf_simple_pull_le16(&comp), \
                  net_buf_simple_pull_le16(&comp), net_buf_simple_pull_le16(&comp), net_buf_simple_pull_le16(&comp));

    while (comp.len > 4) {
        uint8_t sig_model_num;
        uint8_t vnd_model_num;
        uint16_t elem_idx;
        int i;
        elem_idx = net_buf_simple_pull_le16(&comp);
        sig_model_num = net_buf_simple_pull_u8(&comp);
        vnd_model_num = net_buf_simple_pull_u8(&comp);

        atserver_send(",%04x,%02x,%02x", elem_idx, sig_model_num, vnd_model_num);

        if (comp.len < ((sig_model_num * 2) + (vnd_model_num * 4))) {
            atserver_send("\r\n");
            break;
        }

        for (i = 0; i < sig_model_num; i++) {
            u16_t mod_id = net_buf_simple_pull_le16(&comp);
            atserver_send(",%04x", mod_id);
        }


        for (i = 0; i < vnd_model_num; i++) {
            u16_t cid = net_buf_simple_pull_le16(&comp);
            u16_t mod_id = net_buf_simple_pull_le16(&comp);
            atserver_send(",%04x,%04x", cid, mod_id);
        }

        atserver_send("\r\n");

    }

    return 0;
}


void at_model_cb(mesh_model_event_en event, void *p_arg)
{
    int16_t ret;

    switch (event) {
        case BT_MESH_MODEL_CFG_APPKEY_STATUS: {
            if (p_arg) {
                model_message message = *(model_message *)p_arg;
                uint8_t status = message.status_data->data[0];

                if (status) {
                    atserver_send("\r\n+MESHAPPKEYADD:%04x,%x\r\n", message.source_addr, status);
                } else {
                    atserver_send("\r\n+MESHAPPKEYADD:%04x,%x\r\n", message.source_addr, AT_STATUS_OK);
                }
            }
        }
        break;

        case BT_MESH_MODEL_CFG_APPKEY_BIND_STATUS: {
            if (p_arg) {
                model_message message = *(model_message *)p_arg;
                uint8_t status = message.status_data->data[0];

                if (status) {
                    atserver_send("\r\n+MESHAPPKEYBIND:%04x,%x\r\n", message.source_addr, status);
                } else {
                    atserver_send("\r\n+MESHAPPKEYBIND:%04x,%x\r\n", message.source_addr, AT_STATUS_OK);
                }
            }
        }
        break;

        case BT_MESH_MODEL_CFG_COMP_DATA_STATUS: {
            if (p_arg) {
                model_message message = *(model_message *)p_arg;

                struct net_buf_simple *buf = message.status_data;

                if (buf) {
                    uint8_t status = net_buf_simple_pull_u8(buf);

                    if (!status) {
                        ret = print_comp_data(message.source_addr, buf);
                    }

                    if (ret || status) {
                        atserver_send("\r\n+MESHCOMPDATA:%04x,%x\r\nERROR\r\n", message.source_addr, status);
                    }

                }
            }

        }
        break;

        case BT_MESH_MODEL_CFG_SUB_STATUS: {
            if (p_arg) {
                model_message message = *(model_message *)p_arg;
                uint8_t status = message.status_data->data[0];

                if (status) {
                    atserver_send("\r\n+MESHSUBSET:%04x,%x\r\n", message.source_addr, status);
                } else {
                    atserver_send("\r\n+MESHSUBSET:%04x,%x\r\n", message.source_addr, AT_STATUS_OK);
                }
            }

        }
        break;

        case BT_MESH_MODEL_CFG_SUB_LIST: {
            if (p_arg) {
                model_message message = *(model_message *)p_arg;
                uint8_t status_data = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint16_t addr = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                uint16_t mod_id = (uint16_t)net_buf_simple_pull_le16(message.status_data);

                if (!status_data && message.status_data->len >= 2) {
                    uint8_t first_addr = 0;

                    while (message.status_data->len >= 2) {
                        if (!first_addr) {
                            atserver_send("\r\n+MESHSUBLIST:%04x,%04x,%04x,%04x", addr, CID_NVAL, mod_id, (uint16_t)net_buf_simple_pull_le16(message.status_data));
                            first_addr = 1;
                        } else {
                            atserver_send(",%04x", (uint16_t)net_buf_simple_pull_le16(message.status_data));
                        }
                    }

                    if (first_addr) {
                        atserver_send("\r\n");
                    }

                } else if (status_data) {
                    atserver_send("\r\n+MESHSUBLIST:%04x,%x\r\n", addr, status_data);
                } else if (!status_data && message.status_data->len < 2) {
                    atserver_send("\r\n+MESHSUBLIST:%04x,%04X,%04x,NULL\r\n", addr, CID_NVAL, mod_id);
                }

            }
        }
        break;

        case BT_MESH_MODEL_CFG_SUB_LIST_VND: {
            if (p_arg) {
                model_message message = *(model_message *)p_arg;
                uint8_t status_data = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint16_t addr = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                uint16_t company_id = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                uint16_t mod_id = (uint16_t)net_buf_simple_pull_le16(message.status_data);

                if (!status_data && message.status_data->len >= 2) {
                    uint8_t first_addr = 0;

                    while (message.status_data->len >= 2) {
                        if (!first_addr) {
                            atserver_send("\r\n+MESHSUBLIST:%04x,%04x,%04x,%04x", addr, company_id, mod_id, (uint16_t)net_buf_simple_pull_le16(message.status_data));
                            first_addr = 1;
                        } else {
                            atserver_send(",%04x", (uint16_t)net_buf_simple_pull_le16(message.status_data));
                        }
                    }

                    if (first_addr) {
                        atserver_send("\r\n");
                    }

                } else if (status_data) {
                    atserver_send("\r\n+MESHSUBLIST:%d\r\n", status_data);
                } else if (!status_data && message.status_data->len < 2) {
                    atserver_send("\r\n+MESHSUBLIST:%04x,%04x,%04x,NULL\r\n", addr, company_id, mod_id);
                }

            }
        }
        break;

        case BT_MESH_MODEL_CFG_PUB_STATUS: {

            if (p_arg) {
                model_message message = *(model_message *)p_arg;
                uint8_t status = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint16_t elem_addr = (uint16_t)net_buf_simple_pull_le16(message.status_data);

                if (g_mesh_server.get_pub_flag) {
                    if (status) {
                        atserver_send("\r\n+MESHPUB:%04x,%x\r\n", message.source_addr, status);
                        return;
                    }

                    if (message.status_data->len < 1) {
                        atserver_send("\r\n+MESHPUB:%04x,NULL\r\n", elem_addr);
                        return;
                    }

                    uint16_t pub_addr = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                    net_buf_simple_pull_le16(message.status_data);
                    uint8_t ttl = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                    uint8_t period = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                    uint8_t retransmit = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                    uint16_t CID;
                    uint16_t mod_id;

                    if (message.status_data->len >= 4) {
                        CID = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                        mod_id = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                    } else {
                        CID = CID_NVAL;
                        mod_id = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                    }

                    if (BT_MESH_ADDR_UNASSIGNED == pub_addr) {
                        atserver_send("\r\n+MESHPUB:%04x,%04x,%04x,NULL\r\n", elem_addr, CID, mod_id);
                    } else {
                        atserver_send("\r\n+MESHPUB:%04x,%04x,%04x,%04x,%02x,%02x,%02x,%02x\r\n", elem_addr, CID, mod_id, pub_addr, \
                                      ttl, period, BT_MESH_PUB_TRANSMIT_COUNT(retransmit), \
                                      BT_MESH_PUB_TRANSMIT_INT(retransmit));
                    }

                    g_mesh_server.get_pub_flag = 0;
                } else if (status) {
                    atserver_send("\r\n+MESHPUBSET:%04x,%x\r\n", message.source_addr, status);
                } else {
                    atserver_send("\r\n+MESHPUBSET:%04x,%x\r\n", message.source_addr, AT_STATUS_OK);
                }

            }


        }
        break;

        case BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG_STATUS: {
            if (p_arg) {
                model_message *message = (model_message *)p_arg;
                uint8_t *ven_data = (uint8_t *)message->ven_data.user_data;
                uint8_t status = ven_data[1];

                if (0 == status) {
                    atserver_send("\r\n+MESHAUTOCONFIG:%04x,%d\r\n", message->source_addr, AT_STATUS_OK);

                    if (BT_MESH_ADDR_IS_UNICAST(g_mesh_server.auto_config_addr)) {
                        if (g_mesh_server.auto_config_addr == message->source_addr) {
                            k_timer_stop(&g_mesh_server.auto_config_timer);
                        }
                    }
                } else {
                    atserver_send("\r\n+MESHAUTOCONFIG:%04x,%x\r\n", message->source_addr, status);

                }

            }

        }
        break;

        //model status
        case BT_MESH_MODEL_CFG_RELAY_STATUS: {
            if (p_arg) {
                model_message relay_message = *(model_message *)p_arg;
                atserver_send("\r\n+MESHRELAY:%04x,%02x\r\n", relay_message.source_addr, relay_message.status_data->data[0]);
            }
        }
        break;

        case BT_MESH_MODEL_CFG_PROXY_STATUS: {
            if (p_arg) {
                model_message proxy_message = *(model_message *)p_arg;
                atserver_send("\r\n+MESHPROXY:%04x,%02x\r\n", proxy_message.source_addr, proxy_message.status_data->data[0]);
            }
        }
        break;

        case BT_MESH_MODEL_CFG_FRIEND_STATUS: {
            if (p_arg) {
                model_message friend_message = *(model_message *)p_arg;
                atserver_send("\r\n+MESHFRIEND:%04x,%02x\r\n", friend_message.source_addr, friend_message.status_data->data[0]);
            }
        }
        break;

        case BT_MESH_MODEL_CFG_RST_STATUS: {
            if (p_arg) {
                static uint16_t rst_last = 0;
                model_message rst_message = *(model_message *)p_arg;

                if (rst_last == rst_message.source_addr && g_mesh_server.manu_rst_flag) {
                    rst_last = 0;
                    g_mesh_server.manu_rst_flag = 0;
                    return;
                } else {
                    atserver_send("\r\n+MESHRST:%04x\r\n", rst_message.source_addr);
                    rst_last = rst_message.source_addr;
                }

            }
        }
        break;

        case BT_MESH_MODEL_ONOFF_STATUS: {
            if (p_arg) {
                model_message onoff_message = *(model_message *)p_arg;
                atserver_send("\r\n+MESHONOFF:%04x,%02x\r\n", onoff_message.source_addr, onoff_message.status_data->data[0]);
            }
        }
        break;

        case BT_MESH_MODEL_LEVEL_STATUS: {
            if (p_arg) {
                model_message level_message = *(model_message *)p_arg;
                int16_t level = (int16_t)net_buf_simple_pull_le16(level_message.status_data);
                atserver_send("\r\n+MESHLEVEL:%04x,%04x\r\n", level_message.source_addr, level);
            }
        }
        break;

        case BT_MESH_MODEL_LIGHTNESS_STATUS: {
            if (p_arg) {
                model_message lightness_message = *(model_message *)p_arg;
                int16_t lightness = (int16_t)net_buf_simple_pull_le16(lightness_message.status_data);
                atserver_send("\r\n+MESHLIGHTNESS:%04x,%04x\r\n", lightness_message.source_addr, lightness);
            }
        }
        break;

        case BT_MESH_MODEL_LIGHTNESS_LINEAR_STATUS: {
            if (p_arg) {
                model_message lightness_lin_message = *(model_message *)p_arg;
                int16_t lightness_lin = (int16_t)net_buf_simple_pull_le16(lightness_lin_message.status_data);
                atserver_send("\r\n+MESHLIGHTNESSLIN:%04x,%04x\r\n", lightness_lin_message.source_addr, lightness_lin);
            }
        }
        break;

        case BT_MESH_MODEL_LIGHTNESS_RANGE_STATUS: {
            if (p_arg) {
                model_message lightness_range_message = *(model_message *)p_arg;
                uint8_t status_code = (uint8_t)net_buf_simple_pull_u8(lightness_range_message.status_data);
                int16_t range_min = (int16_t)net_buf_simple_pull_le16(lightness_range_message.status_data);
                int16_t range_max = (int16_t)net_buf_simple_pull_le16(lightness_range_message.status_data);
                atserver_send("\r\n+MESHLIGHTNESSRANGE:%04x,%02x,%04x,%04x\r\n", lightness_range_message.source_addr, status_code, range_min, range_max);
            }

        }
        break;

        case BT_MESH_MODEL_LIGHTNESS_DEF_STATUS: {
            if (p_arg) {
                model_message lightness_def_message = *(model_message *)p_arg;
                int16_t lightness_def = (int16_t)net_buf_simple_pull_le16(lightness_def_message.status_data);
                atserver_send("\r\n+MESHLIGHTNESSDEF:%04x,%04x\r\n", lightness_def_message.source_addr, lightness_def);
            }
        }
        break;

        case BT_MESH_MODEL_LIGHTNESS_LAST_STATUS: {
            if (p_arg) {
                model_message lightness_last_message = *(model_message *)p_arg;
                int16_t lightness_last = (int16_t)net_buf_simple_pull_le16(lightness_last_message.status_data);
                atserver_send("\r\n+MESHLIGHTNESSLAST:%04x,%04x\r\n", lightness_last_message.source_addr, lightness_last);
            }
        }
        break;

        case BT_MESH_MODEL_LIGHT_CTL_STATUS: {
            if (p_arg) {
                model_message light_ctl_message = *(model_message *)p_arg;
                int16_t lightness = (int16_t)net_buf_simple_pull_le16(light_ctl_message.status_data);
                int16_t ctl_temp  = (int16_t)net_buf_simple_pull_le16(light_ctl_message.status_data);
                atserver_send("\r\n+MESHCTL:%04x,%04x,%04x\r\n", light_ctl_message.source_addr, lightness, ctl_temp);
            }
        }
        break;

        case BT_MESH_MODEL_LIGHT_CTL_TEMP_STATUS: {
            if (p_arg) {
                model_message light_ctl_temp_message = *(model_message *)p_arg;
                int16_t ctl_temp  = (int16_t)net_buf_simple_pull_le16(light_ctl_temp_message.status_data);
                int16_t uv  = (int16_t)net_buf_simple_pull_le16(light_ctl_temp_message.status_data);
                atserver_send("\r\n+MESHCTLTEMP:%04x,%04x,%04x\r\n", light_ctl_temp_message.source_addr, ctl_temp, uv);
            }
        }
        break;

        case BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_STATUS: {
            if (p_arg) {
                model_message light_ctl_temp_ran_message = *(model_message *)p_arg;
                uint8_t status_code = (uint8_t)net_buf_simple_pull_u8(light_ctl_temp_ran_message.status_data);
                int16_t temp_min  = (int16_t)net_buf_simple_pull_le16(light_ctl_temp_ran_message.status_data);
                int16_t temp_max  = (int16_t)net_buf_simple_pull_le16(light_ctl_temp_ran_message.status_data);
                atserver_send("\r\n+MESHCTLTEMPRANGE:%04x,%02x,%04x,%04x\r\n", light_ctl_temp_ran_message.source_addr, status_code, temp_min, temp_max);
            }
        }
        break;

        case BT_MESH_MODEL_LIGHT_CTL_DEF_STATUS: {
            if (p_arg) {
                model_message light_ctl_def_message = *(model_message *)p_arg;

                int16_t lightness = (int16_t)net_buf_simple_pull_le16(light_ctl_def_message.status_data);
                int16_t ctl_temp  = (int16_t)net_buf_simple_pull_le16(light_ctl_def_message.status_data);
                int16_t uv  = (int16_t)net_buf_simple_pull_le16(light_ctl_def_message.status_data);
                atserver_send("\r\n+MESHCTLDEF:%04x,%04x,%04x,%04x\r\n", light_ctl_def_message.source_addr, lightness, ctl_temp, uv);
            }
        }
        break;

        case BT_MESH_MODEL_VENDOR_MESSAGES: {
            model_message message = *(model_message *)p_arg;
            uint16_t unicast_addr = message.source_addr;
            uint8_t len = message.ven_data.data_len;
            char *data = (char *)message.ven_data.user_data;
            atserver_send("\r\n+MESHTRS:%04x,%02x,%s\r\n", unicast_addr, len, bt_hex_real(data, len));
        }
        break;

        default:
            break;
    }

    if (g_mesh_server.app_cb) {
        g_mesh_server.app_cb(event, p_arg);
    }

}

provisioner_node g_provisioner_param = {
    .config.unicast_addr_local = DEF_PROVISIONER_UNICAST_ADDR_LOCAL,
    .config.unicast_addr_start = DEF_PROVISIONER_UNICAST_ADDR_START,
    .config.attention_time = DEF_ATTENTION_TIMEOUT,
    .config.cb = at_provisioner_cb,
    .local_sub = AUTOCONFIG_LOCAL_SUB_ADDR,
    .local_pub = AUTOCONFIG_LOCAL_PUB_ADDR,

};

node_config_t g_node_param = {
    .role = PROVISIONER,
    .provisioner_config = &g_provisioner_param,
    .dev_uuid = DEV_UUID,
    .dev_name = DEV_NAME,
    .user_model_cb = at_model_cb,
    .user_prov_cb = NULL,
};

int at_mesh_init(at_mesh_cb app_cb)
{

    int16_t ret;

    extern int at_mesh_composition_init();
    ret = at_mesh_composition_init();

    if (ret) {
        return AT_ERR_MESH_INIT_COMP_FAILD;
    }

    ret = ble_mesh_node_init(&g_node_param);

    if (ret < 0) {
        return AT_ERR_MESH_INIT_NODE_FAILD;
    }

    if (!g_mesh_server.provisioner_enabled) {
        k_timer_init(&g_mesh_server.node_report_timer, node_report_timeout, NULL);
        k_timer_init(&g_mesh_server.auto_config_timer, auto_config_timeout, NULL);
    }

    g_mesh_server.provisioner_enabled = 1;
    g_mesh_server.dev_queue.front = 0;
    g_mesh_server.dev_queue.rear = 0;
    g_mesh_server.dev_queue.length = 10;
    g_mesh_server.role = g_node_param.role;
    g_mesh_server.app_cb = app_cb;
    g_mesh_server.init_flag = 1;
    g_mesh_server.oob_info.oob_type = OOB_NULL;
    return 0;

}

//BASE_CMD

void at_cmd_btmesh_reboot(char *cmd, int type, char *data)
{

    if (type == EXECUTE_CMD) {
        AT_BACK_OK();
        aos_reboot();
    }
}

void at_cmd_btmesh_query_sta(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t mod_id;
        uint32_t netkey_idx;
        uint32_t appkey_idx;
        uint32_t status_op;
        uint32_t cid;
        int16_t ret;
        int16_t input_num;

        input_num = atserver_scanf("%x,%x,%x,%x,%x,%x", &unicast_addr, &mod_id, &status_op, &netkey_idx, &appkey_idx, &cid);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num < 4) {
            netkey_idx = 0;
        }

        if (input_num < 5) {
            appkey_idx = 0;

            if (BT_MESH_MODEL_ID_CFG_CLI == mod_id) {
                appkey_idx = BT_MESH_KEY_DEV;
            }
        }

        if (input_num < 6) {
            cid = CID_NVAL;
        }

        if (CID_NVAL == cid && BT_MESH_MODEL_ID_CFG_CLI == mod_id && BT_MESH_KEY_DEV != appkey_idx) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *model = ble_mesh_model_find(0, mod_id, cid);

        if (!model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_NOT_FOUND_MODEL));
            return;
        }

        ret = ble_mesh_model_status_get(netkey_idx, appkey_idx, unicast_addr, model, status_op);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {

        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>,<query_mod_id>,<query_opcode>,[netkey_idx],[appkey_idx],[cid]");

    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}

//for provisioner
void at_cmd_btmesh_prov_en(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (type == WRITE_CMD) {
        int16_t ret;
        int16_t input_num;
        uint32_t onoff = 0;
        input_num = atserver_scanf("%x", &onoff);

        if (input_num < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (onoff) {
            ret = ble_mesh_provisioner_enable();

            if (ret && ret != -EALREADY) {
                AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
                return;
            }

            if (!g_mesh_server.provisioner_enabled) {
                k_timer_init(&g_mesh_server.node_report_timer, node_report_timeout, NULL);
                k_timer_init(&g_mesh_server.auto_config_timer, auto_config_timeout, NULL);
            }

            g_mesh_server.provisioner_enabled = 1;
            g_mesh_server.dev_queue.front = 0;
            g_mesh_server.dev_queue.rear = 0;
            g_mesh_server.dev_queue.length = 10;

        } else {
            ret = ble_mesh_provisioner_disable();

            if (ret && ret != -EALREADY) {
                AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
                return;
            }

            if (g_mesh_server.provisioner_enabled) {
                k_timer_stop(&g_mesh_server.node_report_timer);
                k_timer_stop(&g_mesh_server.auto_config_timer);
            }

            g_mesh_server.provisioner_enabled = 0;
        }

        AT_BACK_OK();

    } else if (type == READ_CMD) {
        if (g_mesh_server.role == NODE) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
        } else {
            AT_BACK_RET_OK_INT(cmd, g_mesh_server.provisioner_enabled);
        }
    } else if (type == TEST_CMD) {
        if (g_mesh_server.role == NODE) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
        } else {
            AT_BACK_TEST_RET_OK(cmd, "<onoff>");
        }
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}

void at_cmd_btmesh_rst_dev(char *cmd, int type, char *data)
{
    int16_t ret = 0;

    if (type == EXECUTE_CMD) {
        aos_kv_reset();
        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_RST_FAILD));
            return;
        } else {
            AT_BACK_OK();
            aos_reboot();
        }
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_rpl_clear(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == EXECUTE_CMD) {
        extern void bt_mesh_rpl_clear_all();
        bt_mesh_rpl_clear_all();
        AT_BACK_OK();
    } else if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        int16_t input_num;
        mesh_node_t *node = NULL;
        input_num = atserver_scanf("%x", &unicast_addr);

        if (input_num < 1 || unicast_addr < DEF_PROVISIONER_UNICAST_ADDR_START) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        extern mesh_node_t *ble_mesh_provisioner_get_node_info(u16_t unicast_addr);
        node = ble_mesh_provisioner_get_node_info(unicast_addr);

        if (!node) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_GET_NODE_INFO_FAILD));
            return;
        }
        extern void bt_mesh_rpl_clear_node(uint16_t unicast_addr, uint8_t elem_num);
        bt_mesh_rpl_clear_node(node->prim_unicast, node->element_num);
        AT_BACK_OK();

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "[unicast_addr]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}

void at_cmd_btmesh_prov_show_dev(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        int16_t input_num;
        uint32_t show_dev;
        uint32_t timeout;
        int16_t ret;
        input_num = atserver_scanf("%x,%x", &show_dev, &timeout);

        if (input_num < 1 || show_dev > 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num < 2) {
            timeout = DEF_FOUND_DEV_TIMEOUT;
        }

        ret = ble_mesh_provisioner_show_dev(show_dev, timeout);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_SHOW_DEV_FAILD));
            return;
        }

        if (show_dev) {
            k_timer_start(&g_mesh_server.node_report_timer, DEF_DEV_REPORT_TIMEOUT);
        } else {
            k_timer_stop(&g_mesh_server.node_report_timer);
        }

        g_mesh_server.dev_queue.front = 0;
        g_mesh_server.dev_queue.rear = 0;
        g_mesh_server.show_unprovison_dev = show_dev;

        AT_BACK_OK();

    } else if (type == READ_CMD) {
        if (g_mesh_server.role == NODE) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
        } else {
            AT_BACK_RET_OK_INT(cmd, g_mesh_server.show_unprovison_dev);
        }
    } else if (type == TEST_CMD) {
        if (g_mesh_server.role == NODE) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
        } else {
            AT_BACK_TEST_RET_OK(cmd, "<onoff>,[timeout]");
        }
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}

static uint8_t char2u8(char c)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    } else {
        return ERR_CHAR;
    }
}

static int str2bt_addr(const char *str, uint8_t addr[6])
{
    uint8_t i, j;
    uint8_t tmp;

    if (strlen(str) != 17 || !addr) {
        return -EINVAL;
    }

    for (i = 5, j = 1; *str != '\0' ; str++, j++) {
        if (!(j % 3) && (*str != ':')) {
            return -EINVAL;
        } else if (*str == ':') {
            i--;
            continue;
        }

        addr[i] = addr[i] << 4;

        tmp = char2u8(*str);

        if (tmp == ERR_CHAR) {
            return -EINVAL;
        }

        addr[i] |= tmp;
    }

    return 0;
}

static int str2hex(uint8_t hex[], char *s, uint8_t cnt)
{
    uint8_t i;

    if (!s) {
        return 0;
    }

    for (i = 0; (*s != '\0') && (i < cnt); i++, s += 2) {
        hex[i] = ((char2u8(*s) & 0x0f) << 4) | ((char2u8(*(s + 1))) & 0x0f);
    }

    return i;
}

void at_cmd_btmesh_prov_add_dev(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        int16_t ret;
        uint8_t dev_addr_str[20] = {0};
        uint8_t uuid_str[33] = {0};
        mesh_node_t node = {0};
        uint32_t addr_type;
        uint32_t oob_info;
        uint32_t bearer;
        uint32_t auto_add_appkey;
        int16_t input_num;

        input_num = atserver_scanf("%[^,],%x,%[^,],%x,%x,%x", dev_addr_str, &addr_type, uuid_str, &oob_info, &bearer, &auto_add_appkey);

        if (input_num < 5) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        node.bearer = bearer;
        node.oob_info = oob_info;
        node.addr_type = addr_type;


        if (input_num < 6) {
            auto_add_appkey = DEF_AUTO_ADD_APPKEY_ONOFF;
        }


        if (node.addr_type != DEV_ADDR_LE_RANDOM && node.addr_type != DEV_ADDR_LE_PUBLIC) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (node.bearer != BT_MESH_PROV_ADV && node.bearer != BT_MESH_PROV_GATT) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = str2bt_addr((char *)dev_addr_str, node.dev_addr);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (str2hex(node.uuid, (char *)uuid_str, sizeof(uuid_str)) < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = ble_mesh_provisioner_dev_add(&node, auto_add_appkey);

        if (ret < 0) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_ADD_DEV_FAILD));
        } else {
            AT_BACK_OK();
        }

        g_mesh_server.oob_info.oob_type = OOB_NULL;

    } else if (type == TEST_CMD) {
        if (g_mesh_server.role == NODE) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
        } else {
            AT_BACK_TEST_RET_OK(cmd, "<addr>,<addr_type>,<uuid>,<oob_info>,<bearer>,[auto_add_appkey]");
        }

    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_prov_node_auto_config(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        int16_t ret;
        uint32_t  addr;
        uint32_t sub_addr;
        uint32_t timeout;
        uint32_t retry;
        int16_t input_num;

        input_num = atserver_scanf("%x,%x,%x,%x", &addr, &sub_addr, &timeout, &retry);

        if (input_num < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num < 2) {
            sub_addr = AUTOCONFIG_DST_SUB_ADDR;
        }

        if (input_num < 3) {
            timeout  = DEF_AUTO_CONFIG_TIMEOUT;
        }

        if (input_num < 4) {
            if (BT_MESH_ADDR_IS_UNICAST(addr)) {
                retry = DEF_UNICAST_ADDR_AUTO_CONFIG_RETRY;
            } else {
                retry = DEF_GROUP_ADDR_AUTO_CONFIG_RETRY;
            }
        }

        if (timeout < DEF_AUTO_CONFIG_TIMEOUT_MIN) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (!BT_MESH_ADDR_IS_GROUP(sub_addr)) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        g_mesh_server.auto_config_addr = addr;
        g_mesh_server.auto_config_timeout = timeout * 1000;
        g_mesh_server.auto_config_retry = retry;
        g_mesh_server.auto_config.sub_addr = sub_addr;
        ret = ble_mesh_node_model_autoconfig(0, 0, addr, g_mesh_server.auto_config);

        if (ret < 0) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
            k_timer_start(&g_mesh_server.auto_config_timer, g_mesh_server.auto_config_timeout);
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,[sub_addr],[timeout],[retry]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_prov_del_dev(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        int16_t ret;
        uint8_t dev_addr_str[20] = {0};
        uint8_t uuid_str[33] = {0};
        uint8_t dev_addr[6];
        uint8_t uuid[16];
        uint32_t addr_type;
        int16_t input_num;

        input_num = atserver_scanf("%[^,],%x,%[^,]", dev_addr_str, &addr_type, uuid_str);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (addr_type != DEV_ADDR_LE_RANDOM && addr_type != DEV_ADDR_LE_PUBLIC) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = str2bt_addr((char *)dev_addr_str, dev_addr);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (str2hex(uuid, (char *)uuid_str, sizeof(uuid_str)) < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = ble_mesh_provisioner_dev_del(dev_addr, addr_type, uuid);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_DEL_DEV_FAILD));
        } else {
            AT_BACK_OK();
        }


    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<addr_type>,<uuid>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_prov_add_oob(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {

        uint8_t oob_data[17] = {0};
        uint32_t oob_num;
        int16_t input_num;
        int16_t ret;

        if (OOB_NUM == g_mesh_server.oob_info.oob_type) {
            input_num = atserver_scanf("%d", &oob_num);

            if (input_num < 1) {
                AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
                return;
            }

            ret = ble_mesh_provisioner_OOB_input_num(oob_num);

        } else if (OOB_STR == g_mesh_server.oob_info.oob_type) {
            input_num = atserver_scanf("%[^]", oob_data);

            if (input_num < 1 || g_mesh_server.oob_info.oob_length != strlen((char *)oob_data)) {
                AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
                return;
            }

            ret = ble_mesh_provisioner_OOB_input_string((char *)oob_data);
        } else if (OOB_STATIC == g_mesh_server.oob_info.oob_type) {
            input_num = atserver_scanf("%[^]", oob_data);

            if (input_num < 1 ||  g_mesh_server.oob_info.oob_length != strlen((char *)oob_data)) {
                AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
                return;
            } else {
                ret = ble_mesh_provisioner_static_OOB_set(oob_data, strlen((char *)oob_data));
            }

        } else {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_OOB_NOT_NEED));
            return;
        }

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_INPUT_OOB_FAILD));
            return;
        }

        AT_BACK_OK();

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<oob>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_prov_netkey_set(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }


    if (type == WRITE_CMD) {
        uint32_t netkey_idx;
        uint8_t net_key_str[33];
        uint8_t net_key[16];
        int16_t input_num;
        int16_t ret;
        input_num = atserver_scanf("%x,%[^]", &netkey_idx, net_key_str);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (str2hex(net_key, (char *)net_key_str, sizeof(net_key_str)) < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = bt_mesh_provisioner_local_net_key_add(net_key, (uint16_t *)&netkey_idx);

        if (-EEXIST == ret) {
            ret = bt_mesh_provisioner_local_net_key_delete(netkey_idx);

            if (ret) {
                AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_ADD_NETKEY_FAILD));
                return;
            } else {
                ret = bt_mesh_provisioner_local_net_key_add(net_key, (uint16_t *)&netkey_idx);
            }
        }

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_ADD_NETKEY_FAILD));
            return;
        } else {
            AT_BACK_RET_OK_INT(cmd, netkey_idx);
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<netkey_idx>,<key_val>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_prov_netkey_get(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }


    if (type == WRITE_CMD) {
        uint32_t netkey_idx;
        const uint8_t *key_val;
        int16_t input_num;
        input_num = atserver_scanf("%x", &netkey_idx);

        if (input_num < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        key_val = bt_mesh_provisioner_local_net_key_get(netkey_idx);

        if (!key_val) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_GET_NETKEY_FAILD));
            return;
        }

        atserver_send("\r\n+MESHGETNETKEY:%02x,%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", netkey_idx, key_val[0], key_val[1], key_val[2], \
                      key_val[3], key_val[4], key_val[5], key_val[6], key_val[7], key_val[8], \
                      key_val[9], key_val[10], key_val[11], key_val[12], key_val[13], key_val[14], key_val[15]);
        AT_BACK_OK();

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<netkey_idx>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_prov_appkey_set(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }


    if (type == WRITE_CMD) {
        uint32_t appkey_idx = 0;
        uint8_t app_key_str[33];
        uint8_t app_key[16];
        int16_t input_num = 0;
        int16_t ret  = 0;

        input_num = atserver_scanf("%x,%[^]", &appkey_idx, app_key_str);

        if (input_num < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (1 == input_num) {
            do {
                ret = bt_mesh_provisioner_local_app_key_add(NULL, 0, (uint16_t *)&appkey_idx);
            } while (-EEXIST == ret && appkey_idx++ < CONFIG_BT_MESH_PROVISIONER_APP_KEY_COUNT);
        } else {
            if (str2hex(app_key, (char *)app_key_str, sizeof(app_key_str)) < 1) {
                AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
                return;
            }

            ret = bt_mesh_provisioner_local_app_key_add(app_key, 0, (uint16_t *)&appkey_idx);

            if (-EEXIST == ret) {
                ret = bt_mesh_provisioner_local_app_key_delete(0, appkey_idx);

                if (ret) {
                    AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_ADD_APPKEY_FAILD));
                    return;
                } else {
                    ret = bt_mesh_provisioner_local_app_key_add(app_key, 0, (uint16_t *)&appkey_idx);
                }
            }
        }

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_ADD_APPKEY_FAILD));
        } else {
            AT_BACK_RET_OK_INT(cmd, appkey_idx);
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<appkey_idx>,[key_val]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_prov_appkey_get(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }


    if (type == WRITE_CMD) {
        uint32_t netkey_idx;
        uint32_t appkey_idx;
        const uint8_t *key_val;
        int16_t input_num;
        input_num = atserver_scanf("%x,%x", &netkey_idx, &appkey_idx);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        key_val = bt_mesh_provisioner_local_app_key_get(netkey_idx, appkey_idx);

        if (!key_val) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_GET_APPKEY_FAILD));
            return;
        }

        atserver_send("\r\n+MESHGETAPPKEY:%02x,%02x,%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", netkey_idx, appkey_idx, key_val[0], key_val[1], key_val[2], \
                      key_val[3], key_val[4], key_val[5], key_val[6], key_val[7], key_val[8], \
                      key_val[9], key_val[10], key_val[11], key_val[12], key_val[13], key_val[14], key_val[15]);

        AT_BACK_OK();
    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<netkey_idx>,<appkey_idx>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_prov_set_uuid_filter(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        char uuid_str[33];
        uint8_t uuid_hex[16];
        uint32_t onoff;
        uint32_t filter_start;
        uuid_filter_t filter;
        int16_t input_num;
        int16_t ret;
        input_num = atserver_scanf("%x,%[^,],%x", &onoff, uuid_str, &filter_start);

        if (input_num < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (!onoff) {
            ret = ble_mesh_provisioner_dev_filter(onoff, NULL);
        } else {
            if (3 != input_num) {
                AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
                return;
            }

            uint8_t str_len = strlen((char *)uuid_str);

            if (str_len < 1 || str_len > 32) {
                AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
                return;
            }

            str2hex(uuid_hex, (char *)uuid_str, 16);

            filter.uuid = uuid_hex;
            filter.uuid_length = (str_len + 1) / 2;
            filter.filter_start = filter_start;
            ret = ble_mesh_provisioner_dev_filter(onoff, &filter);
        }

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_SET_UUID_FILTER_FAILD));
            return;
        }

        AT_BACK_OK();

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<onoff>,[uuid_str],[filter_start]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_prov_get_node_info(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        int16_t input_num;
        mesh_node_t *node;
        uint8_t dev_addr_str[20];

        input_num = atserver_scanf("%x", &unicast_addr);

        if (input_num < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (unicast_addr > 0x8000) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        node = ble_mesh_provisioner_get_node_info(unicast_addr);

        if (!node) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_GET_NODE_INFO_FAILD));
            return;
        } else {
            bt_addr_val_to_str(node->dev_addr, (char *)dev_addr_str, sizeof(dev_addr_str));

            if (node->node_name && strlen((char *)node->node_name)) {
                atserver_send("\r\n+MESHNODEINFO:%04x,%s", unicast_addr, node->node_name);
            } else {
                atserver_send("\r\n+MESHNODEINFO:%04x,NULL", unicast_addr);
            }

            atserver_send(",%s,%02x", dev_addr_str, node->addr_type);
            atserver_send(",%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", node->uuid[0], node->uuid[1], node->uuid[2], node->uuid[3], \
                          node->uuid[4], node->uuid[5], node->uuid[6], node->uuid[7], node->uuid[8], node->uuid[9], node->uuid[10], \
                          node->uuid[11], node->uuid[12], node->uuid[13], node->uuid[14], node->uuid[15]);
            atserver_send(",%04x,%02x,%02x,%02x,%08x", node->net_idx, node->element_num, node->oob_info, node->flags, node->iv_index);
            const uint8_t *key_val;
            key_val = provisioner_get_device_key(unicast_addr);

            if (!key_val) {
                AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_GET_APPKEY_FAILD));
                return;
            }

            atserver_send(",%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n", key_val[0], key_val[1], key_val[2], \
                          key_val[3], key_val[4], key_val[5], key_val[6], key_val[7], key_val[8], \
                          key_val[9], key_val[10], key_val[11], key_val[12], key_val[13], key_val[14], key_val[15]);
        }

        AT_BACK_OK();
    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_prov_add_node(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {

        struct node_info node;
        uint8_t dev_key_str[33];
        uint8_t dev_key[16];
        uint8_t dev_addr_str[20] = {0};
        uint8_t uuid_str[33] = {0};
        uint32_t oob_info;
        uint32_t elem_num;
        uint32_t unicast_addr;
        uint32_t net_idx;
        uint32_t flags;
        uint32_t iv_index;
        uint32_t addr_type;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%[^,],%x,%[^,],%x,%x,%x,%x,%x,%x,%[^,]", dev_addr_str, &addr_type, uuid_str, &net_idx, &unicast_addr, &elem_num, &oob_info, &flags, &iv_index, dev_key_str);

        if (input_num < 10) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = str2bt_addr((char *)dev_addr_str, node.addr.a.val);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (addr_type != DEV_ADDR_LE_RANDOM && addr_type != DEV_ADDR_LE_PUBLIC) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        node.addr.type = addr_type;

        if (str2hex(node.uuid, (char *)uuid_str, sizeof(uuid_str)) < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (str2hex(dev_key, (char *)dev_key_str, sizeof(dev_key_str)) < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        node.net_idx = net_idx;
        node.unicast_addr = unicast_addr;
        node.element_num = elem_num;
        node.oob_info = oob_info;
        node.flags = flags;
        node.iv_index = iv_index;

        ret =  bt_mesh_provisioner_add_node(&node, dev_key);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_ADD_NODE_FAILD));
            return;
        }

        AT_BACK_OK();
    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<addr_type>,<uuid>,<net_idx>,<unicast_addr>,<element_num>,<oob_info>,<flags>,<iv_index>,<dev_key>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

/*-----------------------------------------------------------
config cli cmd api
-----------------------------------------------------------*/

void at_cmd_btmesh_get_comp(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t comp_page;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x", &unicast_addr, &comp_page);

        if (input_num < 1) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num < 2) {
            comp_page = 0;
        }

        ret = bt_mesh_cfg_comp_data_get(0, unicast_addr, comp_page, NULL, NULL);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
            return;
        }

        AT_BACK_OK();

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>,[page]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}




void at_cmd_btmesh_appkey_add(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t appkey_idx;
        int input_num;
        int ret;

        input_num = atserver_scanf("%x,%x", &unicast_addr, &appkey_idx);

        if (input_num != 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = ble_mesh_node_appkey_add(0, appkey_idx, unicast_addr);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }
    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>,<appkey_idx>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}


void at_cmd_btmesh_appkey_bind(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t appkey_idx;
        uint32_t unicast_addr;
        uint32_t mod_id;
        uint32_t cid;
        int input_num;
        int ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &appkey_idx, &mod_id, &cid);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num == 3) {
            ret = bt_mesh_cfg_mod_app_bind(0, unicast_addr, unicast_addr, appkey_idx, mod_id, NULL);
        } else {
            ret = bt_mesh_cfg_mod_app_bind_vnd(0, unicast_addr, unicast_addr, appkey_idx, mod_id, cid, NULL);
        }

        if (ret && TIMEOUT != ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>,<appkey_idx>,<mod_id>,[cid]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}

void at_cmd_btmesh_appkey_unbind(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }


    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t appkey_idx;
        uint32_t unicast_addr;
        uint32_t mod_id;
        uint32_t cid;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &appkey_idx, &mod_id, &cid);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num == 3) {
            ret = bt_mesh_cfg_mod_app_unbind(0, unicast_addr, unicast_addr, appkey_idx, mod_id, NULL);
        } else {
            ret = bt_mesh_cfg_mod_app_unbind_vnd(0, unicast_addr, unicast_addr, appkey_idx, mod_id, cid, NULL);
        }

        if (ret && TIMEOUT != ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>,<appkey_idx>,<mod_id>,[cid]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}



void at_cmd_btmesh_relay(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }


    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t onoff;
        uint32_t count;
        uint32_t interval;
        uint16_t new_transmit;
        uint8_t transmit;
        int16_t input_num;
        int16_t ret;
        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &onoff, &count, &interval);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num < 3) {
            count = 2;
        }

        if (input_num < 4) {
            interval = 20;
        }

        new_transmit = BT_MESH_TRANSMIT(count, interval);

        ret = bt_mesh_cfg_relay_set(0, unicast_addr, onoff,
                                    new_transmit, NULL, &transmit);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
            return;
        }

        AT_BACK_OK();

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>,<onoff>,[count],[interval]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_proxy(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t onoff;
        int16_t input_num;
        int16_t ret;
        input_num = atserver_scanf("%x,%x", &unicast_addr, &onoff);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = bt_mesh_cfg_gatt_proxy_set(0, unicast_addr, onoff, NULL);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
            return;
        }

        AT_BACK_OK();

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>,<onoff>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_friend(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t onoff;
        int16_t input_num;
        int16_t ret;
        input_num = atserver_scanf("%x,%x", &unicast_addr, &onoff);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        ret = bt_mesh_cfg_friend_set(0, unicast_addr, onoff, NULL);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
            return;
        }

        AT_BACK_OK();
    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>,<onoff>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_pub(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }


    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t mod_id;
        struct bt_mesh_cfg_mod_pub pub;
        uint32_t count;
        uint32_t ttl;
        uint32_t period;
        uint32_t cid;
        uint32_t interval;
        uint32_t pub_addr;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x,%x,%x,%x,%x,%x", &unicast_addr, &mod_id, &pub_addr, &ttl, &period, &count, &interval, &appkey_idx, &cid);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num < 4) {
            ttl = DEF_PUB_SET_TTL;
        }

        if (input_num < 5) {
            period = DEF_PUB_SET_PERIOD;
        }

        if (input_num < 6) {
            count = DEF_PUB_SET_COUNT;
        }

        if (input_num < 7) {
            interval = DEF_PUB_SET_INTERVAlL;
        }

        if (input_num < 8) {
            appkey_idx = DEF_PUB_SET_APPKEY_IDX;
        }

        if (interval > (31 * 50) || (interval % 50)) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        pub.ttl = ttl;
        pub.period = period;
        pub.addr = pub_addr;
        pub.app_idx = appkey_idx;
        pub.transmit = BT_MESH_PUB_TRANSMIT(count, interval);

        if (input_num == 9) {
            ret = bt_mesh_cfg_mod_pub_set_vnd(0, unicast_addr, unicast_addr, mod_id, cid, &pub, NULL);
        } else {
            ret = bt_mesh_cfg_mod_pub_set(0, unicast_addr, unicast_addr, mod_id, &pub, NULL);
        }

        if (ret && TIMEOUT != ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_ADD_PUB_ADDR_FAILD));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicat_addr>,<mod_id>,<pub_addr>,[ttl],[period],[count],[interval],[appkey_idx],[cid]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}




void at_cmd_btmesh_sub(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t sub_addr;
        uint32_t mod_id;
        uint32_t cid;
        int input_num;
        int ret;

        input_num = atserver_scanf("%x,%x,%x,%X", &unicast_addr, &mod_id, &sub_addr, &cid);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num == 3) {
            ret = bt_mesh_cfg_mod_sub_add(0, unicast_addr, unicast_addr, sub_addr, mod_id, NULL);
        } else {
            ret = bt_mesh_cfg_mod_sub_add_vnd(0, unicast_addr, unicast_addr, sub_addr, mod_id, cid, NULL);
        }


        if (ret && TIMEOUT != ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_ADD_SUB_ADDR_FAILD));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicat_addr>,<mod_id>,<sub_addr>,[cid]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_sub_del(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t sub_addr;
        uint32_t mod_id;
        uint32_t cid;
        int input_num;
        int ret;

        input_num = atserver_scanf("%x,%x,%x,%X", &unicast_addr, &mod_id, &sub_addr, &cid);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num == 3) {
            ret = bt_mesh_cfg_mod_sub_del(0, unicast_addr, unicast_addr, sub_addr, mod_id, NULL);
        } else {
            ret = bt_mesh_cfg_mod_sub_del_vnd(0, unicast_addr, unicast_addr, sub_addr, mod_id, cid, NULL);
        }

        if (ret && TIMEOUT != ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_DEL_SUB_ADDR_FAILD));
        } else {

            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicat_addr>,<mod_id>,<sub_addr>,[cid]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


int print_local_subaddr(uint16_t unicast_addr, uint8_t elem_id, uint16_t mod_id, uint16_t CID)
{
    const struct bt_mesh_comp *comp = ble_mesh_model_get_comp_data();

    if (NULL == comp) {
        atserver_send("\r\n+MESHGETSUB:%d\r\nERROR\r\n", ERR_MESH_AT(AT_ERR_MESH_GET_SUB_ADDR_FAILD));
        return -1;
    }

    if (elem_id >= comp->elem_count) {
        atserver_send("\r\n+MESHGETSUB:%d\r\nERROR\r\n", ERR_MESH_AT(AT_ERR_MESH_GET_SUB_ADDR_FAILD));
        return -1;
    }

    struct bt_mesh_model *model = NULL;

    model = ble_mesh_model_find(elem_id, mod_id, CID);

    if (!model) {
        atserver_send("\r\n+MESHGETSUB:%d\r\nERROR\r\n", ERR_MESH_AT(AT_ERR_MESH_GET_SUB_ADDR_FAILD));
        return -1;
    }

    AT_BACK_OK();

    uint8_t found_addr = 0;

    for (int i = 0 ; i < sizeof(model->groups) / sizeof(model->groups[0]); i++) {
        if (model->groups[i] != BT_MESH_ADDR_UNASSIGNED) {
            if (!found_addr) {
                atserver_send("\r\n+MESHSUBLIST:%04x,%04x,%04x,%04x", unicast_addr, CID, mod_id, model->groups[i]);
                found_addr = 1;
            } else {
                atserver_send(",%04x", model->groups[i]);
            }
        }
    }

    if (found_addr) {
        atserver_send("\r\n");
    }

    return 0;
}

void at_cmd_btmesh_sub_get(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t mod_id;
        uint32_t cid;
        int input_num;
        int ret;

        input_num = atserver_scanf("%x,%x,%x", &unicast_addr, &mod_id, &cid);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num == 2) {
            ret =  bt_mesh_cfg_mod_sub_get(0, unicast_addr, mod_id, NULL, NULL);
        } else {
            ret = bt_mesh_cfg_mod_sub_get_vnd(0, unicast_addr, mod_id, cid, NULL, NULL);
        }

        if (ret && TIMEOUT != ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_GET_SUB_ADDR_FAILD));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicat_addr>,<mod_id>,[cid]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_pub_get(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t mod_id;
        uint32_t cid;
        int input_num;
        int ret;

        input_num = atserver_scanf("%x,%x,%x", &unicast_addr, &mod_id, &cid);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (input_num == 2) {
            ret =  bt_mesh_cfg_mod_pub_get(0, unicast_addr, unicast_addr, mod_id, NULL, NULL);
        } else {
            ret = bt_mesh_cfg_mod_pub_get_vnd(0, unicast_addr, unicast_addr, mod_id, cid, NULL, NULL);
        }

        g_mesh_server.get_pub_flag = 1;

        if (ret && TIMEOUT != ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_GET_PUB_ADDR_FAILD));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicat_addr>,<mod_id>,[cid]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_rst(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        int input_num;
        int ret;

        input_num = atserver_scanf("%x", &unicast_addr);

        if (1 != input_num) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        bt_mesh_cfg_cli_timeout_set(0);
        ret = bt_mesh_cfg_node_reset(0, unicast_addr);
        bt_mesh_cfg_cli_timeout_set(2);

        if (ret && TIMEOUT1 != ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
            g_mesh_server.manu_rst_flag = 1;
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<unicast_addr>");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


/*-----------------------------------------------------------
onoff cli cmd api
-----------------------------------------------------------*/


set_onoff_arg set_onoff_data;

void at_cmd_btmesh_onoff(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t onoff;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &onoff, &ack, &appkey_idx);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *onoff_cli_model;

        onoff_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, CID_NVAL);

        if (!onoff_cli_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 2) {
            ack = 0;
        }

        if (input_num <= 3) {
            appkey_idx = 0;
        }

        set_onoff_data.onoff = onoff;

        ret = ble_mesh_generic_onoff_set(0, appkey_idx, unicast_addr, onoff_cli_model, &set_onoff_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<onoff>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

/*-----------------------------------------------------------
level cli cmd api
-----------------------------------------------------------*/


set_level_arg set_level_data;


void at_cmd_btmesh_level(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t level;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &level, &ack, &appkey_idx);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *level_cli_model;

        level_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_LEVEL_CLI, CID_NVAL);

        if (!level_cli_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 2) {
            ack = 0;
        }

        if (input_num <= 3) {
            appkey_idx = 0;
        }

        set_level_data.level = level;

        ret = ble_mesh_generic_level_set(0, appkey_idx, unicast_addr, level_cli_model, &set_level_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<level>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_level_move(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t move;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &move, &ack, &appkey_idx);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *level_cli_model;

        level_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_LEVEL_CLI, CID_NVAL);

        if (!level_cli_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 2) {
            ack = 0;
        }

        if (input_num <= 3) {
            appkey_idx = 0;
        }

        set_level_data.move = move;

        ret = ble_mesh_generic_level_move_set(0, appkey_idx, unicast_addr, level_cli_model, &set_level_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<move>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_level_move_delta(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t move_delta;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &move_delta, &ack, &appkey_idx);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *level_cli_model;

        level_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_LEVEL_CLI, CID_NVAL);

        if (!level_cli_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 2) {
            ack = 0;
        }

        if (input_num <= 3) {
            appkey_idx = 0;
        }

        set_level_data.delta = move_delta;

        ret = ble_mesh_generic_level_delta_set(0, appkey_idx, unicast_addr, level_cli_model, &set_level_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<delta>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}



/*-----------------------------------------------------------
lightness cli cmd api
-----------------------------------------------------------*/


set_lightness_arg set_lightness_data;


void at_cmd_btmesh_lightness(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t lightness;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &lightness, &ack, &appkey_idx);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *lightness_cli_model;

        lightness_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, CID_NVAL);

        if (!lightness_cli_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 2) {
            ack = 0;
        }

        if (input_num <= 3) {
            appkey_idx = 0;
        }

        set_lightness_data.lightness = lightness;
        ret = ble_mesh_light_lightness_set(0, appkey_idx, unicast_addr, lightness_cli_model, &set_lightness_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<lightness>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_lightness_linear(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t lightness_linear;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &lightness_linear, &ack, &appkey_idx);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *lightness_cli_model;

        lightness_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, CID_NVAL);

        if (!lightness_cli_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 2) {
            ack = 0;
        }

        if (input_num <= 3) {
            appkey_idx = 0;
        }

        set_lightness_data.lightness_linear = lightness_linear;
        ret = ble_mesh_light_lightness_linear_set(0, appkey_idx, unicast_addr, lightness_cli_model, &set_lightness_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<lightnesslin>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_lightness_range(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }


    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t range_min;
        uint32_t range_max;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x,%x", &unicast_addr, &range_min, &range_max, &ack, &appkey_idx);

        if (input_num < 3 || !range_min || !range_max || range_min > range_max) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *lightness_cli_model;

        lightness_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, CID_NVAL);

        if (!lightness_cli_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 3) {
            ack = 0;
        }

        if (input_num <= 4) {
            appkey_idx = 0;
        }

        set_lightness_data.range_min = range_min;
        set_lightness_data.range_max = range_max;

        ret = ble_mesh_light_lightness_range_set(0, appkey_idx, unicast_addr, lightness_cli_model, &set_lightness_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<range_min>,<range_max>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_lightness_def(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t lightness_def;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x", &unicast_addr, &lightness_def, &ack, &appkey_idx);

        if (input_num < 2) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *lightness_cli_model;

        lightness_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, CID_NVAL);

        if (!lightness_cli_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 2) {
            ack = 0;
        }

        if (input_num <= 3) {
            appkey_idx = 0;
        }

        set_lightness_data.def = lightness_def;

        ret = ble_mesh_light_lightness_def_set(0, appkey_idx, unicast_addr, lightness_cli_model, &set_lightness_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<lightness>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


/*-----------------------------------------------------------
light ctl cli cmd api
-----------------------------------------------------------*/
set_light_ctl_arg set_light_ctl_data;



void at_cmd_btmesh_light_ctl(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t lightness;
        uint32_t temperature;
        uint32_t delta_uv;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x,%x,%x", &unicast_addr, &lightness, &temperature, &delta_uv, &ack, &appkey_idx);

        if (input_num < 4 || temperature < CTL_TEMP_MIN || temperature > CTL_TEMP_MAX) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *light_ctl_model;

        light_ctl_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_CTL_CLI, CID_NVAL);

        if (!light_ctl_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num < 5) {
            ack = 0;
        }

        if (input_num < 6) {
            appkey_idx = DEF_SEND_APPKEY_IDX;
        }

        set_light_ctl_data.lightness  = lightness;
        set_light_ctl_data.temperature = temperature;
        set_light_ctl_data.delta_uv = delta_uv;

        ret = ble_mesh_light_ctl_set(0, appkey_idx, unicast_addr, light_ctl_model, &set_light_ctl_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<lightness>,<temperature>,<delta_UV>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_light_ctl_temp(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t temperature;
        uint32_t delta_uv;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x,%x,%x", &unicast_addr, &temperature, &delta_uv, &ack, &appkey_idx);

        if (input_num < 3 || temperature < CTL_TEMP_MIN || temperature > CTL_TEMP_MAX) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *light_ctl_model;

        light_ctl_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_CTL_CLI, CID_NVAL);

        if (!light_ctl_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num < 4) {
            ack = 0;
        }

        if (input_num < 5) {
            appkey_idx = DEF_SEND_APPKEY_IDX;
        }

        set_light_ctl_data.temperature = temperature;
        set_light_ctl_data.delta_uv = delta_uv;
        set_light_ctl_data.tid++;

        ret = ble_mesh_light_ctl_temp_set(0, appkey_idx,  unicast_addr, light_ctl_model, &set_light_ctl_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<temperature>,<delta_UV>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

void at_cmd_btmesh_light_ctl_range(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t range_min;
        uint32_t range_max;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x,%x", &unicast_addr, &range_min, &range_max, &ack, &appkey_idx);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        if (range_min < CTL_TEMP_MIN || range_max > CTL_TEMP_MAX || range_min > range_max) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *light_ctl_model;

        light_ctl_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_CTL_CLI, CID_NVAL);

        if (!light_ctl_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num < 4) {
            ack = 0;
        }

        if (input_num < 5) {
            appkey_idx = DEF_SEND_APPKEY_IDX;
        }

        set_light_ctl_data.range_min = range_min;
        set_light_ctl_data.range_max = range_max;


        ret = ble_mesh_light_ctl_temp_range_set(0, appkey_idx, unicast_addr, light_ctl_model, &set_light_ctl_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<range_min>,<range_max>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}


void at_cmd_btmesh_light_ctl_def(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }


    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t lightness;
        uint32_t temperature;
        uint32_t delta_uv;
        uint32_t ack;
        uint32_t appkey_idx;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x,%x,%x,%x,%x,%x", &unicast_addr, &lightness, &temperature, &delta_uv, &ack, &appkey_idx);

        if (input_num < 4 || temperature < CTL_TEMP_MIN || temperature > CTL_TEMP_MAX) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *light_ctl_model;

        light_ctl_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_CTL_CLI, CID_NVAL);

        if (!light_ctl_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num < 5) {
            ack = 0;
        }

        if (input_num < 6) {
            appkey_idx = DEF_SEND_APPKEY_IDX;
        }

        set_light_ctl_data.lightness = lightness;
        set_light_ctl_data.temperature = temperature;
        set_light_ctl_data.delta_uv = delta_uv;
        set_light_ctl_data.tid++;

        ret = ble_mesh_light_ctl_def_set(0, appkey_idx, unicast_addr, light_ctl_model, &set_light_ctl_data, ack);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<lightness>,<temperature>,<delta_UV>,[ack],[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }
}

/*-----------------------------------------------------------
vendor cli cmd api
-----------------------------------------------------------*/
void at_cmd_btmesh_vendor_send(char *cmd, int type, char *data)
{
    if (!g_mesh_server.init_flag) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_AT_NOT_INIT));
        return;
    }

    if (!g_mesh_server.provisioner_enabled) {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_PROVISIONER_NOT_ENABLED));
        return;
    }

    if (type == WRITE_CMD) {
        uint32_t unicast_addr;
        uint32_t len;
        uint8_t  data_str[2 * DEF_TRS_MAX_LENGTH];
        uint8_t  data_hex[DEF_TRS_MAX_LENGTH];
        uint32_t appkey_idx;
        int16_t input_num;
        static uint8_t tid = 0;
        int16_t ret;
        vnd_model_msg vendor_msg = {0};
        input_num = atserver_scanf("%x,%x,%[^,],%x", &unicast_addr, &len, data_str, &appkey_idx);

        if (input_num < 3) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        struct bt_mesh_model *vendor_model;

        vendor_model = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_CLI, VND_CID);

        if (!vendor_model) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_MESH_CMD_NOT_SUPPORTED));
            return;
        }

        if (input_num <= 3) {
            appkey_idx = 0;
        }

        uint8_t msg_len = (strlen((char *)data_str) + 1) / 2 < len ? (strlen((char *)data_str) + 1) / 2 : len;

        if (msg_len > DEF_TRS_MAX_LENGTH) {
            AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
            return;
        }

        vendor_msg.netkey_idx = 0;
        vendor_msg.appkey_idx = appkey_idx;
        vendor_msg.dst_addr = unicast_addr;
        vendor_msg.model = vendor_model;
        vendor_msg.len = msg_len;
        vendor_msg.tid = tid++;
        vendor_msg.opid = VENDOR_OP_ATTR_TRANS_MSG;
        str2hex(data_hex, (char *)data_str, vendor_msg.len);
        vendor_msg.data = data_hex;

        ret = ble_mesh_vendor_cli_model_msg_send(&vendor_msg);

        if (ret) {
            AT_BACK_RET_ERR(cmd, ERR_MESH(ret));
        } else {
            AT_BACK_OK();
        }

    } else if (type == TEST_CMD) {
        AT_BACK_TEST_RET_OK(cmd, "<addr>,<len>,<data>,[appkey_idx]");
    } else {
        AT_BACK_RET_ERR(cmd, ERR_MESH_AT(AT_ERR_INVAL));
    }

}

#endif
