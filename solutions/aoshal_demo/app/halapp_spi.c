/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#if (!CONFIG_CHIP_BL606P_E907)

#include <stdio.h>
#include <drv/pin.h>
#include <aos/aos.h>
#include <aos/hal/spi.h>
#include "soc.h"

#define SPI0_PORT_NUM       0
#define SPI1_PORT_NUM       1
#define SPI_DATA_NUM        10
#define SPI_TX_TIMEOUT      10
#define SPI_RX_TIMEOUT      10

/* define dev */
static spi_dev_t spi_master;
static spi_dev_t spi_slave;

/* data buffer */
uint8_t  master_tx_data[64]  __attribute__((aligned(32)));
uint8_t  slave_rx_data[64]  __attribute__((aligned(32)));

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

int hal_spi_demo(uint32_t *gpio_pin)
{
    int ret     = -1;
    int i       = 0;

    printf("spi demo start\n");

    uint32_t pin_func[8] = {0};
    for (i = 0; i < 8; i++) {
        pin_func[i] = get_spi_pin_func(gpio_pin[i], spi_pinmap);
    }

    uint8_t spi0_idx = get_spi_idx(gpio_pin[0], spi_pinmap);
    uint8_t spi1_idx = get_spi_idx(gpio_pin[4], spi_pinmap);

    for (i = 0; i < 8; i++) {
        csi_pin_set_mux(gpio_pin[i], pin_func[i]);
    }

    /* spi master port set */
    spi_master.port = spi0_idx;

    /* spi  master attr config */
    spi_master.config.mode  = HAL_SPI_MODE_MASTER;
    spi_master.config.freq = 100000;

    /* spi slave port set */
    spi_slave.port = spi1_idx;

    /* spi slave attr config */
    spi_slave.config.mode  = HAL_SPI_MODE_SLAVE;

    /* init spi_slave with the given settings */
    ret = hal_spi_init(&spi_master);
    if (ret != 0) {
        printf("spi_master init error !\n");
        return -1;
    }

    ret = hal_spi_init(&spi_slave);
    if (ret != 0) {
        printf("spi_slave init error !\n");
        return -1;
    }

    for (ret = 0; ret < SPI_DATA_NUM; ret++) {
        master_tx_data[ret] = ret + 1;
        slave_rx_data[ret] = 0;
    }

    ret = hal_spi_recv(&spi_slave, slave_rx_data, SPI_DATA_NUM, SPI_RX_TIMEOUT);

    ret = hal_spi_send(&spi_master, master_tx_data, SPI_DATA_NUM, SPI_TX_TIMEOUT);

    if (0 != memcmp(master_tx_data, slave_rx_data, SPI_DATA_NUM)) {
        printf("transfer error!\n");
        return -1;
    }

    hal_spi_finalize(&spi_master);
    hal_spi_finalize(&spi_slave);

    printf("transfer success!\n");

    return 0;
}

#endif
