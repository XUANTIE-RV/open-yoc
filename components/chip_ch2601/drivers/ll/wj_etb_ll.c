/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_etb_ll.c
 * @brief    etb ll driver file
 * @version  V1.0
 * @date     19. December 2019
 ******************************************************************************/

#include <wj_etb_ll.h>

void wj_etb_ch_one_trig_one_config(wj_etb_regs_t *etb_base, uint32_t ch_id, uint32_t src, uint32_t dest,  uint32_t mode)
{
    etb_base->CFG_CHX[ch_id] &= ~(WJ_ETB_CFG_CHX_SRC_SEL_CHX_Msk);
    etb_base->CFG_CHX[ch_id] |= src << WJ_ETB_CFG_CHX_SRC_SEL_CHX_Pos;
    etb_base->CFG_CHX[ch_id] &= ~(WJ_ETB_CFG_CHX_DEST_USER_Msk);
    etb_base->CFG_CHX[ch_id] |= dest << WJ_ETB_CFG_CHX_DEST_USER_Pos;
    etb_base->CFG_CHX[ch_id] &= ~(WJ_ETB_CFG_CHX_TRIG_MODE_Msk);
    etb_base->CFG_CHX[ch_id] |= (mode << WJ_ETB_CFG_CHX_TRIG_MODE_Pos);
    etb_base->CFG_CHX[ch_id] |= WJ_ETB_CFG_CHX_EN_SET;
}

void wj_etb_channel_operate(wj_etb_regs_t *etb_base, uint32_t ch_id, uint32_t operate)
{
    if (ch_id == 0U) {
        etb_base->CFG1_CH0 = ((~WJ_ETB_CFG1_CH0_EN_SET) & (etb_base->CFG1_CH0)) | operate;
    } else if (ch_id == 1U) {
        etb_base->CH1_2[0U].CFG1 = ((~WJ_ETB_CFG1_CHX_EN_SET) & (etb_base->CH1_2[0].CFG1)) | operate;
    } else if (ch_id == 2U) {
        etb_base->CH1_2[1].CFG1 = ((~WJ_ETB_CFG1_CHX_EN_SET) & (etb_base->CH1_2[1].CFG1)) | operate;
    } else if ((ch_id >= 3U) && (ch_id <= 31U)) {
        etb_base->CFG_CHX[ch_id - 3U] = ((~WJ_ETB_CFG_CHX_EN_SET) & (etb_base->CFG_CHX[ch_id - 3U])) | operate;
    }
}

//TODO
//ONE TRIGGER MORE...
