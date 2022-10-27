/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/ble.h"
#include "app_main.h"
#include "app_init.h"
#include "pin_name.h"
#include <yoc/hrs.h>
#include <gpio.h>
#include <pm.h>
#include "pinmux.h"
#include "drv/pmu.h"
#include "drv/gpio.h"
#include <gpio.h>
#include "aos/kv.h"
#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define TAG "DEMO"

#define DEVICE_NAME "YoC HRS LPM"
#define DEVICE_ADDR                                                                                                    \
    {                                                                                                                  \
        0xCC, 0x3B, 0xE3, 0x88, 0xBF, 0xC0                                                                             \
    }
#define MEA_CHA_FLAG         0X00
#define BLE_ADV_INTERVAL_TAG "BLE_ADV_INT"

uint8_t g_hrs_mea_level = 0;

int16_t           g_conn_hanlde = 0xFFFF;
int16_t           adv_onging    = 0;
hrs_t             g_hrs;
hrs_handle_t      g_hrs_handle = NULL;
gpio_pin_handle_t g_wakeup_handler;
aos_timer_t       g_update_timer;
aos_sem_t         g_adv_sem;
uint8_t           mea_data[2]   = { 0, 60 };
uint8_t           g_update_time = 0;

#define HRS_UPDATE_INTERVAL 1000 // ms
#define HRS_UPDATE_MAX_TIME 10
#define GPIO_WAKEUP_IO      P14
#define PIN_FUNC_GPIO       99
#define LPM_FLAG            0XFF
#if (CONFIG_HRS_LPM_STANDBY_TEST > 0)
extern void gpio_wakeup_set(gpio_pin_e pin, gpio_polarity_e type);
static int  wakeup_gpio_init()
{
    int ret = 0;
    drv_pinmux_config(GPIO_WAKEUP_IO, PIN_FUNC_GPIO);
    g_wakeup_handler = csi_gpio_pin_initialize(GPIO_WAKEUP_IO, NULL);

    if (!g_wakeup_handler) {
        LOGE(TAG, "%d gpio init failed", GPIO_WAKEUP_IO);
        return -1;
    }

    ret = csi_gpio_pin_config_mode(g_wakeup_handler, GPIO_MODE_PULLUP);

    if (ret) {
        LOGE(TAG, "%d config mode failed", GPIO_WAKEUP_IO);
        return -1;
    }

    ret = csi_gpio_pin_config_direction(g_wakeup_handler, GPIO_DIRECTION_INPUT);

    if (ret) {
        LOGE(TAG, "%d config direction failed", GPIO_WAKEUP_IO);
        return -1;
    }

    // phy_gpio_wakeup_set(GPIO_WAKEUP_IO,NEGEDGE);
    gpio_wakeup_set(GPIO_WAKEUP_IO, POL_FALLING);

    return 0;
}
#endif

void hrs_update_timer_cb()
{
    g_update_time++;

#if (CONFIG_HRS_LPM_STANDBY_TEST > 0)
    if (g_update_time > HRS_UPDATE_MAX_TIME) {
        LOGI(TAG, "Enter Standby");
        drv_pm_enter_standby();
    }
#endif

    int ret     = 0;
    mea_data[0] = MEA_CHA_FLAG;
    mea_data[1]++;
    ret = ble_prf_hrs_measure_level_update(g_hrs_handle, mea_data, sizeof(mea_data));

    if (ret != 0) {
        LOGD(TAG, "update fail:%d", ret);
    }

    if (mea_data[1] > 180) {
        mea_data[1] = 60;
    }
}

static void hrs_update_timer_init()
{
    aos_timer_new(&g_update_timer, hrs_update_timer_cb, NULL, HRS_UPDATE_INTERVAL, 1);
    aos_timer_stop(&g_update_timer);
}
static void hrs_update_timer_start()
{
    aos_timer_start(&g_update_timer);
}

static void hrs_update_timer_stop()
{
    aos_timer_stop(&g_update_timer);
}

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        g_conn_hanlde = e->conn_handle;
        LOGI(TAG, "Connected");
        g_update_time = 0;
        hrs_update_timer_start();
    } else {
        g_conn_hanlde = 0xFFFF;
        LOGI(TAG, "Disconnected");
        g_update_time = 0;
        hrs_update_timer_stop();
        aos_sem_signal(&g_adv_sem);
        adv_onging = 0;
    }
}

static void conn_param_update(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_param_update_t *e = event_data;

    LOGI(TAG, "LE conn param updated: int 0x%04x lat %d to %d\n", e->interval, e->latency, e->timeout);
}

static void mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = (evt_data_gatt_mtu_exchange_t *)event_data;

    if (e->err == 0) {
        LOGI(TAG, "mtu exchange, MTU %d", ble_stack_gatt_mtu_get(e->conn_handle));
    } else {
        LOGE(TAG, "mtu exchange fail, %d", e->err);
    }
}

static int event_callback(ble_event_en event, void *event_data)
{
    LOGD(TAG, "event %x", event);

    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GAP_CONN_PARAM_UPDATE:
            conn_param_update(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            mtu_exchange(event, event_data);
            break;

        default:
            LOGD(TAG, "Unhandle event %x", event);
            break;
    }

    return 0;
}

static int start_adv(void)
{
    int       ret;
    ad_data_t ad[2] = { 0 };

    uint8_t flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;
    ad[0].type   = AD_DATA_TYPE_FLAGS;
    ad[0].data   = (uint8_t *)&flag;
    ad[0].len    = 1;

    uint8_t uuid16_list[] = { 0x0d, 0x18 }; /* UUID_HRS */
    ad[1].type            = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data            = (uint8_t *)uuid16_list;
    ad[1].len             = sizeof(uuid16_list);

    int adv_interval_min = ADV_FAST_INT_MIN_1;
    int adv_interval_max = ADV_FAST_INT_MAX_1;
    ret                  = aos_kv_getint(BLE_ADV_INTERVAL_TAG, &adv_interval_min);
    if (ret != 0) {
        adv_interval_min = ADV_FAST_INT_MIN_1;
        adv_interval_max = ADV_FAST_INT_MAX_1;
    } else {
        adv_interval_max = adv_interval_min;
    }
    LOGI(TAG, "adv min %d ms, max %d ms!\n", (adv_interval_min * 625 / 1000), (adv_interval_max * 625 / 1000));
    adv_param_t param = {
        ADV_IND, ad, NULL, BLE_ARRAY_NUM(ad), 0, adv_interval_min, adv_interval_max,
    };

    ret = ble_stack_adv_start(&param);

    if (ret) {
        LOGE(TAG, "hrs adv start fail %d!", ret);
    } else {
        LOGI(TAG, "hrs adv start!");
        adv_onging = 1;
    }

    return ret;
}

static ble_event_cb_t ble_cb = {
    .callback = event_callback,
};

int main()
{
    dev_addr_t addr = { DEV_ADDR_LE_RANDOM, DEVICE_ADDR };
    char       node_name[40];
    int        length = 40, ret = -1;

    g_hrs_mea_level = 0;
    g_conn_hanlde   = 0xFFFF;
    adv_onging      = 0;
    g_hrs_handle    = NULL;
    adv_onging      = 0;

    board_yoc_init();

    memset(node_name, 0X00, sizeof(node_name));

    LOGI(TAG, "Bluetooth HRS demo!");
    ret = aos_kv_get("HRS_NAME", node_name, &length);

    if (ret != 0 || strlen(node_name) == 0) {
        sprintf(node_name, DEVICE_NAME);
    }

    LOGI(TAG, "DEV_NAME:%s", node_name);
    init_param_t init = {
        .dev_name     = node_name,
        .dev_addr     = &addr,
        .conn_num_max = 1,
    };
    ble_stack_init(&init);
    ble_stack_setting_load();
    ble_stack_event_register(&ble_cb);
    g_hrs_handle = ble_prf_hrs_init(&g_hrs);

    if (g_hrs_handle == NULL) {
        LOGE(TAG, "HRS init FAIL!!!!");
        return -1;
    }

    hrs_update_timer_init();

#if (CONFIG_HRS_LPM_STANDBY_TEST > 0)
    wakeup_gpio_init();
#endif
    aos_sem_new(&g_adv_sem, 1);

    while (1) {
        aos_sem_wait(&g_adv_sem, AOS_WAIT_FOREVER);

        if (!adv_onging) {
            ret = start_adv();

            if (ret != 0) {
                LOGI(TAG, "adv fail:%d", ret);
                return -1;
            }
        }
    }

    return 0;
}
