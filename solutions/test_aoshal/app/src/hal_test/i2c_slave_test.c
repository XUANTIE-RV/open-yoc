/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <aos/hal/i2c.h>
#include <soc.h>
#include <ulog/ulog.h>
#include <yoc/atserver.h>
#include <soc.h>

#define PIN_IIC0_SDA             PA22
#define PIN_IIC0_SCL             PA21
#define PIN_IIC0_SDA_FUNC        0
#define PIN_IIC0_SCL_FUNC        0

#define I2C0_PORT_NUM  0
#define I2C_BUF_SIZE   32
#define I2C_TX_TIMEOUT 10000
#define I2C_RX_TIMEOUT 10000
#define I2C_TX_TIMEOUT_2 HAL_WAIT_FOREVER
#define I2C_RX_TIMEOUT_2 HAL_WAIT_FOREVER

#define I2C_DEV_ADDR       0x40
#define I2C_DEV_ADDR_WIDTH 16

#define AT_BACK_READY()                       atserver_send("\r\nREADY\r\n")

/* define dev */
i2c_dev_t i2c0;
/* data buffer */
char i2c_data_slave_buf[I2C_BUF_SIZE] = {0};
char i2c_data_slave_buf1[I2C_BUF_SIZE] = {0};


void hal_iic_slaver_recv(uint32_t freq,uint32_t width,uint16_t size,uint32_t rx_timeout)
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* i2c port set */
    i2c0.port = I2C0_PORT_NUM;

    /* i2c attr config */
    i2c0.config.dev_addr = I2C_DEV_ADDR;
    i2c0.config.freq = freq;
    i2c0.config.address_width = width;
    i2c0.config.mode = I2C_MODE_SLAVE;

    /* init i2c with the given settings */
    ret = hal_i2c_init(&i2c0);
    if (ret != 0) {
        printf("i2c0 init error !\n");
        AT_BACK_ERR();
        return;
    }

    memset(i2c_data_slave_buf1, 0, I2C_BUF_SIZE);

    ret = hal_i2c_slave_recv(&i2c0, i2c_data_slave_buf1,
                       size, rx_timeout);

    if (ret == 0) {
        printf("slave data received succeed !\n");
    } else {
        AT_BACK_ERR();
        return;
    }

    for (i; i < size; i++) {
        printf("slave recv is %d\n",i2c_data_slave_buf1[i]);
        if (i2c_data_slave_buf1[i] != i+1) {
            AT_BACK_ERR();
            return;
        }
    }

    ret = hal_i2c_finalize(&i2c0);
    if (ret != 0) {
        printf("i2c0 finalize error !\n");
        AT_BACK_ERR();
        return;
    }

    AT_BACK_OK(); 
}

void hal_iic_slaver_send(uint32_t freq,uint32_t width,uint16_t size,uint32_t tx_timeout)
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* i2c port set */
    i2c0.port = I2C0_PORT_NUM;

    /* i2c attr config */
    i2c0.config.dev_addr = I2C_DEV_ADDR;
    i2c0.config.freq = freq;
    i2c0.config.address_width = width;
    i2c0.config.mode = I2C_MODE_SLAVE;

    /* init i2c with the given settings */
    ret = hal_i2c_init(&i2c0);
    if (ret != 0) {
        printf("i2c0 init error !\n");
        AT_BACK_ERR();
        return;
    }

    /* init the tx buffer */
    for (int i = 0; i < size; i++) {
        i2c_data_slave_buf[i] = i;
    }

    ret = hal_i2c_slave_send(&i2c0, i2c_data_slave_buf,
                       size, tx_timeout);
    for (int i = 0; i < size; i++)
    {
        printf("slave send is %d\n",i2c_data_slave_buf[i]);
    }
    if (ret == 0) {
        printf("slave data send succeed !\n");
    } else {
        AT_BACK_ERR();
        return;
    }

    ret = hal_i2c_finalize(&i2c0);
    if (ret != 0) {
        printf("i2c0 finalize error !\n");
        AT_BACK_ERR();
        return;
    }

    AT_BACK_OK(); 
}

void test_hal_iic_slave(char *cmd, int type, char *data)
{
    csi_pin_set_mux(PIN_IIC0_SDA, PIN_IIC0_SDA_FUNC);
    csi_pin_set_mux(PIN_IIC0_SCL, PIN_IIC0_SCL_FUNC);

    if (strcmp((const char *)data, "'freq=100K'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_recv(100000,I2C_HAL_ADDRESS_WIDTH_10BIT,16,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'freq=400K'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_recv(400000,I2C_HAL_ADDRESS_WIDTH_7BIT,16,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'freq=3.4M'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_recv(3400000,I2C_HAL_ADDRESS_WIDTH_10BIT,16,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'width=7bit'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_recv(100000,I2C_HAL_ADDRESS_WIDTH_7BIT,16,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'width=10bit'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_recv(100000,I2C_HAL_ADDRESS_WIDTH_10BIT,16,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'size=1'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_recv(100000,I2C_HAL_ADDRESS_WIDTH_10BIT,1,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'size=32'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_recv(100000,I2C_HAL_ADDRESS_WIDTH_10BIT,32,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'send_size=1'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_send(100000,I2C_HAL_ADDRESS_WIDTH_7BIT,1,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'send'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_send(100000,I2C_HAL_ADDRESS_WIDTH_7BIT,16,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'TIMEOUT_FOREVER'\0") == 0) {
        AT_BACK_READY();
        hal_iic_slaver_recv(100000,I2C_HAL_ADDRESS_WIDTH_7BIT,16,I2C_RX_TIMEOUT_2);
    }
}