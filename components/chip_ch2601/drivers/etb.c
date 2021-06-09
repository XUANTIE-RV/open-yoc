/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_etb.c
 * @brief    CSI Source File for etb Driver
 * @version  V1.0
 * @date     19. December 2019
 * @vendor   csky
 * @name     wj_etb
 * @ip_id    0x111000010
 * @model    etb
 * @tag      DEV_WJ_ETB_TAG
 ******************************************************************************/
#include <soc.h>
#include <drv/etb.h>
#include <wj_etb_ll.h>
#include <drv/common.h>

static uint8_t csi_etb_alloc_status[4];

static int32_t check_is_alloced(int32_t ch_id)
{
    uint32_t ch_offset, ch_group;
    int32_t ret = 0;
    uint8_t status = 0U;

    ch_group = (uint32_t)((uint32_t)ch_id / 8U);
    ch_offset = (uint32_t)((uint32_t)ch_id % 8U);

    status = csi_etb_alloc_status[ch_group];

    status &= (uint8_t)(1U << (uint8_t)ch_offset);

    if (status != 0U) {
        ret = -1;
    }

    return ret;
}

static void set_ch_alloc_status(int32_t ch_id, uint32_t status)
{
    uint32_t ch_offset, ch_group;

    ch_group = (uint32_t)((uint32_t)ch_id / 8U);
    ch_offset = (uint32_t)((uint32_t)ch_id % 8U);

    if (status == 1U) {
        csi_etb_alloc_status[ch_group] |= (uint8_t)(1U << (uint8_t)ch_offset);
    } else if (status == 0U) {
        csi_etb_alloc_status[ch_group] &= ~(uint8_t)(1U << (uint8_t)ch_offset);
    }
}

csi_error_t csi_etb_init(void)
{
    wj_etb_regs_t *etb_base;
    etb_base = (wj_etb_regs_t *)WJ_ETB_BASE;
    wj_etb_enable(etb_base);

    return CSI_OK;
}

void csi_etb_uninit(void)
{
    wj_etb_regs_t *etb_base;
    etb_base = (wj_etb_regs_t *)WJ_ETB_BASE;
    wj_etb_disable(etb_base);
}

int32_t csi_etb_ch_alloc(csi_etb_ch_type_t ch_type)
{
    int32_t ret, ret_ch;
    uint32_t result = csi_irq_save();

    switch (ch_type) {
        case ETB_CH_MORE_TRIGGER_ONE:
            ret_ch = 0;

            if (check_is_alloced(ret_ch) < 0) {
                ret = CSI_ERROR;
            } else {
                ret = ret_ch;
            }

            break;

        case ETB_CH_ONE_TRIGGER_MORE:
            for (ret_ch = 1; ret_ch < 3; ret_ch++) {
                if (check_is_alloced(ret_ch) != -1) {
                    break;
                }
            }

            if (ret_ch == 3) {
                ret = CSI_ERROR;
            } else {
                ret = ret_ch;
            }

            break;

        case ETB_CH_ONE_TRIGGER_ONE:
            for (ret_ch = 3; ret_ch < 32; ret_ch++) {
                if (check_is_alloced(ret_ch) != -1) {
                    break;
                }
            }

            if (ret_ch == 32) {
                ret = CSI_ERROR;
            } else {
                ret = ret_ch;
            }

            break;

        default:
            ret = CSI_ERROR;
    }

    if (ret != CSI_ERROR) {
        set_ch_alloc_status(ret_ch, 1U);
    }

    csi_irq_restore(result);
    return ret;
}

void csi_etb_ch_free(int32_t ch_id)
{
    uint32_t result = csi_irq_save();
    set_ch_alloc_status(ch_id, 0U);
    csi_irq_restore(result);
}

csi_error_t csi_etb_ch_config(int32_t ch_id, csi_etb_config_t *config)
{
    CSI_PARAM_CHK(config, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_etb_regs_t *etb_base;

    etb_base = (wj_etb_regs_t *)WJ_ETB_BASE;

    if (config->ch_type == ETB_CH_ONE_TRIGGER_ONE) {
        if (ch_id < 3) {
            ret = CSI_ERROR;
        } else {
            wj_etb_ch_one_trig_one_config(etb_base, (uint32_t)((uint32_t)ch_id - 3U), (uint32_t)(config->src_ip), (uint32_t)(config->dst_ip), (uint32_t)(config->trig_mode));
        }
    } else {
        ret = CSI_UNSUPPORTED;
    }

    if (ret == CSI_OK) {
        wj_etb_ch_set_softtrig(etb_base, (uint32_t)ch_id);
    }

    return ret;
}

void csi_etb_ch_start(int32_t ch_id)
{
    wj_etb_regs_t *etb_base;
    etb_base = (wj_etb_regs_t *)WJ_ETB_BASE;
    wj_etb_channel_operate(etb_base, (uint32_t)ch_id, WJ_ETB_CH_ENABLE_COMMAND);
}

void csi_etb_ch_stop(int32_t ch_id)
{
    wj_etb_regs_t *etb_base;
    etb_base = (wj_etb_regs_t *)WJ_ETB_BASE;
    wj_etb_channel_operate(etb_base, (uint32_t)ch_id, WJ_ETB_CH_DISABLE_COMMAND);
}
