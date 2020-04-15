/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
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
    PB0,
    PB1,
    PB2,
    PB3,
    PB4,
    PB5,
    PB6,
    PB7,
    PB8,
    PC0,
    PC1,
    PC2,
    PC3,
    PC4,
    PC5,
    PC6,
    PC7,
    PC8,
    PC9,
    PC10,
    PC11,
    PC12,
    PC13,
    PD0,
    PD1,
    PD2,
    PD3,
    PD4,
    PD5,
    PD6,
    PD7,
    PD8,
    PD9,
    PD10
}
pin_name_e;

#define IO_FUNC_GPIO        0
#define IO_FUNC_PWM         6
#define IO_FUNC_TCK         7

#define IO_FUNC_SPI         2
#define IO_FUNC_PDB         3
#define IO_FUNC_SDIO        1

#define IO_FUNC_SPDIF_O     5
#define IO_FUNC_SPDIF_I     4

#define IO_FUNC_PCM_I2S     1
#define IO_FUNC_I2S_20      4
#define IO_FUNC_I2S_71      5

#define PIN_FUNC_GPIO IO_FUNC_GPIO
typedef enum {
    PA1_TCK                = IO_FUNC_TCK,
    PA3_SPDIF_O            = IO_FUNC_SPDIF_O,
    PA4_UART2_TX           = 3,
    PA4_SPDIF_IN           = IO_FUNC_SPDIF_I,
    PA5_UART2_RX           = 3,
    PA5_SPDIF_IN           = IO_FUNC_SPDIF_I,
    PA6_I2C1_SCL           = 2,
    PA6_UART3_TX           = 3,
    PA6_SPDIF_IN           = IO_FUNC_SPDIF_I,
    PA6_TCK                = IO_FUNC_TCK,
    PA7_I2C1_SDA           = 2,
    PA7_UART3_RX           = 3,
    PA7_SPDIF_IN           = IO_FUNC_SPDIF_I,
    PB0_UART1_TX           = 1,
    PB1_UART1_RX           = 1,
    PB2_UART1_CTS          = 1,
    PB2_I2C1_SCL           = 2,
    PB2_UART4_TX           = 3,
    PB2_TCK                = IO_FUNC_TCK,
    PB3_UART1_RTS          = 1,
    PB3_I2C1_SDA           = 2,
    PB3_UART4_RX           = 3,
    PB3_SPDIF_O            = IO_FUNC_SPDIF_O,
    PB4_PCM_I2S_SCLK       = IO_FUNC_PCM_I2S,
    PB4_SPI1_MOSI          = IO_FUNC_SPI,
    PB4_PDB_D0             = IO_FUNC_PDB,
    PB4_I1_I2S_20_MCLK     = IO_FUNC_I2S_20,
    PB4_I1_I2S_71_MCLK     = IO_FUNC_I2S_71,
    PB5_PCM_I2S_WS         = IO_FUNC_PCM_I2S,
    PB5_SPI1_MISO          = IO_FUNC_SPI,
    PB5_PDB_D1             = IO_FUNC_PDB,
    PB5_I1_I2S_20_SCLK     = IO_FUNC_I2S_20,
    PB5_I1_I2S_71_MCLK     = IO_FUNC_I2S_71,
    PB6_PCM_I2S_SDI        = IO_FUNC_PCM_I2S,
    PB6_SPI1_CLK           = IO_FUNC_SPI,
    PB6_PDB_D2             = IO_FUNC_PDB,
    PB6_I1_I2S_20_WS       = IO_FUNC_I2S_20,
    PB6_I1_I2S_71_WS       = IO_FUNC_I2S_71,
    PB7_PCM_I2S_SDO        = IO_FUNC_PCM_I2S,
    PB7_SPI1_CS0           = IO_FUNC_SPI,
    PB7_PDB_D3             = IO_FUNC_PDB,
    PB7_I1_I2S_20_SDIO     = IO_FUNC_I2S_20,
    PB7_I1_I2S_71_SDIO     = IO_FUNC_I2S_71,
    PB8_PCM_I2S_MCLK       = IO_FUNC_PCM_I2S,
    PB8_SPI1_CS1           = IO_FUNC_SPI,
    PB8_PDB_D3             = IO_FUNC_PDB,
    PB8_I2_I2S_SDIO0       = IO_FUNC_I2S_20,
    PB8_I1_I2S_71_SDIO     = IO_FUNC_I2S_71,
    PB8_TCK                = IO_FUNC_TCK,
    PC0_PCM_I2S            = IO_FUNC_PCM_I2S,
    PC0_SD_DET             = 1,
    PC0_PDB                = IO_FUNC_PDB,
    PC0_I2_I2S             = IO_FUNC_I2S_20,
    PC0_I1_I2S_71          = IO_FUNC_I2S_71,
    PC1_SD_D1              = 1,
    PC1_PDB_D6             = IO_FUNC_PDB,
    PC1_I2_I2S_WS          = IO_FUNC_I2S_20,
    PC1_I1_I2S_71_SDI3     = IO_FUNC_I2S_71,
    PC2_SD_D0              = 1,
    PC2_PDB_d7             = IO_FUNC_PDB,
    PC2_I2_I2S_SDIO        = IO_FUNC_I2S_20,
    PC3_SD_CLK             = 1,
    PC3_PDB_CLK            = IO_FUNC_PDB,
    PC3_I3_I2S_MCLK        = IO_FUNC_I2S_20,
    PC4_SD_CMD_CMD         = 1,
    PC4_PDB_D8             = IO_FUNC_PDB,
    PC4_I3_I2S_SCLK        = IO_FUNC_I2S_20,
    PC5_SD_D3              = 1,
    PC5_PDB_D9             = IO_FUNC_PDB,
    PC5_I3_I2S_WS          = IO_FUNC_I2S_20,
    PC6_SD_D2              = 1,
    PC6_PDB_D10            = IO_FUNC_PDB,
    PC6_I3_I2S_SDIO        = IO_FUNC_I2S_20,
    PC7_SDIO_DET           = IO_FUNC_SDIO,
    PC7_PDB_D11            = IO_FUNC_PDB,
    PC7_O1_I2S_20_MCLK     = IO_FUNC_I2S_20,
    PC7_O1_I2S_71_MCLK     = IO_FUNC_I2S_71,
    PC7_TCK                = IO_FUNC_TCK,
    PC8_SDIO_D1            = IO_FUNC_SDIO,
    PC8_PDB_D12            = IO_FUNC_PDB,
    PC8_O1_I2S_20_SCLK     = IO_FUNC_I2S_20,
    PC8_O1_I2S_71_SCLK     = IO_FUNC_I2S_71,
    PC9_SDIO_D0            = IO_FUNC_SDIO,
    PC9_PDB_D13            = IO_FUNC_PDB,
    PC9_O1_I2S_20_WS       = IO_FUNC_I2S_20,
    PC9_O1_I2S_71_WS       = IO_FUNC_I2S_71,
    PC10_SDIO_CLK          = IO_FUNC_SDIO,
    PC10_PDB_D14           = IO_FUNC_PDB,
    PC10_O1_I2S_20_SDO0    = IO_FUNC_I2S_20,
    PC10_O1_I2S_71_SDO0    = IO_FUNC_I2S_71,
    PC11_SDIO_CMD          = IO_FUNC_SDIO,
    PC11_PDB_D15           = IO_FUNC_PDB,
    PC11_O2_I2S_20_MCLK    = IO_FUNC_I2S_20,
    PC11_O1_I2S_71_SDO1    = IO_FUNC_I2S_71,
    PC12_SDIO_D3           = IO_FUNC_SDIO,
    PC12_I2C2_SCL          = 3,
    PC12_O2_I2S_20_SCLK    = IO_FUNC_I2S_20,
    PC12_O1_I2S_71_SDO2    = IO_FUNC_I2S_71,
    PC13_SDIO_D2           = IO_FUNC_SDIO,
    PC13_I2C2_SDA          = IO_FUNC_PDB,
    PC13_O2_I2S_20_WS      = IO_FUNC_I2S_20,
    PC13_O1_I2S_71_SDO3    = IO_FUNC_I2S_71,
    PD0_SSP_MOSI           = 1,
    PD0_SPI2_MOSI          = IO_FUNC_SPI,
    PD0_UART2_TX           = 3,
    PD0_O2_I2S_20_SDO0     = IO_FUNC_I2S_20,
    PD1_SSP_MISO           = 1,
    PD1_SPI2_MISO          = IO_FUNC_SPI,
    PD1_UART2_RX           = 3,
    PD1_O3_I2S_20_MCLK     = IO_FUNC_I2S_20,
    PD2_SSP_CLK            = 1,
    PD2_SPI2_CLK           = IO_FUNC_SPI,
    PD2_UART3_TX           = 3,
    PD2_O3_I2S_20_SCLK     = IO_FUNC_I2S_20,
    PD3_SSP_FSS            = 1,
    PD3_SPI2_CS0           = IO_FUNC_SPI,
    PD3_UART3_RX           = 3,
    PD3_O3_I2S_20_WS       = IO_FUNC_I2S_20,
    PD4_SPI2_CS1           = IO_FUNC_SPI,
    PD4_SD_DET             = 3,
    PD4_O3_I2S_20_SDO0     = IO_FUNC_I2S_20,
    PD4_TCK                = IO_FUNC_TCK,
    PD5_UART2_TX           = 1,
    PD5_SSP_MOSI           = 2,
    PD5_SD_D1              = 3,
    PD5_SPDIF_I            = IO_FUNC_SPDIF_I,
    PD6_UART2_RX           = 1,
    PD6_SSP_MISO           = 2,
    PD6_SD_D0              = 3,
    PD6_SPDIF_I            = IO_FUNC_SPDIF_I,
    PD7_UART3_TX           = 1,
    PD7_SSP_CLK            = 2,
    PD7_SD_clk             = 3,
    PD7_SPDIF_I            = IO_FUNC_SPDIF_I,
    PD8_UART3_RX           = 1,
    PD8_SSP_FSS            = 2,
    PD8_SD_CMD             = 3,
    PD8_SPDIF_I            = IO_FUNC_SPDIF_I,
    PD9_I2C2_SCL           = 1,
    PD9_UART4_TX           = 2,
    PD9_SD_D3              = 3,
    PD9_PDM_CLK            = 4,
    PD9_SPDIF_O            = IO_FUNC_SPDIF_O,
    PD9_TCK                = IO_FUNC_TCK,
    PD10_I2C2_SDA          = 1,
    PD10_UART4_RX          = 2,
    PD10_SD_D2             = 3,
    PD10_PDM_IN            = 4,
    PD10_SPDIF_O           = IO_FUNC_SPDIF_O
} pin_func_e;


typedef enum {
    PORTA = 0,
    PORTB = 1,
    PORTC = 2,
    PORTD = 3,
} port_name_e;

#ifdef __cplusplus
}
#endif

#endif
