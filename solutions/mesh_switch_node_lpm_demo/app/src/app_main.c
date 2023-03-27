/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include <aos/aos.h>
#include <app_main.h>
#include <app_init.h>
#include "mesh_node.h"
#include <api/mesh.h>
#include "mesh_ctrl.h"
#include "switches_led.h"
#include "switches_input.h"
#include "switches_output.h"
#include "switches_queue.h"
#include "switches_report.h"
#include "mesh_lpm.h"

#define TAG "DEMO"

struct {
    uint8_t   button1_press : 1;
    uint8_t   button2_press : 1;
    uint8_t   on_off        : 1;
    aos_sem_t sync_sem;
} switch_state = { 0 };

#define MESH_LPM_SLEEP_TIME  (1100) // unit:ms
#define MESH_LPM_WAKEUP_TIME (60)   // unit:ms

onoff_status g_status_data[3];

void prepare_onoff_state(uint8_t onoff)
{
    switch_state.on_off = onoff;
}

static void switches_init(void)
{
    int ret = 0;

    input_event_init();
    queue_init();
    _output_io_config_t io_config[] = {
        SWITCH_OUTPUT_PIN(OUTPUT_PIN_1_A, OUTPUT_PIN_1_B),
        SWITCH_OUTPUT_PIN(OUTPUT_PIN_2_A, OUTPUT_PIN_2_B),
        SWITCH_OUTPUT_PIN(OUTPUT_PIN_3_A, OUTPUT_PIN_3_B),
    };

    ret = switch_output_gpio_init(io_config, sizeof(io_config) / sizeof(io_config[0]));
    if (ret < 0) {
        LOGE(TAG, "init output pin failed");
    }
#if 0
    ret = switches_report_init();
    if (ret < 0) {
        printf("init report failed");
    }
#endif
    _switch_led_config_t led_config[] = {
        SWITCH_LED_PIN(LED_PIN_1, 0),
        SWITCH_LED_PIN(LED_PIN_2, 0),
        SWITCH_LED_PIN(LED_PIN_3, 0),
    };
    ret = switch_led_init(led_config, sizeof(led_config) / sizeof(led_config[0]));
    if (ret < 0) {
        LOGD(TAG, "init led failed");
    }
}

#ifdef CONFIG_BT_MESH_LPM
__attribute__((section(".__sram.code"))) static void switches_lpm_cb(mesh_lpm_wakeup_reason_e reason,
                                                                     mesh_lpm_status_e status, void *arg)
{
    if (status == STATUS_WAKEUP) {
        if (reason == WAKEUP_BY_IO) {
            _mesh_lpm_io_status_list_t *list = (_mesh_lpm_io_status_list_t *)arg;
            for (int i = 0; i < list->size; i++) {
                if (list->io_status[i].trigger_flag == true) {
                    extern void _key_pres_process(uint8_t port);
                    _key_pres_process(list->io_status[i].port);
                }
            }
        }
    } else {
        LOGI(TAG, "sleep");
    }
}
#endif

static void handle_input_event(uint8_t press_data)
{
#ifdef CONFIG_BT_MESH_LPM
    static uint8_t sleep_toggle_flag = 0;
#endif
    uint8_t port       = press_data & 0x0F;
    uint8_t press_type = (press_data & 0xF0) >> 4;

    switch (press_type) {
        case SWITCH_PRESS_ONCE: {
            LOGI(TAG, "port %d short press", port);
            uint8_t elem_id = 0;
            if (port == INPUT_EVENT_PIN_1) {
                elem_id = 0;
            } else if (port == INPUT_EVENT_PIN_2) {
                elem_id = 1;
            } else if (port == INPUT_EVENT_PIN_3) {
                elem_id = 2;
            }
            g_status_data[elem_id].onoff = !g_status_data[elem_id].onoff;
            switch_led_set(elem_id, g_status_data[elem_id].onoff);
            switches_report_start(elem_id, g_status_data[elem_id].onoff);
        } break;
        case SWITCH_PRESS_LONG: {
            LOGD(TAG, "port %d long press", port);
#ifdef CONFIG_BT_MESH_LPM
            if (sleep_toggle_flag == 0) {
                mesh_lpm_disable();
                sleep_toggle_flag = 1;
                LOGD(TAG, "genie_lpm_disable");
            } else if (sleep_toggle_flag == 1) {
                mesh_lpm_enable(FALSE);
                sleep_toggle_flag = 0;
                LOGD(TAG, "genie_lpm_enable");
            }
#endif
        } break;

        default:
            break;
    }
}

static void switches_lpm_init()
{
#ifdef CONFIG_BT_MESH_LPM
    mesh_lpm_wakeup_io_config_t io[] = { MESH_WAKEUP_PIN(INPUT_EVENT_PIN_1, INPUT_PIN_POL_PIN_1),
                                         MESH_WAKEUP_PIN(INPUT_EVENT_PIN_2, INPUT_PIN_POL_PIN_2),
                                         MESH_WAKEUP_PIN(INPUT_EVENT_PIN_3, INPUT_PIN_POL_PIN_3) };

    mesh_lpm_conf_t lpm_config;
    lpm_config.is_auto_enable                    = 1;
    lpm_config.lpm_mode                          = MESH_LPM_MODE_RX_TX;
    lpm_config.lpm_wakeup_mode                   = WAKEUP_BY_IO_TIMER_MODE;
    lpm_config.lpm_wakeup_io_config.io_list_size = sizeof(io) / sizeof(io[0]);
    lpm_config.lpm_wakeup_io_config.io_config    = io;
    lpm_config.delay_sleep_time                  = 20 * 1000; // Unit:ms
    lpm_config.lpm_wakeup_timer_config.sleep_ms  = MESH_LPM_SLEEP_TIME;
    lpm_config.lpm_wakeup_timer_config.wakeup_ms = MESH_LPM_WAKEUP_TIME;
    lpm_config.mesh_lpm_cb                       = switches_lpm_cb;
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

    LOGI(TAG, "Mesh Switch node lpm demo %s", aos_get_app_version());

    app_set_led_state(LED_OFF);

    /* Mesh Models and Device parameter setting, include models event callback regiter */
    ret = mesh_dev_init();

    if (ret) {
        LOGE(TAG, "mesh dev init failed");
    }

    switches_init();

#ifdef CONFIG_BT_MESH_LPM
    switches_lpm_init();
    if (bt_mesh_is_provisioned()) {
        mesh_lpm_start();
    }
#endif

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
