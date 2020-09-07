/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_ADC_H
#define DEVICE_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

/****** ADC conversion mode *****/
typedef enum {
    HAL_ADC_SINGLE = 0,       ///< Single conversion mode --- select one channel to convert at a time.
    HAL_ADC_CONTINUOUS,       ///< Continuous conversion mode --- select a channel to convert in a specific times.
    HAL_ADC_SCAN,             ///< Scan mode --- select a group channel to convert at a time.
} hal_adc_mode_e;

typedef struct {
    uint32_t    clk_prescaler;  ///< select ADC clock  prescaler.
    hal_adc_mode_e  mode;           ///< \ref adc_mode_e
    uint32_t    trigger;  ///< 0 -- software adc start or 1 -- external event trigger to start adc.
    uint32_t    intrp_mode;  ///< specifies whether the ADC is configured is interrupt mode (1)or in polling mode (0).
    uint32_t   *channel;    ///< channel base address
    uint32_t    sampling_time; ///< sampling time value to be set for the selected channel. Unit:ADC clock cycles.
    uint32_t    offset;  ///< reserved for future use, can be set to 0.
} hal_adc_config_t;


#define adc_open(name) device_open(name)
#define adc_open_id(name, id) device_open_id(name, id)
#define adc_close(dev) device_close(dev)

int adc_config(aos_dev_t *dev, hal_adc_config_t *config);
int adc_pin2channel(aos_dev_t *dev, int pin);
int adc_read(aos_dev_t *dev, void *output, uint32_t timeout);
void adc_config_default(hal_adc_config_t *config);

#ifdef __cplusplus
}
#endif

#endif
