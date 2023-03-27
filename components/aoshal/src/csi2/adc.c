/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
#ifndef CONFIG_HAL_ADC_DISABLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/adc.h>
#include <aos/kernel.h>
#include <aos/hal/adc.h>
#include <ulog/ulog.h>

#define TAG "ADC"

#define EVENT_ADC_INTR_COMPLETE 0x000000F0

#define adc_check(ret, log) \
    do { \
        if (ret < 0) { \
            LOGE(TAG, log); \
            return -1; \
        } \
    } while(0)

#ifdef ADC_MODE_DMA
static csi_dma_ch_t dma_ch;
#endif

typedef struct {
    csi_adc_t   dev;
    int         chl_num;
    uint16_t    flag;
    aos_event_t evt;
} hal_adc_t;

static aos_mutex_t        g_adc_mutex;
static uint16_t           g_ref_count;
static int                g_adc_init_ref;
static hal_adc_t          hal_adc;

#ifndef ADC_MODE_SYNC
static void adc_event_cb(csi_adc_t *adc, csi_adc_event_t event, void *arg)
{
    aos_event_t *event_intr = (aos_event_t *)arg;
    if (event == ADC_EVENT_CONVERT_COMPLETE) {
        if (aos_event_is_valid(event_intr)) {
            aos_event_set(event_intr, EVENT_ADC_INTR_COMPLETE, AOS_EVENT_OR);
        }
    }
}
#endif

int32_t hal_adc_init(adc_dev_t *adc)
{
    int32_t ret = CSI_OK;

    if (!aos_mutex_is_valid(&g_adc_mutex)) {
        aos_mutex_new(&g_adc_mutex);
    }

    aos_mutex_lock(&g_adc_mutex, AOS_WAIT_FOREVER);

    if (g_adc_init_ref == 0) {
        g_adc_init_ref++;
        hal_adc.flag = 0;
        ret = csi_adc_init(&hal_adc.dev, 0);

        if (ret < 0) {
            LOGE(TAG, "csi_adc_init error");
            aos_mutex_unlock(&g_adc_mutex);
            aos_mutex_free(&g_adc_mutex);
            return -1;
        }
#ifndef ADC_MODE_SYNC
        ret = aos_event_new(&hal_adc.evt, 0);
        adc_check(ret, "aos_event_new error");
#endif
    }

    // each bit of flag corresponds to an adc port
    hal_adc.flag |= (1 << adc->port);

    g_ref_count++;
    aos_mutex_unlock(&g_adc_mutex);

    return 0;
}

static int32_t adc_value_buffer_get(adc_dev_t *adc, uint32_t *output, size_t num, uint32_t timeout)
{
#ifdef ADC_MODE_SYNC
    uint32_t *temp_output = output;
#endif

    int ret = CSI_OK;

    if (adc == NULL) {
        LOGE(TAG, "adc handle error");
        return -1;
    }

    if (!(hal_adc.flag & (1 << adc->port))) {
        LOGE(TAG, "this port of ADC is not initialized");
        return -1;
    }

    aos_mutex_lock(&g_adc_mutex, AOS_WAIT_FOREVER);

    ret = csi_adc_channel_enable(&hal_adc.dev, adc->port, true);
    adc_check(ret, "csi_adc_channel_enable error");

    ret = csi_adc_freq_div(&hal_adc.dev, 128);

    if (ret <= 0) {
        LOGE(TAG, "csi_adc_freq_div error");
        return -1;
    }

    ret = csi_adc_sampling_time(&hal_adc.dev, adc->config.sampling_cycle);
    adc_check(ret, "csi_adc_sampling_time error");

    ret = csi_adc_continue_mode(&hal_adc.dev, true);
    adc_check(ret, "csi_adc_continue_mode error");

#ifndef ADC_MODE_SYNC
    unsigned int actl_flags = 0;

    ret = csi_adc_attach_callback(&hal_adc.dev, adc_event_cb, (void *)&hal_adc.evt);
    adc_check(ret, "csi_adc_attach_callback error");

    ret = csi_adc_set_buffer(&hal_adc.dev, output, num);
    adc_check(ret, "csi_adc_set_buffer error");
#endif

#ifdef ADC_MODE_DMA
    ret = csi_adc_link_dma(&hal_adc.dev, &dma_ch);
    adc_check(ret, "csi_adc_link_dma error");
#endif

#ifndef ADC_MODE_SYNC
    ret = aos_event_set(&hal_adc.evt, ~EVENT_ADC_INTR_COMPLETE, AOS_EVENT_AND); // clean event
    adc_check(ret, "aos_event_set error");

    ret = csi_adc_start_async(&hal_adc.dev);
    adc_check(ret, "csi_adc_start_async error");

    ret = aos_event_get(&hal_adc.evt, EVENT_ADC_INTR_COMPLETE, AOS_EVENT_OR, &actl_flags, timeout); // block waiting for interrupt
    adc_check(ret, "timeout error");

    ret = csi_adc_channel_enable(&hal_adc.dev, adc->port, false);
    adc_check(ret, "csi_adc_channel_enable error");

    ret = csi_adc_stop_async(&hal_adc.dev);
    adc_check(ret, "csi_adc_stop_async error");

#ifdef ADC_MODE_DMA
    ret = csi_adc_link_dma(&hal_adc.dev, NULL);
    adc_check(ret, "csi_adc_link_dma error");
#endif

    aos_mutex_unlock(&g_adc_mutex);
#endif

#ifdef ADC_MODE_SYNC
    ret = csi_adc_start(&hal_adc.dev);

    if (ret < 0) {
        LOGE(TAG, "csi_adc_start error");
        aos_mutex_unlock(&g_adc_mutex);
        return -1;
    }

    for (int i = 0; i < num; i++) {
        *temp_output = csi_adc_read(&hal_adc.dev);
        temp_output++;
    }

    ret = csi_adc_stop(&hal_adc.dev);
    aos_mutex_unlock(&g_adc_mutex);

    adc_check(ret, "csi_adc_stop error");
#endif

    return 0;
}

int32_t hal_adc_value_multiple_get(adc_dev_t *adc, uint32_t *output, size_t num, uint32_t timeout)
{
    int ret = 0;

    ret = adc_value_buffer_get(adc, output, num, timeout);
    adc_check(ret, "adc_value_buffer_get error");

    return ret;
}

int32_t hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
    int ret = 0;

    ret = adc_value_buffer_get(adc, output, 1, timeout);
    adc_check(ret, "adc_value_buffer_get error");

    return ret;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    if (adc == NULL) {
        return -1;
    }

    aos_mutex_lock(&g_adc_mutex, AOS_WAIT_FOREVER);

    if (g_ref_count > 0) {
        g_ref_count--;
    }

    if (g_ref_count == 0) {
        g_adc_init_ref = 0;
        csi_adc_uninit(&hal_adc.dev);
#ifndef ADC_MODE_SYNC    
        aos_event_free(&hal_adc.evt);
#endif
    }

    aos_mutex_unlock(&g_adc_mutex);

    if (g_ref_count == 0) {
        aos_mutex_free(&g_adc_mutex);
    }

    return 0;
}
#endif //#ifndef CONFIG_HAL_ADC_DISABLED
