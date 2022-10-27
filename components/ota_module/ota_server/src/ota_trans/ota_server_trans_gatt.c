/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_OTA_SERVER_TRANS_GATT) && CONFIG_OTA_SERVER_TRANS_GATT
#include <stdio.h>
#include <errno.h>
#include <ble_os.h>
#include <aos/debug.h>
#include <aos/kernel.h>
#include <aos/ble.h>
#include <aos/aos.h>
#include "yoc/ais_client.h"
#include "ota_server.h"
#include "bluetooth/conn.h"
#include "common/log.h"
#include "yoc/ais_client.h"
#include "mesh/access.h"
#include "mesh/main.h"
#include "ota_trans/ota_server_trans_gatt.h"
#include "inc/net.h"
#include "inc/proxy.h"
#include "inc/adv.h"
#include "ais_ota/ais_ota_server.h"
#include "ais_ota.h"
#include "timer_port.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "inc/provisioner_proxy.h"
#include "inc/provisioner_main.h"
#endif
#define TAG "OTA_TRANS_GATT"

ota_beacon_cb g_ota_service_cb;
extern void   device_list_mutex_lock();
extern void   device_list_mutex_unlock();

enum
{
    CONN_FLAG_DISCONNECTED = 0x00,
    CONN_FLAG_CONN         = 0x01,
    CONN_FLAG_CONNECTED    = 0x02,
};

typedef struct {
    uint8_t             ais_gatt_init_flag;
    ais_handle_t        ais_handle;
    slist_t *           dev_list;
    ota_timer_t         ais_status_timeout;
    aos_sem_t           ais_conn_sem;
    ota_server_event_cb ota_server_cb;
    uint8_t             conn_flag;
} ota_server_trans_gatt_ctx;

#define DEF_AIS_TRANS_GATT_TIMEOUT     10000 // 10s
#define DEF_AIS_WAIT_DISCONECT_TIMEOUT 10000 // 10s

ota_server_trans_gatt_ctx g_server_trans_gatt_ctx;

static void _ble_scan_cb(const struct bt_le_scan_recv_info *info, struct net_buf *buf)
{
    dev_addr_t addr_cmp;
    memcpy(addr_cmp.val, info->addr->a.val, 6);
    addr_cmp.type             = info->addr->type;
    upgrade_device *up_device = NULL;
    // slist_t *tmp;
    uint8_t found_device_flag = 0;
    int     ret               = 0;

    if (g_server_trans_gatt_ctx.conn_flag != CONN_FLAG_DISCONNECTED) {
        return;
    }

    up_device = dev_list_dev_get(g_server_trans_gatt_ctx.dev_list);
    if (!up_device) {
        return;
    }

    do {
        if (!memcmp(&addr_cmp, &up_device->device.addr, sizeof(dev_addr_t))
            && (up_device->ota_state == OTA_STATE_IDLE || up_device->ota_state == OTA_STATE_LOAD_COMPLETE)
            && up_device->conect_failed_count < DEF_MAX_CONN_RETRY)
        {
            LOGD(TAG, "Found the device");
            found_device_flag = 1;
            break;
        }
        up_device = dev_list_dev_get(&up_device->list);
    } while (up_device);

    if (found_device_flag) {
        ret = ota_server_trans_gatt_connect(&addr_cmp);
        if (!ret) {
            ota_timer_stop(&g_server_trans_gatt_ctx.ais_status_timeout);
            ota_timer_start(&g_server_trans_gatt_ctx.ais_status_timeout, DEF_AIS_TRANS_GATT_TIMEOUT);
        } else {
            up_device->conect_failed_count++;
            if (up_device->conect_failed_count >= DEF_MAX_CONN_RETRY) {
                if (g_server_trans_gatt_ctx.ota_server_cb) {
                    g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_SERVER_STATE_CHAN_CREATE_CONN_FAIL, up_device);
                }
            }
        }
    } // TODO connect retry
}

static void _ota_msg_handle(ais_pdu_t *p_msg, uint16_t len)
{
    LOGD(TAG, "cmd %02x", p_msg->header.cmd);
    // LOGD(TAG,"len %d: %s", len, bt_hex(p_msg, len));

    switch (p_msg->header.cmd) {
        case AIS_OTA_VER_RESP:
            if (g_server_trans_gatt_ctx.ota_server_cb) {
                g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_VER_RESP, p_msg);
            }
            break;
        case AIS_OTA_UPD_RESP:
            if (g_server_trans_gatt_ctx.ota_server_cb) {
                g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_UPD_RESP, p_msg);
            }
            break;
        case AIS_OTA_STATUS:
            if (g_server_trans_gatt_ctx.ota_server_cb) {
                g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_STATUS, p_msg);
            }
            break;
        case AIS_OTA_CHECK_RESP:
            if (g_server_trans_gatt_ctx.ota_server_cb) {
                g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_CHECK_RESP, p_msg);
            }
            break;
        case AIS_RESP_ERR: {
            if (g_server_trans_gatt_ctx.ota_server_cb) {
                g_server_trans_gatt_ctx.ota_server_cb(AIS_RESP_ERR, p_msg);
            }
        } break;
        default:
            /* recv some unsupport cmd, just return */
            LOGW(TAG, "unsupport cmd %x", p_msg->header.cmd);
            return;
    }
}

#if defined(CONFIG_BT_MESH)
static int mesh_connect_suspend()
{
    int ret = 0;
    LOGD(TAG, "mesh connect suspend");
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    if (bt_mesh_is_provisioned()) {
        ret = bt_mesh_proxy_gatt_disable();
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "mesh proxy gatt disable fail %d", ret);
            return -1;
        }
    } else {
        ret = bt_mesh_proxy_prov_disable(false);
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "mesh proxy gatt prov disable fail %d", ret);
            return -1;
        }
    }
#if defined(CONFIG_BT_MESH_PROVISIONER) && defined(CONFIG_BT_MESH_PB_GATT)
    if (bt_mesh_is_provisioner_en()) {
        ret = bt_mesh_provisioner_proxy_disable();
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "mesh provisioner proxy  disable fail %d", ret);
            return -1;
        }
    }
#endif
    bt_mesh_adv_update();

    ret = bt_mesh_scan_disable();
    if (ret < 0 && ret != -EALREADY) {
        LOGE(TAG, "mesh scan disable fail");
        return -1;
    }

    aos_msleep(10);
    return 0;
#endif
}

static int mesh_connect_resume()
{
    int ret = 0;
    LOGD(TAG, "mesh connect resume");
    bt_mesh_adv_update();
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    if (bt_mesh_is_provisioned()) {
        ret = bt_mesh_proxy_gatt_enable();
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "mesh proxy gatt enable fail %d", ret);
            return -1;
        }
    } else {
        ret = bt_mesh_proxy_prov_enable();
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "mesh proxy gatt prov enable fail %d", ret);
            return -1;
        }
    }
#if defined(CONFIG_BT_MESH_PROVISIONER) && defined(CONFIG_BT_MESH_PB_GATT)
    if (bt_mesh_is_provisioner_en()) {
        ret = bt_mesh_provisioner_proxy_enable();
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "mesh provisioner proxy  enable fail %d", ret);
            return -1;
        }
    }
#endif
    bt_mesh_adv_disable();
    bt_mesh_scan_disable();
    // enable scan again
    bt_mesh_scan_enable();
#endif
    return 0;
}

static int mesh_connected_handler()
{
    // make sure adv and scan stop first
    bt_mesh_adv_disable();
    bt_mesh_scan_disable();
    // enable scan again
    bt_mesh_scan_enable();
    return 0;
}
#endif

static void ais_event_cb(uint8_t event, const void *event_data, uint32_t length)
{
    LOGD(TAG, "ais event:%02x", event);
    switch (event) {
        case AIS_EVENT_CONN: {
            connect_status conn_status = *(connect_status *)event_data;
            if (conn_status.status) {
                LOGE(TAG, "trans gatt connect fail %02x", conn_status.status);
#if defined(CONFIG_BT_MESH)
                mesh_connect_resume();
#endif
                g_server_trans_gatt_ctx.conn_flag = CONN_FLAG_DISCONNECTED;
                ota_timer_stop(&g_server_trans_gatt_ctx.ais_status_timeout);

                if (g_server_trans_gatt_ctx.ota_server_cb) {
                    g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_SERVER_STATE_CHAN_CREATE_CONN_FAIL,
                                                          &conn_status.addr);
                }

            } else {
#if defined(CONFIG_BT_MESH)
                mesh_connected_handler();
#endif
                g_server_trans_gatt_ctx.conn_flag = CONN_FLAG_CONN;
                if (g_server_trans_gatt_ctx.ota_server_cb) {
                    g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_SERVER_STATE_CHAN_CONN, &conn_status);
                }
            }

        } break;
        case AIS_EVENT_DISCONN: {
            ota_timer_stop(&g_server_trans_gatt_ctx.ais_status_timeout);
#if defined(CONFIG_BT_MESH)
            mesh_connect_resume();
#endif
            g_server_trans_gatt_ctx.conn_flag = CONN_FLAG_DISCONNECTED;

            if (g_server_trans_gatt_ctx.ota_server_cb) {
                g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_SERVER_STATE_CHAN_DISCONN, event_data);
                g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_SERVER_STATE_CHAN_CREATE_CONN_FAIL, event_data);
            }

            if (g_server_trans_gatt_ctx.ais_gatt_init_flag) { // gatt trans has been ungrister
                aos_sem_signal(&g_server_trans_gatt_ctx.ais_conn_sem);
            }

        } break;
        case AIS_EVENT_MTU_EXCHANGE:
            if (*(uint8_t *)event_data <= 3) {
                LOGE(TAG, "invaild mtu size %d,it should be larger than 3", *(uint8_t *)event_data);
            } else {
                uint8_t real_size = *(uint8_t *)event_data - 3;
                if (g_server_trans_gatt_ctx.ota_server_cb) {
                    g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_SERVER_STATE_CHAN_READY, &real_size);
                }
            }
            break;
        case AIS_EVENT_DISCOVER_SUCCEED:
            ota_timer_stop(&g_server_trans_gatt_ctx.ais_status_timeout);
            if (g_server_trans_gatt_ctx.ota_server_cb) {
                g_server_trans_gatt_ctx.ota_server_cb(AIS_OTA_SERVER_STATE_ONGOING, event_data);
            }
            break;
        case AIS_EVENT_NOTIFY: {
            _ota_msg_handle((ais_pdu_t *)(event_data), length);
        } break;
        default:
            break;
    }

    return;
}

static void ota_server_trans_gatt_timeout(void *timer, void *arg)
{
    int ret = 0;
    LOGE(TAG, "ais gatt discover timeout");
    ota_timer_stop(&g_server_trans_gatt_ctx.ais_status_timeout);
    ota_server_trans_gatt_disconnect();
    if (ret) {
        LOGE(TAG, "gatt disconnect fail %d", ret);
    }
}

struct bt_le_scan_cb scan_cb = {
    .buf_recv = _ble_scan_cb,
    .timeout  = NULL,
};

int ota_server_trans_gatt_init(ota_server_event_cb cb)
{
    if (g_server_trans_gatt_ctx.ais_gatt_init_flag) {
        return -EALREADY;
    }
    memset(&g_server_trans_gatt_ctx, 0x00, sizeof(ota_server_trans_gatt_ctx));

    g_server_trans_gatt_ctx.ais_handle = ble_prf_ais_client_init(ais_event_cb);
    if (!g_server_trans_gatt_ctx.ais_handle) {
        return -1;
    }

    bt_le_scan_cb_register(&scan_cb);
    ota_timer_init(&g_server_trans_gatt_ctx.ais_status_timeout, ota_server_trans_gatt_timeout, NULL);
    aos_sem_new(&g_server_trans_gatt_ctx.ais_conn_sem, 0);
    g_server_trans_gatt_ctx.ota_server_cb      = cb;
    g_server_trans_gatt_ctx.ais_gatt_init_flag = 1;
    return 0;
}

int ota_server_trans_gatt_connect(dev_addr_t *dev)
{
    conn_param_t param = {
        CONN_INT_MIN_INTERVAL,
        CONN_INT_MAX_INTERVAL,
        0,
        400,
    };

    int ret = 0;

#if defined(CONFIG_BT_MESH)
    mesh_connect_suspend();
#endif

    ret = ble_prf_ais_client_conn(g_server_trans_gatt_ctx.ais_handle, dev, &param);
    if (ret) {
#if defined(CONFIG_BT_MESH)
        mesh_connect_resume();
#endif
        LOGE(TAG, "ais client conn create failed %d", ret);
        return -1;
    }

    g_server_trans_gatt_ctx.conn_flag = CONN_FLAG_CONN;

    return 0;
}

int ota_server_trans_gatt_disconnect()
{
    return ble_prf_ais_client_disconn(g_server_trans_gatt_ctx.ais_handle);
}

int ota_server_trans_gatt_prepare(slist_t *dev_list)
{
    int ret;
    if (!dev_list || !dev_list_dev_get(dev_list)) {
        LOGE(TAG, "dev_list is null");
        return -EINVAL;
    }

    ret = bt_mesh_scan_enable();
    if (ret < 0 && ret != -EALREADY) {
        LOGE(TAG, "gatt prepare start scan failed");
        return -1;
    }

    g_server_trans_gatt_ctx.dev_list = dev_list;

    return 0;
}

int ota_server_trans_gatt_unregister()
{
    int ret = 0;
    if (g_server_trans_gatt_ctx.conn_flag != CONN_FLAG_DISCONNECTED) {
        LOGD(TAG, "Gatt unregister,disconnect direct");
        ret = ota_server_trans_gatt_disconnect();
        if (ret && ret != -ENOTCONN) {
            LOGE(TAG, "gatt unregister disconnect fail %d", ret);
            goto unregister;
        }

        LOGD(TAG, "Wait for gatt disconnect");
        aos_sem_wait(&g_server_trans_gatt_ctx.ais_conn_sem, DEF_AIS_WAIT_DISCONECT_TIMEOUT);
    } else {
        LOGD(TAG, "Gatt unregister,unreister direct");
        goto unregister;
    }

unregister:
    g_server_trans_gatt_ctx.ais_gatt_init_flag = 0;
    LOGD(TAG, "Gatt Unregister");
    ble_prf_ais_client_unregister();
#if defined(CONFIG_BT_MESH)
    mesh_connect_resume();
#endif
    ota_timer_free(&g_server_trans_gatt_ctx.ais_status_timeout);
    aos_sem_free(&g_server_trans_gatt_ctx.ais_conn_sem);
    memset(&g_server_trans_gatt_ctx, 0x00, sizeof(ota_server_trans_gatt_ctx));
    return 0;
}

int ota_server_trans_gatt_send(uint8_t *data, uint8_t len, uint8_t ack)
{
    // LOGD(TAG,"Send:%d %p %s",len,data,hex_dump(data,len));
    if (ack) {
        return ble_prf_ais_client_send(g_server_trans_gatt_ctx.ais_handle,
                                   g_server_trans_gatt_ctx.ais_handle->ais_profile.write_char_handle,
                                   (const char *)data, len, GATT_WRITE, NULL);
    } else {
        return ble_prf_ais_client_send(g_server_trans_gatt_ctx.ais_handle,
                                   g_server_trans_gatt_ctx.ais_handle->ais_profile.write_char_handle,
                                   (const char *)data, len, GATT_WRITE_WITHOUT_RESPONSE, NULL);
    }
}

#endif
