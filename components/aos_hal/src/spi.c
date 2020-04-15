/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/hal/spi.h>
#include <aos/kernel.h>
#include <errno.h>
#include <drv/spi.h>

static aos_sem_t spi_sem[6];

static void spi_event_cb_fun(int32_t idx, spi_event_e event)
{
    switch (event) {
    case SPI_EVENT_TRANSFER_COMPLETE:
    case SPI_EVENT_TX_COMPLETE:
    case SPI_EVENT_RX_COMPLETE:
        aos_sem_signal(&spi_sem[idx]);
        break;
    default:;
    }
}

int32_t hal_spi_init(spi_dev_t *spi)
{
    int32_t ret;
    spi_handle_t spi_handle;

    if (spi == NULL)
        return -1;

    spi_handle = csi_spi_initialize(spi->port, spi_event_cb_fun);
    if (spi_handle == NULL) {
        return -1;
    }
    spi->priv = spi_handle;
    ret = csi_spi_config(spi_handle, spi->config.freq, spi->config.mode,
                         SPI_FORMAT_CPOL0_CPHA0, SPI_ORDER_MSB2LSB,
                         SPI_SS_MASTER_SW, 8);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret;
    if (spi == NULL)
        return -1;
    ret = aos_sem_new(&spi_sem[spi->port], 0);
    if (ret != 0) {
        return -1;
    }
    csi_spi_send(spi->priv, data, size);
    ret = aos_sem_wait(&spi_sem[spi->port], timeout);
    aos_sem_free(&spi_sem[spi->port]);
    return ret;
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret;
    if (spi == NULL)
        return -1;
    ret = aos_sem_new(&spi_sem[spi->port], 0);
    if (ret != 0) {
        return -1;
    }
    csi_spi_receive(spi->priv, data, size);
    ret = aos_sem_wait(&spi_sem[spi->port], timeout);
    aos_sem_free(&spi_sem[spi->port]);
    return ret;
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    int32_t ret;
    if (spi == NULL)
        return -1;
    ret = aos_sem_new(&spi_sem[spi->port], 0);
    if (ret != 0) {
        return -1;
    }
    csi_spi_transfer(spi->priv, tx_data, rx_data, size, size);
    ret = aos_sem_wait(&spi_sem[spi->port], timeout);
    aos_sem_free(&spi_sem[spi->port]);
    return ret;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
    csi_spi_uninitialize(spi->priv);
    return 0;
}
