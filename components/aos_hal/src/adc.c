/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/adc.h>

#include "soc.h"

#include <aos/hal/adc.h>
#include <aos/kernel.h>

#define ADC_CON_DATA_NUM 1
static adc_handle_t g_adc_handle = NULL;
static uint16_t     g_ref_count  = 0;
static aos_mutex_t  g_adc_mutex  = {NULL};

int hal_adc_pin2channel(int pin)
{
    int channel = 0;
    channel = drv_adc_pin2channel(pin);
    return channel;
}

int32_t hal_adc_init(adc_dev_t *adc)
{
    if (adc == NULL) {
        return -1;
    }

    if (g_adc_handle == NULL && g_adc_mutex.hdl == NULL) {
        aos_mutex_new(&g_adc_mutex);
    }
  
    aos_mutex_lock(&g_adc_mutex, AOS_WAIT_FOREVER);
  
    if (g_adc_handle == NULL) {
        g_adc_handle = drv_adc_initialize(0, NULL);
    }

    if (g_adc_handle == NULL) {
        aos_mutex_unlock(&g_adc_mutex);
        aos_mutex_free(&g_adc_mutex);
        return -1;
    }

    adc->priv = g_adc_handle;
    g_ref_count++;
    aos_mutex_unlock(&g_adc_mutex);

    return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc, void *output, uint32_t timeout)
{
    int      i;
    uint32_t recv_data[ADC_CON_DATA_NUM];

    if (adc == NULL) {
        return -1;
    }

    if (adc->priv == NULL) {
        return -1;
    }

    aos_mutex_lock(&g_adc_mutex, AOS_WAIT_FOREVER);

    adc_handle_t handle = adc->priv;

    /* adc config */
    adc_conf_t sconfig;
    uint32_t   ch_array[ADC_CON_DATA_NUM] = {0};

    ch_array[0] = adc->port;

    sconfig.mode          = ADC_SINGLE;
    sconfig.trigger       = 0;
    sconfig.intrp_mode    = 0;
    sconfig.channel_array = ch_array;
    sconfig.channel_nbr   = 1;
    sconfig.conv_cnt      = ADC_CON_DATA_NUM;

    int ret = drv_adc_config(handle, &sconfig);
    if (ret < 0) {
        aos_mutex_unlock(&g_adc_mutex);
        return -1;
    }

    for (i = 0; i < ADC_CON_DATA_NUM; i++) {
        recv_data[i] = 0;
    }

    /* adc start */
    ret = drv_adc_start(handle);

    if (ret < 0) {
        aos_mutex_unlock(&g_adc_mutex);
        return -1;
    }

    /* adc read */
    ret = drv_adc_read(handle, &recv_data[0], ADC_CON_DATA_NUM);
    ret = drv_adc_read(handle, &recv_data[0], ADC_CON_DATA_NUM);

    if (ret == 0) {
        *((uint32_t *)output) = recv_data[0];
    }

    drv_adc_stop(handle);

    aos_mutex_unlock(&g_adc_mutex);

    return ret;
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    if (adc == NULL) {
        return -1;
    }

    if (adc->priv == NULL) {
        return -1;
    }

    aos_mutex_lock(&g_adc_mutex, AOS_WAIT_FOREVER);
    if (g_ref_count > 0) {
        g_ref_count--;
    }

    if (g_adc_handle && g_ref_count == 0) {
        drv_adc_uninitialize(g_adc_handle);
    }

    adc->priv = NULL;
    aos_mutex_unlock(&g_adc_mutex);

    if (g_adc_handle && g_ref_count == 0) {
      aos_mutex_free(&g_adc_mutex);
      g_adc_handle = NULL;
    }

    return 0;
}
