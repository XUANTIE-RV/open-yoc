/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sysctl.h
 * @brief    header file for the sysctl
 * @version  V1.0
 * @date     04. April 2019
 ******************************************************************************/

#include <stdint.h>
#include <soc.h>

#define SYS_LOCK_LOCK_MAGIC         (0xA05F0010)
#define SYS_LOCK_UNLOCK_MAGIC       (0x05FA59A6)

#define SYS_CTR0_SYSCLK_CS_Pos    (29U)
#define SYS_CTR0_SYSCLK_CS_Msk    (0x3U << SYS_CTR0_SYSCLK_CS_Pos)
#define SYS_CTR0_SYSCLK_CS_RCH    (0x0U << SYS_CTR0_SYSCLK_CS_Pos)
#define SYS_CTR0_SYSCLK_CS_OSC    (0x1U << SYS_CTR0_SYSCLK_CS_Pos)
#define SYS_CTR0_SYSCLK_CS_PLL    (0x2U << SYS_CTR0_SYSCLK_CS_Pos)
#define SYS_CTR0_SYSCLK_CS_RCL    (0x3U << SYS_CTR0_SYSCLK_CS_Pos)
#define SYS_CTR0_RCL_EN_Pos       (28U)
#define SYS_CTR0_RCL_EN_Msk       (0x1U << SYS_CTR0_RCL_EN_Pos)
#define SYS_CTR0_RCH_EN_Pos       (27U)
#define SYS_CTR0_RCH_EN_Msk       (0x1U << SYS_CTR0_RCH_EN_Pos)
#define SYS_CTR0_OSC_EN_Pos       (26U)
#define SYS_CTR0_OSC_EN_Msk       (0x1U << SYS_CTR0_OSC_EN_Pos)
#define SYS_CTR0_SYS_PLL_EN_Pos   (25U)
#define SYS_CTR0_SYS_PLL_EN_Msk   (0x1U << SYS_CTR0_SYS_PLL_EN_Pos)
#define SYS_CTR0_32K_PLL_EN_Pos   (24U)
#define SYS_CTR0_32K_PLL_EN_Msk   (0x1U << SYS_CTR0_32K_PLL_EN_Pos)
#define SYS_CTR0_AUDIO_PLL_EN_Pos (23U)
#define SYS_CTR0_AUDIO_PLL_EN_Msk (0x1U << SYS_CTR0_AUDIO_PLL_EN_Pos)

#define SYS_CTL2_WDT_RESET_EN_Pos (28U)
#define SYS_CTL2_WDT_RESET_EN_Msk (0x1U << SYS_CTL2_WDT_RESET_EN_Pos)

typedef struct {
    __IOM uint32_t SYS_LOCK;         /* Offset: 0x000 (R/W) */
    __IOM uint32_t SYS_CTR0;         /* Offset: 0x004 (R/W) */
    __IOM uint32_t SYS_CTR1;         /* Offset: 0x008 (R/W) */
    __IOM uint32_t SYS_CTR2;         /* Offset: 0x00C (R/W) */
    __IOM uint32_t SYS_STS;          /* Offset: 0x010 (R/W) */
    __IOM uint32_t WDT_TG;           /* Offset: 0x014 (R/W) */
    __IOM uint32_t SYS_PLL_CTRL1;    /* Offset: 0x018 (R/W) */
    __IOM uint32_t SYS_PLL_CTRL2;    /* Offset: 0x01C (R/W) */
    __IOM uint32_t SSCG_CTRL;        /* Offset: 0x020 (R/W) */
} silan_sysctl_reg_t;
