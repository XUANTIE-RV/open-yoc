/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     spi_device.c
 * @brief
 * @version
 * @date     2020-03-18
 ******************************************************************************/
#include <string.h>

#include <drv/spiflash.h>
#include <drv/irq.h>
#include <drv/tick.h>

#include "spiflash_vendor.h"

/* communication parameters */
#define DEFUALT_SPI_DEVICE_TX_RX_TIMEOUT 1000000U
#define DEFUALT_SPI_DEVICE_CLOCK         500000U
#define DEFUALT_SPI_DEVICE_PHASE         SPI_FORMAT_CPOL0_CPHA0
#define DEFUALT_SPI_DEVICE_SALVE_IDX     0U

#define FLASH_CMD_WRITE_STATUS       0x01U
#define FLASH_CMD_READ_STATUS        0x05U
#define FLASH_CMD_WRITE_ENABLE       0x06U
#define FLASH_CMD_READ_JEDEC_ID      0x9FU
#define FLASH_CMD_SECTOR_ERASE       0x20U
#define FLASH_CMD_CHIP_ERASE         0x60U
#define FLASH_CMD_PAGE_PROGRAM       0x02U
#define FLASH_CMD_READ_DATA          0x03U
#define FLASH_CMD_SECTOR_ERASE       0x20U
#define FALSH_CMD_POWER_DOWN         0xB9U
#define FALSH_CMD_RELEASE_POWER_DOWN 0xABU

static int32_t send(void *spiflash, uint8_t cmd, uint32_t addr, uint32_t addr_size, const void *data, uint32_t size)
{
    int32_t  ret = 0;
    csi_spiflash_t *handle;
    uint32_t  addr_l;

    do {

        handle = (csi_spiflash_t *)spiflash;
        handle->spi_cs_callback(GPIO_PIN_LOW);

        /* command phase */
        ret = csi_spi_send(&handle->spi_qspi.spi, &cmd, 1U, DEFUALT_SPI_DEVICE_TX_RX_TIMEOUT);

        if( ret < 0 ){
            break;
        }

        /* address phase */
        if (addr_size > 0U) {

            // TUDO: replace by msb/lsv convert function
            if (addr_size == 3U) {
                addr_l  = ((addr & 0x000000FFU) << 16) |
                          ((addr & 0x0000FF00U))       |
                          ((addr & 0x00FF0000U) >> 16);
            } else if (addr_size == 4U) {
                addr_l  = ((addr & 0x000000FFU) << 24) |
                          ((addr & 0x0000FF00U) << 8) |
                          ((addr & 0x00FF0000U) >> 8)  |
                          ((addr & 0xFF000000U) >> 24);
            }

            ret = csi_spi_send(&handle->spi_qspi.spi, &addr_l, addr_size, DEFUALT_SPI_DEVICE_TX_RX_TIMEOUT);

            if( ret < 0 ){
                break;
            }
        }

        /* data phase */
        if (size > 0U) {
            ret = csi_spi_send(&handle->spi_qspi.spi, data, size, DEFUALT_SPI_DEVICE_TX_RX_TIMEOUT);
        }

        handle->spi_cs_callback(GPIO_PIN_HIGH);
    } while (0);

    return ret;
}

static int32_t receive(void *spiflash, uint8_t cmd, uint32_t addr, uint32_t addr_size, void *data, uint32_t size)
{
    int32_t   ret = 0;
    uint32_t  addr_l;
    csi_spiflash_t *handle;

    handle = (csi_spiflash_t *)spiflash;

    spiflash = (csi_spiflash_t *)spiflash;
    handle->spi_cs_callback(GPIO_PIN_LOW);

    /* command phase */
    do {
        ret = csi_spi_send(&handle->spi_qspi.spi, &cmd, 1U, DEFUALT_SPI_DEVICE_TX_RX_TIMEOUT);

        if( ret < 0 ){
            break;
        }

        /* address phase */
        if (addr_size > 0U) {
            // TUDO: replace by msb/lsv convert function
            if (addr_size == 3U) {
                addr_l  = ((addr & 0x000000FFU) << 16U) |
                          ((addr & 0x0000FF00U)) |
                          ((addr & 0x00FF0000U) >> 16U);
            } else if (addr_size == 4U) {
                addr_l  = ((addr & 0x000000FFU) << 24U) |
                          ((addr & 0x0000FF00U) << 8U) |
                          ((addr & 0x00FF0000U) >> 8U) |
                          ((addr & 0xFF000000U) >> 24U);
            }

            ret = csi_spi_send(&handle->spi_qspi.spi, &addr_l, addr_size, DEFUALT_SPI_DEVICE_TX_RX_TIMEOUT);

            if( ret < 0 ){
                break;
            }
        }

        /* data phase */
        if (size > 0U) {
            ret = csi_spi_receive(&handle->spi_qspi.spi, data, size, DEFUALT_SPI_DEVICE_TX_RX_TIMEOUT);
        }

        handle->spi_cs_callback(GPIO_PIN_HIGH);
    } while (0);

    return ret;
}


csi_error_t csi_spiflash_spi_init(csi_spiflash_t *spiflash, uint32_t spi_idx, void *spi_cs_callback)
{
    CSI_PARAM_CHK(spiflash,  CSI_ERROR);
    CSI_PARAM_CHK(spi_cs_callback, CSI_ERROR);

    uint32_t    addr, addr_size, data_size;
    uint32_t    data = 0U;
    uint32_t    flash_id = 0U;
    uint32_t    baud;
    uint8_t     cmd;
    csi_error_t ret = CSI_OK;

    spiflash->spi_cs_callback = spi_cs_callback;
    spiflash->spi_cs_callback(GPIO_PIN_HIGH);
    spiflash->spi_send      = send;
    spiflash->spi_receive   = receive;

    do {
        ret = csi_spi_init(&spiflash->spi_qspi.spi, spi_idx);

        if (ret != CSI_OK) {
            break;
        }

        ret = csi_spi_mode(&spiflash->spi_qspi.spi, SPI_MASTER);

        if (ret != CSI_OK) {
            break;
        }

        ret = csi_spi_cp_format(&spiflash->spi_qspi.spi, DEFUALT_SPI_DEVICE_PHASE);

        if (ret != CSI_OK) {
            break;
        }

        ret = csi_spi_frame_len(&spiflash->spi_qspi.spi, SPI_FRAME_LEN_8);

        if (ret != CSI_OK) {
            break;
        }

        baud = csi_spi_baud(&spiflash->spi_qspi.spi, DEFUALT_SPI_DEVICE_CLOCK);

        if (baud == 0U) {
            ret = CSI_ERROR;
            break;
        }

        csi_spi_select_slave(&spiflash->spi_qspi.spi, DEFUALT_SPI_DEVICE_SALVE_IDX);

#ifdef CONFIG_PM
        ret = csi_spiflash_release_power_down(spiflash);

        if (ret != CSI_OK) {
            break;
        }

#endif

        cmd       = FLASH_CMD_READ_JEDEC_ID;
        addr      = 0U;
        addr_size = 0U;
        data_size = 3U;
        spiflash->spi_receive(spiflash, cmd, addr, addr_size, &data, data_size);
        flash_id  = ((data & 0x000000FFU) << 16U) |
                    ((data & 0x0000FF00U)) |
                    ((data & 0x00FF0000U) >> 16U);

        ret = get_spiflash_vendor_param(flash_id, &spiflash->flash_prv_info);

        if (ret != CSI_OK) {
            ret = CSI_UNSUPPORTED;
            break;
        }

    } while (0);

    return ret;
}

void csi_spiflash_spi_uninit(csi_spiflash_t *spiflash)
{
    CSI_PARAM_CHK_NORETVAL(spiflash);

#ifdef CONFIG_PM
    csi_spiflash_power_down(spiflash);
#endif

    csi_spi_uninit(&spiflash->spi_qspi.spi);
}

__attribute__((weak)) csi_error_t spiflash_config_data_line(csi_spiflash_t *spiflash, csi_spiflash_data_line_t line)
{
    return CSI_UNSUPPORTED;
}

