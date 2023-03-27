/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <drv/pin.h>
#include <aos/aos.h>
#include <aos/hal/spi.h>
#include "soc.h"
#include "app_main.h"

#if defined(CONFIG_CHIP_BL606P_E907)
#include <bl606p_gpio.h>
#include <bl606p_glb.h>
#include <bl606p_spi.h>
#endif

#define TAG "HALAPP_SPI"

#define SPI0_PORT_NUM       0
#define SPI1_PORT_NUM       1
#define SPI_DATA_NUM        10
#define SPI_TX_TIMEOUT      2000
#define SPI_RX_TIMEOUT      2000

extern const csi_pinmap_t spi_pinmap[];

static uint32_t get_spi_pin_func(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
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

static uint8_t get_spi_idx(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
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

int hal_spi_demo(APP_TEST_SPI_FUNCTION function, uint32_t *gpio_pin)
{

    /* define dev */
    static spi_dev_t spi_master;
    static spi_dev_t spi_slave;
    static spi_dev_t spi_dev;
    /* data buffer */
    uint8_t  spi_tx_data[64]  __attribute__((aligned(32)));
    uint8_t  spi_rx_data[64]  __attribute__((aligned(32)));

    int ret = -1;
    bool check_recv = true;

    /* 配置管脚复用 */
#if !defined(CONFIG_CHIP_BL606P_E907)
    if (function == APP_TEST_SPI_SLAVE_RECV_MASTER_SEND) { // 需复用8个pin
        uint32_t pin_func[8] = {0};
        for (ret = 0; ret < 8; ret++) {
            pin_func[ret] = get_spi_pin_func(gpio_pin[ret], spi_pinmap);
        }
        for (ret = 0; ret < 8; ret++) {
            csi_pin_set_mux(gpio_pin[ret], pin_func[ret]);
        }
    } else { // 需复用4个pin
        uint32_t pin_func[4] = {0};
        for (ret = 0; ret < 4; ret++) {
            pin_func[ret] = get_spi_pin_func(gpio_pin[ret], spi_pinmap);
        }
        for (ret = 0; ret < 4; ret++) {
            csi_pin_set_mux(gpio_pin[ret], pin_func[ret]);
        }
    }
#else
    // 待BL完善 csi_pin_set_mux 接口后删除此 GLB_GPIO_Init 代码片段
    GLB_GPIO_Cfg_Type gpio_cfg = {
        .drive = 2,
        .smtCtrl = 1,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .gpioFun = GPIO_FUN_SPI0,
    };

    GLB_GPIO_Type spi_gpio[] = {gpio_pin[0], gpio_pin[1], gpio_pin[2], gpio_pin[3]};

    for (ret = 0; ret < sizeof(spi_gpio) / sizeof(spi_gpio[0]); ret++) {
        gpio_cfg.gpioPin = spi_gpio[ret];
        GLB_GPIO_Init(&gpio_cfg);
    }
#endif

    /* 配置spi参数与功能初始化*/
    if (function == APP_TEST_SPI_SLAVE_RECV_MASTER_SEND) { // 设备有两个spi时
        spi_master.port = get_spi_idx(gpio_pin[0], spi_pinmap);
        spi_master.config.mode  = HAL_SPI_MODE_MASTER;
        spi_master.config.freq = 500 * 1000; // 500KHz

        spi_slave.port = get_spi_idx(gpio_pin[4], spi_pinmap);
        spi_slave.config.mode  = HAL_SPI_MODE_SLAVE;

        ret = hal_spi_init(&spi_master);
        if (ret != 0) {
            LOGE(TAG, "spi_master init error !");
            return -1;
        }

        ret = hal_spi_init(&spi_slave);
        if (ret != 0) {
            LOGE(TAG, "spi_slave init error !");
            return -1;
        }
    } else { // 设备仅一个spi时
        spi_dev.port = get_spi_idx(gpio_pin[0], spi_pinmap);
        if (function >= APP_TEST_SPI_MASTER_SEND) {
            spi_dev.config.mode = HAL_SPI_MODE_MASTER;
        } else {
            spi_dev.config.mode = HAL_SPI_MODE_SLAVE;
        }
        spi_dev.config.freq = 500 * 1000; // 500KHz

        ret = hal_spi_init(&spi_dev);
        if (ret != 0) {
            LOGE(TAG, "spi_dev init error !");
            return -1;
        }
    }

    /* 初始化收发数据*/
    for (ret = 0; ret < SPI_DATA_NUM; ret++) {
        spi_tx_data[ret] = ret + 1;
        spi_rx_data[ret] = 0xff;
    }

    /* 进行spi通信 */
    switch (function)
    {
    case APP_TEST_SPI_SLAVE_RECV_MASTER_SEND: // 注意：此 case 不支持 SYNC 同步模式
        ret = hal_spi_recv(&spi_slave, spi_rx_data, SPI_DATA_NUM, SPI_RX_TIMEOUT);
        if (hal_spi_send(&spi_master, spi_tx_data, SPI_DATA_NUM, SPI_TX_TIMEOUT)) {
            LOGE(TAG, "send error !");
            goto fail;
        }
        break;
    case APP_TEST_SPI_SLAVE_SEND:
    case APP_TEST_SPI_MASTER_SEND:
        check_recv = false;
        if (hal_spi_send(&spi_dev, spi_tx_data, SPI_DATA_NUM, SPI_TX_TIMEOUT)) {
            LOGE(TAG, "send error !");
            goto fail;
        }
        break;
    case APP_TEST_SPI_SLAVE_RECV:
    case APP_TEST_SPI_MASTER_RECV:
        if (hal_spi_recv(&spi_dev, spi_rx_data, SPI_DATA_NUM, SPI_RX_TIMEOUT)) {
            LOGE(TAG, "recv error !");
            goto fail;
        }
        break;
    case APP_TEST_SPI_SLAVE_SEND_RECV:
    case APP_TEST_SPI_MASTER_SEND_RECV:
        if (hal_spi_send_recv(&spi_dev, spi_tx_data, spi_rx_data, SPI_DATA_NUM, SPI_TX_TIMEOUT)) {
            LOGE(TAG, "send_recv error !");
            goto fail;
        }
        break;
    default:
        LOGE(TAG, "error, please check params !");
        goto fail;
        break;
    }

    if (check_recv) {
        if (memcmp(spi_tx_data, spi_rx_data, SPI_DATA_NUM)) {
            LOGE(TAG, "transfer error !");
        } else {
            LOGD(TAG, "transfer success !");
        }
    } else {
        LOGD(TAG, "End send, please check recv !");
    }

fail:
    /* spi设备反初始化 */
    hal_spi_finalize(&spi_dev);

    return 0;
}