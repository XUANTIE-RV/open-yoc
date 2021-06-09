/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SC5654_USART2_TX            PA4
#define SC5654_USART2_RX            PA5
#define SC5654_USART2_TX_FUNC       PA4_UART2_TX
#define SC5654_USART2_RX_FUNC       PA5_UART2_RX
#define CONSOLE_IDX         1

#define SC5654_USART3_TX            PD7
#define SC5654_USART3_RX            PD8
#define SC5654_USART3_TX_FUN        PD7_UART3_TX
#define SC5654_USART3_RX_FUN        PD8_UART3_RX

/* for sdio wifi below */
#define EXAMPLE_PIN_SDIO0_D0         PC2
#define EXAMPLE_PIN_SDIO0_D1         PC1
#define EXAMPLE_PIN_SDIO0_D2         PC6
#define EXAMPLE_PIN_SDIO0_D3         PC5
#define EXAMPLE_PIN_SDIO0_CLK        PC3
#define EXAMPLE_PIN_SDIO0_CMD        PC4
#define EXAMPLE_PIN_SDIO0_DET        PC0

#define EXAMPLE_PIN_SDIO0_D0_FUNC     PC2_SD_D0
#define EXAMPLE_PIN_SDIO0_D1_FUNC     PC1_SD_D1
#define EXAMPLE_PIN_SDIO0_D2_FUNC     PC6_SD_D2
#define EXAMPLE_PIN_SDIO0_D3_FUNC     PC5_SD_D3
#define EXAMPLE_PIN_SDIO0_CLK_FUNC    PC3_SD_CLK
#define EXAMPLE_PIN_SDIO0_CMD_FUNC    PC4_SD_CMD_CMD
#define EXAMPLE_PIN_SDIO0_DET_FUNC    PC0_SD_DET

/* for sd card below */
#define EXAMPLE_PIN_SDIO1_D0         PC9
#define EXAMPLE_PIN_SDIO1_D1         PC8
#define EXAMPLE_PIN_SDIO1_D2         PC13
#define EXAMPLE_PIN_SDIO1_D3         PC12
#define EXAMPLE_PIN_SDIO1_CLK        PC10
#define EXAMPLE_PIN_SDIO1_CMD        PC11
#define EXAMPLE_PIN_SDIO1_DET        PC7

#define EXAMPLE_PIN_SDIO1_D0_FUNC     PC9_SDIO_D0
#define EXAMPLE_PIN_SDIO1_D1_FUNC     PC8_SDIO_D1
#define EXAMPLE_PIN_SDIO1_D2_FUNC     PC13_SDIO_D2
#define EXAMPLE_PIN_SDIO1_D3_FUNC     PC12_SDIO_D3
#define EXAMPLE_PIN_SDIO1_CLK_FUNC    PC10_SDIO_CLK
#define EXAMPLE_PIN_SDIO1_CMD_FUNC    PC11_SDIO_CMD
#define EXAMPLE_PIN_SDIO1_DET_FUNC    PC7_SDIO_DET

#ifdef __cplusplus
}
#endif

#endif