/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_spi_ll.h
 * @brief
 * @version
 * @date     2020-02-10
 ******************************************************************************/

#ifndef _DW_SPI_LL_H_
#define _DW_SPI_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPI register bit definitions
 */

/* CTRLR0, offset: 0x00 */
#define DW_SPI_CTRLR0_SSTE_Pos                    (24U)
#define DW_SPI_CTRLR0_SSTE_Msk                    (0x1U << DW_SPI_CTRLR0_SSTE_Pos) 
#define DW_SPI_CTRLR0_SSTE_EN                     DW_SPI_CTRLR0_SSTE_Msk

#define DW_SPI_CTRLR0_CFS_Pos                     (12U)
#define DW_SPI_CTRLR0_CFS_Msk                     (0xFU << DW_SPI_CTRLR0_CFS_Pos)

#define DW_SPI_CTRLR0_SRL_Pos                     (11U)
#define DW_SPI_CTRLR0_SRL_Msk                     (0x1U << DW_SPI_CTRLR0_SRL_Pos)
#define DW_SPI_CTRLR0_SRL_EN                      DW_SPI_CTRLR0_SRL_Msk

#define DW_SPI_CTRLR0_TMOD_Pos                    (8U)
#define DW_SPI_CTRLR0_TMOD_Msk                    (0x3U << DW_SPI_CTRLR0_TMOD_Pos)
#define DW_SPI_CTRLR0_TMOD_TX_RX                  (0x0U << DW_SPI_CTRLR0_TMOD_Pos)
#define DW_SPI_CTRLR0_TMOD_TX                     (0x1U << DW_SPI_CTRLR0_TMOD_Pos)
#define DW_SPI_CTRLR0_TMOD_RX                     (0x2U << DW_SPI_CTRLR0_TMOD_Pos)
#define DW_SPI_CTRLR0_TMOD_EEPROM                 (0x3U << DW_SPI_CTRLR0_TMOD_Pos)

#define DW_SPI_CTRLR0_SCPOL_Pos                   (7U)
#define DW_SPI_CTRLR0_SCPOL_Msk                   (0x1U << DW_SPI_CTRLR0_SCPOL_Pos)
#define DW_SPI_CTRLR0_SCPOL_EN                    DW_SPI_CTRLR0_SCPOL_Msk

#define DW_SPI_CTRLR0_SCPH_Pos                    (6U)
#define DW_SPI_CTRLR0_SCPH_Msk                    (0x1U << DW_SPI_CTRLR0_SCPH_Pos)
#define DW_SPI_CTRLR0_SCPH_EN                     DW_SPI_CTRLR0_SCPH_Msk

#define DW_SPI_CTRLR0_FRF_Pos                     (4U)
#define DW_SPI_CTRLR0_FRF_Msk                     (0x3U << DW_SPI_CTRLR0_FRF_Pos)
#define DW_SPI_CTRLR0_FRF_MOTOROLA_SPI            (0x0U << DW_SPI_CTRLR0_FRF_Pos)
#define DW_SPI_CTRLR0_FRF_TI_SSP                  (0x1U << DW_SPI_CTRLR0_FRF_Pos)
#define DW_SPI_CTRLR0_FRF_MW_SPI                  (0x2U << DW_SPI_CTRLR0_FRF_Pos)

#define DW_SPI_CTRLR0_DFS_Pos                     (0U)
#define DW_SPI_CTRLR0_DFS_Msk                     (0xFU << DW_SPI_CTRLR0_DFS_Pos)

/* CTRLR1, offset: 0x04 */
#define DW_SPI_CTRLR1_NDF_Pos                     (0U)
#define DW_SPI_CTRLR1_NDF_Msk                     (0xFF << DW_SPI_CTRLR1_NDF_Pos)

/* SSIENR, offset: 0x08 */
#define DW_SPI_SSIENR_SSI_HW1_EN_Pos              (2U)
#define DW_SPI_SSIENR_SSI_HW1_EN_Msk              (0x1U << DW_SPI_SSIENR_SSI_HW1_EN_Pos)
#define DW_SPI_SSIENR_SSI_HW1_EN                  DW_SPI_SSIENR_SSI_HW1_EN_Msk

#define DW_SPI_SSIENR_SSI_HW0_EN_Pos              (1U)
#define DW_SPI_SSIENR_SSI_HW0_EN_Msk              (0x1U << DW_SPI_SSIENR_SSI_HW0_EN_Pos)
#define DW_SPI_SSIENR_SSI_HW0_EN                  DW_SPI_SSIENR_SSI_HW0_EN_Msk

#define DW_SPI_SSIENR_SSI_EN_Pos                  (0U)
#define DW_SPI_SSIENR_SSI_EN_Msk                  (0x1U << DW_SPI_SSIENR_SSI_EN_Pos)
#define DW_SPI_SSIENR_SSI_EN                      DW_SPI_SSIENR_SSI_EN_Msk

/* MWCR, offset: 0x10 */
#define DW_SPI_MWCR_MHS_Pos                       (2U)
#define DW_SPI_MWCR_MHS_Msk                       (0x1U << DW_SPI_MWCR_MHS_Pos)
#define DW_SPI_MWCR_MHS_EN                        DW_SPI_MWCR_MHS_Msk

#define DW_SPI_MWCR_MDD_Pos                       (1U)
#define DW_SPI_MWCR_MDD_Msk                       (0x1U << DW_SPI_MWCR_MHS_Pos)
#define DW_SPI_MWCR_MDD_INPUT                     (0x0U << DW_SPI_MWCR_MHS_Pos)
#define DW_SPI_MWCR_MDD_OUTPUT                    (0x1U << DW_SPI_MWCR_MHS_Pos)

#define DW_SPI_MWCR_MWMOD_Pos                     (0U)
#define DW_SPI_MWCR_MWMOD_Msk                     (0x1U << DW_SPI_MWCR_MWMOD_Pos)
#define DW_SPI_MWCR_MWMOD_SQUENTIAL               (0x1U << DW_SPI_MWCR_MWMOD_Pos)
#define DW_SPI_MWCR_MWMOD_NOSQUENTIAL             (0x0U << DW_SPI_MWCR_MWMOD_Pos)

/* BAUDR, offset: 0x14 */
#define DW_SPI_BAUDR_SCKDV_Pos                    (0U)
#define DW_SPI_BAUDR_SCKDV_Msk                    (0xFFU << DW_SPI_BAUDR_SCKDV_Pos)

/* TXFTLR, offset: 0x18 */
#define DW_SPI_TXFTLR_TFT_Pos                     (0U)
#define DW_SPI_TXFTLR_TFT_Msk                     (0xFFU << DW_SPI_TXFTLR_TFT_Pos)

/* RXFTLR, offset: 0x1C */
#define DW_SPI_RXFTLR_RFT_Pos                     (0U)
#define DW_SPI_RXFTLR_RFT_Msk                     (0xFFU << DW_SPI_RXFTLR_RFT_Pos)

/* SR, offset: 0x28 */
#define DW_SPI_SR_Pos                             (0U)
#define DW_SPI_SR_Msk                             (0x7FU << DW_SPI_SR_Pos)
#define DW_SPI_SR_DCOL                            (0x40U << DW_SPI_SR_Pos)
#define DW_SPI_SR_TXE                             (0x20U << DW_SPI_SR_Pos)
#define DW_SPI_SR_RFF                             (0x10U << DW_SPI_SR_Pos)
#define DW_SPI_SR_RFNE                            (0x08U << DW_SPI_SR_Pos)
#define DW_SPI_SR_TFE                             (0x04U << DW_SPI_SR_Pos)
#define DW_SPI_SR_TFNF                            (0x02U << DW_SPI_SR_Pos)
#define DW_SPI_SR_BUSY                            (0x01U << DW_SPI_SR_Pos)

/* IMR, offset: 0x2C */
#define DW_SPI_IMR_MSTIM_Pos                      (5U)
#define DW_SPI_IMR_MSTIM_Msk                      (0x1U << DW_SPI_IMR_MSTIM_Pos)
#define DW_SPI_IMR_MSTIM_EN                       DW_SPI_IMR_MSTIM_Msk

#define DW_SPI_IMR_RXFIM_Pos                      (4U)
#define DW_SPI_IMR_RXFIM_Msk                      (0x1U << DW_SPI_IMR_RXFIM_Pos)
#define DW_SPI_IMR_RXFIM_EN                       DW_SPI_IMR_RXFIM_Msk

#define DW_SPI_IMR_RXOIM_Pos                      (3U)
#define DW_SPI_IMR_RXOIM_Msk                      (0x1U << DW_SPI_IMR_RXOIM_Pos)
#define DW_SPI_IMR_RXOIM_EN                       DW_SPI_IMR_RXOIM_Msk

#define DW_SPI_IMR_RXUIM_Pos                      (2U)
#define DW_SPI_IMR_RXUIM_Msk                      (0x1U << DW_SPI_IMR_RXUIM_Pos)
#define DW_SPI_IMR_RXUIM_EN                       DW_SPI_IMR_RXUIM_Msk

#define DW_SPI_IMR_TXOIM_Pos                      (1U)
#define DW_SPI_IMR_TXOIM_Msk                      (0x1U << DW_SPI_IMR_TXOIM_Pos)
#define DW_SPI_IMR_TXOIM_EN                       DW_SPI_IMR_TXOIM_Msk

#define DW_SPI_IMR_TXEIM_Pos                      (0U)
#define DW_SPI_IMR_TXEIM_Msk                      (0x1U << DW_SPI_IMR_TXEIM_Pos)
#define DW_SPI_IMR_TXEIM_EN                       DW_SPI_IMR_TXEIM_Msk

/* ISR, offset: 0x30 */
#define DW_SPI_ISR_Pos                            (0U)
#define DW_SPI_ISR_Msk                            (0x3FU << DW_SPI_ISR_Pos)
#define DW_SPI_ISR_MSTIS                          (0x20U << DW_SPI_ISR_Pos)
#define DW_SPI_ISR_RXFIS                          (0x10U << DW_SPI_ISR_Pos)
#define DW_SPI_ISR_RXOIS                          (0x08U << DW_SPI_ISR_Pos)
#define DW_SPI_ISR_RXUIS                          (0x04U << DW_SPI_ISR_Pos)
#define DW_SPI_ISR_TXOIS                          (0x02U << DW_SPI_ISR_Pos)
#define DW_SPI_ISR_TXEIS                          (0x01U << DW_SPI_ISR_Pos)

/* RISR, offset: 0x34 */
#define DW_SPI_RISR_Pos                            (0U)
#define DW_SPI_RISR_Msk                            (0x3FU << DW_SPI_RISR_Pos)
#define DW_SPI_RISR_MSTIR                          (0x20U << DW_SPI_RISR_Pos)
#define DW_SPI_RISR_RXFIR                          (0x10U << DW_SPI_RISR_Pos)
#define DW_SPI_RISR_RXOIR                          (0x08U << DW_SPI_RISR_Pos)
#define DW_SPI_RISR_RXUIR                          (0x04U << DW_SPI_RISR_Pos)
#define DW_SPI_RISR_TXOIR                          (0x02U << DW_SPI_RISR_Pos)
#define DW_SPI_RISR_TXEIR                          (0x01U << DW_SPI_RISR_Pos)


/* DMACR, offset: 0x4C */
#define DW_SPI_DMACR_TDMAE_Pos                     (1U)
#define DW_SPI_DMACR_TDMAE_Msk                     (0x1U << DW_SPI_DMACR_TDMAE_Pos)
#define DW_SPI_DMACR_TDMAE_EN                      DW_SPI_DMACR_TDMAE_Msk

#define DW_SPI_DMACR_RDMAE_Pos                     (0U)
#define DW_SPI_DMACR_RDMAE_Msk                     (0x1U << DW_SPI_DMACR_RDMAE_Pos)
#define DW_SPI_DMACR_RDMAE_EN                      DW_SPI_DMACR_RDMAE_Msk


/* DMATDLR, offset: 0x50 */
#define DW_SPI_DMATDLR_DMATDL_Pos                  (0U)
#define DW_SPI_DMATDLR_DMATDL_Msk                  (0x1FU << DW_SPI_DMATDLR_DMATDL_Pos)

/* DMARDLR, offset: 0x54 */
#define DW_SPI_DMARDLR_DMARDL_Pos                  (0U)
#define DW_SPI_DMARDLR_DMARDL_Msk                  (0x1FU << DW_SPI_DMARDLR_DMARDL_Pos)

/* SPIMSSEL, offset: 0xA0 */
#define DW_SPI_SPIMSSEL_Pos                        (0U)
#define DW_SPI_SPIMSSEL_Msk                        (0x1U << DW_SPI_SPIMSSEL_Pos)
#define DW_SPI_SPIMSSEL_MASTER                     (0x1U << DW_SPI_SPIMSSEL_Pos)
#define DW_SPI_SPIMSSEL_SLAVE                      (0x0U << DW_SPI_SPIMSSEL_Pos)

typedef struct {
    __IOM uint32_t CTRLR0;      /* Offset: 0x000 (R/W)  Control register 0 */
    __IOM uint32_t CTRLR1;      /* Offset: 0x004 (R/W)  Control register 1 */
    __IOM uint32_t SSIENR;      /* Offset: 0x008 (R/W)  SSI enable regiseter */
    __IOM uint32_t MWCR;;
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

    uint32_t RESERVED2[3];
    __IOM uint32_t SPIMSSEL;    /* Offset: 0x200 (R/W)  SPI is Master or Slave Select Register */
    __IOM uint32_t SPIINTR;     /* Offset: 0x204 (R/W)  SPI Interrupt is from SPI Master or SPI Slave */
} dw_spi_regs_t;

static inline void dw_spi_enable_slave_select_toggle(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_SSTE_EN;
}

static inline void dw_spi_disable_slave_select_toggle(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_SSTE_EN;
}

static inline void dw_spi_config_ctl_frame_len(dw_spi_regs_t *spi_base, uint32_t len)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_CFS_Msk;
    spi_base->CTRLR0 |= (len << DW_SPI_CTRLR0_CFS_Pos);
}

static inline void dw_spi_enable_test_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_SRL_EN;
}

static inline void dw_spi_disable_test_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_SRL_EN;
}

static inline void dw_spi_set_tx_rx_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_TMOD_Msk;
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_TMOD_TX_RX;
}

static inline void dw_spi_set_tx_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_TMOD_Msk;
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_TMOD_TX;
}

static inline void dw_spi_set_rx_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_TMOD_Msk;
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_TMOD_RX;
}

static inline uint32_t dw_spi_get_transfer_mode(dw_spi_regs_t *spi_base)
{
    return spi_base->CTRLR0 & DW_SPI_CTRLR0_TMOD_Msk;
}

static inline void dw_spi_set_eeprom_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_TMOD_Msk;
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_TMOD_EEPROM;
}

static inline void dw_spi_set_cpol0(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~(DW_SPI_CTRLR0_SCPOL_EN);
}

static inline void dw_spi_set_cpol1(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_SCPOL_EN;
}

static inline void dw_spi_set_cpha0(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~(DW_SPI_CTRLR0_SCPH_EN);
}

static inline void dw_spi_set_cpha1(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_SCPH_EN;
}

static inline void dw_spi_set_motorola_spi_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_FRF_Msk;
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_FRF_MOTOROLA_SPI;
}

static inline void dw_spi_set_ti_ssp_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_FRF_Msk;
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_FRF_TI_SSP;
}

static inline void dw_spi_set_mw_mode(dw_spi_regs_t *spi_base)
{
    spi_base->CTRLR0 &= ~DW_SPI_CTRLR0_FRF_Msk;
    spi_base->CTRLR0 |= DW_SPI_CTRLR0_FRF_MW_SPI;
}

static inline void dw_spi_config_rx_data_len(dw_spi_regs_t *spi_base, uint32_t len)
{
    spi_base->CTRLR1 = len;
}

static inline void dw_spi_enable_trigger0(dw_spi_regs_t *spi_base)
{
    spi_base->SSIENR |= DW_SPI_SSIENR_SSI_HW0_EN;
}

static inline void dw_spi_disable_trigger0(dw_spi_regs_t *spi_base)
{
    spi_base->SSIENR &= ~DW_SPI_SSIENR_SSI_HW0_EN;
}

static inline void dw_spi_enable_trigger1(dw_spi_regs_t *spi_base)
{
    spi_base->SSIENR |= DW_SPI_SSIENR_SSI_HW1_EN;
}

static inline void dw_spi_disable_trigger1(dw_spi_regs_t *spi_base)
{
    spi_base->SSIENR &= DW_SPI_SSIENR_SSI_HW1_EN;
}

static inline void dw_spi_enable(dw_spi_regs_t *spi_base)
{
    spi_base->SSIENR |= DW_SPI_SSIENR_SSI_EN;
}

static inline void dw_spi_disable(dw_spi_regs_t *spi_base)
{
    spi_base->SSIENR &= ~(DW_SPI_SSIENR_SSI_EN);
}

static inline void dw_spi_set_mw_sequential_transfer(dw_spi_regs_t *spi_base)
{
    spi_base->MWCR |= DW_SPI_MWCR_MWMOD_SQUENTIAL;
}

static inline void dw_spi_set_mw_nonsequential_transfer(dw_spi_regs_t *spi_base)
{
    spi_base->MWCR &= ~DW_SPI_MWCR_MWMOD_Msk;
    spi_base->MWCR |= DW_SPI_MWCR_MWMOD_NOSQUENTIAL;
}

static inline void dw_spi_set_mw_direction_output(dw_spi_regs_t *spi_base)
{
    spi_base->MWCR &= ~DW_SPI_MWCR_MDD_Msk;
    spi_base->MWCR |= DW_SPI_MWCR_MDD_OUTPUT;
}

static inline void dw_spi_set_mw_direction_input(dw_spi_regs_t *spi_base)
{
    spi_base->MWCR &= ~DW_SPI_MWCR_MDD_Msk;
    spi_base->MWCR |= DW_SPI_MWCR_MDD_INPUT;
}

static inline void dw_spi_enable_mw_handshaking(dw_spi_regs_t *spi_base)
{
    spi_base->MWCR |= DW_SPI_MWCR_MHS_EN;
}

static inline void dw_spi_disable_mw_handshaking(dw_spi_regs_t *spi_base)
{
    spi_base->MWCR &= DW_SPI_MWCR_MHS_EN;
}

static inline void dw_spi_enable_slave(dw_spi_regs_t *spi_base, uint32_t idx)
{
    spi_base->SER |= ((uint32_t)1U << idx);
}

static inline void dw_spi_disable_slave(dw_spi_regs_t *spi_base, uint32_t idx)
{
    spi_base->SER &= ((uint32_t) 1U << idx);
}

static inline void dw_spi_disable_all_slave(dw_spi_regs_t *spi_base)
{
    spi_base->SER = 0U;
}

static inline void dw_spi_config_tx_fifo_threshold(dw_spi_regs_t *spi_base, uint32_t value)
{
    spi_base->TXFTLR = value & DW_SPI_TXFTLR_TFT_Msk;
}

static inline void dw_spi_config_rx_fifo_threshold(dw_spi_regs_t *spi_base, uint32_t value)
{
    spi_base->RXFTLR = value & DW_SPI_RXFTLR_RFT_Msk;
}

static inline uint32_t dw_spi_get_tx_fifo_level(dw_spi_regs_t *spi_base)
{
    return spi_base->TXFLR;
}

static inline uint32_t dw_spi_get_rx_fifo_level(dw_spi_regs_t *spi_base)
{
    return spi_base->RXFLR;
}

static inline uint32_t dw_spi_get_status(dw_spi_regs_t *spi_base)
{
    return spi_base->SR;
}

static inline void dw_spi_enable_all_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR |= DW_SPI_IMR_TXEIM_EN;
    spi_base->IMR |= DW_SPI_IMR_TXOIM_EN;
    spi_base->IMR |= DW_SPI_IMR_RXUIM_EN;
    spi_base->IMR |= DW_SPI_IMR_RXOIM_EN;
    spi_base->IMR |= DW_SPI_IMR_RXFIM_EN;
    spi_base->IMR |= DW_SPI_IMR_MSTIM_EN;
}

static inline void dw_spi_disable_all_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR = 0U;
}

static inline void dw_spi_enable_multi_master_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR |= DW_SPI_IMR_MSTIM_EN;
}

static inline void dw_spi_disable_multi_master_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR &= ~DW_SPI_IMR_MSTIM_EN;
}

static inline void dw_spi_enable_rx_fifo_full_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR |= DW_SPI_IMR_RXFIM_EN;
}

static inline void dw_spi_disable_rx_fifo_full_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR &= ~DW_SPI_IMR_RXFIM_EN;
}

static inline void dw_spi_enable_rx_fifo_overflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR |= DW_SPI_IMR_RXOIM_EN;
}

static inline void dw_spi_disable_rx_fifo_overflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR &= ~DW_SPI_IMR_RXOIM_EN;
}

static inline void dw_spi_enable_rx_fifo_underflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR |= DW_SPI_IMR_RXUIM_EN;
}

static inline void dw_spi_disable_rx_fifo_underflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR &= ~DW_SPI_IMR_RXUIM_EN;
}

static inline void dw_spi_enable_tx_fifo_overflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR |= DW_SPI_IMR_TXOIM_EN;
}

static inline void dw_spi_disable_tx_fifo_overflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR &= ~DW_SPI_IMR_TXOIM_EN;
}

static inline void dw_spi_enable_tx_empty_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR |= DW_SPI_IMR_TXEIM_EN;
}

static inline void dw_spi_disable_tx_empty_irq(dw_spi_regs_t *spi_base)
{
    spi_base->IMR &= ~DW_SPI_IMR_TXEIM_EN;
}

static inline uint32_t dw_spi_get_interrupt_status(dw_spi_regs_t *spi_base)
{
    return spi_base->ISR;
}

static inline uint32_t dw_spi_get_raw_interrupt_status(dw_spi_regs_t *spi_base)
{
    return spi_base->RISR;
}

static inline void dw_spi_clr_tx_fifo_overflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->TXOICR;
}

static inline void dw_spi_clr_rx_fifo_overflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->RXOICR;
}

static inline void dw_spi_clr_rx_fifo_underflow_irq(dw_spi_regs_t *spi_base)
{
    spi_base->RXUICR;
}

static inline void dw_spi_clr_multi_master_irq(dw_spi_regs_t *spi_base)
{
    spi_base->MSTICR;
}

static inline void dw_spi_clr_all_irqs(dw_spi_regs_t *spi_base)
{
    spi_base->ICR;
}

static inline void dw_spi_enable_tx_dma(dw_spi_regs_t *spi_base)
{
    spi_base->DMACR |= DW_SPI_DMACR_TDMAE_EN;
}

static inline void dw_spi_disable_tx_dma(dw_spi_regs_t *spi_base)
{
    spi_base->DMACR &= ~DW_SPI_DMACR_TDMAE_EN;
}

static inline void dw_spi_enable_rx_dma(dw_spi_regs_t *spi_base)
{
    spi_base->DMACR |= DW_SPI_DMACR_RDMAE_EN;
}

static inline void dw_spi_disable_rx_dma(dw_spi_regs_t *spi_base)
{
    spi_base->DMACR &= ~DW_SPI_DMACR_RDMAE_EN;
}

static inline void dw_spi_config_dma_tx_data_level(dw_spi_regs_t *spi_base, uint32_t value)
{
    spi_base->DMATDLR = value & DW_SPI_DMATDLR_DMATDL_Msk;
}

static inline uint32_t dw_spi_get_dma_tx_data_level(dw_spi_regs_t *spi_base)
{
    return spi_base->DMATDLR;
}

static inline void dw_spi_config_dma_rx_data_level(dw_spi_regs_t *spi_base, uint32_t value)
{
    spi_base->DMARDLR = value & DW_SPI_DMARDLR_DMARDL_Msk;
}

static inline uint32_t dw_spi_get_dma_rx_data_level(dw_spi_regs_t *spi_base)
{
    return spi_base->DMARDLR;
}

static inline uint32_t dw_spi_get_id(dw_spi_regs_t *spi_base)
{
    return spi_base->IDR;
}

static inline uint32_t dw_spi_get_version(dw_spi_regs_t *spi_base)
{
    return spi_base->SSI_COMP_VERSION;
}

static inline void dw_spi_transmit_data(dw_spi_regs_t *spi_base, uint32_t data)
{
    spi_base->DR = data;
}

static inline uint32_t dw_spi_receive_data(dw_spi_regs_t *spi_base)
{
    return spi_base->DR;
}

static inline uint32_t dw_spi_get_rx_sample_deley(dw_spi_regs_t *spi_base)
{
    return spi_base->RX_SAMPLE_DLY;
}

static inline void dw_spi_config_rx_sample_delay(dw_spi_regs_t *spi_base, uint8_t delay)
{
    spi_base->RX_SAMPLE_DLY = delay;
}

static inline void dw_spi_set_master_mode(dw_spi_regs_t *spi_base)
{
    spi_base->SPIMSSEL &= ~DW_SPI_SPIMSSEL_Msk;
    spi_base->SPIMSSEL |= DW_SPI_SPIMSSEL_MASTER;
}

static inline void dw_spi_set_slave_mode(dw_spi_regs_t *spi_base)
{
    spi_base->SPIMSSEL &= ~DW_SPI_SPIMSSEL_Msk;
    spi_base->SPIMSSEL |= DW_SPI_SPIMSSEL_SLAVE;
}

static inline uint32_t dw_spi_get_slave_mode(dw_spi_regs_t *spi_base)
{
    return spi_base->SPIMSSEL & DW_SPI_SPIMSSEL_Msk;
}

extern void dw_spi_config_sclk_clock(dw_spi_regs_t *spi_base, uint32_t clock_in, uint32_t clock_out);
extern uint32_t dw_spi_get_sclk_clock_div(dw_spi_regs_t *spi_base);
extern uint32_t dw_spi_get_data_frame_len(dw_spi_regs_t *spi_base);
extern void dw_spi_config_data_frame_len(dw_spi_regs_t *spi_base, uint32_t size);
extern void dw_spi_reset_regs(dw_spi_regs_t *spi_base);
#ifdef __cplusplus
}
#endif

#endif  /* _DW_SPI_LL_H_*/
