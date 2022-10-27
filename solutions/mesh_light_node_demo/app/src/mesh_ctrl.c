/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <aos/aos.h>

#include <app_init.h>
#include <aos/cli.h>

#include "mesh_node.h"
#include "app_main.h"

#include "vendor/vendor_model.h"
#include "mesh_model/mesh_model.h"
#include "sig_model/generic_onoff_srv.h"
#if defined(CONFIG_OCC_AUTH) && CONFIG_OCC_AUTH
#include "occ_auth.h"
#endif
static const char *TAG = "Mesh CTRL";

#define BT_MESH_MODEL_ELEMENT_IDX 0

#ifdef CONFIG_PACKET_LOSS_TEST
#define PACKET_LOSS_TEST_SET       0xEE
#define PACKET_LOSS_TEST_SET_SATRT 0x01
#define PACKET_LOSS_TEST_SET_STOP  0x02
#define PACKET_LOSS_TEST_SET_GET   0x03
#define PACKET_LOSS_TEST_SET_ACK   0xEF
enum test_status
{
    TEST_NOT_START,
    TEST_START,
    TEST_FINISH,
};
uint32_t g_onoff_set_num = 0;
uint8_t  g_test_status   = TEST_NOT_START;
#endif

extern const char *bt_hex_real(const void *buf, size_t len);
extern void        bt_mesh_prov_set_output_oob_num(uint32_t oob);
extern void        triples_get_process(model_message *message);
extern int         dut_hal_mac_get(uint8_t addr[6]);

static uint8_t att_off_flag;
static uint8_t prov_succeed_flag;

/* Mesh Models Event Callback Function */
void app_models_event_cb(mesh_model_event_en event, void *p_arg)
{
    switch (event) {
        case BT_MESH_MODEL_ONOFF_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;

                if (elem_state->state.onoff[T_TAR]) {
                    app_set_led_state(LED_ON);
                } else {
                    app_set_led_state(LED_OFF);
                }
#ifdef CONFIG_PACKET_LOSS_TEST
                {
                    if (g_test_status == TEST_START)
                        g_onoff_set_num++;
                }
#endif
                LOGI(TAG, "src:0x%04x,led:%s", message.source_addr, elem_state->state.onoff[T_TAR] ? "ON" : "OFF");
            }

        } break;

        case BT_MESH_MODEL_LEVEL_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,level:%d", message.source_addr, elem_state->state.level[T_TAR]);
            }
        } break;

        case BT_MESH_MODEL_LEVEL_DELTA_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,level+delta:%d", message.source_addr, elem_state->state.level[T_TAR]);
            }
        } break;

        case BT_MESH_MODEL_LIGHTNESS_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,lightness:%d", message.source_addr, elem_state->state.lightness_actual[T_TAR]);
            }
        } break;

        case BT_MESH_MODEL_LIGHTNESS_LINEAR_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,lightness linear:%d", message.source_addr,
                     elem_state->state.lightness_linear[T_TAR]);
            }
        } break;

        case BT_MESH_MODEL_LIGHTNESS_RANGE_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,lightness range: min:%d max %d", message.source_addr,
                     elem_state->powerup.lightness_range.range_min, elem_state->powerup.lightness_range.range_max);
            }

        } break;

        case BT_MESH_MODEL_LIGHTNESS_DEF_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,lightness default:%d", message.source_addr,
                     elem_state->powerup.lightness_actual_default);
            }
        } break;

        case BT_MESH_MODEL_LIGHT_CTL_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,ctlset:lightness:%d temp:%d delta:%d", message.source_addr,
                     elem_state->state.lightness[T_TAR], elem_state->state.temp[T_TAR], elem_state->state.UV[T_TAR]);
            }
        } break;

        case BT_MESH_MODEL_LIGHT_CTL_TEMP_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,ctl temp set:temp:%d uv:%d", message.source_addr, elem_state->state.temp[T_TAR],
                     elem_state->state.UV[T_TAR]);
            }
        } break;

        case BT_MESH_MODEL_LIGHT_CTL_RANGE_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,ctl range set:min:%d max:%d", message.source_addr,
                     elem_state->powerup.ctl_temp_range.range_min, elem_state->powerup.ctl_temp_range.range_max);
            }
        } break;

        case BT_MESH_MODEL_LIGHT_CTL_DEF_SET: {
            if (p_arg) {
                model_message message    = *(model_message *)p_arg;
                S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
                LOGI(TAG, "src:0x%04x,ctldefset:temp:%d delta:%d", message.source_addr, elem_state->state.temp[T_TAR],
                     elem_state->state.UV[T_TAR]);
            }
        } break;

        case BT_MESH_MODEL_VENDOR_MESSAGES: {
            if (p_arg) {
                model_message      message = *(model_message *)p_arg;
                extern const char *bt_hex(const void *buf, size_t len);
                LOGI(TAG, "vendor messages:0x%04x,data:%s", message.source_addr,
                     bt_hex_real(message.ven_data.user_data, message.ven_data.data_len));
#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT
                triples_get_process(&message);
#endif
#ifdef CONFIG_PACKET_LOSS_TEST
                struct bt_mesh_model *vendor_model;
                uint8_t *             data_get = (uint8_t *)message.ven_data.user_data;
                vendor_model                   = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_SRV, CONFIG_CID_TAOBAO);
                if (!vendor_model) {
                    return;
                }
                if (data_get[0] == PACKET_LOSS_TEST_SET) {
                    if (data_get[1] == PACKET_LOSS_TEST_SET_SATRT) {
                        g_test_status          = TEST_START;
                        g_onoff_set_num        = 0;
                        vnd_model_msg vnd_data = { 0 };
                        uint8_t       data[2]  = { 0 };
                        data[0]                = PACKET_LOSS_TEST_SET_ACK;
                        data[1]                = TEST_START;
                        vnd_data.netkey_idx    = 0;
                        vnd_data.appkey_idx    = 0;
                        vnd_data.dst_addr      = message.source_addr;
                        vnd_data.model         = vendor_model;
                        vnd_data.opid          = VENDOR_OP_ATTR_TRANS_MSG;
                        vnd_data.data          = data;
                        vnd_data.retry         = 0;
                        vnd_data.len           = sizeof(data);
                        ble_mesh_vendor_srv_model_msg_send(&vnd_data);
                        LOGI(TAG, "Loss Test Start");
                    } else if (data_get[1] == PACKET_LOSS_TEST_SET_STOP || data_get[1] == PACKET_LOSS_TEST_SET_GET) {
                        vnd_model_msg vnd_data = { 0 };
                        uint8_t       data[6]  = { 0 };
                        data[0]                = PACKET_LOSS_TEST_SET_ACK;
                        data[1]                = data_get[1] == PACKET_LOSS_TEST_SET_STOP ? TEST_FINISH : TEST_START;
                        data[2]                = g_onoff_set_num & 0xFF;
                        data[3]                = (g_onoff_set_num >> 8) & 0xFF;
                        data[4]                = (g_onoff_set_num >> 16) & 0xFF;
                        data[5]                = (g_onoff_set_num >> 24) & 0xFF;
                        vnd_data.netkey_idx    = 0;
                        vnd_data.appkey_idx    = 0;
                        vnd_data.dst_addr      = message.source_addr;
                        vnd_data.model         = vendor_model;
                        vnd_data.opid          = VENDOR_OP_ATTR_TRANS_MSG;
                        vnd_data.data          = data;
                        vnd_data.retry         = 0;
                        vnd_data.len           = sizeof(data);
                        ble_mesh_vendor_srv_model_msg_send(&vnd_data);
                        if (data_get[1] == PACKET_LOSS_TEST_SET_STOP) {
                            LOGI(TAG, "Loss Test Stop,recv %d onoff set messages", g_onoff_set_num);
                            g_test_status   = TEST_NOT_START;
                            g_onoff_set_num = 0;
                        }
                    }
                }
#endif
            }
        } break;

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
        } break;

        case BT_MESH_EVENT_NODE_REST: {
            LOGI(TAG, "node reset");
            app_set_led_state(LED_UNPROVED);
        } break;

        case BT_MESH_EVENT_NODE_OOB_INPUT_NUM: {
            if (p_arg) {
                LOGI(TAG, "oob input num size:%d", *(uint8_t *)p_arg);
            }
        } break;

        case BT_MESH_EVENT_NODE_OOB_INPUT_STRING: {
            LOGI(TAG, "oob input string size:%d", *(uint8_t *)p_arg);
        } break;

        default:
            break;
    }
}

void app_attention_on()
{
    att_off_flag = 0;
    app_set_led_state(LED_ATTENTION_ON);
}

void app_attention_off()
{
    att_off_flag = 1;

    if (prov_succeed_flag) {
        app_set_led_state(LED_ATTENTION_OFF);
    }
}

health_srv_cb g_app_health_cb = {
    .att_on  = app_attention_on,
    .att_off = app_attention_off,
};

static struct bt_mesh_model elem0_root_models[] = {
    MESH_MODEL_CFG_SRV_NULL(),       MESH_MODEL_HEALTH_SRV_NULL(),    MESH_MODEL_GEN_ONOFF_SRV_NULL(),
    MESH_MODEL_GEN_LEVEL_SRV_NULL(), MESH_MODEL_LIGHTNESS_SRV_NULL(), MESH_MODEL_CTL_SRV_NULL(),
    MESH_MODEL_CTL_SETUP_SRV_NULL(), MESH_MODEL_CTL_TEMP_SRV_NULL(),
};

static struct bt_mesh_model elem0_vnd_models[] = {
    MESH_MODEL_VENDOR_SRV_NULL(),
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, elem0_root_models, elem0_vnd_models, 0),
};

/* Define device composition data */
static const struct bt_mesh_comp mesh_comp = {
    .cid        = CONFIG_CID_TAOBAO,
    .elem       = elements,
    .elem_count = ARRAY_SIZE(elements),
};

/* Define node parameters */
static node_config_t g_node_param = {
    .dev_uuid      = LIGHT_DEV_UUID,
    .dev_name      = DEVICE_NAME,
    .user_model_cb = app_models_event_cb,
    .user_prov_cb  = app_prov_event_cb,
    .health_cb     = &g_app_health_cb,
};

int mesh_set_uuid(uint8_t uuid[16])
{
#if !defined(CONFIG_OCC_AUTH) || !CONFIG_OCC_AUTH

#ifdef CONFIG_DUT_SERVICE_ENABLE
    uint8_t mac[6] = { 0x0 };
    dut_hal_mac_get(mac);
    memcpy(uuid, mac, 6);
#else
    LOGW(TAG, "Dut mac get not support");
#endif

#endif

#if defined(CONFIG_BT_MESH_LPM) && CONFIG_BT_MESH_LPM > 0 && !defined(CONFIG_BT_MESH_PROVISIONER)
#if defined(CONFIG_BT_MESH_LPM_MODE_TX_ONLY) && CONFIG_BT_MESH_LPM_MODE_TX_ONLY > 0
    uuid[13] = BT_MESH_NODE_LPM_NO_RX;
#elif defined(CONFIG_BT_MESH_LPM_MODE_TX_RX) && CONFIG_BT_MESH_LPM_MODE_TX_RX > 0
    uuid[13] = BT_MESH_NODE_LPM_TX_RX;
#elif defined(CONFIG_BT_MESH_LPM_MODE_RX_TX) && CONFIG_BT_MESH_LPM_MODE_RX_TX > 0
    uuid[13] = BT_MESH_NODE_LPM_RX_TX;
#else
    LOGE(TAG, "lpm mode not support");
    return -ENOTSUP;
#endif
#else
    uuid[13] = 0x0;
#endif
    return 0;
}

int mesh_dev_init(void)
{
    int ret;

#if defined(CONFIG_OCC_AUTH) && CONFIG_OCC_AUTH
    uint32_t short_oob;
    ret = occ_auth_init();
    if (ret) {
        LOGE(TAG, "mesh kp init failed %d", ret);
        return ret;
    }
    ret = occ_auth_set_dev_mac_by_kp();
    if (ret) {
        LOGE(TAG, "mesh kp mac set failed %d", ret);
        return ret;
    }
    ret = occ_auth_get_uuid_and_oob(g_node_param.dev_uuid, &short_oob);
    if (ret) {
        LOGE(TAG, "mesh kp get failed %d", ret);
        return ret;
    }

    bt_mesh_prov_set_output_oob_num(short_oob);

    g_node_param.node_oob.output_action   = ACTION_NUM;
    g_node_param.node_oob.output_max_size = OCC_AUTH_MESH_OOB_SIZE;
#endif

    ret = mesh_set_uuid(g_node_param.dev_uuid);
    if (ret) {
        LOGE(TAG, "App mesh set uuid failed %d", ret);
        return ret;
    }

    ret = ble_mesh_model_init(&mesh_comp);
    if (ret) {
        return -1;
    }

    ret = ble_mesh_node_init(&g_node_param);
    if (ret < 0) {
        LOGE(TAG, "mesh node init failed");
    }

    return 0;
}
