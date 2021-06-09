/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     26. Dec 2019
 ******************************************************************************/

#include <stdio.h>
#include <csi_config.h>
#include <soc.h>
#include <drv/dma.h>
#include <drv/pin.h>
#include "sys_tipc.h"

const csi_perip_info_t g_soc_info[] = {
    {DW_UART0_BASE, DW_UART0_HS_IRQn,    0, DEV_DW_UART_TAG},
    {DW_UART1_BASE, DW_UART1_HS_IRQn,    1, DEV_DW_UART_TAG},
    {DW_UART2_BASE, DW_UART2_IRQn,       2, DEV_DW_UART_TAG},
    {DW_UART3_BASE, DW_UART3_IRQn,       3, DEV_DW_UART_TAG},
    {WJ_USI0_BASE,  WJ_USI0_IRQn,        0, DEV_WJ_USI_TAG},
    {WJ_USI1_BASE,  WJ_USI1_IRQn,        1, DEV_WJ_USI_TAG},
    {WJ_USI2_BASE,  WJ_USI2_IRQn,        2, DEV_WJ_USI_TAG},
    {WJ_USI3_BASE,  WJ_USI3_IRQn,        3, DEV_WJ_USI_TAG},
    {WJ_RSA_BASE,   WJ_RSA_IRQn,         0, DEV_WJ_RSA_TAG},
    {WJ_MCC0_BASE,  WJ_WCOM_IRQn,        0, DEV_WJ_MBOX_TAG},
    {DW_DMAC0_BASE, DW_DMAC0_IRQn,       0, DEV_DW_DMA_TAG},
    {DW_DMAC1_BASE, DW_DMAC1_IRQn,       1, DEV_DW_DMA_TAG},
    {WJ_SHA_BASE,   WJ_SHA_IRQn,         0, DEV_WJ_SHA_TAG},
    {DW_GPIO0_BASE, DW_GPIO0_IRQn,       0, DEV_DW_GPIO_TAG},
    {DW_GPIO1_BASE, DW_GPIO1_IRQn,       1, DEV_DW_GPIO_TAG},
    {DW_TIM0_BASE,  DW_TIMER0_INT0_IRQn, 0, DEV_DW_TIMER_TAG},
    {DW_TIM1_BASE,  DW_TIMER0_INT1_IRQn, 1, DEV_DW_TIMER_TAG},
    {WJ_LPTIM6_BASE,  WJ_LPTIMER_INT0_IRQn, 2, DEV_DW_TIMER_TAG},
    {WJ_LPTIM7_BASE,  WJ_LPTIMER_INT1_IRQn, 3, DEV_DW_TIMER_TAG},
    {DW_TIM2_BASE,  DW_TIMER1_INT0_IRQn, 4, DEV_DW_TIMER_TAG},
    {DW_TIM3_BASE,  DW_TIMER1_INT1_IRQn, 5, DEV_DW_TIMER_TAG},
    {DW_TIM4_BASE,  DW_TIMER2_INT0_IRQn, 6, DEV_DW_TIMER_TAG},
    {DW_TIM5_BASE,  DW_TIMER2_INT1_IRQn, 7, DEV_DW_TIMER_TAG},
    {WJ_AES_BASE,   WJ_AES_IRQn,         0, DEV_WJ_AES_TAG},
    {DW_WDT_BASE,   DW_WDT_IRQn,         0, DEV_DW_WDT_TAG},
    {CD_QSPIC0_BASE, CD_QSPI_IRQn,       0, DEV_CD_QSPI_TAG},
    {WJ_PWM0_BASE,  WJ_PWM0_IRQn,        0, DEV_WJ_PWM_TAG},
    {WJ_PWM1_BASE,  WJ_PWM1_IRQn,        1, DEV_WJ_PWM_TAG},
    {WJ_RTC_BASE,   WJ_RTC_IRQn,         0, DEV_WJ_RTC_TAG},
    {WJ_I2S0_BASE,  WJ_I2S0_IRQn,        0, DEV_WJ_I2S_TAG},
    {WJ_I2S1_BASE,  WJ_I2S1_IRQn,        1, DEV_WJ_I2S_TAG},
    {WJ_I2S2_BASE,  WJ_I2S2_IRQn,        2, DEV_WJ_I2S_TAG},
    {WJ_I2S3_BASE,  WJ_I2S3_IRQn,        3, DEV_WJ_I2S_TAG},
    {DW_SDIO0_BASE, WJ_SDIO0_IRQn,       0, DEV_DW_SDMMC_TAG},
    {DW_SDIO1_BASE, WJ_SDIO1_IRQn,       1, DEV_DW_SDMMC_TAG},
    {WJ_CODEC_BASE, WJ_CODEC_INTC_IRQn,  0, DEV_WJ_CODEC_TAG},
    {WJ_EFUSE_BASE, 0,                   0, DEV_WJ_EFUSE_TAG},
    {0, 0, 0, 0}
};

const uint8_t g_dma_chnum[] = {4, 8};

/* DMA handshake number */
/* The member of uart_tx_hs_num is the handshake number for ETB */
const uint16_t uart_tx_hs_num[2] = {8, 10};
const uint16_t uart_rx_hs_num[2] = {9, 11};
const uint16_t usi_tx_hs_num[3]  = {4, 6, 18};
const uint16_t usi_rx_hs_num[3]  = {5, 7, 19};
const uint16_t i2s_tx_hs_num[4]  = {0, 1, 20, 26};
const uint16_t i2s_rx_hs_num[4]  = {1, 3, 21, 27};
const uint16_t codec_tx_hs_num[1]  = {24};
const uint16_t codec_rx_hs_num[8]  = {12, 13, 50, 51, 14, 15, 22, 23};
const csi_dma_ch_desc_t uart0_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2}, {0, 3},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t uart1_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2}, {0, 3},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t uart2_dma_ch_list[] = {
    {1, 0}, {1, 1}, {1, 2}, {1, 3},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t usi0_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2}, {0, 3},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t usi1_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2}, {0, 3},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t usi2_dma_ch_list[] = {
    {1, 0}, {1, 1}, {1, 2}, {1, 3},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s0_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2}, {0, 3},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s1_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2}, {0, 3},
    {0xff, 0xff}
};

const csi_dma_ch_desc_t i2s2_dma_ch_list[] = {
    {1, 0}, {1, 1}, {1, 2}, {1, 3},
    {0xff, 0xff}
};
const csi_dma_ch_desc_t codec_dma_ch_list[] = {
    {1, 0}, {1, 1}, {1, 2}, {1, 3},
    {0xff, 0xff}
};
const csi_dma_ch_spt_list_t dma_spt_list[] = {
    {DEV_DW_UART_TAG, 0, uart0_dma_ch_list},
    {DEV_DW_UART_TAG, 1, uart1_dma_ch_list},
    {DEV_DW_UART_TAG, 2, uart2_dma_ch_list},
    {DEV_WJ_USI_TAG,  0, usi0_dma_ch_list},
    {DEV_WJ_USI_TAG,  1, usi1_dma_ch_list},
    {DEV_WJ_USI_TAG,  2, usi2_dma_ch_list},
    {DEV_WJ_I2S_TAG,  0, i2s0_dma_ch_list},
    {DEV_WJ_I2S_TAG,  0, i2s0_dma_ch_list},
    {DEV_WJ_I2S_TAG,  1, i2s1_dma_ch_list},
    {DEV_WJ_I2S_TAG,  2, i2s2_dma_ch_list},
    {DEV_WJ_CODEC_TAG, 0, codec_dma_ch_list},
    {0xFFFFU,         0xFFU,         NULL},
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
    {PB0,           1,     0,           PIN_FUNC_GPIO },
    {PB1,           1,     1,           PIN_FUNC_GPIO },
    {PB2,           1,     2,           PIN_FUNC_GPIO },
    {PB3,           1,     3,           PIN_FUNC_GPIO },
    {PB4,           1,     4,           PIN_FUNC_GPIO },
    {PB5,           1,     5,           PIN_FUNC_GPIO },
    {PB6,           1,     6,           PIN_FUNC_GPIO },
    {PB7,           1,     7,           PIN_FUNC_GPIO },
    {PB8,           1,     8,           PIN_FUNC_GPIO },
    {PB9,           1,     9,           PIN_FUNC_GPIO },
    {PB10,          1,    10,           PIN_FUNC_GPIO },
    {PB11,          1,    11,           PIN_FUNC_GPIO },
    {PB12,          1,    12,           PIN_FUNC_GPIO },
    {PB13,          1,    13,           PIN_FUNC_GPIO },
    {PB14,          1,    14,           PIN_FUNC_GPIO },
    {PB15,          1,    15,           PIN_FUNC_GPIO },
    {PB16,          1,    16,           PIN_FUNC_GPIO },
    {PB17,          1,    17,           PIN_FUNC_GPIO },
    {PB18,          1,    18,           PIN_FUNC_GPIO },
    {PB19,          1,    19,           PIN_FUNC_GPIO },
    {PB20,          1,    20,           PIN_FUNC_GPIO },
    {PB21,          1,    21,           PIN_FUNC_GPIO },
    {PB22,          1,    22,           PIN_FUNC_GPIO },
    {PB23,          1,    23,           PIN_FUNC_GPIO },
    {PB24,          1,    24,           PIN_FUNC_GPIO },
    {PB25,          1,    25,           PIN_FUNC_GPIO },
    {PB26,          1,    26,           PIN_FUNC_GPIO },
    {PB27,          1,    27,           PIN_FUNC_GPIO },
    {PB28,          1,    28,           PIN_FUNC_GPIO },
    {PB29,          1,    29,           PIN_FUNC_GPIO },
    {PB30,          1,    30,           PIN_FUNC_GPIO },
    {PB31,          1,    31,           PIN_FUNC_GPIO },
    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU   },
};

const csi_pinmap_t pwm_pinmap[] = {
    {PA26,           0,     0,           PA26_PWM0_IO0 },
    {PA20,           0,     1,           PA20_PWM0_O1  },
    {PA27,           0,     2,           PA27_PWM0_IO2 },
    {PA21,           0,     3,           PA21_PWM0_O3  },
    {PA28,           0,     4,           PA28_PWM0_IO4 },
    {PA22,           0,     5,           PA22_PWM0_O5  },
    {PA29,           0,     6,           PA29_PWM0_IO6 },
    {PA23,           0,     7,           PA23_PWM0_O7  },
    {PA30,           0,     8,           PA30_PWM0_IO8 },
    {PA24,           0,     9,           PA24_PWM0_O9  },
    {PA31,           0,     10,          PA31_PWM0_IO10},
    {PA25,           0,     11,          PA25_PWM0_O11 },
    {PB26,           1,      0,          PB26_PWM1_IO0 },
    {PB20,           1,      1,          PB20_PWM1_O1  },
    {PB27,           1,      2,          PB27_PWM1_IO2 },
    {PB21,           1,      3,          PB21_PWM1_O3  },
    {PB28,           1,      4,          PB28_PWM1_IO4 },
    {PB22,           1,      5,          PB22_PWM1_O5  },
    {PB29,           1,      6,          PB29_PWM1_IO6 },
    {PB23,           1,      7,          PB23_PWM1_O7  },
    {PB30,           1,      8,          PB30_PWM1_IO8 },
    {PB24,           1,      9,          PB24_PWM1_O9  },
    {PB31,           1,      10,         PB31_PWM1_IO10},
    {PB25,           1,      11,         PB25_PWM1_O11 },
    {0xFFFFFFFFU,    0xFFU, 0xFFU,       0xFFFFFFFFU   },
};

const csi_pinmap_t uart_pinmap[] = {
    {PA0,             0,     PIN_UART_RX,    PA0_UART0_RX   },
    {PA1,             0,     PIN_UART_RTS,   PA1_UART0_RTS  },
    {PA2,             0,     PIN_UART_TX,    PA2_UART0_TX   },
    {PA29,            0,     PIN_UART_CTS,   PA29_UART0_CTS },
    {PA19,            1,     PIN_UART_TX,    PA19_UART1_TX  },
    {PA20,            1,     PIN_UART_RX,    PA20_UART1_RX  },
    {PB25,            2,     PIN_UART_RX,    PB25_UART2_RX  },
    {PB24,            2,     PIN_UART_TX,    PB24_UART2_TX  },
    {PB26,            2,     PIN_UART_RTS,   PB26_UART2_RTS },
    {PB0,             3,     PIN_UART_TX,    PB0_UART3_TX   },
    {PB1,             3,     PIN_UART_RX,    PB1_UART3_RX   },
    {0xFFFFFFFFU,     0xFFU, 0xFFU,          0xFFFFFFFFU    },
};

const csi_pinmap_t iic_pinmap[] = {
    {PA21,        0,           PIN_IIC_SCL,   PA21_USI0_SCLK},
    {PA22,        0,           PIN_IIC_SDA,   PA22_USI0_SD0},
    {PA26,        1,           PIN_IIC_SCL,   PA26_USI1_SCLK},
    {PA27,        1,           PIN_IIC_SDA,   PA27_USI1_SD0},
    {PB28,        2,           PIN_IIC_SCL,   PB28_USI2_SCLK},
    {PB29,        2,           PIN_IIC_SDA,   PB29_USI2_SD0},
    {PB4,         3,           PIN_IIC_SCL,   PB4_USI3_SCLK},
    {PB5,         3,           PIN_IIC_SDA,   PB5_USI3_SD0},
    {0xFFFFFFFFU, 0xFFU,       0xFFU,   0xFFFFFFFFU},
};

const csi_pinmap_t spi_pinmap[] = {
    {PA21,         0,           PIN_SPI_SCK,    PA21_USI0_SCLK},
    {PA22,         0,           PIN_SPI_MOSI,   PA22_USI0_SD0},
    {PA30,         0,           PIN_SPI_MISO,   PA30_USI0_SD1},
    {PA31,         0,           PIN_SPI_CS,     PA31_USI0_NSS},
    {PA26,         1,           PIN_SPI_SCK,    PA26_USI1_SCLK},
    {PA27,         1,           PIN_SPI_MOSI,   PA27_USI1_SD0},
    {PA28,         1,           PIN_SPI_MISO,   PA28_USI1_SD1},
    {PA29,         1,           PIN_SPI_CS,     PA29_USI1_NSS},
    {PB28,         2,           PIN_SPI_SCK,    PB28_USI2_SCLK},
    {PB29,         2,           PIN_SPI_MOSI,   PB29_USI2_SD0},
    {PB30,         2,           PIN_SPI_MISO,   PB30_USI2_SD1},
    {PB31,         2,           PIN_SPI_CS,     PB31_USI2_NSS},
    {0xFFFFFFFFU, 0xFFU,        0xFFU,    0xFFFFFFFFU},
};

const csi_pinmap_t i2s_pinmap[] = {
    {PA3,             0,      PIN_I2S_SCLK, PA3_I2S0_SCLK   },
    {PA4,             0,      PIN_I2S_WSCLK, PA4_I2S0_WSCLK  },
    {PA5,             0,      PIN_I2S_SDI,  PA5_I2S0_SDI    },
    {PA6,             0,      PIN_I2S_SDO,  PA6_I2S0_SDO    },
    {PA23,            1,      PIN_I2S_SCLK, PA23_I2S1_SCLK  },
    {PA24,            1,      PIN_I2S_WSCLK, PA24_I2S1_WSCLK },
    {PA25,            1,      PIN_I2S_SDA,  PA25_I2S1_SDA   },
    {PA26,            1,      PIN_I2S_MCLK, PA26_I2S1_MCLK  },
    {PB0,             2,      PIN_I2S_SCLK, PB0_I2S2_SCLK   },
    {PB1,             2,      PIN_I2S_WSCLK, PB1_I2S2_WSCLK  },
    {PB2,             2,      PIN_I2S_SDA,  PB2_I2S2_SDA    },
    {PB3,             2,      PIN_I2S_MCLK, PB3_I2S2_MCLK   },
    {PB4,             2,      PIN_I2S_SCLK, PB4_I2S3_SCLK   },
    {PB5,             2,      PIN_I2S_WSCLK, PB5_I2S3_WSCLK  },
    {PB6,             2,      PIN_I2S_SDA,  PB6_I2S3_SDA    },
    {PB7,             2,      PIN_I2S_MCLK, PB7_I2S3_MCLK   },
    {0xFFFFFFFFU,     0xFFU,  0xFFU,        0xFFFFFFFFU     },
};

const csi_clkmap_t clk_map[] = {
    {AONTIM_CLK,    DEV_DW_TIMER_TAG,       0},
    {RTC_CLK,       DEV_WJ_RTC_TAG,         0},
    {FFT_CLK,       DEV_WJ_FFT_TAG,         0},
    {ROM_CLK,       DEV_WJ_ROM_TAG,         0},
    {QSPI_CLK,      DEV_CD_QSPI_TAG,        0},
    {EFUSE_CLK,     DEV_WJ_EFUSE_TAG,       0},
    {SASC_CLK,      DEV_WJ_SASC_TAG,        0},
    {TIPC_CLK,      DEV_WJ_TIPC_TAG,        0},
    {SDIO_CLK,      DEV_DW_SDMMC_TAG,        0},
    {SDMMC_CLK,     DEV_DW_SDMMC_TAG,       0},
    {AES_CLK,       DEV_WJ_AES_TAG,         0},
    {RSA_CLK,       DEV_WJ_RSA_TAG,         0},
    {SHA_CLK,       DEV_WJ_SHA_TAG,         0},
    {TRNG_CLK,      DEV_OSR_RNG_TAG,        0},
    {DMA0_CLK,      DEV_DW_DMA_TAG,         0},
    {DMA1_CLK,      DEV_DW_DMA_TAG,         1},
    {WDT_CLK,       DEV_DW_WDT_TAG,         0},
    {PWM0_CLK,      DEV_WJ_PWM_TAG,         0},
    {PWM1_CLK,      DEV_WJ_PWM_TAG,         1},
    {TIM1_CLK,      DEV_DW_TIMER_TAG,       1},
    {TIM2_CLK,      DEV_DW_TIMER_TAG,       2},
    {TIM3_CLK,      DEV_DW_TIMER_TAG,       3},
    {GPIO0_CLK,     DEV_DW_GPIO_TAG,        0},
    {GPIO1_CLK,     DEV_DW_GPIO_TAG,        1},
    {USI0_CLK,      DEV_WJ_USI_TAG,         0},
    {USI1_CLK,      DEV_WJ_USI_TAG,         1},
    {USI2_CLK,      DEV_WJ_USI_TAG,         2},
    {USI3_CLK,      DEV_WJ_USI_TAG,         3},
    {UART0_CLK,     DEV_DW_UART_TAG,        0},
    {UART1_CLK,     DEV_DW_UART_TAG,        1},
    {UART2_CLK,     DEV_DW_UART_TAG,        2},
    {UART3_CLK,     DEV_DW_UART_TAG,        3},
    {I2S0_CLK,      DEV_WJ_I2S_TAG,         0},
    {I2S1_CLK,      DEV_WJ_I2S_TAG,         1},
    {I2S2_CLK,      DEV_WJ_I2S_TAG,         2},
    {I2S3_CLK,      DEV_WJ_I2S_TAG,         3},
    {SPDIF_CLK,     DEV_WJ_SPDIF_TAG,       0},
    {FMC_AHB0_CLK,  DEV_BLANK_TAG,          0},
    {FMC_AHB1_CLK,  DEV_BLANK_TAG,          1},
    {TDM_CLK,       DEV_WJ_TDM_TAG,         0},
    {PDM_CLK,       DEV_WJ_PDM_TAG,         0},
    {CODEC_CLK,     DEV_WJ_CODEC_TAG,       1},
    {0xFFFFFFFFU,   0xFFFFU,             0xFFU}
};

const csi_tipcmap_t tipc_map[] = {
    {AHB0_SDIO_TRUST,    DEV_DW_SDMMC_TAG,       0},
    {AHB0_SDMMC_TRUST,   DEV_DW_SDMMC_TAG,       1},
    {AHB0_DMAC0_TRUST,   DEV_DW_DMA_TAG,        0},
    {AHB0_AES_TRUST,     DEV_WJ_AES_TAG,        0},
    {AHB0_SHA_TRUST,     DEV_WJ_SHA_TAG,        0},
    {AHB0_RSA_TRUST,     DEV_WJ_RSA_TAG,        0},
    {AHB0_TRNG_TRUST,    DEV_OSR_RNG_TAG,       0},
    {AHB1_DMAC1_TRUST,   DEV_DW_DMA_TAG,        1},
    {APB0_WDT_TRUST,     DEV_DW_WDT_TAG,        0},
    {APB0_RTC_TRUST,     DEV_WJ_RTC_TAG,        0},
    {APB0_TIM0_TRUST,    DEV_DW_TIMER_TAG,      0},
    {APB0_TIM1_TRUST,    DEV_DW_TIMER_TAG,      1},
    {APB0_QSPI0_TRUST,   DEV_CD_QSPI_TAG,       0},
    {APB0_PADREG_TRUST,  DEV_WJ_PADREG_TAG,     0},
    {APB0_GPIO0_TRUST,   DEV_DW_GPIO_TAG,       0},
    {APB0_PWM0_TRUST,    DEV_WJ_PWM_TAG,        0},
    {APB0_I2S0_TRUST,    DEV_WJ_I2S_TAG,        0},
    {APB0_I2S1_TRUST,    DEV_WJ_I2S_TAG,        1},
    {APB0_USI0_TRUST,    DEV_WJ_USI_TAG,        0},
    {APB0_USI1_TRUST,    DEV_WJ_USI_TAG,        1},
    {APB0_UART0_TRUST,   DEV_DW_UART_TAG,       0},
    {APB0_UART1_TRUST,   DEV_DW_UART_TAG,       1},
    {0xFFFFFFFFU,        0xFFFFU,           0xFFU},
};
