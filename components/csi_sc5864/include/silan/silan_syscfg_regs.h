/*
 * silan_syscfg_regs.h
 */

#ifndef __SILAN_SYSTEM_CONFIG_REGS_H__
#define __SILAN_SYSTEM_CONFIG_REGS_H__

#include "soc.h"


#define SILAN_SYSCFG_REG0                (SILAN_SYSCFG_BASE + 0x00)
#define SILAN_SYSCFG_REG1                (SILAN_SYSCFG_BASE + 0x04)

#define SILAN_SYSCFG_REG3               (SILAN_SYSCFG_BASE + 0x0c)
#define SILAN_SYSCFG_REG4               (SILAN_SYSCFG_BASE + 0x10)
#define SILAN_SYSCFG_REG2               (SILAN_SYSCFG_BASE + 0x08)
#define SILAN_SYSCFG_REG9                (SILAN_SYSCFG_BASE + 0x24)
#define SILAN_SYSCFG_REG11                (SILAN_SYSCFG_BASE + 0x2c)
#define SILAN_SYSCFG_REG12                (SILAN_SYSCFG_BASE + 0x30)

/********************************************************************
*                           audio clock                             *
********************************************************************/
#define SILAN_SYSCFG_REG5                 (SILAN_SYSCFG_BASE + 0x14)
#define SILAN_SYSCFG_REG6                 (SILAN_SYSCFG_BASE + 0x18)
#define SILAN_SYSCFG_REG7                 (SILAN_SYSCFG_BASE + 0x1C)
#define SILAN_SYSCFG_REG15                  (SILAN_SYSCFG_BASE + 0x3c)
#define SILAN_SYSCFG_REG16                  (SILAN_SYSCFG_BASE + 0x40)
#define SILAN_SYSCFG_REG17                (SILAN_SYSCFG_BASE + 0x44)
#define SILAN_SYSCFG_REG18                (SILAN_SYSCFG_BASE + 0x48)
#define SILAN_SYSCFG_REG19                (SILAN_SYSCFG_BASE + 0x4C)

#define SILAN_SYSCFG_REG8                 (SILAN_SYSCFG_BASE + 0x20)
#define SILAN_SYSCFG_REG13                (SILAN_SYSCFG_BASE + 0x34)

/*
GPIO PULL UP
*/
#define SILAN_SYSCFG_CTRPAD0            (SILAN_SYSCFG_BASE + 0x100)
#define SILAN_SYSCFG_CTRPAD1            (SILAN_SYSCFG_BASE + 0x104)
#define SILAN_SYSCFG_CTRPAD2            (SILAN_SYSCFG_BASE + 0x108)
#define SILAN_SYSCFG_CTRPAD3            (SILAN_SYSCFG_BASE + 0x10c)
#define SILAN_SYSCFG_CTRPAD4            (SILAN_SYSCFG_BASE + 0x110)
#define SILAN_SYSCFG_CTRPAD5            (SILAN_SYSCFG_BASE + 0x114)
#define SILAN_SYSCFG_CTRPAD6            (SILAN_SYSCFG_BASE + 0x118)
#define SILAN_SYSCFG_CTRPAD7            (SILAN_SYSCFG_BASE + 0x11c)
#define SILAN_SYSCFG_CTRPAD8            (SILAN_SYSCFG_BASE + 0x120)
#define SILAN_SYSCFG_CTRPAD9            (SILAN_SYSCFG_BASE + 0x124)
#define SILAN_SYSCFG_CTRPAD10           (SILAN_SYSCFG_BASE + 0x128)


#define SILAN_SYSCFG_SOC0               (SILAN_SYSCFG_BASE + 0x300)
#define SILAN_SYSCFG_SOC1               (SILAN_SYSCFG_BASE + 0x304)
#define SILAN_SYSCFG_SDRAMCTR           (SILAN_SYSCFG_BASE + 0x600)
#define SILAN_SYSCFG_PD                 (SILAN_SYSCFG_BASE + 0x600)

#define SILAN_SYSSTS_PD                 (SILAN_SYSCFG_BASE + 0xF10)

typedef union misc_clk_reg1 {
    /** raw data */
    uint32_t d32;
    /** bits def */
    struct {
        unsigned ispdif_clksel: 3;
#define ISPDIF_CCLK_APLL_DIV1    0
#define ISPDIF_CCLK_APLL_DIV2    1
#define ISPDIF_CCLK_APLL_DIV3    2
#define ISPDIF_CCLK_APLL_DIV4    3
#define ISPDIF_CCLK_CLKSYS_DIV1  4
#define ISPDIF_CCLK_CLKSYS_DIV2  5
#define ISPDIF_CCLK_CLKSYS_DIV3  6
#define ISPDIF_CCLK_CLKSYS_DIV4  7

        unsigned ispdif_clkon: 1;
        unsigned reserved0: 4;
        unsigned sd_clksel: 2;
#define SD_CCLK_CLKSYS_DIV1    0
#define SD_CCLK_CLKSYS_DIV2    1
#define SD_CCLK_PLLREF_DIV2    2
#define SD_CCLK_RCL_DIV2       3

        unsigned reserved1: 1;
        unsigned sd_clkon: 1;
        unsigned sdio_clksel: 2;
#define SDIO_CCLK_CLKSYS_DIV1    0
#define SDIO_CCLK_CLKSYS_DIV2    1
#define SDIO_CCLK_PLLREF_DIV2    2
#define SDIO_CCLK_RCL_DIV2       3

        unsigned reserved2: 1;
        unsigned sdio_clkon: 1;
        unsigned otp_clksel: 2;
#define OTP_CCLK_PLLREF_DIV2    0
#define OTP_CCLK_CLKSYS_DIV8    1
#define OTP_CCLK_CLKSYS_DIV16   2
#define OTP_CCLK_RCL_DIV2       3

        unsigned reserved3: 1;
        unsigned otp_clkon: 1;
        unsigned reserved4: 3;
        unsigned codec1_clkon: 1;
        unsigned codec2_clkon: 1;
        unsigned reserved5: 1;
        unsigned mcu_wdog_clkon: 1;
        unsigned risc_wdog_clkon: 1;
        unsigned reserved6: 4;
    } __attribute__((packed)) b;
} __attribute__((packed)) misc_clk_reg1_t;

typedef union misc_clk_reg2 {
    /** raw data */
    uint32_t d32;
    /** bits def */
    struct {
        unsigned pwm_clksel: 2;
#define PWM_CCLK_CLKSYS_DIV2    0
#define PWM_CCLK_CLKSYS_DIV4    1
#define PWM_CCLK_PLLREF_DIV2    2
#define PWM_CCLK_RCL_DIV2       3

        unsigned reserved0: 1;
        unsigned pwm_clkon: 1;
        unsigned adc_clksel: 2;
#define ADC_CCLK_CLKSYS_DIV8    0
#define ADC_CCLK_CLKSYS_DIV12   1
#define ADC_CCLK_PLLREF_DIV2    2
#define ADC_CCLK_RCL_DIV2       3

        unsigned reserved1: 1;
        unsigned adc_clkon: 1;
        unsigned reserved2: 12;
        unsigned timer_clksel: 2;
#define TIMER_CCLK_CLKSYS_DIV2    0
#define TIMER_CCLK_CLKSYS_DIV3    1
#define TIMER_CCLK_PLLREF_DIV2    2
#define TIMER_CCLK_RCL_DIV2       3

        unsigned reserved3: 1;
        unsigned timer_clkon: 1;
        unsigned rtc_clkon: 1;
        unsigned usbfs_clkon: 1;
        unsigned pdp_clksel1: 2;
#define PDP_CCLK_CLKSYS_DIV4    0
#define PDP_CCLK_CLKSYS_DIV8    1
#define PDP_CCLK_PLLREF_DIV12   2
#define PDP_CCLK_RCL_DIV2       3

        unsigned pdp_clksel2: 2;
#define PDP_CCLK_POST_DIV1    0
#define PDP_CCLK_POST_DIV2    1
#define PDP_CCLK_POST_DIV4    2
#define PDP_CCLK_POST_DIV8    3

        unsigned pdp_clkon: 1;
        unsigned reserved4: 1;
    } __attribute__((packed)) b;
} __attribute__((packed)) misc_clk_reg2_t;

typedef union SOC_clk_reg {
    /** raw data */
    uint32_t d32;
    /** bits def */
    struct {
        unsigned reserved0: 2;
        unsigned risc_cclkon: 1;
        unsigned dsp_cclkon: 1;
        unsigned mcu_cclkon: 1;
        unsigned bus_clksel: 1;
        unsigned dsp_cclksel: 1;
        unsigned sf_hclksel: 1;
        unsigned syspre_clksel: 2;
        unsigned mcu_cclksel: 2;
        unsigned reserved1: 2;
        unsigned dsp_hclkon: 1;
        unsigned reserved2: 5;
        unsigned lp_en: 1;
        unsigned reserved3: 2;
        unsigned lp_otp_clkon: 1;
        unsigned lp_shdw_clkon: 1;
        unsigned lp_bus_clkon: 1;
        unsigned lp_sf_clkon: 1;
        unsigned lp_sdram_clkon: 1;
        unsigned lp_sr_clkon: 1;
        unsigned reserved4: 3;
    } __attribute__((packed)) b;
} __attribute__((packed)) SOC_CLK_REG;

#endif

