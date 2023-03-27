/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include <aos/aos.h>
#include "mesh_node.h"
#include "api/mesh.h"
#include "body_sensor_led.h"
#include "body_sensor_input.h"
#include "body_sensor_queue.h"
#include "mesh_ctrl.h"
#include "app_main.h"
#include "app_init.h"

#ifdef CONFIG_BT_MESH_LPM
#include "mesh_lpm.h"
#endif

#define TAG "DEMO"

#ifndef CONFIG_BT_MESH_SLEEP_DELAY
#define CONFIG_BT_MESH_SLEEP_DELAY 180000
#endif

struct bt_mesh_model *g_onoff_model = NULL;

struct {
    uint8_t button1_press : 1;
    uint8_t button2_press : 1;
    uint8_t on_off        : 1;
} body_sensor_state = { 0 };

void prepare_onoff_state(uint8_t onoff)
{
    body_sensor_state.on_off = onoff;
}

static void body_sensor_init(void)
{
    int ret = 0;

    input_event_init();
    queue_init();

    _body_sensor_led_config_t led_config[] = {
        BODY_SENSOR_LED_PIN(LED_PIN_1, 0),
    };
    ret = body_sensor_led_init(led_config, sizeof(led_config) / sizeof(led_config[0]));
    if (ret < 0) {
        LOGD(TAG, "init led failed");
    }
}

#ifdef CONFIG_BT_MESH_LPM
__attribute__((section(".__sram.code"))) static void body_sensor_lpm_cb(mesh_lpm_wakeup_reason_e reason,
                                                                        mesh_lpm_status_e status, void *arg)
{
    if (status == STATUS_WAKEUP) {
        if (reason == WAKEUP_BY_IO) {
            _mesh_lpm_io_status_list_t *list = (_mesh_lpm_io_status_list_t *)arg;
            for (int i = 0; i < list->size; i++) {
                if (list->io_status[i].trigger_flag == true) {
                    extern void _key_input_process(uint8_t port);
                    _key_input_process(list->io_status[i].port);
                }
            }
        }
    } else {
        LOGI(TAG, "sleep");
    }
}
#endif

static void handle_input_event(uint8_t input_data)
{
    static uint8_t onoff = 0;
    int            ret   = 0;
    // uint8_t port = input_data & 0x0F;
    uint8_t input_type = (input_data & 0xF0) >> 4;
    switch (input_type) {
        case BODY_SENSOR_INPUT_ONCE: {
            onoff = !onoff;
            extern int report_onoff_status(uint8_t onoff, uint8_t is_ack);
            ret = report_onoff_status(onoff, 1);
            if (ret < 0) {
                LOGE(TAG, "report fail,enter sleep directly");
#ifdef CONFIG_BT_MESH_LPM
                ret = mesh_lpm_enable(true);
                if (ret < 0) {
                    LOGE(TAG, "lpm enter failed %d", ret);
                }
#endif
            }
        } break;
        default:
            break;
    }
}

static void body_sensor_lpm_init()
{
#ifdef CONFIG_BT_MESH_LPM
    mesh_lpm_wakeup_io_config_t io = MESH_WAKEUP_PIN(INPUT_EVENT_PIN_1, INPUT_PIN_POL_PIN_1);

    mesh_lpm_conf_t lpm_config;
    lpm_config.is_auto_enable                    = 1;
    lpm_config.lpm_mode                          = MESH_LPM_MODE_TX_RX;
    lpm_config.lpm_wakeup_mode                   = WAKEUP_BY_IO_MODE;
    lpm_config.lpm_wakeup_io_config.io_list_size = 1;
    lpm_config.lpm_wakeup_io_config.io_config    = &io;
    lpm_config.delay_sleep_time                  = CONFIG_BT_MESH_SLEEP_DELAY; // Unit:ms
    lpm_config.mesh_lpm_cb                       = body_sensor_lpm_cb;
    mesh_lpm_init(&lpm_config);
#endif
    return;
}

int main()
{
    int          ret;
    queue_mesg_t queue_mesg;
    uint8_t      press_data = 0;

    /* Board Hardware and yoc modules init */
    board_yoc_init();

    LOGI(TAG, "Mesh Body Sensor demo %s", aos_get_app_version());

    app_set_led_state(LED_OFF);

    /* Mesh Models and Device parameter setting, include models event callback regiter */
    ret = mesh_dev_init();

    if (ret) {
        LOGE(TAG, "mesh dev init failed");
    }

    g_onoff_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, CID_NVAL);
    if (!g_onoff_model) {
        LOGE(TAG, "onoff cli model not found");
    }

    body_sensor_init();

#ifdef CONFIG_BT_MESH_LPM
    body_sensor_lpm_init();

    if (bt_mesh_is_provisioned()) {
        mesh_lpm_start();
    }
#endif

    LOGI(TAG, "Mesh Body Sensor node demo");

    /* Wait for button signal to publish on/off command to default group */
    while (1) {
        if (queue_recv_data(&queue_mesg) < 0) {
            continue;
        }

        if (QUEUE_MESG_TYPE_INPUT == queue_mesg.type) {
            press_data = queue_mesg.data;
            handle_input_event(press_data);
        }
    }

    return 0;
}
