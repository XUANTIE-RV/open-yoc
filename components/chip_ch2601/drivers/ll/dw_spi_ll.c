/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_spi_ll.c
 * @brief
 * @version
 * @date     2020-02-10
 ******************************************************************************/

#include "dw_spi_ll.h"

void dw_spi_config_sclk_clock(dw_spi_regs_t *spi_base, uint32_t clock_in, uint32_t clock_out)
{
    uint32_t div;

    div = clock_in / clock_out;
    div = (div > 65534U) ? 65534U : div;
    spi_base->BAUDR &= DW_SPI_BAUDR_SCKDV_Msk;
    spi_base->BAUDR = div;
}

uint32_t dw_spi_get_sclk_clock_div(dw_spi_regs_t *spi_base)
{
    return spi_base->BAUDR;
}

uint32_t dw_spi_get_data_frame_len(dw_spi_regs_t *spi_base)
{
    uint32_t len = spi_base->CTRLR0 & DW_SPI_CTRLR0_DFS_Msk;
    len >>= DW_SPI_CTRLR0_DFS_Pos;
    len++;
    return len;
}

void dw_spi_config_data_frame_len(dw_spi_regs_t *spi_base, uint32_t size)
{
    uint32_t temp;

    if ((size >= 4U) & (size <= 16U)) {
        temp = spi_base->CTRLR0;
        temp &= ~DW_SPI_CTRLR0_DFS_Msk;
        temp |= ((size - 1U) << DW_SPI_CTRLR0_DFS_Pos);
        spi_base->CTRLR0 = temp;
    }
}

void dw_spi_reset_regs(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0   = 7U;
    spi_base->CTRLR1   = 0U;
    spi_base->SSIENR   = 0U;
    spi_base->SER      = 0U;
    spi_base->BAUDR    = 0U;
    spi_base->TXFTLR   = 0x10U;
    spi_base->RXFTLR   = 0x10U;
    spi_base->IMR      = 0x00U;
    spi_base->SPIMSSEL = 0x1U;
}
