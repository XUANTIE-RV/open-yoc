/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_dma_ll.c
 * @brief    wj dma ll driver
 * @version  V1.0
 * @date     27. December 2019
 ******************************************************************************/

#include <wj_dma_ll.h>

void wj_dma_set_transferwidth(wj_dma_ch_regs_t *addr, uint32_t src_width, uint32_t dst_width)
{
    addr->CH_CTRLA &= ~(WJ_DMA_CH_CTRLA_DST_TR_WIDTH_Msk);
    addr->CH_CTRLA |= (dst_width << WJ_DMA_CH_CTRLA_DST_TR_WIDTH_Pos);
    addr->CH_CTRLA &= ~(WJ_DMA_CH_CTRLA_SRC_TR_WIDTH_Msk);
    addr->CH_CTRLA |= (src_width << WJ_DMA_CH_CTRLA_SRC_TR_WIDTH_Pos);
}

void wj_dma_set_addrinc(wj_dma_ch_regs_t *addr, uint32_t src_addrinc, uint32_t dst_addrinc)
{
    addr->CH_CTRLA &= ~(WJ_DMA_CH_CTRLA_DINC_Msk);
    addr->CH_CTRLA |= (dst_addrinc << WJ_DMA_CH_CTRLA_DINC_Pos);
    addr->CH_CTRLA &= ~(WJ_DMA_CH_CTRLA_SINC_Msk);
    addr->CH_CTRLA |= (src_addrinc << WJ_DMA_CH_CTRLA_SINC_Pos);
}

void wj_dma_set_addr_endian(wj_dma_ch_regs_t *addr, uint32_t src_endian, uint32_t dst_endian)
{
    addr->CH_CTRLB &= ~(WJ_DMA_CH_CTRLB_SRCDTLGC_Msk);
    addr->CH_CTRLB |= (src_endian << WJ_DMA_CH_CTRLB_SRCDTLGC_Pos);
    addr->CH_CTRLB &= ~(WJ_DMA_CH_CTRLB_DSTDTLGC_Msk);
    addr->CH_CTRLB |= (dst_endian << WJ_DMA_CH_CTRLB_DSTDTLGC_Pos);
}

void wj_dma_trans_mode_set(wj_dma_ch_regs_t *addr, uint32_t mode)
{
    addr->CH_CTRLB &= ~(WJ_DMA_CH_CTRLB_TRGTMDC_Msk);
    addr->CH_CTRLB |= (mode << WJ_DMA_CH_CTRLB_TRGTMDC_Pos);
}

void wj_dma_set_singlemode(wj_dma_ch_regs_t *addr, uint32_t preemption, uint32_t single_dir)
{
    addr->CH_CTRLA &= ~(uint32_t)(WJ_DMA_CH_CTRLA_SGMDC_Msk);
    addr->CH_CTRLA |= (uint32_t)(WJ_DMA_CH_CTRLA_SGMDC_DST_SG_TRIG_MODE << WJ_DMA_CH_CTRLA_SGMDC_Pos);
    addr->CH_CTRLA |= (uint32_t)(WJ_DMA_CH_CTRLA_SRCSGLINTE_SG_READ_EN << WJ_DMA_CH_CTRLA_SRCSGLINTE_Pos);

    if (preemption == 0U) {
        if (single_dir == 1U) {
            addr->CH_CTRLA &= ~(uint32_t)(WJ_DMA_CH_CTRLA_SRCSGLINTE_Msk);
            addr->CH_CTRLA |= (uint32_t)(WJ_DMA_CH_CTRLA_SRCSGLINTE_SG_READ_DIS << WJ_DMA_CH_CTRLA_SRCSGLINTE_Pos);
        } else {
            addr->CH_CTRLA &= ~(uint32_t)(WJ_DMA_CH_CTRLA_DSTSGLINTE_Msk);
            addr->CH_CTRLA |= (uint32_t)(WJ_DMA_CH_CTRLA_DSTSGLINTE_SG_WRITE_DIS << WJ_DMA_CH_CTRLA_DSTSGLINTE_Pos);
        }
    } else if (preemption == 1U) {
        if (single_dir == 0U) {
            addr->CH_CTRLA |= (uint32_t)(WJ_DMA_CH_CTRLA_SRCSGLINTE_SG_READ_EN << WJ_DMA_CH_CTRLA_SRCSGLINTE_Pos);
        } else {
            addr->CH_CTRLA |= (uint32_t)(WJ_DMA_CH_CTRLA_DSTSGLINTE_SG_WRITE_EN << WJ_DMA_CH_CTRLA_DSTSGLINTE_Pos);
        }
    }
}

void wj_dma_set_groupmode(wj_dma_ch_regs_t *addr, uint32_t src_inc, uint32_t dst_inc, uint32_t preemption, uint8_t group_len)
{
    if (preemption == 0U) {
        addr->CH_CTRLA &= ~(uint32_t)(WJ_DMA_CH_CTRLA_GRPINTE_Msk);
        addr->CH_CTRLA |= (uint32_t)(WJ_DMA_CH_CTRLA_GRPINTE_GP_COMPLETE_DIS << WJ_DMA_CH_CTRLA_GRPINTE_Pos);
    } else if (preemption == 1U) {
        //Channel interrupt by high priority channel mode Control
        addr->CH_CTRLA &= ~(uint32_t)(WJ_DMA_CH_CTRLA_GRPINTE_Msk);
        addr->CH_CTRLA |= (uint32_t)(WJ_DMA_CH_CTRLA_GRPINTE_GP_COMPLETE_EN << WJ_DMA_CH_CTRLA_GRPINTE_Pos);
    }

    addr->CH_CTRLA &= ~(uint32_t)(WJ_DMA_CH_CTRLA_GROUP_LEN_Msk);
    addr->CH_CTRLA |= (uint32_t)(((uint32_t)group_len - 1U) << WJ_DMA_CH_CTRLA_GROUP_LEN_Pos);
}

uint32_t get_power(uint32_t base, uint32_t power)
{
    uint32_t ret, i;
    ret =  1U;

    for (i = 0U; i < power; i++) {
        ret *= base;
    }

    return ret;
}
void wj_dma_get_groupmode(wj_dma_ch_regs_t *addr, uint32_t *src_inc, uint32_t *dst_inc, uint8_t *group_len)
{
    uint32_t  tmp = addr->CH_CTRLA;

    *src_inc = get_power(2U, (tmp & 0xcU) >> 2U);
    *dst_inc = get_power(2U, tmp & 0x3U);
    *group_len = (uint8_t)(((tmp & 0xf00U) >> 8U) + 1U);
}
void wj_dma_set_int_mask_tfr(wj_dma_ch_regs_t *addr, uint32_t mask)
{
    if (mask == 0U) {
        addr->CH_INTM |= WJ_DMA_CH_INTM_HTFR;
    } else if (mask == 1U) {
        addr->CH_INTM &= ~(WJ_DMA_CH_INTM_HTFR);
    }
}

void wj_dma_set_int_clear_tfr(wj_dma_ch_regs_t *addr, uint32_t clear)
{
    if (clear == 1U) {
        addr->CH_INTC |= (uint32_t)(WJ_DMA_CH_INTC_HTFR << WJ_DMA_CH_INTC_HTFR_Pos);
    } else {
        addr->CH_INTC &= ~(uint32_t)(WJ_DMA_CH_INTC_HTFR << WJ_DMA_CH_INTC_HTFR_Pos);
    }
}


void wj_dma_set_total_size(wj_dma_ch_regs_t *addr, uint32_t len)
{
    addr->CH_CTRLA &= ~(uint32_t)(WJ_DMA_CH_CTRLA_BLOCK_TL_Msk);
    addr->CH_CTRLA |= (uint32_t)((len - 1U) << WJ_DMA_CH_CTRLA_BLOCK_TL_Pos);
}

void wj_dma_set_channel_addr(wj_dma_ch_regs_t *addr, uint32_t src, uint32_t dst)
{
    addr->SAR = src;
    addr->DAR = dst;
}
