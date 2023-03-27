/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <aos/kernel.h>
#include "common/log.h"
#include "gateway_mgmt.h"
#include "gateway_mgmt_offline.h"
#include "gateway_settings.h"
#include "gateway_port.h"

#define TAG "GW_OFFLINE"

typedef struct {
    uint64_t         last_check_time;
    uint8_t          check_num;
    gw_subdev_ctx_t *node;
} active_check_node_t;

enum
{
    ACTIVE_CHECK_TIMER_STOP,
    ACTIVE_CHECK_TIMER_RUN,
};

typedef enum
{
    ACTIVE_CHECK_SUCCESS   = 0x00,
    ACTIVE_CHECK_FAIL      = 0x01,
    ACTIVE_CHECK_NOT_START = 0x02,
} active_check_state_en;

typedef struct {
    aos_mutex_t         list_mutex;
    aos_timer_t         hb_check_timer;
    aos_timer_t         active_check_timer;
    uint8_t             active_timer_state; // 0 STOP 1 run
    active_check_node_t nodes[GW_ACTIVE_CHECK_NODE_MAX_NUM];
} gw_subdev_active_check_ctx_t;

gw_subdev_active_check_ctx_t g_active_check_ctx;

static int _gateway_subdev_active_check_node_add(gw_subdev_ctx_t *dev)
{
    int      i     = 0;
    int      ret   = 0;
    uint16_t index = 0xFFFF;

    aos_mutex_lock(&g_active_check_ctx.list_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < ARRAY_SIZE(g_active_check_ctx.nodes); i++) {
        if (g_active_check_ctx.nodes[i].node == dev) {
            aos_mutex_unlock(&g_active_check_ctx.list_mutex);
            return 0;
        }
        if (index == 0xFFFF && !g_active_check_ctx.nodes[i].node) {
            index = i;
        }
    }
    aos_mutex_unlock(&g_active_check_ctx.list_mutex);

    if (index == 0xFFFF) {
        return -ENOBUFS;
    }

    aos_mutex_lock(&g_active_check_ctx.list_mutex, AOS_WAIT_FOREVER);
    g_active_check_ctx.nodes[index].node        = dev;
    g_active_check_ctx.nodes[index].node->state = GW_NODE_ACTIVE_CHECK;
    g_active_check_ctx.nodes[index].check_num   = 0;
    aos_mutex_unlock(&g_active_check_ctx.list_mutex);
    LOGD(TAG, "Active check node add index:%d %d", g_active_check_ctx.nodes[index].node->subdev, index);
    if (g_active_check_ctx.active_timer_state == ACTIVE_CHECK_TIMER_STOP) {
        ret = aos_timer_start(&g_active_check_ctx.active_check_timer);
        if (ret) {
            LOGE(TAG, "active check timer start failed %d", ret);
        }
    }

    return 0;
}

static int _gateway_subdev_active_check_node_remove(gw_subdev_ctx_t *dev)
{
    int i = 0;
    aos_mutex_lock(&g_active_check_ctx.list_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < ARRAY_SIZE(g_active_check_ctx.nodes); i++) {
        if (g_active_check_ctx.nodes[i].node == dev) {
            g_active_check_ctx.nodes[i].node = NULL;
            aos_mutex_unlock(&g_active_check_ctx.list_mutex);
            return 0;
        }
    }
    aos_mutex_unlock(&g_active_check_ctx.list_mutex);
    return -1;
}

static int _is_node_need_active_check(gw_subdev_ctx_t *node, uint64_t recv_intervial, uint32_t update_intervial)
{
    if (node->dev_type.lpm_type == GW_DEV_NO_LPM && recv_intervial > GW_HB_CHECK_MAX_LOSS * update_intervial
        && node->state != GW_NODE_NOT_ACTIVE)
    {
        return 1;
    } else if (node->dev_type.lpm_type == GW_DEV_LPM_RX_TX && recv_intervial > update_intervial
               && node->state != GW_NODE_NOT_ACTIVE)
    {
        /* lpm node will not be rechecked when not active */
        return 1;
    }
    return 0;
}

static int _gateway_subdev_hb_period_update(gw_subdev_ctx_t *subdev_ctx, uint16_t period_s)
{
    if (subdev_ctx->period != period_s) {
        LOGD(TAG, "Update Subdev:%d hb period %ds ---> %ds", subdev_ctx->subdev, subdev_ctx->period, period_s);
        subdev_ctx->period = period_s;
        gateway_subdev_ctx_store(subdev_ctx);
    }

    return 0;
}

static void _gateway_subdev_active_check_timer(void *timer, void *arg)
{
    int     i                    = 0;
    int     ret                  = 0;
    uint8_t no_faults_check_flag = 1;

    long long time_now = aos_now_ms();

    aos_mutex_lock(&g_active_check_ctx.list_mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < ARRAY_SIZE(g_active_check_ctx.nodes); i++) {
        active_check_node_t *check_node = &g_active_check_ctx.nodes[i];
        if (check_node->node) {
            uint16_t check_interval = 0;
            if (check_node->node->dev_type.lpm_type == GW_DEV_NO_LPM) {
                check_interval = GW_ACTIVE_CHECK_INTERVAL;
            } else {
                check_interval = check_node->node->period * 1000;
            }

            if ((time_now - check_node->last_check_time) >= check_interval) {
                if (check_node->check_num < GW_ACTIVE_CHECK_MAX_RETRY_NUM) {
                    LOGD(TAG, "Node %d check num:%d", check_node->node->subdev, check_node->check_num);
                    gateway_check_subdev_msg_t check_msg = {
                        .subdev = check_node->node->subdev,
                        .period = check_node->node->period,
                    };

                    ret = gateway_mgmt_send_process_msg(GW_SUBDEV_ACTIVE_CHECK_EVENT, (void *)&check_msg);
                    if (ret) {
                        LOGE(TAG, "subdev active check process send failed %d", ret);
                    }
                    check_node->check_num++;
                    check_node->last_check_time = time_now;
                    break;
                } else {
                    LOGD(TAG, "Node %d active check failed", check_node->node->subdev);
                    check_node->check_num       = 0;
                    check_node->last_check_time = 0;
                    if (check_node->node->dev_type.lpm_type == GW_DEV_LPM_RX_TX) {
                        /* this will enable active recheck after the period */
                        check_node->node->hb_last_recv = time_now;
                    }
                    gateway_subdev_state_update(check_node->node, GW_NODE_NOT_ACTIVE,
                                                GW_SUBDEV_STATUS_CHANGE_BY_ACTIVE_CHECK_FAIL);
                    check_node->node = NULL;
                }
            }
            no_faults_check_flag = 0;
        }
    }
    aos_mutex_unlock(&g_active_check_ctx.list_mutex);
    if (no_faults_check_flag) {
        ret = aos_timer_stop(&g_active_check_ctx.active_check_timer);
        if (ret) {
            LOGE(TAG, "Active check timer stop failed %d", ret);
        }
        g_active_check_ctx.active_timer_state = ACTIVE_CHECK_TIMER_STOP;
    }
}

static int _gateway_subdev_active_check(gw_subdev_ctx_t *subdev_ctx, uint16_t check_index)
{
    int      ret              = 0;
    uint64_t recv_intervial   = aos_now_ms() - subdev_ctx->hb_last_recv;
    uint32_t update_intervial = subdev_ctx->period * 1000;

    if (subdev_ctx->state != GW_NODE_ACTIVE_CHECK && subdev_ctx->period != GW_HB_PERIOD_NOT_CHECK) {
        if (_is_node_need_active_check(subdev_ctx, recv_intervial, update_intervial)) {
            ret = _gateway_subdev_active_check_node_add(subdev_ctx);
            if (ret) {
                return ACTIVE_CHECK_FAIL;
            }
            return ACTIVE_CHECK_SUCCESS;
        }
    }
    return ACTIVE_CHECK_NOT_START;
}

static void _gateway_subdev_hb_check_timer(void *timer, void *arg)
{
    int             ret         = 0;
    static uint16_t check_index = 1;

    for (; check_index < CONFIG_GW_MAX_DEVICES_META_NUM; check_index++) {
        gw_subdev_ctx_t *ctx = gateway_subdev_get_ctx_by_index(check_index);
        if (ctx == NULL)
            continue;
        ret = _gateway_subdev_active_check(ctx, check_index);
        if (ret == ACTIVE_CHECK_FAIL) {
            break;
        } else if (ret == ACTIVE_CHECK_SUCCESS) {
            check_index++;
            break;
        }
    }

    if (check_index == CONFIG_GW_MAX_DEVICES_META_NUM) {
        check_index = 1;
    }
}

/* user api */

int gateway_mgmt_offline_check_init()
{
    int ret = 0;

    memset(&g_active_check_ctx, 0x0, sizeof(gw_subdev_active_check_ctx_t));

    ret = aos_timer_new_ext(&g_active_check_ctx.active_check_timer, _gateway_subdev_active_check_timer, NULL,
                            GW_ACTIVE_CHECK_INTERVAL, 1, 0);
    if (ret) {
        LOGE(TAG, "Init faults check timer failed %d", ret);
        return -EIO;
    }
    ret = aos_mutex_new(&g_active_check_ctx.list_mutex);
    if (ret) {
        LOGE(TAG, "List mutex init timer  %d", ret);
        return -EIO;
    }

    g_active_check_ctx.active_timer_state = ACTIVE_CHECK_TIMER_STOP;

    ret = aos_timer_new_ext(&g_active_check_ctx.hb_check_timer, _gateway_subdev_hb_check_timer, NULL,
                            GW_HB_CHECK_INTERVAL, 1, 0);
    if (ret) {
        LOGE(TAG, "Init node hb check timer failed %d", ret);
    }

    return ret;
}

int gateway_default_offline_check_timeout_get(uint8_t lpm_type)
{
#if defined(CONFIG_GATEWAY_SUBDEV_OFFLINE_CHECK) && CONFIG_GATEWAY_SUBDEV_OFFLINE_CHECK > 0
    switch (lpm_type) {
        case GW_DEV_NO_LPM: {
            return GW_DEF_HB_SUBDEV_CHECK_PERIOD;
        } break;
        case GW_DEV_LPM_RX_TX: {
            return GW_DEF_HB_LPM_SUBDEV_CHECK_PERIOD;
        } break;
        default:
            return GW_HB_PERIOD_NOT_CHECK;
    }
#else
    return GW_HB_PERIOD_NOT_CHECK;
#endif
}

void gateway_subdev_hb_recv(gw_subdev_ctx_t *subdev_ctx)
{
    if (!subdev_ctx) {
        LOGE(TAG, "Invalid subdev ctx");
        return;
    }
    subdev_ctx->hb_last_recv = aos_now_ms();
    if (subdev_ctx->state == GW_NODE_ACTIVE_CHECK || subdev_ctx->state == GW_NODE_NOT_ACTIVE) {
        gateway_subdev_state_update(subdev_ctx, GW_NODE_ACTIVE, GW_SUBDEV_STATUS_CHANGE_BY_HB_RECV);
        if (subdev_ctx->state == GW_NODE_ACTIVE_CHECK) {
            _gateway_subdev_active_check_node_remove(subdev_ctx);
        }
    }
}

void gateway_subdev_active_status_recv(gw_subdev_ctx_t *subdev_ctx, gw_active_check_status_t active_status)
{
    if (!subdev_ctx) {
        LOGE(TAG, "Get subdev by mac failed");
        return;
    }

    _gateway_subdev_active_check_node_remove(subdev_ctx);
    subdev_ctx->hb_last_recv = aos_now_ms();

    if (atomic_test_bit(&active_status.status, NODE_STATUS_HB_NOT_ENABLE)) {
        if (!atomic_test_bit(&active_status.status, NODE_STATUS_IN_LPM)) {
            LOGE(TAG, "Subdev:%d hb not enable, period_s:%04x", subdev_ctx->subdev, subdev_ctx->period);
            if (subdev_ctx->period != 0xFFFF) {
                gateway_subdev_state_update(subdev_ctx, GW_NODE_NOT_ACTIVE, GW_SUBDEV_STATUS_CHANGE_BY_HB_NOT_ENABLE);
            } else {
                LOGD(TAG, "Node hb check has been disabled, Ignore it");
            }
        } else {
            gateway_subdev_state_update(subdev_ctx, GW_NODE_ACTIVE, GW_SUBDEV_STATUS_CHANGE_BY_ACTIVE_CHECK_SUCCESS);
        }
    } else {
        if (atomic_test_bit(&active_status.status, NODE_STATUS_HB_PARAM_ERR)) {
            LOGE(TAG, "Subdev:%d hb param err", subdev_ctx->subdev);
            gateway_subdev_state_update(subdev_ctx, GW_NODE_NOT_ACTIVE, GW_SUBDEV_STATUS_CHANGE_BY_HB_ERR_PARAM);
        }
        if (atomic_test_bit(&active_status.status, NODE_STATUS_HB_PERIOD_MISS)) {
            LOGE(TAG, "Subdev:%d hb period miss", subdev_ctx->subdev);
            _gateway_subdev_hb_period_update(subdev_ctx, active_status.node_period);
        }
    }
}

int gateway_subdev_hb_pub_status_update(gw_subdev_ctx_t *subdev_ctx, gw_subdev_hb_set_status_t hb_set_status)
{
    uint16_t period_last = 0;

    if (!subdev_ctx) {
        LOGE(TAG, "Infailed Subdev ctx");
        return -EINVAL;
    }

    period_last = subdev_ctx->period;

    if (hb_set_status.status == GW_HB_ENABLE_SUCCESS || hb_set_status.status == GW_HB_ENABLE_FAILED) {
        LOGD(TAG, "Update Dev %d hb period %d ---> %d", subdev_ctx->subdev, subdev_ctx->period, hb_set_status.period_s);
        subdev_ctx->period = hb_set_status.period_s;
    } else {
        subdev_ctx->period = 0xFFFF;
    }

    if (period_last != subdev_ctx->period) {
        gateway_subdev_ctx_store(subdev_ctx);
    }
    return 0;
}

gw_status_t gateway_heartbeat_mgmt_enable(uint8_t enable)
{
    int ret = 0;

    ret = gateway_porting_gw_hb_recv_enable(1);
    if (ret) {
        LOGE(TAG, "Gateway hb recv %s failed %d", enable ? "enable" : "disable", ret);
        return ret;
    }

    if (enable) {
        ret = aos_timer_start(&g_active_check_ctx.hb_check_timer);
        if (ret) {
            LOGE(TAG, "Gateway hb check timer start failed %d", ret);
            return -ENOMEM;
        }
    } else {
        ret = aos_timer_stop(&g_active_check_ctx.hb_check_timer);
        if (ret) {
            LOGE(TAG, "Gateway hb check timer stop failed %d", ret);
            return EINVAL;
        }
        ret = aos_timer_stop(&g_active_check_ctx.active_check_timer);
        if (ret) {
            LOGE(TAG, "Gateway active check timer stop failed %d", ret);
            return -EINVAL;
        }
    }
    return 0;
}

gw_status_t gateway_subdev_heartbeat_period_set(gw_subdev_t subdev, uint16_t period_s)
{
    uint8_t enable;

    gw_subdev_ctx_t *subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    if (period_s == GW_HB_PERIOD_NOT_CHECK) {
        subdev_ctx->period = 0xFFFF;
        enable             = 0;
    } else {
        enable = 1;
    }

    /* disable node pub */
    return gateway_porting_subdev_heartbeat_period_set(subdev_ctx, enable, period_s);
}
