/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * @version  V1.0
 * @date     7. April 2020
 ******************************************************************************/

#ifndef _SOC_H_
#define _SOC_H_

#include <stdint.h>
#include <csi_core.h>
#include <sys_clk.h>
// #include <gpio/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EHS_VALUE
#define EHS_VALUE               20000000U
#endif

#ifndef ELS_VALUE
#define ELS_VALUE               32768U
#endif

#ifndef IHS_VALUE
#define IHS_VALUE               50000000// 24000000U
#endif

#ifndef ILS_VALUE
#define ILS_VALUE               32768U
#endif

typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_16,
    GPIO_PIN_17,
    GPIO_PIN_18,
    GPIO_PIN_19,
    GPIO_PIN_20,
    GPIO_PIN_21,
    GPIO_PIN_22,
    GPIO_PIN_23,
    GPIO_PIN_24,
    GPIO_PIN_25,
    GPIO_PIN_26,
    GPIO_PIN_27,
    GPIO_PIN_28,
    GPIO_PIN_29,
    GPIO_PIN_30,
    GPIO_PIN_31,
    GPIO_PIN_32,
    GPIO_PIN_33,
    GPIO_PIN_34,
    GPIO_PIN_35,
    GPIO_PIN_36,
    GPIO_PIN_37,
    GPIO_PIN_38,
    GPIO_PIN_39,
    GPIO_PIN_40,
    GPIO_PIN_41,
    GPIO_PIN_42,
    GPIO_PIN_43,
    GPIO_PIN_44,
    GPIO_PIN_45,
    GPIO_PIN_MAX,
} pin_name_t;

typedef enum {
    GPIO0_SDH_CLK                    =  0U,
    GPIO0_SPI_SS                     =  1U,

    GPIO0_I2S_BCLK                   =  3U,
    GPIO0_PDM_0_IN                   =  4U,
    GPIO0_I2C0_SCL                   =  5U,
    GPIO0_I2C1_SCL                   =  6U,
    GPIO0_UART_SIG0                  =  7U,

    GPIO0_SWGPIO0                    =  11U,
    GPIO0_PWM0_CH0P                  =  16U,
    GPIO0_PWM1_CH0P                  =  17U,
    GPIO0_DSP_SPI_SS                 =  18U,
    GPIO0_DSP_I2C0_SCL               =  19U,
    GPIO0_DSP_I2C1_SCL               =  20U,
    GPIO0_DSP_UART_TXD               =  21U,
    GPIO0_E907_JTAG_TCLK             =  26U,
    GPIO0_C906_JTAG_TCK              =  27U,

    GPIO1_SDH_CMD                    =  0U,
    GPIO1_SPI_MISO                   =  1U,

    GPIO1_I2S_FS                     =  3U,
    GPIO1_PDM_1_IN                   =  4U,
    GPIO1_I2C0_SDA                   =  5U,
    GPIO1_I2C1_SDA                   =  6U,
    GPIO1_UART_SIG1                  =  7U,

    GPIO1_SWGPIO1                    =  11U,
    GPIO1_PWM0_CH1P                  =  16U,
    GPIO1_PWM1_CH1P                  =  17U,
    GPIO1_DSP_SPI_MISO               =  18U,
    GPIO1_DSP_I2C0_SDA               =  19U,
    GPIO1_DSP_I2C1_SDA               =  20U,
    GPIO1_DSP_UART_RXD               =  21U,
    GPIO1_E907_JTAG_TDI              =  26U,
    GPIO1_C906_JTAG_TDI              =  27U,

    GPIO2_SDH_DAT0                   =  0U,
    GPIO2_SPI_MOSI                   =  1U,

    GPIO2_I2S_DI                     =  3U,
    GPIO2_PDM_CLK_O                  =  4U,
    GPIO2_I2C0_SCL                   =  5U,
    GPIO2_I2C1_SCL                   =  6U,
    GPIO2_UART_SIG2                  =  7U,

    GPIO2_SWGPIO2                    =  11U,
    GPIO2_PWM0_CH2P                  =  16U,
    GPIO2_PWM1_CH2P                  =  17U,
    GPIO2_DSP_SPI_MOSI               =  18U,
    GPIO2_DSP_I2C0_SCL               =  19U,
    GPIO2_DSP_I2C1_SCL               =  20U,
    GPIO2_DSP_UART_RTS               =  21U,
    GPIO2_E907_JTAG_TMS              =  26U,
    GPIO2_C906_JTAG_TMS              =  27U,

    GPIO3_SDH_DAT1                   =  0U,
    GPIO3_SPI_SCLK                   =  1U,

    GPIO3_I2S_DO                     =  3U,
    GPIO3_PDM_0_IN                   =  4U,
    GPIO3_I2C0_SDA                   =  5U,
    GPIO3_I2C1_SDA                   =  6U,
    GPIO3_UART_SIG3                  =  7U,

    GPIO3_SWGPIO3                    =  11U,
    GPIO3_PWM0_CH3P                  =  16U,
    GPIO3_PWM1_CH3P                  =  17U,
    GPIO3_DSP_SPI_SCLK               =  18U,
    GPIO3_DSP_I2C0_SDA               =  19U,
    GPIO3_DSP_I2C1_SDA               =  20U,
    GPIO3_DSP_UART_CTS               =  21U,
    GPIO3_E907_JTAG_TDO              =  26U,
    GPIO3_C906_JTAG_TDO              =  27U,

    GPIO4_SDH_DAT2                   =  0U,
    GPIO4_SPI_SS                     =  1U,

    GPIO4_I2S_BCLK                   =  3U,
    GPIO4_PDM_1_IN                   =  4U,
    GPIO4_I2C0_SCL                   =  5U,
    GPIO4_I2C1_SCL                   =  6U,
    GPIO4_UART_SIG4                  =  7U,

    GPIO4_ADC_CH2                    =  10U,
    GPIO4_SWGPIO4                    =  11U,
    GPIO4_PWM0_CH0P                  =  16U,
    GPIO4_PWM1_CH0P                  =  17U,
    GPIO4_DSP_SPI_SS                 =  18U,
    GPIO4_DSP_I2C0_SCL               =  19U,
    GPIO4_DSP_I2C1_SCL               =  20U,
    GPIO4_DSP_UART_TXD               =  21U,
    GPIO4_E907_JTAG_TCLK             =  26U,
    GPIO4_C906_JTAG_TCK              =  27U,

    GPIO5_SDH_DAT3                   =  0U,
    GPIO5_SPI_MISO                   =  1U,

    GPIO5_I2S_FS                     =  3U,
    GPIO5_PDM_CLK_O                  =  4U,
    GPIO5_I2C0_SDA                   =  5U,
    GPIO5_I2C1_SDA                   =  6U,
    GPIO5_UART_SIG5                  =  7U,

    GPIO5_ADC_CH1                    =  10U,
    GPIO5_SWGPIO5                    =  11U,
    GPIO5_PWM0_CH1P                  =  16U,
    GPIO5_PWM1_CH1P                  =  17U,
    GPIO5_DSP_SPI_MISO               =  18U,
    GPIO5_DSP_I2C0_SDA               =  19U,
    GPIO5_DSP_I2C1_SDA               =  20U,
    GPIO5_DSP_UART_RXD               =  21U,
    GPIO5_E907_JTAG_TDI              =  26U,
    GPIO5_C906_JTAG_TDI              =  27U,

    GPIO11_SPI_SCLK                  =  1U,

    GPIO11_I2S_DO                    =  3U,

    GPIO11_I2C0_SDA                  =  5U,
    GPIO11_I2C1_SDA                  =  6U,
    GPIO11_UART_SIG11                =  7U,

    GPIO11_ADC_CH3                   =  10U,
    GPIO11_SWGPIO11                  =  11U,
    GPIO11_PWM0_CH3P                 =  16U,
    GPIO11_PWM1_CH3P                 =  17U,
    GPIO11_DSP_SPI_SCLK              =  18U,
    GPIO11_DSP_I2C0_SDA              =  19U,
    GPIO11_DSP_I2C1_SDA              =  20U,
    GPIO11_DSP_UART_CTS              =  21U,
    GPIO11_E907_JTAG_TDO             =  26U,
    GPIO11_C906_JTAG_TDO             =  27U,

    GPIO12_SPI_SS                    =  1U,

    GPIO12_I2S_BCLK                  =  3U,

    GPIO12_I2C0_SCL                  =  5U,
    GPIO12_I2C1_SCL                  =  6U,
    GPIO12_UART_SIG0                 =  7U,

    GPIO12_ADC_CH6                   =  10U,
    GPIO12_SWGPIO12                  =  11U,
    GPIO12_PWM0_CH0P                 =  16U,
    GPIO12_PWM1_CH0P                 =  17U,
    GPIO12_DSP_SPI_SS                =  18U,
    GPIO12_DSP_I2C0_SCL              =  19U,
    GPIO12_DSP_I2C1_SCL              =  20U,
    GPIO12_DSP_UART_TXD              =  21U,
    GPIO12_E907_JTAG_TCLK            =  26U,
    GPIO12_C906_JTAG_TCK             =  27U,

    GPIO16_SPI_SS                    =  1U,

    GPIO16_I2S_BCLK                  =  3U,
    GPIO16_PDM_1_IN                  =  4U,
    GPIO16_I2C0_SCL                  =  5U,
    GPIO16_I2C1_SCL                  =  6U,
    GPIO16_UART_SIG4                 =  7U,

    GPIO16_ADC_CH8                   =  10U,
    GPIO16_SWGPIO16                  =  11U,
    GPIO16_PWM0_CH0P                 =  16U,
    GPIO16_PWM1_CH0P                 =  17U,
    GPIO16_DSP_SPI_SS                =  18U,
    GPIO16_DSP_I2C0_SCL              =  19U,
    GPIO16_DSP_I2C1_SCL              =  20U,
    GPIO16_DSP_UART_TXD              =  21U,
    GPIO16_E907_JTAG_TCLK            =  26U,
    GPIO16_C906_JTAG_TCK             =  27U,

    GPIO17_SPI_MISO                  =  1U,

    GPIO17_I2S_FS                    =  3U,
    GPIO17_PDM_CLK_O                 =  4U,
    GPIO17_I2C0_SDA                  =  5U,
    GPIO17_I2C1_SDA                  =  6U,
    GPIO17_UART_SIG5                 =  7U,

    GPIO17_ADC_CH0                   =  10U,
    GPIO17_SWGPIO17                  =  11U,
    GPIO17_PWM0_CH1P                 =  16U,
    GPIO17_PWM1_CH1P                 =  17U,
    GPIO17_DSP_SPI_MISO              =  18U,
    GPIO17_DSP_I2C0_SDA              =  19U,
    GPIO17_DSP_I2C1_SDA              =  20U,
    GPIO17_DSP_UART_RXD              =  21U,
    GPIO17_E907_JTAG_TDI             =  26U,
    GPIO17_C906_JTAG_TDI             =  27U,

    GPIO18_SPI_MOSI                  =  1U,

    GPIO18_I2S_DI                    =  3U,
    GPIO18_PDM_0_IN                  =  4U,
    GPIO18_I2C0_SCL                  =  5U,
    GPIO18_I2C1_SCL                  =  6U,
    GPIO18_UART_SIG6                 =  7U,

    GPIO18_ADC_CH9                   =  10U,
    GPIO18_SWGPIO18                  =  11U,
    GPIO18_PWM0_CH2P                 =  16U,
    GPIO18_PWM1_CH2P                 =  17U,
    GPIO18_DSP_SPI_MOSI              =  18U,
    GPIO18_DSP_I2C0_SCL              =  19U,
    GPIO18_DSP_I2C1_SCL              =  20U,
    GPIO18_DSP_UART_RTS              =  21U,
    GPIO18_E907_JTAG_TMS             =  26U,
    GPIO18_C906_JTAG_TMS             =  27U,

    GPIO19_SPI_SCLK                  =  1U,

    GPIO19_I2S_DO                    =  3U,
    GPIO19_PDM_1_IN                  =  4U,
    GPIO19_I2C0_SDA                  =  5U,
    GPIO19_I2C1_SDA                  =  6U,
    GPIO19_UART_SIG7                 =  7U,

    GPIO19_ADC_CH10                  =  10U,
    GPIO19_SWGPIO19                  =  11U,
    GPIO19_PWM0_CH3P                 =  16U,
    GPIO19_PWM1_CH3P                 =  17U,
    GPIO19_DSP_SPI_SCLK              =  18U,
    GPIO19_DSP_I2C0_SDA              =  19U,
    GPIO19_DSP_I2C1_SDA              =  20U,
    GPIO19_DSP_UART_CTS              =  21U,
    GPIO19_E907_JTAG_TDO             =  26U,
    GPIO19_C906_JTAG_TDO             =  27U,

    GPIO20_SPI_SS                    =  1U,

    GPIO20_I2S_BCLK                  =  3U,
    GPIO20_PDM_CLK_O                 =  4U,
    GPIO20_I2C0_SCL                  =  5U,
    GPIO20_I2C1_SCL                  =  6U,
    GPIO20_UART_SIG8                 =  7U,

    GPIO20_SWGPIO20                  =  11U,
    GPIO20_PWM0_CH0P                 =  16U,
    GPIO20_PWM1_CH0P                 =  17U,
    GPIO20_DSP_SPI_SS                =  18U,
    GPIO20_DSP_I2C0_SCL              =  19U,
    GPIO20_DSP_I2C1_SCL              =  20U,
    GPIO20_DSP_UART_TXD              =  21U,
    GPIO20_E907_JTAG_TCLK            =  26U,
    GPIO20_C906_JTAG_TCK             =  27U,

    GPIO21_SPI_MISO                  =  1U,

    GPIO21_I2S_FS                    =  3U,
    GPIO21_PDM_0_IN                  =  4U,
    GPIO21_I2C0_SDA                  =  5U,
    GPIO21_I2C1_SDA                  =  6U,
    GPIO21_UART_SIG9                 =  7U,

    GPIO21_SWGPIO21                  =  11U,
    GPIO21_PWM0_CH1P                 =  16U,
    GPIO21_PWM1_CH1P                 =  17U,
    GPIO21_DSP_SPI_MISO              =  18U,
    GPIO21_DSP_I2C0_SDA              =  19U,
    GPIO21_DSP_I2C1_SDA              =  20U,
    GPIO21_DSP_UART_RXD              =  21U,
    GPIO21_E907_JTAG_TDI             =  26U,
    GPIO21_C906_JTAG_TDI             =  27U,

    GPIO24_SPI_SS                    =  1U,

    GPIO24_I2S_BCLK                  =  3U,

    GPIO24_I2C0_SCL                  =  5U,
    GPIO24_I2C1_SCL                  =  6U,
    GPIO24_UART_SIG0                 =  7U,

    GPIO24_SWGPIO24                  =  11U,
    GPIO24_PWM0_CH0P                 =  16U,
    GPIO24_PWM1_CH0P                 =  17U,
    GPIO24_DSP_SPI_SS                =  18U,
    GPIO24_DSP_I2C0_SCL              =  19U,
    GPIO24_DSP_I2C1_SCL              =  20U,
    GPIO24_DSP_UART_TXD              =  21U,
    GPIO24_E907_JTAG_TCLK            =  26U,
    GPIO24_C906_JTAG_TCK             =  27U,

    GPIO25_SPI_MISO                  =  1U,

    GPIO25_I2S_FS                    =  3U,
    GPIO25_PDM_1_IN                  =  4U,
    GPIO25_I2C0_SDA                  =  5U,
    GPIO25_I2C1_SDA                  =  6U,
    GPIO25_UART_SIG1                 =  7U,

    GPIO25_SWGPIO25                  =  11U,
    GPIO25_PWM0_CH1P                 =  16U,
    GPIO25_PWM1_CH1P                 =  17U,
    GPIO25_DSP_SPI_MISO              =  18U,
    GPIO25_DSP_I2C0_SDA              =  19U,
    GPIO25_DSP_I2C1_SDA              =  20U,
    GPIO25_DSP_UART_RXD              =  21U,
    GPIO25_E907_JTAG_TDI             =  26U,
    GPIO25_C906_JTAG_TDI             =  27U,

    GPIO26_SPI_MOSI                  =  1U,

    GPIO26_I2S_DI                    =  3U,
    GPIO26_PDM_CLK_O                 =  4U,
    GPIO26_I2C0_SCL                  =  5U,
    GPIO26_I2C1_SCL                  =  6U,
    GPIO26_UART_SIG2                 =  7U,

    GPIO26_SWGPIO26                  =  11U,
    GPIO26_PWM0_CH2P                 =  16U,
    GPIO26_PWM1_CH2P                 =  17U,
    GPIO26_DSP_SPI_MOSI              =  18U,
    GPIO26_DSP_I2C0_SCL              =  19U,
    GPIO26_DSP_I2C1_SCL              =  20U,
    GPIO26_DSP_UART_RTS              =  21U,
    GPIO26_E907_JTAG_TMS             =  26U,
    GPIO26_C906_JTAG_TMS             =  27U,

    GPIO27_SPI_SCLK                  =  1U,

    GPIO27_I2S_DO                    =  3U,
    GPIO27_PDM_0_IN                  =  4U,
    GPIO27_I2C0_SDA                  =  5U,
    GPIO27_I2C1_SDA                  =  6U,
    GPIO27_UART_SIG3                 =  7U,

    GPIO27_SWGPIO27                  =  11U,
    GPIO27_PWM0_CH3P                 =  16U,
    GPIO27_PWM1_CH3P                 =  17U,
    GPIO27_DSP_SPI_SCLK              =  18U,
    GPIO27_DSP_I2C0_SDA              =  19U,
    GPIO27_DSP_I2C1_SDA              =  20U,
    GPIO27_DSP_UART_CTS              =  21U,
    GPIO27_E907_JTAG_TDO             =  26U,
    GPIO27_C906_JTAG_TDO             =  27U,

    GPIO28_SPI_SS                    =  1U,

    GPIO28_I2S_BCLK                  =  3U,

    GPIO28_I2C0_SCL                  =  5U,
    GPIO28_I2C1_SCL                  =  6U,
    GPIO28_UART_SIG4                 =  7U,

    GPIO28_SWGPIO28                  =  11U,
    GPIO28_PWM0_CH0P                 =  16U,
    GPIO28_PWM1_CH0P                 =  17U,
    GPIO28_DSP_SPI_SS                =  18U,
    GPIO28_DSP_I2C0_SCL              =  19U,
    GPIO28_DSP_I2C1_SCL              =  20U,
    GPIO28_DSP_UART_TXD              =  21U,
    GPIO28_E907_JTAG_TCLK            =  26U,
    GPIO28_C906_JTAG_TCK             =  27U,

    GPIO34_SPI_MOSI                  =  1U,
    GPIO34_SF2_CLK                   =  2U,

    GPIO34_I2S_DI                    =  3U,
    GPIO34_PDM_1_IN                  =  4U,
    GPIO34_I2C0_SCL                  =  5U,
    GPIO34_I2C1_SCL                  =  6U,
    GPIO34_UART_SIG10                =  7U,

    GPIO34_ADC_CH11                  =  10U,
    GPIO34_SWGPIO34                  =  11U,
    GPIO34_PWM0_CH2P                 =  16U,
    GPIO34_PWM1_CH2P                 =  17U,
    GPIO34_DSP_SPI_MOSI              =  18U,
    GPIO34_DSP_I2C0_SCL              =  19U,
    GPIO34_DSP_I2C1_SCL              =  20U,
    GPIO34_DSP_UART_RTS              =  21U,
    GPIO34_E907_JTAG_TMS             =  26U,
    GPIO34_C906_JTAG_TMS             =  27U,

    GPIO35_SPI_SCLK                  =  1U,
    GPIO35_SF2_CS                    =  2U,

    GPIO35_I2S_DO                    =  3U,
    GPIO35_PDM_CLK_O                 =  4U,
    GPIO35_I2C0_SDA                  =  5U,
    GPIO35_I2C1_SDA                  =  6U,
    GPIO35_UART_SIG11                =  7U,

    GPIO35_SWGPIO35                  =  11U,
    GPIO35_PWM0_CH3P                 =  16U,
    GPIO35_PWM1_CH3P                 =  17U,
    GPIO35_DSP_SPI_SCLK              =  18U,
    GPIO35_DSP_I2C0_SDA              =  19U,
    GPIO35_DSP_I2C1_SDA              =  20U,
    GPIO35_DSP_UART_CTS              =  21U,
    GPIO35_E907_JTAG_TDO             =  26U,
    GPIO35_C906_JTAG_TDO             =  27U,

    GPIO36_SPI_SS                    =  1U,
    GPIO36_SF2_D0                    =  2U,

    GPIO36_I2S_BCLK                  =  3U,
    GPIO36_PDM_0_IN                  =  4U,
    GPIO36_I2C0_SCL                  =  5U,
    GPIO36_I2C1_SCL                  =  6U,
    GPIO36_UART_SIG0                 =  7U,

    GPIO36_SWGPIO36                  =  11U,
    GPIO36_PWM0_CH0P                 =  16U,
    GPIO36_PWM1_CH0P                 =  17U,
    GPIO36_DSP_SPI_SS                =  18U,
    GPIO36_DSP_I2C0_SCL              =  19U,
    GPIO36_DSP_I2C1_SCL              =  20U,
    GPIO36_DSP_UART_TXD              =  21U,
    GPIO36_E907_JTAG_TCLK            =  26U,
    GPIO36_C906_JTAG_TCK             =  27U,

    GPIO37_SPI_MISO                  =  1U,
    GPIO37_SF2_D1                    =  2U,

    GPIO37_I2S_FS                    =  3U,
    GPIO37_PDM_1_IN                  =  4U,
    GPIO37_I2C0_SDA                  =  5U,
    GPIO37_I2C1_SDA                  =  6U,
    GPIO37_UART_SIG1                 =  7U,

    GPIO37_SWGPIO37                  =  11U,
    GPIO37_PWM0_CH1P                 =  16U,
    GPIO37_PWM1_CH1P                 =  17U,
    GPIO37_DSP_SPI_MISO              =  18U,
    GPIO37_DSP_I2C0_SDA              =  19U,
    GPIO37_DSP_I2C1_SDA              =  20U,
    GPIO37_DSP_UART_RXD              =  21U,
    GPIO37_E907_JTAG_TDI             =  26U,
    GPIO37_C906_JTAG_TDI             =  27U,

    GPIO38_SPI_MOSI                  =  1U,
    GPIO38_SF2_D2                    =  2U,

    GPIO38_I2S_DI                    =  3U,
    GPIO38_PDM_CLK_O                 =  4U,
    GPIO38_I2C0_SCL                  =  5U,
    GPIO38_I2C1_SCL                  =  6U,
    GPIO38_UART_SIG2                 =  7U,

    GPIO38_SWGPIO38                  =  11U,
    GPIO38_PWM0_CH2P                 =  16U,
    GPIO38_PWM1_CH2P                 =  17U,
    GPIO38_DSP_SPI_MOSI              =  18U,
    GPIO38_DSP_I2C0_SCL              =  19U,
    GPIO38_DSP_I2C1_SCL              =  20U,
    GPIO38_DSP_UART_RTS              =  21U,
    GPIO38_E907_JTAG_TMS             =  26U,
    GPIO38_C906_JTAG_TMS             =  27U,

    GPIO39_SPI_SCLK                  =  1U,
    GPIO39_SF2_D3                    =  2U,

    GPIO39_I2S_DO                    =  3U,
    GPIO39_PDM_0_IN                  =  4U,
    GPIO39_I2C0_SDA                  =  5U,
    GPIO39_I2C1_SDA                  =  6U,
    GPIO39_UART_SIG3                 =  7U,

    GPIO39_SWGPIO39                  =  11U,
    GPIO39_PWM0_CH3P                 =  16U,
    GPIO39_PWM1_CH3P                 =  17U,
    GPIO39_DSP_SPI_SCLK              =  18U,
    GPIO39_DSP_I2C0_SDA              =  19U,
    GPIO39_DSP_I2C1_SDA              =  20U,
    GPIO39_DSP_UART_CTS              =  21U,
    GPIO39_E907_JTAG_TDO             =  26U,
    GPIO39_C906_JTAG_TDO             =  27U,

    GPIO40_SPI_SS                    =  1U,

    GPIO40_I2S_BCLK                  =  3U,

    GPIO40_I2C0_SCL                  =  5U,
    GPIO40_I2C1_SCL                  =  6U,
    GPIO40_UART_SIG4                 =  7U,

    GPIO40_ADC_CH5                   =  10U,
    GPIO40_SWGPIO40                  =  11U,
    GPIO40_PWM0_CH0P                 =  16U,
    GPIO40_PWM1_CH0P                 =  17U,
    GPIO40_DSP_SPI_SS                =  18U,
    GPIO40_DSP_I2C0_SCL              =  19U,
    GPIO40_DSP_I2C1_SCL              =  20U,
    GPIO40_DSP_UART_TXD              =  21U,
    GPIO40_E907_JTAG_TCLK            =  26U,
    GPIO40_C906_JTAG_TCK             =  27U,

    GPIO41_SPI_MISO                  =  1U,

    GPIO41_I2S_FS                    =  3U,

    GPIO41_I2C0_SDA                  =  5U,
    GPIO41_I2C1_SDA                  =  6U,
    GPIO41_UART_SIG5                 =  7U,

    GPIO41_SWGPIO41                  =  11U,
    GPIO41_PWM0_CH1P                 =  16U,
    GPIO41_PWM1_CH1P                 =  17U,
    GPIO41_DSP_SPI_MISO              =  18U,
    GPIO41_DSP_I2C0_SDA              =  19U,
    GPIO41_DSP_I2C1_SDA              =  20U,
    GPIO41_DSP_UART_RXD              =  21U,
    GPIO41_E907_JTAG_TDI             =  26U,
    GPIO41_C906_JTAG_TDI             =  27U,
    PIN_FUNC_GPIO                    =  11U,
} pin_func_t;

typedef enum {
    Supervisor_Software_IRQn        =  1U,
    Machine_Software_IRQn           =  3U,
    Supervisor_Timer_IRQn           =  5U,
    CORET_IRQn                      =  7U,
    Supervisor_External_IRQn        =  9U,
    Machine_External_IRQn           =  11U,
    // FIXME:
    DW_WDT_IRQn                     =  17U,
    DW_UART_IRQn                    =  18U,
    DW_DMA_IRQn                     =  19U,
    DW_TIMER0_IRQn                  =  20U,
    DW_TIMER1_IRQn                  =  21U,
    DW_TIMER2_IRQn                  =  22U,
    DW_TIMER3_IRQn                  =  23U,
    WJ_VAD_IRQn                     =  24U,
    WJ_VAD_TRIG_IRQn                =  25U,
    WJ_I2S0_IRQn                    =  26U,
    WJ_I2S1_IRQn                    =  27U,
    WJ_I2S2_IRQn                    =  28U,
    WJ_I2S_IN_IRQn                  =  29U,
    WJ_TDM_IRQn                     =  30U,
    WJ_SPDIF0_IRQn                  =  31U,
    WJ_SPDIF1_IRQn                  =  32U,
    DW_GPIO_IRQn                    =  33U,
    DW_IIC0_IRQn                    =  34U,
    DW_IIC1_IRQn                    =  35U,
    WJ_MAILBOX_IRQn                 =  36U,
    WJ_BUS_MONITOR_IRQn             =  37U,
    WJ_CPR_WK_IRQn                  =  38U,
    WJ_NPU_IRQn                     =  39U,
} irqn_type_t;

#define CONFIG_GPIO_NUM             2
#define CONFIG_IRQ_NUM              1023U
#define CONFIG_DMA_NUM              1


#define DW_TIMER0_BASE              0xFFCB011000UL
#define DW_TIMER0_SIZE              0x14U

#define DW_TIMER1_BASE              (DW_TIMER0_BASE+DW_TIMER0_SIZE)
#define DW_TIMER1_SIZE              DW_TIMER0_SIZE

#define DW_TIMER2_BASE              (DW_TIMER1_BASE+DW_TIMER1_SIZE)
#define DW_TIMER2_SIZE              DW_TIMER1_SIZE

#define DW_TIMER3_BASE              (DW_TIMER2_BASE+DW_TIMER2_SIZE)
#define DW_TIMER3_SIZE              DW_TIMER2_SIZE

#define DW_UART_BASE                0x02500000UL
#define DW_UART_SIZE                0x1000U

#define DW_GPIO_BASE                0xFFCB013000UL
#define DW_GPIO_SIZE                0x1000U

#define DW_WDT_BASE                 0xFFCB010000UL
#define DW_WDT_BASE_SZIE            0x1000U

#define DW_DMA_BASE                 0xFFC8000000UL
#define DW_DMA_BASE_SZIE            0x1000U

#define DW_BUS_MONITOR_BASE         0xFFCB020000UL
#define DW_BUS_MONITOR_BASE_SZIE    0x1000U

#define DW_MAILBOX_BASE             0xFFCA000000UL
#define DW_MAILBOX_BASE_SZIE        0x1000U

#define WJ_TDM_BASE                 0xFFCB012000UL
#define WJ_TDM_BASE_SZIE            0x1000U

#define WJ_I2S0_BASE                0xFFCB014000UL
#define WJ_I2S0_SIZE                0x1000U
#define WJ_I2S1_BASE                0xFFCB015000UL
#define WJ_I2S1_SIZE                0x1000U
#define WJ_I2S2_BASE                0xFFCB016000UL
#define WJ_I2S2_SIZE                0x1000U
#define WJ_I2S3_BASE                0xFFCB017000UL
#define WJ_I2S3_SIZE                0x1000U

#define WJ_SPDIF0_BASE              0xFFCB018000UL
#define WJ_SPDIF0_SIZE              0x1000U
#define WJ_SPDIF1_BASE              0xFFCB019000UL
#define WJ_SPDIF1_SIZE              0x1000U

#define DW_IIC0_BASE                0xFFCB01A000UL
#define DW_IIC0_BASE_SIZE           0x1000U

#define DW_IIC1_BASE                0xFFCB01B000UL
#define DW_IIC1_BASE_SIZE           0x1000U

#define WJ_IOC_BASE                 0xFFCB01D000UL
#define WJ_IOC_SIZE                 0x1000U

#define WJ_VAD_BASE                 0xFFCB01E000UL
#define WJ_VAD_BASE_SIZE            0x1000U

#define WJ_CPR_BASE                 0xFFCB000000UL
#define WJ_CPR_BASE_SIZE            0x10000U

#define WJ_MBOX_BASE                0xFFEFC48000UL
#define WJ_MBOX_SIZE                0x4000U

#define CONFIG_RTC_FAMILY_D

#define CONFIG_DW_AXI_DMA_32CH_NUM_CHANNELS

#ifndef PLIC_BASE
#define PLIC_BASE           (0xE0000000UL)
#endif
//#define CORET_BASE          (PLIC_BASE + 0x4000000UL)               /*!< CORET Base Address */
#define PLIC                ((PLIC_Type *)PLIC_BASE)

/* ================================================================================ */
/* ================                  otp declaration               ================ */
/* ================================================================================ */
#define CONFIG_OTP_BASE_ADDR 0U
#define CONFIG_OTP_BANK_SIZE 256U    // Bytes

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_H_ */
