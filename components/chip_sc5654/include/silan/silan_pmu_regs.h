#ifndef _SILAN_PMU_REGS_H
#define _SILAN_PMU_REGS_H
#include <stdint.h>

typedef volatile struct sysclk_struct {
    uint32_t  write_key;         // offset 0x000
    uint32_t  sys_ctr0;          // offset 0x004
    uint32_t  sys_ctr1;          // offset 0x008
    uint32_t  sys_ctr2;          // offset 0x00C
    uint32_t  sys_sts;           // offset 0x010
    uint32_t  sys_wdt_tg;           // offset 0x014
    uint32_t  sys_pll_ctrl1;     // offset 0x018
    uint32_t  sys_pll_ctrl2;        // offset 0x01c
    uint32_t  sscg;                            // offset 0x020
    uint32_t  audio_pll_ctrl1;    // offset 0x024
    uint32_t  audio_pll_ctrl2;    // offset 0x028
    uint32_t  iso_en;
    uint32_t  sys_wdt_clr;
} sysclk_struct_t;

#define sysclk_st  ((sysclk_struct_t *)0x42040000)

/******mode level*******/

//key
#define W_PROTECT_OPEN        0x05fa59a6
#define W_PROTECT_CLOSE        0xa05f0010


//sys_ctr0
#define    SYSCLK_EN                        (1<<31)
#define SYSCLK_CS(x)                        ((x&0x03)<<29)
#define RCL_EN                            (1<<28)
#define RCH_EN                            (1<<27)
#define OSC_EN                            (1<<26)
#define SYS_PLL_EN                        (1<<25)
#define _32KPLL_EN                        (1<<24)
#define AUDIO_PLL_EN                        (1<<23)
#define REF_CLK_CS                        (1<<22)
#define USB20_CLK_CS(x)                        ((x&0x3)<<20)
#define USB20_SYSPLL_DIV(x)                    ((x&0x07)<<17)
#define TCK_CS(x)                        ((x&0x07)<<14)
#define U11BCK_DIV                        (1<<13)
#define    OSC12M_CC                        ((x&0x07)<<10)
#define OSC12M_RSEL                        (1<<9)
#define    UPLL32K_EN                        (1<<8)
#define    UPLL32K_SEL(x)                        ((x&0x07)<<5)
#define    U11_BCK_EN                        (1<<4)
#define    U11_BCK_PD                        (1<<3)
#define    U11_BCK_REFSEL                        (1<<2)
#define    U11_BCK_RSTN                        (1<<1)
#define    U11_USB11_RSTN                        (1<<0)


//sys_ctr1

#define CLKMUX_ERR_IRQ_EN            (1<<31)
#define    CLKMUX_RESETORE_EN            (1<<30)
#define    WDT_WAKE_EN                    (1<<29)
#define    IO_WAKE_EN                    (1<<28)
#define    LVD_WAKE_EN                    (1<<27)
#define    KEY_DET_EN                    (1<<26)
#define    KEY_DET_MASK_EN                (1<<25)
#define    LVR_IRQ_EN                    (1<<24)
#define    LVD_IRQ_EN                    (1<<23)
#define    POC_IRQ_EN                    (1<<22)
#define OSC_IRQ_EN                    (1<<21)
#define    SPLL_IRQ_EN                    (1<<20)
#define    RCH_IRQ_EN                    (1<<19)
#define    _32KPLL_IRQ_EN                (1<<18)
#define    APLL_IRQ_EN                    (1<<17)
#define    RCL_D(x)                    ((x&0x7f)<<10)
#define    RCH_FREQS(x)                ((x&0x03)<<8)
#define    RCH_TRIM(x)                    ((x&(0xff))<<0)


//sys_ctr2
#define    SF_PWR                           (1<<31)
#define    OSC_PLL_SEL                    (1<<30)
#define    SYS_WDT_IRQ_EN                (1<<29)
#define    SYS_WDT_RST_EN                (1<<28)
#define    RTC_WAKE_EN                    (1<<27)
#define    SAR_VREF_SEL                (1<<26)
#define    UPLL_S_CMP                    (1<<25)
#define    LVRLS(x)                    ((x&0x3)<<23)
#define    LVREN                        (1<<22)
#define    LVDLS(x)                    ((x&0x7)<<19)
#define    LVDEN                        (1<<18)
#define    BUFSEL                        (1<<17)
#define    BUFEN                        (1<<16)
#define    LDO_DSP_EN                    (1<<10)
#define    VSEL(x)                        ((x&0x3)<<8)
#define    PMU_STOP                    (1<<7)
#define    LVR_DSEL(x)                    ((x&0x3)<<5)
#define    LVR_MASK                    (1<<4)
#define    POR_MASK                    (1<<3)
#define    POR_TSTN_SEL                (1<<2)
#define    POR_DSEL(x)                    ((x&0x3)<<0)

//sys_sts
#define XRSN_STS                    (1<<31)
#define    POC_STS                        (1<<30)
#define    POR_STS                        (1<<29)
#define LVR_STS                        (1<<28)
#define    LVD_STS                        (1<<27)
#define    RCH_STB_ASYN                (1<<26)
#define    OSC_CLKDET                    (1<<25)
#define    SPLL_LOCK                    (1<<24)
#define    RCH_STS_REG                    (1<<23)
#define    OSC_STS_REG                    (1<<22)
#define    SPLL_STS_REG                (1<<21)
#define    CLK_MUX_ERR                    (1<<20)
#define    CLK_MUX_LOCK                (1<<19)
#define    ACCESS_EN                    (1<<18)
#define    _32KPLL_STS_REG                (1<<17)
#define    APLL_STS_REG                (1<<16)
#define    M31_32KPLL_STS_REG            (1<<15)
#define    KEY_STS_REG                    (1<<14)
#define    _32KPLL_LOCK                (1<<13)
#define    APLL_LOCK                    (1<<12)
#define    M31_32KPLL_LOCK                (1<<11)
#define    PMU_LVR                        (1<<10)
#define    PMU_LVD                        (1<<9)
#define    SPI_CRCERR_RST                (1<<8)

//sys_pll_ctrl1
#define USB20_REFCLK_DIV             ((x&0xff)<<24)
#define    SPLL_BYPASS                    (1<<23)
#define    SPLL_DACPD                    (1<<22)
#define SPLL_DSMPD                    (1<<21)
#define    SPLL_FOUTPOSTDIVPD            (1<<20)
#define    SPLL_FOUTVCOPD                (1<<19)
#define    SPLL_FOUT4PHASEPD            (1<<18)
#define    SPLL_POST_DIV2(x)            ((x&0x7)<<15)
#define    SPLL_POST_DIV1(x)            ((x&0x7)<<12)
#define    SPLL_FBDIV(x)                ((x&0xfff)<<0)

//sys_pll_ctrl2
#define    SPLL_CLK_EN                    (1<<30)
#define    SPLL_REFDIV(x)                ((x&0x3f)<<24)
#define    SPLL_FRAC                    ((x&0xffffff)<<0)

//sscg
#define    DISABLE_SSCG                (1<<11)
#define    SEL_EXITWAVE                (1<<10)
#define    DOWNSPREAD                    (1<<9)
#define    SPREAD(x)                    ((x&0x1f)<<4)
#define    DIVVAL(x)                    ((x&0xf)<<0)

//audio_pll_ctrl1
#define AUDIO_MCLK_S                (1<<26)
#define    APLL_CLK_EN                    (1<<24)
#define APLL_BYPASS                    (1<<23)
#define    APLL_DACPD                    (1<<22)
#define APLL_DSMPD                    (1<<21)
#define    APLL_FOUTPOSTDIVPD            (1<<20)
#define    APLL_FOUTVCOPD                (1<<19)
#define    APLL_FOUT4PHASEPD            (1<<18)
#define    APLL_POST_DIV2(x)            ((x&0x7)<<15)
#define    APLL_POST_DIV1(x)            ((x&0x7)<<12)
#define    APLL_FBDIV(x)                ((x&0xfff)<<0)

//audio_pll_ctrl2
#define    APLL_REFDIV(x)                ((x&0x3f)<<24)
#define    APLL_FRAC(x)                ((x&0xffffff)<<0)

#define    PMU_IRQ_KEYDET                (KEY_DET_MASK_EN)
#define    PMU_IRQ_POC                    (POC_IRQ_EN)
#define    PMU_IRQ_APLL                (APLL_IRQ_EN)
#define    PMU_IRQ_32KPLL                (_32KPLL_IRQ_EN)
#define    PMU_IRQ_SPLL                (SPLL_IRQ_EN)
#define    PMU_IRQ_OSC                    (OSC_IRQ_EN)
#define    PMU_IRQ_RCH                    (RCH_IRQ_EN)
#define    PMU_IRQ_CLKMUX                (CLKMUX_ERR_IRQ_EN)
#define    PMU_IRQ_LVR                    (LVR_IRQ_EN)
#define    PMU_IRQ_LVD                    (LVD_IRQ_EN)
#define    PMU_IRQ_WDT                    (SYS_WDT_IRQ_EN)

#define    PMU_WAKEUP_KEY                (KEY_DET_EN)
#define    PMU_WAKEUP_IO                (IO_WAKE_EN)
#define    PMU_WAKEUP_LVD                (LVD_WAKE_EN)
#define    PMU_WAKEUP_WDT                (WDT_WAKE_EN)
#define    PMU_WAKEUP_RTC                (RTC_WAKE_EN)

#endif

