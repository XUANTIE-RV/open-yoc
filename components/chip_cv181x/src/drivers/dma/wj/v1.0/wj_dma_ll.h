/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_dma_ll.h
 * @brief    header file for dma ll driver
 * @version  V1.0
 * @date     27. December 2019
 ******************************************************************************/

#ifndef _WJ_DMA_LL_H_
#define _WJ_DMA_LL_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Work Flow
 * 1.Configure register: SAR, DAR, CH_CTRLA, CH_CTRLB and any other register. And last set
 * DMACCFG.DMACEN and CH_EN.chnen 1'b1.
 * 2.Config ETB, configure channel trigger enable control registers
 * 3.Wait int flag, the CH_INTS.statustfr interrupt flag will clear CH_EN.chn_en to 0
 * 4. After receive CH_INTC.cleartfr clear interrupt fla. Before user this channel, should config
 * CH_EN.chn_en to 1.
 * Note: SAR, DAR, CH_CTRLA, CH_CTRLB is enable protected (use channel internal enable protect, not
 * DMACEN). Before set DMACCFG.DMACEN 1'b1, user should configure it's channel registers.
 */

/*! CH_CTRLA, offset: 0x8 */
#define WJ_DMA_CH_CTRLA_DST_TR_WIDTH_Pos           (0U)
#define WJ_DMA_CH_CTRLA_DST_TR_WIDTH_Msk           (0x3U << WJ_DMA_CH_CTRLA_DST_TR_WIDTH_Pos)
#define WJ_DMA_CH_CTRLA_DST_TR_WIDTH_8_BITS        0U
#define WJ_DMA_CH_CTRLA_DST_TR_WIDTH_16_BITS       1U
#define WJ_DMA_CH_CTRLA_DST_TR_WIDTH_32_BITS       2U

#define WJ_DMA_CH_CTRLA_SRC_TR_WIDTH_Pos           (2U)
#define WJ_DMA_CH_CTRLA_SRC_TR_WIDTH_Msk           (0x3U << WJ_DMA_CH_CTRLA_SRC_TR_WIDTH_Pos)
#define WJ_DMA_CH_CTRLA_SRC_TR_WIDTH_8_BITS        0U
#define WJ_DMA_CH_CTRLA_SRC_TR_WIDTH_16_BITS       1U
#define WJ_DMA_CH_CTRLA_SRC_TR_WIDTH_32_BITS       2U

#define WJ_DMA_CH_CTRLA_DINC_Pos                   (4U)
#define WJ_DMA_CH_CTRLA_DINC_Msk                   (0x3U << WJ_DMA_CH_CTRLA_DINC_Pos)
#define WJ_DMA_CH_CTRLA_DINC_INCREMENT             0U
#define WJ_DMA_CH_CTRLA_DINC_DECREASE              1U
#define WJ_DMA_CH_CTRLA_DINC_NOCHANGE              2U

#define WJ_DMA_CH_CTRLA_SINC_Pos                   (6U)
#define WJ_DMA_CH_CTRLA_SINC_Msk                   (0x3U << WJ_DMA_CH_CTRLA_SINC_Pos)
#define WJ_DMA_CH_CTRLA_SINC_INCREMENT             0U
#define WJ_DMA_CH_CTRLA_SINC_DECREASE              1U
#define WJ_DMA_CH_CTRLA_SINC_NOCHANGE              2U

#define WJ_DMA_CH_CTRLA_GROUP_LEN_Pos              (8U)
#define WJ_DMA_CH_CTRLA_GROUP_LEN_Msk              (0xFU << WJ_DMA_CH_CTRLA_GROUP_LEN_Pos)
#define WJ_DMA_CH_CTRLA_GROUP_LEN_1_BYTE           0U
#define WJ_DMA_CH_CTRLA_GROUP_LEN_2_BYTES          1U
#define WJ_DMA_CH_CTRLA_GROUP_LEN_3_BYTES          2U
#define WJ_DMA_CH_CTRLA_GROUP_LEN_4_BYTES          3U
#define WJ_DMA_CH_CTRLA_GROUP_LEN_5_BYTES          4U
#define WJ_DMA_CH_CTRLA_GROUP_LEN_6_BYTES          5U
#define WJ_DMA_CH_CTRLA_GROUP_LEN_7_BYTES          6U
#define WJ_DMA_CH_CTRLA_GROUP_LEN_8_BYTES          7U

#define WJ_DMA_CH_CTRLA_BLOCK_TL_Pos               (12U)
#define WJ_DMA_CH_CTRLA_BLOCK_TL_Msk               (0xFFFU << WJ_DMA_CH_CTRLA_BLOCK_TL_Pos)

#define WJ_DMA_CH_CTRLA_GRPINTE_Pos                (24U)
#define WJ_DMA_CH_CTRLA_GRPINTE_Msk                (0x1U << WJ_DMA_CH_CTRLA_GRPINTE_Pos)
#define WJ_DMA_CH_CTRLA_GRPINTE_GP_COMPLETE_DIS    0U
#define WJ_DMA_CH_CTRLA_GRPINTE_GP_COMPLETE_EN     1U

#define WJ_DMA_CH_CTRLA_DSTSGLINTE_Pos             (25U)
#define WJ_DMA_CH_CTRLA_DSTSGLINTE_Msk             (0x1U << WJ_DMA_CH_CTRLA_DSTSGLINTE_Pos)
#define WJ_DMA_CH_CTRLA_DSTSGLINTE_SG_WRITE_DIS    0U
#define WJ_DMA_CH_CTRLA_DSTSGLINTE_SG_WRITE_EN     1U

#define WJ_DMA_CH_CTRLA_SRCSGLINTE_Pos             (26U)
#define WJ_DMA_CH_CTRLA_SRCSGLINTE_Msk             (0x1U << WJ_DMA_CH_CTRLA_SRCSGLINTE_Pos)
#define WJ_DMA_CH_CTRLA_SRCSGLINTE_SG_READ_DIS     0U
#define WJ_DMA_CH_CTRLA_SRCSGLINTE_SG_READ_EN      1U

#define WJ_DMA_CH_CTRLA_SGMDC_Pos                  (28U)
#define WJ_DMA_CH_CTRLA_SGMDC_Msk                  (0x1U << WJ_DMA_CH_CTRLA_SGMDC_Pos)
#define WJ_DMA_CH_CTRLA_SGMDC_DST_SG_TRIG_MODE     0U
#define WJ_DMA_CH_CTRLA_SGMDC_SRC_SG_TRIG_MODE     1U

#define WJ_DMA_CH_CTRLA_SGRPADDRC_Pos              (29U)
#define WJ_DMA_CH_CTRLA_SGRPADDRC_Msk              (0x1U << WJ_DMA_CH_CTRLA_SGRPADDRC_Pos)
#define WJ_DMA_CH_CTRLA_SGRPADDRC_USE_CRTADDR      0U
#define WJ_DMA_CH_CTRLA_SGRPADDRC_USE_SARn         1U

#define WJ_DMA_CH_CTRLA_DGRPADDRC_Pos              (31U)
#define WJ_DMA_CH_CTRLA_DGRPADDRC_Msk              (0x1U << WJ_DMA_CH_CTRLA_DGRPADDRC_Pos)
#define WJ_DMA_CH_CTRLA_DGRPADDRC_USE_CRTADDR      0U
#define WJ_DMA_CH_CTRLA_DGRPADDRC_USE_DARn         1U

/*! CH_CTRLB, offset: 0xC */
#define WJ_DMA_CH_CTRLB_INT_EN_Pos                 (0U)
#define WJ_DMA_CH_CTRLB_INT_EN_Msk                 (0x1U << WJ_DMA_CH_CTRLB_INT_EN_Pos)
#define WJ_DMA_CH_CTRLB_INT_EN                     WJ_DMA_CH_CTRLB_INT_EN_Msk

#define WJ_DMA_CH_CTRLB_TRGTMDC_Pos                (1U)
#define WJ_DMA_CH_CTRLB_TRGTMDC_Msk                (0x3U << WJ_DMA_CH_CTRLB_TRGTMDC_Pos)
#define WJ_DMA_CH_CTRLB_TRGTMDC_SINGLE_TRIG_MODE   0U
#define WJ_DMA_CH_CTRLB_TRGTMDC_GROUP_TRIG_MODE    1U
#define WJ_DMA_CH_CTRLB_TRGTMDC_BLOCK_TRIG_MODE    2U

#define WJ_DMA_CH_CTRLB_SRCDTLGC_Pos               (13U)
#define WJ_DMA_CH_CTRLB_SRCDTLGC_Msk               (0x1U << WJ_DMA_CH_CTRLB_SRCDTLGC_Pos)
#define WJ_DMA_CH_CTRLB_SRCDTLGC_LITTLE_ENDIAN     0U
#define WJ_DMA_CH_CTRLB_SRCDTLGC_BIG_ENDIAN        1U

#define WJ_DMA_CH_CTRLB_DSTDTLGC_Pos               (14U)
#define WJ_DMA_CH_CTRLB_DSTDTLGC_Msk               (0x1U << WJ_DMA_CH_CTRLB_DSTDTLGC_Pos)
#define WJ_DMA_CH_CTRLB_DSTDTLGC_LITTLE_ENDIAN     0U
#define WJ_DMA_CH_CTRLB_DSTDTLGC_BIG_ENDIAN        1U

#define WJ_DMA_CH_CTRLB_PROTCTL_Pos                (15U)
#define WJ_DMA_CH_CTRLB_PROTCTL_Msk                (0x1U << WJ_DMA_CH_CTRLB_PROTCTL_Pos)
#define WJ_DMA_CH_CTRLB_PROTCTL_NORMAL_ACCESS      0U
#define WJ_DMA_CH_CTRLB_PROTCTL_SEC_ACCESS         1U

/*! CH_INTM, offset: 0x10 */
#define WJ_DMA_CH_INTM_ERR_Pos                     (0U)
#define WJ_DMA_CH_INTM_ERR_Msk                     (0x1U << WJ_DMA_CH_INTM_ERR_Pos)
#define WJ_DMA_CH_INTM_ERR                         WJ_DMA_CH_INTM_ERR_Msk

#define WJ_DMA_CH_INTM_TFR_Pos                     (1U)
#define WJ_DMA_CH_INTM_TFR_Msk                     (0x1U << WJ_DMA_CH_INTM_TFR_Pos)
#define WJ_DMA_CH_INTM_TFR                         WJ_DMA_CH_INTM_TFR_Msk

#define WJ_DMA_CH_INTM_HTFR_Pos                    (2U)
#define WJ_DMA_CH_INTM_HTFR_Msk                    (0x1U << WJ_DMA_CH_INTM_HTFR_Pos)
#define WJ_DMA_CH_INTM_HTFR                        WJ_DMA_CH_INTM_HTFR_Msk

#define WJ_DMA_CH_INTM_TRGETCMPFR_Pos              (3U)
#define WJ_DMA_CH_INTM_TRGETCMPFR_Msk              (0x1U << WJ_DMA_CH_INTM_TRGETCMPFR_Pos)
#define WJ_DMA_CH_INTM_TRGETCMPFR                  WJ_DMA_CH_INTM_TRGETCMPFR_Msk

#define WJ_DMA_CH_INTM_PENDFR_Pos                  (4U)
#define WJ_DMA_CH_INTM_PENDFR_Msk                  (0x1U << WJ_DMA_CH_INTM_PENDFR_Pos)
#define WJ_DMA_CH_INTM_PENDFR                      WJ_DMA_CH_INTM_PENDFR_Msk

/*! CH_INTS, offset: 0x14 */
#define WJ_DMA_CH_INTS_ERR_Pos                     (0U)
#define WJ_DMA_CH_INTS_ERR_Msk                     (0x1U << WJ_DMA_CH_INTS_ERR_Pos)
#define WJ_DMA_CH_INTS_ERR                         WJ_DMA_CH_INTS_ERR_Msk

#define WJ_DMA_CH_INTS_TFR_Pos                     (1U)
#define WJ_DMA_CH_INTS_TFR_Msk                     (0x1U << WJ_DMA_CH_INTS_TFR_Pos)
#define WJ_DMA_CH_INTS_TFR                         WJ_DMA_CH_INTS_TFR_Msk

#define WJ_DMA_CH_INTS_HTFR_Pos                    (2U)
#define WJ_DMA_CH_INTS_HTFR_Msk                    (0x1U << WJ_DMA_CH_INTS_HTFR_Pos)
#define WJ_DMA_CH_INTS_HTFR                        WJ_DMA_CH_INTS_HTFR_Msk

#define WJ_DMA_CH_INTS_TRGETCMPFR_Pos              (3U)
#define WJ_DMA_CH_INTS_TRGETCMPFR_Msk              (0x1U << WJ_DMA_CH_INTS_TRGETCMPFR_Pos)
#define WJ_DMA_CH_INTS_TRGETCMPFR                  WJ_DMA_CH_INTS_TRGETCMPFR_Msk

/*! CH_INTC, offset: 0x18 */
#define WJ_DMA_CH_INTC_ERR_Pos                     (0U)
#define WJ_DMA_CH_INTC_ERR_Msk                     (0x1U << WJ_DMA_CH_INTC_ERR_Pos)
#define WJ_DMA_CH_INTC_ERR                         WJ_DMA_CH_INTC_ERR_Msk

#define WJ_DMA_CH_INTC_TFR_Pos                     (1U)
#define WJ_DMA_CH_INTC_TFR_Msk                     (0x1U << WJ_DMA_CH_INTC_TFR_Pos)
#define WJ_DMA_CH_INTC_TFR                         WJ_DMA_CH_INTC_TFR_Msk

#define WJ_DMA_CH_INTC_HTFR_Pos                    (2U)
#define WJ_DMA_CH_INTC_HTFR_Msk                    (0x1U << WJ_DMA_CH_INTC_HTFR_Pos)
#define WJ_DMA_CH_INTC_HTFR                        WJ_DMA_CH_INTC_HTFR_Msk

#define WJ_DMA_CH_INTC_TRGETCMPFR_Pos              (3U)
#define WJ_DMA_CH_INTC_TRGETCMPFR_Msk              (0x1U << WJ_DMA_CH_INTC_TRGETCMPFR_Pos)
#define WJ_DMA_CH_INTC_TRGETCMPFR                  WJ_DMA_CH_INTC_TRGETCMPFR_Msk

#define WJ_DMA_CH_INTC_PENDFR_Pos                  (4U)
#define WJ_DMA_CH_INTC_PENDFR_Msk                  (0x1U << WJ_DMA_CH_INTC_PENDFR_Pos)
#define WJ_DMA_CH_INTC_PENDFR                      WJ_DMA_CH_INTC_PENDFR_Msk

/*! CH_SREQ, offset: 0x1C */
#define WJ_DMA_CH_SREQ_Pos                         (0U)
#define WJ_DMA_CH_SREQ_Msk                         (0x1U << WJ_DMA_CH_SREQ_Pos)
#define WJ_DMA_CH_SREQ_REQ                         WJ_DMA_CH_SREQ_Msk

/*! CH_EN, offset: 0x20 */
#define WJ_DMA_CH_EN_Pos                           (0U)
#define WJ_DMA_CH_EN_Msk                           (0x1U << WJ_DMA_CH_EN_Pos)
#define WJ_DMA_CH_EN                               WJ_DMA_CH_EN_Msk


/* The registers of single channel, channel is 0~15 */
typedef struct {
    __IOM uint32_t  SAR;        /* offset: 0x00+n*0x30 (R/W) Channel Source Address Register        */
    __IOM uint32_t  DAR;        /* offset: 0x04+n*0x30 (R/W) Channel Destination Address Register   */
    __IOM uint32_t  CH_CTRLA;   /* offset: 0x08+n*0x30 (R/W) Channel Control Register A             */
    __IOM uint32_t  CH_CTRLB;   /* offset: 0x0C+n*0x30 (R/W) Channel Control Register B             */
    __IOM uint32_t  CH_INTM;    /* offset: 0x10+n*0x30 (R/W) Channel Interrupt Mask Register        */
    __IM  uint32_t  CH_INTS;    /* offset: 0x14+n*0x30 (R/ ) Channel Interrupt Status Register      */
    __IOM uint32_t  CH_INTC;    /* offset: 0x18+n*0x30 (R/W) Channel Interrupt Clear Register       */
    __IOM uint32_t  CH_SREQ;    /* offset: 0x1C+n*0x30 (R/W) Channel Software Request Register      */
    __IOM uint32_t  CH_EN;      /* offset: 0x20+n*0x30 (R/W) Channel Enable Register                */
    __IOM uint32_t  CH_GLE;     /* offset: 0x24+n*0x30 (R/W) Channel GropLength extension register  */
    uint32_t RESERVED[2];
} wj_dma_ch_regs_t;

typedef struct {
    wj_dma_ch_regs_t dma_array[16];
    uint32_t RESERVED0[12];
    __IM  uint32_t  CHPENDIFR;  /* offset: 0x330 (R/ ) Pend interrupt flag register */
    __IOM uint32_t  RESERVED1;
    __IM  uint32_t  CHSR;       /* offset: 0x338 (R/ ) Channel status register */
    __IOM uint32_t  DMACCFG;    /* offset: 0x33C (R/W) DMA configure register */
} wj_dma_regs_t;

static inline uint32_t dma_get_int_status(wj_dma_ch_regs_t *dma_addr)
{
    return dma_addr->CH_INTS;
}

static inline void dma_int_all_clear(wj_dma_ch_regs_t *dma_addr)
{
    dma_addr->CH_INTC = (WJ_DMA_CH_INTC_ERR | WJ_DMA_CH_INTC_TFR | WJ_DMA_CH_INTC_HTFR |
                         WJ_DMA_CH_INTC_TRGETCMPFR | WJ_DMA_CH_INTC_PENDFR);
}

static inline void dma_int_clear_status(wj_dma_ch_regs_t *dma_addr, uint32_t status)
{
    dma_addr->CH_INTC = status;
}

static inline void dma_set_int_mask(wj_dma_ch_regs_t *dma_addr, uint32_t umask)
{
    dma_addr->CH_INTM = umask;
}

static inline void dma_set_ch_soft_req(wj_dma_ch_regs_t *dma_addr, uint32_t req)
{
    if (req == WJ_DMA_CH_SREQ_REQ) {
        dma_addr->CH_SREQ = WJ_DMA_CH_SREQ_REQ;
    } else {
        dma_addr->CH_SREQ = 0U;
    }
}

static inline void dma_set_ch_int_en(wj_dma_ch_regs_t *dma_addr, uint32_t en)
{
    if (en == WJ_DMA_CH_CTRLB_INT_EN) {
        dma_addr->CH_CTRLB |= WJ_DMA_CH_CTRLB_INT_EN;
    } else {
        dma_addr->CH_CTRLB &= ~(WJ_DMA_CH_CTRLB_INT_EN);
    }
}

static inline void dma_set_ch_en(wj_dma_ch_regs_t *dma_addr, uint32_t en)
{
    if (en == WJ_DMA_CH_EN) {
        dma_addr->CH_EN |= WJ_DMA_CH_EN;
    } else {
        dma_addr->CH_EN &= ~(WJ_DMA_CH_EN);
    }
}

static inline void dma_set_en(wj_dma_regs_t *dma_addr, uint32_t en)
{
    if (en == WJ_DMA_CH_EN) {
        dma_addr->DMACCFG |= WJ_DMA_CH_EN;
    } else {
        dma_addr->DMACCFG &= ~(WJ_DMA_CH_EN);
    }
}

void wj_dma_set_transferwidth(wj_dma_ch_regs_t *addr, uint32_t src_width, uint32_t dst_width);

void wj_dma_set_addrinc(wj_dma_ch_regs_t *addr, uint32_t src_addrinc, uint32_t dst_addrinc);

void wj_dma_set_addr_endian(wj_dma_ch_regs_t *addr, uint32_t src_endian, uint32_t dst_endian);

void wj_dma_trans_mode_set(wj_dma_ch_regs_t *addr, uint32_t mode);

void wj_dma_set_singlemode(wj_dma_ch_regs_t *addr, uint32_t preemption, uint32_t single_dir);

void wj_dma_set_groupmode(wj_dma_ch_regs_t *addr, uint32_t src_inc, uint32_t dst_inc, uint32_t preemption, uint8_t group_len);

void wj_dma_set_int_mask_tfr(wj_dma_ch_regs_t *addr, uint32_t mask);

void wj_dma_set_int_clear_tfr(wj_dma_ch_regs_t *addr, uint32_t clear);

void wj_dma_set_total_size(wj_dma_ch_regs_t *addr, uint32_t len);

void wj_dma_set_channel_addr(wj_dma_ch_regs_t *addr, uint32_t src, uint32_t dst);

void wj_dma_get_groupmode(wj_dma_ch_regs_t *addr, uint32_t *src_inc, uint32_t *dst_inc, uint8_t * group_len);

#ifdef __cplusplus
}
#endif

#endif /* _WJ_DMA_LL_H_ */
