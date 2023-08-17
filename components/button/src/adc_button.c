/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <soc.h>

#include <yoc/button.h>
#include "internal.h"

#define b_adc_param(b) ((adc_button_param_t *)(b->param))

static void *button_adc_find(uint8_t port)
{
    button_t *b = NULL;

    slist_for_each_entry(&g_button_srv.button_head, b, button_t, next)
    {
        button_ops_t *ops = &adc_ops;
        void *adc = b_adc_param(b)->adc_hdl;
        if ((b->ops == ops) && adc && (b_adc_param(b)->channel == port)) {
            return adc;
        }
    }

    return NULL;
}

static uint32_t button_adc_get_value(button_t *button)
{
    /* fixme: why 64?workaround hal or csi driver Cross-border access */
    uint32_t val[64];

    int avr_count = 2;
    uint32_t avr_val = 0;
    int      ret;

    memset(val, 0, sizeof(val));

    ret = rvm_hal_adc_read_multiple(b_adc_param(button)->adc_hdl, b_adc_param(button)->channel, val, avr_count, 1000);
    if (ret == 0) {
        for (int i = 0; i < avr_count; i++) {
            avr_val += val[i];
        }
        avr_val /= avr_count;
    }

    return avr_val;
}

static int button_adc_check(button_t *button, uint32_t value)
{
    int range = b_adc_param(button)->range;
    int vref  = b_adc_param(button)->vref;

    if (value < (vref + range) && value > (vref - range)) {
        return 1;
    } else {
        return 0;
    }
}

static int button_adc_init(button_t *button)
{
    uint8_t port = b_adc_param(button)->channel;

    void *adc = button_adc_find(port);

    if (adc == NULL) {
        rvm_adc_drv_register(0);
        adc = rvm_hal_adc_open("adc");
        rvm_hal_adc_config_t config;
        rvm_hal_adc_config_default(&config);
        rvm_hal_adc_config(adc, &config);
    }

    b_adc_param(button)->adc_hdl = adc;

    return 0;
}

static int button_adc_deinit(button_t *button)
{
    void *adc = b_adc_param(button)->adc_hdl;

    if (adc) {
        rvm_hal_adc_close(adc);
        b_adc_param(button)->adc_hdl = NULL;
    }

    return 0;
}

/* ADC 按键扫描时，无需每个button都读一次 */
static int      g_adc_start_read = 0;
static uint32_t g_adc_value      = 0;

void button_adc_start_read()
{
    g_adc_start_read = 1;
}

static int button_adc_read(button_t *button)
{
    uint32_t value = 0;

    if (g_adc_start_read == 1) {
        g_adc_value      = button_adc_get_value(button);
        g_adc_start_read = 0;
    }

    value = g_adc_value;

    return button_adc_check(button, value);
}

static int button_adc_enable(button_t *button)
{
    button->st_ms = 0;

    return 0;
}

static int button_adc_disable(button_t *button)
{
    button->st_ms = -1;

    return 0;
}

button_ops_t adc_ops = {
    .init        = button_adc_init,
    .deinit      = button_adc_deinit,
    .read        = button_adc_read,
    .irq_enable  = button_adc_enable,
    .irq_disable = button_adc_disable,
};
