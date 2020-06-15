/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     07. Mar 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <stdio.h>
#include <soc.h>
#include <drv/usart.h>
#include <drv/timer.h>
#include <drv/rtc.h>
#include <drv/trng.h>
#include <drv/crc.h>
#include <drv/aes.h>
#include <drv/rsa.h>
#include <drv/eflash.h>
#include <drv/spiflash.h>
#include <drv/spi.h>
#include <drv/gpio.h>
#include <drv/efusec.h>

#include "pin_name.h"
#include "pinmux.h"
#include "usi_pin_planning.h"

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

extern void USI0_IRQHandler(void);
extern void USI1_IRQHandler(void);
extern void USI2_IRQHandler(void);
extern void USI3_IRQHandler(void);
extern void TIM0_IRQHandler(void);
extern void TIM1_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void TIM4_IRQHandler(void);
extern void TIM5_IRQHandler(void);
extern void TIM6_IRQHandler(void);
extern void TIM7_IRQHandler(void);
extern void PWM0_IRQHandler(void);
extern void PWM1_IRQHandler(void);
extern void GPIO0_IRQHandler(void);
extern void GPIO1_IRQHandler(void);
extern void DMAC0_IRQHandler(void);
extern void DMAC1_IRQHandler(void);
extern void MAILBOX_IRQHandler(void);
extern void USART0_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void CODEC_GSK_IRQHandler(void);
extern void I2S0_IRQHandler(void);
extern void I2S1_IRQHandler(void);
extern void I2S2_IRQHandler(void);
extern void I2S3_IRQHandler(void);
extern void SDIO_IRQHandler(void);
extern void SDMMC_IRQHandler(void);
extern void WDT_IRQHandler(void);
extern void RTC_IRQHandler(void);

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const static sg_usart_config[CONFIG_USART_NUM] = {
    {CSKY_UART0_BASE, UART0_HS_IRQn, USART0_IRQHandler},
    {CSKY_UART1_BASE, UART1_HS_IRQn, USART1_IRQHandler},
    {CSKY_UART2_BASE, UART2_IRQn, USART2_IRQHandler},
    {CSKY_UART3_BASE, UART3_IRQn, USART3_IRQHandler}
};

int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= CONFIG_USART_NUM) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_usart_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_usart_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_usart_config[idx].handler;
    }

    return idx;
}


struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const sg_usi_config[CONFIG_USI_NUM] = {
    {CSKY_USI0_BASE, USI0_IRQn, USI0_IRQHandler},
    {CSKY_USI1_BASE, USI1_IRQn, USI1_IRQHandler},
    {CSKY_USI2_BASE, USI2_IRQn, USI2_IRQHandler},
    {CSKY_USI3_BASE, USI3_IRQn, USI3_IRQHandler},
};

int32_t target_usi_init(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= CONFIG_USI_NUM || idx < 0) {
        return -1;
    }

    *base = sg_usi_config[idx].base;
    *irq = sg_usi_config[idx].irq;

    return idx;
}

int32_t target_usi_usart_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= CONFIG_USI_NUM || idx < 0) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_usi_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_usi_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_usi_config[idx].handler;
    }

    return idx;
}

int32_t target_usi_spi_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler, uint32_t *ssel)
{
    if (idx >= CONFIG_USI_SPI_NUM || idx < 0) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_usi_config[idx].base;
    }

    if (irq != NULL) {
        *irq  = sg_usi_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_usi_config[idx].handler;
    }

    return idx;
}

int32_t target_usi_iic_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= CONFIG_USI_NUM || idx < 0) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_usi_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_usi_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_usi_config[idx].handler;
    }

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const sg_timer_config[CONFIG_TIMER_NUM] = {
    {CSKY_TIM0_BASE, TIMER0_INT0_IRQn, TIM0_IRQHandler},
    {CSKY_TIM0_BASE + 0x14, TIMER0_INT1_IRQn, TIM1_IRQHandler},
    {CSKY_LPTIM_BASE, LPTIMER_INT0_IRQn, TIM2_IRQHandler},
    {CSKY_LPTIM_BASE + 0x14, LPTIMER_INT1_IRQn, TIM3_IRQHandler},
    {CSKY_TIM1_BASE, TIMER1_INT0_IRQn, TIM4_IRQHandler},
    {CSKY_TIM1_BASE + 0x14, TIMER1_INT1_IRQn, TIM5_IRQHandler},
    {CSKY_TIM2_BASE, TIMER2_INT0_IRQn, TIM6_IRQHandler},
    {CSKY_TIM2_BASE + 0x14, TIMER2_INT1_IRQn, TIM7_IRQHandler},
};

int32_t target_get_timer_count(void)
{
    return CONFIG_TIMER_NUM;
}

int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= target_get_timer_count()) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_timer_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_timer_config[idx].irq;
    }

    if (irq != NULL) {
        *handler = sg_timer_config[idx].handler;
    }

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const sg_pwm_config[CONFIG_PWM_NUM] = {
    {CSKY_PWM0_BASE, PWM0_IRQn, PWM0_IRQHandler},
    {CSKY_PWM1_BASE, PWM1_IRQn, PWM1_IRQHandler},
};

int32_t target_get_pwm_count(void)
{
    return CONFIG_PWM_NUM;
}

int32_t target_pwm_init(uint32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= target_get_pwm_count()) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_pwm_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_pwm_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_pwm_config[idx].handler;
    }

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
    uint32_t pin_num;
    port_name_e port;
}
const sg_gpio_config[CONFIG_GPIO_NUM] = {
    {CSKY_GPIO0_BASE, GPIO0_IRQn, 32, PORTA},
    {CSKY_GPIO1_BASE, GPIO1_IRQn, 32, PORTB},
};

typedef struct {
    int32_t    gpio_pin;
    uint32_t cfg_idx;
} gpio_pin_map_t;
const static gpio_pin_map_t s_gpio_pin_map[] = {
    {PA0, 0},
    {PA1, 0},
    {PA2, 0},
    {PA3, 0},
    {PA4, 0},
    {PA5, 0},
    {PA6, 0},
    {PA7, 0},
    {PA8, 0},
    {PA9, 0},
    {PA10, 0},
    {PA11, 0},
    {PA12, 0},
    {PA13, 0},
    {PA14, 0},
    {PA15, 0},
    {PA16, 0},
    {PA17, 0},
    {PA18, 0},
    {PA19, 0},
    {PA20, 0},
    {PA21, 0},
    {PA22, 0},
    {PA23, 0},
    {PA24, 0},
    {PA25, 0},
    {PA26, 0},
    {PA27, 0},
    {PA28, 0},
    {PA29, 0},
    {PA30, 0},
    {PA31, 0},
    {PB0, 1},
    {PB1, 1},
    {PB2, 1},
    {PB3, 1},
    {PB4, 1},
    {PB5, 1},
    {PB6, 1},
    {PB7, 1},
    {PB8, 1},
    {PB9, 1},
    {PB10, 1},
    {PB11, 1},
    {PB12, 1},
    {PB13, 1},
    {PB14, 1},
    {PB15, 1},
    {PB16, 1},
    {PB17, 1},
    {PB18, 1},
    {PB19, 1},
    {PB20, 1},
    {PB21, 1},
    {PB22, 1},
    {PB23, 1},
    {PB24, 1},
    {PB25, 1},
    {PB26, 1},
    {PB27, 1},
    {PB28, 1},
    {PB29, 1},
    {PB30, 1},
    {PB31, 1},
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

            if (pin_num != NULL) {
                *pin_num = sg_gpio_config[i].pin_num;
            }

            if (handler != NULL) {
                if (port == PORTA) {
                    *handler = (void *)GPIO0_IRQHandler;
                } else {
                    *handler = (void *)GPIO1_IRQHandler;
                }
            }

            return i;
        }
    }

    return -1;
}

int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx)
{
    uint32_t idx;

    for (idx = 0; idx < sizeof(s_gpio_pin_map) / sizeof(gpio_pin_map_t); idx++) {
        if (s_gpio_pin_map[idx].gpio_pin == gpio_pin) {
            *port_idx = s_gpio_pin_map[idx].cfg_idx;

            if (idx >= 32) {
                return idx - 32;
            } else {
                return idx;
            }
        }
    }

    return -1;
}

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const sg_dmac_config[CONFIG_DMA_CHANNEL_NUM] = {
    {CSKY_DMAC0_BASE, DMAC0_IRQn, DMAC0_IRQHandler},
    {CSKY_DMAC1_BASE, DMAC1_IRQn, DMAC1_IRQHandler},
};

int32_t target_get_dma_channel_count(void)
{
    return CONFIG_DMA_CHANNEL_NUM;
}

int32_t target_get_dmac(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= target_get_dma_channel_count()) {
        return -1;
    }

    if (idx < CONFIG_PER_DMAC0_CHANNEL_NUM) {
        if (base != NULL) {
            *base = sg_dmac_config[0].base;
        }

        if (irq != NULL) {
            *irq = sg_dmac_config[0].irq;
        }

        if (handler != NULL) {
            *handler = sg_dmac_config[0].handler;
        }
    } else {
        if (base != NULL) {
            *base = sg_dmac_config[1].base;
        }

        if (irq != NULL) {
            *irq = sg_dmac_config[1].irq;
        }

        if (handler != NULL) {
            *handler = sg_dmac_config[1].handler;
        }

    }

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const sg_mailbox_config[CONFIG_MAILBOX_NUM] = {
    {CSKY_MCC0_BASE, NULL, NULL},
    {CSKY_MCC0_BASE, NULL, NULL},
    {CSKY_MCC1_BASE, WCOM_IRQn, MAILBOX_IRQHandler},
};

int32_t target_mailbox_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= CONFIG_MAILBOX_NUM) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_mailbox_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_mailbox_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_mailbox_config[idx].handler;
    }

    return idx;
}

struct {
    spiflash_info_t info;
}
const sg_spiflash_config[CONFIG_SPIFLASH_NUM] = {
    {{CSKY_QSPIMEM_BASE, CSKY_QSPIMEM_BASE + 0x7fffff, 0x800}}
};

int32_t target_get_spiflash(int32_t idx, spiflash_info_t *info)
{
    if (info != NULL) {
        info->start = sg_spiflash_config[idx].info.start;
        info->end = sg_spiflash_config[idx].info.end;
        info->sector_count = sg_spiflash_config[idx].info.sector_count;
    }

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_qspi_config[CONFIG_QSPI_NUM] = {
    {CSKY_QSPIC0_BASE, QSPI_IRQn}
};

int32_t target_qspi_init(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= CONFIG_QSPI_NUM) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_qspi_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_qspi_config[idx].irq;
    }

    return idx;
}

uint8_t drv_dma_get_ahb_master_idx(uint32_t addr)
{
    return 0;
}

struct {
    uint32_t base;
    uint32_t gsk_irq;
    void *gsk_handler;
}

const sg_codec_config[] = {
    {CSKY_CODEC_BASE, GSK_INTC_IRQn, CODEC_GSK_IRQHandler}
};

const uint32_t adc_hs_array[8] = {
    DWENUM_DMA_CODEC_ADC_0, DWENUM_DMA_CODEC_ADC_1,
    DWENUM_DMA_CODEC_ADC_2, DWENUM_DMA_CODEC_ADC_3,
    DWENUM_DMA_CODEC_ADC_4, DWENUM_DMA_CODEC_ADC_5,
    DWENUM_DMA_CODEC_ADC_6, DWENUM_DMA_CODEC_ADC_7,
};

int32_t target_codec_init(int32_t idx, uint32_t *base, uint32_t *gsk_irq, void **gsk_handler)
{
    if (idx >= 1) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_codec_config[idx].base;
    }

    if (gsk_irq != NULL) {
        *gsk_irq = sg_codec_config[idx].gsk_irq;
    }

    if (gsk_handler != NULL) {
        *gsk_handler = sg_codec_config[idx].gsk_handler;
    }

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
static const sg_i2s_config[CONFIG_I2S_NUM] = {
    {CSKY_I2S0_BASE, I2S0_IRQn, I2S0_IRQHandler},
    {CSKY_I2S1_BASE, I2S1_IRQn, I2S1_IRQHandler},
    {CSKY_I2S2_BASE, I2S2_IRQn, I2S2_IRQHandler},
    {CSKY_I2S3_BASE, I2S3_IRQn, I2S3_IRQHandler},
};

uint32_t i2s_tx_dma_hs_if_array[] = {
    DWENUM_DMA_I2S0_TX,
    DWENUM_DMA_I2S1_TX,
    DWENUM_DMA_I2S2_TX,
    DWENUM_DMA_I2S3_TX,
};

uint32_t i2s_rx_dma_hs_if_array[] = {
    DWENUM_DMA_I2S0_RX,
    DWENUM_DMA_I2S1_RX,
    DWENUM_DMA_I2S2_RX,
    DWENUM_DMA_I2S3_RX,
};

int32_t target_i2s_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= CONFIG_I2S_NUM) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_i2s_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_i2s_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_i2s_config[idx].handler;
    }

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const sg_rtc_config[CONFIG_RTC_NUM] = {
    {CSKY_RTC_BASE, RTC_IRQn, RTC_IRQHandler},

};

int32_t target_get_rtc_count(void)
{
    return CONFIG_RTC_NUM;
}

int32_t target_get_rtc(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= target_get_rtc_count()) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_rtc_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_rtc_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_rtc_config[idx].handler;
    }

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const sg_wdt_config[CONFIG_WDT_NUM] = {
    {CSKY_WDT_BASE, WDT_IRQn, WDT_IRQHandler}
};

int32_t target_get_wdt_count(void)
{
    return CONFIG_WDT_NUM;
}

int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= target_get_wdt_count()) {
        return -1;
    }

    *base = sg_wdt_config[idx].base;
    *irq = sg_wdt_config[idx].irq;
    *handler = sg_wdt_config[idx].handler;

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
const sg_sdio_config[] = {
    {CSKY_SDIO0_BASE, SDIO0_IRQn, SDIO_IRQHandler},
    {CSKY_SDIO1_BASE, SDIO1_IRQn, SDMMC_IRQHandler},
};

int32_t target_sdio_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    if (idx >= CONFIG_SDIO_NUM) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_sdio_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_sdio_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_sdio_config[idx].handler;
    }

    return idx;
}

int32_t target_get_addr_space(uint32_t addr)
{
    if (addr >= CSKY_QSPIMEM_BASE && addr < (CSKY_QSPIMEM_BASE + 0x800000)) {
        return ADDR_SPACE_EFLASH;
    } else if (addr >= CSKY_FMC_BASE && addr < (CSKY_FMC_BASE + CSKY_FMC_SIZE)) {
        return ADDR_SPACE_SRAM;
    } else if (addr >= 0xE000E010UL && addr <= 0xE000FFFFUL) {
        return ADDR_SPACE_TCIP;
    } else {
        return ADDR_SPACE_ERROR;
    }
}

