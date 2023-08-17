/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_ADC_IMPL_H
#define HAL_ADC_IMPL_H

#include <stdint.h>

#include <devices/adc.h>
#include <devices/driver.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct adc_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_adc_config_t *config);
    int (*pin2channel)(rvm_dev_t *dev, int pin);
#ifdef CONFIG_CSI_V2
    int (*read)(rvm_dev_t *dev, uint8_t ch, void *output, size_t num, uint32_t timeout);
#else
    int (*read)(rvm_dev_t *dev, void *output, uint32_t timeout);
#endif
    int (*trans_dma_enable)(rvm_dev_t *dev, bool enable);
} adc_driver_t;

#ifdef __cplusplus
}
#endif

#endif
