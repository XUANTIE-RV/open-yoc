/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_dma.c
 * @brief    CSI Source File for uart Driver
 * @version  V1.0
 * @date     16. Mar 2020
 * @vendor   csky
 * @name     dw_dma
 * @ip_id    0x111000010
 * @model    dma
 * @tag      DEV_DW_DMA_TAG
 ******************************************************************************/

#include <drv/dma.h>
#include <drv/irq.h>
#include <csi_core.h>
#include "dw_dma_ll.h"

#define DMA_REG_SARx            0x0
#define DMA_REG_DARx            0x8
#define DMA_REG_LLPX            0x10
#define DMA_REG_CTRLax          0x18
#define DMA_REG_CTRLbx          0x1c
#define DMA_REG_CFGax           0x40
#define DMA_REG_CFGbx           0x44

#define DMA_REG_RawTfr          0x2c0
#define DMA_REG_RawBlock        0x2c8
#define DMA_REG_RawSrcTran      0x2d0
#define DMA_REG_RawDstTran      0x2d8
#define DMA_REG_RawErr          0x2e0

#define DMA_REG_StatusTfr       0x2e8
#define DMA_REG_StatusBlock     0x2f0
#define DMA_REG_StatusSrcTran   0x2f8
#define DMA_REG_StatusDstTran   0x300
#define DMA_REG_StatusErr       0x308

#define DMA_REG_MaskTfr         0x310
#define DMA_REG_MaskBlock       0x318
#define DMA_REG_MaskSrcTran     0x320
#define DMA_REG_MaskDstTran     0x328
#define DMA_REG_MaskErr         0x330

#define DMA_REG_ClearTfr        0x338
#define DMA_REG_ClearBlock      0x340
#define DMA_REG_ClearSrcTran    0x348
#define DMA_REG_ClearDstTran    0x350
#define DMA_REG_ClearErr        0x358
#define DMA_REG_StatusInt       0x360

#define DMA_REG_ReqSrc          0x368
#define DMA_REG_ReqDst          0x370
#define DMA_REG_SglReqSrc       0x378
#define DMA_REG_SglReqDst       0x380
#define DMA_REG_LstReqSrc       0x388
#define DMA_REG_LstReqDst       0x390

#define DMA_REG_Cfg             0x398
#define DMA_REG_ChEn            0x3a0

#define CK_DMA_INT_EN      1
#define CK_DMA_CH_EN       1
#define CK_DMA_MAXCHANNEL  2
#define DMA_REG_CTRLax          0x18
#define CONFIG_DMA_CHANNEL_NUM  8
#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

#ifdef CONFIG_CHIP_PANGU
extern int32_t dma_mux_channel_alloc(dw_dma_hf_t func);
extern int32_t dma_mux_channel_release(dw_dma_hf_t func);
#endif

#define  REGS_OFFSET  0x58U
extern uint8_t g_dma_chnum[];
csi_dma_t *dma_array[2] = {0};
static uint32_t inited_ctrl_num = 0U;

void dw_dma_set_mask_dma(dw_dma_regs_t *dma_addr, uint8_t ch_id, uint64_t  value)
{
    switch (ch_id) {
        case 0U:
            dw_dma_set_masktfr_dma0(dma_addr, value);
            dw_dma_set_maskblock_dma0(dma_addr, value);
            dw_dma_set_maskerr_dma0(dma_addr, value);
            break;

        case 1U:
            dw_dma_set_masktfr_dma1(dma_addr, value);
            dw_dma_set_maskblock_dma1(dma_addr, value);
            dw_dma_set_maskerr_dma1(dma_addr, value);
            break;

        default:
            break;
    }

}
void dw_dma_reset_mask_dma(dw_dma_regs_t *dma_addr, uint8_t ch_id, uint64_t  value)
{
    switch (ch_id) {
        case 0U:
            dw_dma_reset_masktfr_dma0(dma_addr, value);
            dw_dma_reset_maskblock_dma0(dma_addr, value);
            dw_dma_reset_masksrctran_dma0(dma_addr, value);
            dw_dma_reset_maskdsttran_dma0(dma_addr, value);
            dw_dma_reset_maskerr_dma0(dma_addr, value);
            break;

        case 1U:
            dw_dma_reset_masktfr_dma1(dma_addr, value);
            dw_dma_reset_maskblock_dma1(dma_addr, value);
            dw_dma_reset_masksrctran_dma1(dma_addr, value);
            dw_dma_reset_maskdsttran_dma1(dma_addr, value);
            dw_dma_reset_maskerr_dma1(dma_addr, value);
            break;

        default:
            break;
    }

}

void dw_dma_clear_int_dma(dw_dma_regs_t *dma_addr, uint8_t ch_id, uint64_t  value)
{
    switch (ch_id) {
        case 0U:
            dw_dma_set_cleartfr_dma0(dma_addr, value);
            dw_dma_set_clearblock_dma0(dma_addr, value);
            dw_dma_set_clearsrctran_dma0(dma_addr, value);
            dw_dma_set_cleardsttran_dma0(dma_addr, value);
            dw_dma_set_clearerr_dma0(dma_addr, value);
            break;

        case 1U:
            dw_dma_set_cleartfr_dma1(dma_addr, value);
            dw_dma_set_clearblock_dma1(dma_addr, value);
            dw_dma_set_clearsrctran_dma1(dma_addr, value);
            dw_dma_set_cleardsttran_dma1(dma_addr, value);
            dw_dma_set_clearerr_dma1(dma_addr, value);
            break;

        default:
            break;
    }
}

void dw_dma_irq_handler(void *arg)
{
    csi_dma_t *dma = (csi_dma_t *)arg;
    dw_dma_regs_t *dma_addr = (dw_dma_regs_t *)dma->dev.reg_base;
    csi_dma_ch_t *dma_ch;
    uint32_t int_status, cb_state, ch_index;
    uint32_t (*func_tfr)(dw_dma_regs_t *dma);
    uint32_t (*func_err)(dw_dma_regs_t *dma);

    slist_for_each_entry(&dma->head, dma_ch, csi_dma_ch_t, next) {
        if (dma_ch->ctrl_id == 0) {
            func_tfr = dw_dma_get_statusblock_dma0;
            func_err = dw_dma_get_statuserr_dma0;
            ch_index = 0U;
        } else if (dma_ch->ctrl_id == 1) {
            func_tfr = dw_dma_get_statusblock_dma1;
            func_err = dw_dma_get_statuserr_dma1;
            ch_index = 1U;
        } else {
            break;
        }


        int_status = 0;
        int_status = func_tfr(dma_addr);
        cb_state = 0;

        if ((int_status & (1U << dma_ch->ch_id)) != 0) {
            cb_state = DMA_EVENT_TRANSFER_DONE;
        } else {
            int_status = func_err(dma_addr);

            if ((int_status & (1U << dma_ch->ch_id)) != 0U) {
                cb_state = DMA_EVENT_TRANSFER_ERROR;

            }
        }

        dw_dma_clear_int_dma(dma_addr, ch_index, (1U << dma_ch->ch_id));

        if ((1U << dma_ch->ch_id) & int_status) {
            if (dma_ch->callback != NULL) {
                dw_dma_clear_int_dma(dma_addr, ch_index, (1U << dma_ch->ch_id));
                dma_ch->callback(dma_ch, cb_state, dma_ch->arg);
            }
        }

    }
}

csi_error_t csi_dma_init(csi_dma_t *dma, int8_t ctrl_id)
{
    CSI_PARAM_CHK(dma, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    dw_dma_regs_t *dma_addr;
    ret = target_get(DEV_DW_DMA_TAG, (uint32_t) ctrl_id, &dma->dev);

    if (ret == CSI_OK) {

        slist_init(&dma->head);
        dma->alloc_status = 0U;
        dma_array[ctrl_id] = dma;
        dma_array[ctrl_id]->ch_num = g_dma_chnum[ctrl_id];
        dma_addr = (dw_dma_regs_t *)(dma_array[ctrl_id]->dev.reg_base);

        if (ctrl_id == 0) {
            uint32_t addr = dma_array[ctrl_id]->dev.reg_base;
            writel(0xf, addr + DMA_REG_ClearTfr);
            writel(0xf, addr + DMA_REG_ClearBlock);
            writel(0xf, addr + DMA_REG_ClearErr);
            writel(0xf, addr + DMA_REG_ClearSrcTran);
            writel(0xf, addr + DMA_REG_ClearDstTran);
        } else if (ctrl_id == 1) {
            uint32_t addr = dma_array[ctrl_id]->dev.reg_base;
            writel(0xff, addr + DMA_REG_ClearTfr);
            writel(0xff, addr + DMA_REG_ClearBlock);
            writel(0xff, addr + DMA_REG_ClearErr);
            writel(0xff, addr + DMA_REG_ClearSrcTran);
            writel(0xff, addr + DMA_REG_ClearDstTran);
        }


        csi_irq_attach((uint32_t)dma->dev.irq_num, &dw_dma_irq_handler, &dma->dev);
        csi_irq_enable((uint32_t)dma->dev.irq_num);

        ///<enable dma module
        if (ctrl_id == 0) {
            for (uint32_t i = 0; i < 4; i++) {
                dw_dma_reset_channel_en(dma_addr, i);
            }
        } else if (ctrl_id == 1) {
            for (uint32_t i = 0; i < 8; i++) {
                dw_dma_reset_channel_en(dma_addr, i);
            }
        }

        dw_dma_en(dma_addr);
        uint32_t result = csi_irq_save();
        inited_ctrl_num++;
        csi_irq_restore(result);
    }

    return ret;
}

void csi_dma_uninit(csi_dma_t *dma)
{
    CSI_PARAM_CHK_NORETVAL(dma);
    dw_dma_regs_t *dma_addr;
    slist_init(&dma->head);
    dma->alloc_status = 0U;
    dma_addr = (dw_dma_regs_t *)(dma_array[dma->dev.idx]->dev.reg_base);
    dma_array[dma->dev.idx] = NULL;
    csi_irq_disable((uint32_t)dma->dev.irq_num);
    csi_irq_detach((uint32_t)dma->dev.irq_num);
    dw_dma_dis(dma_addr);
    uint32_t result = csi_irq_save();
    inited_ctrl_num--;
    csi_irq_restore(result);
}

csi_error_t csi_dma_ch_alloc(csi_dma_ch_t *dma_ch, int8_t ch_id, int8_t ctrl_id)
{
    CSI_PARAM_CHK(dma_ch, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    csi_dma_ch_desc_t ch_info = {0};
    csi_dev_t *dev_info;
    dev_info = (csi_dev_t *)(dma_ch->parent);
    uint32_t result = csi_irq_save();

    if ((ctrl_id == -1) && (ch_id == -1)) {
        ret = target_get_optimal_dma_channel(dma_array, inited_ctrl_num, dev_info, &ch_info);

        if (ret == CSI_OK) {
            dma_array[ch_info.ctrl_idx]->alloc_status |= (uint32_t)((uint32_t)1U << (uint32_t)ch_info.ch_idx);

            dma_ch->ch_id = ch_info.ch_idx;
            dma_ch->ctrl_id = ch_info.ctrl_idx;
        }
    } else if ((ctrl_id >= 0) && (ch_id >= 0)) {
        if (dma_array[ctrl_id]->alloc_status & (uint32_t)((uint32_t)1U << (uint32_t)ch_id)) {
            ret = CSI_ERROR;
        } else {
            dma_array[ctrl_id]->alloc_status |= (uint32_t)((uint32_t)1U << (uint32_t)ch_id);
            dma_ch->ch_id = ch_id;
            dma_ch->ctrl_id = ctrl_id;
            uint32_t addr = dma_array[dma_ch->ctrl_id]->dev.reg_base;
            writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearTfr);
            writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearBlock);
            writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearErr);
            writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearSrcTran);
            writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearDstTran);

            uint32_t value = 1 << dma_ch->ch_id | (1 << (dma_ch->ch_id + 8));

            writel(value, addr + DMA_REG_MaskTfr);
            writel(value, addr + DMA_REG_MaskBlock);
            writel(value, addr + DMA_REG_MaskErr);

            dw_dma_regs_t *dma_addr;
            dma_addr = (dw_dma_regs_t *)(dma_array[dma_ch->ctrl_id]->dev.reg_base);
            dw_dma_reset_channel_en(dma_addr, (uint64_t)dma_ch->ch_id);
        }
    } else {
        ret = CSI_ERROR;
    }

    if (ret == CSI_ERROR) {
        csi_irq_restore(result);

    } else {
        slist_add(&dma_ch->next, &dma_array[dma_ch->ctrl_id]->head);
        csi_irq_restore(result);
    }

    dma_ch->etb_ch_id = -1;

    return ret;
}

void csi_dma_ch_free(csi_dma_ch_t *dma_ch)
{
    CSI_PARAM_CHK_NORETVAL(dma_ch);
    uint32_t result = csi_irq_save();
#ifdef CONFIG_CHIP_PANGU
    dma_mux_channel_release(dma_ch->etb_ch_id);
#endif
    uint32_t temp_u32;
    temp_u32 = 1U << (uint8_t)(dma_ch->ch_id);

    if (dma_array[dma_ch->ctrl_id]->alloc_status & temp_u32) {
        dma_array[dma_ch->ctrl_id]->alloc_status &= ~((uint32_t)1 << (uint32_t)dma_ch->ch_id);

        uint32_t addr = dma_array[dma_ch->ctrl_id]->dev.reg_base;
        writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearTfr);
        writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearBlock);
        writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearErr);
        writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearSrcTran);
        writel((1 << dma_ch->ch_id), addr + DMA_REG_ClearDstTran);

        uint32_t value = (1 << (dma_ch->ch_id + 8));
        writel(value, addr + DMA_REG_MaskTfr);
        writel(value, addr + DMA_REG_MaskErr);

        dw_dma_regs_t *dma_addr;
        dma_addr = (dw_dma_regs_t *)(dma_array[dma_ch->ctrl_id]->dev.reg_base);
        dw_dma_reset_channel_en(dma_addr, (uint64_t)dma_ch->ch_id);

        slist_del(&dma_ch->next, &dma_array[dma_ch->ctrl_id]->head);
    }

    csi_irq_restore(result);
}

csi_error_t csi_dma_ch_config(csi_dma_ch_t *dma_ch, csi_dma_ch_config_t *config)
{
    CSI_PARAM_CHK(dma_ch, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    uint32_t temp;
    uint16_t group_len = 0;
    temp = ((uint32_t)(dma_ch->ch_id) * REGS_OFFSET) + dma_array[dma_ch->ctrl_id]->dev.reg_base;
    dw_dma_ch_regs_t *dma_ch_addr = (dw_dma_ch_regs_t *)(temp);
    dw_dma_dis_int(dma_ch_addr);

    /* Initializes corresponding channel registers */

    switch (config->dst_tw) {
        case DMA_DATA_WIDTH_8_BITS:
            dw_dma_set_dst_transfer_width(dma_ch_addr, DW_DMA_CTL_DST_TR_WIDTH_8);
            break;

        case DMA_DATA_WIDTH_16_BITS:
            dw_dma_set_dst_transfer_width(dma_ch_addr, DW_DMA_CTL_DST_TR_WIDTH_16);
            break;

        case DMA_DATA_WIDTH_32_BITS:
            dw_dma_set_dst_transfer_width(dma_ch_addr, DW_DMA_CTL_DST_TR_WIDTH_32);
            break;

        default:
            break;
    }

    switch (config->src_tw) {
        case DMA_DATA_WIDTH_8_BITS:
            dw_dma_set_src_transfer_width(dma_ch_addr, DW_DMA_CTL_SRT_TR_WIDTH_8);
            break;

        case DMA_DATA_WIDTH_16_BITS:
            dw_dma_set_src_transfer_width(dma_ch_addr, DW_DMA_CTL_SRT_TR_WIDTH_16);
            break;

        case DMA_DATA_WIDTH_32_BITS:
            dw_dma_set_src_transfer_width(dma_ch_addr, DW_DMA_CTL_SRT_TR_WIDTH_32);
            break;

        default:
            break;
    }

    switch (config->src_inc) {
        case  DMA_ADDR_INC  :
            dw_dma_set_sinc(dma_ch_addr, DW_DMA_CTL_SINC_INC);
            break;

        case  DMA_ADDR_DEC:
            dw_dma_set_sinc(dma_ch_addr, DW_DMA_CTL_SINC_DINC);
            break;

        case  DMA_ADDR_CONSTANT:
            dw_dma_set_sinc(dma_ch_addr, DW_DMA_CTL_SINC_NOCHANAGE);
            break;

        default:
            break;
    }

    switch (config->dst_inc) {
        case  DMA_ADDR_INC  :
            dw_dma_set_dinc(dma_ch_addr, DW_DMA_CTL_DINC_INC);
            break;

        case  DMA_ADDR_DEC:
            dw_dma_set_dinc(dma_ch_addr, DW_DMA_CTL_DINC_DINC);
            break;

        case  DMA_ADDR_CONSTANT:
            dw_dma_set_dinc(dma_ch_addr, DW_DMA_CTL_DINC_NOCHANAGE);
            break;

        default:
            break;
    }

    switch (dw_dma_get_dst_transfer_width(dma_ch_addr)) {
        case DW_DMA_CTL_DST_TR_WIDTH_8 :
            break;

        case DW_DMA_CTL_DST_TR_WIDTH_16:
            if (config->group_len % 2 != 0) {
                ret = CSI_ERROR;
            }

            group_len = config->group_len / 2;
            break;

        case DW_DMA_CTL_DST_TR_WIDTH_32:
            if (config->group_len % 4 != 0) {
                ret = CSI_ERROR;
            }

            group_len = config->group_len / 4;
            break;

        default:
            break;
    }

    switch (group_len) {
        case 1U:
            dw_dma_set_dest_burst_size(dma_ch_addr, DW_DMA_CTL_DEST_MSIZE_1);
            dw_dma_set_src_burst_size(dma_ch_addr, DW_DMA_CTL_SRC_MSIZE_1);
            break;

        case 4:
            dw_dma_set_dest_burst_size(dma_ch_addr, DW_DMA_CTL_DEST_MSIZE_4);
            dw_dma_set_src_burst_size(dma_ch_addr, DW_DMA_CTL_SRC_MSIZE_4);
            break;

        case 8:
            dw_dma_set_dest_burst_size(dma_ch_addr, DW_DMA_CTL_DEST_MSIZE_8);
            dw_dma_set_src_burst_size(dma_ch_addr, DW_DMA_CTL_SRC_MSIZE_8);
            break;

        case 16:
            dw_dma_set_dest_burst_size(dma_ch_addr, DW_DMA_CTL_DEST_MSIZE_16);
            dw_dma_set_src_burst_size(dma_ch_addr, DW_DMA_CTL_SRC_MSIZE_16);
            break;

        case 32:
            dw_dma_set_dest_burst_size(dma_ch_addr, DW_DMA_CTL_DEST_MSIZE_32);
            dw_dma_set_src_burst_size(dma_ch_addr, DW_DMA_CTL_SRC_MSIZE_32);
            break;

        case 64:
            dw_dma_set_dest_burst_size(dma_ch_addr, DW_DMA_CTL_DEST_MSIZE_64);
            dw_dma_set_src_burst_size(dma_ch_addr, DW_DMA_CTL_SRC_MSIZE_64);
            break;

        case 128:
            dw_dma_set_dest_burst_size(dma_ch_addr, DW_DMA_CTL_DEST_MSIZE_128);
            dw_dma_set_src_burst_size(dma_ch_addr, DW_DMA_CTL_SRC_MSIZE_128);
            break;

        default:
            dw_dma_set_dest_burst_size(dma_ch_addr, DW_DMA_CTL_DEST_MSIZE_1);
            dw_dma_set_src_burst_size(dma_ch_addr, DW_DMA_CTL_SRC_MSIZE_1);
            break;
    }

    int64_t hs_if = -1;
#ifdef CONFIG_CHIP_PANGU

    if ((config->trans_dir != DMA_MEM2MEM) && (config->handshake > DMA_HF_UART1_RX)) {
        if (dma_ch->etb_ch_id != -1) {
            dma_mux_channel_release(dma_ch->etb_ch_id);
        }

        hs_if = dma_mux_channel_alloc(config->handshake);

        if (hs_if < 0) {
            return -1;
        }

        dma_ch->etb_ch_id = config->handshake;
    } else {
        hs_if = config->handshake;
    }


#endif

    /* set little endian */
    ///<not support
    switch (config->trans_dir) {
        case DMA_MEM2MEM:
            dw_dma_set_transfer_type(dma_ch_addr, DW_DMA_CTL_TT_FC_M2M);
            dw_dma_dst_hs_sft(dma_ch_addr);
            dw_dma_dst_hs_sft(dma_ch_addr);
            break;

        case DMA_MEM2PERH:
            dw_dma_set_transfer_type(dma_ch_addr, DW_DMA_CTL_TT_FC_M2P);
            dw_dma_dst_hs_hw(dma_ch_addr);
            dw_dma_src_hs_sft(dma_ch_addr);
            dw_dma_set_max_abrst(dma_ch_addr, 0U);
            dw_dma_set_src_per(dma_ch_addr, 0U);
            dw_dma_set_dest_per(dma_ch_addr, (uint64_t)hs_if);
            break;

        case DMA_PERH2MEM:
            dw_dma_set_transfer_type(dma_ch_addr, DW_DMA_CTL_TT_FC_P2M);
            dw_dma_dst_hs_sft(dma_ch_addr);
            dw_dma_src_hs_hw(dma_ch_addr);
            dw_dma_set_max_abrst(dma_ch_addr, 0U);
            dw_dma_set_src_per(dma_ch_addr, (uint64_t)hs_if);
            dw_dma_set_dest_per(dma_ch_addr, 0U);
            break;

        default:
            break;
    }

    if (config->src_reload_en == 1) {
        dw_dma_en_reload_src(dma_ch_addr);
    } else {
        dw_dma_dis_reload_src(dma_ch_addr);
    }

    if (config->dst_reload_en == 1) {
        dw_dma_en_reload_dst(dma_ch_addr);
    } else {
        dw_dma_dis_reload_dst(dma_ch_addr);
    }

    ///<Don't suspend dma
    //dw_dma_dis_ch_susp(dma_ch_addr);

    return ret;
}

csi_error_t csi_dma_ch_attach_callback(csi_dma_ch_t *dma_ch, void *callback, void *arg)
{
    csi_error_t ret = CSI_OK;
    CSI_PARAM_CHK(dma_ch, CSI_ERROR);
    dw_dma_regs_t *dma_addr;
    dma_ch->callback = callback;
    dma_ch->arg = arg;
    dma_addr = (dw_dma_regs_t *)(dma_array[dma_ch->ctrl_id]->dev.reg_base);
    dw_dma_set_mask_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, (uint64_t)(DW_DMA_MaskTfr_DMA0_En_CH0 << (uint32_t)dma_ch->ch_id));

    if (dma_ch->ctrl_id == 0) {
        dw_dma_clear_int_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, DW_DMA_MaskTfr_DMA0_Msk);
    } else if (dma_ch->ctrl_id == 1) {
        dw_dma_clear_int_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, DW_DMA_MaskTfr_DMA1_Msk);
    }

    return ret;
}

void csi_dma_ch_detach_callback(csi_dma_ch_t *dma_ch)
{
    dw_dma_regs_t *dma_addr;
    CSI_PARAM_CHK_NORETVAL(dma_ch);
    uint32_t temp;
    temp = ((uint32_t)(dma_ch->ch_id) * REGS_OFFSET) + dma_array[dma_ch->ctrl_id]->dev.reg_base;
    dw_dma_ch_regs_t *dma_ch_addr = (dw_dma_ch_regs_t *)(temp);
    dma_ch->callback = NULL;
    dma_ch->arg = NULL;

    dma_addr = (dw_dma_regs_t *)(dma_array[dma_ch->ctrl_id]->dev.reg_base);
    dw_dma_reset_mask_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, (uint8_t)dma_ch->ch_id);
    dw_dma_clear_int_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, DW_DMA_MaskTfr_DMA0_Msk);

    if (dma_ch->ctrl_id == 0) {
        dw_dma_clear_int_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, DW_DMA_MaskTfr_DMA0_Msk);
    } else if (dma_ch->ctrl_id == 1) {
        dw_dma_clear_int_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, DW_DMA_MaskTfr_DMA1_Msk);
    }

    dw_dma_dis_int(dma_ch_addr);

}
void delay_x(uint32_t value)
{
    for (uint32_t i = 0U; i < value; i++) {
        printf("%d", i);
    }
}

void csi_dma_ch_start(csi_dma_ch_t *dma_ch, void *srcaddr, void *dstaddr, uint32_t length)
{
    dw_dma_regs_t *dma_addr;
    CSI_PARAM_CHK_NORETVAL(dma_ch);
    uint32_t temp, temp1, temp2;
    temp = ((uint32_t)(dma_ch->ch_id) * REGS_OFFSET) + dma_array[dma_ch->ctrl_id]->dev.reg_base;
    dw_dma_ch_regs_t *dma_ch_addr = (dw_dma_ch_regs_t *)(temp);
    dma_addr = (dw_dma_regs_t *)(dma_array[dma_ch->ctrl_id]->dev.reg_base);
    dw_dma_reset_channel_en(dma_addr, (uint32_t)dma_ch->ch_id);

    switch (dw_dma_get_dst_transfer_width(dma_ch_addr)) {
        case DW_DMA_CTL_DST_TR_WIDTH_8 :
            break;

        case DW_DMA_CTL_DST_TR_WIDTH_16:
            length >>= 1;
            break;

        case DW_DMA_CTL_DST_TR_WIDTH_32:
            length >>= 2;
            break;

        default:
            break;
    }

    dw_dma_set_transfer_size(dma_ch_addr, (uint64_t)length);
    temp1 = (uint32_t)srcaddr;
    temp2 = (uint32_t)dstaddr;

    if (srcaddr != NULL) {
        dw_dma_set_sar(dma_ch_addr, (uint64_t)temp1);
    }

    if (dstaddr != NULL) {
        dw_dma_set_dar(dma_ch_addr, (uint64_t)temp2);
    }

    ///<enable channel
    if (dma_ch->ctrl_id == 0) {
        dw_dma_clear_int_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, DW_DMA_MaskTfr_DMA0_Msk);
    } else if (dma_ch->ctrl_id == 1) {
        dw_dma_clear_int_dma(dma_addr, (uint8_t)dma_ch->ctrl_id, DW_DMA_MaskTfr_DMA1_Msk);
    }

    dw_dma_en_int(dma_ch_addr);
    csi_dcache_clean_invalid();
    dw_dma_set_channel_en(dma_addr, (uint32_t)dma_ch->ch_id);

}

// void csi_dma_ch_stop(csi_dma_ch_t *dma_ch)
// {
//     dw_dma_regs_t *dma_addr;
//     uint32_t temp;
//     CSI_PARAM_CHK_NORETVAL(dma_ch);
//     temp = ((uint32_t)(dma_ch->ch_id) * REGS_OFFSET) + dma_array[dma_ch->ctrl_id]->dev.reg_base;
//     dw_dma_ch_regs_t *dma_ch_addr = (dw_dma_ch_regs_t *)(temp);

//     dma_addr = (dw_dma_regs_t *)(dma_array[dma_ch->ctrl_id]->dev.reg_base);
//     dw_dma_reset_channel_en(dma_addr, (uint32_t)(dma_ch->ch_id));
//     dw_dma_dis_int(dma_ch_addr);
// }

void csi_dma_ch_stop(csi_dma_ch_t *dma_ch)
{
    int8_t ch = dma_ch->ch_id;

    if (ch >= CONFIG_DMA_CHANNEL_NUM || ch < 0) {
        return;
    }

    uint32_t addr = dma_array[dma_ch->ctrl_id]->dev.reg_base;
    uint32_t prer_dmac_ch_num = 0;

    if (dma_ch->ctrl_id == 0) {
        prer_dmac_ch_num = 4;
    } else if (dma_ch->ctrl_id == 1) {
        prer_dmac_ch_num = 8;
    }

    uint32_t value = readl(addr + DMA_REG_ChEn);
    value |= (CK_DMA_CH_EN << (8 + (ch % prer_dmac_ch_num)));
    value &= ~(CK_DMA_CH_EN << (ch % prer_dmac_ch_num));
    writel(value, addr + DMA_REG_ChEn);
    // interrupt disable
    value = readl(addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
    value &= ~CK_DMA_INT_EN;
    writel(value, addr + (ch % prer_dmac_ch_num) * 0x58 + DMA_REG_CTRLax);
}
