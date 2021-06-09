/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     tst_io.h
 * @brief    input/output for test
 * @version  V0.1
 * @date     2020-01-01
 ******************************************************************************/

#ifndef _TST_IO_H_
#define _TST_IO_H_

#include <board_config.h>

#ifdef __cplusplus
extern "C" {
#endif
#define TST_IO_BUFFER_SZ              (256)

#define TST_UART_IDX                  (CONSOLE_UART_IDX)
//#define TST_UART_TX_PIN_PORT          (CONSOLE_TX_PORT)
#define TST_UART_TX_PIN_IDX           (CONSOLE_TXD)
#define TST_UART_TX_PIN_FUNC          (CONSOLE_TXD_FUNC)
//#define TST_UART_RX_PIN_PORT          (CONSOLE_RX_PORT)
#define TST_UART_RX_PIN_IDX           (CONSOLE_RXD)
#define TST_UART_RX_PIN_FUNC          (CONSOLE_RXD_FUNC)
/*！
 * \def     pin_config_t
 * \brief   discribe a io-mux pin
 */

typedef struct {
    //uint8_t pin_port;                                   ///< IO port index
    uint8_t pin_idx ;                                   ///< offset of IO port
    uint8_t pin_func;                                   ///< function of pin
    uint8_t pad;                                        ///< pad
}tst_pin_config_t;

typedef struct{
    uint32_t                baudrate;
    usart_data_bits_e       data_bits;
    usart_parity_e          parity;
    usart_stop_bits_e       stop_bits;
}tst_uart_attr_t;

/*！
 * \def     uart_config_t
 * \brief   config setting for one hw-uart
 */

typedef struct {
    uint32_t          uart_id;                          ///< uart index
    tst_pin_config_t  uart_rx;                          ///< io config for rx pin
    tst_pin_config_t  uart_tx;                          ///< io config for tx pin
    tst_uart_attr_t   uart_attr;                        ///< uart attributes
}tst_uart_config_t;

typedef struct {
    usart_handle_t       io_handle;
    dev_ringbuf_t*       fifo_handle;
    uint32_t             io_status;
}tst_io_ctrl_t;

/**
 * \brief              init io device
 * \param              uart_config  device param set by user
 *                     if NULL,use default param.
 * \return             err code
 */
int32_t tst_io_init(tst_uart_config_t * uart_config);

/**
 * \brief              send data to io
 * \param[in]          buf      data buf
 * \param[in]          buf_len  lenght of data
 * \return             err code
 */

int32_t tst_io_send(char *buf, uint32_t buf_len);


/**
 * \brief              receive data from io
 * \param[in]          buf      point to data buf
 * \param[in]          buf_len  lenght of data
 * \return             valid data size
 */

int32_t tst_io_rcv(char* buf,uint32_t buf_len);
//void tst_delay(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif
