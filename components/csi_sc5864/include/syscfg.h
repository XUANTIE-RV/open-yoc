/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     syscfg.h
 * @brief    header file for the sysctl
 * @version  V1.0
 * @date     13. June 2019
 ******************************************************************************/

#include <stdint.h>
#include <soc.h>
#include <silan_syscfg.h>

#define CTR_REG0_SD_CLKON_Pos            (0U)
#define CTR_REG0_SD_CLKON_Msk            (0x1U << CTR_REG0_SD_CLKON_Pos)
#define CTR_REG0_SDIO_CLKON_Pos          (1U)
#define CTR_REG0_SDIO_CLKON_Msk          (0x1U << CTR_REG0_SDIO_CLKON_Pos)
#define CTR_REG0_USBHS_CLKON_Pos         (2U)
#define CTR_REG0_USBHS_CLKON_Msk         (0x1U << CTR_REG0_USBHS_CLKON_Pos)
#define CTR_REG0_USBFS_CLKON_Pos         (3U)
#define CTR_REG0_USBFS_CLKON_Msk         (0x1U << CTR_REG0_USBFS_CLKON_Pos)
#define CTR_REG0_SDMAC_CLKON_Pos         (4U)
#define CTR_REG0_SDMAC_CLKON_Msk         (0x1U << CTR_REG0_SDMAC_CLKON_Pos)
#define CTR_REG0_OSPDIF_CLKON_Pos        (5U)
#define CTR_REG0_OSPDIF_CLKON_Msk        (0x1U << CTR_REG0_OSPDIF_CLKON_Pos)
#define CTR_REG0_ISPDIF_CLKON_Pos        (6U)
#define CTR_REG0_ISPDIF_CLKON_Msk        (0x1U << CTR_REG0_ISPDIF_CLKON_Pos)
#define CTR_REG0_I1_I2S_CLKON_Pos        (7U)
#define CTR_REG0_I1_I2S_CLKON_Msk        (0x1U << CTR_REG0_I1_I2S_CLKON_Pos)
#define CTR_REG0_I2_I2S_CLKON_Pos        (8U)
#define CTR_REG0_I2_I2S_CLKON_Msk        (0x1U << CTR_REG0_I2_I2S_CLKON_Pos)
#define CTR_REG0_I3_I2S_CLKON_Pos        (9U)
#define CTR_REG0_I3_I2S_CLKON_Msk        (0x1U << CTR_REG0_I3_I2S_CLKON_Pos)
#define CTR_REG0_O1_I2S_CLKON_Pos        (10U)
#define CTR_REG0_O1_I2S_CLKON_Msk        (0x1U << CTR_REG0_O1_I2S_CLKON_Pos)
#define CTR_REG0_O2_I2S_CLKON_Pos        (11U)
#define CTR_REG0_O2_I2S_CLKON_Msk        (0x1U << CTR_REG0_O2_I2S_CLKON_Pos)
#define CTR_REG0_O3_I2S_CLKON_Pos        (12U)
#define CTR_REG0_O3_I2S_CLKON_Msk        (0x1U << CTR_REG0_O3_I2S_CLKON_Pos)
#define CTR_REG0_PCM_I2S_CLKON_Pos       (13U)
#define CTR_REG0_PCM_I2S_CLKON_Msk       (0x1U << CTR_REG0_PCM_I2S_CLKON_Pos)
#define CTR_REG0_PDM_CLKON_Pos           (14U)
#define CTR_REG0_PDM_CLKON_Msk           (0x1U << CTR_REG0_PDM_CLKON_Pos)
#define CTR_REG0_ADMAC_CLKON_Pos         (15U)
#define CTR_REG0_ADMAC_CLKON_Msk         (0x1U << CTR_REG0_ADMAC_CLKON_Pos)
#define CTR_REG0_GPIO1_CLKON_Pos         (16U)
#define CTR_REG0_GPIO1_CLKON_Msk         (0x1U << CTR_REG0_GPIO1_CLKON_Pos)
#define CTR_REG0_GPIO2_CLKON_Pos         (17U)
#define CTR_REG0_GPIO2_CLKON_Msk         (0x1U << CTR_REG0_GPIO2_CLKON_Pos)
#define CTR_REG0_SPI_CLKON_Pos           (18U)
#define CTR_REG0_SPI_CLKON_Msk           (0x1U << CTR_REG0_SPI_CLKON_Pos)
#define CTR_REG0_UART1_CLKON_Pos         (19U)
#define CTR_REG0_UART1_CLKON_Msk         (0x1U << CTR_REG0_UART1_CLKON_Pos)
#define CTR_REG0_UART2_CLKON_Pos         (20U)
#define CTR_REG0_UART2_CLKON_Msk         (0x1U << CTR_REG0_UART2_CLKON_Pos)
#define CTR_REG0_UART3_CLKON_Pos         (21U)
#define CTR_REG0_UART3_CLKON_Msk         (0x1U << CTR_REG0_UART3_CLKON_Pos)
#define CTR_REG0_UART4_CLKON_Pos         (22U)
#define CTR_REG0_UART4_CLKON_Msk         (0x1U << CTR_REG0_UART4_CLKON_Pos)
#define CTR_REG0_I2C1_CLKON_Pos          (23U)
#define CTR_REG0_I2C1_CLKON_Msk          (0x1U << CTR_REG0_I2C1_CLKON_Pos)
#define CTR_REG0_I2C2_CLKON_Pos          (24U)
#define CTR_REG0_I2C2_CLKON_Msk          (0x1U << CTR_REG0_I2C2_CLKON_Pos)
#define CTR_REG0_PMU_CLKON_Pos           (25U)
#define CTR_REG0_PMU_CLKON_Msk           (0x1U << CTR_REG0_PMU_CLKON_Pos)
#define CTR_REG0_CXC_CLKON_Pos           (26U)
#define CTR_REG0_CXC_CLKON_Msk           (0x1U << CTR_REG0_CXC_CLKON_Pos)
#define CTR_REG0_IIR_CLKON_Pos           (27U)
#define CTR_REG0_IIR_CLKON_Msk           (0x1U << CTR_REG0_IIR_CLKON_Pos)
#define CTR_REG0_TIMER_CLKON_Pos         (28U)
#define CTR_REG0_TIMER_COKON_Msk         (0x1U << CTR_REG0_TIMER_CLKON_Pos)
#define CTR_REG0_PDP_CLKON_Pos           (29U)
#define CTR_REG0_PDP_CLKON_Msk           (0x1U << CTR_REG0_PDP_CLKON_Pos)
#define CTR_REG0_PWM_CLKON_Pos           (30U)
#define CTR_REG0_PWM_CLKON_Msk           (0x1U << CTR_REG0_PWM_CLKON_Pos)
#define CTR_REG0_ADC_CLKON_Pos           (31U)
#define CTR_REG0_ADC_CLKON_Msk           (0x1U << CTR_REG0_ADC_CLKON_Pos)

#define CTR_SDR_PD_SREF_REQ_Pos      (0U)
#define CTR_SDR_PD_SREF_REQ_Msk      (0x1U << CTR_SDR_PD_SREF_REQ_Pos)
#define CTR_SDR_PD_SREF_ACK_Pos      (4U)
#define CTR_SDR_PD_SREF_ACK_Msk      (0x1U << CTR_SDR_PD_SREF_ACK_Pos)

#define CTR_SOC0_RUN_STALL_Pos       (0U)
#define CTR_SOC0_RUN_STALL_Msk       (0x1U << CTR_SOC0_RUN_STALL_Pos)

typedef struct {
    __IOM uint32_t CTR_REG0;         /* Offset: 0x000 (R/W) */
    __IOM uint32_t CTR_REG1;         /* Offset: 0x004 (R/W) */
    __IOM uint32_t CTR_REG2;         /* Offset: 0x008 (R/W) */
    __IOM uint32_t CTR_REG3;         /* Offset: 0x00C (R/W) */
    __IOM uint32_t CTR_REG4;         /* Offset: 0x010 (R/W) */
    __IOM uint32_t CTR_REG5;         /* Offset: 0x014 (R/W) */
    __IOM uint32_t CTR_REG6;         /* Offset: 0x018 (R/W) */
    __IOM uint32_t CTR_REG7;         /* Offset: 0x01C (R/W) */
    __IOM uint32_t CTR_REG8;         /* Offset: 0x020 (R/W) */
    __IOM uint32_t CTR_REG9;         /* Offset: 0x024 (R/W) */
          uint32_t _reserved0;
    __IOM uint32_t CTR_REG11;        /* Offset: 0x02C (R/W) */
    __IOM uint32_t CTR_REG12;        /* Offset: 0x030 (R/W) */
          uint32_t _reserved1;
    __IOM uint32_t CTR_REG14;        /* Offset: 0x038 (R/W) */
    __IOM uint32_t CTR_REG15;        /* Offset: 0x03C (R/W) */
    __IOM uint32_t CTR_REG16;        /* Offset: 0x040 (R/W) */
    __IOM uint32_t CTR_REG17;        /* Offset: 0x044 (R/W) */
    __IOM uint32_t CTR_REG18;        /* Offset: 0x048 (R/W) */
    __IOM uint32_t CTR_REG19;        /* Offset: 0x04C (R/W) */
    __IOM uint32_t _reserved2[44];
    __IOM uint32_t CTR_PAD0;         /* Offset: 0x100 (R/W) */
    __IOM uint32_t CTR_PAD1;         /* Offset: 0x104 (R/W) */
    __IOM uint32_t CTR_PAD2;         /* Offset: 0x108 (R/W) */
    __IOM uint32_t CTR_PAD3;         /* Offset: 0x10C (R/W) */
    __IOM uint32_t CTR_PAD4;         /* Offset: 0x110 (R/W) */
    __IOM uint32_t CTR_PAD5;         /* Offset: 0x114 (R/W) */
    __IOM uint32_t CTR_PAD6;         /* Offset: 0x118 (R/W) */
    __IOM uint32_t CTR_PAD7;         /* Offset: 0x11C (R/W) */
    __IOM uint32_t CTR_PAD8;         /* Offset: 0x120 (R/W) */
    __IOM uint32_t CTR_PAD9;         /* Offset: 0x124 (R/W) */
    __IOM uint32_t CTR_PAD10;        /* Offset: 0x128 (R/W) */
          uint32_t _reserved3[53];
    __IOM uint32_t CTR_DKEY0;        /* Offset: 0x200 (R/W) */
    __IOM uint32_t CTR_DKEY1;        /* Offset: 0x204 (R/W) */
    __IOM uint32_t CTR_DKEY2;        /* Offset: 0x208 (R/W) */
    __IOM uint32_t CTR_DKEY3;        /* Offset: 0x20C (R/W) */
          uint32_t _reserved4[60];
    __IOM uint32_t CTR_SOC0;         /* Offset: 0x300 (R/W) */
    __IOM uint32_t CTR_SOC1;         /* Offset: 0x304 (R/W) */
    __IOM uint32_t _reserved5[126];
    __IOM uint32_t CTR_SYS_LOCK;     /* Offset: 0x500 (R/W) */
          uint32_t _reserved6[63];
    __IOM uint32_t CTR_SDR_PD;       /* Offset: 0x600 (R/W) */
} silan_syscfg_reg_t;

void drv_clk_enable(uint32_t module);
void drv_clk_disable(uint32_t module);
