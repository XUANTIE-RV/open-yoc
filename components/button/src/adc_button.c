/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <yoc/button.h>
#ifdef CONFIG_CSI_V2
#include <soc.h>
#else
#include <pinmux.h>
#endif
#include "internal.h"
#include <drv/adc.h>
#include <devices/adc.h>

#define b_adc_param(b) ((adc_button_param_t*)(b->param))

int button_adc_check(button_t *b, int vol)
{
    int range = b_adc_param(b)->range;
    int vref  = b_adc_param(b)->vref;

    if (vol < (vref + range) && vol > (vref - range)) {
        return 1;
    } else {
        return 0;
    }
}

static int button_adc_read(button_t *b)
{
    int ret = 0, vol = 0;
    uint32_t ch = 0;
    hal_adc_config_t config;

    aos_dev_t *dev = adc_open(b_adc_param(b)->adc_name);
    ch = b_adc_param(b)->channel;
    adc_config_default(&config);

#ifdef CONFIG_CSI_V2
#else
    config.channel = &ch;
#endif
    ret = adc_config(dev, &config);

    if (ret == 0) {

#ifdef CONFIG_CSI_V2
        ret = adc_read(dev, ch, &vol, 0);
#else
        ret = adc_read(dev, &vol, 0);
#endif
    }

    adc_close(dev);

    return button_adc_check(b, vol);
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
    .read = button_adc_read,
    .irq_enable = button_adc_enable,
    .irq_disable = button_adc_disable,
};
