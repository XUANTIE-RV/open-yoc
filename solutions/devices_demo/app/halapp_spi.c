/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "soc.h"
#include "app_main.h"

#ifndef CONFIG_HAL_SPI_DISABLED
#include <devices/device.h>
#include <devices/driver.h>
#include <devices/spi.h>
#include <devices/devicelist.h>

#define TAG "HALAPP_SPI"

#define SPI0_PORT_NUM       0
#define SPI1_PORT_NUM       1
#define SPI_DATA_NUM        10
#define SPI_TX_TIMEOUT      20000
#define SPI_RX_TIMEOUT      20000

extern const csi_pinmap_t spi_pinmap[];

static void dump_data(uint8_t *data, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\n");
}

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
    printf("hal_spi_demo start\n");

    /* define dev */
    rvm_dev_t *spi0_dev = NULL;
    rvm_dev_t *spi1_dev = NULL;
    rvm_dev_t *spi2_dev = NULL;

    uint8_t spi0_idx;
    uint8_t spi1_idx;
    uint8_t spi2_idx;

    rvm_hal_spi_config_t spi0_config;
    rvm_hal_spi_config_t spi1_config;
    rvm_hal_spi_config_t spi2_config;

    char spi0_filename[8];
    char spi1_filename[8];
    char spi2_filename[8];

    /* data buffer */
    uint8_t  spi_tx_data[64]  __attribute__((aligned(32)));
    uint8_t  spi_rx_data[64]  __attribute__((aligned(32)));

    int ret = -1;
    bool check_recv = true;

    rvm_hal_spi_default_config_get(&spi0_config);
    rvm_hal_spi_default_config_get(&spi1_config);
    rvm_hal_spi_default_config_get(&spi2_config);

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
        spi0_idx = get_spi_idx(gpio_pin[0], spi_pinmap);
        spi0_config.mode  = RVM_HAL_SPI_MODE_MASTER;
        spi0_config.freq = 500 * 1000; // 500KHz

        spi1_idx = get_spi_idx(gpio_pin[4], spi_pinmap);
        spi1_config.mode  = RVM_HAL_SPI_MODE_SLAVE;

        rvm_spi_drv_register(spi0_idx);

        sprintf(spi0_filename, "spi%d", spi0_idx);
        spi0_dev = rvm_hal_spi_open(spi0_filename);

        rvm_hal_spi_config(spi0_dev, &spi0_config);

        rvm_spi_drv_register(spi1_idx);

        sprintf(spi1_filename, "spi%d", spi1_idx);
        spi1_dev = rvm_hal_spi_open(spi1_filename);

        rvm_hal_spi_config(spi1_dev, &spi1_config);
    } else { // 设备仅一个spi时
        spi2_idx = get_spi_idx(gpio_pin[0], spi_pinmap);
        if (function >= APP_TEST_SPI_MASTER_SEND) {
            spi2_config.mode = RVM_HAL_SPI_MODE_MASTER;
        } else {
            spi2_config.mode = RVM_HAL_SPI_MODE_SLAVE;
        }
        spi2_config.freq = 500 * 1000; // 500KHz

        rvm_spi_drv_register(spi2_idx);

        sprintf(spi2_filename, "spi%d", spi2_idx);
        spi2_dev = rvm_hal_spi_open(spi2_filename);

        rvm_hal_spi_config(spi2_dev, &spi2_config);
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
        ret = rvm_hal_spi_recv(spi1_dev, spi_rx_data, SPI_DATA_NUM, AOS_NO_WAIT);

        if (rvm_hal_spi_send(spi0_dev, spi_tx_data, SPI_DATA_NUM, SPI_TX_TIMEOUT)) {
            LOGE(TAG, "send error !");
            goto fail;
        }
        break;
    case APP_TEST_SPI_SLAVE_SEND:
    case APP_TEST_SPI_MASTER_SEND:
        check_recv = false;
        if (rvm_hal_spi_send(spi2_dev, spi_tx_data, SPI_DATA_NUM, SPI_TX_TIMEOUT)) {
            LOGE(TAG, "send error !");
            goto fail;
        }
        break;
    case APP_TEST_SPI_SLAVE_RECV:
    case APP_TEST_SPI_MASTER_RECV:
        if (rvm_hal_spi_recv(spi2_dev, spi_rx_data, SPI_DATA_NUM, SPI_RX_TIMEOUT)) {
            LOGE(TAG, "recv error !");
            goto fail;
        }
        break;
    case APP_TEST_SPI_SLAVE_SEND_RECV:
    case APP_TEST_SPI_MASTER_SEND_RECV:
        if (rvm_hal_spi_send_recv(spi2_dev, spi_tx_data, spi_rx_data, SPI_DATA_NUM, SPI_TX_TIMEOUT)) {
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
            printf("spi_rx_data:");
            dump_data(spi_rx_data,SPI_DATA_NUM);
            LOGE(TAG, "transfer error !");
            goto fail;
        } else {
            LOGD(TAG, "transfer success !");
        }
    } else {
        LOGD(TAG, "End send, please check recv !");
    }

    printf("spi demo successfully !\n");

    rvm_hal_spi_close(spi0_dev);
    rvm_hal_spi_close(spi1_dev);
    rvm_hal_spi_close(spi2_dev);

    return 0;

fail:
    rvm_hal_spi_close(spi0_dev);
    rvm_hal_spi_close(spi1_dev);
    rvm_hal_spi_close(spi2_dev);

    return -1;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int devfs_spi_demo(APP_TEST_SPI_FUNCTION function, uint32_t *gpio_pin)
{
    printf("devfs_spi_demo start\n");

    /* define dev */
    int fd_spi0 = 0;
    int fd_spi1 = 0;
    int fd_spi2 = 0;

    uint8_t spi0_idx;
    uint8_t spi1_idx;
    uint8_t spi2_idx;

    rvm_hal_spi_config_t spi0_config;
    rvm_hal_spi_config_t spi1_config;
    rvm_hal_spi_config_t spi2_config;

    char spi0_filename[16];
    char spi1_filename[16];
    char spi2_filename[16];

    /* data buffer */
    uint8_t  spi_tx_data[64]  __attribute__((aligned(32)));
    uint8_t  spi_rx_data[64]  __attribute__((aligned(32)));

    int ret = -1;
    bool check_recv = true;
    uint32_t timeout = 20000;

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
        spi0_idx = get_spi_idx(gpio_pin[0], spi_pinmap);
        spi1_idx = get_spi_idx(gpio_pin[4], spi_pinmap);

        rvm_spi_drv_register(spi0_idx);

        sprintf(spi0_filename, "/dev/spi%d", spi0_idx);
        fd_spi0 = open(spi0_filename, O_RDWR);
        printf("open %s fd:%d\n", spi0_filename, fd_spi0);
        if (fd_spi0 < 0) {
            printf("open %s failed. fd:%d\n", spi0_filename, fd_spi0);
            return -1;
        }

       ret = ioctl(fd_spi0, SPI_IOC_TIMEOUT, &timeout);
       if (ret) {
            printf("SPI_IOC_TIMEOUT is error\n");
       }

        ioctl(fd_spi0, SPI_IOC_GET_DEFAULT_CONFIG, &spi0_config);
        spi0_config.mode  = RVM_HAL_SPI_MODE_MASTER;
        spi0_config.freq = 500 * 1000; // 500KHz
        ret = ioctl(fd_spi0, SPI_IOC_CONFIG, &spi0_config);
        if (ret < 0) {
            printf("SPI_IOC_CONFIG fail !\n");
            goto failure;
        }

        rvm_spi_drv_register(spi1_idx);

        sprintf(spi1_filename, "/dev/spi%d", spi1_idx);
        fd_spi1 = open(spi1_filename, O_RDWR | O_NONBLOCK);
        printf("open %s fd:%d\n", spi1_filename, fd_spi1);
        if (fd_spi1 < 0) {
            printf("open %s failed. fd:%d\n", spi1_filename, fd_spi1);
            return -1;
        }

       ret = ioctl(fd_spi1, SPI_IOC_TIMEOUT, &timeout);
       if (ret) {
            printf("SPI_IOC_TIMEOUT is error\n");
       }
        ioctl(fd_spi1, SPI_IOC_GET_DEFAULT_CONFIG, &spi1_config);
        spi1_config.mode  = RVM_HAL_SPI_MODE_SLAVE;
        ret = ioctl(fd_spi1, SPI_IOC_CONFIG, &spi1_config);
        if (ret < 0) {
            printf("SPI_IOC_CONFIG fail !\n");
            goto failure;
        }
    } else { // 设备仅一个spi时
        spi2_idx = get_spi_idx(gpio_pin[0], spi_pinmap);
        rvm_spi_drv_register(spi2_idx);

        sprintf(spi2_filename, "/dev/spi%d", spi2_idx);
        fd_spi2 = open(spi2_filename, O_RDWR);
        printf("open %s fd:%d\n", spi2_filename, fd_spi2);
        if (fd_spi2 < 0) {
            printf("open %s failed. fd:%d\n", spi2_filename, fd_spi2);
            return -1;
        }
        ret = ioctl(fd_spi2, SPI_IOC_TIMEOUT, &timeout);
        if (ret) {
            printf("SPI_IOC_TIMEOUT is error\n");
        }
        ioctl(fd_spi2, SPI_IOC_GET_DEFAULT_CONFIG, &spi2_config);
        if (function >= APP_TEST_SPI_MASTER_SEND) {
            spi2_config.mode = RVM_HAL_SPI_MODE_MASTER;
        } else {
            spi2_config.mode = RVM_HAL_SPI_MODE_SLAVE;
        }
        spi2_config.freq = 500 * 1000; // 500KHz
        ret = ioctl(fd_spi2, SPI_IOC_CONFIG, &spi2_config);
        if (ret < 0) {
            printf("SPI_IOC_CONFIG fail !\n");
            goto failure;
        }
    }

    /* 初始化收发数据*/
    for (ret = 0; ret < SPI_DATA_NUM; ret++) {
        spi_tx_data[ret] = ret + 1;
        spi_rx_data[ret] = 0xff;
    }

    rvm_spi_dev_msg_t msg_spi;

    /* 进行spi通信 */
    if (function >= APP_TEST_SPI_MASTER_SEND) {
        aos_msleep(1000);
    }
    switch (function)
    {
    case APP_TEST_SPI_SLAVE_RECV_MASTER_SEND: // 注意：此 case 不支持 SYNC 同步模式
        ret = read(fd_spi1, spi_rx_data, SPI_DATA_NUM);

        ret = write(fd_spi0, spi_tx_data, SPI_DATA_NUM);
        if (ret < 0) {
            printf("fd:%d write fail !\n", fd_spi0);
            goto failure;
        }
        break;
    case APP_TEST_SPI_SLAVE_SEND:
    case APP_TEST_SPI_MASTER_SEND:
        check_recv = false;
        ret = write(fd_spi2, spi_tx_data, SPI_DATA_NUM);
        if (ret < 0) {
            printf("fd:%d write fail !\n", fd_spi2);
            goto failure;
        }
        break;
    case APP_TEST_SPI_SLAVE_RECV:
    case APP_TEST_SPI_MASTER_RECV:
        ret = read(fd_spi2, spi_rx_data, SPI_DATA_NUM);
        if (ret < 0) {
            printf("fd:%d write fail !!!\n", fd_spi2);
            goto failure;
        }
        break;
    case APP_TEST_SPI_SLAVE_SEND_RECV:
    case APP_TEST_SPI_MASTER_SEND_RECV:
        msg_spi.tx_data = spi_tx_data;
        msg_spi.rx_data = spi_rx_data;
        msg_spi.size = SPI_DATA_NUM;
        msg_spi.timeout = SPI_TX_TIMEOUT;
        ret = ioctl(fd_spi2, SPI_IOC_SEND_RECV, &msg_spi);
        if (ret < 0) {
            printf("SPI_IOC_SEND_RECV fail !\n");
            goto failure;
        }
        break;
    default:
        LOGE(TAG, "error, please check params !");
        goto failure;
        break;
    }

    if (check_recv) {
        if (memcmp(spi_tx_data, spi_rx_data, SPI_DATA_NUM)) {
            LOGE(TAG, "transfer error !");
            printf("spi_rx_data:");
            dump_data(spi_rx_data,SPI_DATA_NUM);
            goto failure;
        } else {
            LOGD(TAG, "transfer success !");
        }
    } else {
        LOGD(TAG, "End send, please check recv !");
    }

    printf("devfs spi demo successfully !\n");

    /* close spi */
    close(fd_spi0);
    close(fd_spi1);
    close(fd_spi2);

    return 0;

failure:
    close(fd_spi0);
    close(fd_spi1);
    close(fd_spi2);
    return -1;
}

#endif

#else

int hal_spi_demo(APP_TEST_SPI_FUNCTION function, uint32_t *gpio_pin)
{
    printf("hal_spi_demo not support.\n");
    return -1;
}

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
int devfs_spi_demo(APP_TEST_SPI_FUNCTION function, uint32_t *gpio_pin)
{
    printf("devfs_spi_demo not support.\n");
    return -1;
}
#endif

#endif /*CONFIG_HAL_SPI_DISABLED*/