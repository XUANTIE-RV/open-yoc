/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     2019-12-18
******************************************************************************/

#include <stdio.h>
#include <csi_config.h>
#include <soc.h>
#include <drv/uart.h>
#include <drv/timer.h>
#include <drv/dma.h>
#include <drv/iic.h>
#include <drv/rtc.h>
#include <drv/spi.h>
#include <drv/adc.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include <drv/pin.h>

const csi_perip_info_t g_soc_info[] = {
    {DW_UART0_BASE,            DW_UART0_IRQn,            0,    DEV_DW_UART_TAG},
    {DW_UART1_BASE,            DW_UART1_IRQn,            1,    DEV_DW_UART_TAG},
    {WJ_DMA_BASE,              WJ_DMA_IRQn,              0,    DEV_WJ_DMA_TAG},
    {DW_IIC_BASE,              DW_IIC_IRQn,              0,    DEV_DW_IIC_TAG},
    {DW_TIMER0_BASE,           DW_TIMER0_IRQn,           0,    DEV_DW_TIMER_TAG},
    {DW_TIMER1_BASE,           DW_TIMER1_IRQn,           1,    DEV_DW_TIMER_TAG},
    {DW_TIMER2_BASE,           DW_TIMER2_IRQn,           2,    DEV_DW_TIMER_TAG},
    {DW_TIMER3_BASE,           DW_TIMER3_IRQn,           3,    DEV_DW_TIMER_TAG},
    {DW_TIMER4_BASE,           DW_TIMER4_IRQn,           4,    DEV_DW_TIMER_TAG},
    {DW_TIMER5_BASE,           DW_TIMER5_IRQn,           5,    DEV_DW_TIMER_TAG},
    {DW_TIMER6_BASE,           DW_TIMER6_IRQn,           6,    DEV_DW_TIMER_TAG},
    {DW_TIMER7_BASE,           DW_TIMER7_IRQn,           7,    DEV_DW_TIMER_TAG},
    {DW_WDT_BASE,              DW_WDT_IRQn,              0,    DEV_DW_WDT_TAG},
    {DW_GPIO_BASE,             DW_GPIO_IRQn,             0,    DEV_DW_GPIO_TAG},
    {DW_SPI0_BASE,             DW_SPI0_IRQn,             0,    DEV_DW_SPI_TAG},
    {DW_SPI1_BASE,             DW_SPI1_IRQn,             1,    DEV_DW_SPI_TAG},
    {CD_QSPI_BASE,             CD_QSPI_IRQn,             0,    DEV_CD_QSPI_TAG},
    {WJ_PWM_BASE,              WJ_PWM_IRQn,              0,    DEV_WJ_PWM_TAG},
    {WJ_RTC_BASE,              WJ_RTC_IRQn,              0,    DEV_WJ_RTC_TAG},
    {WJ_I2S0_BASE,             WJ_I2S0_IRQn,             0,    DEV_WJ_I2S_TAG},
    {WJ_I2S1_BASE,             WJ_I2S1_IRQn,             1,    DEV_WJ_I2S_TAG},
    {WJ_I2S2_BASE,             WJ_I2S2_IRQn,             2,    DEV_WJ_I2S_TAG},
    {WJ_I2S3_BASE,             WJ_I2S3_IRQn,             3,    DEV_WJ_I2S_TAG},
    {WJ_I2S4_BASE,             WJ_I2S456_IRQn,           4,    DEV_WJ_I2S_TAG},
    {WJ_I2S5_BASE,             WJ_I2S456_IRQn,           5,    DEV_WJ_I2S_TAG},
    {WJ_I2S6_BASE,             WJ_I2S456_IRQn,           6,    DEV_WJ_I2S_TAG},
    {WJ_ADC_BASE,              WJ_ADC_IRQn,              0,    DEV_WJ_ADC_TAG},
    {WJ_EFUSE_BASE,            -1,                       0,    DEV_WJ_EFUSE_TAG},
    {RCHBAND_CODEC_BASE,       -1,                       0,    DEV_RCHBAND_CODEC_TAG},
    {0, 0, 0, 0}
};

const uint8_t g_dma_chnum[] = {8, 0};

/* DMA handshake number */
/* The member of uart_tx_hs_num is the handshake number for ETB */
const uint16_t uart_tx_hs_num[4] = {82, 114};
const uint16_t uart_rx_hs_num[4] = {83, 115};
const uint16_t iic_tx_hs_num[4]  = {86, 118};
const uint16_t iic_rx_hs_num[4]  = {87, 119};
const uint16_t spi_tx_hs_num[4]  = {80, 112};
const uint16_t spi_rx_hs_num[4]  = {81, 113};
const uint16_t i2s_tx_hs_num[7]  = {55, 57, 59, 61, 63, 65, 67};
const uint16_t i2s_rx_hs_num[7]  = {54, 56, 58, 60, 62, 64, 66};

const csi_dma_ch_desc_t uart0_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t uart1_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t iic0_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t iic1_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t spi0_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t spi1_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};
const csi_dma_ch_desc_t i2s0_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s1_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s2_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s3_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s4_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s5_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s6_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t adc_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
    {0xff, 0xff}
};

const csi_dma_ch_spt_list_t dma_spt_list[] = {
    {DEV_DW_UART_TAG, 0, uart0_dma_ch_list},
    {DEV_DW_UART_TAG, 1, uart1_dma_ch_list},
    {DEV_DW_IIC_TAG,  0, iic0_dma_ch_list},
    {DEV_DW_IIC_TAG,  1, iic1_dma_ch_list},
    {DEV_DW_SPI_TAG,  0, spi0_dma_ch_list},
    {DEV_DW_SPI_TAG,  1, spi1_dma_ch_list},
    {DEV_WJ_ADC_TAG,  0, adc_dma_ch_list},
    {DEV_WJ_I2S_TAG,  0, i2s0_dma_ch_list},
    {DEV_WJ_I2S_TAG,  1, i2s1_dma_ch_list},
    {DEV_WJ_I2S_TAG,  2, i2s2_dma_ch_list},
    {DEV_WJ_I2S_TAG,  3, i2s3_dma_ch_list},
    {DEV_WJ_I2S_TAG,  4, i2s4_dma_ch_list},
    {DEV_WJ_I2S_TAG,  5, i2s5_dma_ch_list},
    {DEV_WJ_I2S_TAG,  6, i2s6_dma_ch_list},
    {0xFFFFU,         0xFFU,         NULL},
};

const csi_pinmap_t adc_pinmap[] = {
    {PA3,           0,     1,           PA3_ADC_A1 },
    {PA4,           0,     2,           PA4_ADC_A2 },
    {PA5,           0,     3,           PA5_ADC_A3 },
    {PA6,           0,     4,           PA6_ADC_A4 },
    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU },
};

const csi_pinmap_t pwm_pinmap[] = {
    {PA0,    0,    0,    PA0_PWM_CH0  },
    {PA1,    0,    1,    PA1_PWM_CH1  },
    {PA2,    0,    2,    PA2_PWM_CH2  },
    {PA3,    0,    3,    PA3_PWM_CH3  },
    {PA4,    0,    4,    PA4_PWM_CH4  },
    {PA5,    0,    5,    PA5_PWM_CH5  },
    {PA6,    0,    6,    PA6_PWM_CH6  },
    {PA7,    0,    7,    PA7_PWM_CH7  },
    {PA8,    0,    8,    PA8_PWM_CH8  },
    {PA9,    0,    9,    PA9_PWM_CH9  },
    {PA15,   0,    4,    PA15_PWM_CH4 },
    {PA16,   0,    5,    PA16_PWM_CH5 },
    {PA17,   0,   10,    PA17_PWM_CH10},
    {PA18,   0,   11,    PA18_PWM_CH11},
    {PA23,   0,    0,    PA23_PWM_CH0 },
    {PA24,   0,    1,    PA24_PWM_CH1 },
    {PA25,   0,    2,    PA25_PWM_CH2 },
    {PA26,   0,    3,    PA26_PWM_CH3 },
    {PA27,   0,    4,    PA27_PWM_CH4 },
    {PA28,   0,    5,    PA28_PWM_CH5 },
    {PA29,   0,    6,    PA29_PWM_CH6 },
    {PA30,   0,    7,    PA30_PWM_CH7 },
    {PA31,   0,    8,    PA31_PWM_CH8 },
    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU },
};

const csi_pinmap_t gpio_pinmap[] = {
    {PA0,           0,     0,           PIN_FUNC_GPIO },
    {PA1,           0,     1,           PIN_FUNC_GPIO },
    {PA2,           0,     2,           PIN_FUNC_GPIO },
    {PA3,           0,     3,           PIN_FUNC_GPIO },
    {PA4,           0,     4,           PIN_FUNC_GPIO },
    {PA5,           0,     5,           PIN_FUNC_GPIO },
    {PA6,           0,     6,           PIN_FUNC_GPIO },
    {PA7,           0,     7,           PIN_FUNC_GPIO },
    {PA8,           0,     8,           PIN_FUNC_GPIO },
    {PA9,           0,     9,           PIN_FUNC_GPIO },
    {PA10,          0,    10,           PIN_FUNC_GPIO },
    {PA11,          0,    11,           PIN_FUNC_GPIO },
    {PA12,          0,    12,           PIN_FUNC_GPIO },
    {PA13,          0,    13,           PIN_FUNC_GPIO },
    {PA14,          0,    14,           PIN_FUNC_GPIO },
    {PA15,          0,    15,           PIN_FUNC_GPIO },
    {PA16,          0,    16,           PIN_FUNC_GPIO },
    {PA17,          0,    17,           PIN_FUNC_GPIO },
    {PA18,          0,    18,           PIN_FUNC_GPIO },
    {PA19,          0,    19,           PIN_FUNC_GPIO },
    {PA20,          0,    20,           PIN_FUNC_GPIO },
    {PA21,          0,    21,           PIN_FUNC_GPIO },
    {PA22,          0,    22,           PIN_FUNC_GPIO },
    {PA23,          0,    23,           PIN_FUNC_GPIO },
    {PA24,          0,    24,           PIN_FUNC_GPIO },
    {PA25,          0,    25,           PIN_FUNC_GPIO },
    {PA26,          0,    26,           PIN_FUNC_GPIO },
    {PA27,          0,    27,           PIN_FUNC_GPIO },
    {PA28,          0,    28,           PIN_FUNC_GPIO },
    {PA29,          0,    29,           PIN_FUNC_GPIO },
    {PA30,          0,    30,           PIN_FUNC_GPIO },
    {PA31,          0,    31,           PIN_FUNC_GPIO },
    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU   },
};

const csi_pinmap_t uart_pinmap[] = {
    {PA0,             0,     PIN_UART_RX,    PA0_UART0_RX   },
    {PA1,             0,     PIN_UART_TX,    PA1_UART0_TX   },
    {PA4,             1,    PIN_UART_CTS,    PA4_UART1_CTS  },
    {PA5,             1,    PIN_UART_RTS,    PA5_UART1_RTS  },
    {PA6,             0,     PIN_UART_TX,    PA6_UART0_TX   },
    {PA8,             0,     PIN_UART_RX,    PA8_UART0_RX   },
    {PA10,            0,     PIN_UART_TX,    PA10_UART0_TX  },
    {PA16,            1,     PIN_UART_TX,    PA16_UART1_TX  },
    {PA17,            0,     PIN_UART_RX,    PA17_UART0_RX  },
    {PA18,            0,     PIN_UART_TX,    PA18_UART0_TX  },
    {PA19,            1,     PIN_UART_RX,    PA19_UART1_RX  },
    {PA20,            1,     PIN_UART_TX,    PA20_UART1_TX  },
    {PA21,            0,    PIN_UART_CTS,    PA21_UART0_CTS },
    {PA21,            1,    PIN_UART_CTS,    PA21_UART1_CTS },
    {PA22,            0,    PIN_UART_RTS,    PA22_UART0_RTS },
    {PA22,            1,    PIN_UART_RTS,    PA22_UART1_RTS },
    {PA23,            0,     PIN_UART_TX,    PA23_UART0_TX  },
    {PA24,            0,     PIN_UART_RX,    PA24_UART0_RX  },
    {PA25,            0,    PIN_UART_CTS,    PA25_UART0_CTS },
    {PA26,            0,    PIN_UART_RTS,    PA26_UART0_RTS },
    {PA27,            1,     PIN_UART_RX,    PA27_UART1_RX  },
    {PA28,            1,     PIN_UART_TX,    PA28_UART1_TX  },
    {PA29,            1,    PIN_UART_CTS,    PA29_UART1_CTS },
    {PA30,            1,    PIN_UART_RTS,    PA30_UART1_RTS },
    {PA31,            0,     PIN_UART_TX,    PA31_UART0_TX  },
    {0xFFFFFFFFU, 0xFFU, 0xFFU,      0xFFFFFFFFU  },
};

const csi_pinmap_t iic_pinmap[] = {
    {PA8,             0, PIN_IIC_SCL, PA8_IIC0_SCL},
    {PA9,             0, PIN_IIC_SDA, PA9_IIC0_SDA},
    {PA15,            0, PIN_IIC_SCL, PA15_IIC0_SCL},
    {PA16,            0, PIN_IIC_SDA, PA16_IIC0_SDA},
    {0xFFFFFFFFU, 0xFFU,       0xFFU,   0xFFFFFFFFU},
};


const csi_pinmap_t spi_pinmap[] = {
    {PA0,             0,    PIN_SPI_CS,     PA0_SPI0_CS},
    {PA1,             0,    PIN_SPI_SCK,    PA1_SPI0_SCK},
    {PA2,             1,    PIN_SPI_CS,     PA2_SPI1_CS},
    {PA3,             1,    PIN_SPI_SCK,    PA3_SPI1_SCK},
    {PA4,             0,    PIN_SPI_MOSI,   PA4_SPI0_MOSI},
    {PA5,             0,    PIN_SPI_MISO,   PA5_SPI0_MISO},
    {PA6,             1,    PIN_SPI_MOSI,   PA6_SPI1_MOSI},
    {PA7,             1,    PIN_SPI_MISO,   PA7_SPI1_MISO},
    {PA23,            0,    PIN_SPI_CS,     PA23_SPI0_CS},
    {PA24,            0,    PIN_SPI_SCK,    PA24_SPI0_SCK},
    {PA25,            0,    PIN_SPI_MOSI,   PA25_SPI0_MOSI},
    {PA26,            0,    PIN_SPI_MISO,   PA26_SPI0_MISO},
    {PA27,            1,    PIN_SPI_CS,     PA27_SPI1_CS},
    {PA28,            1,    PIN_SPI_SCK,    PA28_SPI1_SCK},
    {PA29,            1,    PIN_SPI_MOSI,   PA29_SPI1_MOSI},
    {PA30,            1,    PIN_SPI_MISO,   PA30_SPI1_MISO},
    {0xFFFFFFFFU, 0xFFU,        0xFFU,    0xFFFFFFFFU},
};

const csi_pinmap_t i2s_pinmap[] = {
    {PA2,             1,      PIN_I2S_SCLK, PA2_I2S1_SCLK },
    {PA3,             1,     PIN_I2S_WSCLK, PA3_I2S1_WSCLK},
    {PA5,             1,       PIN_I2S_SDA, PA5_I2S1_SDA  },
    {PA6,             0,      PIN_I2S_SCLK, PA6_I2S0_SCLK },
    {PA7,             0,     PIN_I2S_WSCLK, PA7_I2S0_WSCLK},
    {PA8,             0,       PIN_I2S_SDA, PA8_I2S0_SDA  },
    {PA10,            0,      PIN_I2S_MCLK, PA10_I2S0_MCLK},
    {PA26,            1,      PIN_I2S_MCLK, PA26_I2S1_MCLK},
    {0xFFFFFFFFU, 0xFFU, 0xFFU, 0xFFFFFFFFU   },
};

const csi_clkmap_t clk_map[] = {
    {TIM0_CLK,   DEV_DW_TIMER_TAG,      0},
    {TIM1_CLK,   DEV_DW_TIMER_TAG,      1},
    {RTC0_CLK,   DEV_WJ_RTC_TAG,        0},
    {WDT_CLK,    DEV_DW_WDT_TAG,        0},
    {SPI0_CLK,   DEV_DW_SPI_TAG,        0},
    {UART0_CLK,  DEV_DW_UART_TAG,       0},
    {IIC0_CLK,   DEV_DW_IIC_TAG,        0},
    {PWM_CLK,    DEV_WJ_PWM_TAG,        0},
    {QSPI0_CLK,  DEV_CD_QSPI_TAG,       0},
    {PWMR_CLK,   DEV_BLANK_TAG,         0},
    {EFUSE_CLK,  DEV_WJ_EFUSE_TAG,      0},
    {I2S0_CLK,   DEV_WJ_I2S_TAG,        0},
    {I2S1_CLK,   DEV_WJ_I2S_TAG,        1},
    {GPIO0_CLK,  DEV_DW_GPIO_TAG,       0},
    {TIM2_CLK,   DEV_DW_TIMER_TAG,      2},
    {TIM3_CLK,   DEV_DW_TIMER_TAG,      3},
    {SPI1_CLK,   DEV_DW_SPI_TAG,        1},
    {UART1_CLK,  DEV_DW_UART_TAG,       1},
    {I2S567_CLK, DEV_WJ_I2S_TAG,        5},
    {I2S567_CLK, DEV_WJ_I2S_TAG,        6},
    {I2S567_CLK, DEV_WJ_I2S_TAG,        7},
    {ADC_CLK,    DEV_WJ_ADC_TAG,        0},
    {ETB_CLK,    DEV_WJ_ETB_TAG,        0},
    {I2S2_CLK,   DEV_WJ_I2S_TAG,        2},
    {IOC_CLK,    DEV_BLANK_TAG,         0},
    {CODEC_CLK,  DEV_RCHBAND_CODEC_TAG, 0},
    {0xFFFFFFFFU, 0xFFFFU,          0xFFU}
};
