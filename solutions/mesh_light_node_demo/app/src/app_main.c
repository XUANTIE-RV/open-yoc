/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include <aos/aos.h>
#include "app_main.h"
#include <app_init.h>
#include "mesh_node.h"
#include <api/mesh.h>

#define TAG "DEMO"

#define DEVICE_NAME "YoC Light NODE"

#define LIGHT_DEV_UUID {0xcf, 0xa0, 0xe3, 0x7e, 0x17, 0xd9, 0x11, 0xe8, 0x86, 0xd1, 0x5f, 0x1c, 0xe2, 0x8a, 0xde, 0x02}
#define DEV_NOT_PROV_INDICATE_TIMEOUT 100
#define NODE_ATTENTION_TIMEOUT 1000


static aos_sem_t sync_sem;
static uint8_t att_off_flag;
static uint8_t prov_succeed_flag;
extern const char *bt_hex_real(const void *buf, size_t len);

#ifdef CONFIG_PACKET_LOSS_TEST
#define PACKET_LOSS_TEST_SET 0xEE
#define PACKET_LOSS_TEST_SET_SATRT 0x01
#define PACKET_LOSS_TEST_SET_STOP  0x02
#define PACKET_LOSS_TEST_SET_GET   0x03
#define PACKET_LOSS_TEST_SET_ACK   0xEF
enum test_status {
    TEST_NOT_START,
    TEST_START,
    TEST_FINISH,
};
uint32_t g_onoff_set_num = 0;
uint8_t  g_test_status = TEST_NOT_START;
#endif

void app_event_cb(mesh_model_event_en event, void *p_arg)
{
    switch (event) {
    case BT_MESH_MODEL_ONOFF_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;

            if (elem_state->state.onoff[T_TAR]) {
                app_set_led_state(LED_ON);
            } else {
                app_set_led_state(LED_OFF);
            }
#ifdef CONFIG_PACKET_LOSS_TEST
            {
                if(g_test_status == TEST_START)
                    g_onoff_set_num++;
            }
#endif
            LOGI(TAG, "src:0x%04x,led:%s", message.source_addr, elem_state->state.onoff[T_TAR] ? "ON" : "OFF");
        }

    }
    break;

    case BT_MESH_MODEL_LEVEL_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,level:%d", message.source_addr, elem_state->state.level[T_TAR]);
        }
    }
    break;

    case BT_MESH_MODEL_LEVEL_DELTA_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,level+delta:%d", message.source_addr, elem_state->state.level[T_TAR]);
        }
    }
    break;

    case BT_MESH_MODEL_LIGHTNESS_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,lightness:%d", message.source_addr, elem_state->state.lightness_actual[T_TAR]);
        }
    }
    break;

    case BT_MESH_MODEL_LIGHTNESS_LINEAR_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,lightness linear:%d", message.source_addr, elem_state->state.lightness_linear[T_TAR]);
        }
    }
    break;

    case BT_MESH_MODEL_LIGHTNESS_RANGE_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,lightness range: min:%d max %d", message.source_addr, \
                 elem_state->powerup.lightness_range.range_min, elem_state->powerup.lightness_range.range_max);
        }

    }
    break;


    case BT_MESH_MODEL_LIGHTNESS_DEF_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,lightness default:%d", message.source_addr, elem_state->powerup.lightness_actual_default);
        }
    }
    break;

    case BT_MESH_MODEL_LIGHT_CTL_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,ctlset:lightness:%d temp:%d delta:%d", message.source_addr, elem_state->state.lightness_actual[T_TAR], \
                 elem_state->state.temp[T_TAR], elem_state->state.UV[T_TAR]);
        }
    }
    break;

    case BT_MESH_MODEL_LIGHT_CTL_TEMP_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,ctl temp set:temp:%d uv:%d", message.source_addr, elem_state->state.temp[T_TAR], elem_state->state.UV[T_TAR]);
        }
    }
    break;

    case BT_MESH_MODEL_LIGHT_CTL_RANGE_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,ctl range set:min:%d max:%d", message.source_addr, elem_state->powerup.ctl_temp_range.range_min, elem_state->powerup.ctl_temp_range.range_max);
        }
    }
    break;

    case BT_MESH_MODEL_LIGHT_CTL_DEF_SET: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGI(TAG, "src:0x%04x,ctldefset:temp:%d delta:%d", message.source_addr, elem_state->state.temp[T_TAR], elem_state->state.UV[T_TAR]);
        }
    }
    break;

    case BT_MESH_MODEL_VENDOR_MESSAGES: {
        if (p_arg) {
            model_message message = *(model_message *)p_arg;
            extern const char *bt_hex(const void *buf, size_t len);
            LOGI(TAG, "vendor messages:0x%04x,data:%s", message.source_addr, bt_hex_real(message.ven_data.user_data, message.ven_data.data_len));
#ifdef CONFIG_PACKET_LOSS_TEST
            struct bt_mesh_model *vendor_model;
            uint8_t* data_get = (uint8_t*)message.ven_data.user_data;
            vendor_model = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_SRV, CONFIG_CID_TAOBAO);
            if (!vendor_model) {
                return;
            }
            if(data_get[0] == PACKET_LOSS_TEST_SET) {
                if(data_get[1] == PACKET_LOSS_TEST_SET_SATRT) {
                    g_test_status = TEST_START;
                    g_onoff_set_num = 0;
                    vnd_model_msg  vnd_data = {0};
                    uint8_t data[2]= {0};
                    data[0] = PACKET_LOSS_TEST_SET_ACK;
                    data[1] = TEST_START;
                    vnd_data.netkey_idx = 0;
                    vnd_data.appkey_idx = 0;
                    vnd_data.dst_addr = message.source_addr;
                    vnd_data.model = vendor_model;
                    vnd_data.opid = VENDOR_OP_ATTR_TRANS_MSG;
                    vnd_data.data = data;
                    vnd_data.retry = 0;
                    vnd_data.len = sizeof(data);
                    ble_mesh_vendor_srv_model_msg_send(&vnd_data);
                    LOGI(TAG,"Loss Test Start\r\n");
                } else if(data_get[1] == PACKET_LOSS_TEST_SET_STOP || data_get[1] == PACKET_LOSS_TEST_SET_GET) {
                    vnd_model_msg  vnd_data = {0};
                    uint8_t data[6]= {0};
                    data[0] = PACKET_LOSS_TEST_SET_ACK;
                    data[1] = data_get[1] == PACKET_LOSS_TEST_SET_STOP ? TEST_FINISH : TEST_START;
                    data[2] = g_onoff_set_num & 0xFF;
                    data[3] = (g_onoff_set_num >> 8) & 0xFF;
                    data[4] = (g_onoff_set_num >> 16) & 0xFF;
                    data[5] = (g_onoff_set_num >> 24) & 0xFF;
                    vnd_data.netkey_idx = 0;
                    vnd_data.appkey_idx = 0;
                    vnd_data.dst_addr = message.source_addr;
                    vnd_data.model = vendor_model;
                    vnd_data.opid = VENDOR_OP_ATTR_TRANS_MSG;
                    vnd_data.data = data;
                    vnd_data.retry = 0;
                    vnd_data.len = sizeof(data);
                    ble_mesh_vendor_srv_model_msg_send(&vnd_data);
                    if(data_get[1] == PACKET_LOSS_TEST_SET_STOP) {
                        LOGI(TAG,"Loss Test Stop,recv %d onoff set messages\r\n",g_onoff_set_num);
                        g_test_status = TEST_NOT_START;
                        g_onoff_set_num = 0;
                    }
                }

            }
#endif
        }
    }
    break;

    default:
        break;
    }
}

void app_prov_event_cb(mesh_prov_event_en event, void *p_arg)
{
    switch (event) {
    case BT_MESH_EVENT_NODE_PROV_COMP: {
        if (p_arg) {
            mesh_node_local_t *node = (mesh_node_local_t *)p_arg;
            LOGI(TAG, "prov complete %04x", node->prim_unicast);
            prov_succeed_flag = 1;
            app_set_led_state(LED_PROVED);
        }
    }
    break;

    case BT_MESH_EVENT_NODE_REST : {
        LOGI(TAG, "node reset");
        app_set_led_state(LED_UNPROVED);
    }
    break;

    case BT_MESH_EVENT_NODE_OOB_INPUT_NUM : {
        if (p_arg) {
            LOGI(TAG, "oob input num size:%d", *(uint8_t *)p_arg);
        }
    }
    break;

    case BT_MESH_EVENT_NODE_OOB_INPUT_STRING : {
        LOGI(TAG, "oob input string size:%d", *(uint8_t *)p_arg);
    }
    break;

    default:
        break;
    }

}


void app_attention_on()
{
    att_off_flag = 0;
    app_set_led_state(LED_ATTENTION_ON);
    LOGI(TAG, "attention on");
}

void app_attention_off()
{
    att_off_flag = 1;

    if (prov_succeed_flag) {
        app_set_led_state(LED_ATTENTION_OFF);
        LOGI(TAG, "attention off");
    }
}

health_srv_cb g_app_health_cb = {
    .att_on = app_attention_on,
    .att_off = app_attention_off,
};

node_config_t g_node_param = {
    .role = NODE,
    .dev_uuid = LIGHT_DEV_UUID,
    .dev_name = DEVICE_NAME,
    .user_model_cb = app_event_cb,
    .user_prov_cb = app_prov_event_cb,
    .health_cb = &g_app_health_cb,
};

int main()
{
    int ret;

    board_yoc_init();

    LOGI(TAG, "Mesh light node demo\n");

    extern int app_mesh_composition_init();
    ret = app_mesh_composition_init();

    if (ret) {
        LOGE(TAG, "mesh comp init faild\n");
    }

    ret = ble_mesh_node_init(&g_node_param);

    if (ret < 0) {
        LOGE(TAG, "mesh node init faild\n");
    }

    aos_sem_new(&sync_sem, 0);

    while (1) {
        aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
    }

    return 0;
}




