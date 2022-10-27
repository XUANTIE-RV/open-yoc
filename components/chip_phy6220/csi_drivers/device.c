/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     15. May 2019
 ******************************************************************************/

#include <stdio.h>
#include <soc.h>
//#include <drv_usart.h>
//#include <drv_rtc.h>
#include <pin_name.h>
#include "bus_dev.h"
#include "gpio.h"
//extern void CSI_UART0_IRQHandler(void);
//extern void CSI_GPIO_IRQHandler(void);

extern void UART0_IRQHandler(void);
extern void UART1_IRQHandler(void);
extern void CSI_GPIO_IRQHandler(void);

//extern void CSI_AP_TIMER_IRQHandler(void);
extern void CSI_SPI0_IRQHandler(void);
extern void CSI_SPI1_IRQHandler(void);
extern void CSI_RTC_IRQHandler(void);
extern void CSI_WDT_IRQHandler(void);
extern void CSI_IIC0_IRQHandler(void);
extern void CSI_IIC1_IRQHandler(void);

extern void hal_ADC_IRQHandler(void);

extern void CSI_AP_TIMER5_IRQHandler(void);
extern void CSI_AP_TIMER6_IRQHandler(void);

extern void CSI_IIC0_IRQHandler(void);
extern void CSI_IIC1_IRQHandler(void);
extern void CSI_DMAC_IRQHandler(void);

typedef struct {
    int count;
    struct {
        uint32_t base;
        uint32_t irq;
        void *handler;
    } config[];
} device_irq_table_t;

int32_t target_get(const device_irq_table_t *table, int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx < table->count) {
        if (base) {
            *base = table->config[idx].base;
        }

        if (irq) {
            *irq = table->config[idx].irq;
        }

        if (handler) {
            *handler = table->config[idx].handler;
        }

        return idx;
    }

    return -1;
}

#if 1
static const device_irq_table_t sg_usart_config = {
    .count = CONFIG_USART_NUM,
    .config = {
        // {AP_UART0_BASE, UART_IRQ, CSI_UART0_IRQHandler},
        {AP_UART0_BASE, UART0_IRQn, UART0_IRQHandler},
        {AP_UART1_BASE, UART1_IRQn, UART1_IRQHandler},
    }
};

int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_usart_config, idx, base, irq, handler);
}
#endif


struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
    uint32_t pin_num;
    port_name_e port;
}
const sg_gpio_config[CONFIG_GPIO_NUM] = {
    {AP_GPIOA_BASE, GPIO_IRQn, CSI_GPIO_IRQHandler, 23, PORTA},
};

typedef struct {
    uint8_t gpio_pin;
    uint8_t cfg_idx;
} gpio_pin_map_t;
const static gpio_pin_map_t s_gpio_pin_map[] = {
    {P0, 0},
    {P1, 0},
    {P2, 0},
    {P3, 0},
    {P7, 0},
    {P9, 0},
    {P10, 0},
    {P11, 0},
    {P14, 0},
    {P15, 0},
    {P16, 0},
    {P17, 0},
    {P18, 0},
    {P20, 0},
    {P23, 0},
    {P24, 0},
    {P25, 0},
    {P26, 0},
    {P27, 0},
    {P31, 0},
    {P32, 0},
    {P33, 0},
    {P34, 0},
};

int32_t target_gpio_port_init(port_name_e port, uint32_t *base, uint32_t *irq, void **handler, uint32_t *pin_num)
{
    int i;

    for (i = 0; i < CONFIG_GPIO_NUM; i++) {
        if (sg_gpio_config[i].port == port) {
            if (base != NULL) {
                *base = sg_gpio_config[i].base;
            }

            if (irq != NULL) {
                *irq = sg_gpio_config[i].irq;
            }

            if (pin_num !=  NULL) {
                *pin_num = sg_gpio_config[i].pin_num;
            }

            if (handler !=  NULL) {
                *handler = sg_gpio_config[i].handler;
            }

            return i;
        }
    }

    return -1;
}

int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx)
{
    uint32_t idx;
    uint8_t match;

    match = 0;

    for (idx = 0; idx < sizeof(s_gpio_pin_map) / sizeof(gpio_pin_map_t); idx++) {
        if (s_gpio_pin_map[idx].gpio_pin == gpio_pin) {
            if (port_idx != NULL) {
                *port_idx = s_gpio_pin_map[idx].cfg_idx;
                match = 1;
                break;
            }
        }
    }

    if (match) {
        return idx;
    } else {
        return -1;
    }
}


static const device_irq_table_t sg_timer_config = {
    .count = CONFIG_TIMER_NUM,
    .config = {
        //{AP_TIM1_BASE, TIMER_IRQ, CSI_AP_TIMER_IRQHandler},//os and ble stack use
        //{AP_TIM2_BASE, TIMER_IRQ, CSI_AP_TIMER_IRQHandler},//os and ble stack use
        //{AP_TIM3_BASE, TIMER_IRQ, CSI_AP_TIMER_IRQHandler},//os and ble stack use
        //{AP_TIM4_BASE, TIMER_IRQ, CSI_AP_TIMER_IRQHandler},//os and ble stack use
        {AP_TIM5_BASE, TIM5_IRQn, CSI_AP_TIMER5_IRQHandler},
        {AP_TIM6_BASE, TIM6_IRQn, CSI_AP_TIMER6_IRQHandler},
    },
};

int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_timer_config, idx, base, irq, handler);
}


#if 1

static const device_irq_table_t sg_spi_config = {
    .count = CONFIG_SPI_NUM,
    .config = {
        {AP_SPI0_BASE, SPI0_IRQn, CSI_SPI0_IRQHandler},
        {AP_SPI1_BASE, SPI1_IRQn, CSI_SPI1_IRQHandler},
    }
};
static uint8_t spi_ssel[CONFIG_SPI_NUM] = {P0, P1};

int32_t target_spi_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler, uint32_t *ssel)
{
    int ret = target_get(&sg_spi_config, idx, base, irq, handler);

    if (ret != 0) {
        if (ssel != NULL) {
            *ssel = spi_ssel[idx];
        }
    }

    return ret;
}

#endif
#if 1
typedef struct {
    uint32_t base;
} rtc_config_t;

const rtc_config_t sg_rtc_config[CONFIG_RTC_NUM] = {
    {AP_RTC_BASE},
};

int32_t target_get_rtc_count(void)
{
    return CONFIG_RTC_NUM;
}

int32_t target_get_rtc(int32_t idx, uint32_t *base)
{
    if (idx >= target_get_rtc_count()) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_rtc_config[idx].base;
    }

//    if (irq != NULL) {
//        *irq = sg_rtc_config[idx].irq;
//    }
//
//    if (handler != NULL) {
//        *handler = (void *)sg_rtc_config[idx].handler;
//    }

    return idx;
}
#endif

static const device_irq_table_t sg_wdt_config = {
    .count = CONFIG_WDT_NUM,
    .config = {
        {AP_WDT_BASE, WDT_IRQn, CSI_WDT_IRQHandler}
    }
};

int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_wdt_config, idx, base, irq, handler);
}


static const device_irq_table_t sg_iic_config = {
    .count = CONFIG_IIC_NUM,
    .config = {
        {AP_I2C0_BASE, I2C0_IRQn, CSI_IIC0_IRQHandler},
        {AP_I2C1_BASE, I2C1_IRQn, CSI_IIC1_IRQHandler}
    }
};

int32_t target_iic_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_iic_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_dmac_config = {
    .count = CONFIG_DMA_CHANNEL_NUM,
    .config = {
        {AP_DMAC_BASE, DMAC_IRQn, CSI_DMAC_IRQHandler},
        {AP_DMAC_BASE, DMAC_IRQn, CSI_DMAC_IRQHandler},
        {AP_DMAC_BASE, DMAC_IRQn, CSI_DMAC_IRQHandler},
        {AP_DMAC_BASE, DMAC_IRQn, CSI_DMAC_IRQHandler},
    }
};

int32_t target_get_dmac(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_dmac_config, idx, base, irq, handler);
}

