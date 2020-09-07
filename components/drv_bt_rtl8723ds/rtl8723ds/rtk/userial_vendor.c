/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Realtek Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Filename:      userial_vendor.c
 *
 *  Description:   Contains vendor-specific userial functions
 *
 ******************************************************************************/
#undef NDEBUG
#define LOG_TAG "bt_userial_vendor"
// #include "bt_trace.h"
#include <aos/debug.h>
#include <aos/log.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "userial_vendor.h"
#ifndef __NEWLIB__
#include <termios.h>
#endif

#include <devices/uart.h>

static char *TAG = "vendor";

/******************************************************************************
**  Constants & Macros
******************************************************************************/
#ifndef CONFIG_HCI_UART_ID
#define CONFIG_HCI_UART_ID 0
#endif



/******************************************************************************
**  Local type definitions
******************************************************************************/

/* vendor serial control block */
typedef struct {
    aos_dev_t* uart_dev;
    uart_config_t config;
    //struct termios termios;     /* serial terminal of BT port */
    //char port_name[VND_PORT_NAME_MAXLEN];
} vnd_userial_cb_t;

/******************************************************************************
**  Static variables
******************************************************************************/

static vnd_userial_cb_t vnd_userial;


// static void uart_event(aos_dev_t *dev, int event_id, void *priv)
// {
//     if (event_id == USART_EVENT_READ || event_id == USART_OVERFLOW) {
//         hci_hal_h5_task_post(100);
//     }
// }

/*****************************************************************************
**   Helper Functions
*****************************************************************************/

/*******************************************************************************
**
** Function        userial_to_tcio_baud
**
** Description     helper function converts USERIAL baud rates into TCIO
**                  conforming baud rates
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t userial_to_tcio_baud(uint8_t cfg_baud, uint32_t *baud)
{
    if (cfg_baud == USERIAL_BAUD_115200) {
        *baud = 115200;
    } else if (cfg_baud == USERIAL_BAUD_4M) {
        *baud = 4000000;
    } else if (cfg_baud == USERIAL_BAUD_3M) {
        *baud = 3000000;
    } else if (cfg_baud == USERIAL_BAUD_2M) {
        *baud = 2000000;
    } else if (cfg_baud == USERIAL_BAUD_1M) {
        *baud = 1000000;
    } else if (cfg_baud == USERIAL_BAUD_1_5M) {
        *baud = 1500000;
    } else if (cfg_baud == USERIAL_BAUD_921600) {
        *baud = 921600;
    } else if (cfg_baud == USERIAL_BAUD_460800) {
        *baud = 460800;
    } else if (cfg_baud == USERIAL_BAUD_230400) {
        *baud = 230400;
    } else if (cfg_baud == USERIAL_BAUD_57600) {
        *baud = 57600;
    } else if (cfg_baud == USERIAL_BAUD_19200) {
        *baud = 19200;
    } else if (cfg_baud == USERIAL_BAUD_9600) {
        *baud = 9600;
    } else if (cfg_baud == USERIAL_BAUD_1200) {
        *baud = 1200;
    } else if (cfg_baud == USERIAL_BAUD_600) {
        *baud = 600;
    } else {
        LOGE(TAG, "userial vendor open: unsupported baud idx %i", cfg_baud);
        *baud = 115200;
        return 0;
    }

    return 1;
}

/*****************************************************************************
**   Userial Vendor API Functions
*****************************************************************************/

/*******************************************************************************
**
** Function        userial_vendor_init
**
** Description     Initialize userial vendor-specific control block
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_init()
{
    //vnd_userial.fd = -1;
    //snprintf(vnd_userial.port_name, VND_PORT_NAME_MAXLEN, "%s",
    //        bt_device_node);
}


/*******************************************************************************
**
** Function        userial_vendor_open
**
** Description     Open the serial port with the given configuration
**
** Returns         device fd
**
*******************************************************************************/
int userial_vendor_open(tUSERIAL_CFG *p_cfg, void *uart_event)
{
    uint32_t baud;
    hal_uart_data_width_t data_bits;
    hal_uart_parity_t parity;
    hal_uart_stop_bits_t stop_bits;
    hal_uart_flow_control_t fc;

    if (!userial_to_tcio_baud(p_cfg->baud, &baud)) {
        return -1;
    }

    if (p_cfg->fmt & USERIAL_DATABITS_8) {
        data_bits = DATA_WIDTH_8BIT;
    } else if (p_cfg->fmt & USERIAL_DATABITS_7) {
        data_bits = DATA_WIDTH_7BIT;
    } else if (p_cfg->fmt & USERIAL_DATABITS_6) {
        data_bits = DATA_WIDTH_6BIT;
    } else if (p_cfg->fmt & USERIAL_DATABITS_5) {
        data_bits = DATA_WIDTH_5BIT;
    } else {
        LOGE(TAG, "userial vendor open: unsupported data bits");
        return -1;
    }

    if (p_cfg->fmt & USERIAL_PARITY_NONE) {
        parity = PARITY_NONE;
    } else if (p_cfg->fmt & USERIAL_PARITY_EVEN) {
        parity = PARITY_EVEN;
    } else if (p_cfg->fmt & USERIAL_PARITY_ODD) {
        parity = PARITY_ODD;
    } else {
        LOGE(TAG, "userial vendor open: unsupported parity bit mode");
        return -1;
    }

    if (p_cfg->fmt & USERIAL_STOPBITS_1) {
        stop_bits = STOP_BITS_1;
    } else if (p_cfg->fmt & USERIAL_STOPBITS_2) {
        stop_bits = STOP_BITS_2;
    } else {
        LOGE(TAG, "userial vendor open: unsupported stop bits");
        return -1;
    }

    if (p_cfg->fmt & USERIAL_HW_FLOW_CTRL_OFF) {
        fc = FLOW_CONTROL_DISABLED;
    } else {
        fc = FLOW_CONTROL_CTS_RTS;
    }

    vnd_userial.uart_dev = uart_open_id("uart", p_cfg->uart_id);

    if (vnd_userial.uart_dev == NULL) {
        return -1;
    }

    uart_config_default(&vnd_userial.config);
    vnd_userial.config.baud_rate = 115200;
    vnd_userial.config.parity = parity;
    vnd_userial.config.data_width = data_bits;
    vnd_userial.config.stop_bits = stop_bits;
    vnd_userial.config.flow_control = fc;
    uart_config(vnd_userial.uart_dev, &vnd_userial.config);

    uart_set_event(vnd_userial.uart_dev, uart_event, NULL);

    return 0;
}

/*******************************************************************************
**
** Function        userial_vendor_close
**
** Description     Conduct vendor-specific close work
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_close(void)
{
    aos_check_param(vnd_userial.uart_dev);

    uart_close(vnd_userial.uart_dev);
    vnd_userial.uart_dev = NULL;

}

/*******************************************************************************
**
** Function        userial_vendor_set_baud
**
** Description     Set new baud rate
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_set_baud(uint8_t userial_baud)
{
    aos_check_param(vnd_userial.uart_dev);
    uint32_t tcio_baud;

    userial_to_tcio_baud(userial_baud, &tcio_baud);

    vnd_userial.config.baud_rate = tcio_baud;
    uart_config(vnd_userial.uart_dev, &vnd_userial.config);
    uart_set_buffer_size(vnd_userial.uart_dev, 10240);
}


void userial_vendor_set_parity(uint8_t mode)
{
    aos_check_param(vnd_userial.uart_dev);
    vnd_userial.config.parity = mode;
    uart_config(vnd_userial.uart_dev, &vnd_userial.config);
    uart_set_buffer_size(vnd_userial.uart_dev, 10240);
}
/*******************************************************************************
**
** Function        userial_set_port
**
** Description     Configure UART port name
**
** Returns         0 : Success
**                 Otherwise : Fail
**
*******************************************************************************/
int userial_set_port(char *p_conf_name, char *p_conf_value, int param)
{
    return 0;
}

/*******************************************************************************
**
** Function        userial_vendor_set_hw_fctrl
**
** Description     Conduct vendor-specific close work
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_set_hw_fctrl(uint8_t hw_fctrl)
{
    aos_check_param(vnd_userial.uart_dev);

    if (hw_fctrl) {
        vnd_userial.config.flow_control = FLOW_CONTROL_CTS_RTS;
    } else {
        vnd_userial.config.flow_control = FLOW_CONTROL_DISABLED;
    }
}

int userial_vendor_send_data(uint8_t *data, uint32_t len)
{
    return uart_send(vnd_userial.uart_dev, data, len);
}

int userial_vendor_recv_data(uint8_t *data, uint32_t len, uint32_t timeout)
{
    return uart_recv(vnd_userial.uart_dev, data, len, timeout);
}
