/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
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
#include <drv/adc.h>
#include <drv/gpio.h>
#include <drv/mailbox.h>

#include <stdio.h>

#include <pin_name.h>
#include <pinmux.h>

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

extern void Default_Handler(void);
extern void TIMA0_IRQHandler(void);
extern void TIMA1_IRQHandler(void);
extern void TIMB0_IRQHandler(void);
extern void TIMB1_IRQHandler(void);
extern void WDT_IRQHandler(void);
extern void USART0_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void I2C0_IRQHandler(void);
extern void I2C1_IRQHandler(void);
extern void GPIO0_IRQHandler(void);
extern void SPI0_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void MBOX_IRQHandler(void);
extern void RTC_IRQHandler(void);

struct {
    uint32_t base;
    uint32_t cpu;
    uint32_t irq;
    void *handler;
    uint32_t pin_num;
    port_name_e port;
}
const sg_gpio_config[CONFIG_GPIO_NUM] = {
    {CSKY_GPIO0_BASE, 1, GPIO_IRQn, GPIO0_IRQHandler, 64, 0},
};


int32_t target_gpio_port_init(port_name_e port, uint32_t *base, uint32_t *cpu, uint32_t *irq, void **handler, uint32_t *pin_num)
{
    int i;

    for (i = 0; i < CONFIG_GPIO_NUM; i++) {
        if (sg_gpio_config[i].port == port) {
            if (base != NULL) {
                *base = sg_gpio_config[i].base;
            }

            if (cpu != NULL) {
                *cpu = sg_gpio_config[i].cpu;
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
typedef struct {
    int count;
    struct {
        uint32_t base;
        int32_t irq;
        void *handler;
     } config[];
} device_irq_table_t;

int32_t target_get(const device_irq_table_t *table, int32_t idx, uint32_t *base, int32_t *irq, void **handler)
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
        {CSKY_UART0_BASE, UART0_IRQn, USART0_IRQHandler},
        {CSKY_UART1_BASE, UART1_IRQn, USART1_IRQHandler},
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_usart_count()
  \brief       get usart instance.
  \return      pointer to usart instance
*/
int32_t target_usart_init(int32_t idx, uint32_t *base, int32_t *irq, void **handler)
{
    return target_get(&sg_usart_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_mbox_config = {
    .count = CONFIG_MBOX_NUM,
    .config = {
        {CSKY_MAILBOX_AP_BASE, MAILBOX_IRQn, MBOX_IRQHandler},
        {CSKY_MAILBOX_CP_BASE, -1,           NULL},
        {CSKY_MAILBOX_SP_BASE, -1,           NULL},
    }
};

int32_t target_mbox_init(int32_t idx, uint32_t *base, int32_t *irq, void **handler)
{
    return target_get(&sg_mbox_config, idx, base, irq, handler);
}

static const mailbox_chnl_config_t sg_mbox_chnl = {
    .count = 2,
    .config = {
        { CPU_INDEX_CP, 3,  3 },
        { CPU_INDEX_SP, 15, 15},
    }
};

const mailbox_chnl_config_t *target_get_mbox_chnl(void)
{
    return &sg_mbox_chnl;
}

int32_t target_get_addr_space(uint32_t addr)
{
#if 0
    if (addr >= CSKY_EFLASH_BASE && addr < (CSKY_EFLASH_BASE + CSKY_EFLASH_SIZE)) {
        return ADDR_SPACE_EFLASH;
    } else if (addr >= CSKY_SRAM_BASE && addr < (CSKY_SRAM_BASE + CSKY_SRAM_SIZE)) {
        return ADDR_SPACE_SRAM;
    } else if (addr >= 0x40000000UL && addr < 0x60000000UL) {
        return ADDR_SPACE_PERIPHERAL;
    } else if (addr >= 0xE000E010UL && addr <= 0xE000FFFFUL) {
        return ADDR_SPACE_TCIP;
    } else {
        return ADDR_SPACE_ERROR;
    }
#endif
    return ADDR_SPACE_SRAM;
}

static const device_irq_table_t sg_spi_config = {
    .count = CONFIG_SPI_NUM,
    .config = {
        {CSKY_SPI0_BASE, SPI0_IRQn, SPI0_IRQHandler},
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_spi_count()
  \brief       get spi instance.
  \return      pointer to spi instance
*/
int32_t target_spi_init(int32_t idx, uint32_t *base, int32_t *irq, void **handler)
{
    int ret = target_get(&sg_spi_config, idx, base, irq, handler);
    return ret;
}

static const device_irq_table_t sg_rtc_config = {
    .count = CONFIG_RTC_NUM,
    .config = {
        {CSKY_RTC_BASE, RTC_IRQn, RTC_IRQHandler},
    }
};

static const device_irq_table_t sg_iic_config = {
    .count = CONFIG_IIC_NUM,
    .config = {
        {CSKY_I2C0_BASE, I2C0_IRQn, I2C0_IRQHandler},
        {CSKY_I2C1_BASE, I2C1_IRQn, I2C1_IRQHandler}
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_iic_count()
  \brief       get iic instance.
  \return      pointer to iic instance
*/
int32_t target_iic_init(int32_t idx, uint32_t *base, int32_t *irq, void **handler)
{
     return target_get(&sg_iic_config, idx, base, irq, handler);
}

int32_t target_get_rtc(int32_t idx, uint32_t *base, int32_t *irq, void **handler)
{
    return target_get(&sg_rtc_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_wdt_config = {
    .count = CONFIG_WDT_NUM,
    .config = {
        {CSKY_WDT_BASE, APWDT_IRQn, WDT_IRQHandler}
    }
};

int32_t target_get_wdt(int32_t idx, uint32_t *base, int32_t *irq, void **handler)
{
    return target_get(&sg_wdt_config, idx, base, irq, handler);
}
