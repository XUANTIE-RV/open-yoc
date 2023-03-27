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
#include "mesh_lpm.h"

#define TAG "TEMPERATURE_SENSOR_DEMO"

#ifndef CONFIG_BT_MESH_SLEEP_DELAY
#define CONFIG_BT_MESH_SLEEP_DELAY 180000
#endif

#define MESH_LPM_SLEEP_TIME  (120000) // unit:ms
#define MESH_LPM_WAKEUP_TIME (60)     // unit:ms
#define TEMP_MAX (100)
#define TEM_MIN  (-20)

struct bt_mesh_model *g_level_model = NULL;

static int16_t get_temperature_data()
{
    static int16_t temp = 0;
    if (temp >= TEMP_MAX) {
        temp = TEM_MIN;
    }
    return temp++;
}

#ifdef CONFIG_BT_MESH_LPM
__attribute__((section(".__sram.code"))) static void temperature_lpm_cb(mesh_lpm_wakeup_reason_e reason,
                                                                        mesh_lpm_status_e status, void *arg)
{
    int ret = 0;
    if (status == STATUS_WAKEUP) {
        if (reason == WAKEUP_BY_TIMER) {
            ret = ble_mesh_generic_level_publication(g_level_model, get_temperature_data());
            if (ret) {
                LOGE(TAG, "pub level failed %d", ret);
            }
        }
    } else {
        LOGI(TAG, "sleep");
    }
}
#endif

static void temperature_senosr_lpm_init()
{
#ifdef CONFIG_BT_MESH_LPM
    mesh_lpm_conf_t lpm_config;
    lpm_config.is_auto_enable                    = 1;
    lpm_config.lpm_mode                          = MESH_LPM_MODE_TX_ONLY;
    lpm_config.lpm_wakeup_mode                   = WAKEUP_BY_TIMER_MODE;
    lpm_config.delay_sleep_time                  = CONFIG_BT_MESH_SLEEP_DELAY; // Unit:ms
    lpm_config.lpm_wakeup_timer_config.sleep_ms  = MESH_LPM_SLEEP_TIME;
    lpm_config.lpm_wakeup_timer_config.wakeup_ms = MESH_LPM_WAKEUP_TIME;
    lpm_config.mesh_lpm_cb                       = temperature_lpm_cb;
    mesh_lpm_init(&lpm_config);
#endif
    return;
}

int main()
{
    int ret;

    /* Board Hardware and yoc modules init */
    board_yoc_init();

    LOGI(TAG, "Mesh Temperature Sensor node demo %s", aos_get_app_version());

    app_set_led_state(LED_OFF);

    /* Mesh Models and Device parameter setting, include models event callback regiter */
    ret = mesh_dev_init();

    if (ret) {
        LOGE(TAG, "mesh dev init failed");
    }

#ifdef CONFIG_BT_MESH_LPM
    temperature_senosr_lpm_init();
    if (bt_mesh_is_provisioned()) {
        mesh_lpm_start();
    }
#endif

    g_level_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_LEVEL_SRV, CID_NVAL);
    if (!g_level_model) {
        LOGE(TAG, "level model not found");
    }

    return 0;
}
