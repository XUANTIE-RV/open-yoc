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
// #include <ulog/ulog.h>
#include <yoc/atserver.h>
#include <soc.h>

#define CK_IIC_SLAVE_ADDR         0x50
#define EEPROM_PAGE_ADDR          0x00
#define EEPROM_PAGE_SIZE          10

#define DEVICE_ZERO               0x0
#define DEVICE_ONE                0x1

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

/* define dev */
i2c_dev_t i2c0,i2c_dev0;
/* data buffer */
char i2c_data_buf[I2C_BUF_SIZE] = {0};
char i2c_data_buf1[I2C_BUF_SIZE] = {0};

void hal_iic_mem(uint16_t size,uint32_t tx_timeout,uint32_t rx_timeout,uint32_t freq)
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* i2c port set */
    i2c0.port = I2C0_PORT_NUM;

    /* i2c attr config */
    i2c0.config.dev_addr = CK_IIC_SLAVE_ADDR;
    i2c0.config.freq = freq;
    i2c0.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c0.config.mode = I2C_MODE_MASTER;


    /* init i2c with the given settings */
    ret = hal_i2c_init(&i2c0);
    if (ret != 0) {
        printf("i2c0 init error !\n");
        hal_i2c_finalize(&i2c0);
        AT_BACK_ERR();
        return;
    }

    /* init the tx buffer */
    for (int i = 0; i < size; i++) {
        i2c_data_buf[i] = i;
    }

    ret = hal_i2c_mem_write(&i2c0, CK_IIC_SLAVE_ADDR, EEPROM_PAGE_ADDR, I2C_MEM_ADDR_SIZE_16BIT, i2c_data_buf, size, tx_timeout);
    if (ret != 0) {
        printf("i2c0 write error !\n");
        hal_i2c_finalize(&i2c0);
        AT_BACK_ERR();
        return;
    }
    aos_msleep(5000);

    ret = hal_i2c_mem_read(&i2c0, CK_IIC_SLAVE_ADDR, EEPROM_PAGE_ADDR, I2C_MEM_ADDR_SIZE_16BIT, i2c_data_buf1, size, rx_timeout);
    if (ret == 0) {
        printf("i2c0 mem read succeed !\n");
    } else {
        printf("i2c0 mem read error !\n");
        hal_i2c_finalize(&i2c0);
        AT_BACK_ERR();
        return;
    }
    for (i = 0; i < size; i++) {
        printf("%x,%x \t", i2c_data_buf[i], i2c_data_buf1[i]);

        if (((i + 3) % 4) == 0) {
            printf("\n");
        }
        if (i2c_data_buf[i]!=i2c_data_buf1[i]) {
            hal_i2c_finalize(&i2c0);
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


void hal_iic_api(void)
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* i2c port set */
    i2c0.port = I2C0_PORT_NUM;

    /* i2c attr config */
    i2c0.config.mode          = I2C_MODE_MASTER;
    i2c0.config.freq          = 100000;
    i2c0.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c0.config.dev_addr      = I2C_DEV_ADDR;

    /* init i2c with the given settings */
    ret = hal_i2c_init(NULL);
    if (ret == 0) {
        AT_BACK_ERR();
        return;
    }

    ret = hal_i2c_finalize(NULL);
    if (ret == 0) {
        AT_BACK_ERR();
        return;
    }

    //Except!
    /*ret = hal_i2c_slave_recv(NULL, i2c_data_buf1,
                       I2C_BUF_SIZE, I2C_RX_TIMEOUT);
    if (ret == 0) {
        AT_BACK_ERR();
    }

    ret = hal_i2c_master_send(NULL, I2C_DEV_ADDR, i2c_data_buf,
                              I2C_BUF_SIZE, I2C_TX_TIMEOUT);

    if (ret == 0) {
        AT_BACK_ERR();
    }

    ret = hal_i2c_mem_write(NULL, I2C_DEV_ADDR, 0x00, I2C_MEM_ADDR_SIZE_16BIT, i2c_data_buf, I2C_BUF_SIZE, 3000);
    if (ret == 0) {
        AT_BACK_ERR();
    }

    hal_i2c_mem_read(NULL, I2C_DEV_ADDR, 0x00, I2C_MEM_ADDR_SIZE_16BIT, i2c_data_buf1, I2C_BUF_SIZE, 1000);
    if (ret == 0) {
        AT_BACK_ERR();
    } */
    AT_BACK_OK(); 
}

void hal_iic_master_send(uint32_t freq,uint32_t width,uint16_t size,uint32_t tx_timeout)
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* i2c port set */
    i2c0.port = I2C0_PORT_NUM;

    /* i2c attr config */
    i2c0.config.mode          = I2C_MODE_MASTER;
    i2c0.config.freq          = freq;
    i2c0.config.address_width = width;
    i2c0.config.dev_addr      = I2C_DEV_ADDR;

    /* init i2c with the given settings */
    ret = hal_i2c_init(&i2c0);
    if (ret != 0) {
        printf("i2c0 init error !\n");
        AT_BACK_ERR();
        return;
    }
    else {
        printf("i2c0 init succeed !\n");
    }

    /* init the tx buffer */
    for (int i = 0; i < size; i++) {
        i2c_data_buf[i] = i+1;
    }

    /* send 0,1,2,3,4,5,6,7,8,9 by i2c */
    ret = hal_i2c_master_send(&i2c0, I2C_DEV_ADDR, i2c_data_buf,
                              size, tx_timeout);
    for (int i = 0; i < size; i++)
    {
        printf("master send is %d\n",i2c_data_buf[i]);
    }
    if (ret == 0) {
        printf("master data send succeed !\n");
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

void hal_iic_master_recv(uint32_t freq,uint32_t width,uint16_t size,uint32_t rx_timeout)
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* i2c port set */
    i2c0.port = I2C0_PORT_NUM;

    /* i2c attr config */
    i2c0.config.mode          = I2C_MODE_MASTER;
    i2c0.config.freq          = freq;
    i2c0.config.address_width = width;
    i2c0.config.dev_addr      = I2C_DEV_ADDR;

    /* init i2c with the given settings */
    ret = hal_i2c_init(&i2c0);
    if (ret != 0) {
        printf("i2c0 init error !\n");
        AT_BACK_ERR();
        return;
    }
    else {
        printf("i2c0 init succeed !\n");
    }

    memset(i2c_data_buf1, 0, I2C_BUF_SIZE);

    ret = hal_i2c_master_recv(&i2c0, I2C_DEV_ADDR, i2c_data_buf1,
                              size, rx_timeout);

    if (ret == 0) {
        printf("master data received succeed !\n");
    } else {
        AT_BACK_ERR();
        return;
    }

    for (i = 0; i < size; i++) {
        printf("master recv is %d\n",i2c_data_buf1[i]);
        if (i2c_data_buf1[i] != i) {
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

void test_hal_iic_master(char *cmd, int type, char *data)
{
    csi_pin_set_mux(PIN_IIC0_SDA, PIN_IIC0_SDA_FUNC);
    csi_pin_set_mux(PIN_IIC0_SCL, PIN_IIC0_SCL_FUNC);
    
    if (strcmp((const char *)data, "'freq=100K'\0") == 0) {
        hal_iic_master_send(100000,I2C_HAL_ADDRESS_WIDTH_10BIT,16,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'freq=400K'\0") == 0) {
        hal_iic_master_send(400000,I2C_HAL_ADDRESS_WIDTH_7BIT,16,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'freq=3.4M'\0") == 0) {
        hal_iic_master_send(3400000,I2C_HAL_ADDRESS_WIDTH_10BIT,16,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'width=7bit'\0") == 0) {
        hal_iic_master_send(100000,I2C_HAL_ADDRESS_WIDTH_7BIT,16,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'width=10bit'\0") == 0) {
        hal_iic_master_send(100000,I2C_HAL_ADDRESS_WIDTH_10BIT,16,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'size=1'\0") == 0) {
        hal_iic_master_send(100000,I2C_HAL_ADDRESS_WIDTH_10BIT,1,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'size=32'\0") == 0) {
        hal_iic_master_send(100000,I2C_HAL_ADDRESS_WIDTH_10BIT,32,I2C_TX_TIMEOUT);
    } else if (strcmp((const char *)data, "'recv_size=1'\0") == 0) {
        hal_iic_master_recv(100000,I2C_HAL_ADDRESS_WIDTH_7BIT,1,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'recv'\0") == 0) {
        hal_iic_master_recv(100000,I2C_HAL_ADDRESS_WIDTH_7BIT,16,I2C_RX_TIMEOUT);
    } else if (strcmp((const char *)data, "'API'\0") == 0) {
        hal_iic_api();
    } else if (strcmp((const char *)data, "'MEM_SIZE=16'\0") == 0) {
        hal_iic_mem(16,I2C_TX_TIMEOUT,I2C_RX_TIMEOUT,100000);
    } else if (strcmp((const char *)data, "'MEM_SIZE=1'\0") == 0) {
        hal_iic_mem(1,I2C_TX_TIMEOUT,I2C_RX_TIMEOUT,100000);
    } else if (strcmp((const char *)data, "'MEM_freq=400K'\0") == 0) {
        hal_iic_mem(16,I2C_TX_TIMEOUT,I2C_RX_TIMEOUT,400000);
    } else if (strcmp((const char *)data, "'MEM_freq=3.4M'\0") == 0) {
        hal_iic_mem(16,I2C_TX_TIMEOUT,I2C_RX_TIMEOUT,3400000);
    } else if (strcmp((const char *)data, "'MEM_TIMEOUT_FOREVER'\0") == 0) {
        hal_iic_mem(16,I2C_TX_TIMEOUT_2,I2C_RX_TIMEOUT_2,100000);
    } else if (strcmp((const char *)data, "'TIMEOUT_FOREVER'\0") == 0) {
        hal_iic_master_send(100000,I2C_HAL_ADDRESS_WIDTH_7BIT,16,I2C_TX_TIMEOUT_2);
    }
}