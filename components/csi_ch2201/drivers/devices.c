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
#include <drv/adc.h>
#include <drv/gpio.h>

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
extern void SPI0_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void GPIO0_IRQHandler(void);
extern void GPIO1_IRQHandler(void);
extern void RTC_IRQHandler(void);
extern void RTC1_IRQHandler(void);
extern void AES_IRQHandler(void);
extern void SHA_IRQHandler(void);
extern void RSA_IRQHandler(void);
extern void ADC_IRQHandler(void);
extern void I2S_IRQHandler(void);
extern void DMAC0_IRQHandler(void);
extern void DMAC1_IRQHandler(void);

struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
    uint32_t pin_num;
    port_name_e port;
}
const sg_gpio_config[CONFIG_GPIO_NUM] = {
    {CSKY_GPIO0_BASE, GPIOA_IRQn, GPIO0_IRQHandler, 28, PORTA},
    {CSKY_GPIO1_BASE, GPIOB_IRQn, GPIO1_IRQHandler, 4, PORTB},
};

typedef struct {
    uint8_t gpio_pin;
    uint8_t cfg_idx;    //idx of sg_gpio_config[]
} gpio_pin_map_t;
const static gpio_pin_map_t s_gpio_pin_map[] = {
    {PA0, 0},
    {PA1, 0},
    {PA2, 0},
    {PA3, 0},
    {PA4, 0},
    {PA5, 0},

    {PB0, 1},
    {PB1, 1},
    {PB2, 1},
    {PB3, 1},

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
    {PC0, 2},
    {PC1, 2}
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
/**
  \param[in]   instance idx, must not exceed return value of target_get_gpio_count()
  \brief       get gpio instance.
  \return      pointer to gpio instance
*/
int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx)
{
    uint32_t idx;

    for (idx = 0; idx < sizeof(s_gpio_pin_map) / sizeof(gpio_pin_map_t); idx++) {
        if (s_gpio_pin_map[idx].gpio_pin == gpio_pin) {
            *port_idx = s_gpio_pin_map[idx].cfg_idx;

            /*pinmux*/
            if (idx >= 32) {
                return idx - 32;
            } else if (idx >= 10) {
                return idx - 4;
            } else if (idx >= 6) {
                return idx - 6;
            } else {
                return idx;
            }
        }
    }

    return -1;
}

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

static const device_irq_table_t sg_timer_config = {
    .count = CONFIG_TIMER_NUM,
    .config = {
        {CSKY_TIM0_BASE, TIMA0_IRQn, TIMA0_IRQHandler},
        {CSKY_TIM0_BASE + 0x14, TIMA1_IRQn, TIMA1_IRQHandler},
        {CSKY_TIM1_BASE, TIMB0_IRQn, TIMB0_IRQHandler},
        {CSKY_TIM1_BASE + 0x14, TIMB1_IRQn, TIMB1_IRQHandler},
    },
};

int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_timer_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_pmu_config = {
    .count = CONFIG_PMU_NUM,
    .config = {
        {CSKY_CLKGEN_BASE, POWM_IRQn},
    },
};

int32_t target_get_pmu(int32_t idx, uint32_t *base, uint32_t *irq)
{
    return target_get(&sg_pmu_config, idx, base, irq, NULL);
}

static const device_irq_table_t sg_rtc_config = {
    .count = CONFIG_RTC_NUM,
    .config = {
        {CSKY_RTC0_BASE, RTC_IRQn, RTC_IRQHandler},
        {CSKY_RTC1_BASE, RTC1_IRQn, RTC1_IRQHandler}
    }
};

int32_t target_get_rtc(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
#undef  CSKY_PMU_BASE
#define CSKY_PMU_BASE  0x40002000
#define BIT1 (0x1)

     unsigned int value = readl(CSKY_PMU_BASE);
     value &= ~BIT1;
     writel(value, CSKY_PMU_BASE);

    return target_get(&sg_rtc_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_trng_config = {
    .count = CONFIG_TRNG_NUM,
    .config = {
        {CSKY_TRNG_BASE},
    }
};

/**
  \param[in]   instance idx
  \brief       get trng instance.
  \return      pointer to trng instance
*/
int32_t target_get_trng(int32_t idx, uint32_t *base)
{
    return target_get(&sg_trng_config, idx, base, NULL, NULL);
}

static const device_irq_table_t sg_crc_config = {
    .count = CONFIG_CRC_NUM,
    .config = {
        {CSKY_CRC_BASE},
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_crc_count()
  \brief       get crc instance.
  \return      pointer to crc instance
*/
int32_t target_get_crc(int32_t idx, uint32_t *base)
{
    return target_get(&sg_crc_config, idx, base, NULL, NULL);
}

static const device_irq_table_t sg_usart_config = {
    .count = CONFIG_USART_NUM,
    .config = {
        {CSKY_UART0_BASE, UART0_IRQn, USART0_IRQHandler},
        {CSKY_UART1_BASE, UART1_IRQn, USART1_IRQHandler},
        {CSKY_UART2_BASE, UART2_IRQn, USART2_IRQHandler}
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

static const device_irq_table_t sg_spi_config = {
    .count = CONFIG_SPI_NUM,
    .config = {
        {CSKY_SPI0_BASE, SPI0_IRQn, SPI0_IRQHandler},
        {CSKY_SPI1_BASE, SPI1_IRQn, SPI1_IRQHandler}
    }
};
static uint8_t spi_ssel[CONFIG_SPI_NUM] = {PA7, PA18};

/**
  \param[in]   instance idx, must not exceed return value of target_get_spi_count()
  \brief       get spi instance.
  \return      pointer to spi instance
*/
int32_t target_spi_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler, uint32_t *ssel)
{
    int ret = target_get(&sg_spi_config, idx, base, irq, handler);
    if (ret != 0) {
        if (ssel != NULL)
            *ssel = spi_ssel[idx];
    }

    return ret;
}

static const device_irq_table_t sg_aes_config = {
    .count = CONFIG_AES_NUM,
    .config = {
        {CSKY_AES_BASE, AES_IRQn, AES_IRQHandler}
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_aes_count()
  \brief       get aes instance.
  \return      pointer to aes instance
*/
int32_t target_get_aes(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_aes_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_rsa_config = {
    .count = CONFIG_RSA_NUM,
    .config = {
        {CSKY_RSA_BASE, RSA_IRQn}
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_rsa_count()
  \brief       get rsa instance.
  \return      pointer to rsa instance
*/
int32_t target_get_rsa(int32_t idx, uint32_t *base, uint32_t *irq)
{
    return target_get(&sg_rsa_config, idx, base, irq, NULL);
}

struct {
    uint32_t base;
    eflash_info_t info;
}
const sg_eflash_config[CONFIG_EFLASH_NUM] = {
    {CSKY_EFLASH_CONTROL_BASE, {0x10000000, 0x1003f7ff, 0x1fc, 0x200}}
};

/**
  \brief       get eflash instance count.
  \return      eflash instance count
*/
int32_t target_get_eflash_count(void)
{
    return CONFIG_EFLASH_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_eflash_count()
  \brief       get eflash instance.
  \return      pointer to eflash instance
*/
int32_t target_get_eflash(int32_t idx, uint32_t *base, eflash_info_t *info)
{
    if (idx >= target_get_eflash_count()) {
        return -1;
    }

    if (base != NULL) {
        *base = sg_eflash_config[idx].base;
    }

    if (info != NULL) {
        info->start = sg_eflash_config[idx].info.start;
        info->end = sg_eflash_config[idx].info.end;
        info->sector_count = sg_eflash_config[idx].info.sector_count;
    }

    return idx;
}

static const device_irq_table_t sg_wdt_config = {
    .count = CONFIG_WDT_NUM,
    .config = {
        {CSKY_WDT_BASE, WDT_IRQn, WDT_IRQHandler}
    }
};

int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_wdt_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_dmac_config = {
    .count = CONFIG_DMA_CHANNEL_NUM,
    .config = {
        {CSKY_DMAC0_BASE, SEU_DMAC_IRQn, DMAC0_IRQHandler},
        {CSKY_DMAC0_BASE, SEU_DMAC_IRQn, DMAC0_IRQHandler},
        {CSKY_DMAC1_BASE, NONSEU_DMAC_IRQn, DMAC1_IRQHandler},
        {CSKY_DMAC1_BASE, NONSEU_DMAC_IRQn, DMAC1_IRQHandler}
    }
};

int32_t target_get_dmac(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_dmac_config, idx, base, irq, handler);
}

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
int32_t target_iic_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
     return target_get(&sg_iic_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_pwm_config = {
    .count = CONFIG_PWM_NUM,
    .config = {
        {CSKY_PWM_BASE, PWM_IRQn}
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_pwm_count()
  \brief       get pwm instance.
  \return      pointer to pwm instance
*/
int32_t target_pwm_init(uint32_t idx, uint32_t *base, uint32_t *irq)
{
    return target_get(&sg_pwm_config, idx, base, irq, NULL);
}

static const device_irq_table_t sg_sha_config = {
    .count = CONFIG_SHA_NUM,
    .config = {
        {CSKY_SHA_BASE, SHA_IRQn, NULL}
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_sha_count()
  \brief       get sha instance.
  \return      pointer to sha instance
*/
int32_t target_get_sha(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
    return target_get(&sg_sha_config, idx, base, irq, handler);
}

static const device_irq_table_t sg_adc_config = {
    .count = CONFIG_ADC_NUM,
    .config = {
        {CSKY_ADC_CTL_BASE, ADC_IRQn}
    }
};

/**
  \param[in]   initialize adc channel;
  \brief       get  instance.
  \return      0 - success, (-1) - fail.
*/
int32_t target_adc_init(int32_t channel)
{
    return 0;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_adc_count()
  \brief       get sha instance.
  \return      pointer to adc instance
*/
int32_t target_get_adc(int32_t idx, uint32_t *base, uint32_t *irq)
{
    return target_get(&sg_adc_config, idx, base, irq, NULL);
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


static const device_irq_table_t sg_acmp_config = {
    .count = CONFIG_ACMP_NUM,
    .config = {
        {CSKY_CMP_CTL_BASE, CMP_IRQn},
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_acmp_count()
  \brief       get acmp instance.
  \return      pointer to acmp instance
*/
int32_t target_get_acmp(int32_t idx, uint32_t *base, uint32_t *irq)
{
    return target_get(&sg_acmp_config, idx, base, irq, NULL);
}

int32_t target_get_addr_space(uint32_t addr)
{
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
}
