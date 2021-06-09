/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/aos.h>

#include <app_init.h>
#include <aos/cli.h>

#include "mesh_node.h"
#include "app_main.h"

#include "vendor/vendor_model.h"
#include "mesh_model/mesh_model.h"
#include "sig_model/generic_onoff_cli.h"

static const char *TAG = "Mesh CTRL";

#define BT_MESH_MODEL_ELEMENT_IDX  0

static set_onoff_arg set_onoff_data;
static uint8_t att_off_flag;
static uint8_t prov_succeed_flag;

/* Send OnOff Control data to Server */
int gen_onoff_set(uint8_t onoff, uint8_t is_ack)
{
    int ret;
    struct bt_mesh_model *onoff_cli_model;

    /* Find Generic OnOff Client Model in device primary element */
    onoff_cli_model = ble_mesh_model_find(BT_MESH_MODEL_ELEMENT_IDX, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, 0xFFFF);

    if (!onoff_cli_model) {
        LOGE(TAG, "Not Support!");
        return -1;
    }

    set_onoff_data.onoff = onoff;
    set_onoff_data.send_trans = 0;

    /* publish on or off command to destinated address */
    ret = ble_mesh_generic_onoff_cli_publish(onoff_cli_model, &set_onoff_data, is_ack);

    if (ret) {
        LOGE(TAG, "Gen OnOff Pub err %d", ret);
        return ret;
    }

    return 0;
}

/* Mesh Models Event Callback Function */
static void app_models_event_cb(mesh_model_event_en event, void *p_arg)
{

    switch (event) {
        case BT_MESH_MODEL_ONOFF_STATUS: {
            if (p_arg) {
                /* Receive Status from OnOff Server */
                model_message onoff_message = *(model_message *)p_arg;
                uint8_t onoff = onoff_message.status_data->data[0];
                LOGI(TAG, "Addr %04x, OnOff status %s\n", onoff_message.source_addr, onoff ? "ON" : "OFF");
                prepare_onoff_state(!onoff);
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
}

void app_attention_off()
{
    att_off_flag = 1;

    if (prov_succeed_flag) {
        app_set_led_state(LED_ATTENTION_OFF);
    }
}

health_srv_cb g_app_health_cb = {
    .att_on = app_attention_on,
    .att_off = app_attention_off,
};


static struct bt_mesh_model elem0_root_models[] = {
    MESH_MODEL_CFG_SRV_NULL(),
    MESH_MODEL_HEALTH_SRV_NULL(),
    MESH_MODEL_GEN_ONOFF_CLI_NULL(),
};

static struct bt_mesh_model elem0_vnd_models[] = {
    MESH_MODEL_VENDOR_SRV_NULL(),
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, elem0_root_models, elem0_vnd_models, 0),
};

/* Define device composition data */
static const struct bt_mesh_comp mesh_comp = {
    .cid = CONFIG_CID_TAOBAO,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

/* Define node parameters */
static node_config_t g_node_param = {
    .role = NODE,
    .dev_uuid = SWITCH_DEV_UUID,
    .dev_name = DEVICE_NAME,
    .user_model_cb = app_models_event_cb,
    .user_prov_cb = app_prov_event_cb,
    .health_cb = &g_app_health_cb,
};

int mesh_dev_init(void)
{
    int ret;

    memset(&set_onoff_data, 0, sizeof(set_onoff_arg));

    ret = ble_mesh_model_init(&mesh_comp);

    if (ret) {
        return -1;
    }

    ret = ble_mesh_node_init(&g_node_param);

    if (ret < 0) {
        LOGE(TAG, "mesh node init faild\n");
    }

    return 0;
}
