/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pin.h
 * @brief    header File for pin definition
 * @version  V1.0
 * @date     02. June 2018
 ******************************************************************************/

#ifndef _PIN_H_
#define _PIN_H_

#include <stdint.h>
#include "pin_name.h"
#include "pinmux.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_TXD         PA19
#define CONSOLE_RXD         PA20
#define CONSOLE_TXD_FUNC    PA19_UART1_TX
#define CONSOLE_RXD_FUNC    PA20_UART1_RX
#define CONSOLE_IDX    1

#define EXAMPLE_SRAM_ADDR   CSKY_CK804_TCM7_BASE
#define EXAMPLE_SRAM_LEN    CSKY_CK804_TCM_SIZE

#define EXAMPLE_DTCM_ADDR   CSKY_CK804_TCM1_BASE
#define EXAMPLE_DTCM_LEN    CSKY_CK804_TCM_SIZE

#define EXAMPLE_SDRAM_ADDR  CSKY_FMC_BASE
#define EXAMPLE_SDRAM_LEN   (CSKY_FMC_SIZE >> 3)

/* example pin manager */
#define EXAMPLE_USART_IDX                   1
#define EXAMPLE_PIN_USART_TX                PA19
#define EXAMPLE_PIN_USART_RX                PA20
#define EXAMPLE_PIN_USART_TX_FUNC           PA19_UART1_TX
#define EXAMPLE_PIN_USART_RX_FUNC           PA20_UART1_RX

#define EXAMPLE_USI_USART_IDX               3
#define EXAMPLE_PIN_USI_USART_TX            PA11
#define EXAMPLE_PIN_USI_USART_RX            PA10
#define EXAMPLE_PIN_USI_USART_TX_FUNC       PA11_USI3_SD0
#define EXAMPLE_PIN_USI_USART_RX_FUNC       PA10_USI3_SCLK

#define EXAMPLE_USI_SPI_IDX             1
#define EXAMPLE_PIN_USI_SPI_MISO        PA6
#define EXAMPLE_PIN_USI_SPI_MOSI        PA5
#define EXAMPLE_PIN_USI_SPI_CS          PA7
#define EXAMPLE_PIN_USI_SPI_SCK         PA4
#define EXAMPLE_PIN_USI_SPI_MISO_FUNC   PA6_USI1_SD1
#define EXAMPLE_PIN_USI_SPI_MOSI_FUNC   PA5_USI1_SD0
#define EXAMPLE_PIN_USI_SPI_CS_FUNC     PIN_FUNC_GPIO
#define EXAMPLE_PIN_USI_SPI_SCK_FUNC    PA4_USI1_SCLK

#define EXAMPLE_SPI_IDX             1
#define EXAMPLE_PIN_SPI_MISO        PA6
#define EXAMPLE_PIN_SPI_MOSI        PA5
#define EXAMPLE_PIN_SPI_CS          PA7
#define EXAMPLE_PIN_SPI_SCK         PA4
#define EXAMPLE_PIN_SPI_MISO_FUNC   PA6_USI1_SD1
#define EXAMPLE_PIN_SPI_MOSI_FUNC   PA5_USI1_SD0
#define EXAMPLE_PIN_SPI_CS_FUNC     PIN_FUNC_GPIO
#define EXAMPLE_PIN_SPI_SCK_FUNC    PA4_USI1_SCLK

#define EXAMPLE_USI_IIC_IDX             0
#define EXAMPLE_PIN_USI_IIC_SDA         PA1
#define EXAMPLE_PIN_USI_IIC_SCL         PA0
#define EXAMPLE_PIN_USI_IIC_SDA_FUNC    PA1_USI0_SD0
#define EXAMPLE_PIN_USI_IIC_SCL_FUNC    PA0_USI0_SCLK

#define EXAMPLE_IIC_IDX                 0
#define EXAMPLE_PIN_IIC_SDA             PA1
#define EXAMPLE_PIN_IIC_SCL             PA0
#define EXAMPLE_PIN_IIC_SDA_FUNC        PA1_USI0_SD0
#define EXAMPLE_PIN_IIC_SCL_FUNC        PA0_USI0_SCLK

#define EXAMPLE_SDIO_IDX                1
#define EXAMPLE_PIN_SDIO_D0             PA24
#define EXAMPLE_PIN_SDIO_D1             PA25
#define EXAMPLE_PIN_SDIO_D2             PA26
#define EXAMPLE_PIN_SDIO_D3             PA27
#define EXAMPLE_PIN_SDIO_CLK            PA23
#define EXAMPLE_PIN_SDIO_CMD            PA28

#define EXAMPLE_PIN_SDIO_D0_FUNC        PA24_EMMC_DAT0
#define EXAMPLE_PIN_SDIO_D1_FUNC        PA25_EMMC_DAT1
#define EXAMPLE_PIN_SDIO_D2_FUNC        PA26_EMMC_DAT2
#define EXAMPLE_PIN_SDIO_D3_FUNC        PA27_EMMC_DAT3
#define EXAMPLE_PIN_SDIO_CLK_FUNC       PA23_EMMC_CLK
#define EXAMPLE_PIN_SDIO_CMD_FUNC       PA28_EMMC_CMD

#define QSPIFLASH_IDX                   0
#define EXAMPLE_QSPI_IDX                0
#define EXAMPLE_PIN_QSPI_MOSI           PA18
#define EXAMPLE_PIN_QSPI_MISO           PA17
#define EXAMPLE_PIN_QSPI_CS             PA13
#define EXAMPLE_PIN_QSPI_SCK            PA12
#define EXAMPLE_PIN_QSPI_HOLD           PA20
#define EXAMPLE_PIN_QSPI_WP             PA19
#define EXAMPLE_PIN_QSPI_MOSI_FUNC      PA18_QSPI_DATA1
#define EXAMPLE_PIN_QSPI_MISO_FUNC      PA17_QSPI_DATA0
#define EXAMPLE_PIN_QSPI_CS_FUNC        PA13_QSPI_SS0
#define EXAMPLE_PIN_QSPI_SCK_FUNC       PA12_QSPI_SCLK
#define EXAMPLE_PIN_QSPI_WP_FUNC        PA20_QSPI_DATA3
#define EXAMPLE_PIN_QSPI_HOLD_FUNC      PA19_QSPI_DATA2

#define EXAMPLE_DEST_MAILBOX_IDX             0

#define EXAMPLE_GPIO_PIN            PA21
#define EXAMPLE_BOARD_GPIO_PIN_NAME "J21.14"
#define EXAMPLE_GPIO_PIN_FUNC       PIN_FUNC_GPIO

/* tests pin manager */

#define TEST_USART_IDX               4
#define TEST_PIN_USART_TX            PA23
#define TEST_PIN_USART_RX            PA22
#define TEST_PIN_USART_TX_FUNC       PA23_USI4_SD0
#define TEST_PIN_USART_RX_FUNC       PA22_USI4_SCLK

#define TEST_USI_USART_IDX           1
#define TEST_PIN_USI_USART_TX        PA5
#define TEST_PIN_USI_USART_RX        PA4
#define TEST_PIN_USI_USART_TX_FUNC   0
#define TEST_PIN_USI_USART_RX_FUNC   0

#define TEST_USI_SPI_IDX             0
#define TEST_PIN_USI_SPI_MISO        PA0
#define TEST_PIN_USI_SPI_MOSI        PA1
#define TEST_PIN_USI_SPI_CS          PA2
#define TEST_PIN_USI_SPI_SCK         PA3
#define TEST_PIN_USI_SPI_MISO_FUNC   0
#define TEST_PIN_USI_SPI_MOSI_FUNC   0
#define TEST_PIN_USI_SPI_CS_FUNC     4
#define TEST_PIN_USI_SPI_SCK_FUNC    0

#define TEST_USI_IIC_IDX             2
#define TEST_PIN_USI_IIC_SDA         PA9
#define TEST_PIN_USI_IIC_SCL         PA8
#define TEST_PIN_USI_IIC_SDA_FUNC    0
#define TEST_PIN_USI_IIC_SCL_FUNC    0

#define TEST_PIN_QSPI_MOSI           PA18
#define TEST_PIN_QSPI_MISO           PA17
#define TEST_PIN_QSPI_CS             PA13
#define TEST_PIN_QSPI_SCK            PA12
#define TEST_PIN_QSPI_HOLD           PA20
#define TEST_PIN_QSPI_WP             PA19
#define TEST_PIN_QSPI_MOSI_FUNC      PA18_QSPI_DATA1
#define TEST_PIN_QSPI_MISO_FUNC      PA17_QSPI_DATA0
#define TEST_PIN_QSPI_CS_FUNC        PA13_QSPI_SS0
#define TEST_PIN_QSPI_SCK_FUNC       PA12_QSPI_SCLK
#define TEST_PIN_QSPI_WP_FUNC        PA20_QSPI_DATA3
#define TEST_PIN_QSPI_HOLD_FUNC      PA19_QSPI_DATA2

#define TEST_GPIO_PIN               PA22
#define TEST_BOARD_GPIO_PIN_NAME    "J21.15"
#define TEST_GPIO_PIN_FUNC          PIN_FUNC_GPIO

#ifdef __cplusplus
}
#endif

#endif /* _PIN_H_ */

