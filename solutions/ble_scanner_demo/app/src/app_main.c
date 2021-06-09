/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/ble.h"
#include "app_main.h"
#include "app_init.h"
#include "pin_name.h"

#define TAG "DEMO"
#define CHECK_NAME "YoC HRS"
#define DEVICE_NAME "YoC Test"
#define DEVICE_ADDR {0xCC,0x3B,0xE3,0x88,0xBA,0xC0}

#define EVENT_SCAN  (EVENT_USER + 1)
#define EVENT_DIS_CHAR (EVENT_SCAN + 1)
static int g_conn_handle = -1;
static int scan_flag = 0;

static int g_mtu_exchanged = 0;
static aos_sem_t sync_sem;
char *adv_type_s[] = {
    "ADV_CONNECTABLE",
    "ADV_DIRECT",
    "ADV_SCAN_CONNECTABLE",
    "ADV_UNCONNECTABLE",
    "ADV_DIRECT_LOW_DUTY",
};

static int discovery_state = 0;

enum {
    DISC_IDLE = 0,
    DISC_SRV,
    DISC_CHAR,
    DISC_DES,
};

static void scan_work(const void *arg)
{
    int ret;
    scan_param_t param = {
        SCAN_PASSIVE,
        SCAN_FILTER_DUP_ENABLE,
        SCAN_FAST_INTERVAL,
        SCAN_FAST_WINDOW,
    };

    if (g_conn_handle != -1) {
        return;
    }

    if (scan_flag == 1) {
        return;
    }

    ret = ble_stack_scan_start(&param);

    if (ret) {
        LOGE(TAG, "scan start fail %d!", ret);
    } else {
        LOGI(TAG, "scan start!");
    }

    scan_flag = 1;
}

static uint16_t bas_handle;
static uint16_t bas_end_handle;
static uint16_t bas_char_handle = 0;
static uint16_t bas_ccc_handle = 0;

static void hex_to_str(uint8_t *out, const uint8_t *digest, size_t n)
{
    char const encode[] = "0123456789abcdef";
    int        j        = 0;
    int        i        = 0;

    for (i = 0; i < n; i++) {
        int a    = digest[i];
        out[j++] = encode[(a >> 4) & 0xf];
        out[j++] = encode[a & 0xf];
    }
}

static int check_name(const char *name, uint8_t *ad, int ad_len)
{
    uint16_t len;
    uint8_t type;
    char peer_name[30] = {0};
    const char *value;

    while (ad_len) {
        len = *ad++;
        type = *ad++;
        value = (char *)ad;

        if (len + 1 > ad_len || len == 0) {
            LOGE(TAG, "invaild ad(%d)", len);
            return 0;
        }

        if (type == AD_DATA_TYPE_NAME_SHORTENED || type == AD_DATA_TYPE_NAME_COMPLETE) {
            memcpy(peer_name, value, len - 1);
            peer_name[len] = '\0';

            if (!strcmp(name, peer_name)) {
                LOGI(TAG, "find device %s", peer_name);
                return 1;
            }
        }

        ad_len -= (len + 1);
        ad += (len - 1);
    }

    return 0;
}

static void device_find(ble_event_en event, void *event_data)
{
    evt_data_gap_dev_find_t ee;
    evt_data_gap_dev_find_t *e;

    memcpy(&ee, event_data, sizeof(evt_data_gap_dev_find_t));

    e = &ee;
    char addr_s[36] = {0};
    char adv_data_s[64] = {0};

    if (e->adv_type != ADV_IND) {
        return;
    }

    if (e->adv_len > 31) {
        LOGE(TAG, "invail ad len %d", e->adv_len);
        return;
    }

    snprintf(addr_s, sizeof(addr_s), "%02x:%02x:%02x:%02x:%02x:%02x|(type %d)",
             e->dev_addr.val[5], e->dev_addr.val[4], e->dev_addr.val[3],
             e->dev_addr.val[2], e->dev_addr.val[1], e->dev_addr.val[0], e->dev_addr.type);
    hex_to_str((uint8_t *)adv_data_s, e->adv_data, e->adv_len);

    LOGI(TAG, "find device %s,%s,%s", addr_s, adv_type_s[e->adv_type], adv_data_s);

    if (!check_name(CHECK_NAME, e->adv_data, e->adv_len)) {
        return;
    }

    ble_stack_scan_stop();
    scan_flag = 0;

    conn_param_t param = {
        CONN_INT_MIN_INTERVAL,
        CONN_INT_MAX_INTERVAL,
        0,
        400,
    };

    g_conn_handle = ble_stack_connect(&e->dev_addr, &param, 0);
}

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        LOGI(TAG, "Connected, conn handle %d", e->conn_handle);
        ble_stack_gatt_mtu_exchange(e->conn_handle);
        g_conn_handle = e->conn_handle;
    } else {
        LOGI(TAG, "Disconnected");
        bas_handle = 0;
        bas_end_handle = 0;
        bas_char_handle = 0;
        bas_ccc_handle = 0;
        discovery_state = 0;
        g_conn_handle = -1;
        g_mtu_exchanged = 0;
        scan_flag = 0;
    }
}

static void mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = (evt_data_gatt_mtu_exchange_t *)event_data;

    if (e->err == 0) {
        g_mtu_exchanged = 1;
        aos_sem_signal(&sync_sem);
    } else {
        LOGE(TAG, "mtu exchange fail, %d", e->err);
    }
}

static void service_discovery(ble_event_en event, void *event_data)
{
    char uuid_str[sizeof(uuid_t)] = {0};

    if (event == EVENT_GATT_DISCOVERY_COMPLETE) {
        evt_data_gatt_discovery_complete_t *e = event_data;

        if (discovery_state == DISC_SRV) {
            ble_stack_gatt_discovery_char(e->conn_handle, UUID_HRS_MEASUREMENT, bas_handle + 1, bas_end_handle);
        } else if (discovery_state == DISC_CHAR) {
            ble_stack_gatt_discovery_descriptor(e->conn_handle, UUID_GATT_CCC, bas_char_handle + 1, 0xffff);
        } else if (discovery_state == DISC_DES) {
            uint16_t notify_enable = CCC_VALUE_NOTIFY;
            ble_stack_gatt_write_response(e->conn_handle, bas_ccc_handle, &notify_enable, sizeof(notify_enable), 0);
            LOGI(TAG, "HRS service discovery complete");
            return;
        }
    }

    if (event == EVENT_GATT_DISCOVERY_SVC) {
        evt_data_gatt_discovery_svc_t *e = event_data;
        hex_to_str((uint8_t *)uuid_str, get_uuid_val(&(e->uuid)), UUID_LEN(&e->uuid));  //(uint8_t *)&e->uuid.val
        discovery_state = DISC_SRV;

        if (UUID_EQUAL(&e->uuid, UUID_HRS)) {
            bas_handle = e->start_handle;
            bas_end_handle = e->end_handle;
            LOGI(TAG, "find service HRS, start handle %x, end handle %x", e->start_handle, e->end_handle);
        }
    }

    if (event == EVENT_GATT_DISCOVERY_CHAR) {
        evt_data_gatt_discovery_char_t *e = event_data;
        bas_char_handle = e->val_handle;
        discovery_state = DISC_CHAR;
        LOGI(TAG, "HRS CHAR handle %x, val handle %x", e->attr_handle, e->val_handle);
    }

    if (event == EVENT_GATT_DISCOVERY_CHAR_DES) {
        evt_data_gatt_discovery_char_des_t *e = event_data;
        bas_ccc_handle = e->attr_handle;
        discovery_state = DISC_DES;
        hex_to_str((uint8_t *)uuid_str, get_uuid_val(&(e->uuid)), UUID_LEN(&e->uuid));
        LOGI(TAG, "HRS DES handle %x, UUID %s", e->attr_handle, uuid_str);
    }
}

static int event_callback(ble_event_en event, void *event_data)
{
    //LOGI(TAG, "event %d\n", event);

    switch (event) {
        case EVENT_GAP_DEV_FIND:
            device_find(event, event_data);
            break;

        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            mtu_exchange(event, event_data);
            break;

        case EVENT_GATT_DISCOVERY_SVC:
        case EVENT_GATT_DISCOVERY_CHAR:
        case EVENT_GATT_DISCOVERY_CHAR_DES:
        case EVENT_GATT_DISCOVERY_COMPLETE:
            service_discovery(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE_CB: {
            evt_data_gatt_write_cb_t *e = event_data;
            LOGI(TAG, "GATT write %s", e->err ? "FAIL" : "SUCCESS");
            break;
        }

        case EVENT_GATT_CHAR_READ_CB: {
            evt_data_gatt_read_cb_t *e = event_data;
            char val_str[10] = {0};
            hex_to_str((uint8_t *)val_str, (uint8_t *)e->data, e->len);
            LOGI(TAG, "GATT read %s", val_str);
            break;
        }

        case EVENT_GATT_NOTIFY: {
            evt_data_gatt_notify_t *e = event_data;
            char val_str[10] = {0};
            hex_to_str((uint8_t *)val_str, (uint8_t *)e->data, e->len);
            LOGI(TAG, "GATT notify %s", val_str);
            break;
        }

        default:
            LOGW(TAG, "Unhandle event %x\n", event);
            break;
    }

    return 0;
}

static ble_event_cb_t ble_cb = {
    .callback = event_callback,
};


int main()
{
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR};
    init_param_t init = {
        .dev_name = DEVICE_NAME,
        .dev_addr = &addr,
        .conn_num_max = 1,
    };

    board_yoc_init();

    g_mtu_exchanged = 0;
    g_conn_handle = -1;
    scan_flag = 0;
    LOGI(TAG, "Bluetooth scanner demo!");
    aos_sem_new(&sync_sem, 0);

    ble_stack_init(&init);
    ble_stack_event_register(&ble_cb);

    scan_work(NULL);

    while (1) {
        aos_sem_wait(&sync_sem, 2000);

        if (g_mtu_exchanged) {
            ble_stack_gatt_discovery_primary(g_conn_handle, UUID_HRS, 0x0001, 0xFFFF);
            g_mtu_exchanged = 0;
        }

        if (g_conn_handle == -1) {
            scan_work(NULL);
        }
    }

    return 0;
}

