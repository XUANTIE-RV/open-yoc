#include <string.h>

#include <drv/qspi.h>
#include <drv/tick.h>
#include <drv/irq.h>

#include "cd_qspi_ll.h"

#define CD_QSPI_DEFAULT_TRANSFER_SIZE   8U

#if defined(CONFIG_XIP)
#define ATTRIBUTE_DATA __attribute__((section(".ram.code")))
#else
#define ATTRIBUTE_DATA
#endif

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

csi_error_t csi_qspi_init(csi_qspi_t *qspi, uint32_t idx)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if (target_get(DEV_CD_QSPI_TAG, idx, &qspi->dev) != CSI_OK) {
        ret = CSI_ERROR;
    } else {
        qspi->state.writeable = 1U;
        qspi->state.readable  = 1U;
        qspi->state.error     = 0U;
        qspi->callback        = NULL;
        qspi->arg             = NULL;
        qspi->tx_data         = NULL;
        qspi->rx_data         = NULL;
        qspi->send            = NULL;
        qspi->receive         = NULL;
        qspi->send_receive    = NULL;
        qspi->tx_dma          = NULL;
        qspi->rx_dma          = NULL;

#ifdef CONFIG_SYSTEM_SECURE
        cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);
        *( volatile uint32_t*)((uint32_t)qspi_base + 0x200U) = FLASH_CMD_READ_JEDEC_ID;
        *( volatile uint32_t*)((uint32_t)qspi_base + 0x204U) = FLASH_CMD_WRITE_ENABLE;
        *( volatile uint32_t*)((uint32_t)qspi_base + 0x208U) = FLASH_CMD_SECTOR_ERASE;
        *( volatile uint32_t*)((uint32_t)qspi_base + 0x20cU) = FLASH_CMD_CHIP_ERASE;
        *( volatile uint32_t*)((uint32_t)qspi_base + 0x210U) = FLASH_CMD_READ_STATUS;
        *( volatile uint32_t*)((uint32_t)qspi_base + 0x214U) = FLASH_CMD_READ_DATA;
        *( volatile uint32_t*)((uint32_t)qspi_base + 0x218U) = FLASH_CMD_PAGE_PROGRAM;
        *( volatile uint32_t*)((uint32_t)qspi_base + 0x21cU) = FALSH_CMD_RELEASE_POWER_DOWN;
#endif
    }

    cd_qspi_disable_write_directly((cd_qspi_regs_t *)HANDLE_REG_BASE(qspi));

    return ret;
}

void csi_qspi_uninit(csi_qspi_t *qspi)
{
    CSI_PARAM_CHK_NORETVAL(qspi);

    cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);

    /* reset all registers */
    cd_qspi_reset_regs(qspi_base);

    /* unregister irq */
    csi_irq_disable((uint32_t)qspi->dev.irq_num);
    csi_irq_detach((uint32_t)qspi->dev.irq_num);
}

csi_error_t csi_qspi_attach_callback(csi_qspi_t *qspi, void *callback, void *arg)
{
    CSI_PARAM_CHK(qspi    , CSI_ERROR);
    CSI_PARAM_CHK(callback, CSI_ERROR);

    qspi->callback  = callback;
    qspi->arg       = arg;

    csi_irq_attach((uint32_t)qspi->dev.irq_num, NULL, &qspi->dev);
    csi_irq_enable((uint32_t)qspi->dev.irq_num);

    return CSI_OK;
}

void csi_qspi_detach_callback(csi_qspi_t *qspi)
{
    CSI_PARAM_CHK_NORETVAL(qspi);

    qspi->callback  = NULL;
    qspi->arg       = NULL;
}

ATTRIBUTE_DATA uint32_t csi_qspi_frequence(csi_qspi_t *qspi, uint32_t hz)
{
    CSI_PARAM_CHK(qspi, 0U);
    CSI_PARAM_CHK(hz,   0U);

    int div;

    cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);
    cd_qspi_config_baudrate(qspi_base, soc_get_qspi_freq(0U), hz);
    div = cd_qspi_get_freq_div(qspi_base);
    hz  = soc_get_qspi_freq(0U) / (uint32_t)div;

    return hz;
}

csi_error_t csi_qspi_mode(csi_qspi_t *qspi, csi_qspi_mode_t mode)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);

    switch (mode) {
        case QSPI_CLOCK_MODE_0:
            cd_qspi_set_cpol0(qspi_base);
            cd_qspi_set_cpha0(qspi_base);
            break;

        case QSPI_CLOCK_MODE_3:
            cd_qspi_set_cpol1(qspi_base);
            cd_qspi_set_cpha0(qspi_base);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

ATTRIBUTE_DATA void qspi_config_command(csi_qspi_t *qspi, csi_qspi_command_t *cmd)
{
    cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);

    /* configure instruction */
    if (cmd->instruction.disabled == (bool)false) {

        /* used for STIG mode */
        cd_qspi_config_stig_cmd_opcode(qspi_base, cmd->instruction.value);
    }

    /* configure address */
    if (cmd->address.disabled == (bool)false) {
        /* used for STIG mode */
        cd_qspi_enable_stig_command_address(qspi_base);
        cd_qspi_config_stig_address_bytes(qspi_base, (uint32_t)cmd->address.size);
        cd_qspi_config_stig_command_address(qspi_base, cmd->address.value);
    }

    /* configure dummy cycles */
    if (cmd->dummy_count) {
        /* used for STIG mode */
        cd_qspi_config_stig_dummy_cycles(qspi_base, cmd->dummy_count);
    }

    /* configure mode */
    if (cmd->alt.disabled == (bool)false) {
        cd_qspi_enable_stig_mode_bit(qspi_base);
        cd_qspi_config_mode_byte(qspi_base, (uint32_t)cmd->alt.size);
    }

    /* configure DDR */
    if (cmd->ddr_enable) {
        cd_qspi_enable_ddr(qspi_base);
    }
}

ATTRIBUTE_DATA csi_error_t qspi_execute_stig_command(csi_qspi_t *qspi, uint32_t timeout)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);
    cd_qspi_execute_stig_command(qspi_base);

    /* wait qspi process complete */
    while ((cd_qspi_get_stig_process_state(qspi_base) != 0U) || (cd_qspi_get_idle_state(qspi_base) == 0U)) {
        if (timeout-- == 0U) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    return ret;
}

ATTRIBUTE_DATA int32_t qspi_send(csi_qspi_t *qspi, uint32_t timeout)
{
    int32_t ret = 0;

    ret = qspi_execute_stig_command(qspi, timeout);

    /* return actual send count */
    if (ret == 0) {
        ret = (int32_t)qspi->tx_size;
    }

    qspi->tx_size = 0U;
    return ret;
}

ATTRIBUTE_DATA int32_t csi_qspi_send(csi_qspi_t *qspi, csi_qspi_command_t *cmd, const void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);
    CSI_PARAM_CHK(cmd,  CSI_ERROR);
    CSI_PARAM_CHK(timeout, CSI_ERROR);

    uint32_t address;
    int32_t  ret = CSI_OK;

    do {
        if ((qspi->state.writeable == 0U) || (qspi->state.readable == 0U)) {
            ret = CSI_BUSY;
            break;
        }

        cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);

        /* reset qspi */
        cd_qspi_reset_stig(qspi_base);

        qspi_config_command(qspi, cmd);

        if (size == 0U) {

            ret = qspi_execute_stig_command(qspi, timeout);

            /* reset qspi */
            cd_qspi_reset_stig(qspi_base);
            break;
        }

        qspi->state.writeable = 0U;
        qspi->tx_data         = (void *)data;
        qspi->tx_size         = size;
        address = cd_qspi_get_stig_command_address(qspi_base);

        /* TODU: transfer use indirect access mode, only support STIG mode */
        if (qspi->tx_size > CD_QSPI_DEFAULT_TRANSFER_SIZE) {
            qspi->state.writeable = 1U;
            ret = CSI_ERROR;
            break;
        }

        /* configure qspi registers */
        cd_qspi_enable_stig_write_data(qspi_base);
        cd_qspi_config_stig_write_data_bytes(qspi_base, qspi->tx_size);
        cd_qspi_config_stig_command_address(qspi_base, address);
        cd_qspi_write_stig_data(qspi_base, qspi->tx_data, qspi->tx_size);

        qspi->state.writeable = 1U;
        ret = qspi_send(qspi, timeout);

    } while (0);

    return ret;
}


ATTRIBUTE_DATA int32_t csi_qspi_receive(csi_qspi_t *qspi, csi_qspi_command_t *cmd, void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);
    CSI_PARAM_CHK(cmd,  CSI_ERROR);
    CSI_PARAM_CHK(timeout, CSI_ERROR);

    uint32_t address;
    int32_t  ret   = CSI_OK;
    uint32_t count = 0U;
    uint32_t current_size = 0U;

    do {
        if ((qspi->state.writeable == 0U) || (qspi->state.readable == 0U)) {
            ret = CSI_BUSY;
            break;
        }

        cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);
        qspi_config_command(qspi, cmd);

        if (size == 0U) {
            ret = qspi_execute_stig_command(qspi, timeout);

            /* reset qspi */
            cd_qspi_reset_stig(qspi_base);
            break;
        }


        qspi->rx_data        = data;
        qspi->rx_size        = size;
        qspi->state.readable = 0U;
        address              = cd_qspi_get_stig_command_address(qspi_base);

        /* transfer loop */
        /* TODU: transfer use indirect access mode */
        while (qspi->rx_size > 0U) {
            current_size = (qspi->rx_size > CD_QSPI_DEFAULT_TRANSFER_SIZE) ? (uint32_t) CD_QSPI_DEFAULT_TRANSFER_SIZE : qspi->rx_size;

            /* configure qspi registers */
            cd_qspi_config_stig_read_bytes(qspi_base, current_size);
            cd_qspi_enable_stig_read_data(qspi_base);
            cd_qspi_config_stig_command_address(qspi_base, address);
            ret = (int32_t)qspi_execute_stig_command(qspi, timeout);

            if (ret < 0) {
                break;
            }

            cd_qspi_read_stig_data(qspi_base, qspi->rx_data, current_size);
            qspi->rx_data  += current_size;
            qspi->rx_size  -= current_size;
            count   += current_size;
            address +=  current_size;
        }

        qspi->state.readable = 1U;

        /* configure qspi registers */
        cd_qspi_reset_stig(qspi_base);
    } while (0);

    /* return actual sent count if no error occur */
    if (ret >= 0) {
        ret = (int32_t)count;
    }

    return ret;
}

int32_t csi_qspi_send_receive(csi_qspi_t *qspi, csi_qspi_command_t *cmd, const void *tx_data, void *rx_data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);
    CSI_PARAM_CHK(cmd,  CSI_ERROR);
    CSI_PARAM_CHK(tx_data, CSI_ERROR);
    CSI_PARAM_CHK(rx_data, CSI_ERROR);
    CSI_PARAM_CHK(size,    CSI_ERROR);
    CSI_PARAM_CHK(timeout, CSI_ERROR);

    return 0;
}

csi_error_t csi_qspi_send_async(csi_qspi_t *qspi, csi_qspi_command_t *cmd, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);
    CSI_PARAM_CHK(cmd,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    return CSI_UNSUPPORTED;
}

csi_error_t csi_qspi_receive_async(csi_qspi_t *qspi, csi_qspi_command_t *cmd, void *data, uint32_t size)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);
    CSI_PARAM_CHK(cmd,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    return CSI_UNSUPPORTED;
}

csi_error_t csi_qspi_send_receive_async(csi_qspi_t *qspi, csi_qspi_command_t *cmd, const void *tx_data, void *rx_data, uint32_t size)
{
    CSI_PARAM_CHK(qspi,    CSI_ERROR);
    CSI_PARAM_CHK(cmd,     CSI_ERROR);
    CSI_PARAM_CHK(tx_data, CSI_ERROR);
    CSI_PARAM_CHK(rx_data, CSI_ERROR);
    CSI_PARAM_CHK(size,    CSI_ERROR);

    return CSI_UNSUPPORTED;
}

csi_error_t csi_qspi_get_state(csi_qspi_t *qspi, csi_state_t *state)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);
    CSI_PARAM_CHK(state, CSI_ERROR);

    *state = qspi->state;
    return CSI_OK;
}

csi_error_t csi_qspi_link_dma(csi_qspi_t *qspi, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);
    CSI_PARAM_CHK(tx_dma, CSI_ERROR);
    CSI_PARAM_CHK(rx_dma, CSI_ERROR);

    return CSI_UNSUPPORTED;
}

ATTRIBUTE_DATA csi_error_t csi_qspi_memory_mapped(csi_qspi_t *qspi, csi_qspi_command_t *cmd)
{
    CSI_PARAM_CHK(qspi, CSI_ERROR);
    CSI_PARAM_CHK(cmd,  CSI_ERROR);

    cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)HANDLE_REG_BASE(qspi);

    /* used for indirect/direct mode */
    cd_qspi_config_read_opcode(qspi_base, cmd->instruction.value);

    /* used for indirect/direct mode */
    cd_qspi_config_read_instruction_type(qspi_base, (uint32_t) cmd->instruction.bus_width);

    /* used for indirect/direct mode */
    cd_qspi_config_read_address_type(qspi_base, (uint32_t) cmd->address.bus_width);
    cd_qspi_config_read_data_type(qspi_base, (uint32_t) cmd->data.bus_width);

    /* used for indirect/direct mode */
    cd_qspi_config_read_dummy_cycles(qspi_base,  cmd->dummy_count);

    /* configure mode */
    if (cmd->alt.disabled == (bool)false) {
        cd_qspi_enable_read_mode_bit(qspi_base);
        cd_qspi_config_mode_byte(qspi_base, (uint32_t) cmd->alt.size);
    } else {
        cd_qspi_disable_read_mode_bit(qspi_base);
    }

    /* configure DDR */
    if (cmd->ddr_enable) {
        cd_qspi_enable_ddr(qspi_base);
    }

    return CSI_OK;
}

#ifdef CONFIG_PM
static csi_error_t dw_qspi_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action)
{
    CSI_PARAM_CHK(dev, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csi_pm_dev_t *pm_dev = &dev->pm_dev;

    switch (action) {
        case PM_DEV_SUSPEND:
            csi_pm_dev_save_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 4U);
            break;

        case PM_DEV_RESUME:
            csi_pm_dev_restore_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 4U);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t csi_qspi_enable_pm(csi_qspi_t *qspi)
{
    return csi_pm_dev_register(&qspi->dev, dw_qspi_pm_action, 16U, 0U);
}

void csi_qspi_disable_pm(csi_qspi_t *qspi)
{
    csi_pm_dev_unregister(&qspi->dev);
}
#endif
