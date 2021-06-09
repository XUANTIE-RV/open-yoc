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
#include "drv/common.h"
#include <drv/uart.h>
#include "soc.h"
#include "drv/ringbuffer.h"
//#include <board_init.h>
#include "board_config.h"
#include "tst_io.h"
#include "drv/pin.h"

static csi_uart_t               g_uart;
csi_uart_t *gp_uart_copy = &g_uart;
static ringbuffer_t             g_io_ringbuffer;
static tst_io_ctrl_t            g_io_device;
static tst_uart_config_t g_uart_config;
static uint8_t g_io_rx_buf[TST_IO_BUFFER_SZ] = {0};

void tst_io_config_default(void)
{
    g_uart_config.uart_id = TST_UART_IDX;

    //g_uart_config.uart_rx.pin_port = TST_UART_RX_PIN_PORT;
    g_uart_config.uart_rx.pin_idx  = TST_UART_RX_PIN_IDX;
    g_uart_config.uart_rx.pin_func = TST_UART_RX_PIN_FUNC;

    //g_uart_config.uart_tx.pin_port = TST_UART_TX_PIN_PORT;
    g_uart_config.uart_tx.pin_idx  = TST_UART_TX_PIN_IDX;
    g_uart_config.uart_tx.pin_func = TST_UART_TX_PIN_FUNC;

    g_uart_config.uart_attr.baudrate = 115200;
    g_uart_config.uart_attr.data_bits = UART_DATA_BITS_8;
    g_uart_config.uart_attr.parity    = UART_PARITY_NONE;
    g_uart_config.uart_attr.stop_bits = UART_STOP_BITS_1;

    return;
}

/**
 * \brief              io callback
 * \param[in]          uart  handle of hw device
 * \param[in]          event hw-event signaled by hw-driver
 * \param[in]          arg   user private data
 * \return             none
 */
static void tst_io_callback(csi_uart_t *uart, csi_uart_event_t event, void *arg)
{
    uint32_t rcv_num = 0,buf_remain,wr_cnt;
    #define  TST_IO_RCV_BUF_LEN 16
    uint8_t  temp_buf[TST_IO_RCV_BUF_LEN];

    switch (event)
        {
            case UART_EVENT_RECEIVE_COMPLETE:
            case UART_EVENT_RECEIVE_FIFO_READABLE:
            {
                /* get valid data count and available spaec in ringbuf
                 * set rcv_num = MIN(rcv_num,buf_remain)
                 */
                rcv_num = csi_uart_receive(uart, temp_buf,TST_IO_RCV_BUF_LEN ,0);
                buf_remain = ringbuffer_avail(g_io_device.fifo_handle);

                wr_cnt = (buf_remain <= rcv_num)?buf_remain:rcv_num;
                /* write data to io buf */
                ringbuffer_in(g_io_device.fifo_handle,temp_buf, wr_cnt);
                if(wr_cnt < rcv_num)
                {
                  //todo data lost due to buffer size
                }

            }
            break;

            case UART_EVENT_ERROR_OVERFLOW:
            case UART_EVENT_ERROR_PARITY:
            case UART_EVENT_ERROR_FRAMING:
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
csi_error_t tst_io_init(tst_uart_config_t * uart_config)
{
    csi_error_t ret = CSI_OK;

    /* init io buf*/

    memset(&g_io_ringbuffer,0,sizeof(g_io_ringbuffer));
    g_io_ringbuffer.buffer = g_io_rx_buf;
    if(g_io_ringbuffer.buffer == NULL)
    {
        return CSI_ERROR;
    }
    g_io_ringbuffer.size = TST_IO_BUFFER_SZ;
    g_io_device.fifo_handle = (ringbuffer_t*)&g_io_ringbuffer;

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
    csi_pin_set_mux(  g_uart_config.uart_rx.pin_idx,\
                      g_uart_config.uart_rx.pin_func);
    // tx pin-mux config
    csi_pin_set_mux(g_uart_config.uart_tx.pin_idx,\
                      g_uart_config.uart_tx.pin_func);

    /* init io handle */
    ret = csi_uart_init((csi_uart_t*)&g_uart,g_uart_config.uart_id);
    if(ret != CSI_OK)
    {
        return ret;
    }
    /* init config io param */
    ret = csi_uart_baud((csi_uart_t*)&g_uart, g_uart_config.uart_attr.baudrate);
    if(ret != CSI_OK)
    {
        return ret;
    }
    ret =  csi_uart_format((csi_uart_t*)&g_uart,g_uart_config.uart_attr.data_bits,
                            g_uart_config.uart_attr.parity, g_uart_config.uart_attr.stop_bits);
    if(ret != CSI_OK)
    {
        return ret;
    }

    ret = csi_uart_attach_callback((csi_uart_t*)&g_uart,tst_io_callback, NULL);
    if(ret != CSI_OK)
    {
        return ret;
    }


    g_io_device.io_handle = (csi_uart_t*)&g_uart;

    return CSI_OK;
}

/**
 * \brief              send data to io
 * \param[in]          buf      data buf
 * \param[in]          buf_len  lenght of data
 * \return             err code
 */

csi_error_t tst_io_send(char *buf, uint32_t buf_len)
{
    uint32_t send_num;
    send_num = csi_uart_send(g_io_device.io_handle, (const void *) buf, buf_len, 0xFFFF);
    return (send_num == buf_len)?CSI_OK:CSI_ERROR;
}

/**
 * \brief              receive data from io
 * \param[in]          buf      point to data buf
 * \param[in]          buf_len  lenght of data
 * \return             valid data size
 */

uint32_t tst_io_rcv(char* buf,uint32_t buf_len)
{
    uint32_t io_availabe_len = 0,read_len;
    CSI_ASSERT(buf && buf_len > 0);

    io_availabe_len = ringbuffer_avail(g_io_device.fifo_handle);
    read_len = (io_availabe_len <= buf_len)?io_availabe_len:buf_len;
    if(read_len)
    {
        return ringbuffer_out(g_io_device.fifo_handle,buf,read_len);
    }

    return 0;
}

