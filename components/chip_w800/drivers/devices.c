/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     24. August 2017
 ******************************************************************************/
#include "soc.h"
#include <drv/usart.h>
#include <drv/timer.h>
#include <drv/rtc.h>
#include <drv/trng.h>
#include <drv/crc.h>
#include <drv/aes.h>
#include <drv/rsa.h>
#include <drv/eflash.h>
#include <drv/spi.h>
#include <drv/spiflash.h>
#include <drv/adc.h>
#include <drv/gpio.h>

#include <stdio.h>

#include <pin_name.h>
#include <pinmux.h>

#include "wm_flash_map.h"
#include "wm_internal_flash.h"

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

extern void Default_Handler(void);
extern void UART0_IRQHandler(void);
extern void UART1_IRQHandler(void);
extern void UART234_IRQHandler(void);
extern void WDG_IRQHandler(void);
extern void PMU_RTC_IRQHandler(void);
extern void GPIOA_IRQHandler(void);
extern void GPIOB_IRQHandler(void);
extern void TIM0_5_IRQHandler(void);
extern void I2C_IRQHandler(void);
extern void I2S_IRQHandler(void);
extern void SPI_LS_IRQHandler(void);
extern void PWM_IRQHandler(void);

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
        if (base)
            *base = table->config[idx].base;

        if (irq)
            *irq = table->config[idx].irq;

        if (handler)
            *handler = table->config[idx].handler;

        return idx;
    }

    return -1;
}

static const device_irq_table_t sg_usart_config = {
    .count = CONFIG_USART_NUM,
    .config = {
        {CSKY_UART0_BASE, UART0_IRQn, UART0_IRQHandler},
        {CSKY_UART1_BASE, UART1_IRQn, UART1_IRQHandler},
        {CSKY_UART2_BASE, UART24_IRQn, UART234_IRQHandler},
        {CSKY_UART3_BASE, UART24_IRQn, UART234_IRQHandler},
        {CSKY_UART4_BASE, UART24_IRQn, UART234_IRQHandler}
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_usart_count()
  \brief       get usart instance.
  \return      pointer to usart instance
*/
int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_usart_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_wdt_config = {
    .count = CONFIG_WDT_NUM,
    .config = {
        {CSKY_WDT_BASE, WDG_IRQn, WDG_IRQHandler}
    }
};

int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_wdt_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_rtc_config = {
    .count = CONFIG_RTC_NUM,
    .config = {
        {CSKY_RTC_BASE, PMU_IRQn, PMU_RTC_IRQHandler},
    }
};

int32_t target_get_rtc(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_rtc_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_gpio_config = {
    .count = CONFIG_GPIO_NUM,
    .config = {
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOA_BASE, GPIOA_IRQn, GPIOA_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler},
        {CSKY_GPIOB_BASE, GPIOB_IRQn, GPIOB_IRQHandler}
    }
};

int32_t target_gpio_init(int32_t gpio_pin, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_gpio_config, gpio_pin, base, irq, handler);
}

static const device_irq_table_t sg_timer_config = {
    .count = CONFIG_TIMER_NUM,
    .config = {
        {CSKY_TIMER_BASE, TIMER_IRQn, TIM0_5_IRQHandler},
        {CSKY_TIMER_BASE, TIMER_IRQn, TIM0_5_IRQHandler},
        {CSKY_TIMER_BASE, TIMER_IRQn, TIM0_5_IRQHandler},
        {CSKY_TIMER_BASE, TIMER_IRQn, TIM0_5_IRQHandler},
        {CSKY_TIMER_BASE, TIMER_IRQn, TIM0_5_IRQHandler},
        {CSKY_TIMER_BASE, TIMER_IRQn, TIM0_5_IRQHandler}
    }
};

int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_timer_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_lspi_config = {
    .count = CONFIG_LSPI_NUM,
    .config = {
        {CSKY_LSPI_BASE, SPI_LS_IRQn, SPI_LS_IRQHandler}
    }
};

int32_t target_get_lspi(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_lspi_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_iic_config = {
    .count = CONFIG_IIC_NUM,
    .config = {
        {CSKY_I2C_BASE, I2C_IRQn, I2C_IRQHandler}
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_iic_count()
  \brief       get iic instance.
  \return      pointer to iic instance
*/
int32_t target_iic_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
     return target_get(&sg_iic_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_pwm_config = {
    .count = CONFIG_PWM_NUM,
    .config = {
        {CSKY_PWM_BASE, PWM_IRQn, PWM_IRQHandler},
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_usart_count()
  \brief       get usart instance.
  \return      pointer to usart instance
*/
int32_t target_pwm_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_pwm_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_pmu_config = {
    .count = CONFIG_PMU_NUM,
    .config = {
        {CSKY_PMU_BASE, PMU_IRQn, PMU_RTC_IRQHandler},
    }
};

int32_t target_get_pmu(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_pmu_config, idx, base, irq, handler);
}

#define EFLASH_AREA_LEN    (TLS_FLASH_END_ADDR - FLASH_BASE_ADDR)
struct {
    uint32_t base;
    eflash_info_t info;
}
const sg_eflash_config[CONFIG_EFLASH_NUM] = {
    {CSKY_EFLASH_CONTROL_BASE, 
     {FLASH_BASE_ADDR, 
      0,//FLASH_BASE_ADDR + EFLASH_AREA_LEN, 
      0,//EFLASH_AREA_LEN / INSIDE_FLS_SECTOR_SIZE, 
      INSIDE_FLS_SECTOR_SIZE,
      INSIDE_FLS_PAGE_SIZE,
      INSIDE_FLS_SECTOR_SIZE,
      0xFF}
     }
};

int32_t target_get_eflash(int32_t idx, uint32_t *base, eflash_info_t *info)
{
    if (idx >= CONFIG_EFLASH_NUM) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_eflash_config[idx].base;
    }

    if (info != NULL) {
        info->start = sg_eflash_config[idx].info.start;
        info->end = FLASH_BASE_ADDR + EFLASH_AREA_LEN;//sg_eflash_config[idx].info.end;
        info->sector_count = EFLASH_AREA_LEN / INSIDE_FLS_SECTOR_SIZE;//sg_eflash_config[idx].info.sector_count;
        info->sector_size = sg_eflash_config[idx].info.sector_size;
        info->page_size = sg_eflash_config[idx].info.page_size;
        info->program_unit = sg_eflash_config[idx].info.program_unit;
        info->erased_value = sg_eflash_config[idx].info.erased_value;
    }

    return idx;
}

struct {
    uint32_t base;
    spiflash_info_t info;
}
const sg_spiflash_config[CONFIG_SPIFLASH_NUM] = {
    {0, {0x0, 0x100000, 0x100000 / 0x1000, 0x1000, 0x100, 0x1000, 0xFF}}
};

int32_t target_get_spiflash(int32_t idx, uint32_t *base, spiflash_info_t *info)
{
    if (idx >= CONFIG_EFLASH_NUM) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_spiflash_config[idx].base;
    }

    if (info != NULL) {
        info->start = sg_spiflash_config[idx].info.start;
        info->end = sg_eflash_config[idx].info.end;
        info->sector_count = sg_eflash_config[idx].info.sector_count;
        info->sector_size = sg_spiflash_config[idx].info.sector_size;
        info->page_size = sg_spiflash_config[idx].info.page_size;
        info->program_unit = sg_spiflash_config[idx].info.program_unit;
        info->erased_value = sg_spiflash_config[idx].info.erased_value;
    }

    return idx;
}

static const device_irq_table_t sg_i2s_config = {
    .count = CONFIG_I2S_NUM,
    .config = {
        {CSKY_I2S_BASE, I2S_IRQn, I2S_IRQHandler},
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_i2s_count()
  \brief       get i2s instance.
  \return      pointer to i2s instance
*/
int32_t target_i2s_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_i2s_config, idx, base, irq, handler);
}
