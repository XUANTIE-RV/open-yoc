/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "soc.h"
#include <drv/pin.h>
#include <aos/aos.h>
#include <drv/common.h>
#include "app_main.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/iic.h>
#include <devices/devicelist.h>

#if defined(CONFIG_CHIP_BL606P_E907)
#include <bl606p_gpio.h>
#include <bl606p_glb.h>
#endif

#define TAG "HALAPP_IIC"

#define I2C_BUF_SIZE                10
#define I2C_TX_TIMEOUT              10000
#define I2C_RX_TIMEOUT              10000
#define I2C_TASK_SLEEP_TIME         10000
#define I2C_SLAVE_DEV_ADDR          0x50
#define I2C_MEM_DEV_ADDR            0x20

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

int hal_iic_demo(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    // i2c_dev_t i2c;
    char i2c_data_recv[I2C_BUF_SIZE], i2c_data_send[I2C_BUF_SIZE];
    bool master_check_recv = true;
    int ret = -1;
    uint8_t iic_idx = 0xff;

    /* 管脚复用 */
#if !defined(CONFIG_CHIP_BL606P_E907)
    uint32_t pin0_func = get_iic_pin_func(gpio_pin0, iic_pinmap);
    uint32_t pin1_func = get_iic_pin_func(gpio_pin1, iic_pinmap);

    csi_pin_set_mux(gpio_pin0, pin0_func); // ch2601: PA2 PA3这一组不能用，无时序输出
    csi_pin_set_mux(gpio_pin1, pin1_func);
#else
    // 待BL完善 csi_pin_set_mux 接口后删除此 GLB_GPIO_Init 代码片段
    GLB_GPIO_Cfg_Type gpio_cfg;
    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioFun = GPIO_FUN_I2C0;

    gpio_cfg.gpioPin = gpio_pin0;
    GLB_GPIO_Init(&gpio_cfg);

    gpio_cfg.gpioPin = gpio_pin1;
    GLB_GPIO_Init(&gpio_cfg);
#endif
    /* 配置设备参数 */
    iic_idx = get_iic_idx(gpio_pin0, iic_pinmap);

    rvm_iic_drv_register(iic_idx);

    rvm_dev_t *iic_dev = NULL;
    char filename[8] = {0};
    sprintf(filename, "iic%d", iic_idx);

    iic_dev = rvm_hal_iic_open(filename);

    rvm_hal_iic_config_t config = {0};
    if (function >= APP_TEST_IIC_MASTER_SEND_RECV) {
        config.mode          = MODE_MASTER;
    } else {
        config.mode          = MODE_SLAVE;
        config.slave_addr      = I2C_SLAVE_DEV_ADDR;
    }
    config.speed          = BUS_SPEED_STANDARD;
    config.addr_mode = ADDR_7BIT;

    /* 初始化收发数据*/
    for (ret = 0; ret < I2C_BUF_SIZE; ret++) {
        i2c_data_send[ret] = ret + 1;
        i2c_data_recv[ret] = 0xff;
    }

    ret = rvm_hal_iic_config(iic_dev, &config);
    if (ret != 0) {
        LOGE(TAG, "rvm_hal_iic_config fail !");
        return -1;
    }


    /* 这里的逻辑是：
        主机：先发出一段数据，再进行接收
        从机：先接收一段数据，再发出收到的数据
        最后主设备将收到的数据和发出的数据进行对比校验
    */
    switch (function)
    {
    case APP_TEST_IIC_MASTER_SEND_RECV:
        if (rvm_hal_iic_master_send(iic_dev,  I2C_SLAVE_DEV_ADDR, (uint8_t *)i2c_data_send, I2C_BUF_SIZE, I2C_TX_TIMEOUT)) {
            LOGE(TAG, "master send error !");
            goto fail;
        }
        aos_msleep(200);
        if (rvm_hal_iic_master_recv(iic_dev, I2C_SLAVE_DEV_ADDR, (uint8_t *)i2c_data_recv, I2C_BUF_SIZE, I2C_RX_TIMEOUT)) {
            LOGE(TAG, "master recv error !");
            goto fail;
        }
        break;
    case APP_TEST_IIC_SLAVE_RECV_SEND:
        master_check_recv = false;
        if (rvm_hal_iic_slave_recv(iic_dev, (uint8_t *)i2c_data_recv, I2C_BUF_SIZE, I2C_RX_TIMEOUT)) {
            LOGE(TAG, "slave recv error !");
            goto fail;
        }
        aos_msleep(200);
        if (rvm_hal_iic_slave_send(iic_dev, (uint8_t *)i2c_data_recv, I2C_BUF_SIZE, I2C_TX_TIMEOUT)) {
            LOGE(TAG, "slave send error !");
            goto fail;
        }
        break;
    case APP_TEST_IIC_MEM_WRITE_READ:
        if (rvm_hal_iic_mem_write(iic_dev, I2C_SLAVE_DEV_ADDR, I2C_MEM_DEV_ADDR, I2C_MEM_ADDR_SIZE_16BIT, (uint8_t *)i2c_data_send, I2C_BUF_SIZE, I2C_TX_TIMEOUT)) {
            LOGE(TAG, "mem write error !");
            goto fail;
        }
        aos_msleep(200);
        if (rvm_hal_iic_mem_read(iic_dev, I2C_SLAVE_DEV_ADDR, I2C_MEM_DEV_ADDR, I2C_MEM_ADDR_SIZE_16BIT, (uint8_t *)i2c_data_recv, I2C_BUF_SIZE, I2C_RX_TIMEOUT)) {
            LOGE(TAG, "mem read error !");
            goto fail;
        }
        break;
    default:
        break;
    }

    if (master_check_recv) {
        if (memcmp(i2c_data_send, i2c_data_recv, I2C_BUF_SIZE) != 0) {
            LOGE(TAG, "transfer error !");
        } else {
            LOGD(TAG, "transfer success !");
        }
    } else {
        LOGI(TAG, "go to master check result !");
    }

fail:
    /* 关闭IIC设备 */
    rvm_hal_iic_close(iic_dev);

    return 0;
}
