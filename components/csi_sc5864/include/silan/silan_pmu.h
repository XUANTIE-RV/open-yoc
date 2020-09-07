#ifndef _SILAN_PMU_H
#define _SILAN_PMU_H

#include "silan_pmu_regs.h"
#include "silan_syscfg.h"

typedef enum {
    SYSCLK_FROM_RCH  = 0,
    SYSCLK_FROM_OSC,
    SYSCLK_FROM_32KPLL,
    SYSCLK_FROM_PLL,
    SYSCLK_FROM_RCL,
    SYSCLK_FROM_BCK,
} silan_sysclk_ref_t;


typedef enum {
    _32KPLL_4M  = 0,
    _32KPLL_2M,
    _32KPLL_1M,
    _32KPLL_500K,
    _32KPLL_8M,
    _32KPLL_12M,
    _32KPLL_16M,
    _32KPLL_32K,

    //SYSPLL?
} silan_sysclk_ifsubclk_t;

// usb20
typedef enum {
    USB20_REFCLKSEL_12MOSC  = 0,
    USB20_REFCLKSEL_M32KPLL ,
    USB20_REFCLKSEL_SYSPLL_DIV ,
} silan_usb20_ref_t;

typedef enum {
    USB20_SYSPLL_DIV2  = 0,
    USB20_SYSPLL_DIV4      ,
    USB20_SYSPLL_DIV6      ,
    USB20_SYSPLL_DIV8      ,
    USB20_SYSPLL_DIV10  ,
    USB20_SYSPLL_DIV12  ,
    USB20_SYSPLL_DIV14  ,
    USB20_SYSPLL_DIV16  ,
} silan_usb20_under_syspll_div_t;

typedef enum {
    AUDIOPLL_45M  = 0,
    AUDIOPLL_49M,
} silan_audiopllsel_t;

typedef enum {
    SYSPLL_12M = 0,
    SYSPLL_30M ,
    SYSPLL_60M ,
    SYSPLL_80M ,
    SYSPLL_100M ,
    SYSPLL_120M ,
    SYSPLL_140M ,
    SYSPLL_160M ,
    SYSPLL_180M ,
    SYSPLL_200M ,
} silan_syspllsel_t;

#define SYSPLL_SSCG_ON    1
#define SYSPLL_SSCG_OFF   0

#define LDO_ON  1
#define LDO_OFF 0
typedef enum {
    LDO_OUT_12V, //1.2V
    LDO_OUT_10V,
    LDO_OUT_08V,
    LDO_OUT_14V,
} silan_ldo_level_sel_t;

#define DEFAULT_VREF 0
typedef enum {
    LVDLS_20  = 0,
    LVDLS_21,
    LVDLS_22,
    LVDLS_23,
    LVDLS_24,
    LVDLS_25,
    LVDLS_27,
    LVDLS_29,
} silan_lvd_level_t;

typedef enum {
    LVRLS_19  = 0,
    LVRLS_21,
    LVRLS_23,
    LVRLS_25,
} silan_lvr_level_t;

typedef struct {
    uint32_t  sysclk;       //
    uint32_t  sysclk_ref;     // res
} sysclk_cfg_t;

extern sysclk_cfg_t sysclk_init;

typedef enum PLL_mode
{
        PLL_MODE_FRAC   = 0,
        PLL_MODE_INT,
}
PLL_MODE, *PPLL_MODE;

typedef enum PLL_refclk
{
        PLL_REFCLK_OSC  = 0,
        PLL_REFCLK_BCK,
        PLL_REFCLK_PLL32K
}
PLL_REFCLK, *PPLL_REFCLK;

typedef enum SYSCLK_sel
{
        SYSCLK_SEL_RCH  = 0,
        SYSCLK_SEL_REFCLK,
        SYSCLK_SEL_SPLL,
        SYSCLK_SEL_RCL
}
SYSCLK_SEL, *PSYSCLK_SEL;

typedef struct PLL_cfg
{
    PLL_MODE        mode;
    uint8_t         sscg_en;
    uint8_t         downspread;
    uint8_t         spread;
}
PLL_CFG, *PPLL_CFG;

typedef enum {
    TCK_CLKSEL_RCL = 0,
    TCK_CLKSEL_RCH,
    TCK_CLKSEL_OSC32K,
    TCK_CLKSEL_OSC12M,
    TCK_CLKSEL_32KPLL,
    TCK_CLKSEL_UBCK,
    TCK_CLKSEL_APLLDIV4,
    TCK_CLKSEL_SPLLDIV16,
} silan_tck_clksel_t;

typedef union PMU_syspll_ctrl1_reg
{
    uint32_t d32;

    struct
    {
        uint32_t fbdiv : 12;
        uint32_t post_div1 : 3;
        uint32_t post_div2 : 3;
        uint32_t reserved18_19 : 2;
        uint32_t foutpostdiv_pd : 1;
        uint32_t dsm_pd : 1;
        uint32_t dac_pd : 1;
        uint32_t pll_bypass : 1;
        uint32_t usb20_refclk_div : 8;
    }
    b;
} PMU_SYSPLL_CTRL1_REG ;

typedef union PMU_syspll_ctrl2_reg
{
    uint32_t d32;

    struct
    {
        uint32_t frac : 24;
        uint32_t refdiv : 6;
        uint32_t spll_clken : 1;
        uint32_t reserved31 : 1;
    }
    b;
} PMU_SYSPLL_CTRL2_REG ;

typedef union PMU_sscg_ctrl_reg
{
    uint32_t d32;

    struct
    {
        uint32_t divval : 4;
        uint32_t spread : 5;
        uint32_t downspread : 1;
        uint32_t reserved10 : 1;
        uint32_t disable_sscg : 1;
        uint32_t reset_sscg : 1;
        uint32_t reserved13_31 : 19;
    }
    b;
} PMU_SSCG_CTRL_REG ;

typedef union PMU_sys_ctrl0_reg
{
    uint32_t d32;

    struct
    {
        uint32_t u11_phy_ponrst : 1;
        uint32_t u11_bck_rstn : 1;
        uint32_t u11_bck_refsel : 1;
        uint32_t u11_bck_pd : 1;
        uint32_t u11_bck_en : 1;
        uint32_t pll32k_sel : 3;
        uint32_t pll32k_clken : 1;
        uint32_t osc12m_rsel : 1;
        uint32_t osc12m_cc : 3;
        uint32_t u11_bck_div : 1;
        uint32_t tck_cs : 3;
        uint32_t usb20_syspll_div : 3;
        uint32_t usb20_refclk_sel : 2;
        uint32_t pll_refclk_sel : 1;
        uint32_t apll_en : 1;
        uint32_t pll32k_en : 1;
        uint32_t syspll_en : 1;
        uint32_t osc12m_en : 1;
        uint32_t rch_en : 1;
        uint32_t rcl_en : 1;
        uint32_t sysclk_cs : 2;
        uint32_t hsclk_en : 1;
    }
    b;
}PMU_SYS_CTRL0_REG ;

typedef union PMU_sys_ctrl1_reg
{
    uint32_t d32;

    struct
    {
        uint32_t rch_trim : 8;
        uint32_t rch_freq : 2;
        uint32_t rcl_trim : 7;
        uint32_t apll_irq_en : 1;
        uint32_t pll32k_irq_en : 1;
        uint32_t rch_irq_en : 1;
        uint32_t spll_irq_en : 1;
        uint32_t osc_irq_en : 1;
        uint32_t poc_irq_en : 1;
        uint32_t lvd_irq_en : 1;
        uint32_t lvr_irq_en : 1;
        uint32_t key_det_irq_en : 1;
        uint32_t key_det_en : 1;
        uint32_t lvd_wake_en : 1;
        uint32_t io_wake_en : 1;
        uint32_t wdt_wake_en : 1;
        uint32_t clkmux_restore_en : 1;
        uint32_t clkmux_err_en : 1;
    }
    b;
}PMU_SYS_CTRL1_REG ;

typedef union PMU_sys_ctrl2_reg
{
    uint32_t d32;

    struct
    {
        uint32_t por_dsel : 2;
        uint32_t por_rstn_sel : 1;
        uint32_t por_mask : 1;
        uint32_t lvr_mask : 1;
        uint32_t lvr_dsel : 2;
        uint32_t pmu_stop : 1;
        uint32_t dsp_ldo_vsel : 2;
        uint32_t dsp_ldo_en : 1;
        uint32_t reserved11_15 : 5;
        uint32_t buf_en : 1;
        uint32_t buf_sel : 1;
        uint32_t lvd_en : 1;
        uint32_t lvd_ls : 3;
        uint32_t lvr_en : 1;
        uint32_t lvr_ls : 2;
        uint32_t pll32k_cmp : 1;
        uint32_t sar_vref_sel : 1;
        uint32_t rtc_wake_en : 1;
        uint32_t wdt_reset_en : 1;
        uint32_t wdt_irq_en : 1;
        uint32_t pll_refclk_sel : 1;
        uint32_t sf_pwr : 1;
    }
    b;
}PMU_SYS_CTRL2_REG ;

typedef union PMU_sys_sts_reg
{
    uint32_t d32;

    struct
    {
        uint32_t reserved0_5 : 6;
        uint32_t u20_pll_lock : 1;
        uint32_t u11_bck_rdy : 1;
        uint32_t sf_crcerr_rst : 1;
        uint32_t lvd : 1;
        uint32_t lvr : 1;
        uint32_t u11_bck_lock : 1;
        uint32_t apll_lock : 1;
        uint32_t pll32k_lock : 1;
        uint32_t key_sts : 1;
        uint32_t u11_bck_sts : 1;
        uint32_t apll_sts : 1;
        uint32_t pll32k_sts : 1;
        uint32_t acess : 1;
        uint32_t clkmux_lock : 1;
        uint32_t clkmux_err : 1;
        uint32_t spll_sts : 1;
        uint32_t osc_sts : 1;
        uint32_t rch_sts : 1;
        uint32_t spll_lock : 1;
        uint32_t osc_clkdet : 1;
        uint32_t rch_stb_asyn : 1;
        uint32_t lvd_sts : 1;
        uint32_t lvr_sts : 1;
        uint32_t por_sts : 1;
        uint32_t poc_sts : 1;
        uint32_t xrst_sts : 1;
    }
    b;
}PMU_SYS_STS_REG ;



// Static Inline Func
static inline void sysclk_regs_protect_open(void)
{
    sysclk_st->write_key = W_PROTECT_OPEN;
}

static inline void sysclk_regs_protect_close(void)
{
    sysclk_st->write_key = W_PROTECT_CLOSE;
}

int get_sysclk_val_settled(void);
void syspll_config(silan_syspllsel_t type);
void sysclk_init_op(void);
int pre_sysclk_sel(silan_sysclk_ref_t sysclk_ref, silan_syspllsel_t if_clk_sel);

void silan_spiflash_power_enable(void);
void silan_pmu_wakeup_enable(uint32_t id);
void silan_pmu_wakeup_disable(uint32_t id);
void silan_sar_adc_vref_select(int inner);

void silan_dsp_ldo_disable(void);
void silan_dsp_ldo_config(int level);
void silan_pmu_wdt_init(uint32_t wdt_cnt);
void silan_pmu_wdt_reset_config(uint32_t wdt_cnt);
void silan_pmu_wdt_reset_disable(void);
void silan_pmu_wdt_clr(void);
void silan_pwr_iso_disable(void);
void silan_pwr_iso_enable(void);

void silan_pmu_wakeup_disable(uint32_t id);
void silan_sar_adc_vref_select(int inner);

void silan_soc_stop(void);
void silan_sdram_pwd_deal(void);
void silan_sdram_pwd_quit(void);
void silan_usb_pwd_deal(void);
void silan_usb_pwd_quit(void);

void silan_audiopll_init(void);
void silan_syspll_init(uint32_t sscg);
void silan_sysclk_change(silan_sysclk_ref_t sysclk_ref, silan_syspllsel_t if_clk_sel);
void silan_set_sys_freq(silan_syspllsel_t if_clk_sel);
void audiopll_config(silan_audiopllsel_t type);
void silan_module_clkon(int mod, silan_clk_onff_t onoff);

void silan_pmu_wakeup_disable(uint32_t id);
void silan_sar_adc_vref_select(int inner);

void silan_ubck_enable(void);
uint32_t silan_get_audiopll_clk(void);
uint32_t silan_pmu_get_sysclk(void);

uint32_t silan_clock_change_pll_to_osc(void);
void silan_clock_change_osc_to_pll(uint32_t val);

void silan_codec1_pwd_deal_save(uint32_t *regs);
void silan_codec1_pwd_deal_restore(uint32_t *regs);
void silan_codec2_pwd_deal_save(uint32_t *regs);
void silan_codec2_pwd_deal_restore(uint32_t *regs);

#endif
