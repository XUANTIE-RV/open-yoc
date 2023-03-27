#include <stdint.h>
#include <av/output/ao.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

#define TAG "VOLEXT"

static ao_cls_t   *g_ao_ext    = NULL;
static uint8_t    *g_vol_map   = NULL;
static aos_mutex_t g_vol_mutex = NULL;

void smtaudio_btvol_set_attach_ao(ao_cls_t *ao, uint8_t *vol_map)
{
    if (!aos_mutex_is_valid(&g_vol_mutex)) {
        aos_mutex_new(&g_vol_mutex);
    }

    aos_mutex_lock(&g_vol_mutex, AOS_WAIT_FOREVER);
    g_ao_ext  = ao;
    g_vol_map = vol_map;
    aos_mutex_unlock(&g_vol_mutex);
}

void smtaudio_btvol_set_vol(int vol)
{
    size_t     size;
    ovol_set_t para;

    if (!aos_mutex_is_valid(&g_vol_mutex)) {
        return;
    }

    if (g_ao_ext == NULL) {
        return;
    }

    if (g_vol_map == NULL) {
        LOGE(TAG, "vol map error");
        return;
    }

    aos_mutex_lock(&g_vol_mutex, AOS_WAIT_FOREVER);
    if (vol < 0) {
        vol = 0;
    }

    if (vol > 100) {
        vol = 100;
    }

    size           = sizeof(para);
    para.vol_index = g_vol_map[vol];
    ao_control(g_ao_ext, AO_CMD_VOL_SET, (void *)&para, &size);

    aos_mutex_unlock(&g_vol_mutex);
}
