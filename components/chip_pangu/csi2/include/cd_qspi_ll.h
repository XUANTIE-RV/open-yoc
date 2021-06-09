/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     cs_qspi_ll.h
 * @brief
 * @version
 * @date     2020-02-22
 ******************************************************************************/

#ifndef _CS_QSPI_LL_H_
#define _CS_QSPI_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CFGR, offset: 0x00 */
#define CD_QSPI_CFGR_IDLE_Pos                 ( 31U )
#define CD_QSPI_CFGR_IDLE_Msk                 ( 0x1U << CD_QSPI_CFGR_IDLE_Pos )
#define CD_QSPI_CFGR_IDLE_EN                  CD_QSPI_CFGR_IDLE_Msk

#define CD_QSPI_CFGR_DTR_Pos                  (24U)
#define CD_QSPI_CFGR_DTR_Msk                  (0x1U << CD_QSPI_CFGR_DTR_Pos)
#define CD_QSPI_CFGR_DTR_EN                   CD_QSPI_CFGR_DTR_Msk

#define CD_QSPI_CFGR_AHBDC_Pos                (23U)
#define CD_QSPI_CFGR_AHBDC_Msk                (0x1U << CD_QSPI_CFGR_AHBDC_Pos)
#define CD_QSPI_CFGR_AHBDC_EN                 CD_QSPI_CFGR_AHBDC_Msk

#define CD_QSPI_CFGR_BDDIV_Pos                (19U)
#define CD_QSPI_CFGR_BDDIV_Msk                (0xFU << CD_QSPI_CFGR_BDDIV_Pos)

#define CD_QSPI_CFGR_XIP_Pos                  (17U)
#define CD_QSPI_CFGR_XIP_Msk                  (0x1U << CD_QSPI_CFGR_XIP_Pos)
#define CD_QSPI_CFGR_XIP_EN                   CD_QSPI_CFGR_XIP_Msk

#define CD_QSPI_CFGR_AHBARM_Pos               (16U)
#define CD_QSPI_CFGR_AHBARM_Msk               (0x1U << CD_QSPI_CFGR_AHBARM_Pos)
#define CD_QSPI_CFGR_AHBARM_EN                CD_QSPI_CFGR_AHBARM_Msk

#define CD_QSPI_CFGR_DMA_Pos                  (15U)
#define CD_QSPI_CFGR_DMA_Msk                  (0x1U << CD_QSPI_CFGR_DMA_Pos)
#define CD_QSPI_CFGR_DMA_EN                   CD_QSPI_CFGR_DMA_Msk

#define CD_QSPI_CFGR_WPP_Pos                  (14U)
#define CD_QSPI_CFGR_WPP_Msk                  (0x1U << CD_QSPI_CFGR_WPP_Pos)
#define CD_QSPI_CFGR_WPP_EN                   CD_QSPI_CFGR_WPP_Msk

#define CD_QSPI_CFGR_PSCL_Pos                 (10U)
#define CD_QSPI_CFGR_PSCL_Msk                 (0xFU << CD_QSPI_CFGR_PSCL_Pos)

#define CD_QSPI_CFGR_PSD_Pos                  (9U)
#define CD_QSPI_CFGR_PSD_Msk                  (0x1U << CD_QSPI_CFGR_PSD_Pos)
#define CD_QSPI_CFGR_PSD_EN                   CD_QSPI_CFGR_PSD_Msk

#define CD_QSPI_CFGR_LEGACY_Pos               (8U)
#define CD_QSPI_CFGR_LEGACY_Msk               (0x1U << CD_QSPI_CFGR_LEGACY_Pos)
#define CD_QSPI_CFGR_LEGACY_EN                CD_QSPI_CFGR_LEGACY_Msk

#define CD_QSPI_CFGR_DAC_Pos                  (7U)
#define CD_QSPI_CFGR_DAC_Msk                  (0x1U << CD_QSPI_CFGR_DAC_Pos)
#define CD_QSPI_CFGR_DAC_EN                   CD_QSPI_CFGR_DAC_Msk

#define CD_QSPI_CFGR_CPHA_Pos                 (2U)
#define CD_QSPI_CFGR_CPHA_Msk                 (0x1U << CD_QSPI_CFGR_CPHA_Pos)
#define CD_QSPI_CFGR_CPHA_EN                  CD_QSPI_CFGR_CPHA_Msk

#define CD_QSPI_CFGR_CPOL_Pos                 (1U)
#define CD_QSPI_CFGR_CPOL_Msk                 (0x1U << CD_QSPI_CFGR_CPOL_Pos)
#define CD_QSPI_CFGR_CPOL_EN                  CD_QSPI_CFGR_CPOL_Msk

#define CD_QSPI_CFGR_ENABLE_Pos               (0U)
#define CD_QSPI_CFGR_ENABLE_Msk               (0x1U << CD_QSPI_CFGR_ENABLE_Pos)
#define CD_QSPI_CFGR_ENABLE                   CD_QSPI_CFGR_ENABLE_Msk

/* DRIR, offset: 0x00 */
#define CD_QSPI_DRIR_DMYC_Pos                 (24U)
#define CD_QSPI_DRIR_DMYC_Msk                 (0x1FU << CD_QSPI_DRIR_DMYC_Pos)

#define CD_QSPI_DRIR_MB_Pos                   (20U)
#define CD_QSPI_DRIR_MB_Msk                   (0x1U << CD_QSPI_DRIR_MB_Pos)
#define CD_QSPI_DRIR_MB_EN                    CD_QSPI_DRIR_MB_Msk

#define CD_QSPI_DRIR_DTT_Pos                  (16U)
#define CD_QSPI_DRIR_DTT_Msk                  (0x3U << CD_QSPI_DRIR_DTT_Pos)

#define CD_QSPI_DRIR_ATT_Pos                  (12U)
#define CD_QSPI_DRIR_ATT_Msk                  (0x3U << CD_QSPI_DRIR_ATT_Pos)

#define CD_QSPI_DRIR_DDR_Pos                  (10U)
#define CD_QSPI_DRIR_DDR_Msk                  (0x1U << CD_QSPI_DRIR_DDR_Pos)
#define CD_QSPI_DRIR_DDR_EN                   CD_QSPI_DRIR_DDR_Msk

#define CD_QSPI_DRIR_INST_Pos                 (8U)
#define CD_QSPI_DRIR_INST_Msk                 (0x3U << CD_QSPI_DRIR_INST_Pos)

#define CD_QSPI_DRIR_ROPC_Pos                 (0U)
#define CD_QSPI_DRIR_ROPC_Msk                 (0xFFU << CD_QSPI_DRIR_ROPC_Pos)

/* DWIR, offset: 0x00 */
#define CD_QSPI_DWIR_DMYC_Pos                 (24U)
#define CD_QSPI_DWIR_DMYC_Msk                 (0x1FU << CD_QSPI_DWIR_DMYC_Pos)

#define CD_QSPI_DWIR_DTT_Pos                  (16U)
#define CD_QSPI_DWIR_DTT_Msk                  (0x3U << CD_QSPI_DRIR_DTT_Pos)

#define CD_QSPI_DWIR_ATT_Pos                  (12U)
#define CD_QSPI_DWIR_ATT_Msk                  (0x3U << CD_QSPI_DRIR_ATT_Pos)

#define CD_QSPI_DWIR_DDR_Pos                  (10U)
#define CD_QSPI_DWIR_DDR_Msk                  (0x1U << CD_QSPI_DRIR_DDR_Pos)
#define CD_QSPI_DWIR_DDR_EN                   CD_QSPI_DRIR_DDR_Msk

#define CD_QSPI_DWIR_WEL_Pos                  (8U)
#define CD_QSPI_DWIR_WEL_Msk                  (0x1U << CD_QSPI_DWIR_WEL_Pos)
#define CD_QSPI_DWIR_WEL_EN                   CD_QSPI_DWIR_WEL_Msk

#define CD_QSPI_DWIR_WOPC_Pos                 (0U)
#define CD_QSPI_DWIR_WOPC_Msk                 (0xFFU << CD_QSPI_DRIR_ROPC_Pos)

/* DDR, offset: 0x00 */
#define CD_QSPI_DDR_CSDA_Pos                  (24U)
#define CD_QSPI_DDR_CSDA_Msk                  (0xFFU << CD_QSPI_DDR_CSDA_Pos)

#define CD_QSPI_DDR_CSDADS_Pos                (16U)
#define CD_QSPI_DDR_CSDADS_Msk                (0xFFU << CD_QSPI_DDR_CSDADS_Pos)

#define CD_QSPI_DDR_CSEOT_Pos                 (8U)
#define CD_QSPI_DDR_CSEOT_Msk                 (0xFFU << CD_QSPI_DDR_CSEOT_Pos)

#define CD_QSPI_DDR_CSSOT_Pos                 (0U)
#define CD_QSPI_DDR_CSSOT_Msk                 (0xFFU << CD_QSPI_DDR_CSSOT_Pos)

/* RDCR, offset: 0x00 */
#define CD_QSPI_RDCR_TXDD_Pos                 (16U)
#define CD_QSPI_RDCR_TXDD_Msk                 (0xFU << CD_QSPI_RDCR_TXDD_Pos)

#define CD_QSPI_RDCR_SES_Pos                  (5U)
#define CD_QSPI_RDCR_SES_Msk                  (0x1U << CD_QSPI_RDCR_SES_Pos)
#define CD_QSPI_RDCR_SES_FALLING              (0x0U << CD_QSPI_RDCR_SES_Pos)
#define CD_QSPI_RDCR_SES_RISING               (0x1U << CD_QSPI_RDCR_SES_Pos)

#define CD_QSPI_RDCR_RDCD_Msk                 (1U)
#define CD_QSPI_RDCR_RDCD_Pos                 (0xFU << CD_QSPI_RDCR_RDCD_Msk)

#define CD_QSPI_RDCR_LOOPBACK_Pos             (0U)
#define CD_QSPI_RDCR_LOOPBACK_Msk             (0x1U << CD_QSPI_RDCR_LOOPBACK_Pos)
#define CD_QSPI_RDCR_LOOPBACK_EN              CD_QSPI_RDCR_LOOPBACK_Msk

/* DSCR, offset: 0x00 */
#define CD_QSPI_DSCR_FS3_Pos                  (27U)
#define CD_QSPI_DSCR_FS3_Msk                  (0x3U << CD_QSPI_DSCR_FS0_Pos)

#define CD_QSPI_DSCR_FS2_Pos                  (25U)
#define CD_QSPI_DSCR_FS2_Msk                  (0x3U << CD_QSPI_DSCR_FS2_Pos)

#define CD_QSPI_DSCR_FS1_Pos                  (23U)
#define CD_QSPI_DSCR_FS1_Msk                  (0x3U << CD_QSPI_DSCR_FS1_Pos)

#define CD_QSPI_DSCR_FS0_Pos                  (21U)
#define CD_QSPI_DSCR_FS0_Msk                  (0x3U << CD_QSPI_DSCR_FS0_Pos)

#define CD_QSPI_DSCR_BS_Pos                   (16U)
#define CD_QSPI_DSCR_BS_Msk                   (0x1FU << CD_QSPI_DSCR_BS_Pos)

#define CD_QSPI_DSCR_PS_Pos                   (4U)
#define CD_QSPI_DSCR_PS_Msk                   (0xFFFU << CD_QSPI_DSCR_PS_Pos)

#define CD_QSPI_DSCR_AS_Pos                   (0U)
#define CD_QSPI_DSCR_AS_Msk                   (0xFU << CD_QSPI_DSCR_AS_Pos)

/* DPCFGR, offset: 0x00 */
#define CD_QSPI_DPCFGR_BSTNUM_Pos             (8U)
#define CD_QSPI_DPCFGR_BSTNUM_Msk             (0xFU << CD_QSPI_DPCFGR_BSTNUM_Pos)

#define CD_QSPI_DPCFGR_SIGNUM_Pos             (0U)
#define CD_QSPI_DPCFGR_SIGNUM_Msk             (0xFU << CD_QSPI_DPCFGR_SIGNUM_Pos)

/* SRAMFLR, offset: 0x00 */
#define CD_QSPI_SRAMFLR_TXPAT_Pos             (16U)
#define CD_QSPI_SRAMFLR_TXPAT_Msk             (0xFFU << CD_QSPI_SRAMFLR_TXPAT_Pos)

#define CD_QSPI_SRAMFLR_RXPAT_Pos             (0U)
#define CD_QSPI_SRAMFLR_RXPAT_Msk             (0xFFU << CD_QSPI_SRAMFLR_RXPAT_Pos)

/* WCCTRLR, offset: 0x00 */
#define CD_QSPI_WCCTRLR_PRD_Pos               (24U)
#define CD_QSPI_WCCTRLR_PRD_Msk               (0xFFU << CD_QSPI_WCCTRLR_PRD_Pos)

#define CD_QSPI_WCCTRLR_PC_Pos                (16U)
#define CD_QSPI_WCCTRLR_PC_Msk                (0xFFU << CD_QSPI_WCCTRLR_PC_Pos)

#define CD_QSPI_WCCTRLR_PE_Pos                (15U)
#define CD_QSPI_WCCTRLR_PE_Msk                (0x1U << CD_QSPI_WCCTRLR_PE_Pos)
#define CD_QSPI_WCCTRLR_PE_EN                 CD_QSPI_WCCTRLR_PE_Msk

#define CD_QSPI_WCCTRLR_AP_Pos                (14U)
#define CD_QSPI_WCCTRLR_AP_Msk                (0x1U << CD_QSPI_WCCTRLR_AP_Pos)
#define CD_QSPI_WCCTRLR_AP_EN                 CD_QSPI_WCCTRLR_AP_Msk

#define CD_QSPI_WCCTRLR_PP_Pos                (13U)
#define CD_QSPI_WCCTRLR_PP_Msk                (0x1U << CD_QSPI_WCCTRLR_PP_Pos)
#define CD_QSPI_WCCTRLR_PP0                   (0x0U << CD_QSPI_WCCTRLR_PP_Pos)
#define CD_QSPI_WCCTRLR_PP1                   (0x1U << CD_QSPI_WCCTRLR_PP_Pos)

#define CD_QSPI_WCCTRLR_PB_Pos                (8U)
#define CD_QSPI_WCCTRLR_PB_Msk                (0x7U << CD_QSPI_WCCTRLR_PB_Pos)

#define CD_QSPI_WCCTRLR_POPC_Pos              (8U)
#define CD_QSPI_WCCTRLR_POPC_Msk              (0x7U << CD_QSPI_WCCTRLR_POPC_Pos)

/* IMR, offset: 0x2C */
#define CD_QSPI_IMR_MDF_Pos                    (0U)
#define CD_QSPI_IMR_MDF_Msk                    (0x1U << CD_QSPI_IMR_MDF_Pos)
#define CD_QSPI_IMR_MDF_EN                     (CD_QSPI_IMR_MDF_Msk)

#define CD_QSPI_IMR_UNDERFLOW_Pos              (1U)
#define CD_QSPI_IMR_UNDERFLOW_Msk              (0x1U << CD_QSPI_IMR_UNDERFLOW_Pos)
#define CD_QSPI_IMR_UNDERFLOW_EN               CD_QSPI_IMR_UNDERFLOW_Msk

#define CD_QSPI_IMR_COMPLETE_Pos               (2U)
#define CD_QSPI_IMR_COMPLETE_Msk               (0x1U << CD_QSPI_IMR_COMPLETE_Pos)
#define CD_QSPI_IMR_COMPLETE_EN                CD_QSPI_IMR_COMPLETE_Msk

#define CD_QSPI_IMR_INDREJ_Pos                 (3U)
#define CD_QSPI_IMR_INDREJ_Msk                 (0x1U << CD_QSPI_IMR_INDREJ_Pos)
#define CD_QSPI_IMR_INDREJ_EN                  CD_QSPI_IMR_INDREJ_Msk

#define CD_QSPI_IMR_WTREJ_Pos                  (4U)
#define CD_QSPI_IMR_WTREJ_Msk                  (0x1U << CD_QSPI_IMR_INDREJ_Pos)
#define CD_QSPI_IMR_WTREJ_EN                   CD_QSPI_IMR_WTREJ_Msk

#define CD_QSPI_IMR_ILLEGAL_Pos                (5U)
#define CD_QSPI_IMR_ILLEGAL_Msk                (0x1U << CD_QSPI_IMR_ILLEGAL_Pos)
#define CD_QSPI_IMR_ILLEGAL_EN                 CD_QSPI_IMR_ILLEGAL_Msk

#define CD_QSPI_IMR_WTLEVBREC_Pos              (6U)
#define CD_QSPI_IMR_WTLEVBREC_Msk              (0x1U << CD_QSPI_IMR_WTLEVBREC_Pos)
#define CD_QSPI_IMR_WTLEVBREC_EN               CD_QSPI_IMR_WTLEVBREC_Msk

#define CD_QSPI_IMR_OVERFLOW_Pos               (7U)
#define CD_QSPI_IMR_OVERFLOW_Msk               (0x1U << CD_QSPI_IMR_OVERFLOW_Pos)
#define CD_QSPI_IMR_OVERFLOW_EN                CD_QSPI_IMR_OVERFLOW_Msk

#define CD_QSPI_IMR_TFNF_Pos                   (8U)
#define CD_QSPI_IMR_TFNF_Msk                   (0x1U << CD_QSPI_IMR_TFNF_Pos)
#define CD_QSPI_IMR_TFNF_EN                    CD_QSPI_IMR_TFNF_Msk

#define CD_QSPI_IMR_TFF_Pos                    (9U)
#define CD_QSPI_IMR_TFF_Msk                    (0x1U << CD_QSPI_IMR_TFF_Pos)
#define CD_QSPI_IMR_TFF_EN                     CD_QSPI_IMR_TFF_Msk

#define CD_QSPI_IMR_RFNE_Pos                   (10U)
#define CD_QSPI_IMR_RFNE_Msk                   (0x1U << CD_QSPI_IMR_RFNE_Pos)
#define CD_QSPI_IMR_RFNE_EN                    CD_QSPI_IMR_RFNE_Msk

#define CD_QSPI_IMR_RFF_Pos                    (11U)
#define CD_QSPI_IMR_RFF_Msk                    (0x1U << CD_QSPI_IMR_RFF_Pos)
#define CD_QSPI_IMR_RFF_EN                     CD_QSPI_IMR_RFF_Msk

#define CD_QSPI_IMR_SF_Pos                     (12U)
#define CD_QSPI_IMR_SF_Msk                     (0x1U << CD_QSPI_IMR_SF_Pos)
#define CD_QSPI_IMR_SF_EN                      CD_QSPI_IMR_SF_Msk

#define CD_QSPI_IMR_PE_Pos                     (13U)
#define CD_QSPI_IMR_PE_Msk                     (0x1U << CD_QSPI_IMR_PE_Pos )
#define CD_QSPI_IMR_PE_EN                      CD_QSPI_IMR_PE_Msk

#define CD_QSPI_IMR_STIGCPLT_Pos               (14U)
#define CD_QSPI_IMR_STIGCPLT_Msk               (0x1U << CD_QSPI_IMR_STIGCPLT_Pos)
#define CD_QSPI_IMR_STIGCPLT_EN                CD_QSPI_IMR_STIGCPLT_Msk

/* WRP, offset: 0x58 */
#define CD_QSPI_WPR_WPEB_Pos                   (1U)
#define CD_QSPI_WPR_WPEB_Msk                   (0x1U << CD_QSPI_WPR_WPEB_Pos)
#define CD_QSPI_WPR_WPEB_EN                    CD_QSPI_WPR_WPEB_Msk

#define CD_QSPI_WPR_WPEBINVER_Pos              (0U)
#define CD_QSPI_WPR_WPEBINVER_Msk              (0x1U << CD_QSPI_WPR_WPEBINVER_Pos)
#define CD_QSPI_WPR_WPEBINVER_EN               CD_QSPI_WPR_WPEBINVER_Msk

/* IRTCR, offset: 0x58 */
#define CD_QSPI_IRTCR_IOCN_Pos                (6U)
#define CD_QSPI_IRTCR_IOCN_Msk                (0x3U << CD_QSPI_IRTCR_IOCN_Pos)

/* IRTCR, offset: 0x58 */
#define CD_QSPI_IRTCR_IS_Pos                  (2U)
#define CD_QSPI_IRTCR_IS_Msk                  (0xFU << CD_QSPI_IRTCR_IS_Pos)

#define CD_QSPI_IRTCR_CTRL_START_Pos          (0U)
#define CD_QSPI_IRTCR_CTRL_START_Msk          (0x1U << CD_QSPI_IRTCR_CTRL_START_Pos)
#define CD_QSPI_IRTCR_CTRL_START              CD_QSPI_IRTCR_CTRL_START_Msk

#define CD_QSPI_IRTCR_CTRL_STOP_Pos           (1U)
#define CD_QSPI_IRTCR_CTRL_STOP_Msk           (0x1U << CD_QSPI_IRTCR_CTRL_STOP_Pos)
#define CD_QSPI_IRTCR_CTRL_STOP               CD_QSPI_IRTCR_CTRL_STOP_Msk

/* IWTCR, offset: 0x58 */
#define CD_QSPI_IWTCR_IOCN_Pos                (6U)
#define CD_QSPI_IWTCR_IOCN_Msk                (0x3U << CD_QSPI_IWTCR_IOCN_Pos)

#define CD_QSPI_IWTCR_IS_Pos                  (2U)
#define CD_QSPI_IWTCR_IS_Msk                  (0xFU << CD_QSPI_IWTCR_IS_Pos)

#define CD_QSPI_IWTCR_CTRL_START_Pos          (0U)
#define CD_QSPI_IWTCR_CTRL_START_Msk          (0x1U << CD_QSPI_IRTCR_CTRL_START_Pos)
#define CD_QSPI_IWTCR_CTRL_START              CD_QSPI_IRTCR_CTRL_START_Msk

#define CD_QSPI_IWTCR_CTRL_STOP_Pos           (1U)
#define CD_QSPI_IWTCR_CTRL_STOP_Msk           (0x1U << CD_QSPI_IRTCR_CTRL_Pos)
#define CD_QSPI_IWTCR_CTRL_STOP               CD_QSPI_IRTCR_CTRL_STOP_Msk

/* FCCR, offset: 0x58 */
#define CD_QSPI_FCCR_OPC_Pos                  ( 24U )
#define CD_QSPI_FCCR_OPC_Msk                  (0xFFU << CD_QSPI_FCCR_OPC_Pos)

#define CD_QSPI_FCCR_RD_Pos                   ( 23U )
#define CD_QSPI_FCCR_RD_Msk                   ( 0x1U <<  CD_QSPI_FCCR_RD_Pos)
#define CD_QSPI_FCCR_RD_EN                    CD_QSPI_FCCR_RD_Msk

#define CD_QSPI_FCCR_NORDB_Pos                (20U)
#define CD_QSPI_FCCR_NORDB_Msk                (0x7U << CD_QSPI_FCCR_NORDB_Pos)

#define CD_QSPI_FCCR_CA_Pos                   (19U)
#define CD_QSPI_FCCR_CA_Msk                   (0x1U << CD_QSPI_FCCR_CA_Pos)
#define CD_QSPI_FCCR_CA_EN                    CD_QSPI_FCCR_CA_Msk

#define CD_QSPI_FCCR_MDB_Pos                   (18U)
#define CD_QSPI_FCCR_MDB_Msk                   (0x1U << CD_QSPI_FCCR_MDB_Pos)
#define CD_QSPI_FCCR_MDB_EN                    CD_QSPI_FCCR_MDB_Msk

#define CD_QSPI_FCCR_NOAB_Pos                 (16U)
#define CD_QSPI_FCCR_NOAB_Msk                 (0x3U << CD_QSPI_FCCR_NOAB_Pos)

#define CD_QSPI_FCCR_WD_Pos                   (15U)
#define CD_QSPI_FCCR_WD_Msk                   (0x1U << CD_QSPI_FCCR_WD_Pos)
#define CD_QSPI_FCCR_WD_EN                    CD_QSPI_FCCR_WD_Msk

#define CD_QSPI_FCCR_NOWDB_Pos                (12U)
#define CD_QSPI_FCCR_NOWDB_Msk                (0x7U << CD_QSPI_FCCR_NOWDB_Pos)

#define CD_QSPI_FCCR_NODMY_Pos                (7U)
#define CD_QSPI_FCCR_NODMY_Msk                (0x1FU << CD_QSPI_FCCR_NODMY_Pos)

#define CD_QSPI_FCCR_MB_Pos                   (2U)
#define CD_QSPI_FCCR_MB_Msk                   (0x1U << CD_QSPI_FCCR_MB_Pos)
#define CD_QSPI_FCCR_MB_EN                    CD_QSPI_FCCR_MB_Msk

#define CD_QSPI_FCCR_PS_Pos                   (1U)
#define CD_QSPI_FCCR_PS_Msk                   (0x1U << CD_QSPI_FCCR_PS_Pos)
#define CD_QSPI_FCCR_PS_EN                    CD_QSPI_FCCR_PS_Msk

#define CD_QSPI_FCCR_EXEC_Pos                 (0U)
#define CD_QSPI_FCCR_EXEC_Msk                 (0x1U << CD_QSPI_FCCR_EXEC_Pos)
#define CD_QSPI_FCCR_EXEC_EN                  CD_QSPI_FCCR_EXEC_Msk

/* PFSR, offset: 0x58 */
#define CD_QSPI_PFSR_NODCFAP_Pos              (16U)
#define CD_QSPI_PFSR_NODCFAP_Msk              (0xFU << CD_QSPI_PFSR_NODCFAP_Pos)

#define CD_QSPI_PFSR_PSV_Pos                  (8U)
#define CD_QSPI_PFSR_PSV_Msk                  (0x1U << CD_QSPI_PFSR_PSV_Pos)

#define CD_QSPI_PFSR_FS_Pos                   (0U)
#define CD_QSPI_PFSR_FS_Msk                   (0xFFU << CD_QSPI_PFSR_FS_Pos)

/* FCCMR, offset: 0x58 */
#define CD_QSPI_FCCMR_MBA_Pos                 (20U)
#define CD_QSPI_FCCMR_MBA_Msk                 (0xFFU << CD_QSPI_FCCMR_MBA_Pos)

#define CD_QSPI_FCCMR_MBRBN_Pos               (16U)
#define CD_QSPI_FCCMR_MBRBN_Msk               (0x7U << CD_QSPI_FCCMR_MBRBN_Pos)

#define CD_QSPI_FCCMR_MBRD_Pos                (8U)
#define CD_QSPI_FCCMR_MBRD_Msk                (0x7U << CD_QSPI_FCCMR_MBRBN_Pos)

#define CD_QSPI_FCCMR_MBDRIP_Pos              (1U)
#define CD_QSPI_FCCMR_MBDRIP_Msk              (0x1U << CD_QSPI_FCCMR_MBDRIP_Pos)

#define CD_QSPI_FCCMR_TIMBDR_Pos              (0U)
#define CD_QSPI_FCCMR_TIMBDR_Msk              (0x1U << CD_QSPI_FCCMR_TIMBDR_Pos)
#define CD_QSPI_FCCMR_TIMBDR_EN               CD_QSPI_FCCMR_TIMBDR_Msk

typedef struct {
    __IOM uint32_t CFGR;          /* Offset: 0x000 (R/W)  QSPI Configuration Register */
    __IOM uint32_t DRIR;          /* Offset: 0x004 (R/W)  Device Read Instruction Register */
    __IOM uint32_t DWIR;          /* Offset: 0x008 (R/W)  Device Write Instruction Register */
    __IOM uint32_t DDR;           /* Offset: 0x00C (R/W)  Device Delay Register */
    __IOM uint32_t RDCR;          /* Offset: 0x010 (R/W)  Read Data Capture Register */
    __IOM uint32_t DSCFGR;        /* Offset: 0x014 (R/W)  Device Size Configuration Register */
    __IOM uint32_t SRAMPCFGR;     /* Offset: 0x018 (R/W)  SRAM Partition Configuration Register */
    __IOM uint32_t IAATR;         /* Offset: 0x01C (R/W)  Indirect AHB Address Trigger Register */
    __IOM uint32_t DPCFGR;        /* Offset: 0x020 (R/W)  DMA Peripheral Configuration Register */
    __IOM uint32_t RMAR;          /* Offset: 0x024 (R/W)  Remap Address Register */
    __IOM uint32_t MBCFGR;        /* Offset: 0x028 (R/W)  Mode Bit Configuration Register */
    __IOM uint32_t SRAMFLR;       /* Offset: 0x02C (R/W)  SRAM Fill Level Register */
    __IOM uint32_t TXTHR;         /* Offset: 0x030 (R/W)  TX Threshold Register */
    __IOM uint32_t RXTHR;         /* Offset: 0x034 (R/W)  RX Threshold Register */
    __IOM uint32_t WCCTRLR;       /* Offset: 0x038 (R/W)  Write Completion Control Register */
    __IOM uint32_t PER;           /* Offset: 0x03C (R/W)  Polling Expiration Register */
    __IOM uint32_t ISR;           /* Offset: 0x040 (R/W)  Interrupt Status Register */
    __IOM uint32_t IMR;           /* Offset: 0x044 (R/W)  Interrupt Mask Register */
    uint32_t RESERVED[2];
    __IOM uint32_t LWPR;          /* Offset: 0x050 (R/W)  Lower Write Protection Register */
    __IOM uint32_t UWPR;          /* Offset: 0x054 (R/W)  Upper Write Protection Register */
    __IOM uint32_t WPR;           /* Offset: 0x058 (R/W)  Write Protection Register */
    uint32_t RESERVED1;
    __IOM uint32_t IRTCR;         /* Offset: 0x060 (R/W)  Indirect Read Transfer Control Register */
    __IOM uint32_t IRTWR;         /* Offset: 0x064 (R/W)  Indirect Read Transfer Watermark Register */
    __IOM uint32_t IRTSAR;        /* Offset: 0x068 (R/W)  Indirect Read Transfer Start Address Register */
    __IOM uint32_t IRTNBR;        /* Offset: 0x06C (R/W)  Indirect Read Transfer Number Bytes Register */
    __IOM uint32_t IWTCR;         /* Offset: 0x070 (R/W)  Indirect Write Transfer Control Register */
    __IOM uint32_t IWTWR;         /* Offset: 0x074 (R/W)  Indirect Write Transfer Watermark Register */
    __IOM uint32_t IWTSAR;        /* Offset: 0x078 (R/W)  Indirect Write Transfer Start Address Register */
    __IOM uint32_t IWTNBR;        /* Offset: 0x07C (R/W)  Indirect Write Transfer Number Bytes Register */
    __IOM uint32_t ITARR;         /* Offset: 0x080 (R/W)  Indirect Trigger Address Range Register */
    uint32_t RESERVED2[2];
    __IOM uint32_t FCCMR;         /* Offset: 0x08C (R/W)  Flash Command Control Memory Register (Using STIG) */
    __IOM uint32_t FCCR;          /* Offset: 0x090 (R/W)  Flash Command Control Register (Using STIG) */
    __IOM uint32_t FCAR;          /* Offset: 0x094 (R/W)  Flash Command Address Register */
    uint32_t RESERVED3[2];
    __IOM uint32_t FCRDRL;        /* Offset: 0x0A0 (R/W)  Flash Command Read Data Register (Lower) */
    __IOM uint32_t FCRDRU;        /* Offset: 0x0A4 (R/W)  Flash Command Read Data Register (Upper) */
    __IOM uint32_t FCWDRL;        /* Offset: 0x0A8 (R/W)  Flash Command Write Data Register (Lower) */
    __IOM uint32_t FCWDRU;        /* Offset: 0x0AC (R/W)  Flash Command Write Data Register (Upper) */
    __IOM uint32_t PFSR;          /* Offset: 0x0B0 (R/W)  Polling Flash Status Register */
    uint32_t RESERVED4[18];
    __IOM uint32_t MIDR;          /* Offset: 0x0FC (R/W)  Module ID Register */
} cd_qspi_regs_t;

static inline void cd_qspi_enable_dtr_protocol(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_DTR_EN;
}

static inline void cd_qspi_disable_dtr_protocol(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_DTR_EN;
}

static inline void cd_qspi_enable_ahb_decoder(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_AHBDC_EN;
}

static inline void cd_qspi_disable_ahb_decoder(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_AHBDC_EN;
}

static inline void cd_qspi_enter_xip_mode(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_XIP_EN;
}

static inline void cd_qspi_exit_xip_mode(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_XIP_EN;
}

static inline void cd_qspi_enable_remapping(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_AHBARM_EN;
}

static inline void cd_qspi_disable_remapping(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_AHBARM_EN;
}

static inline void cd_qspi_enable_dma_peripheral(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_DMA_EN;
}

static inline void cd_qspi_disable_dma_peripheral(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_DMA_EN;
}

static inline void cd_qspi_enable_write_protect_pin(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_WPP_EN;
}

static inline void cd_qspi_disable_write_protect_pin(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_WPP_EN;
}

static inline void cd_qspi_enable_legacy_mode(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_LEGACY_EN;
}

static inline void cd_qspi_disable_legacy_mode(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_LEGACY_EN;
}

static inline void cd_qspi_enable_direct_access_mode(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_DAC_EN;
}

static inline void cd_qspi_set_cpol0(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_CPOL_EN;
}

static inline void cd_qspi_set_cpol1(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_CPOL_EN;
}

static inline void cd_qspi_set_cpha0(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= ~CD_QSPI_CFGR_CPHA_EN;
}

static inline void cd_qspi_set_cpha1(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_CPHA_EN;
}

static inline void cd_qspi_enable(cd_qspi_regs_t *qspi)
{
    qspi->CFGR |= CD_QSPI_CFGR_ENABLE;
}

static inline void cd_qspi_disable(cd_qspi_regs_t *qspi)
{
    qspi->CFGR &= CD_QSPI_CFGR_ENABLE;
}

static inline void cd_qspi_disable_read_mode_bit(cd_qspi_regs_t *qspi)
{
    qspi->DRIR &= ~CD_QSPI_DRIR_MB_Msk;
}

static inline void cd_qspi_disable_ddr(cd_qspi_regs_t *qspi)
{
    qspi->DRIR &= ~CD_QSPI_DRIR_DDR_EN;
}


static inline void cd_qspi_config_write_dummy_cycles(cd_qspi_regs_t *qspi, uint32_t cycles)
{
    qspi->DWIR &= ~CD_QSPI_DWIR_DMYC_Msk;
    qspi->DWIR |= (cycles << CD_QSPI_DWIR_DMYC_Pos);
}

static inline void cd_qspi_config_write_data_type(cd_qspi_regs_t *qspi, uint32_t type)
{
    qspi->DWIR &= ~CD_QSPI_DWIR_DTT_Msk;
    qspi->DWIR |= (type << CD_QSPI_DWIR_DTT_Pos);
}

static inline void cd_qspi_config_write_address_type(cd_qspi_regs_t *qspi, uint32_t type)
{
    qspi->DWIR &= ~CD_QSPI_DWIR_ATT_Msk;
    qspi->DWIR |= (type << CD_QSPI_DWIR_ATT_Pos);
}

static inline void cd_qspi_enable_wel(cd_qspi_regs_t *qspi)
{
    qspi->DWIR |= CD_QSPI_DWIR_WEL_EN;
}

static inline void cd_qspi_disable_wel(cd_qspi_regs_t *qspi)
{
    qspi->DWIR |= CD_QSPI_DWIR_WEL_EN;
}

static inline void cd_qspi_config_write_opcode(cd_qspi_regs_t *qspi, uint32_t opcode)
{
    qspi->DWIR &= ~CD_QSPI_DWIR_WOPC_Msk;
    qspi->DWIR |= (opcode << CD_QSPI_DWIR_WOPC_Pos);
}

static inline void cd_qspi_disable_write_directly(cd_qspi_regs_t *qspi)
{
    cd_qspi_config_write_opcode(qspi, 0x3U);
}

static inline void cd_qspi_enable_write_directly(cd_qspi_regs_t *qspi)
{
    cd_qspi_config_write_opcode(qspi, 0x2U);
}

static inline void cd_qspi_config_csdas_delay(cd_qspi_regs_t *qspi, uint32_t delay_cycles)
{
    qspi->DDR &=  ~CD_QSPI_DDR_CSDADS_Msk;
    qspi->DDR |=  delay_cycles << CD_QSPI_DDR_CSDADS_Pos;
}

static inline void cd_qspi_config_cseot_delay(cd_qspi_regs_t *qspi, uint32_t delay_cycles)
{
    qspi->DDR &=  ~CD_QSPI_DDR_CSEOT_Msk;
    qspi->DDR |=  delay_cycles << CD_QSPI_DDR_CSEOT_Pos;
}

static inline void cd_qspi_config_cssot_delay(cd_qspi_regs_t *qspi, uint32_t delay_cycles)
{
    qspi->DDR &=  ~CD_QSPI_DDR_CSSOT_Msk;
    qspi->DDR |=  delay_cycles << CD_QSPI_DDR_CSSOT_Pos;
}

static inline void cd_qspi_config_ddr_transmit_delay(cd_qspi_regs_t *qspi, uint32_t delay_cycles)
{
    qspi->RDCR &= ~CD_QSPI_RDCR_TXDD_Msk;
    qspi->RDCR |= delay_cycles << CD_QSPI_RDCR_TXDD_Pos;
}

static inline void cd_qspi_set_falling_edge_sample(cd_qspi_regs_t *qspi)
{
    qspi->RDCR &= ~CD_QSPI_RDCR_SES_Msk;
    qspi->RDCR &= ~CD_QSPI_RDCR_SES_FALLING;
}

static inline void cd_qspi_set_rising_edge_sample(cd_qspi_regs_t *qspi)
{
    qspi->RDCR &= ~CD_QSPI_RDCR_SES_Msk;
    qspi->RDCR &= ~CD_QSPI_RDCR_SES_RISING;
}

static inline void cd_qspi_enable_loopback(cd_qspi_regs_t *qspi)
{
    qspi->RDCR |= CD_QSPI_RDCR_LOOPBACK_EN;
}

static inline void cd_qspi_disable_loopback(cd_qspi_regs_t *qspi)
{
    qspi->RDCR &= ~CD_QSPI_RDCR_LOOPBACK_EN;
}

static inline void cd_qspi_config_block_size(cd_qspi_regs_t *qspi, uint32_t idx, uint32_t size)
{
    qspi->DSCFGR &= ~CD_QSPI_DSCR_BS_Msk;
    qspi->DSCFGR |= (size << CD_QSPI_DSCR_BS_Pos);
}

static inline void cd_qspi_config_page_size(cd_qspi_regs_t *qspi, uint32_t idx, uint32_t size)
{
    qspi->DSCFGR &= ~CD_QSPI_DSCR_PS_Msk;
    qspi->DSCFGR |= (size << CD_QSPI_DSCR_PS_Pos);
}

static inline void cd_qspi_config_address_size(cd_qspi_regs_t *qspi, uint32_t idx, uint32_t size)
{
    qspi->DSCFGR &= ~CD_QSPI_DSCR_AS_Msk;
    qspi->DSCFGR |= (size << CD_QSPI_DSCR_AS_Pos);
}


static inline void cd_qspi_config_sram_partition(cd_qspi_regs_t *qspi, uint32_t location)
{
    qspi->SRAMPCFGR = location;
}

static inline void cd_qspi_config_indac_trig_addr(cd_qspi_regs_t *qspi, uint32_t addr)
{
    qspi->IAATR = addr;
}

static inline void cd_qspi_config_dma_burst_num(cd_qspi_regs_t *qspi, uint32_t num)
{
    qspi->DPCFGR &= ~CD_QSPI_DPCFGR_BSTNUM_Msk;
    qspi->DPCFGR |= (num << CD_QSPI_DPCFGR_BSTNUM_Pos);
}

static inline void cd_qspi_config_dma_signal_num(cd_qspi_regs_t *qspi, uint32_t num)
{
    qspi->DPCFGR &= ~CD_QSPI_DPCFGR_SIGNUM_Msk;
    qspi->DPCFGR |= (num << CD_QSPI_DPCFGR_SIGNUM_Pos);
}

static inline void cd_qspi_config_dac_remap_addr(cd_qspi_regs_t *qspi, uint32_t addr)
{
    qspi->RMAR = addr;
}

static inline void cd_qspi_config_mode_byte(cd_qspi_regs_t *qspi, uint32_t byte)
{
    qspi->MBCFGR = byte;
}

static inline uint32_t cd_qspi_get_rx_sram_level(cd_qspi_regs_t *qspi)
{
    return qspi->SRAMFLR & CD_QSPI_SRAMFLR_RXPAT_Msk;
}

static inline uint32_t cd_qspi_get_tx_sram_level(cd_qspi_regs_t *qspi)
{
    return qspi->SRAMFLR & CD_QSPI_SRAMFLR_TXPAT_Msk;
}

static inline void cd_qspi_config_tx_fifo_threshold(cd_qspi_regs_t *qspi, uint32_t threshold)
{
    qspi->TXTHR = threshold;
}

static inline void cd_qspi_config_rx_fifo_threshold(cd_qspi_regs_t *qspi, uint32_t threshold)
{
    qspi->RXTHR = threshold;
}

static inline void cd_qspi_config_polling_repetition_delay(cd_qspi_regs_t *qspi, uint32_t delay)
{
    qspi->WCCTRLR &= ~CD_QSPI_WCCTRLR_PRD_Msk;
    qspi->WCCTRLR |= delay << CD_QSPI_WCCTRLR_PRD_Pos;
}

static inline void cd_qspi_config_polling_count(cd_qspi_regs_t *qspi, uint32_t num)
{
    qspi->WCCTRLR &= ~CD_QSPI_WCCTRLR_PC_Msk;
    qspi->WCCTRLR |= num << CD_QSPI_WCCTRLR_PC_Pos;
}

static inline void cd_qspi_enable_polling_expiration(cd_qspi_regs_t *qspi)
{
    qspi->WCCTRLR |= CD_QSPI_WCCTRLR_PE_EN;
}

static inline void cd_qspi_disable_polling_expiration(cd_qspi_regs_t *qspi)
{
    qspi->WCCTRLR &= ~CD_QSPI_WCCTRLR_PE_EN;
}

static inline void cd_qspi_enable_polling(cd_qspi_regs_t *qspi)
{
    qspi->WCCTRLR &= ~CD_QSPI_WCCTRLR_AP_EN;
}

static inline void cd_qspi_disable_polling(cd_qspi_regs_t *qspi)
{
    qspi->WCCTRLR |= CD_QSPI_WCCTRLR_AP_EN;
}

static inline void cd_qspi_set_polling_polarity(cd_qspi_regs_t *qspi, uint32_t polarity)
{
    qspi->WCCTRLR &= ~CD_QSPI_WCCTRLR_PP_Msk;
    qspi->WCCTRLR |= CD_QSPI_WCCTRLR_PP0;
}

static inline void cd_qspi_config_polling_bit_index(cd_qspi_regs_t *qspi, uint32_t idx)
{
    qspi->WCCTRLR &= ~CD_QSPI_WCCTRLR_PB_Msk;
    qspi->WCCTRLR |= (idx << CD_QSPI_WCCTRLR_PB_Pos);
}

static inline void cd_qspi_config_polling_opcode(cd_qspi_regs_t *qspi, uint32_t opcode)
{
    qspi->WCCTRLR &= ~CD_QSPI_WCCTRLR_POPC_Msk;
    qspi->WCCTRLR |= (opcode << CD_QSPI_WCCTRLR_POPC_Pos);
}

static inline void cd_qspi_config_polling_expiration(cd_qspi_regs_t *qspi, uint32_t cycles)
{
    qspi->PER = cycles;
}

static inline uint32_t cd_qspi_get_interrupt_status(cd_qspi_regs_t *qspi)
{
    return qspi->ISR;
}

static inline void cd_qspi_enable_mode_fail_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_MDF_EN;
}

static inline void cd_qspi_enable_underflow_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_UNDERFLOW_EN;
}

static inline void cd_qspi_enable_complete_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_COMPLETE_EN;
}

static inline void cd_qspi_enable_indac_reject_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_INDREJ_EN;
}

static inline void cd_qspi_enable_write_reject_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_WTREJ_EN;
}

static inline void cd_qspi_enable_illegal_access_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_ILLEGAL_EN;
}

static inline void cd_qspi_enable_watermark_level_breached_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_WTLEVBREC_EN;
}

static inline void cd_qspi_enable_receive_overflow_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_OVERFLOW_EN;
}

static inline void cd_qspi_enable_tx_fifo_not_full_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_TFNF_EN;
}

static inline void cd_qspi_enable_tx_fifo_full_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_TFF_EN;
}

static inline void cd_qspi_enable_rx_fifo_not_empty_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_RFNE_EN;
}

static inline void cd_qspi_enable_rx_fifo_full_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_RFF_EN;
}

static inline void cd_qspi_enable_sram_full_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_SF_EN;
}

static inline void cd_qspi_enable_polling_expired_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_PE_EN;
}

static inline void cd_qspi_enable_stig_completion_irq(cd_qspi_regs_t *qspi)
{
    qspi->IMR |= CD_QSPI_IMR_STIGCPLT_EN;
}

static inline void cd_qspi_config_write_protection_area(cd_qspi_regs_t *qspi, uint32_t start_addr, uint32_t end_addr)
{
    qspi->LWPR = start_addr;
    qspi->LWPR = end_addr;
}

static inline void cd_qspi_enable_write_protect(cd_qspi_regs_t *qspi)
{
    qspi->WPR |= CD_QSPI_WPR_WPEB_EN;
}

static inline void cd_qspi_disable_write_protect(cd_qspi_regs_t *qspi)
{
    qspi->WPR &= ~CD_QSPI_WPR_WPEB_EN;
}

static inline void cd_qspi_enable_write_protect_invers(cd_qspi_regs_t *qspi)
{
    qspi->WPR |= CD_QSPI_WPR_WPEBINVER_EN;
}

static inline void cd_qspi_disable_write_protect_invers(cd_qspi_regs_t *qspi)
{
    qspi->WPR &= ~CD_QSPI_WPR_WPEBINVER_EN;
}

static inline uint32_t cd_qspi_get_indirect_cmplt_read_num(cd_qspi_regs_t *qspi)
{
    return qspi->IRTCR & CD_QSPI_IRTCR_IOCN_Msk;
}

static inline uint32_t cd_qspi_get_indirect_read_status(cd_qspi_regs_t *qspi)
{
    return qspi->IRTCR & CD_QSPI_IRTCR_IS_Msk;
}

static inline void cd_qspi_start_indirect_read(cd_qspi_regs_t *qspi)
{
    qspi->IRTCR |= CD_QSPI_IRTCR_CTRL_START;
}

static inline void cd_qspi_cancle_indirect_read(cd_qspi_regs_t *qspi)
{
    qspi->IRTCR |= CD_QSPI_IRTCR_CTRL_STOP;
}

static inline void cd_qspi_config_indirect_read_watermark(cd_qspi_regs_t *qspi, uint32_t watermark)
{
    qspi->IRTWR = watermark;
}

static inline void cd_qspi_config_indirect_read_start_addr(cd_qspi_regs_t *qspi, uint32_t addr)
{
    qspi->IRTSAR = addr;
}

static inline void cd_qspi_config_indirect_read_num(cd_qspi_regs_t *qspi, uint32_t num)
{
    qspi->IRTNBR = num;
}

static inline uint32_t cd_qspi_get_indirect_cmpt_write_num(cd_qspi_regs_t *qspi)
{
    return qspi->IWTCR & CD_QSPI_IWTCR_IOCN_Msk;
}

static inline uint32_t cd_qspi_get_indirect_write_status(cd_qspi_regs_t *qspi)
{
    return qspi->IWTCR & CD_QSPI_IWTCR_IS_Msk;
}

static inline void cd_qspi_start_indirect_write(cd_qspi_regs_t *qspi)
{
    qspi->IWTCR |= CD_QSPI_IWTCR_CTRL_START;
}

static inline void cd_qspi_cancle_indirect_write(cd_qspi_regs_t *qspi)
{
    qspi->IWTCR |= CD_QSPI_IWTCR_CTRL_STOP;
}

static inline void cd_qspi_config_write_watermark(cd_qspi_regs_t *qspi, uint32_t watermark)
{
    qspi->IWTWR = watermark;
}

static inline void cd_qspi_config_write_start_addr(cd_qspi_regs_t *qspi, uint32_t addr)
{
    qspi->IWTSAR = addr;
}

static inline void cd_qspi_config_write_num(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    qspi->IWTNBR = bytes;
}

static inline void cd_qspi_config_stig_address(cd_qspi_regs_t *qspi, uint32_t address)
{
    qspi->FCCMR &= ~CD_QSPI_FCCMR_MBA_Msk;
    qspi->FCCMR |= (address << CD_QSPI_FCCMR_MBA_Pos);
}


static inline uint32_t cd_qspi_get_stig_data(cd_qspi_regs_t *qspi)
{
    return qspi->FCCMR & CD_QSPI_FCCMR_MBRD_Msk;
}

static inline uint32_t cd_qspi_get_stig_status(cd_qspi_regs_t *qspi)
{
    return qspi->FCCMR & CD_QSPI_FCCMR_MBDRIP_Msk;
}

static inline void cd_qspi_trigger_stig(cd_qspi_regs_t *qspi)
{
    qspi->FCCMR |= CD_QSPI_FCCMR_TIMBDR_EN;
}

static inline void cd_qspi_set_cmd_opcode(cd_qspi_regs_t *qspi, uint32_t val)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_OPC_Msk;
    qspi->FCCR |= (val << CD_QSPI_FCCR_OPC_Pos)&CD_QSPI_FCCR_OPC_Msk;
}

static inline uint32_t cd_qspi_get_cmd_opcode(cd_qspi_regs_t *qspi)
{
    return (qspi->FCCR & CD_QSPI_FCCR_OPC_Msk);
}

static inline void cd_qspi_disable_stig_read_data(cd_qspi_regs_t *qspi)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_RD_EN;
}

static inline uint32_t cd_qspi_get_read_data_en(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    return (qspi->FCCR & CD_QSPI_FCCR_RD_Msk);
}

static inline void cd_qspi_set_read_data_num(cd_qspi_regs_t *qspi, uint32_t val)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_NORDB_Msk;
    qspi->FCCR |= (CD_QSPI_FCCR_NORDB_Msk & (val << CD_QSPI_FCCR_NORDB_Pos));
}

static inline uint32_t cd_qspi_get_cmd_addr_en(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    return (qspi->FCCR & CD_QSPI_FCCR_CA_Msk);
}

static inline void cd_qspi_enable_cmd_addr(cd_qspi_regs_t *qspi)
{
    qspi->FCCR |= CD_QSPI_FCCR_CA_EN;
}

static inline void cd_qspi_disable_cmd_addr(cd_qspi_regs_t *qspi)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_CA_EN;
}

static inline void cd_qspi_disable_stig_mode_bit(cd_qspi_regs_t *qspi)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_MDB_EN;
}

static inline uint32_t cd_qspi_get_mode(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    return (qspi->FCCR & CD_QSPI_FCCR_MDB_Msk);
}

static inline void cd_qspi_set_addr_num(cd_qspi_regs_t *qspi, uint32_t val)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_NOAB_Msk;
    qspi->FCCR |= (val << CD_QSPI_FCCR_NOAB_Pos)&CD_QSPI_FCCR_NOAB_Msk;
}

static inline void cd_qspi_enable_stig_write_data(cd_qspi_regs_t *qspi)
{
    qspi->FCCR |= CD_QSPI_FCCR_WD_EN;
}

static inline void cd_qspi_disable_stig_write_data(cd_qspi_regs_t *qspi)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_WD_EN;
}

static inline uint32_t cd_qspi_get_write_data_en(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    return (qspi->FCCR & CD_QSPI_FCCR_WD_Msk);
}

static inline void cd_qspi_config_stig_write_data_bytes(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_NOWDB_Msk;
    qspi->FCCR |= ((bytes - 1U) << CD_QSPI_FCCR_NOWDB_Pos);
}

static inline uint32_t cd_qspi_get_write_data_bytes(cd_qspi_regs_t *qspi, uint32_t bytes)
{
    return (qspi->FCCR & CD_QSPI_FCCR_NOWDB_Msk);
}

static inline void cd_qspi_set_dummy_num(cd_qspi_regs_t *qspi, uint32_t val)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_NODMY_Msk;
    qspi->FCCR |= (val << CD_QSPI_FCCR_NODMY_Pos)&CD_QSPI_FCCR_NODMY_Msk;
}

static inline uint32_t cd_qspi_get_dummy_num(cd_qspi_regs_t *qspi)
{
    return (qspi->FCCR & CD_QSPI_FCCR_NODMY_Msk);
}

static inline void cd_qspi_disable_stig_memmory_bank(cd_qspi_regs_t *qspi)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_MB_Msk;
}

static inline void cd_qspi_enable_stig_memmory_bank(cd_qspi_regs_t *qspi)
{
    qspi->FCCR |= CD_QSPI_FCCR_MB_EN;
}

static inline uint32_t cd_qspi_get_stig_memmory_bank(cd_qspi_regs_t *qspi)
{
    return (qspi->FCCR & CD_QSPI_FCCR_MB_Msk);
}

static inline void cd_qspi_enable_exe_cmd(cd_qspi_regs_t *qspi)
{
    qspi->FCCR |= CD_QSPI_FCCR_EXEC_EN;
}

static inline void cd_qspi_disable_exe_cmd(cd_qspi_regs_t *qspi)
{
    qspi->FCCR &= ~CD_QSPI_FCCR_EXEC_Msk;
}

static inline void cd_qspi_config_polling_dummy_cycle(cd_qspi_regs_t *qspi, uint32_t cycles)
{
    qspi->PFSR = CD_QSPI_PFSR_NODCFAP_Msk;
    qspi->PFSR = (cycles << CD_QSPI_PFSR_NODCFAP_Pos);
}

static inline uint32_t cd_qspi_get_polling_status(cd_qspi_regs_t *qspi)
{
    return qspi->PFSR & CD_QSPI_PFSR_PSV_Msk;
}

static inline uint32_t cd_qspi_get_flash_status(cd_qspi_regs_t *qspi)
{
    return qspi->PFSR & CD_QSPI_PFSR_FS_Msk;
}

static inline uint32_t cd_qspi_get_read_datal(cd_qspi_regs_t *qspi)
{
    return qspi->FCRDRL;
}

static inline uint32_t cd_qspi_get_read_datau(cd_qspi_regs_t *qspi)
{
    return qspi->FCRDRU;
}

extern int cd_qspi_config_baudrate(cd_qspi_regs_t *qspi, uint32_t clock_in, uint32_t clock_out);
extern void cd_qspi_enable_slave(cd_qspi_regs_t *qspi, uint32_t idx);
extern void cd_qspi_disable_slave(cd_qspi_regs_t *qspi, uint32_t idx);
extern int cd_qspi_config_csda_delay(cd_qspi_regs_t *qspi, uint32_t delay_cycles);
extern int cd_qspi_config_read_capture_delay(cd_qspi_regs_t *qspi, uint32_t delay_cycles);
extern int cd_qspi_config_flash_size(cd_qspi_regs_t *qspi, uint32_t idx, uint32_t mb);
extern void cd_qspi_read_stig_data(cd_qspi_regs_t *qspi, uint8_t *data, uint32_t num);
extern void cd_qspi_write_stig_data(cd_qspi_regs_t *qspi, uint8_t *data, uint32_t num);
extern void cd_qspi_config_stig_memory_read_bytes(cd_qspi_regs_t *qspi, uint32_t bytes);
extern void cd_qspi_reset_regs(cd_qspi_regs_t *qspi);
extern void cd_qspi_reset_stig(cd_qspi_regs_t *qspi);
extern int cd_qspi_get_freq_div(cd_qspi_regs_t *qspi);
extern uint32_t cd_qspi_get_idle_state(cd_qspi_regs_t *qspi);
extern uint32_t cd_qspi_get_stig_process_state(cd_qspi_regs_t *qspi);
extern void cd_qspi_execute_stig_command(cd_qspi_regs_t *qspi);
extern void cd_qspi_config_stig_cmd_opcode(cd_qspi_regs_t *qspi, uint8_t opcode);
extern void cd_qspi_enable_stig_command_address(cd_qspi_regs_t *qspi);
extern void cd_qspi_config_stig_address_bytes(cd_qspi_regs_t *qspi, uint32_t bytes);
extern void cd_qspi_disable_stig_command_address(cd_qspi_regs_t *qspi);
extern void cd_qspi_config_stig_dummy_cycles(cd_qspi_regs_t *qspi, uint8_t cycles);
extern void cd_qspi_enable_stig_mode_bit(cd_qspi_regs_t *qspi);
extern void cd_qspi_enable_ddr(cd_qspi_regs_t *qspi);
extern uint32_t cd_qspi_get_stig_command_address(cd_qspi_regs_t *qspi);
extern void cd_qspi_config_stig_read_bytes(cd_qspi_regs_t *qspi, uint32_t num);
extern void cd_qspi_enable_stig_read_data(cd_qspi_regs_t *qspi);
extern void cd_qspi_config_stig_command_address(cd_qspi_regs_t *qspi, uint32_t address);
extern void cd_qspi_reset_stig(cd_qspi_regs_t *qspi);
extern void cd_qspi_config_read_opcode(cd_qspi_regs_t *qspi, uint8_t opcode);
extern void cd_qspi_config_read_instruction_type(cd_qspi_regs_t *qspi, uint32_t type);
extern void cd_qspi_config_read_address_type(cd_qspi_regs_t *qspi, uint32_t type);
extern void cd_qspi_config_read_data_type(cd_qspi_regs_t *qspi, uint32_t type);
extern void cd_qspi_config_read_dummy_cycles(cd_qspi_regs_t *qspi, uint8_t cycles);
extern void cd_qspi_enable_read_mode_bit(cd_qspi_regs_t *qspi);
#ifdef __cplusplus
}
#endif

#endif  /* _CS_QSPI_LL_H_*/
