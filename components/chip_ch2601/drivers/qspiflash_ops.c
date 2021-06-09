/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     qspi_device.c
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
#define DEFUALT_QSPI_DEVICE_TX_RX_TIMEOUT        0x100000U
#define DEFUALT_QSPI_DEVICE_PHASE         SPI_FORMAT_CPOL0_CPHA0
#define DEFUALT_QSPI_DEVICE_SALVE_IDX     0U

#define MAX_TRANSFER_SIZE_ONCE            8U

#define FLASH_CMD_WRITE_STATUS       0x01U
#define FLASH_CMD_READ_STATUS        0x05U
#define FLASH_CMD_WRITE_ENABLE       0x06U
#define FLASH_CMD_READ_JEDEC_ID      0x9FU
#define FLASH_CMD_SECTOR_ERASE       0x20U
#define FLASH_CMD_CHIP_ERASE         0x60U
#define FLASH_CMD_PAGE_PROGRAM       0x02U
#define FLASH_CMD_READ_DATA          0x03U
#define FLASH_CMD_FAST_READ_DUAL_IO  0xbbU
#define FLASH_CMD_FAST_READ_QUAL_IO  0xebU
#define FLASH_CMD_SECTOR_ERASE       0x20U
#define FALSH_CMD_POWER_DOWN         0xB9U
#define FALSH_CMD_RELEASE_POWER_DOWN 0xABU

#if defined(CONFIG_XIP)
#define ATTRIBUTE_DATA __attribute__((section(".ram.code")))
#else
#define ATTRIBUTE_DATA
#endif

static csi_qspi_command_t command;

ATTRIBUTE_DATA int32_t get_flash_status(csi_qspi_t *qspi, uint8_t *status)
{
    int32_t ret = 0;

    /* transmit READ_STATUS command */
    command.instruction.value     = FLASH_CMD_READ_STATUS;
    command.instruction.bus_width = QSPI_CFG_BUS_SINGLE;
    command.instruction.disabled  = (bool)false;
    command.alt.disabled          = (bool)true;
    command.address.disabled      = (bool)true;
    command.data.bus_width        = QSPI_CFG_BUS_SINGLE;

    /* receive status */
    ret = csi_qspi_receive(qspi, &command,  status, 1U, DEFUALT_QSPI_DEVICE_TX_RX_TIMEOUT);

    if (ret != 1) {
        ret = -1;
    } else {
        ret = 0;
    }

    return ret;
}

ATTRIBUTE_DATA int32_t flash_wait_ready(csi_qspi_t *qspi)
{
    uint32_t timeout = DEFUALT_QSPI_DEVICE_TX_RX_TIMEOUT;
    uint8_t  status;
    int32_t  ret = -1;

    while (timeout-- > 0U) {
        ret = get_flash_status(qspi, &status);

        if ((ret == 0) && ((status & 0x01U) == 0U)) {
            ret = 0;
            break;
        }
    }

    return ret;
}

ATTRIBUTE_DATA int32_t qspi_send_and_wait(csi_qspi_t *qspi, csi_qspi_command_t *command, const void *data, uint32_t size)
{
    int32_t  ret;
    uint32_t count = 0U;
    uint32_t result;

    do {
        result = csi_irq_save();
        ret = csi_qspi_send(qspi, command, data, size, DEFUALT_QSPI_DEVICE_TX_RX_TIMEOUT);

        if (ret < 0) {
            break;
        }

        count = (uint32_t)ret;

        ret = flash_wait_ready(qspi);
        csi_irq_restore(result);

    } while (0);

    if (ret >= 0) {
        ret = (int32_t)count;
    }

    return ret;
}

static int32_t send(void *spiflash, uint8_t cmd, uint32_t addr, uint32_t addr_size, const void *data, uint32_t size)
{
    int32_t  ret = 0;
    uint32_t current_size, current_addr ;
    csi_spiflash_t *handle;
    uint8_t  *temp;
    uint32_t count = 0U;

    handle = (csi_spiflash_t *)spiflash;
    temp   = (uint8_t *)data;

    // TODO: qspi unsupport large amout data write now
    do {

        if (cmd == 0x02U) {

            current_addr = addr;
            current_size = size;

            while (size > 0U) {
                if (current_size >= MAX_TRANSFER_SIZE_ONCE) {
                    current_size = MAX_TRANSFER_SIZE_ONCE;
                } else {
                    current_size = size;
                }

                if (size < current_size) {
                    current_size = size;
                }
                /* transmit READ_STATUS command */
                command.instruction.value     = FLASH_CMD_WRITE_ENABLE;
                command.instruction.bus_width = QSPI_CFG_BUS_SINGLE;
                command.instruction.disabled  = (bool)false;
                command.alt.disabled          = (bool)true;
                command.address.disabled      = (bool)true;
                command.data.bus_width        = QSPI_CFG_BUS_SINGLE;

                ret = qspi_send_and_wait(&handle->spi_qspi.qspi, &command, NULL, 0U);

                if (ret < 0) {
                    break;
                }

                /* page program command */
                command.instruction.value     = FLASH_CMD_PAGE_PROGRAM;
                command.instruction.bus_width = QSPI_CFG_BUS_SINGLE;
                command.instruction.disabled  = (bool)false;
                command.alt.disabled          = (bool)true;
                command.address.disabled      = (bool)false;
                command.address.value         = current_addr;
                command.address.size          = (addr_size > 0U) ? ((csi_qspi_address_size_t)addr_size - (csi_qspi_address_size_t)1U) : (csi_qspi_address_size_t)0U;
                command.address.bus_width     = QSPI_CFG_BUS_SINGLE;
                command.data.bus_width        = QSPI_CFG_BUS_SINGLE;

                ret = qspi_send_and_wait(&handle->spi_qspi.qspi, &command, temp, current_size);

                if (ret < 0) {
                    break;
                }

                /* update address */
                current_addr += current_size;
                temp  += current_size;
                size  -= current_size;
                count += current_size;
            }

        } else {
            command.instruction.value     = cmd;
            command.instruction.bus_width = QSPI_CFG_BUS_SINGLE;
            command.instruction.disabled  = (bool)false;
            command.alt.disabled          = (bool)true;
            command.address.disabled      = (addr_size > 0U) ? (bool)false : (bool)true;
            command.address.value         = addr;
            command.address.size          = (addr_size > 0U) ? ((csi_qspi_address_size_t)addr_size - (csi_qspi_address_size_t)1U) : (csi_qspi_address_size_t)0U;
            command.address.bus_width     = QSPI_CFG_BUS_SINGLE;
            command.data.bus_width        = QSPI_CFG_BUS_SINGLE;
            ret = qspi_send_and_wait(&handle->spi_qspi.qspi, &command, data, size);
        }
    } while (0);

    // return actual count if count != 0
    if (count != 0U) {
        ret = (int32_t)count;
    }

    return ret;
}

static int32_t receive(void *spiflash, uint8_t cmd, uint32_t addr, uint32_t addr_size, void *data, uint32_t size)
{
    int32_t  ret = 0;
    csi_spiflash_t *handle;

    handle = (csi_spiflash_t *)spiflash;

    command.instruction.value     = cmd;
    command.instruction.bus_width = QSPI_CFG_BUS_SINGLE;
    command.instruction.disabled  = (bool)false;
    command.alt.disabled          = (bool)true;
    command.address.disabled      = (addr_size > 0U) ? (bool)false : (bool)true;
    command.address.value         = addr;
    command.address.size          = (addr_size > 0U) ? ((csi_qspi_address_size_t)addr_size - (csi_qspi_address_size_t)1U) : (csi_qspi_address_size_t)0U;
    command.address.bus_width     = QSPI_CFG_BUS_SINGLE;
    command.data.bus_width        = QSPI_CFG_BUS_SINGLE;
    ret = csi_qspi_receive(&handle->spi_qspi.qspi, &command, data, size, DEFUALT_QSPI_DEVICE_TX_RX_TIMEOUT);

    return ret;
}

csi_error_t csi_spiflash_qspi_init(csi_spiflash_t *spiflash, uint32_t qspi_idx, void *qspi_cs_callback)
{
    CSI_PARAM_CHK(spiflash, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    uint32_t    flash_id = 0U;
    uint32_t    addr, addr_size, data_size;
    uint32_t    data = 0U;
    uint8_t     cmd;

    spiflash->spi_cs_callback = NULL;
    spiflash->spi_send        = send;
    spiflash->spi_receive     = receive;
    memset(&command, (int)0, sizeof(csi_qspi_command_t));

    do {
        ret = csi_qspi_init(&spiflash->spi_qspi.qspi, qspi_idx);

        if (ret != CSI_OK) {
            ret = CSI_ERROR;
            break;
        }

#ifdef CONFIG_PM
        ret = csi_spiflash_release_power_down(spiflash);

        if (ret != CSI_OK) {
            ret = CSI_ERROR;
            break;
        }

#endif

        cmd       = FLASH_CMD_READ_JEDEC_ID;
        addr      = 0U;
        addr_size = 0U;
        data_size = 3U;
        ret = (csi_error_t)spiflash->spi_receive(spiflash, cmd, addr, addr_size, &data, data_size);

        if (ret < 0) {
            break;
        }

        flash_id  = ((data & 0x000000FFU) << 16) |
                    ((data & 0x0000FF00U))       |
                    ((data & 0x00FF0000U) >> 16);

        ret = get_spiflash_vendor_param(flash_id, &spiflash->flash_prv_info);

        if (ret != CSI_OK) {
            ret = CSI_UNSUPPORTED;
        }
    } while (0);

    return ret;
}

void csi_spiflash_qspi_uninit(csi_spiflash_t *spiflash)
{
    CSI_PARAM_CHK_NORETVAL(spiflash);

#ifdef CONFIG_PM
    csi_spiflash_power_down(spiflash);
#endif

    csi_qspi_uninit(&spiflash->spi_qspi.qspi);
}

csi_error_t spiflash_config_data_line(csi_spiflash_t *spiflash, csi_spiflash_data_line_t line)
{
    csi_error_t ret = CSI_OK;

    memset(&command, (int)0, sizeof(command));

    switch (line) {
        case SPIFLASH_DATA_1_LINE:

            command.instruction.value     = FLASH_CMD_READ_DATA;
            command.instruction.bus_width = QSPI_CFG_BUS_SINGLE;
            command.address.bus_width     = QSPI_CFG_BUS_SINGLE;
            command.data.bus_width        = QSPI_CFG_BUS_SINGLE;
            command.dummy_count           = 0U;
            command.alt.disabled          = (bool)true;
            break;

        case SPIFLASH_DATA_2_LINES:

            command.instruction.value     = FLASH_CMD_FAST_READ_DUAL_IO;
            command.instruction.bus_width = QSPI_CFG_BUS_SINGLE;
            command.address.bus_width     = QSPI_CFG_BUS_DUAL;
            command.data.bus_width        = QSPI_CFG_BUS_DUAL;
            command.dummy_count           = 0U;
            break;

        case SPIFLASH_DATA_4_LINES:

            command.instruction.value     = FLASH_CMD_FAST_READ_QUAL_IO;
            command.instruction.bus_width = QSPI_CFG_BUS_SINGLE;
            command.address.bus_width     = QSPI_CFG_BUS_QUAD;
            command.data.bus_width        = QSPI_CFG_BUS_QUAD;
            command.dummy_count           = 4U;
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    if (ret == CSI_OK) {
        ret = csi_qspi_memory_mapped(&spiflash->spi_qspi.qspi, &command);

        if (ret != CSI_OK) {
            ret = CSI_ERROR;
        }
    }

    memset(&command, (int)0, sizeof(command));
    return ret;
}
