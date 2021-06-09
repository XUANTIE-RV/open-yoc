/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     tst_io.c
 * @brief    input/output for test
 * @version  V0.1
 * @date     2020-01-01
 ******************************************************************************/
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "drv/errno.h"
#include "drv/usart.h"
#include "soc.h"
#include "dev_ringbuf.h"
#include "pin_name.h"
#include "tst_io.h"
#include "pin_name.h"
#include "pinmux.h"

static dev_ringbuf_t             g_io_ringbuffer;
static tst_io_ctrl_t            g_io_device;
static tst_uart_config_t g_uart_config;
static uint8_t g_io_rx_buf[TST_IO_BUFFER_SZ] = {0};

void tst_io_config_default(void)
{
    g_uart_config.uart_id = TST_UART_IDX;

    g_uart_config.uart_rx.pin_idx  = TST_UART_RX_PIN_IDX;
    g_uart_config.uart_rx.pin_func = TST_UART_RX_PIN_FUNC;

    g_uart_config.uart_tx.pin_idx  = TST_UART_TX_PIN_IDX;
    g_uart_config.uart_tx.pin_func = TST_UART_TX_PIN_FUNC;

    g_uart_config.uart_attr.baudrate = 115200;
    g_uart_config.uart_attr.data_bits = USART_DATA_BITS_8;
    g_uart_config.uart_attr.parity    = USART_PARITY_NONE;
    g_uart_config.uart_attr.stop_bits = USART_STOP_BITS_1;

    return;
}

/**
 * \brief              io callback
 * \param[in]          uart  handle of hw device
 * \param[in]          event hw-event signaled by hw-driver
 * \param[in]          arg   user private data
 * \return             none
 */
static void tst_io_callback(int32_t idx, usart_event_e event)
{
    uint32_t rcv_num = 0,buf_remain,wr_cnt;
    #define  TST_IO_RCV_BUF_LEN 16
    uint8_t  temp_buf[TST_IO_RCV_BUF_LEN];

    switch (event)
        {
            case USART_EVENT_RECEIVE_COMPLETE:
            case USART_EVENT_RECEIVED:
            {
                /* get valid data count and available spaec in ringbuf
                 * set rcv_num = MIN(rcv_num,buf_remain)
                 */

                rcv_num = csi_usart_receive_query(g_io_device.io_handle, temp_buf,TST_IO_RCV_BUF_LEN );
                buf_remain = dev_ringbuf_avail(g_io_device.fifo_handle);

                wr_cnt = (buf_remain <= rcv_num)?buf_remain:rcv_num;
                /* write data to io buf */
                dev_ringbuf_in(g_io_device.fifo_handle,temp_buf, wr_cnt);
                if(wr_cnt < rcv_num)
                {
                  //todo data lost due to buffer size
                }

            }
            break;

            case USART_EVENT_RX_OVERFLOW:
            case USART_EVENT_RX_PARITY_ERROR:
            case USART_EVENT_RX_FRAMING_ERROR:
            {
                //todo
                g_io_device.io_status |= 1<<event;
            }
            break;

            default:
            break;
        }
}

/**
 * \brief              init io device
 * \param              uart_config  device param set by user
 *                     if NULL,use default param.
 * \return             err code
 */
int32_t  tst_io_init(tst_uart_config_t * uart_config)
{
    int32_t ret = 0;

    /* init io buf*/

    memset(&g_io_ringbuffer,0,sizeof(g_io_ringbuffer));
    g_io_ringbuffer.buffer = g_io_rx_buf;
    if(g_io_ringbuffer.buffer == NULL)
    {
        return DRV_ERROR;
    }
    g_io_ringbuffer.size = TST_IO_BUFFER_SZ;
    g_io_device.fifo_handle = (dev_ringbuf_t*)&g_io_ringbuffer;

    /* build uart config */
    if(uart_config)
    {
        memcpy(&g_uart_config,uart_config,sizeof(g_uart_config));
    }
    else
    {
        tst_io_config_default();
    }

    /* init io mux */

    // rx pin-mux config
    drv_pinmux_config( g_uart_config.uart_rx.pin_idx, g_uart_config.uart_rx.pin_func);
    // tx pin-mux config
    drv_pinmux_config(g_uart_config.uart_tx.pin_idx, g_uart_config.uart_tx.pin_func);

    /* init io handle */
    g_io_device.io_handle = csi_usart_initialize(g_uart_config.uart_id, tst_io_callback);
    if(g_io_device.io_handle == NULL)
    {
        return DRV_ERROR;
    }

    ret = csi_usart_config(g_io_device.io_handle, \
                           g_uart_config.uart_attr.baudrate, \
                           USART_MODE_ASYNCHRONOUS, \
                           g_uart_config.uart_attr.parity, \
                           g_uart_config.uart_attr.stop_bits, \
                           g_uart_config.uart_attr.data_bits);
    return ret;
}

/**
 * \brief              send data to io
 * \param[in]          buf      data buf
 * \param[in]          buf_len  lenght of data
 * \return             err code
 */

int32_t tst_io_send(char *buf, uint32_t buf_len)
{
    uint32_t ret;
    int time_out = 0x7ffff;
    usart_status_t status;
    ret = csi_usart_send(g_io_device.io_handle, (const void *) buf, buf_len);
    while (time_out) {
        time_out--;
        status = csi_usart_get_status(g_io_device.io_handle);

        if (!status.tx_busy) {
            break;
        }
    }

    if (0 == time_out) {
        return -1;
    }

    return ret;
}

/**
 * \brief              receive data from io
 * \param[in]          buf      point to data buf
 * \param[in]          buf_len  lenght of data
 * \return             valid data size
 */

int32_t tst_io_rcv(char* buf,uint32_t buf_len)
{
    int32_t io_availabe_len = 0,read_len;
    if(buf == NULL || buf == 0)
    {
       return -1;
    }
    io_availabe_len = dev_ringbuf_avail(g_io_device.fifo_handle);
    read_len = (io_availabe_len <= buf_len)?io_availabe_len:buf_len;
    if(read_len)
    {
        return dev_ringbuf_out(g_io_device.fifo_handle,buf,read_len);
    }

    return 0;
}
/*
void tst_delay(uint32_t ms)
{
    uint32_t i,j;
    volatile uint32_t k;

    for(i=0;i<ms;i++)
        for(j=0;j<3950;j++){k++;}

}*/

