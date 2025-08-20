 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <drv/clk.h>
#include <drv/porting.h>
#include <devices/impl/clk_impl.h>

#define TAG "clk_drv"

typedef struct {
    rvm_dev_t      device;
} clk_dev_t;

#define CLKDEV(dev) ((clk_dev_t *)dev)

static rvm_dev_t *_clk_init(driver_t *drv, void *config, int id)
{
    clk_dev_t *clk = (clk_dev_t *)rvm_hal_device_new(drv, sizeof(clk_dev_t), id);

    return (rvm_dev_t *)clk;
}

#define _clk_uninit rvm_hal_device_free

static int _clk_open(rvm_dev_t *dev)
{
    soc_clk_init();
    return 0;
}

static int _clk_close(rvm_dev_t *dev)
{
    return 0;
}

static int _clk_enable(rvm_dev_t *dev, const char *dev_name)
{
    rvm_dev_t *device = rvm_hal_device_open(dev_name);
    if (!device) {
        return -1;
    }
    if (device->drv->clk_en)
        return device->drv->clk_en(device, true);
    return 0;
}

static int _clk_disable(rvm_dev_t *dev, const char *dev_name)
{
    rvm_dev_t *device = rvm_hal_device_open(dev_name);
    if (!device) {
        return -1;
    }
    if (device->drv->clk_en)
        return device->drv->clk_en(device, false);
    return 0;
}

static int _clk_get_freq(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t *freq)
{
    uint32_t tmp_freq;

    switch (clk_id)
    {
    case RVM_HAL_APB_CLK:
        tmp_freq = soc_get_apb_freq(idx);
        break;
    case RVM_HAL_AHB_CLK:
        tmp_freq = soc_get_ahb_freq(idx);
        break;
    case RVM_HAL_CPU_CLK:
        tmp_freq = soc_get_cpu_freq(idx);
        break;
    case RVM_HAL_SYS_CLK:
        tmp_freq = soc_get_sys_freq();
        break;
    case RVM_HAL_CORETIME_CLK:
        tmp_freq = soc_get_coretim_freq();
        break;
    case RVM_HAL_UART_CLK:
        tmp_freq = soc_get_uart_freq(idx);
        break;
    case RVM_HAL_SPI_CLK:
        tmp_freq = soc_get_spi_freq(idx);
        break;
    case RVM_HAL_IIC_CLK:
        tmp_freq = soc_get_iic_freq(idx);
        break;
    case RVM_HAL_I2S_CLK:
        tmp_freq = soc_get_i2s_freq(idx);
        break;
    case RVM_HAL_PWM_CLK:
        tmp_freq = soc_get_pwm_freq(idx);
        break;
    case RVM_HAL_ADC_CLK:
        tmp_freq = soc_get_adc_freq(idx);
        break;
    case RVM_HAL_QSPI_CLK:
        tmp_freq = soc_get_qspi_freq(idx);
        break;
    case RVM_HAL_USI_CLK:
        tmp_freq = soc_get_usi_freq(idx);
        break;
    case RVM_HAL_TIMER_CLK:
        tmp_freq = soc_get_timer_freq(idx);
        break;
    case RVM_HAL_RTC_CLK:
        tmp_freq = soc_get_rtc_freq(idx);
        break;
    case RVM_HAL_WDT_CLK:
        tmp_freq = soc_get_wdt_freq(idx);
        break;
    case RVM_HAL_SDIO_CLK:
        tmp_freq = soc_get_sdio_freq(idx);
        break;
    case RVM_HAL_EMMC_CLK:
        tmp_freq = soc_get_emmc_freq(idx);
        break;
    case RVM_HAL_USB_CLK:
        tmp_freq = soc_get_usb_freq(idx);
        break;
    case RVM_HAL_REF_CLK:
        tmp_freq = soc_get_ref_clk_freq(idx);
        break;
    default:
        tmp_freq = 0;
        break;
    }
    *freq = tmp_freq;
    return 0;
}

static int _clk_set_freq(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t freq)
{
    switch (clk_id)
    {
    case RVM_HAL_APB_CLK:
    case RVM_HAL_AHB_CLK:
    case RVM_HAL_CPU_CLK:
    case RVM_HAL_CORETIME_CLK:
    case RVM_HAL_UART_CLK:
    case RVM_HAL_SPI_CLK:
    case RVM_HAL_IIC_CLK:
    case RVM_HAL_I2S_CLK:
    case RVM_HAL_PWM_CLK:
    case RVM_HAL_ADC_CLK:
    case RVM_HAL_QSPI_CLK:
    case RVM_HAL_USI_CLK:
    case RVM_HAL_TIMER_CLK:
    case RVM_HAL_RTC_CLK:
    case RVM_HAL_WDT_CLK:
    case RVM_HAL_SDIO_CLK:
    case RVM_HAL_EMMC_CLK:
    case RVM_HAL_USB_CLK:
    case RVM_HAL_REF_CLK:
        break;
    case RVM_HAL_SYS_CLK:
        soc_set_sys_freq(freq);
        return 0;
    default:
        break;
    }
    return -1;
}

static clk_driver_t clk_driver = {
    .drv = {
        .name   = "clk",
        .init   = _clk_init,
        .uninit = _clk_uninit,
        .open   = _clk_open,
        .close  = _clk_close
    },
    .enable     = _clk_enable,
    .disable    = _clk_disable,
    .get_freq   = _clk_get_freq,
    .set_freq   = _clk_set_freq
};

void rvm_clk_drv_register(void)
{
    rvm_driver_register(&clk_driver.drv, NULL, 0);
}
