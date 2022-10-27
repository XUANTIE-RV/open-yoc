/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#if (!CONFIG_CHIP_BL606P_E907)

#include <stdio.h>
#include "soc.h"
#include <drv/pin.h>
#include "aos/hal/i2c.h"

#define I2C_BUF_SIZE                10
#define I2C_TX_TIMEOUT              10000
#define I2C_RX_TIMEOUT              10000
#define I2C_MASTER_DEV_ADDR         0x40
#define I2C_SLAVE_DEV_ADDR          0x50

/* define dev */
static i2c_dev_t i2c0;

extern const csi_pinmap_t iic_pinmap[];

static uint32_t get_iic_pin_func(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFF;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin) {
            ret = (uint32_t)map->pin_func;
            break;
        }

        map++;
    }

    return ret;
}

static uint8_t get_iic_idx(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFFU;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin) {
            ret = (uint32_t)map->idx;
            break;
        }

        map++;
    }

    return ret;
}

int hal_iic_master_demo(uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    static char i2c_data_send[I2C_BUF_SIZE];
    int ret     = -1;
    int i       = 0;

    printf("board0: iic master receive data\n");

    uint32_t pin0_func = get_iic_pin_func(gpio_pin0, iic_pinmap);
    uint32_t pin1_func = get_iic_pin_func(gpio_pin1, iic_pinmap);
    uint8_t iic_idx = get_iic_idx(gpio_pin0, iic_pinmap);

    csi_pin_set_mux(gpio_pin0, pin0_func); //ch2601: PA2 PA3这一组不能用，无时序输出
    csi_pin_set_mux(gpio_pin1, pin1_func);

    /* i2c port set */
    i2c0.port = iic_idx;

    /* i2c attr config */
    i2c0.config.mode          = I2C_MODE_MASTER;
    i2c0.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c0.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c0.config.dev_addr      = I2C_MASTER_DEV_ADDR;

    /* init i2c0 with the given settings */
    ret = hal_i2c_init(&i2c0);
    if (ret != 0) {
        printf("i2c0 init error !\n");
        return -1;
    }

    /* init the tx buffer */
    for (i = 0; i < I2C_BUF_SIZE; i++) {
        i2c_data_send[i] = i + 1;
    }

    /* send data by the i2c0 of board0 */
    ret = hal_i2c_master_send(&i2c0, I2C_SLAVE_DEV_ADDR, (uint8_t *)i2c_data_send,
                              I2C_BUF_SIZE, I2C_TX_TIMEOUT);
    if (ret == 0) {
        printf("i2c0 data send succeed !\n");
    } else {
        return -1;
    }

    for (i = 0; i < I2C_BUF_SIZE; i++) {
        printf("i2c_data_send[i]: %x\n", i2c_data_send[i]);
    }

    hal_i2c_finalize(&i2c0);

    printf("iic master demo successfuly\n");

    return 0;
}

int hal_iic_slave_demo(uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    static char i2c_data_recv[I2C_BUF_SIZE];
    int ret     = -1;
    int i       = 0;

    printf("board1: iic slave receive data\n");

    uint32_t pin0_func = get_iic_pin_func(gpio_pin0, iic_pinmap);
    uint32_t pin1_func = get_iic_pin_func(gpio_pin1, iic_pinmap);
    uint8_t iic_idx = get_iic_idx(gpio_pin0, iic_pinmap);

    csi_pin_set_mux(gpio_pin0, pin0_func); //ch2601: PA2 PA3这一组不能用，无时序输出
    csi_pin_set_mux(gpio_pin1, pin1_func);

    /* i2c port set */
    i2c0.port = iic_idx;

    /* i2c attr config */
    i2c0.config.mode          = I2C_MODE_SLAVE;
    i2c0.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c0.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c0.config.dev_addr      = I2C_SLAVE_DEV_ADDR;

    /* init i2c0 with the given settings */
    ret = hal_i2c_init(&i2c0);
    if (ret != 0) {
        printf("i2c0 init error !\n");
        return -1;
    }

    /* init the rx buffer */
    for (i = 0; i < I2C_BUF_SIZE; i++) {
        i2c_data_recv[i] = 0;
    }

    /* recv data by the i2c0 of board1 */
    ret = hal_i2c_slave_recv(&i2c0, (uint8_t *)i2c_data_recv, I2C_BUF_SIZE, I2C_RX_TIMEOUT);
    if (ret == 0) {
        printf("i2c0 data received succeed !\n");
    } else {
        return -1;
    }

    for (i = 0; i < I2C_BUF_SIZE; i++) {
        printf("i2c_data_recv[i]: %x\n", i2c_data_recv[i]);
    }

    hal_i2c_finalize(&i2c0);

    printf("iic slave demo successfuly\n");

    return 0;
}

#endif
