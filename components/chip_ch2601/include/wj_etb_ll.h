/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_etb_ll.h
 * @brief    header file for etb ll driver
 * @version  V1.0
 * @date     19. December 2019
 ******************************************************************************/

#ifndef _WJ_ETB_LL_H_
#define _WJ_ETB_LL_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

/*
 * Usage
 * 1. Configure dest trigger user IP1, and enable the IP1
 * 2. Configure channel in the ETB
 *      1)source-trigger select
 *      2)dest-trigger select
 *      3)mode select
 *      4)channel enable
 *      5)ETB enable
 *      6)softtrigger if need
 * 3. Configure source trigger generator IP2, and enable the IP2
 * For example, etb channel3 for timer trigger adc working, and etb channel4 for adc trigger dma to carry data
 * 1)configure adc and dma IP first and waiting trigger signal.
 * 2)configure etb channel3 and channel4 include source,dest,mode select.
 * 3)enable etb channel3 and channel4
 * 4)enable etb
 * 5)configure timer and enable
 * Note:The GPIO can be configured as level or edge mode. when using GPIO trigger, the GPIO must be level mode.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! ETB_EN, offset: 0x00 */
#define WJ_ETB_ETB_EN_Pos                       (0U)
#define WJ_ETB_ETB_EN_Msk                       (0x1U << WJ_ETB_ETB_EN_Pos)
#define WJ_ETB_ETB_EN_SET                       WJ_ETB_ETB_EN_Msk

/*! SOFTTRIG, offset: 0x04, 0 <= n <= 31 */
#define WJ_ETB_SOFTTRIG_CH_Pos(n)               (n)
#define WJ_ETB_SOFTTRIG_CH_Msk(n)               (0x1U << WJ_ETB_SOFTTRIG_CH_Pos(n))
#define WJ_ETB_SOFTTRIG_CH_CONFIG(n)            WJ_ETB_SOFTTRIG_CH_Msk(n)

/*! CFG0_CH0, offset: 0x08 */
#define WJ_ETB_CFG0_CH0_SRC0_EN_CH0_Pos         (0U)
#define WJ_ETB_CFG0_CH0_SRC0_EN_CH0_Msk         (0x1U << WJ_ETB_CFG0_CH0_SRC0_EN_CH0_Pos)
#define WJ_ETB_CFG0_CH0_SRC0_EN_CH0_SET         WJ_ETB_CFG0_CH0_SRC0_EN_CH0_Msk

#define WJ_ETB_CFG0_CH0_SRC0_SEL_CH0_Pos        (1U)
#define WJ_ETB_CFG0_CH0_SRC0_SEL_CH0_Msk        (0x7FU << WJ_ETB_CFG0_CH0_SRC0_SEL_CH0_Pos)

#define WJ_ETB_CFG0_CH0_SRC1_EN_CH0_Pos         (10U)
#define WJ_ETB_CFG0_CH0_SRC1_EN_CH0_Msk         (0x1U << WJ_ETB_CFG0_CH0_SRC1_EN_CH0_Pos)
#define WJ_ETB_CFG0_CH0_SRC1_EN_CH0_SET         WJ_ETB_CFG0_CH0_SRC1_EN_CH0_Msk

#define WJ_ETB_CFG0_CH0_SRC1_SEL_CH0_Pos        (11U)
#define WJ_ETB_CFG0_CH0_SRC1_SEL_CH0_Msk        (0x7FU << WJ_ETB_CFG0_CH0_SRC1_SEL_CH0_Pos)

#define WJ_ETB_CFG0_CH0_SRC2_EN_CH0_Pos         (20U)
#define WJ_ETB_CFG0_CH0_SRC2_EN_CH0_Msk         (0x7FU << WJ_ETB_CFG0_CH0_SRC2_EN_CH0_Pos)
#define WJ_ETB_CFG0_CH0_SRC2_EN_CH0_SET         WJ_ETB_CFG0_CH0_SRC2_EN_CH0_Msk

#define WJ_ETB_CFG0_CH0_SRC2_SEL_CH0_Pos        (21U)
#define WJ_ETB_CFG0_CH0_SRC2_SEL_CH0_Msk        (0x7FU << WJ_ETB_CFG0_CH0_SRC2_SEL_CH0_Pos)

/*! CFG1_CH0, offset: 0x0C */
#define WJ_ETB_CFG1_CH0_EN_Pos                  (0U)
#define WJ_ETB_CFG1_CH0_EN_Msk                  (0x1U << WJ_ETB_CFG1_CH0_EN_Pos)
#define WJ_ETB_CFG1_CH0_EN_SET                  WJ_ETB_CFG1_CH0_EN_Msk

#define WJ_ETB_CFG1_CH0_TRIG_MODE_Pos           (1U)
#define WJ_ETB_CFG1_CH0_TRIG_MODE_Msk           (0x1U << WJ_ETB_CFG1_CH0_TRIG_MODE_Pos)
#define WJ_ETB_CFG1_CH0_TRIG_MODE_HARDWARE      0
#define WJ_ETB_CFG1_CH0_TRIG_MODE_SOFTWARE      1

#define WJ_ETB_CFG1_CH0_DEST_USER_Pos           (24U)
#define WJ_ETB_CFG1_CH0_DEST_USER_Msk           (0xFFU << WJ_ETB_CFG1_CH0_DEST_USER_Pos)

/*! CFG0_CHX(X is 1 or 2), offset: 0x10 or 0x18 */
#define WJ_ETB_CFG0_CHX_DEST0_EN_CHX_Pos        (0U)
#define WJ_ETB_CFG0_CHX_DEST0_EN_CHX_Msk        (0x1U << WJ_ETB_CFG0_CHX_DEST0_EN_CHX_Pos)
#define WJ_ETB_CFG0_CHX_DEST0_EN_CHX_SET        WJ_ETB_CFG0_CHX_DEST0_EN_CHX_Msk

#define WJ_ETB_CFG0_CHX_DEST0_USER_CHX_Pos      (1U)
#define WJ_ETB_CFG0_CHX_DEST0_USER_CHX_Msk      (0xFFU << WJ_ETB_CFG0_CHX_DEST0_USER_CHX_Pos)

#define WJ_ETB_CFG0_CHX_DEST1_EN_CHX_Pos        (10U)
#define WJ_ETB_CFG0_CHX_DEST1_EN_CHX_Msk        (0x1U << WJ_ETB_CFG0_CHX_DEST1_EN_CHX_Pos)
#define WJ_ETB_CFG0_CHX_DEST1_EN_CHX_SET        WJ_ETB_CFG0_CH1_DEST1_EN_CHX_Msk

#define WJ_ETB_CFG0_CHX_DEST1_USER_CHX_Pos      (11U)
#define WJ_ETB_CFG0_CHX_DEST1_USER_CHX_Msk      (0xFFU << WJ_ETB_CFG0_CHX_DEST1_USER_CHX_Pos)

#define WJ_ETB_CFG0_CHX_DEST2_EN_CHX_Pos        (20U)
#define WJ_ETB_CFG0_CHX_DEST2_EN_CHX_Msk        (0x1U << WJ_ETB_CFG0_CHX_DEST2_EN_CHX_Pos)
#define WJ_ETB_CFG0_CHX_DEST2_EN_CHX_SET        WJ_ETB_CFG0_CHX_DEST2_EN_CHX_Msk

#define WJ_ETB_CFG0_CHX_DEST2_USER_CHX_Pos      (21U)
#define WJ_ETB_CFG0_CHX_DEST2_USER_CHX_Msk      (0xFFU << WJ_ETB_CFG0_CHX_DEST2_USER_CHX_Pos)

/*! CFG1_CHX(X is 1 or 2), offset: 0x14 or 0x1C */
#define WJ_ETB_CFG1_CHX_EN_Pos                  (0U)
#define WJ_ETB_CFG1_CHX_EN_Msk                  (0x1U << WJ_ETB_CFG1_CHX_EN_Pos)
#define WJ_ETB_CFG1_CHX_EN_SET                  WJ_ETB_CFG1_CHX_EN_Msk

#define WJ_ETB_CFG1_TRIG_MODE_CHX_Pos           (1U)
#define WJ_ETB_CFG1_TRIG_MODE_CHX_Msk           (0x1U << WJ_ETB_CFG1_TRIG_MODE_CHX_Pos)
#define WJ_ETB_CFG1_TRIG_MODE_CHX_HARDWARE      0
#define WJ_ETB_CFG1_TRIG_MODE_CHX_SOFTWARE      1

#define WJ_ETB_CFG1_SRC_SEL_CHX_Pos             (25U)
#define WJ_ETB_CFG1_SRC_SEL_CHX_Msk             (0x7FU << WJ_ETB_CFG1_SRC_SEL_CHX_Pos)

/*! CFG_CH3~31, offset: 0x30 ~ 0xa0 */
#define WJ_ETB_CFG_CHX_EN_Pos                   (0U)
#define WJ_ETB_CFG_CHX_EN_Msk                   (0x1U << WJ_ETB_CFG_CHX_EN_Pos)
#define WJ_ETB_CFG_CHX_EN_SET                   WJ_ETB_CFG_CHX_EN_Msk

#define WJ_ETB_CFG_CHX_TRIG_MODE_Pos            (1U)
#define WJ_ETB_CFG_CHX_TRIG_MODE_Msk            (0x1U << WJ_ETB_CFG_CHX_TRIG_MODE_Pos)
#define WJ_ETB_CFG_CHX_TRIG_MODE_HARDWARE       0
#define WJ_ETB_CFG_CHX_TRIG_MODE_SOFTWARE       1

#define WJ_ETB_CFG_CHX_SRC_SEL_CHX_Pos          (12U)
#define WJ_ETB_CFG_CHX_SRC_SEL_CHX_Msk          (0x7FU << WJ_ETB_CFG_CHX_SRC_SEL_CHX_Pos)

#define WJ_ETB_CFG_CHX_DEST_USER_Pos            (24U)
#define WJ_ETB_CFG_CHX_DEST_USER_Msk            (0xFFU << WJ_ETB_CFG_CHX_DEST_USER_Pos)

#define WJ_ETB_CH_ENABLE_COMMAND                (1U)
#define WJ_ETB_CH_DISABLE_COMMAND               (0U)

typedef struct {
    __IOM uint32_t CFG0;
    __IOM uint32_t CFG1;
} wj_etb_ch1_2_regs_t;

typedef struct {
    __IOM uint32_t ETB_EN;          /* offset: 0x00 (R/W) ETB enable Register                    */
    __IOM uint32_t SOFTTRIG;        /* offset: 0x04 (R/W) Software trigger Configure Register    */
    __IOM uint32_t CFG0_CH0;        /* offset: 0x08 (R/W) ETB Channel0 Configure0 Register       */
    __IOM uint32_t CFG1_CH0;        /* offset: 0x0c (R/W) ETB Channel0 Configure1 Register       */
    wj_etb_ch1_2_regs_t CH1_2[2];   /* offset: 0x10 ~ 0x14 (R/W) ETB Channel1 Configure0~1 Register */
    /* offset: 0x18 ~ 0x1C (R/W) ETB Channel2 Configure0~1 Register */
    uint32_t RESERVED0[4];
    __IOM uint32_t CFG_CHX[29];     /* offset: 0x30~0xa0 (R/W) ETB CH_3~CH_31 Configure Register */
} wj_etb_regs_t;

static inline void wj_etb_enable(wj_etb_regs_t *etb_base)
{
    etb_base->ETB_EN |= WJ_ETB_ETB_EN_SET;
}

static inline void wj_etb_disable(wj_etb_regs_t *etb_base)
{
    etb_base->ETB_EN &= ~(WJ_ETB_ETB_EN_SET);
}

static inline void wj_etb_soft_trigger(wj_etb_regs_t *etb_base, uint32_t ch)
{
    if (ch >= 3U) {
        etb_base->CFG_CHX[ch - 3U] |= (uint32_t)(1U << 1U);
    }
}

static inline void wj_etb_hardware_trigger(wj_etb_regs_t *etb_base, uint32_t ch)
{
    if (ch >= 3U) {
        etb_base->CFG_CHX[ch - 3U] &= ~(uint32_t)(1U << 1U);
    }
}

static inline void wj_etb_ch_set_softtrig(wj_etb_regs_t *etb_base, uint32_t ch_id)
{
    uint8_t temp = WJ_ETB_SOFTTRIG_CH_CONFIG(ch_id);
    etb_base->SOFTTRIG |= (uint32_t)temp;
}

void wj_etb_ch_one_trig_one_config(wj_etb_regs_t *etb_base, uint32_t ch_idx, uint32_t src, uint32_t dest,  uint32_t mode);

void wj_etb_channel_operate(wj_etb_regs_t *etb_base, uint32_t ch_idx, uint32_t operate);

#ifdef __cplusplus
}
#endif

#endif /* _WJ_ETB_LL_H_ */
