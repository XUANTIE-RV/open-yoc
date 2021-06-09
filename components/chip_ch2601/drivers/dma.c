/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_dma.c
 * @brief    CSI Source File for uart Driver
 * @version  V2.01
 * @date     27. December 2019
 * @vendor   csky
 * @name     wj_dma
 * @ip_id    0x111000010
 * @model    dma
 * @tag      DEV_WJ_DMA_TAG
 ******************************************************************************/

#include <csi_core.h>
#include <wj_dma_ll.h>
#include <drv/common.h>
#include <drv/irq.h>
#include <drv/dma.h>
#include <drv/etb.h>

extern uint8_t g_dma_chnum[];
static csi_dma_t *dma_array[4];
static uint32_t inited_ctrl_num = 0U;

__attribute__((section(".ram.code"))) void wj_dma_irq_handler(void *arg)
{
    csi_dma_t *dma = (csi_dma_t *)arg;
    wj_dma_regs_t *dma_addr = (wj_dma_regs_t *)dma->dev.reg_base;
    wj_dma_ch_regs_t *dma_ch_addr;
    csi_dma_ch_t *dma_ch;
    uint32_t int_status;

   slist_for_each_entry(&dma->head, dma_ch, csi_dma_ch_t, next) {
        dma_ch_addr = &dma_addr->dma_array[dma_ch->ch_id];
        int_status = dma_get_int_status(dma_ch_addr);

        if (int_status != 0U) {
            if (dma_ch != NULL) {
                if (int_status & WJ_DMA_CH_INTS_ERR) {
                    dma_int_clear_status(dma_ch_addr, WJ_DMA_CH_INTC_ERR);

                    if (dma_ch->callback != NULL) {
                        dma_ch->callback(dma_ch, DMA_EVENT_TRANSFER_ERROR, dma_ch->arg);
                    }
                }

                if (int_status & WJ_DMA_CH_INTS_HTFR) {
                    dma_int_clear_status(dma_ch_addr, WJ_DMA_CH_INTC_HTFR);

                    if (dma_ch->callback != NULL) {
                        dma_ch->callback(dma_ch, DMA_EVENT_TRANSFER_HALF_DONE, dma_ch->arg);
                    }
                }

                if (int_status & WJ_DMA_CH_INTS_TFR) {
                    dma_int_clear_status(dma_ch_addr, WJ_DMA_CH_INTC_TFR);
                    dma_int_clear_status(dma_ch_addr, WJ_DMA_CH_INTC_TRGETCMPFR);

                    if (dma_ch->callback != NULL) {
                        dma_ch->callback(dma_ch, DMA_EVENT_TRANSFER_DONE, dma_ch->arg);
                    }
                }
            }
        }
    }
}

csi_error_t csi_dma_init(csi_dma_t *dma, int8_t ctrl_id)
{
    CSI_PARAM_CHK(dma, CSI_ERROR);
    uint32_t i, temp;
    csi_error_t ret = CSI_OK;
    wj_dma_regs_t *dma_addr;
    wj_dma_ch_regs_t *dma_ch_addr;
    ret = target_get(DEV_WJ_DMA_TAG, (uint32_t)ctrl_id, &dma->dev);

    if (ret != CSI_OK) {

        ret = CSI_ERROR;

    } else {

        slist_init(&dma->head);
        dma->alloc_status = 0U;
        dma_array[ctrl_id] = dma;
        dma_array[ctrl_id]->ch_num = g_dma_chnum[ctrl_id];
        dma_addr = (wj_dma_regs_t *)(dma_array[ctrl_id]->dev.reg_base);

        /* close channel intr */
        for (i = 0U; i < dma_array[ctrl_id]->ch_num; i++) {
            temp = (i * 0x30U) + dma_array[(uint32_t)ctrl_id]->dev.reg_base;
            dma_ch_addr = (wj_dma_ch_regs_t *)(temp);
            dma_set_ch_int_en(dma_ch_addr, 0U);
            dma_int_all_clear(dma_ch_addr);
            dma_set_ch_en(dma_ch_addr, 0U);
        }

        csi_irq_attach((uint32_t)(dma->dev.irq_num), &wj_dma_irq_handler, &dma->dev);
        csi_irq_enable((uint32_t)(dma->dev.irq_num));
        dma_set_en(dma_addr, WJ_DMA_CH_EN);
        uint32_t result = csi_irq_save();
        inited_ctrl_num++;
        csi_irq_restore(result);
    }

    return ret;
}

void csi_dma_uninit(csi_dma_t *dma)
{
    CSI_PARAM_CHK_NORETVAL(dma);
    wj_dma_regs_t *dma_addr;
    slist_init(&dma->head);
    dma->alloc_status = 0U;
    dma_addr = (wj_dma_regs_t *)(dma_array[dma->dev.idx]->dev.reg_base);
    dma_array[dma->dev.idx] = NULL;
    dma_set_en(dma_addr, 0U);
    csi_irq_disable((uint32_t)(dma->dev.irq_num));
    csi_irq_detach((uint32_t)(dma->dev.irq_num));
    uint32_t result = csi_irq_save();
    inited_ctrl_num--;
    csi_irq_restore(result);
}

csi_error_t csi_dma_ch_alloc(csi_dma_ch_t *dma_ch, int8_t ch_id, int8_t ctrl_id)
{
    CSI_PARAM_CHK(dma_ch, CSI_ERROR);
    uint32_t temp, ch_alloc_status;
    csi_error_t ret = CSI_OK;
    wj_dma_ch_regs_t *dma_ch_addr;
    csi_dma_ch_desc_t ch_info;
    csi_dev_t *dev_info;
    dev_info = (csi_dev_t *)(dma_ch->parent);
    ch_info.ch_idx = 0;
    ch_info.ctrl_idx = 0;
    uint32_t result = csi_irq_save();

    if ((ctrl_id == -1) && (ch_id == -1)) {
        ret = target_get_optimal_dma_channel(dma_array, inited_ctrl_num, dev_info, &ch_info);

        if (ret == CSI_OK) {
            ch_alloc_status = (1U << (uint8_t)ch_info.ch_idx);
            dma_array[ch_info.ctrl_idx]->alloc_status |= ch_alloc_status;
            temp = ((uint32_t)ch_info.ch_idx * 0x30U) + dma_array[(uint32_t)ch_info.ctrl_idx]->dev.reg_base;
            dma_ch_addr = (wj_dma_ch_regs_t *)(temp);
            dma_ch->ch_id = ch_info.ch_idx;
            dma_ch->ctrl_id = ch_info.ctrl_idx;
        } else {
            ret = CSI_ERROR;
        }
    } else if ((ctrl_id >= 0) && (ch_id >= 0)) {
        ch_alloc_status = (1U << (uint8_t)ch_id);
        temp = dma_array[ctrl_id]->alloc_status & ch_alloc_status;

        if (temp) {
            ret = CSI_ERROR;
        } else {
            ch_alloc_status = (1U << (uint8_t)ch_id);
            dma_array[ctrl_id]->alloc_status |= ch_alloc_status;
            temp = (uint32_t)((uint32_t)ch_id * 0x30U) + dma_array[ctrl_id]->dev.reg_base;
            dma_ch_addr = (wj_dma_ch_regs_t *)(temp);
            dma_ch->ch_id = ch_id;
            dma_ch->ctrl_id = ctrl_id;
        }
    } else {
        ret = CSI_ERROR;
    }

    if (ret != CSI_ERROR) {
        dma_ch->etb_ch_id = -1;
        slist_add(&dma_ch->next, &dma_array[dma_ch->ctrl_id]->head);
        dma_set_int_mask(dma_ch_addr, WJ_DMA_CH_INTM_ERR | WJ_DMA_CH_INTM_TFR | WJ_DMA_CH_INTM_PENDFR);
        dma_int_all_clear(dma_ch_addr);
    }

    csi_irq_restore(result);
    return ret;
}

void csi_dma_ch_free(csi_dma_ch_t *dma_ch)
{
    CSI_PARAM_CHK_NORETVAL(dma_ch);
    uint32_t result = csi_irq_save();
    uint32_t ch_alloc_status;

    ch_alloc_status = 1U << (uint8_t)(dma_ch->ch_id);

    if (dma_array[dma_ch->ctrl_id]->alloc_status & ch_alloc_status) {
        dma_array[dma_ch->ctrl_id]->alloc_status &= ~ch_alloc_status;
        slist_del(&dma_ch->next, &dma_array[dma_ch->ctrl_id]->head);

        if (dma_ch->etb_ch_id != -1) {
            csi_etb_ch_free((int32_t)(dma_ch->etb_ch_id));
        }
    }

    csi_irq_restore(result);
}

csi_error_t csi_dma_ch_config(csi_dma_ch_t *dma_ch, csi_dma_ch_config_t *config)
{
    CSI_PARAM_CHK(dma_ch, CSI_ERROR);
    int32_t etb_ch;
    csi_error_t ret = CSI_OK;
    csi_etb_config_t etb_config;
    uint32_t temp;
    temp = ((uint32_t)(dma_ch->ch_id) * 0x30U) + dma_array[dma_ch->ctrl_id]->dev.reg_base;
    wj_dma_ch_regs_t *dma_ch_addr = (wj_dma_ch_regs_t *)(temp);

    /* Initializes corresponding channel registers */
    wj_dma_set_transferwidth(dma_ch_addr, (uint32_t)(config->src_tw), (uint32_t)(config->dst_tw));
    wj_dma_set_addrinc(dma_ch_addr, (uint32_t)(config->src_inc), (uint32_t)(config->dst_inc));
    /* set little endian */
    wj_dma_set_addr_endian(dma_ch_addr, 0U, 0U);

    if (config->trans_dir == DMA_MEM2MEM) {
        wj_dma_trans_mode_set(dma_ch_addr, WJ_DMA_CH_CTRLB_TRGTMDC_BLOCK_TRIG_MODE);
        wj_dma_set_groupmode(dma_ch_addr, (uint32_t)(config->src_inc), (uint32_t)(config->dst_inc), 0U, (uint8_t)config->group_len);
    } else {
        /* set group mode */
        wj_dma_trans_mode_set(dma_ch_addr, WJ_DMA_CH_CTRLB_TRGTMDC_GROUP_TRIG_MODE);
        wj_dma_set_groupmode(dma_ch_addr, (uint32_t)(config->src_inc), (uint32_t)(config->dst_inc), 0U, (uint8_t)config->group_len);
    }

    if (config->half_int_en == 1U) {
        wj_dma_set_int_mask_tfr(dma_ch_addr, 0U);
        wj_dma_set_int_clear_tfr(dma_ch_addr, 1U);
    } else {
        wj_dma_set_int_mask_tfr(dma_ch_addr, 1U);
        wj_dma_set_int_clear_tfr(dma_ch_addr, 1U);
    }

    /* etb config */
    if ((dma_ch->etb_ch_id == -1) && (config->trans_dir != DMA_MEM2MEM)) {
        etb_ch = csi_etb_ch_alloc(ETB_CH_ONE_TRIGGER_ONE);

        if (etb_ch == CSI_ERROR) {
            ret = CSI_ERROR;
        }

        dma_ch->etb_ch_id = (int16_t)etb_ch;
    }

    if (config->trans_dir != DMA_MEM2MEM) {
        dma_set_ch_soft_req(dma_ch_addr, 0U);
        etb_config.src_ip = (uint8_t)(config->handshake);
        etb_config.dst_ip = (uint8_t)(dma_ch->ch_id);
        etb_config.ch_type = ETB_CH_ONE_TRIGGER_ONE;
        etb_config.trig_mode = ETB_HARDWARE_TRIG;
        ret = csi_etb_ch_config((int32_t)(dma_ch->etb_ch_id), &etb_config);
    }

    if (config->dst_reload_en) {
        dma_ch_addr->CH_CTRLA |= WJ_DMA_CH_CTRLA_DGRPADDRC_Msk;
    } else {
        dma_ch_addr->CH_CTRLA &= ~WJ_DMA_CH_CTRLA_DGRPADDRC_Msk;
    }

    if (config->src_reload_en) {
        dma_ch_addr->CH_CTRLA |= WJ_DMA_CH_CTRLA_SGRPADDRC_Msk;
    } else {
        dma_ch_addr->CH_CTRLA &= ~WJ_DMA_CH_CTRLA_SGRPADDRC_Msk;
    }

    return ret;
}

csi_error_t csi_dma_ch_attach_callback(csi_dma_ch_t *dma_ch, void *callback, void *arg)
{
    CSI_PARAM_CHK(dma_ch, CSI_ERROR);
    dma_ch->callback = callback;
    dma_ch->arg = arg;
    return CSI_OK;
}

void csi_dma_ch_detach_callback(csi_dma_ch_t *dma_ch)
{
    CSI_PARAM_CHK_NORETVAL(dma_ch);
    dma_ch->callback = NULL;
    dma_ch->arg = NULL;
}

int8_t  judge_valid(uint32_t src_l, uint32_t dest_l, uint8_t group_l, uint32_t block_l)
{
    int8_t  ret;

    if ((block_l % src_l) != 0U) {
        ret = -1;
    } else if ((block_l % dest_l) != 0U) {
        ret = -1;
    } else {
        ret = 0;
    }

    return ret;
}

void csi_dma_ch_start(csi_dma_ch_t *dma_ch, void *srcaddr, void *dstaddr, uint32_t length)
{
    CSI_PARAM_CHK_NORETVAL(dma_ch);
    uint32_t temp1, src_len, dest_len;
    uint8_t group_len;
    temp1 = ((uint32_t)(dma_ch->ch_id) * 0x30U) + dma_array[dma_ch->ctrl_id]->dev.reg_base;
    wj_dma_ch_regs_t *dma_ch_addr = (wj_dma_ch_regs_t *)(temp1);

    wj_dma_get_groupmode(dma_ch_addr, &src_len, &dest_len, &group_len);

    if (judge_valid(src_len, dest_len, group_len, length) == 0) {
        wj_dma_set_total_size(dma_ch_addr, length);
        wj_dma_set_channel_addr(dma_ch_addr, (uint32_t)soc_dma_address_remap((unsigned long)srcaddr), (uint32_t)soc_dma_address_remap((unsigned long)dstaddr));
        dma_set_ch_int_en(dma_ch_addr, WJ_DMA_CH_CTRLB_INT_EN);
        dma_set_ch_en(dma_ch_addr, WJ_DMA_CH_EN);

        if (dma_ch->etb_ch_id != -1) {
            csi_etb_ch_start((int32_t)(dma_ch->etb_ch_id));
        } else {
            dma_set_ch_soft_req(dma_ch_addr, WJ_DMA_CH_SREQ_REQ);
        }
    }
}

void csi_dma_ch_stop(csi_dma_ch_t *dma_ch)
{
    CSI_PARAM_CHK_NORETVAL(dma_ch);
    uint32_t temp;
    temp = ((uint32_t)(dma_ch->ch_id) * 0x30U) + dma_array[dma_ch->ctrl_id]->dev.reg_base;
    wj_dma_ch_regs_t *dma_ch_addr = (wj_dma_ch_regs_t *)(temp);
    dma_set_ch_int_en(dma_ch_addr, 0U);
    dma_set_ch_en(dma_ch_addr, 0U);
    dma_set_ch_soft_req(dma_ch_addr, 0U);

    if (dma_ch->etb_ch_id != -1) {
        csi_etb_ch_stop((int32_t)(dma_ch->etb_ch_id));
    }
}
