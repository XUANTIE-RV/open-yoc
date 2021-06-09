/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_MODEL)
#include <aos/kv.h>
#include <aos/aos.h>
#include <yoc/partition.h>
#include <api/mesh.h>


struct k_timer reset_by_repeat_timer;
#define KV_RESET_KEY "REST_CNT"
#define DEF_MESH_RESET_BY_REPEAT_COUNTER      5
#define DEF_MESH_RESET_BY_REPEAT_TIMEOUT      (3*1000)
#define TAG   "MESH_RESET"

static int reset_by_repeat_write_reset_cnt(uint16_t cnt)
{
    uint32_t data = cnt;
    //LOGD(TAG, "%s 0x%04x", __func__, cnt);
    return aos_kv_setint(KV_RESET_KEY, data);
}

static int reset_by_repeat_read_reset_cnt()
{
    int  cnt;
    int ret;
    //LOGD(TAG, "%s", __func__);

    ret = aos_kv_getint(KV_RESET_KEY, &cnt);

    if (ret) {
        LOGE(TAG, "read size error");
        return ret;
    }

    return cnt;
}

static void _reset_by_repeat_timer_cb(void *p_timer, void *args)
{
    //LOGD(TAG, "%s", __func__);
    reset_by_repeat_write_reset_cnt(0);
}

static void hard_reset()
{
    LOGD(TAG, "HW RST");
    aos_kv_reset();
    aos_reboot();
}

void reset_by_repeat_init(void)
{
    int number;

    number = reset_by_repeat_read_reset_cnt();

    if (number < 0) {
        number = 0;
    }

    if (number < DEF_MESH_RESET_BY_REPEAT_COUNTER) {
        number++;
    }

    LOGI(TAG, "%s, number = %d", __func__, number);
    reset_by_repeat_write_reset_cnt(number);

    if (number == DEF_MESH_RESET_BY_REPEAT_COUNTER) {
        hard_reset();
    } else {
        k_timer_init(&reset_by_repeat_timer, _reset_by_repeat_timer_cb, NULL);
        k_timer_start(&reset_by_repeat_timer, DEF_MESH_RESET_BY_REPEAT_TIMEOUT);
    }
}

