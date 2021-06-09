/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_adc_ll.h
 * @brief
 * @version
 * @date     2020-03-05
 ******************************************************************************/

#ifndef _WJ_ADC_LL_H_
#define _WJ_ADC_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ADC_CR, offset: 0x00 */
#define WJ_ADC_CR_NUSMPCLR_Pos                         (25U)
#define WJ_ADC_CR_NUSMPCLR_Msk                         (0x1FU << WJ_ADC_CR_NUSMPCLR_Pos)

#define WJ_ADC_CR_DLRC_Pos                             (23U)
#define WJ_ADC_CR_DLRC_Msk                             (0x1U << WJ_ADC_CR_DLRC_Pos)
#define WJ_ADC_CR_DLRC_LEFT_ALIGN                      (0x0U << WJ_ADC_CR_DLRC_Pos)
#define WJ_ADC_CR_DLRC_RIGHT_ALIGN                     (0x1U << WJ_ADC_CR_DLRC_Pos)

#define WJ_ADC_CR_OFSTMC_Pos                           (20U)
#define WJ_ADC_CR_OFSTMC_Msk                           (0x1U << WJ_ADC_CR_OFSTMC_Pos)
#define WJ_ADC_CR_OFSTMC_PLUS                          (0x0U << WJ_ADC_CR_OFSTMC_Pos)
#define WJ_ADC_CR_OFSTMC_MINUS                         (0x1U << WJ_ADC_CR_OFSTMC_Pos)

#define WJ_ADC_CR_AVGC_Pos                             (16U)
#define WJ_ADC_CR_AVGC_Msk                             (0x7U << WJ_ADC_CR_AVGC_Pos)
#define WJ_ADC_CR_AVGC_0                               (0x0U << WJ_ADC_CR_AVGC_Pos)
#define WJ_ADC_CR_AVGC_2                               (0x1U << WJ_ADC_CR_AVGC_Pos)
#define WJ_ADC_CR_AVGC_4                               (0x2U << WJ_ADC_CR_AVGC_Pos)
#define WJ_ADC_CR_AVGC_8                               (0x3U << WJ_ADC_CR_AVGC_Pos)
#define WJ_ADC_CR_AVGC_16                              (0x4U << WJ_ADC_CR_AVGC_Pos)
#define WJ_ADC_CR_AVGC_32                              (0x5U << WJ_ADC_CR_AVGC_Pos)

#define WJ_ADC_CR_DRBC_Pos                             (13U)
#define WJ_ADC_CR_DRBC_Msk                             (0x3U << WJ_ADC_CR_DRBC_Pos)
#define WJ_ADC_CR_DRBC_12_BIT                          (0x0U << WJ_ADC_CR_DRBC_Pos)
#define WJ_ADC_CR_DRBC_11_BIT                          (0x1U << WJ_ADC_CR_DRBC_Pos)
#define WJ_ADC_CR_DRBC_10_BIT                          (0x2U << WJ_ADC_CR_DRBC_Pos)
#define WJ_ADC_CR_DRBC_9_BIT                           (0x3U << WJ_ADC_CR_DRBC_Pos)

#define WJ_ADC_CR_ADCCLKDIV_Pos                        (1U)
#define WJ_ADC_CR_ADCCLKDIV_Msk                        (0x1FU << WJ_ADC_CR_ADCCLKDIV_Pos)
#define WJ_ADC_CR_ADCCLKDIV_NO                         (0x0U << WJ_ADC_CR_ADCCLKDIV_Pos)
#define WJ_ADC_CR_ADCCLKDIV_2                          (0x1U << WJ_ADC_CR_ADCCLKDIV_Pos)
#define WJ_ADC_CR_ADCCLKDIV_4                          (0x2U << WJ_ADC_CR_ADCCLKDIV_Pos)
#define WJ_ADC_CR_ADCCLKDIV_8                          (0x3U << WJ_ADC_CR_ADCCLKDIV_Pos)
#define WJ_ADC_CR_ADCCLKDIV_16                         (0x4U << WJ_ADC_CR_ADCCLKDIV_Pos)
#define WJ_ADC_CR_ADCCLKDIV_32                         (0x5U << WJ_ADC_CR_ADCCLKDIV_Pos)
#define WJ_ADC_CR_ADCCLKDIV_64                         (0x6U << WJ_ADC_CR_ADCCLKDIV_Pos)
#define WJ_ADC_CR_ADCCLKDIV_128                        (0x7U << WJ_ADC_CR_ADCCLKDIV_Pos)

#define WJ_ADC_CR_ADCPWD_Pos                           (0U)
#define WJ_ADC_CR_ADCPWD_Msk                           (0x1U << WJ_ADC_CR_ADCPWD_Pos)
#define WJ_ADC_CR_ADCPWD_EN                            WJ_ADC_CR_ADCPWD_Msk

/* ADC_SCFG, offset: 0x04 */
#define WJ_ADC_SCFG_HPSINTOSMS_Pos                     (21U)
#define WJ_ADC_SCFG_HPSINTOSMS_Msk                     (0x1U << WJ_ADC_SCFG_HPSINTOSMS_Pos)
#define WJ_ADC_SCFG_HPSINTOSMS_IMMEDIATELY             (0x0U << WJ_ADC_SCFG_HPSINTOSMS_Pos)
#define WJ_ADC_SCFG_HPSINTOSMS_DELAY                   (0x1U << WJ_ADC_SCFG_HPSINTOSMS_Pos)

#define WJ_ADC_SCFG_HPSAUTO_Pos                        (20U)
#define WJ_ADC_SCFG_HPSAUTO_Msk                        (0x1U << WJ_ADC_SCFG_HPSAUTO_Pos)
#define WJ_ADC_SCFG_HPSAUTO_EN                         WJ_ADC_SCFG_HPSAUTO_Msk

#define WJ_ADC_SCFG_OSMC_Pos                           (16U)
#define WJ_ADC_SCFG_OSMC_Msk                           (0x3U << WJ_ADC_SCFG_OSMC_Pos)
#define WJ_ADC_SCFG_OSMC_SINGLE                        (0x0U << WJ_ADC_SCFG_OSMC_Pos)
#define WJ_ADC_SCFG_OSMC_DIS_CONTINUE                  (0x1U << WJ_ADC_SCFG_OSMC_Pos)
#define WJ_ADC_SCFG_OSMC_CONTINUE                      (0x2U << WJ_ADC_SCFG_OSMC_Pos)
#define WJ_ADC_SCFG_OSMC_SCAN                          (0x3U << WJ_ADC_SCFG_OSMC_Pos)

#define WJ_ADC_SCFG_HPSMC_Pos                          (12U)
#define WJ_ADC_SCFG_HPSMC_Msk                          (0x3U << WJ_ADC_SCFG_HPSMC_Pos)
#define WJ_ADC_SCFG_HPSMC_SINGLE                       (0x0U << WJ_ADC_SCFG_HPSMC_Pos)
#define WJ_ADC_SCFG_HPSMC_DIS_CONTINUE                 (0x1U << WJ_ADC_SCFG_HPSMC_Pos)
#define WJ_ADC_SCFG_HPSMC_CONTINUE                     (0x2U << WJ_ADC_SCFG_HPSMC_Pos)

#define WJ_ADC_SCFG_NUOHPSDCM_Pos                      (8U)
#define WJ_ADC_SCFG_NUOHPSDCM_Msk                      (0xFU << WJ_ADC_SCFG_NUOHPSDCM_Pos)

#define WJ_ADC_SCFG_NUOOSDCM_Pos                       (0U)
#define WJ_ADC_SCFG_NUOOSDCM_Msk                       (0xFU << WJ_ADC_SCFG_NUOHPSDCM_Pos)

/* ADC_OSVCNU, offset: 0x08 */
#define WJ_ADC_OSVCNU_Pos                              (0U)
#define WJ_ADC_OSVCNU_Msk                              (0x1FU << WJ_ADC_OSVCNU_Pos)

/* ADC_HPSVCNU, offset: 0x0C */
#define WJ_ADC_HPSVCNU_Pos                             (0U)
#define WJ_ADC_HPSVCNU_Msk                             (0x1FU << WJ_ADC_OSVCNU_Pos)

/* ADC_SR, offset: 0x10 */
#define WJ_ADC_SR_TGLST_Pos                            (29)
#define WJ_ADC_SR_TGLST_Msk                            (0x1U << WJ_ADC_SR_TGLST_Pos)

#define WJ_ADC_SR_VALID_Pos                            (27)
#define WJ_ADC_SR_VALID_Msk                            (0x1U << WJ_ADC_SR_VALID_Pos)
#define WJ_ADC_SR_VALID                                WJ_ADC_SR_VALID_Msk

#define WJ_ADC_SR_CCC_Pos                              (19)
#define WJ_ADC_SR_CCC_Msk                              (0xFU << WJ_ADC_SR_CCC_Pos)

#define WJ_ADC_SR_BUSY_Pos                             (15)
#define WJ_ADC_SR_BUSY_Msk                             (0x1U << WJ_ADC_SR_BUSY_Pos)
#define WJ_ADC_SR_BUSY                                 WJ_ADC_SR_BUSY_Msk

#define WJ_ADC_SR_DVC_Pos                              (0)
#define WJ_ADC_SR_DVC_Msk                              (0xFU << WJ_ADC_SR_DVC_Pos)

/* ADC_CMPCR, offset: 0x18 */
#define WJ_ADC_CMPCR_ACGCLRC_Pos                       (16)
#define WJ_ADC_CMPCR_ACGCLRC_Msk                       (0x1U << WJ_ADC_CMPCR_ACGCLRC_Pos)
#define WJ_ADC_CMPCR_ACGCLRC_EN                        WJ_ADC_CMPCR_ACGCLRC_Msk

#define WJ_ADC_CMPCR_DCMPMC_Pos                        (12)
#define WJ_ADC_CMPCR_DCMPMC_Msk                        (0x7U << WJ_ADC_CMPCR_DCMPMC_Pos)
#define WJ_ADC_CMPCR_DCMPMC_1                          (0x3U << WJ_ADC_CMPCR_DCMPMC_Pos)
#define WJ_ADC_CMPCR_DCMPMC_2                          (0x1U << WJ_ADC_CMPCR_DCMPMC_Pos)
#define WJ_ADC_CMPCR_DCMPMC_3                          (0x2U << WJ_ADC_CMPCR_DCMPMC_Pos)
#define WJ_ADC_CMPCR_DCMPMC_4                          (0x4U << WJ_ADC_CMPCR_DCMPMC_Pos)

#define WJ_ADC_CMPCR_CMPCHS_Pos                        (4)
#define WJ_ADC_CMPCR_CMPCHS_Msk                        (0xFU << WJ_ADC_CMPCR_CMPCHS_Pos)

#define WJ_ADC_CMPCR_CMPSOCHS_Pos                      (0)
#define WJ_ADC_CMPCR_CMPSOCHS_Msk                      (0xFU << WJ_ADC_CMPCR_CMPSOCHS_Pos)
#define WJ_ADC_CMPCR_CMPSOCHS_ONE_CHANNEL              (0x0U << WJ_ADC_CMPCR_CMPSOCHS_Pos)
#define WJ_ADC_CMPCR_CMPSOCHS_ORDER_SEQUENCE           (0x1U << WJ_ADC_CMPCR_CMPSOCHS_Pos)
#define WJ_ADC_CMPCR_CMPSOCHS_HIGH_PRIORITY_SEQUENCE   (0x2U << WJ_ADC_CMPCR_CMPSOCHS_Pos)
#define WJ_ADC_CMPCR_CMPSOCHS_ALL                      (0x3U << WJ_ADC_CMPCR_CMPSOCHS_Pos)

/* ADC_IE, offset: 0x24 */
#define WJ_ADC_IE_OSSCEDIE_Pos                         (8)
#define WJ_ADC_IE_OSSCEDIE_Msk                         (0x1U << WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_IE_OSSCEDIE                             WJ_ADC_IE_OSSCEDIE_Msk

#define WJ_ADC_IE_HPSSCEDIE_Pos                        (7)
#define WJ_ADC_IE_HPSSCEDIE_Msk                        (0x1U << WJ_ADC_IE_HPSSCEDIE_Pos)
#define WJ_ADC_IE_HPSSCEDIE                            WJ_ADC_IE_HPSSCEDIE_Msk

#define WJ_ADC_IE_CMPIE_Pos                            (5)
#define WJ_ADC_IE_CMPIE_Msk                            (0x1U << WJ_ADC_IE_CMPIE_Pos)
#define WJ_ADC_IE_CMPIE                                WJ_ADC_IE_CMPIE_Msk

#define WJ_ADC_IE_OVWTIE_Pos                           (4)
#define WJ_ADC_IE_OVWTIE_Msk                           (0x1U << WJ_ADC_IE_OVWTIE_Pos)
#define WJ_ADC_IE_OVWTIE                               WJ_ADC_IE_OVWTIE_Msk

#define WJ_ADC_IE_HSSS_EOCIE_Pos                       (4)
#define WJ_ADC_IE_HSSS_EOCIE_Msk                       (0x1U << WJ_ADC_IE_HSSS_EOCIE_Pos)
#define WJ_ADC_IE_HSSS_EOCIE                           WJ_ADC_IE_HSSS_EOCIE_Msk

#define WJ_ADC_IE_OSS_EOCIE_Pos                        (0)
#define WJ_ADC_IE_OSS_EOCIE_Msk                        (0x1U << WJ_ADC_IE_OSS_EOCIE_Pos)
#define WJ_ADC_IE_OSS_EOCIE                            WJ_ADC_IE_OSS_EOCIE_Msk

/* ADC_IF, offset: 0x28 */
#define WJ_ADC_IF_OSSCEDIF_Pos                         (8)
#define WJ_ADC_IF_OSSCEDIF_Msk                         (0x1U << WJ_ADC_IF_OSSCEDIF_Pos)
#define WJ_ADC_IF_OSSCEDIF                             WJ_ADC_IF_OSSCEDIF_Msk

#define WJ_ADC_IF_HPSSCEDIF_Pos                        (7)
#define WJ_ADC_IF_HPSSCEDIF_Msk                        (0x1U << WJ_ADC_IF_HPSSCEDIF_Pos)
#define WJ_ADC_IF_HPSSCEDIF                            WJ_ADC_IF_HPSSCEDIF_Msk

#define WJ_ADC_IF_CMPIF_Pos                            (5)
#define WJ_ADC_IF_CMPIF_Msk                            (0x1U << WJ_ADC_IF_CMPIF_Pos)
#define WJ_ADC_IF_CMPIF                                WJ_ADC_IF_CMPIF_Msk

#define WJ_ADC_IF_OVWTIF_Pos                           (4)
#define WJ_ADC_IF_OVWTIF_Msk                           (0x1U << WJ_ADC_IF_OVWTIF_Pos)
#define WJ_ADC_IF_OVWTIF                               WJ_ADC_IF_OVWTIF_Msk

#define WJ_ADC_IF_HSSS_EOCIF_Pos                       (1)
#define WJ_ADC_IF_HSSS_EOCIF_Msk                       (0x1U << WJ_ADC_IF_HSSS_EOCIF_Pos)
#define WJ_ADC_IF_HSSS_EOCIF                           WJ_ADC_IF_HSSS_EOCIF_Msk

#define WJ_ADC_IF_OSS_EOCIF_Pos                        (0)
#define WJ_ADC_IF_OSS_EOCIF_Msk                        (0x1U << WJ_ADC_IF_OSS_EOCIF_Pos)
#define WJ_ADC_IF_OSS_EOCIF                            WJ_ADC_IF_OSS_EOCIF_Msk

/* ADC_SWTC, offset: 0x2C */
#define WJ_ADC_SWTC_ADCCOFF_Pos                        (3)
#define WJ_ADC_SWTC_ADCCOFF_Msk                        (0x1U << WJ_ADC_SWTC_ADCCOFF_Pos)
#define WJ_ADC_SWTC_ADCCOFF                            WJ_ADC_SWTC_ADCCOFF_Msk

#define WJ_ADC_SWTC_SWHPSTR_Pos                        (2)
#define WJ_ADC_SWTC_SWHPSTR_Msk                        (0x1U << WJ_ADC_SWTC_SWHPSTR_Pos)
#define WJ_ADC_SWTC_SWHPSTR                            WJ_ADC_SWTC_SWHPSTR_Msk

#define WJ_ADC_SWTC_SWOSTR_Pos                         (0)
#define WJ_ADC_SWTC_SWOSTR_Msk                         (0x1U << WJ_ADC_SWTC_SWOSTR_Pos)
#define WJ_ADC_SWTC_SWOSTR                             WJ_ADC_SWTC_SWOSTR_Msk

/* OFFSETDR, offset: 0x30 */
#define WJ_ADC_OFFSETDR_Pos                            (0)
#define WJ_ADC_OFFSETDR_Msk                            (0x1FFFU << WJ_ADC_OFFSETDR_Pos)

/* AADCCFGEX, offset: 0x3c */
#define WJ_ADC_AADCCFGEX_EOCMD_Pos                     (30)
#define WJ_ADC_AADCCFGEX_EOCMD_Msk                     (0x1U << WJ_ADC_AADCCFGEX_EOCMD_Pos)
#define WJ_ADC_AADCCFGEX_EOCMD_EN                      WJ_ADC_AADCCFGEX_EOCMD_Msk

#define WJ_ADC_AADCCFGEX_NUCLKCNV_Pos                  (0)
#define WJ_ADC_AADCCFGEX_NUCLKCNV_Msk                  (0xFU << WJ_ADC_AADCCFGEX_NUCLKCNV_Pos)
#define WJ_ADC_AADCCFGEX_NUCLKCNV_EN                   WJ_ADC_AADCCFGEX_NUCLKCNV_Msk

/* ADC_ACFG0, offset: 0x50 */
#define WJ_ADC_ACFG0_CONV_CAL_Pos                      (11)
#define WJ_ADC_ACFG0_CONV_CAL_Msk                      (0x1U << WJ_ADC_ACFG0_CONV_CAL_Pos)
#define WJ_ADC_ACFG0_CONV_CAL_EN                       WJ_ADC_ACFG0_CONV_CAL_Msk

#define WJ_ADC_ACFG0_ADCCR_Pos                         (9)
#define WJ_ADC_ACFG0_ADCCR_Msk                         (0x1U << WJ_ADC_ACFG0_ADCCR_Pos)
#define WJ_ADC_ACFG0_ADCCR_32M                         (0x0U << WJ_ADC_ACFG0_ADCCR_Pos)
#define WJ_ADC_ACFG0_ADCCR_4M                          (0x1U << WJ_ADC_ACFG0_ADCCR_Pos)

#define WJ_ADC_ACFG0_ADCSREF_Pos                       (9)
#define WJ_ADC_ACFG0_ADCSREF_Msk                       (0x1U << WJ_ADC_ACFG0_ADCCR_Pos)
#define WJ_ADC_ACFG0_ADCSREF_32M                       (0x0U << WJ_ADC_ACFG0_ADCCR_Pos)
#define WJ_ADC_ACFG0_ADCSREF_4M                        (0x1U << WJ_ADC_ACFG0_ADCCR_Pos)

#define WJ_ADC_ACFG0_AINS3_Pos                         (3)
#define WJ_ADC_ACFG0_AINS3_Msk                         (0x1U << WJ_ADC_ACFG0_AINS3_Pos)
#define WJ_ADC_ACFG0_AINS3_SINGLE                      ( 0x0U << WJ_ADC_ACFG0_AINS3_Pos )
#define WJ_ADC_ACFG0_AINS3_DIFF                        ( 0x1U << WJ_ADC_ACFG0_AINS3_Pos )

#define WJ_ADC_ACFG0_AINS2_Pos                         (2)
#define WJ_ADC_ACFG0_AINS2_Msk                         (0x1U << WJ_ADC_ACFG0_AINS2_Pos)
#define WJ_ADC_ACFG0_AINS2_SINGLE                      ( 0x0U << WJ_ADC_ACFG0_AINS2_Pos )
#define WJ_ADC_ACFG0_AINS2_DIFF                        ( 0x1U << WJ_ADC_ACFG0_AINS2_Pos )

#define WJ_ADC_ACFG0_AINS1_Pos                         (1)
#define WJ_ADC_ACFG0_AINS1_Msk                         (0x1U << WJ_ADC_ACFG0_AINS1_Pos)
#define WJ_ADC_ACFG0_AINS1_SINGLE                      ( 0x0U << WJ_ADC_ACFG0_AINS1_Pos )
#define WJ_ADC_ACFG0_AINS1_DIFF                        ( 0x1U << WJ_ADC_ACFG0_AINS1_Pos )

#define WJ_ADC_ACFG0_AINS0_Pos                         (0)
#define WJ_ADC_ACFG0_AINS0_Msk                         (0x1U << WJ_ADC_ACFG0_AINS0_Pos)
#define WJ_ADC_ACFG0_AINS0_SINGLE                      ( 0x0U << WJ_ADC_ACFG0_AINS0_Pos )
#define WJ_ADC_ACFG0_AINS0_DIFF                        ( 0x1U << WJ_ADC_ACFG0_AINS0_Pos )

/* ADC_ACFG1, offset: 0x54 */
#define WJ_ADC_ACFG1_CONV_ERR_CLR_Pos                  (31)
#define WJ_ADC_ACFG1_CONV_ERR_CLR_Msk                  (0x1U << WJ_ADC_ACFG1_CONV_ERR_CLR_Pos)
#define WJ_ADC_ACFG1_CONV_ERR_CLR_EN                   WJ_ADC_ACFG1_CONV_ERR_CLR_Msk

#define WJ_ADC_ACFG1_CAL_ERR_CLR_Pos                   (30)
#define WJ_ADC_ACFG1_CAL_ERR_CLR_Msk                   (0x1U << WJ_ADC_ACFG1_CAL_ERR_CLR_Pos)
#define WJ_ADC_ACFG1_CAL_ERR_CLR_EN                    WJ_ADC_ACFG1_CAL_ERR_CLR_Msk

#define WJ_ADC_ACFG1_EOCAL_CLR_Pos                     (4)
#define WJ_ADC_ACFG1_EOCAL_CLR_Msk                     (0x1U << WJ_ADC_ACFG1_EOCAL_CLR_Pos)
#define WJ_ADC_ACFG1_EOCAL_CLR_EN                      WJ_ADC_ACFG1_EOCAL_CLR_Msk

#define WJ_ADC_ACFG1_RESET_Pos                         (3)
#define WJ_ADC_ACFG1_RESET_Msk                         (0x1U << WJ_ADC_ACFG1_RESET_Pos)
#define WJ_ADC_ACFG1_RESET                             WJ_ADC_ACFG1_RESET_Msk

#define WJ_ADC_ACFG1_POR_Pos                           (2)
#define WJ_ADC_ACFG1_POR_Msk                           (0x1U << WJ_ADC_ACFG1_POR_Pos)
#define WJ_ADC_ACFG1_POR                               WJ_ADC_ACFG1_POR_Msk

#define WJ_ADC_CAL_EN_Pos                              (0)
#define WJ_ADC_CAL_EN_Msk                              (0x1U << WJ_ADC_CAL_EN_Pos)
#define WJ_ADC_CAL_EN                                  WJ_ADC_CAL_EN_Msk

/* ADC_ASTATUS, offset: 0x58 */
#define WJ_ADC_ASTATUS_CONV_ERR_Pos                    (31)
#define WJ_ADC_ASTATUS_CONV_ERR_Msk                    (0x1U << WJ_ADC_ASTATUS_CONV_ERR_Pos)
#define WJ_ADC_ASTATUS_CONV_ERR                        WJ_ADC_ASTATUS_CONV_ERR_Msk

#define WJ_ADC_ASTATUS_COL_ERR_Pos                     (30)
#define WJ_ADC_ASTATUS_COL_ERR_Msk                     (0x1U << WJ_ADC_ASTATUS_COL_ERR_Pos)
#define WJ_ADC_ASTATUS_COL_ERR                         WJ_ADC_ASTATUS_COL_ERR_Msk

#define WJ_ADC_ASTATUS_BUSY_Pos                        (1)
#define WJ_ADC_ASTATUS_BUSY_Msk                        (0x1U << WJ_ADC_ASTATUS_BUSY_Pos)
#define WJ_ADC_ASTATUS_BUSY                            WJ_ADC_ASTATUS_BUSY_Msk

#define WJ_ADC_ASTATUS_EOCAL_Pos                       (0)
#define WJ_ADC_ASTATUS_EOCAL_Msk                       (0x1U << WJ_ADC_ASTATUS_EOCAL_Pos)
#define WJ_ADC_ASTATUS_EOCAL                           WJ_ADC_ASTATUS_EOCAL_Msk

/* CONVERR, offset: 0x60 */
#define WJ_ADC_CONVERR_Pos                             (0)
#define WJ_ADC_CONVERR_Msk                             (0x1U << WJ_ADC_CONVERR_INTEN_Pos)
#define WJ_ADC_CONVERR_EN                              WJ_ADC_CONVERR_INTEN_Pos

typedef struct {
    __IOM uint32_t ADC_CR;           /* offset: 0x00 (R/W) A/D Control Register        */
    __IOM uint32_t ADC_SCFG;         /* offset: 0x04 (R/W) A/D Sequence config Register        */
    __IOM uint32_t ADC_OSVCNU;       /* offset: 0x08 (R/W) A/D ADC order sequence0 valid channel number control Register        */
    __IOM uint32_t ADC_HPSVCNU;      /* offset: 0x0c (R/W) A/D ADC order sequence0 valid channel number control Register        */
    __IOM uint32_t ADC_SR;           /* offset: 0x10 (R/W) A/D Status Register        */
    __IM uint32_t ADC_DR;            /* offset: 0x14 (R) A/D Data Register        */
    __IOM uint32_t ADC_CMPCR;        /* offset: 0x18 (R/W) A/D Comparator Control Register        */
    __IOM uint32_t ADC_CMPR0;        /* offset: 0x1c (R/W) A/D Comparator Control Register        */
    __IOM uint32_t ADC_CMPR1;        /* offset: 0x20 (R/W) A/D Comparator Control Register        */
    __IOM uint32_t ADC_IE;           /* offset: 0x24 (R/W) A/D Interrupt Enable Register        */
    __IOM uint32_t ADC_IF;           /* offset: 0x28 (R/W) A/D Interrupt Flag Register        */
    __OM uint32_t ADC_SWTC;          /* offset: 0x2c (W) A/D software starting Register        */
    __IOM uint32_t OFFSETDR;         /* offset: 0x30 (R/W) A/D conversion result offset data Register */
    uint32_t RESERVED0[2];
    __IOM uint32_t AADCCFGEX;        /* offset: 0x3c (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_OSCHCDCFG0;   /* offset: 0x40 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_OSCHCDCFG1;   /* offset: 0x44 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_HPSCHCDCFG0;  /* offset: 0x48 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_HPSCHCDCFG1;  /* offset: 0x4c (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_ACFG0;        /* offset: 0x50 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_ACFG1;        /* offset: 0x54 (R/W) Analog configure of extend figures Register */
    __IM uint32_t ADC_ASTATUS;       /* offset: 0x58 (R/W) Analog configure of extend figures Register */
    uint32_t RESERVED1[0];
    __IOM uint32_t CONVERR;          /* offset: 0x60 (R/W) ADC conversion error interrupt Register */
} wj_adc_regs_t;

static inline void wj_adc_config_sample_clock(wj_adc_regs_t *adc_base, uint32_t clock)
{
    adc_base->ADC_CR &= ~WJ_ADC_CR_NUSMPCLR_Msk;
    adc_base->ADC_CR |= (clock - 1U) << WJ_ADC_CR_NUSMPCLR_Pos;
}

static inline void wj_adc_set_left_align(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CR |= WJ_ADC_CR_DLRC_LEFT_ALIGN;
}

static inline void wj_adc_set_right_align(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CR |= WJ_ADC_CR_DLRC_RIGHT_ALIGN;
}

static inline void wj_adc_set_plus_offset(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CR &= ~WJ_ADC_CR_OFSTMC_Msk;
    adc_base->ADC_CR |= WJ_ADC_CR_OFSTMC_PLUS;
}

static inline void wj_adc_set_minus_offset(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CR &= ~WJ_ADC_CR_OFSTMC_Msk;
    adc_base->ADC_CR |= WJ_ADC_CR_OFSTMC_MINUS;
}


static inline void wj_adc_power_on(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CR &= ~WJ_ADC_CR_ADCPWD_Msk;
    adc_base->ADC_CR |= WJ_ADC_CR_ADCPWD_EN;
}

static inline void wj_adc_power_off(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CR &= ~WJ_ADC_CR_ADCPWD_EN;
}

static inline void wj_adc_set_hps_into_sms_immediately(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_HPSINTOSMS_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_HPSINTOSMS_IMMEDIATELY;
}

static inline void wj_adc_set_hps_into_sms_delay(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_HPSINTOSMS_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_HPSINTOSMS_DELAY;
}

static inline void wj_adc_enable_hps_auto_conversion(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_HPSAUTO_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_HPSAUTO_EN;
}

static inline void wj_adc_disable_hps_auto_conversion(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_HPSAUTO_Msk;
}

static inline void wj_adc_set_os_single_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_OSMC_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_OSMC_SINGLE;
}

static inline void wj_adc_set_os_dis_continue_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_OSMC_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_OSMC_DIS_CONTINUE;
}

static inline void wj_adc_set_os_continue_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_OSMC_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_OSMC_CONTINUE;
}

static inline void wj_adc_set_os_scan_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_OSMC_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_OSMC_SCAN;
}

static inline uint32_t wj_adc_get_os_scan_mode(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_SCFG;
}

static inline uint32_t wj_adc_get_os_mode(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_SCFG;
}

static inline void wj_adc_set_hps_single_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_HPSMC_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_HPSMC_SINGLE;
}

static inline void wj_adc_set_hps_dis_continue_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_HPSMC_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_HPSMC_SINGLE;
}

static inline void wj_adc_set_hps_continue_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_HPSMC_Msk;
    adc_base->ADC_SCFG |= WJ_ADC_SCFG_HPSMC_CONTINUE;
}

static inline uint32_t  wj_adc_get_os_channel_nums(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_OSVCNU;
}

static inline uint32_t wj_adc_get_status(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_SR;
}

static inline uint32_t wj_adc_get_data_valid_channel(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_SR >> WJ_ADC_SR_DVC_Pos) & WJ_ADC_SR_DVC_Msk;
}

static inline uint32_t wj_adc_get_current_conv_channel(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_SR & WJ_ADC_SR_CCC_Msk) >> WJ_ADC_SR_CCC_Pos;
}

static inline uint32_t wj_adc_get_data(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_DR;

}

//avgclrc
static inline uint32_t wj_adc_get_comp_status(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_CMPCR & WJ_ADC_CMPCR_DCMPMC_Msk;
}


static inline void wj_adc_set_comp_one_channel(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CMPCR &= ~WJ_ADC_CMPCR_CMPSOCHS_Msk;
    adc_base->ADC_CMPCR |= WJ_ADC_CMPCR_CMPSOCHS_ONE_CHANNEL;
}

static inline void wj_adc_set_comp_order_sequence(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CMPCR &= ~WJ_ADC_CMPCR_CMPSOCHS_Msk;
    adc_base->ADC_CMPCR |= WJ_ADC_CMPCR_CMPSOCHS_ORDER_SEQUENCE;
}

static inline void wj_adc_set_comp_high_priority_channel(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CMPCR &= ~WJ_ADC_CMPCR_CMPSOCHS_Msk;
    adc_base->ADC_CMPCR |= WJ_ADC_CMPCR_CMPSOCHS_HIGH_PRIORITY_SEQUENCE;
}

static inline void wj_adc_set_comp_all_channel(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CMPCR &= ~WJ_ADC_CMPCR_CMPSOCHS_Msk;
    adc_base->ADC_CMPCR |= WJ_ADC_CMPCR_CMPSOCHS_ALL;
}

static inline void wj_adc_config_comp_value0(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CMPR0 = value;
}

static inline void wj_adc_config_comp_value1(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CMPR1 = value;
}

static inline void wj_adc_enable_osc_cmplt_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE |= WJ_ADC_IE_OSSCEDIE;
}

static inline void wj_adc_disable_osc_cmplt_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE &= ~WJ_ADC_IE_OSSCEDIE;
}

static inline void wj_adc_enable_hpsc_cmplt_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE |= WJ_ADC_IE_HPSSCEDIE;
}

static inline void wj_adc_disable_hpsc_cmplt_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE &= ~WJ_ADC_IE_HPSSCEDIE;
}

static inline void wj_adc_enable_data_cmp_wrong_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE |= WJ_ADC_IE_CMPIE;
}

static inline void wj_adc_disable_data_cmp_wrong_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE &= ~WJ_ADC_IE_CMPIE;
}

static inline void wj_adc_enable_data_overwrite_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE |= WJ_ADC_IE_OVWTIE;
}

static inline void wj_adc_disable_data_overwrite_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE &= ~WJ_ADC_IE_OVWTIE;
}

static inline void wj_adc_enable_hps_eoc_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE |= WJ_ADC_IE_HSSS_EOCIE;
}

static inline void wj_adc_disable_hps_eoc_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE &= ~WJ_ADC_IE_HSSS_EOCIE;
}

static inline void wj_adc_enable_os_eoc_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE |= WJ_ADC_IE_OSS_EOCIE;
}

static inline void wj_adc_disable_os_eoc_irq(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_IE &= ~WJ_ADC_IE_OSS_EOCIE;
}

static inline uint32_t wj_adc_get_interrupt_status(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_IF;
}

static inline void wj_adc_clr_interrupt_status(wj_adc_regs_t *adc_base, uint32_t status)
{
    adc_base->ADC_IF |= status;
}

static inline void wj_adc_turn_off_conv(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SWTC |= WJ_ADC_SWTC_ADCCOFF;
}

static inline void wj_adc_hps_trigger(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SWTC |= WJ_ADC_SWTC_SWHPSTR;
}

static inline void wj_adc_os_trigger(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_SWTC |= WJ_ADC_SWTC_SWOSTR;
}

static inline void wj_adc_config_data_offset(wj_adc_regs_t *adc_base, uint32_t offset)
{
    adc_base->OFFSETDR = offset;
}

static inline void wj_adc_config_eoc_mode(wj_adc_regs_t *adc_base, uint32_t mode)
{
    adc_base->AADCCFGEX |= (mode << WJ_ADC_AADCCFGEX_EOCMD_Pos);
}

static inline void wj_adc_config_conv_clock(wj_adc_regs_t *adc_base, uint32_t clock)
{
    adc_base->AADCCFGEX &= ~WJ_ADC_AADCCFGEX_NUCLKCNV_Msk;
    adc_base->AADCCFGEX |= (clock << WJ_ADC_AADCCFGEX_NUCLKCNV_Pos);
}

static inline void wj_adc_enable_calibration(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_CONV_CAL_EN;
}

static inline void wj_adc_disable_calibration(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_CONV_CAL_EN;
}

static inline void wj_adc_set_conv_max_rate_max_4m(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_ADCCR_4M;
}

static inline void wj_adc_set_conv_max_rate_max_32m(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_ADCCR_32M;
}

static inline void wj_adc_set_input_ch6_7_pair_diff(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_AINS3_Msk;
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_AINS3_DIFF;
}

static inline void wj_adc_set_input_ch6_7_pair_single(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_AINS3_Msk;
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_AINS3_SINGLE;
}

static inline void wj_adc_set_input_ch4_5_pair_diff(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_AINS0_Msk;
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_AINS0_DIFF;
}

static inline void wj_adc_set_input_ch4_5_pair_single(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_AINS0_Msk;
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_AINS0_SINGLE;
}

static inline void wj_adc_set_input_ch2_3_pair_diff(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_AINS2_Msk;
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_AINS2_DIFF;
}

static inline void wj_adc_set_input_ch2_3_pair_single(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_AINS2_Msk;
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_AINS2_SINGLE;
}

static inline void wj_adc_set_input_ch0_1_pair_diff(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_AINS1_Msk;
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_AINS1_DIFF;
}

static inline void wj_adc_set_input_ch0_1_pair_single(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG0 &= ~WJ_ADC_ACFG0_AINS1_Msk;
    adc_base->ADC_ACFG0 |= WJ_ADC_ACFG0_AINS1_SINGLE;
}

static inline void wj_adc_clr_conv_err(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 |= WJ_ADC_ACFG1_CONV_ERR_CLR_EN;
}

static inline void wj_adc_clr_cal_err(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 |= WJ_ADC_ACFG1_CAL_ERR_CLR_EN;
}

static inline void wj_adc_clr_eocal_err(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 |= WJ_ADC_ACFG1_EOCAL_CLR_EN;
}

static inline void wj_adc_rst_all_digital_register(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 |= WJ_ADC_ACFG1_RESET;
}

static inline void wj_adc_set_all_digital_register_normal(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 &= ~WJ_ADC_ACFG1_RESET;
}

static inline void wj_adc_rst_all_digital_analog_register(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 |= WJ_ADC_ACFG1_POR;
}

static inline void wj_adc_set_all_digital_analog_register_normal(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 &= ~WJ_ADC_ACFG1_POR;
}

static inline void wj_adc_enable_cal_cycle(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 |= WJ_ADC_CAL_EN;
}

static inline void wj_adc_disable_cal_cycle(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ACFG1 &= ~WJ_ADC_CAL_EN;
}

static inline uint32_t wj_adc_get_analog_status(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_ASTATUS;
}

static inline void wj_adc_enable_conv_error_irq(wj_adc_regs_t *adc_base)
{
    adc_base->CONVERR |= WJ_ADC_ASTATUS_CONV_ERR;
}

static inline void wj_adc_disable_conv_error_irq(wj_adc_regs_t *adc_base)
{
    adc_base->CONVERR &= ~WJ_ADC_ASTATUS_CONV_ERR;
}

extern int wj_adc_get_os_channels(wj_adc_regs_t *adc_base, uint8_t *channels, uint8_t *num);
extern void wj_adc_config_conv_clock_num(wj_adc_regs_t *adc_base, uint32_t num);
extern void wj_adc_config_os_channel(wj_adc_regs_t *adc_base, uint32_t rank, uint32_t channel);
extern void wj_adc_config_hps_channel(wj_adc_regs_t *adc_base, uint32_t rank, uint32_t channel);
extern void wj_adc_reset_regs(wj_adc_regs_t *adc_base);
extern void wj_adc_config_hardware_average(wj_adc_regs_t *adc_base, uint32_t average);
extern void wj_adc_config_data_bit(wj_adc_regs_t *adc_base, uint32_t data_bit);
extern uint32_t wj_adc_config_conversion_clock_div(wj_adc_regs_t *adc_base, uint32_t div);
extern uint32_t wj_adc_get_conversion_clock_div(wj_adc_regs_t *adc_base);
extern void wj_adc_config_hpsdc_num_in_dis_continue_mode(wj_adc_regs_t *adc_base, uint32_t nums);
extern void wj_adc_config_osdc_num_in_dis_continue_mode(wj_adc_regs_t *adc_base, uint32_t nums);
extern void wj_adc_config_os_channel_nums(wj_adc_regs_t *adc_base, uint32_t nums);
extern void wj_adc_config_hps_channel_nums(wj_adc_regs_t *adc_base, uint32_t nums);
extern void wj_adc_config_comp_channel(wj_adc_regs_t *adc_base, uint32_t channel);
extern uint32_t wj_adc_get_os_scan_mode(wj_adc_regs_t *adc_base);
#ifdef __cplusplus
}
#endif

#endif  /* _WJ_ADC_LL_H_*/
