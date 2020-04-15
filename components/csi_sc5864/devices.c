/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <csi_config.h>
#include <soc.h>
#include <drv/usart.h>
#include <drv/timer.h>
#include <drv/gpio.h>
#include <pin_name.h>
#include <silan_pic.h>
#include <drv/spiflash.h>

extern int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_usart_config[CONFIG_USART_NUM] = {
    {SILAN_UART1_BASE, PIC_SUBID_MISC_UART1},
    {SILAN_UART2_BASE, PIC_SUBID_MISC_UART2},
    {SILAN_UART3_BASE, PIC_SUBID_MISC_UART3},
    {SILAN_UART4_BASE, PIC_SUBID_MISC_UART4},
};
typedef struct {
    int32_t    tx;
    int32_t    rx;
    int32_t    cts;
    int32_t    rts;
    uint16_t cfg_idx;    //idx of sg_usart_config[]
    uint16_t function;
} usart_pin_map_t;
const static usart_pin_map_t s_usart_pin_map[] = {
    {
        PB0,
        PB1,
        PB2,
        PB3,
        0,
        1
    },
    {
        PA4,
        PA5,
        -1,
        -1,
        1,
        3
    },
    {
        PD7,
        PD8,
        -1,
        -1,
        2,
        1
    },
    {
        PD9,
        PD10,
        -1,
        -1,
        3,
        2
    },
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_usart_count()
  \brief       get usart instance.
  \return      pointer to usart instance
*/
int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= CONFIG_USART_NUM) {
        return -1;
    }

    *base = sg_usart_config[idx].base;
    *irq = sg_usart_config[idx].irq;
    return idx;
}

/**
  \brief       control usart flow.
  \param[in]   tx_flow  The TX flow pin name
  \param[in]   rx_flow  The RX flow pin name
  \param[in]   flag 0-disable, 1-enable.
  \return      0 if setting ready ,negative for error code
*/
int32_t target_usart_tx_flowctrl_init(int32_t idx, uint32_t flag)
{
    if (idx >= sizeof(s_usart_pin_map) / sizeof(usart_pin_map_t)) {
        return -1;
    }

    if (flag) {
        drv_pinmux_config(s_usart_pin_map[idx].cts, s_usart_pin_map[idx].function);
    } else if (flag == 0) {
        drv_pinmux_config(s_usart_pin_map[idx].cts, 0xff);
    }

    return 0;
}

/**
  \brief       control usart flow.
  \param[in]   tx_flow  The TX flow pin name
  \param[in]   rx_flow  The RX flow pin name
  \param[in]   flag 0-disable, 1-enable.
  \return      0 if setting ready ,negative for error code
*/
int32_t target_usart_rx_flowctrl_init(int32_t idx, uint32_t flag)
{
    if (idx >= sizeof(s_usart_pin_map) / sizeof(usart_pin_map_t)) {
        return -1;
    }

    if (flag) {
        drv_pinmux_config(s_usart_pin_map[idx].rts, s_usart_pin_map[idx].function);
    } else if (flag == 0) {
        drv_pinmux_config(s_usart_pin_map[idx].rts, 0xff);
    }

    return 0;
}


struct {
    uint32_t base;
    uint32_t irq;
    uint32_t pin_num;
    port_name_e port;
}
const sg_gpio_config[CONFIG_GPIO_NUM] = {
    {SILAN_GPIO1_BASE, GPIO1_IRQn, 23, PORTA},
    {SILAN_GPIO2_BASE, GPIO2_IRQn, 19, PORTB},
};

typedef struct {
    int32_t    gpio_pin;
    uint32_t cfg_idx;    //idx of sg_gpio_config[]
} gpio_pin_map_t;
const static gpio_pin_map_t s_gpio_pin_map[] = {
    {PA0, 1},
    {PA1, 1},
    {PA2, 1},
    {PA3, 1},
    {PA4, 1},
    {PA5, 1},
    {PA6, 1},
    {PA7, 1},

    {PB0, 0},
    {PB1, 0},
    {PB2, 0},
    {PB3, 0},
    {PB4, 0},
    {PB5, 0},
    {PB6, 0},
    {PB7, 0},
    {PB8, 0},

    {PC0, 0},
    {PC1, 0},
    {PC2, 0},
    {PC3, 0},
    {PC4, 0},
    {PC5, 0},
    {PC6, 0},
    {PC7, 0},
    {PC8, 0},
    {PC9, 0},
    {PC10, 0},
    {PC11, 0},
    {PC12, 0},
    {PC13, 0},

    {PD0, 1},
    {PD1, 1},
    {PD2, 1},
    {PD3, 1},
    {PD4, 1},
    {PD5, 1},
    {PD6, 1},
    {PD7, 1},
    {PD8, 1},
    {PD9, 1},
    {PD10, 1},
};

int32_t target_gpio_port_init(port_name_e port, uint32_t *base, uint32_t *irq, uint32_t *pin_num)
{
    int i;

    for (i = 0; i < CONFIG_GPIO_NUM; i++) {
        if (sg_gpio_config[i].port == port) {
            *base = sg_gpio_config[i].base;
            *irq = sg_gpio_config[i].irq;
            *pin_num = sg_gpio_config[i].pin_num;
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
            return idx;
        }
    }

    return -1;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_timer_config[CONFIG_TIMER_NUM] = {
    {SILAN_TIMER_BASE, PIC_SUBID_TIMER_0},
    {SILAN_TIMER_BASE + 0x10, PIC_SUBID_TIMER_1},
    {SILAN_TIMER_BASE + 0x20, PIC_SUBID_TIMER_2},
    {SILAN_TIMER_BASE + 0x30, PIC_SUBID_TIMER_3},
    {SILAN_TIMER_BASE + 0x40, PIC_SUBID_TIMER_4}

};

int32_t target_get_timer_count(void)
{
    return CONFIG_TIMER_NUM;
}

int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_timer_count()) {
        return -1;
    }

    *base = sg_timer_config[idx].base;
    *irq = sg_timer_config[idx].irq;
    return idx;
}

struct {
    spiflash_info_t info;
}
const sg_spiflash_mem_config[CONFIG_SPIFLASH_NUM] = {
    {{0x00000000, 0x007fffff, 0x1000}}
};

/**
  \param[in]   instance idx, device id
  \brief       get spiflash instance.
  \return      pointer to spiflash instance
*/
int32_t target_get_spiflash(int32_t idx, spiflash_info_t *info)
{
    info->start = sg_spiflash_mem_config[idx].info.start;
    info->end = sg_spiflash_mem_config[idx].info.end;
    info->sector_count = sg_spiflash_mem_config[idx].info.sector_count;
    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_spiflash_config[CONFIG_SPIFLASH_NUM] = {
    {SILAN_SF_CFG_BASE, SPIFLASH_IRQn}
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_qspi_count()
  \brief       get qspi instance.
  \return      pointer to qspi instance
*/
int32_t target_spiflash_init(int32_t idx, uint32_t *base, uint32_t *irq)
{
    *base = sg_spiflash_config[idx].base;
    *irq = sg_spiflash_config[idx].irq;

    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_iic_config[CONFIG_IIC_NUM] = {
    {SILAN_I2C1_BASE, PIC_SUBID_MISC_I2C1},
    {SILAN_I2C2_BASE, PIC_SUBID_MISC_I2C2}
};

int32_t target_get_iic_count(void)
{
    return CONFIG_IIC_NUM;
}


/**
  \param[in]   instance idx, must not exceed return value of target_get_iic_count()
  \brief       get iic instance.
  \return      pointer to iic instance
*/
int32_t target_iic_init(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_iic_count()) {
        return -1;
    }

    *base = sg_iic_config[idx].base;
    *irq = sg_iic_config[idx].irq;
    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_adc_config[CONFIG_ADC_NUM] = {
    {SILAN_ADC_BASE, 0}
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
  \brief       get adc instance count.
  \return      adc instance count
*/
int32_t target_get_adc_count(void)
{
    return CONFIG_ADC_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_adc_count()
  \brief       get sha instance.
  \return      pointer to adc instance
*/
int32_t target_get_adc(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_adc_count()) {
        return -1;
    }

    *base = sg_adc_config[idx].base;
    *irq = sg_adc_config[idx].irq;
    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_pwm_config[CONFIG_PWM_NUM] = {
    {SILAN_PWM_BASE, PIC_IRQID_NONE},
};

/**
  \brief       get pwm instance count.
  \return      sha instance count
*/
int32_t target_get_pwm_count(void)
{
    return CONFIG_PWM_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_pwm_count()
  \brief       get pwm instance.
  \return      pointer to pwm instance
*/
int32_t target_pwm_init(uint32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_pwm_count()) {
        return -1;
    }

    *base = sg_pwm_config[idx].base;
    *irq = sg_pwm_config[idx].irq;
    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_rtc_config[CONFIG_RTC_NUM] = {
    {SILAN_RTC_BASE, RTC_IRQn},

};

int32_t target_get_rtc_count(void)
{
    return CONFIG_RTC_NUM;
}

int32_t target_get_rtc(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_rtc_count()) {
        return -1;
    }

    *base = sg_rtc_config[idx].base;
    *irq = sg_rtc_config[idx].irq;
    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_wdt_config[CONFIG_WDT_NUM] = {
    {SILAN_RISC_WDOG_BASE, 30}
};

int32_t target_get_wdt_count(void)
{
    return CONFIG_WDT_NUM;
}

int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_wdt_count()) {
        return -1;
    }

    *base = sg_wdt_config[idx].base;
    *irq = sg_wdt_config[idx].irq;
    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
} const sg_spi_config[CONFIG_SPI_NUM] = {
    {SILAN_SPI1_BASE, PIC_SUBID_MISC_SPI1},
    {SILAN_SPI2_BASE, PIC_SUBID_MISC_SPI2}
};
int32_t target_get_spi_count(void)
{
    return CONFIG_SPI_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_spi_count()
  \brief       get spi instance.
  \return      pointer to spi instance
*/
int32_t target_spi_init(int32_t idx, uint32_t *base, uint32_t *irq, uint32_t *ssel)
{
    if (idx >=  target_get_spi_count()) {
        return -1;
    }

    *base = sg_spi_config[idx].base;
    *irq = sg_spi_config[idx].irq;

    return idx;
}

