/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_pmu_v2.h
 * @brief    head file for ck pmu
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_PMU_V2_H_
#define _CK_PMU_V2_H_

#include <soc.h>
#include <drv/pmu.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_VIC_WAKEUP_BIT       31
#define CONFIG_LPM_RESUME_ADDR      SOC_LPM_RESUME_ADDR
#define CONFIG_PMU_ENTER_WAIT_MODE  0x4
#define CONFIG_PMU_ENTER_DOZE_MODE  0xc
#define CONFIG_PMU_ENTER_STOP_MODE  0x4
#define CONFIG_PMU_ENTER_STANDBY_MODE   0x14
#define CSKY_SOC_WAKEUP_REG0        (CSKY_IOC_BASE + 0x18)
#define CSKY_SOC_WAKEUP_LEVEL       (CSKY_IOC_BASE + 0x78)

#define PDU_DCU_MODE_CR1_Pos              (7U)
#define PDU_DCU_MODE_CR1_Msk              (0x3U << PDU_DCU_MODE_CR1_Pos)
#define PDU_DCU_MODE_CR1_GET_DIV(reg_val) (1 << ((reg_val & PDU_DCU_MODE_CR1_Msk) >> 7))

#define PDU_DCU_MODE_CR2_Pos              (10U)
#define PDU_DCU_MODE_CR2_Msk              (0x3U << PDU_DCU_MODE_CR2_Pos)
#define PDU_DCU_MODE_CR2_GET_DIV(reg_val) (1 << ((reg_val & PDU_DCU_MODE_CR2_Msk) >> 7))

#define MCLK_RATIO_Pos                    (0U)
#define MCLK_RATIO_Msk                    (0xFU << MCLK_RATIO_Pos)
#define MCLK_RATIO_GET_DIV(reg_val)       ((reg_val & MCLK_RATIO_Msk) + 1U)

typedef struct {
    __IOM uint32_t LPCR;           /* Offset: 0x000 (R/W) low power control register */
    uint32_t RESERVED0[5];
    __IOM uint32_t CLKSTBR;        /* Offset: 0x018 (R/W) clock stable register */
    __IOM uint32_t CLKSTBST;       /* Offset: 0x01c (R/W) clock stable interrupt statue register */
    __IOM uint32_t CLKSTBMK;       /* Offset: 0x020 (R/W) clock stable interrupt mask register */
    __IOM uint32_t CSSCR;          /* Offset: 0x024 (R/W) clock source stable counter register */
    __IOM uint32_t DFCC;           /* Offset: 0x028 (R/W) dynamic frequence conversion control register */
    __IOM uint32_t PCR;            /* Offset: 0x02c (R/W) pll control register */
    uint32_t RESERVED1[10];
    __IOM uint32_t RCTCR;          /* Offset: 0x058 (R/W) rc triming control register */
    __IM uint32_t RCTSR;           /* Offset: 0x05c (R/ ) rc triming status register */
    uint32_t RESERVED2[8];
    __IOM uint32_t WIME0;          /* Offset: 0x080 (R/W) wic interrupt mask0 register */
    __IOM uint32_t WIME1;          /* Offset: 0x084 (R/W) wic interrupt mask1 register */
    uint32_t RESERVED3[2];
    __IM uint32_t TISR;            /* Offset: 0x090 (R/ ) timer interrupt status register */
    uint32_t RESERVED4[27];
    __IOM uint32_t MODE_CHG_EN;    /* Offset: 0x100 (R/W) src/dcu mode change register */
    __IOM uint32_t SRC_CLKMD;      /* Offset: 0x104 (R/W) src clock mode register */
    __IOM uint32_t SRC_LDOMD;      /* Offset: 0x108 (R/W) src ldo mode register */
    uint32_t RESERVED5[9];
    __IOM uint32_t CPU_DCU_MODE;     /* Offset: 0x130 (R/W) CPU DCU mode register */
    __IOM uint32_t CPU_DCU_RST_DUR;  /* Offset: 0x134 (R/W) CPU DCU reset time of duration register */
    __IOM uint32_t CPU_DCU_ISO_DUR;  /* Offset: 0x138 (R/W) CPU DCU isolation time of duration register */
    __IOM uint32_t CPU_DCU_CGCR;     /* Offset: 0x13c (R/W) CPU DCU clock gate control register */
    __IM uint32_t CPU_DCU_CGSR;      /* Offset: 0x140 (R/ ) CPU DCU clock gate status register */
    __IOM uint32_t CPU_DCU_SWRST;    /* Offset: 0x144 (R/W) CPU DCU software reset control register */
    uint32_t RESERVED6[6];
    __IOM uint32_t SRAM_DCU_MODE;    /* Offset: 0x160 (R/W) SRAM DCU mode register */
    __IOM uint32_t SRAM_DCU_RST_MODE;/* Offset: 0x164 (R/W) SRAM DCU reset time of duration register */
    __IOM uint32_t SRAM_DCU_ISO_MODE;/* Offset: 0x168 (R/W) SRAM DCU isolation time of duration register */
    __IOM uint32_t SRAM_DCU_CGCR;    /* Offset: 0x16c (R/W) SRAM DCU clock gate control register */
    __IM uint32_t SRAM_DCU_CGSR;     /* Offset: 0x170 (R/ ) SRAM DCU clock gate status register */
    __IOM uint32_t SRAM_DCU_SWRST;   /* Offset: 0x174 (R/W) SRAM DCU software reset control register */
    uint32_t RESERVED7[6];
    __IOM uint32_t PDU_DCU_MODE;     /* Offset: 0x190 (R/W) PDU DCU mode register */
    __IOM uint32_t PDU_DCU_RST_MODE; /* Offset: 0x194 (R/W) PDU DCU reset time of duration register */
    __IOM uint32_t PDU_DCU_ISO_MODE; /* Offset: 0x198 (R/W) PDU DCU isolation time of duration register */
    __IOM uint32_t PDU_DCU_MCGCR;    /* Offset: 0x19c (R/W) PDU DCU matrix clock gate control register */
    __IM uint32_t PDU_DCU_MCGSR;     /* Offset: 0x1a0 (R/ ) PDU DCU matrix clock gate status register */
    __IOM uint32_t PDU_DCU_MSWRST;   /* Offset: 0x1a4 (R/W) PDU DCU matrix software reset control register */
    __IOM uint32_t PDU_DCU_P0CGCR;   /* Offset: 0x1a8 (R/W) PDU DCU apb0 clock gate control register */
    __IM uint32_t PDU_DCU_P0CGSR;    /* Offset: 0x1ac (R/ ) PDU DCU apb0 clock gate status register */
    __IOM uint32_t PDU_DCU_P0SWRST;  /* Offset: 0x1b0 (R/W) PDU DCU apb0 software reset control register */
    __IOM uint32_t PDU_DCU_P1CGCR;   /* Offset: 0x1b4 (R/W) PDU DCU apb1 clock gate control register */
    __IM uint32_t PDU_DCU_P1CGSR;    /* Offset: 0x1b8 (R/ ) PDU DCU apb1 clock gate status register */
    __IOM uint32_t PDU_DCU_P1SWRST;  /* Offset: 0x1bc (R/W) PDU DCU apb1 software reset control register */
    __IOM uint32_t ANA_DCU_MODE;     /* Offset: 0x1c0 (R/W) ANA DCU mode register */
    __IOM uint32_t ANA_DCU_RST_DUR;  /* Offset: 0x1c4 (R/W) ANA DCU reset time of duration register */
    __IOM uint32_t ANA_DCU_ISO_DUR;  /* Offset: 0x1c8 (R/W) ANA DCU isolation time of duration register */
    uint32_t RESERVED8[141];
    __IOM uint32_t USER_DEFINE[4];   /* Offset: 0x400-0x40c (R/W) recode some mesage in lowpower mode */
    uint32_t RESERVED9[60];
    __IOM uint32_t DCU_RST_DUR;      /* Offset: 0x500 (R/W) DCU reset duration register */
    __IOM uint32_t DCU_ISO_DUR;      /* Offset: 0x504 (R/W) DCU isolation duration register */
    __IOM uint32_t LDO_STB_CNT;      /* Offset: 0x508 (R/W) LDO stable counter register */
    __IOM uint32_t MCLK_RATIO;       /* Offset: 0x50c (R/W) MCLK ratio register */
    __IOM uint32_t RST_STATE;        /* Offset: 0x510 (R/W) RESET state register */
    __IOM uint32_t IP_CTRL;          /* Offset: 0x514 (R/W) IP control register */
    __IOM uint32_t DFC_CTRL;         /* Offset: 0x518 (R/W) DFC control register */
    uint32_t RESERVED10[1];
    __IOM uint32_t MODE_CTRL;        /* Offset: 0x520 (R/W) MODE control register */
    __IOM uint32_t MEM_RET_CTRL;     /* Offset: 0x524 (R/W) MEM retention control register */
} ck_pmu_reg_t;

#ifdef __cplusplus
}
#endif

#endif
