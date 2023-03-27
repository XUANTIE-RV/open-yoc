/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_qspi_ll.h
 * @brief
 * @version
 * @date     2020-07-29
 ******************************************************************************/

#ifndef _DW_QSPI_LL_H_
#define _DW_QSPI_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPI register bit definitions
 */

/* CTRLR0, offset: 0x00 */
#define DW_QSPI_CTRLR0_SSTE_Pos                    (24U)
#define DW_QSPI_CTRLR0_SSTE_Msk                    (0x1U << DW_QSPI_CTRLR0_SSTE_Pos) 
#define DW_QSPI_CTRLR0_SSTE_EN                     DW_QSPI_CTRLR0_SSTE_Msk

#define DW_QSPI_CTRLR0_SPI_FRF_Pos                 (21U)
#define DW_QSPI_CTRLR0_SPI_FRF_Msk                 (0x3U << DW_QSPI_CTRLR0_SPI_FRF_Pos) 
#define DW_QSPI_CTRLR0_SPI_FRF_STD                 (0x0U) 
#define DW_QSPI_CTRLR0_SPI_FRF_DUAL                (0x1U) 
#define DW_QSPI_CTRLR0_SPI_FRF_QUAD                (0x2U) 
#define DW_QSPI_CTRLR0_SPI_FRF_OCTAL               (0x3U) 

#define DW_QSPI_CTRLR0_DFS32_Pos                   (16U)
#define DW_QSPI_CTRLR0_DFS32_Msk                   (0x1F << DW_QSPI_CTRLR0_DFS32_Pos)



#define DW_QSPI_CTRLR0_CFS_Pos                     (12U)
#define DW_QSPI_CTRLR0_CFS_Msk                     (0xFU << DW_QSPI_CTRLR0_CFS_Pos)

#define DW_QSPI_CTRLR0_SRL_Pos                     (11U)
#define DW_QSPI_CTRLR0_SRL_EN                      DW_QSPI_CTRLR0_SRL_Msk
#define DW_QSPI_CTRLR0_SRL_Msk                     (0x1U << DW_QSPI_CTRLR0_SRL_Pos)

#define DW_QSPI_CTRLR0_TMOD_Pos                    (8U)
#define DW_QSPI_CTRLR0_TMOD_Msk                    (0x3U << DW_QSPI_CTRLR0_TMOD_Pos)
#define DW_QSPI_CTRLR0_TMOD_TX_RX                  (0x0U << DW_QSPI_CTRLR0_TMOD_Pos)
#define DW_QSPI_CTRLR0_TMOD_TX                     (0x1U << DW_QSPI_CTRLR0_TMOD_Pos)
#define DW_QSPI_CTRLR0_TMOD_RX                     (0x2U << DW_QSPI_CTRLR0_TMOD_Pos)
#define DW_QSPI_CTRLR0_TMOD_EEPROM                 (0x3U << DW_QSPI_CTRLR0_TMOD_Pos)

#define DW_QSPI_CTRLR0_SCPOL_Pos                   (7U)
#define DW_QSPI_CTRLR0_SCPOL_Msk                   (0x1U << DW_QSPI_CTRLR0_SCPOL_Pos)
#define DW_QSPI_CTRLR0_SCPOL_EN                    DW_QSPI_CTRLR0_SCPOL_Msk

#define DW_QSPI_CTRLR0_SCPH_Pos                    (6U)
#define DW_QSPI_CTRLR0_SCPH_Msk                    (0x1U << DW_QSPI_CTRLR0_SCPH_Pos)
#define DW_QSPI_CTRLR0_SCPH_EN                     DW_QSPI_CTRLR0_SCPH_Msk

#define DW_QSPI_CTRLR0_FRF_Pos                     (4U)
#define DW_QSPI_CTRLR0_FRF_Msk                     (0x3U << DW_QSPI_CTRLR0_FRF_Pos)
#define DW_QSPI_CTRLR0_FRF_MOTOROLA_SPI            (0x0U << DW_QSPI_CTRLR0_FRF_Pos)
#define DW_QSPI_CTRLR0_FRF_TI_SSP                  (0x1U << DW_QSPI_CTRLR0_FRF_Pos)
#define DW_QSPI_CTRLR0_FRF_MW_SPI                  (0x2U << DW_QSPI_CTRLR0_FRF_Pos)


/* CTRLR1, offset: 0x04 */
#define DW_QSPI_CTRLR1_NDF_Pos                     (0U)
#define DW_QSPI_CTRLR1_NDF_Msk                     (0xFFFF << DW_QSPI_CTRLR1_NDF_Pos)

/* SSIENR, offset: 0x08 */

#define DW_QSPI_SSIENR_SSI_EN_Pos                  (0U)
#define DW_QSPI_SSIENR_SSI_EN_Msk                  (0x1U << DW_QSPI_SSIENR_SSI_EN_Pos)
#define DW_QSPI_SSIENR_SSI_EN                      DW_QSPI_SSIENR_SSI_EN_Msk

/* MWCR, offset: 0x0C */
#define DW_QSPI_MWCR_MHS_Pos                       (2U)
#define DW_QSPI_MWCR_MHS_Msk                       (0x1U << DW_QSPI_MWCR_MHS_Pos)
#define DW_QSPI_MWCR_MHS_EN                        DW_QSPI_MWCR_MHS_Msk

#define DW_QSPI_MWCR_MDD_Pos                       (1U)
#define DW_QSPI_MWCR_MDD_Msk                       (0x1U << DW_QSPI_MWCR_MHS_Pos)
#define DW_QSPI_MWCR_MDD_INPUT                     (0x0U << DW_QSPI_MWCR_MHS_Pos)
#define DW_QSPI_MWCR_MDD_OUTPUT                    (0x1U << DW_QSPI_MWCR_MHS_Pos)

#define DW_QSPI_MWCR_MWMOD_Pos                     (0U)
#define DW_QSPI_MWCR_MWMOD_Msk                     (0x1U << DW_QSPI_MWCR_MWMOD_Pos)
#define DW_QSPI_MWCR_MWMOD_SQUENTIAL               (0x1U << DW_QSPI_MWCR_MWMOD_Pos)
#define DW_QSPI_MWCR_MWMOD_NOSQUENTIAL             (0x0U << DW_QSPI_MWCR_MWMOD_Pos)

/* SER, offset: 0x10 */
#define DW_QSPI_SER_SEL_Pos                        (0U)
#define DW_QSPI_SER_SEL_Msk                        (0x0F << DW_QSPI_SER_SEL_Pos)

/* BAUDR, offset: 0x14 */
#define DW_QSPI_BAUDR_SCKDV_Pos                    (0U)
#define DW_QSPI_BAUDR_SCKDV_Msk                    (0xFFFFU << DW_QSPI_BAUDR_SCKDV_Pos)

/* TXFTLR, offset: 0x18 */
#define DW_QSPI_TXFTLR_TFT_Pos                     (0U)
#define DW_QSPI_TXFTLR_TFT_Msk                     (0xFFU << DW_QSPI_TXFTLR_TFT_Pos)

/* RXFTLR, offset: 0x1C */
#define DW_QSPI_RXFTLR_RFT_Pos                     (0U)
#define DW_QSPI_RXFTLR_RFT_Msk                     (0xFFU << DW_QSPI_RXFTLR_RFT_Pos)

/* SR, offset: 0x28 */
#define DW_QSPI_SR_Pos                             (0U)
#define DW_QSPI_SR_Msk                             (0x7FU << DW_QSPI_SR_Pos)
#define DW_QSPI_SR_DCOL                            (0x40U << DW_QSPI_SR_Pos)
#define DW_QSPI_SR_TXE                             (0x20U << DW_QSPI_SR_Pos)
#define DW_QSPI_SR_RFF                             (0x10U << DW_QSPI_SR_Pos)
#define DW_QSPI_SR_RFNE                            (0x08U << DW_QSPI_SR_Pos)
#define DW_QSPI_SR_TFE                             (0x04U << DW_QSPI_SR_Pos)
#define DW_QSPI_SR_TFNF                            (0x02U << DW_QSPI_SR_Pos)
#define DW_QSPI_SR_BUSY                            (0x01U << DW_QSPI_SR_Pos)

/* IMR, offset: 0x2C */
#define DW_QSPI_IMR_Pos                            (0U)
#define DW_QSPI_IMR_Msk                            (0x3FU << DW_QSPI_IMR_Pos)

#define DW_QSPI_IMR_MSTIM_Pos                      (5U)
#define DW_QSPI_IMR_MSTIM_Msk                      (0x1U << DW_QSPI_IMR_MSTIM_Pos)
#define DW_QSPI_IMR_MSTIM_EN                       DW_QSPI_IMR_MSTIM_Msk

#define DW_QSPI_IMR_RXFIM_Pos                      (4U)
#define DW_QSPI_IMR_RXFIM_Msk                      (0x1U << DW_QSPI_IMR_RXFIM_Pos)
#define DW_QSPI_IMR_RXFIM_EN                       DW_QSPI_IMR_RXFIM_Msk

#define DW_QSPI_IMR_RXOIM_Pos                      (3U)
#define DW_QSPI_IMR_RXOIM_Msk                      (0x1U << DW_QSPI_IMR_RXOIM_Pos)
#define DW_QSPI_IMR_RXOIM_EN                       DW_QSPI_IMR_RXOIM_Msk

#define DW_QSPI_IMR_RXUIM_Pos                      (2U)
#define DW_QSPI_IMR_RXUIM_Msk                      (0x1U << DW_QSPI_IMR_RXUIM_Pos)
#define DW_QSPI_IMR_RXUIM_EN                       DW_QSPI_IMR_RXUIM_Msk

#define DW_QSPI_IMR_TXOIM_Pos                      (1U)
#define DW_QSPI_IMR_TXOIM_Msk                      (0x1U << DW_QSPI_IMR_TXOIM_Pos)
#define DW_QSPI_IMR_TXOIM_EN                       DW_QSPI_IMR_TXOIM_Msk

#define DW_QSPI_IMR_TXEIM_Pos                      (0U)
#define DW_QSPI_IMR_TXEIM_Msk                      (0x1U << DW_QSPI_IMR_TXEIM_Pos)
#define DW_QSPI_IMR_TXEIM_EN                       DW_QSPI_IMR_TXEIM_Msk

/* ISR, offset: 0x30 */
#define DW_QSPI_ISR_Pos                            (0U)
#define DW_QSPI_ISR_Msk                            (0x3FU << DW_QSPI_ISR_Pos)
#define DW_QSPI_ISR_MSTIS                          (0x20U << DW_QSPI_ISR_Pos)
#define DW_QSPI_ISR_RXFIS                          (0x10U << DW_QSPI_ISR_Pos)
#define DW_QSPI_ISR_RXOIS                          (0x08U << DW_QSPI_ISR_Pos)
#define DW_QSPI_ISR_RXUIS                          (0x04U << DW_QSPI_ISR_Pos)
#define DW_QSPI_ISR_TXOIS                          (0x02U << DW_QSPI_ISR_Pos)
#define DW_QSPI_ISR_TXEIS                          (0x01U << DW_QSPI_ISR_Pos)

/* RISR, offset: 0x34 */
#define DW_QSPI_RISR_Pos                            (0U)
#define DW_QSPI_RISR_Msk                            (0x3FU << DW_QSPI_RISR_Pos)
#define DW_QSPI_RISR_MSTIR                          (0x20U << DW_QSPI_RISR_Pos)
#define DW_QSPI_RISR_RXFIR                          (0x10U << DW_QSPI_RISR_Pos)
#define DW_QSPI_RISR_RXOIR                          (0x08U << DW_QSPI_RISR_Pos)
#define DW_QSPI_RISR_RXUIR                          (0x04U << DW_QSPI_RISR_Pos)
#define DW_QSPI_RISR_TXOIR                          (0x02U << DW_QSPI_RISR_Pos)
#define DW_QSPI_RISR_TXEIR                          (0x01U << DW_QSPI_RISR_Pos)


/* DMACR, offset: 0x4C */
#define DW_QSPI_DMACR_TDMAE_Pos                     (1U)
#define DW_QSPI_DMACR_TDMAE_Msk                     (0x1U << DW_QSPI_DMACR_TDMAE_Pos)
#define DW_QSPI_DMACR_TDMAE_EN                      DW_QSPI_DMACR_TDMAE_Msk

#define DW_QSPI_DMACR_RDMAE_Pos                     (0U)
#define DW_QSPI_DMACR_RDMAE_Msk                     (0x1U << DW_QSPI_DMACR_RDMAE_Pos)
#define DW_QSPI_DMACR_RDMAE_EN                      DW_QSPI_DMACR_RDMAE_Msk


/* DMATDLR, offset: 0x50 */
#define DW_QSPI_DMATDLR_DMATDL_Pos                  (0U)
#define DW_QSPI_DMATDLR_DMATDL_Msk                  (0xFFU << DW_QSPI_DMATDLR_DMATDL_Pos)

/* DMARDLR, offset: 0x54 */
#define DW_QSPI_DMARDLR_DMARDL_Pos                  (0U)
#define DW_QSPI_DMARDLR_DMARDL_Msk                  (0xFFU << DW_QSPI_DMARDLR_DMARDL_Pos)

/*DW_QSPI_RX_SAMPLE_DLY offset: 0xF0 */
#define DW_QSPI_RX_SAMPLE_DLY_Pos                   (0U)
#define DW_QSPI_RX_SAMPLE_DLY_Msk                   (0xF << DW_QSPI_RX_SAMPLE_DLY_Pos)

/*DW_QSPI_CTRLR0 offset: 0xF4 */
#define DW_QSPI_SPI_CTRL0_WAIT_CYCLES_Pos           (11U)
#define DW_QSPI_SPI_CTRL0_WAIT_CYCLES_Msk           (0x1F << DW_QSPI_SPI_CTRL0_WAIT_CYCLES_Pos)

#define DW_QSPI_SPI_CTRL0_INST_L_Pos                (8U)
#define DW_QSPI_SPI_CTRL0_INST_L_Msk                (0x3U << DW_QSPI_SPI_CTRL0_INST_L_Pos)
#define DW_QSPI_SPI_CTRL0_INST_LEN_0                (0x0U)
#define DW_QSPI_SPI_CTRL0_INST_LEN_4                (0x1U)
#define DW_QSPI_SPI_CTRL0_INST_LEN_8                (0x2U)
#define DW_QSPI_SPI_CTRL0_INST_LEN_16               (0x3U)

#define DW_QSPI_SPI_CTRL0_ADDR_L_Pos                (2U)
#define DW_QSPI_SPI_CTRL0_ADDR_L_Msk                (0xFU << DW_QSPI_SPI_CTRL0_ADDR_L_Pos)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_0                (0x0U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_4                (0x1U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_8                (0x2U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_12               (0x3U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_16               (0x4U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_20               (0x5U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_24               (0x6U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_28               (0x7U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_32               (0x8U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_36               (0x9U)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_40               (0xAU)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_44               (0xBU)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_48               (0xCU)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_52               (0xDU)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_56               (0xEU)
#define DW_QSPI_SPI_CTRL0_ADDR_LEN_60               (0xFU)

#define DW_QSPI_SPI_CTRL0_TRANS_TYPE_Pos            (0U )
#define DW_QSPI_SPI_CTRL0_TRANS_TYPE_Msk            (0x3U << DW_QSPI_SPI_CTRL0_TRANS_TYPE_Pos)
#define DW_QSPI_SPI_CTRL0_TRANS_TYPE_ISTD_ASTD      (0x0U)
#define DW_QSPI_SPI_CTRL0_TRANS_TYPE_ISTD_APRF      (0x1U)
#define DW_QSPI_SPI_CTRL0_TRANS_TYPE_IPRF_APRF      (0x2U)


typedef struct {
    __IOM uint32_t CTRLR0;      /* Offset: 0x000 (R/W)  Control register 0 */
    __IOM uint32_t CTRLR1;      /* Offset: 0x004 (R/W)  Control register 1 */
    __IOM uint32_t SSIENR;      /* Offset: 0x008 (R/W)  SSI enable regiseter */
    __IOM uint32_t MWCR;        /* offset: 0x00c (R/W)  setting for micro wire */
    __IOM uint32_t SER;         /* Offset: 0x010 (R/W)  Slave enable register */
    __IOM uint32_t BAUDR;       /* Offset: 0x014 (R/W)  Baud rate select */
    __IOM uint32_t TXFTLR;      /* Offset: 0x018 (R/W)  Transmit FIFO Threshold Level */
    __IOM uint32_t RXFTLR;      /* Offset: 0x01c (R/W)  Receive FIFO Threshold Level */
    __IOM uint32_t TXFLR;       /* Offset: 0x020 (R/W)  Transmit FIFO Level register */
    __IOM uint32_t RXFLR;       /* Offset: 0x024 (R/W)  Receive FIFO Level Register */
    __IOM uint32_t SR;          /* Offset: 0x028 (R/W)  status register */
    __IOM uint32_t IMR;         /* Offset: 0x02C (R/W)  Interrupt Mask Register */
    __IM uint32_t ISR;          /* Offset: 0x030 (R/W)  interrupt status register */
    __IM uint32_t RISR;         /* Offset: 0x034 (R/W)  Raw Interrupt Status Register */
    __IM uint32_t TXOICR;       /* Offset: 0x038 (R/W)  Transmit FIFO Overflow Interrupt Clear Register */
    __IM uint32_t RXOICR;       /* Offset: 0x03C (R/W)  Receive FIFO Overflow Interrupt Clear Register*/
    __IM uint32_t RXUICR;       /* Offset: 0x040 (R/W)  Receive FIFO Underflow Interrupt Clear Register */
    __IM uint32_t MSTICR;       /* Offset: 0x044 (R/W)  Multi-Master Interrupt Clear Register */
    __IM uint32_t ICR;          /* Offset: 0x048 (R/W)  Interrupt Clear Register */
    __IOM uint32_t DMACR;       /* Offset: 0x04C (R/W) DMA Control Register */
    __IOM uint32_t DMATDLR;     /* Offset: 0x050 (R/W)  DMA Transmoit Data Level */
    __IOM uint32_t DMARDLR;     /* Offset: 0x054 (R/W)  DMA Receive Data Level */
    __IM uint32_t IDR;          /* Offset: 0x058 (R/W)  identification register */
    __IM uint32_t SSI_COMP_VERSION;
    __IOM uint32_t DR;          /* Offset: 0x060 (R/W)  Data Register */
    uint32_t RESERVED1[35];
    __IOM uint32_t RX_SAMPLE_DLY; /* Offset: 0x0F0 (R/W)  Rx Sample Delay Register */
    __IOM uint32_t SPI_CTRLR0;  /* offset: 0x0F4 (R/W) addition setting for motorola spi */

} dw_qspi_regs_t;

typedef struct 
{
    uint32_t          mode;
    uint32_t          inst_len;
    uint32_t          addr_len;
    uint32_t          dummy_len;
    uint32_t          spi_prf;
}dw_qspi_xfer_enhance_cfg_t;

static inline void dw_qspi_enable_slave_select_toggle(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_SSTE_EN;
}

static inline void dw_qspi_disable_slave_select_toggle(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_SSTE_EN;
}

static inline void dw_qspi_set_qspi_frame_fmt(dw_qspi_regs_t *qspi_base, uint32_t format)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_SPI_FRF_Msk;
    qspi_base->CTRLR0 |= (format << DW_QSPI_CTRLR0_SPI_FRF_Pos);
}

static inline void dw_qspi_set_ssi_fmt(dw_qspi_regs_t *qspi_base, uint32_t format)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_FRF_Msk;
    qspi_base->CTRLR0 |= (format << DW_QSPI_CTRLR0_FRF_Pos);
}

static inline void dw_qspi_config_ctl_frame_len(dw_qspi_regs_t *qspi_base, uint32_t len)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_CFS_Msk;
    qspi_base->CTRLR0 |= (len << DW_QSPI_CTRLR0_CFS_Pos);
}

static inline void dw_qspi_set_qspi_frame_len(dw_qspi_regs_t *qspi_base, uint32_t len)
{
    qspi_base->CTRLR0 = (qspi_base->CTRLR0 & (~DW_QSPI_CTRLR0_DFS32_Msk)) |((len-1) << DW_QSPI_CTRLR0_DFS32_Pos);
}

static inline void dw_qspi_enable_test_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_SRL_EN;
}

static inline void dw_qspi_disable_test_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_SRL_EN;
}

static inline void dw_qspi_set_tx_rx_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_TMOD_Msk;
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_TMOD_TX_RX;
}

static inline void dw_qspi_set_tx_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_TMOD_Msk;
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_TMOD_TX;
}

static inline void dw_qspi_set_rx_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_TMOD_Msk;
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_TMOD_RX;
}

static inline uint32_t dw_qspi_get_transfer_mode(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->CTRLR0 & DW_QSPI_CTRLR0_TMOD_Msk;
}

static inline void dw_qspi_set_eeprom_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_TMOD_Msk;
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_TMOD_EEPROM;
}

static inline void dw_qspi_set_cpol0(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~(DW_QSPI_CTRLR0_SCPOL_EN);
}

static inline void dw_qspi_set_cpol1(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_SCPOL_EN;
}

static inline void dw_qspi_set_cpha0(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~(DW_QSPI_CTRLR0_SCPH_EN);
}

static inline void dw_qspi_set_cpha1(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_SCPH_EN;
}

static inline void dw_qspi_set_motorola_qspi_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_FRF_Msk;
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_FRF_MOTOROLA_SPI;
}

static inline void dw_qspi_set_ti_ssp_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_FRF_Msk;
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_FRF_TI_SSP;
}

static inline void dw_qspi_set_mw_mode(dw_qspi_regs_t *qspi_base)
{
    qspi_base->CTRLR0 &= ~DW_QSPI_CTRLR0_FRF_Msk;
    qspi_base->CTRLR0 |= DW_QSPI_CTRLR0_FRF_MW_SPI;
}

static inline void dw_qspi_config_rx_data_len(dw_qspi_regs_t *qspi_base, uint32_t len)
{
    qspi_base->CTRLR1 = len;
}

static inline void dw_qspi_enable(dw_qspi_regs_t *qspi_base)
{
    qspi_base->SSIENR |= DW_QSPI_SSIENR_SSI_EN;
}

static inline void dw_qspi_disable(dw_qspi_regs_t *qspi_base)
{
    qspi_base->SSIENR &= ~(DW_QSPI_SSIENR_SSI_EN);
}

static inline void dw_qspi_set_mw_sequential_transfer(dw_qspi_regs_t *qspi_base)
{
    qspi_base->MWCR |= DW_QSPI_MWCR_MWMOD_SQUENTIAL;
}

static inline void dw_qspi_set_mw_nonsequential_transfer(dw_qspi_regs_t *qspi_base)
{
    qspi_base->MWCR &= ~DW_QSPI_MWCR_MWMOD_Msk;
    qspi_base->MWCR |= DW_QSPI_MWCR_MWMOD_NOSQUENTIAL;
}

static inline void dw_qspi_set_mw_direction_output(dw_qspi_regs_t *qspi_base)
{
    qspi_base->MWCR &= ~DW_QSPI_MWCR_MDD_Msk;
    qspi_base->MWCR |= DW_QSPI_MWCR_MDD_OUTPUT;
}

static inline void dw_qspi_set_mw_direction_input(dw_qspi_regs_t *qspi_base)
{
    qspi_base->MWCR &= ~DW_QSPI_MWCR_MDD_Msk;
    qspi_base->MWCR |= DW_QSPI_MWCR_MDD_INPUT;
}

static inline void dw_qspi_enable_mw_handshaking(dw_qspi_regs_t *qspi_base)
{
    qspi_base->MWCR |= DW_QSPI_MWCR_MHS_EN;
}

static inline void dw_qspi_disable_mw_handshaking(dw_qspi_regs_t *qspi_base)
{
    qspi_base->MWCR &= DW_QSPI_MWCR_MHS_EN;
}

static inline void dw_qspi_enable_slave(dw_qspi_regs_t *qspi_base, uint32_t idx)
{
    qspi_base->SER |= ((uint32_t)1U << idx);
}

static inline void dw_qspi_disable_slave(dw_qspi_regs_t *qspi_base, uint32_t idx)
{
    qspi_base->SER &= ~((uint32_t) 1U << idx);
}

static inline void dw_qspi_disable_all_slave(dw_qspi_regs_t *qspi_base)
{
    qspi_base->SER = 0U;
}

static inline void dw_qspi_config_tx_fifo_threshold(dw_qspi_regs_t *qspi_base, uint32_t value)
{
    qspi_base->TXFTLR = value & DW_QSPI_TXFTLR_TFT_Msk;
}

static inline void dw_qspi_config_rx_fifo_threshold(dw_qspi_regs_t *qspi_base, uint32_t value)
{
    qspi_base->RXFTLR = value & DW_QSPI_RXFTLR_RFT_Msk;
}

static inline uint32_t dw_qspi_get_tx_fifo_level(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->TXFLR;
}

static inline uint32_t dw_qspi_get_rx_fifo_level(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->RXFLR;
}

static inline uint32_t dw_qspi_get_status(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->SR;
}

static inline void dw_qspi_enable_all_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR |= DW_QSPI_IMR_TXEIM_EN;
    qspi_base->IMR |= DW_QSPI_IMR_TXOIM_EN;
    qspi_base->IMR |= DW_QSPI_IMR_RXUIM_EN;
    qspi_base->IMR |= DW_QSPI_IMR_RXOIM_EN;
    qspi_base->IMR |= DW_QSPI_IMR_RXFIM_EN;
    qspi_base->IMR |= DW_QSPI_IMR_MSTIM_EN;
}

static inline void dw_qspi_disable_all_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR = 0U;
}

static inline void dw_qspi_enable_multi_master_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR |= DW_QSPI_IMR_MSTIM_EN;
}

static inline void dw_qspi_disable_multi_master_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR &= ~DW_QSPI_IMR_MSTIM_EN;
}

static inline void dw_qspi_enable_rx_fifo_full_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR |= DW_QSPI_IMR_RXFIM_EN;
}

static inline void dw_qspi_disable_rx_fifo_full_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR &= ~DW_QSPI_IMR_RXFIM_EN;
}

static inline void dw_qspi_enable_rx_fifo_overflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR |= DW_QSPI_IMR_RXOIM_EN;
}

static inline void dw_qspi_disable_rx_fifo_overflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR &= ~DW_QSPI_IMR_RXOIM_EN;
}

static inline void dw_qspi_enable_rx_fifo_underflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR |= DW_QSPI_IMR_RXUIM_EN;
}

static inline void dw_qspi_disable_rx_fifo_underflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR &= ~DW_QSPI_IMR_RXUIM_EN;
}

static inline void dw_qspi_enable_tx_fifo_overflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR |= DW_QSPI_IMR_TXOIM_EN;
}

static inline void dw_qspi_disable_tx_fifo_overflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR &= ~DW_QSPI_IMR_TXOIM_EN;
}

static inline void dw_qspi_enable_tx_empty_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR |= DW_QSPI_IMR_TXEIM_EN;
}

static inline void dw_qspi_disable_tx_empty_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->IMR &= ~DW_QSPI_IMR_TXEIM_EN;
}

static inline uint32_t dw_qspi_get_interrupt_status(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->ISR;
}

static inline uint32_t dw_qspi_get_raw_interrupt_status(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->RISR;
}

static inline void dw_qspi_clr_tx_fifo_overflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->TXOICR;
}

static inline void dw_qspi_clr_rx_fifo_overflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->RXOICR;
}

static inline void dw_qspi_clr_rx_fifo_underflow_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->RXUICR;
}

static inline void dw_qspi_clr_multi_master_irq(dw_qspi_regs_t *qspi_base)
{
    qspi_base->MSTICR;
}

static inline void dw_qspi_clr_all_irqs(dw_qspi_regs_t *qspi_base)
{
    qspi_base->ICR;
}

static inline void dw_qspi_enable_tx_dma(dw_qspi_regs_t *qspi_base)
{
    qspi_base->DMACR |= DW_QSPI_DMACR_TDMAE_EN;
}

static inline void dw_qspi_disable_tx_dma(dw_qspi_regs_t *qspi_base)
{
    qspi_base->DMACR &= ~DW_QSPI_DMACR_TDMAE_EN;
}

static inline void dw_qspi_enable_rx_dma(dw_qspi_regs_t *qspi_base)
{
    qspi_base->DMACR |= DW_QSPI_DMACR_RDMAE_EN;
}

static inline void dw_qspi_disable_rx_dma(dw_qspi_regs_t *qspi_base)
{
    qspi_base->DMACR &= ~DW_QSPI_DMACR_RDMAE_EN;
}

static inline void dw_qspi_config_dma_tx_data_level(dw_qspi_regs_t *qspi_base, uint32_t value)
{
    qspi_base->DMATDLR = value & DW_QSPI_DMATDLR_DMATDL_Msk;
}

static inline uint32_t dw_qspi_get_dma_tx_data_level(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->DMATDLR;
}

static inline void dw_qspi_config_dma_rx_data_level(dw_qspi_regs_t *qspi_base, uint32_t value)
{
    qspi_base->DMARDLR = value & DW_QSPI_DMARDLR_DMARDL_Msk;
}

static inline uint32_t dw_qspi_get_dma_rx_data_level(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->DMARDLR;
}

static inline uint32_t dw_qspi_get_id(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->IDR;
}

static inline uint32_t dw_qspi_get_version(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->SSI_COMP_VERSION;
}

static inline void dw_qspi_transmit_data(dw_qspi_regs_t *qspi_base, uint32_t data)
{
    qspi_base->DR = data;
}

static inline uint32_t dw_qspi_receive_data(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->DR;
}

static inline uint32_t dw_qspi_get_rx_sample_deley(dw_qspi_regs_t *qspi_base)
{
    return qspi_base->RX_SAMPLE_DLY;
}

static inline void dw_qspi_config_rx_sample_delay(dw_qspi_regs_t *qspi_base, uint8_t delay)
{
    qspi_base->RX_SAMPLE_DLY = delay;
}

static inline void dw_qspi_set_wait_cycles(dw_qspi_regs_t *qspi_base, uint32_t cycles)
{
    qspi_base->SPI_CTRLR0 &= ~DW_QSPI_SPI_CTRL0_WAIT_CYCLES_Msk;
    qspi_base->SPI_CTRLR0 |= cycles<<DW_QSPI_SPI_CTRL0_WAIT_CYCLES_Pos;    
}

static inline void dw_qspi_set_inst_len(dw_qspi_regs_t *qspi_base, uint32_t inst_len)
{
    qspi_base->SPI_CTRLR0 &= ~DW_QSPI_SPI_CTRL0_INST_L_Msk;
    qspi_base->SPI_CTRLR0 |=inst_len << DW_QSPI_SPI_CTRL0_INST_L_Pos;  
}

static inline void dw_qspi_set_addr_len(dw_qspi_regs_t *qspi_base,uint32_t addr_len)
{
    qspi_base->SPI_CTRLR0 &= ~DW_QSPI_SPI_CTRL0_ADDR_L_Msk;
    qspi_base->SPI_CTRLR0 |= addr_len << DW_QSPI_SPI_CTRL0_ADDR_L_Pos;
}

static inline void dw_qspi_set_trans_type(dw_qspi_regs_t *qspi_base, uint32_t trans_type)
{
    qspi_base->SPI_CTRLR0 &= ~DW_QSPI_SPI_CTRL0_TRANS_TYPE_Msk;
    qspi_base->SPI_CTRLR0 |= trans_type << DW_QSPI_SPI_CTRL0_TRANS_TYPE_Pos;
}

extern void     dw_qspi_config_sclk_clock(dw_qspi_regs_t *qspi_base, uint32_t clock_in, uint32_t clock_out);
extern uint32_t dw_qspi_get_sclk_clock_div(dw_qspi_regs_t *qspi_base);
extern uint32_t dw_qspi_get_data_frame_len(dw_qspi_regs_t *qspi_base);
extern void     dw_qspi_config_data_frame_len(dw_qspi_regs_t *qspi_base, uint32_t size);
extern void     dw_qspi_reset_regs(dw_qspi_regs_t *qspi_base);
extern void dw_qspi_config_enhanced_transfer(dw_qspi_regs_t *qspi_base,dw_qspi_xfer_enhance_cfg_t * cfg);


#ifdef __cplusplus
}
#endif

#endif  /* _DW_QSPI_LL_H_*/
