/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     cs_qspi_ll.c
 * @brief
 * @version
 * @date     2020-02-22
 ******************************************************************************/
#include "cd_qspi_ll.h"
#include <string.h>

#if defined(CONFIG_XIP)
#define ATTRIBUTE_DATA __attribute__((section(".ram.code")))
#else
#define ATTRIBUTE_DATA
#endif


ATTRIBUTE_DATA int cd_qspi_config_baudrate(cd_qspi_regs_t *qspi, uint32_t clock_in, uint32_t clock_out)
{
    uint32_t div;
    int ret = 0;

    do {
        if ((clock_in == 0U) || (clock_out == 0U)) {
            ret = -1;
            break;
        }

        if(clock_in < clock_out) {
            ret = -1;
            break;
        }

        div = clock_in / clock_out;
        div = (div - 2U)  / 2U;

        // limit min CFGR = 4
        if (div <= 1U) {
            div = 1U;
        }

        // limit min CFGR = 32
        if (div >= 15U) {
            div = 15U;
        }


        qspi->CFGR &= ~CD_QSPI_CFGR_BDDIV_Msk;
        qspi->CFGR |= (div << CD_QSPI_CFGR_BDDIV_Pos);
    } while (0);

    return ret;
}

int cd_qspi_get_freq_div(cd_qspi_regs_t *qspi)
{
    uint32_t div;
    div = (qspi->CFGR & CD_QSPI_CFGR_BDDIV_Msk) >> CD_QSPI_CFGR_BDDIV_Pos;
    div = (div + 1U) * 2U;

    return (int)div;
}

void cd_qspi_enable_slave(cd_qspi_regs_t *qspi, uint32_t idx)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_PSCL_Msk;

    if (idx < 4U) {
        qspi->CFGR |= ~(idx << CD_QSPI_CFGR_PSCL_Pos);
    }
}

void cd_qspi_disable_slave(cd_qspi_regs_t *qspi, uint32_t idx)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_PSCL_Msk;

    if (idx < 4U) {
        qspi->CFGR |= (idx << CD_QSPI_CFGR_PSCL_Pos);
    }
}

int cd_qspi_config_csda_delay(cd_qspi_regs_t *qspi, uint32_t delay_cycles)
{
    int ret = 0;

    do {
        if ((delay_cycles > 256U) || (delay_cycles == 0U)) {
            ret = -1;
            break;
        }

        qspi->DDR &=  ~CD_QSPI_DDR_CSDA_Msk;
        qspi->DDR |=  delay_cycles << CD_QSPI_DDR_CSDA_Pos;
    } while (0);

    return ret;
}

int cd_qspi_config_read_capture_delay(cd_qspi_regs_t *qspi, uint32_t delay_cycles)
{
    int ret = 0;

    if (delay_cycles > 0xFU) {
        ret = -1;
    } else {
        qspi->RDCR &= ~CD_QSPI_RDCR_RDCD_Msk;
        qspi->RDCR |= (delay_cycles << CD_QSPI_RDCR_RDCD_Pos);
    }

    return ret;
}

int cd_qspi_config_flash_size(cd_qspi_regs_t *qspi, uint32_t idx, uint32_t mb)
{
    uint32_t size;
    int      ret = 0;

    switch (mb) {
        case 512:
            size = 0U;
            break;

        case 1024:
            size = 1U;
            break;

        case 2048:
            size = 2U;
            break;

        case 4096:
            size = 3U;
            break;

        default:
            ret = -1;
            break;

    }

    if (ret == 0) {
        switch (idx) {
            case 0:
                qspi->DSCFGR &= ~CD_QSPI_DSCR_FS0_Msk;
                qspi->DSCFGR |= (size << CD_QSPI_DSCR_FS0_Pos);
                break;

            case 1:
                qspi->DSCFGR &= ~CD_QSPI_DSCR_FS1_Msk;
                qspi->DSCFGR |= (size << CD_QSPI_DSCR_FS1_Pos);
                break;

            case 2:
                qspi->DSCFGR &= ~CD_QSPI_DSCR_FS2_Msk;
                qspi->DSCFGR |= (size << CD_QSPI_DSCR_FS2_Pos);
                break;

            case 3:
                qspi->DSCFGR &= ~CD_QSPI_DSCR_FS3_Msk;
                qspi->DSCFGR |= (size << CD_QSPI_DSCR_FS3_Pos);
                break;

            default:
                ret = -1;
                break;
        }
    }

    return ret;
}

ATTRIBUTE_DATA void cd_qspi_read_stig_data(cd_qspi_regs_t *qspi, uint8_t *data, uint32_t num)
{
    uint8_t i;
    uint8_t buf[8];
    void    *p;

    p = buf;
    *(uint32_t *)(p) = qspi->FCRDRL;

    p = &buf[4];
    *(uint32_t *)(p) = qspi->FCRDRU;

    if (num <= 8U) {
        for (i = 0U; i < num; i++) {
            *data++ = buf[i];
        }
    }
}

void cd_qspi_write_stig_data(cd_qspi_regs_t *qspi, uint8_t *data, uint32_t num)
{
    uint8_t i;
    uint8_t buf[8];
    void    *p;

    memset(buf, (int)0, sizeof(buf));

    if (num <= 8U) {
        for (i = 0U; i < num; i++) {
            buf[i] = *data;
            data  += sizeof(uint8_t);
        }

        p = buf;
        qspi->FCWDRL = *(uint32_t *)(p);

        p = &buf[4];
        qspi->FCWDRU = *(uint32_t *)(p);
    }

}

void cd_qspi_config_stig_memory_read_bytes(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    uint32_t value;
    qspi->FCCMR &= ~CD_QSPI_FCCMR_MBRBN_Msk;

    switch (bytes) {
        case 16:
            value = 0U;
            qspi->FCCMR |= (value << CD_QSPI_FCCMR_MBRBN_Pos);
            break;

        case 32:
            value = 1U;
            qspi->FCCMR |= (value << CD_QSPI_FCCMR_MBRBN_Pos);
            break;

        case 64:
            value = 2U;
            qspi->FCCMR |= (value << CD_QSPI_FCCMR_MBRBN_Pos);
            break;

        case 128:
            value = 3U;
            qspi->FCCMR |= (value << CD_QSPI_FCCMR_MBRBN_Pos);
            break;

        case 256:
            value = 4U;
            qspi->FCCMR |= (value << CD_QSPI_FCCMR_MBRBN_Pos);
            break;

        case 512:
            value = 5U;
            qspi->FCCMR |= (value << CD_QSPI_FCCMR_MBRBN_Pos);
            break;

        default:
            break;
    }
}

void cd_qspi_reset_regs(cd_qspi_regs_t *qspi)
{
    qspi->CFGR      = 0x80780081U;
    qspi->DRIR      = 0x3U;
    qspi->DWIR      = 0x2U;
    qspi->DDR       = 0x0U;
    qspi->RDCR      = 0x1U;
    qspi->DSCFGR    = 0x101002U;
    qspi->SRAMPCFGR = 0x80U;
    qspi->IAATR     = 0x0U;
    qspi->DPCFGR    = 0x0U;
    qspi->RMAR      = 0x0U;
    qspi->SRAMFLR   = 0x0U;
    qspi->TXTHR     = 0x1U;
    qspi->RXTHR     = 0x1U;
    qspi->WCCTRLR   = 0x10005U;
    qspi->PER       = 0xffffffffU;
    qspi->ISR       = 0x0U;
    qspi->IMR       = 0x0U;
    qspi->LWPR      = 0x0U;
    qspi->UWPR      = 0x0U;
    qspi->WPR       = 0x0U;
    qspi->IRTCR     = 0x0U;
    qspi->IRTWR     = 0x0U;
    qspi->IRTSAR    = 0x0U;
    qspi->IRTNBR    = 0x0U;
    qspi->IWTCR     = 0x0U;
    qspi->IWTWR     = 0xffffffffU;
    qspi->IWTSAR    = 0x0U;
    qspi->IWTNBR    = 0x0U;
    qspi->ITARR     = 0x4U;
    qspi->FCCMR     = 0x0U;
    qspi->FCCR      = 0x0U;
    qspi->FCAR      = 0x0U;
    qspi->FCRDRL    = 0x0U;
    qspi->FCRDRU    = 0x0U;
    qspi->FCWDRL    = 0x0U;
    qspi->FCWDRU    = 0x0U;
    qspi->PFSR      = 0x0U;
}

ATTRIBUTE_DATA uint32_t cd_qspi_get_idle_state(cd_qspi_regs_t *qspi)
{
    return qspi->CFGR & CD_QSPI_CFGR_IDLE_EN;
}

ATTRIBUTE_DATA uint32_t cd_qspi_get_stig_process_state(cd_qspi_regs_t *qspi)
{
    return qspi->FCCR & CD_QSPI_FCCR_PS_Msk;
}

ATTRIBUTE_DATA void cd_qspi_execute_stig_command(cd_qspi_regs_t *qspi)
{
    qspi->FCCR |= CD_QSPI_FCCR_EXEC_EN;
}

ATTRIBUTE_DATA void cd_qspi_config_stig_cmd_opcode(cd_qspi_regs_t *qspi, uint8_t opcode)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_OPC_Msk;
    qspi->FCCR |= (( uint32_t )opcode << CD_QSPI_FCCR_OPC_Pos);
}

ATTRIBUTE_DATA void cd_qspi_enable_stig_command_address(cd_qspi_regs_t *qspi)
{
    qspi->FCCR |= CD_QSPI_FCCR_CA_EN;
}

ATTRIBUTE_DATA void cd_qspi_config_stig_address_bytes(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_NOAB_Msk;
    qspi->FCCR |= ((bytes) << CD_QSPI_FCCR_NOAB_Pos);
}

ATTRIBUTE_DATA void cd_qspi_disable_stig_command_address(cd_qspi_regs_t *qspi)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_CA_EN;
}

ATTRIBUTE_DATA void cd_qspi_config_stig_dummy_cycles(cd_qspi_regs_t *qspi, uint8_t cycles)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_NODMY_Msk;
    qspi->FCCR |= ((( uint32_t )cycles - 1U) << CD_QSPI_FCCR_NODMY_Pos);
}

ATTRIBUTE_DATA void cd_qspi_enable_stig_mode_bit(cd_qspi_regs_t *qspi)
{
    qspi->FCCR |= CD_QSPI_FCCR_MDB_EN;
}

ATTRIBUTE_DATA void cd_qspi_enable_ddr(cd_qspi_regs_t *qspi)
{
    qspi->DRIR |= CD_QSPI_DRIR_DDR_EN;
}

ATTRIBUTE_DATA uint32_t cd_qspi_get_stig_command_address(cd_qspi_regs_t *qspi)
{
    return qspi->FCAR;
}

ATTRIBUTE_DATA void cd_qspi_config_stig_read_bytes(cd_qspi_regs_t *qspi, uint32_t num)
{
    if (num <= 8U) {
        qspi->FCCR &= ~CD_QSPI_FCCR_NORDB_Msk;
        qspi->FCCR |= ((num - 1U) << CD_QSPI_FCCR_NORDB_Pos);
    }
}

ATTRIBUTE_DATA void cd_qspi_enable_stig_read_data(cd_qspi_regs_t *qspi)
{
    qspi->FCCR |= CD_QSPI_FCCR_RD_EN;
}

ATTRIBUTE_DATA void cd_qspi_config_stig_command_address(cd_qspi_regs_t *qspi, uint32_t address)
{
    qspi->FCAR = address;
}

ATTRIBUTE_DATA void cd_qspi_reset_stig(cd_qspi_regs_t *qspi)
{
    qspi->FCCR      = 0x0U;
}

ATTRIBUTE_DATA void cd_qspi_config_read_opcode(cd_qspi_regs_t *qspi, uint8_t opcode)
{
    qspi->DRIR &= ~CD_QSPI_DRIR_ROPC_Msk;
    qspi->DRIR |= (( uint32_t )opcode << CD_QSPI_DRIR_ROPC_Pos);
}

ATTRIBUTE_DATA void cd_qspi_config_read_instruction_type(cd_qspi_regs_t *qspi, uint32_t type)
{
    qspi->DRIR &= ~CD_QSPI_DRIR_INST_Msk;
    qspi->DRIR |= (type << CD_QSPI_DRIR_INST_Pos);
}

ATTRIBUTE_DATA void cd_qspi_config_read_address_type(cd_qspi_regs_t *qspi, uint32_t type)
{
    qspi->DRIR &= ~CD_QSPI_DRIR_ATT_Msk;
    qspi->DRIR |= (type << CD_QSPI_DRIR_ATT_Pos);
}

ATTRIBUTE_DATA void cd_qspi_config_read_data_type(cd_qspi_regs_t *qspi, uint32_t type)
{
    qspi->DRIR &= ~CD_QSPI_DRIR_DTT_Msk;
    qspi->DRIR |= (type << CD_QSPI_DRIR_DTT_Pos);
}

ATTRIBUTE_DATA void cd_qspi_config_read_dummy_cycles(cd_qspi_regs_t *qspi, uint8_t cycles)
{
    qspi->DRIR &= ~CD_QSPI_DRIR_DMYC_Msk;
    qspi->DRIR |= (( uint32_t )cycles << CD_QSPI_DRIR_DMYC_Pos);
}

ATTRIBUTE_DATA void cd_qspi_enable_read_mode_bit(cd_qspi_regs_t *qspi)
{
    qspi->DRIR |= CD_QSPI_DRIR_MB_Msk;
}
