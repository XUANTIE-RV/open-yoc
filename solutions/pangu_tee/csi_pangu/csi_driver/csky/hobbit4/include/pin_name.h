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
 * @file     pin_name.h
 * @brief    header file for the pin_name
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _PINNAMES_H
#define _PINNAMES_H


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PA0 = 0,
    PA1,
    PA2,
    PA3,
    PA4,
    PA5,
    PA6,
    PA7,
    PA8,
    PA9,
    PA10,
    PA11,
    PA12,
    PA13,
    PA14,
    PA15,
    PA16,
    PA17,
    PA18,
    PA19,
    PA20,
    PA21,
    PA22,
    PA23,
    PA24,
    PA25,
    PA26,
    PA27,
    PA28,
    PA29,
    PA30,
    PA31,
    PB0,
    PB1,
    PB2,
    PB3,
    PB4,
    PB5,
    PB6,
    PB7,
    PB8,
    PB9,
    PB10,
    PB11,
    PB12,
    PB13,
    PB14,
    PB15,
    PB16,
    PB17,
    PB18,
    PB19,
    PB20,
    PB21,
    PB22,
    PB23,
    PB24,
    PB25,
    PB26,
    PB27,
    PB28,
    PB29,
    PB30,
    PB31
}
pin_name_e;

typedef enum {
    PA0_UART0_RX      = 0,                                                 PA0_CODEC_I2C_SCL        = 3,
    PA1_UART0_RTS     = 0,                                                 PA1_CODEC_I2C_DOUT       = 3,
    PA2_UART0_TX      = 0,                                                 PA2_CODEC_I2C_DIN        = 3,
    PA3_I2S0_SCLK     = 0,                                                 PA3_APB0_CLK_DIV_2M      = 3,
    PA4_I2S0_WSCLK    = 0,                                                 PA4_RTC_32K              = 3,
    PA5_I2S0_SDI      = 0,                                                 PA5_OSC_32K              = 3,
    PA6_I2S0_SDO      = 0,                                                 PA6_CODEC_I2S_ADC12_BCLK = 3,
    PA7_QSPI_SCLK     = 0,                                                 PA7_CODEC_I2S_ADC12_LRO  = 3,
    PA8_QSPI_SS0      = 0,                                                 PA8_CODEC_I2S_ADC12_SDATA= 3,
    PA9_QSPI_DATA0    = 0,                                                 PA9_CODEC_I2S_ADC34_BCLK = 3,
    PA10_QSPI_DATA1   = 0,                                                 PA10_CODEC_I2S_ADC34_LRO  = 3,
    PA11_QSPI_DATA2   = 0,                                                 PA11_CODEC_I2S_ADC34_SDATA= 3,
    PA12_QSPI_DATA3   = 0,                                                 PA12_CODEC_I2S_ADC56_BCLK = 3,
    PA13_SDIO_DAT2    = 0,                                                 PA13_CODEC_I2S_ADC56_LRO  = 3,
    PA14_SDIO_DAT3    = 0,                                                 PA14_CODEC_I2S_ADC56_SDATA= 3,
    PA15_SDIO_CMD     = 0,                                                 PA15_CODEC_I2S_ADC78_BCLK = 3,
    PA16_SDIO_CLK     = 0,                                                 PA16_CODEC_I2S_ADC78_LRO  = 3,
    PA17_SDIO_DAT0    = 0,                                                 PA17_CODEC_I2S_ADC78_SDATA= 3,
    PA18_SDIO_DAT1    = 0,                                                 PA18_CODDEC_DAC_MBCLK     = 3,
    PA19_UART1_TX     = 0,                                                 PA19_CODDEC_DAC_MLRO      = 3,
    PA20_UART1_RX     = 0,                       PA20_PWM0_O1     = 2,   PA20_CODEC_DAC_MSDATA     = 3,
    PA21_USI0_SCLK    = 0,                       PA21_PWM0_O3     = 2,
    PA22_USI0_SD0     = 0,                       PA22_PWM0_O5     = 2,
    PA23_EMMC_CLK     = 0,  PA23_I2S1_SCLK = 1,  PA23_PWM0_O7      = 2,
    PA24_EMMC_DAT0    = 0,  PA24_I2S1_WSCLK= 1,  PA24_PWM0_O9      = 2,
    PA25_EMMC_DAT1    = 0,  PA25_I2S1_SDA  = 1,  PA25_PWM0_O11      = 2,
    PA26_EMMC_DAT2    = 0,  PA26_I2S1_MCLK = 1,  PA26_PWM0_IO0      = 2,     PA26_USI1_SCLK            = 3,
    PA27_EMMC_DAT3    = 0,  PA27_JTG_TDI   = 1,  PA27_PWM0_IO2      = 2,     PA27_USI1_SD0             = 3,
    PA28_EMMC_CMD     = 0,  PA28_JTG_TDO   = 1,  PA28_PWM0_IO4      = 2,     PA28_USI1_SD1             = 3,
    PA29_UART0_CTS    = 0,  PA29_JTG_TRST  = 1,  PA29_PWM0_IO6      = 2,     PA29_USI1_NSS             = 3,
    PA30_CPU_TCLK     = 0,  PA30_JTG_TCK   = 1,  PA30_PWM0_IO8      = 2,     PA30_USI0_SD1             = 3,
    PA31_CPU_TMS      = 0,  PA31_JTG_TMS   = 1,  PA31_PWM0_IO10      = 2,     PA31_USI0_NSS             = 3,
    PB0_I2S2_SCLK     = 0,  PB0_DMIC_CLK11 = 1,  PB0_UART3_TX      = 2,
    PB1_I2S2_WSCLK    = 0,  PB1_DMIC_CLK12 = 1,  PB1_UART3_RX      = 2,
    PB2_I2S2_SDA      = 0,  PB2_DMIC_IN1   = 1,
    PB3_I2S2_MCLK     = 0,  PB3_DMIC_CLK21 = 1,
    PB4_I2S3_SCLK     = 0,  PB4_DMIC_CLK22 = 1,  PB4_USI3_SCLK      = 2,
    PB5_I2S3_WSCLK    = 0,  PB5_DMIC_IN2   = 1,  PB5_USI3_SD0       = 2,
    PB6_I2S3_SDA      = 0,  PB6_DMIC_CLK31 = 1,
    PB7_I2S3_MCLK     = 0,  PB7_DMIC_CLK32 = 1,
                            PB8_DMIC_IN3   = 1,
                            PB9_DMIC_CLK41 = 1,
                            PB10_DMIC_CLK42= 1,
                            PB11_DMIC_IN4  = 1,
    PB12_SPDIF_DAT_IN = 0,
    PB13_SPDIF_DAT_OUT= 0,
                            PB14_TDM_BCLK  = 1,
                            PB15_TDM_WCLK  = 1,
                            PB16_TDM_DAT   = 1,
                            PB17_PDM_CLK   = 1,
                            PB18_PDM_DAT0  = 1,
                            PB19_PDM_DAT1  = 1,
    PB20_PWM1_O1       = 0, PB20_PDM_DAT2  = 1,
    PB21_PWM1_O3       = 0, PB21_PDM_DAT3  = 1,
    PB22_PWM1_O5       = 0,
    PB23_PWM1_O7       = 0,
    PB24_PWM1_O9       = 0, PB24_UART2_TX  = 1,
    PB25_PWM1_O11      = 0, PB25_UART2_RX  = 1,
    PB26_PWM1_IO0      = 0, PB26_UART2_RTS = 1,
    PB27_PWM1_IO2      = 0, PB27_UART2_CTS = 1,
    PB28_PWM1_IO4      = 0, PB28_USI2_SCLK = 1,
    PB29_PWM1_IO6      = 0, PB29_USI2_SD0  = 1,
    PB30_PWM1_IO8      = 0, PB30_USI2_SD1  = 1,
    PB31_PWM1_IO10     = 0, PB31_USI2_NSS  = 1,
    PIN_FUNC_GPIO      = 4,
} pin_func_e;

typedef enum {
    PORTA = 0,
    PORTB = 1
} port_name_e;

#ifdef __cplusplus
}
#endif

#endif
