/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     yun_pmu.c
 * @brief    source file for pmu.
 * @version  V1.0
 * @date     10. Aug 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <drv/pmu.h>
#include <drv/tee.h>
#include <csi_core.h>
#include <soc.h>
#include <pmu_regs.h>
#include <yun_pmu.h>
#include <io.h>

#define ERR_PMU(errno) (CSI_DRV_ERRNO_PMU_BASE | errno)
#define PMU_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_PMU(DRV_ERROR_PARAMETER))
#define ATTRIBUTE_DATA __attribute__((section(".tcm1")))

#define RUN_ADDR    0x18000000
#define STORAGE_ADDR    0x80a0000
typedef struct {
    uint8_t idx;
    pmu_event_cb_t cb;
} ck_pmu_priv_t;

typedef enum {
    WAIT_MODE = 0,
    DOZE_MODE,
    STOP_MODE,
    STANDBY_MODE,
    SLEEP_MODE
} lpm_mode_e;

extern int32_t arch_do_cpu_resume(void);
extern int32_t arch_resume_context(void);

static uint32_t s_irq_state;
static ck_pmu_priv_t pmu_handle[CONFIG_PMU_NUM];

#define CONFIG_CPU_REGISTER_NUM_SAVE    28
uint32_t g_arch_cpu_saved[CONFIG_CPU_REGISTER_NUM_SAVE];

void yun_set_pmu_clk_src(clk_src_e src)
{
    if (src == ILS_CLK) {
        YUN_PMU->PMU_CLKSRCSEL |= PMU_CLKSRCSEL_EILS_SEL_ILS;
    } else if (src == ELS_CLK) {
        YUN_PMU->PMU_CLKSRCSEL &= ~PMU_CLKSRCSEL_EILS_SEL_ILS;
    }
}

void drv_set_aon_clk_src(clk_src_e src)
{
    if (src == ELS_CLK) {
        YUN_PMU->PMU_CLKSRCSEL &= ~PMU_CLKSRCSEL_EILS_SEL_Msk;
    } else if (src == ILS_CLK) {
        YUN_PMU->PMU_CLKSRCSEL |= PMU_CLKSRCSEL_EILS_SEL_Msk;
    }
}

void drv_reset_cpu1(void)
{
    YUN_PMU->PMU_CPU12SWRST |= PMU_CPU12SWRST_CPU1_RST_EN;
    YUN_PMU->PMU_CPU12SWRST &= ~PMU_CPU12SWRST_CPU1_RST_EN;
}

void drv_reset_cpu2(void)
{
    YUN_PMU->PMU_CPU12SWRST |= PMU_CPU12SWRST_CPU2_RST_EN;
    YUN_PMU->PMU_CPU12SWRST &= ~PMU_CPU12SWRST_CPU2_RST_EN;
}

void yun_enable_lpm_mode(void)
{
    YUN_PMU->PMU_LPCR |= PMU_LPCR_LPEN;
}

void yun_enter_hw_vad0(void)
{
    YUN_PMU->PMU_LPCR &= ~PMU_LPCR_CHANGE_VOLT_ENTER_HW_VAD1;
}

void yun_enter_hw_vad1(void)
{
    YUN_PMU->PMU_LPCR |= PMU_LPCR_CHANGE_VOLT_ENTER_HW_VAD1;
}

uint32_t yun_is_boot_from_lpm(void)
{
    return ((YUN_PMU->PMU_LPCR & PMU_LPCR_BOOT_FROM_LP) == PMU_LPCR_BOOT_FROM_LP);
}

void yun_set_wakeup_source(uint8_t wakeupn)
{
    YUN_PMU->PMU_WKUPMASK |= (1 << wakeupn);
}

void yun_clear_wakeup_source(uint8_t wakeupn)
{
    YUN_PMU->PMU_WKUPMASK &= ~(1 << wakeupn);
}

yun_reset_status_e yun_get_reset_status(void)
{
    return 32 - __FF1(YUN_PMU->PMU_RSTSTA);
}

void yun_enable_ils_clk(void)
{
    YUN_PMU->PMU_RCCTRL |= PMU_RCCTRL_ILS_CLK_EN;
}

void yun_disable_ils_clk(void)
{
    YUN_PMU->PMU_RCCTRL &= ~PMU_RCCTRL_ILS_CLK_EN;
}

void yun_enable_els_clk(void)
{
    YUN_PMU->PMU_RCCTRL |= PMU_RCCTRL_ELS_CLK_EN;
}

void yun_disable_els_clk(void)
{
    YUN_PMU->PMU_RCCTRL &= ~PMU_RCCTRL_ELS_CLK_EN;
}

void yun_set_cpu0_clk_src(clk_src_e src)
{
    if (src == PLL_CLK) {
        YUN_CPR0->CPR0_CPU0CLK_SEL &= ~CPR0_CPU0CLK_SEL_CPU0CLK_SEL_EHS;
    } else if (src == EHS_CLK) {
        YUN_CPR0->CPR0_CPU0CLK_SEL |= CPR0_CPU0CLK_SEL_CPU0CLK_SEL_EHS;
    }
}

void yun_set_cpu2_clk_src(clk_src_e src)
{
    if (src == PLL_CLK) {
        YUN_CPR1->CPR1_CPU2CLK_SEL &= ~CPR1_CPU2CLK_SEL_CPU2CLK_SEL_EHS;
    } else if (src == EHS_CLK) {
        YUN_CPR1->CPR1_CPU2CLK_SEL |= CPR1_CPU2CLK_SEL_CPU2CLK_SEL_EHS;
    }
}

void drv_clk_enable(clk_module_e module)
{
    uint32_t val;
    uint8_t bit_mask = 0x1U;

    if (module == AONTIM_CLK || module == RTC_CLK) {
        bit_mask = 0x11U;
    }

    if (module == TIM1_CLK) {
        *(volatile uint32_t *)0x30000010 &= ~(0x01 << 19);
    }

    val = getreg32((volatile uint32_t *)module);
    val |= bit_mask;
    putreg32(val, (volatile uint32_t *)module);
}

void drv_clk_disable(clk_module_e module)
{
    uint32_t val;
    uint8_t bit_mask = 0x1U;

    if (module == AONTIM_CLK || module == RTC_CLK) {
        bit_mask = 0x11U;
    }

    val = getreg32((volatile uint32_t *)module);
    val &= ~bit_mask;
    putreg32(val, (volatile uint32_t *)module);
}

void resume_context_from_stop_mode(void)
{
    /* config SDRAM on clk 144M */
    *(volatile uint32_t *)0x31000014 = 0x1;
    *(volatile uint32_t *)0x1a000054 = 0x8;
    *(volatile uint32_t *)0x1a000000 = 0x1c0f68;
    *(volatile uint32_t *)0x1a000004 = 0x029e949f;
    *(volatile uint32_t *)0x1a00000c = 0x3009;
    *(volatile uint32_t *)0x30000210 = 0x1f;
    *(volatile uint32_t *)0x3b800004 = 0x100;
}

#ifndef CONFIG_TEE_CA
static void set_resume_func(pmu_handle_t handle, uint32_t *func)
{
    /* enable the lp bootloadr secure */
    *(volatile uint32_t *)(CSKY_TIPC_BASE + 4) = 0x100;
    YUN_PMU->PMU_LPBOOTADDR_804_0 = (uint32_t)func - (uint32_t)RUN_ADDR + (uint32_t)STORAGE_ADDR;
}
#endif

void soc_sleep(pmu_handle_t handle, lpm_mode_e mode)
{
#ifdef CONFIG_TEE_CA
    tee_lpm_mode_e lpm_mode = 0;

    if (mode == WAIT_MODE) {
        lpm_mode = TEE_LPM_MODE_WAIT;
    } else if (mode == DOZE_MODE) {
        lpm_mode = TEE_LPM_MODE_DOZE;
    } else if (mode == STOP_MODE) {
        lpm_mode = TEE_LPM_MODE_STOP;
    } else if (mode == STANDBY_MODE) {
        lpm_mode = TEE_LPM_MODE_STANDBY;
    } else {
        lpm_mode = TEE_LPM_MODE_WAIT;
    }

    csi_tee_enter_lpm((uint32_t)g_arch_cpu_saved, 0, lpm_mode);

    if (mode == STOP_MODE) {
        resume_context_from_stop_mode();
    }

#else

#ifdef CONFIG_ARCH_CSKY
    YUN_PMU->DLC_IFR = 0x1f; /* clear pmu pend */

    while (YUN_PMU->DLC_IFR);   /* sync */

    VIC->ICPR[0] = 0xFFFFFFFF; /* clear pend IRQ */
#endif

    if (mode == WAIT_MODE) {
        yun_enter_hw_vad0();
        yun_enable_lpm_mode();
        __WFI();
    } else if (mode == DOZE_MODE) {
        yun_enter_hw_vad1();
        yun_enable_lpm_mode();
        __WFI();
    } else if (mode == STANDBY_MODE) {
        set_resume_func(handle, (uint32_t *)&arch_resume_context);
        yun_enable_lpm_mode();
        __STOP();
    } else {
        __WFI();
    }

#endif
}

int32_t drv_get_boot_type(void)
{
    return yun_is_boot_from_lpm();
}

/**
  \brief       Initialize PMU Interface. 1. Initializes the resources needed for the PMU interface 2.registers event callback function
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref pmu_event_cb_t
  \return      pointer to pmu handle
*/
pmu_handle_t csi_pmu_initialize(int32_t idx, pmu_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_PMU_NUM) {
        return NULL;
    }

    ck_pmu_priv_t *pmu_priv = &pmu_handle[idx];

    /* initialize the pmu context */
    pmu_priv->idx = idx;
    pmu_priv->cb = cb_event;

    return (pmu_handle_t)pmu_priv;
}

/**
  \brief       De-initialize PMU Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  pmu handle to operate.
  \return      error code
*/
int32_t csi_pmu_uninitialize(pmu_handle_t handle)
{
    PMU_NULL_PARAM_CHK(handle);

    ck_pmu_priv_t *pmu_priv = handle;
    pmu_priv->cb = NULL;

    return 0;
}

int32_t csi_pmu_power_control(pmu_handle_t handle, csi_power_stat_e state)
{
    return ERR_PMU(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       choose the pmu mode to enter
  \param[in]   handle  pmu handle to operate.
  \param[in]   mode    \ref pmu_mode_e
  \return      error code
*/
int32_t csi_pmu_enter_sleep(pmu_handle_t handle, pmu_mode_e mode)
{
    PMU_NULL_PARAM_CHK(handle);

    ck_pmu_priv_t *pmu_priv = handle;

#ifdef CONFIG_ARCH_CSKY
    VIC->ICPR[0] = 0xFFFFFFFF; /* clear pend IRQ */
#endif

    s_irq_state = csi_irq_save();

    switch (mode) {
        case PMU_MODE_RUN:
            break;

        case PMU_MODE_SLEEP:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            soc_sleep(handle, WAIT_MODE);

            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_SLEEP_DONE, mode);
            }

            break;

        case PMU_MODE_DOZE:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            soc_sleep(handle, DOZE_MODE);

            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_SLEEP_DONE, mode);
            }

            break;

        case PMU_MODE_STANDBY:
            if (pmu_priv->cb) {
                pmu_priv->cb(pmu_priv->idx, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            soc_sleep(handle, STANDBY_MODE);

            break;

        case PMU_MODE_DORMANT:
        case PMU_MODE_SHUTDOWN:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);
            break;

        default:
            return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    csi_irq_restore(s_irq_state);

    return 0;
}

/**
  \brief       Config the wakeup source.
  \param[in]   handle  pmu handle to operate
  \param[in]   type    \ref pmu_wakeup_type
  \param[in]   pol     \ref pmu_wakeup_pol
  \param[in]   enable  flag control the wakeup source is enable or not
  \return      error code
*/
int32_t csi_pmu_config_wakeup_source(pmu_handle_t handle, uint32_t wakeup_num, pmu_wakeup_type_e type, pmu_wakeup_pol_e pol, uint8_t enable)
{
    PMU_NULL_PARAM_CHK(handle);

    if (type != PMU_WAKEUP_TYPE_LEVEL) {
        return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    if ((pol > PMU_WAKEUP_POL_HIGH)) {
        return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    if (enable) {
        csi_vic_set_wakeup_irq(PMU_IRQn);
        yun_set_wakeup_source(wakeup_num);
    } else {
        csi_vic_clear_wakeup_irq(PMU_IRQn);
        yun_clear_wakeup_source(wakeup_num);
    }

    return 0;
}

void yun_pll_power_down(int en)
{
    pmu_reg_t *reg = YUN_PMU;

    if(en) {
        reg->PMU_PLLCTRL |= (1 << 18) | (1 << 11);
    } else {
        reg->PMU_PLLCTRL &= ~((1 << 18) | (1 << 11));
    }
}

static void pll_set_doubling(uint16_t val)
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL;
    temp &= ~(0xfff << 20);
    temp |= (val & 0xfff) << 20;

    reg->PMU_PLLCTRL = temp;
}

static void pll_set_prescale(uint8_t val)
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL;
    temp &= ~(0x3f << 12);
    temp |= (val & 0x3f) << 12;

    reg->PMU_PLLCTRL = temp;
}

static void ATTRIBUTE_DATA pll_set_div1(uint8_t val)
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL;
    temp &= ~(0x7 << 4);
    temp |= (val & 0x7) << 4;

    reg->PMU_PLLCTRL = temp;
}

static void pll_set_div2(uint8_t val)
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL;
    temp &= ~(0x7 << 8);
    temp |= (val & 0x7) << 8;

    reg->PMU_PLLCTRL = temp;
}

static void pll_bypass(int en)
{
    pmu_reg_t *reg = YUN_PMU;

    if (en) {
        reg->PMU_PLLCTRL |= 1 << 2;
    } else {
        reg->PMU_PLLCTRL &= ~(1 << 2);
    }
}

static void pll_change_sw_trigger()
{
    pmu_reg_t *reg = YUN_PMU;
    reg->PMU_LPCR |= 0x4;
}

void pll_frac_bypass(int en)
{
    pmu_reg_t *reg = YUN_PMU;

    if (en) {
        reg->PMU_PLLCTRL_FRAC |= 1 << 2;
    } else {
        reg->PMU_PLLCTRL_FRAC &= ~(1 << 2);
    }
}

static void pll_frac_set_prescale(uint8_t val)
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL_FRAC;
    temp &= ~(0x3f << 12);
    temp |= (val & 0x3f) << 12;

    reg->PMU_PLLCTRL_FRAC = temp;
}

static void pll_frac_set_doubling(uint16_t val)
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL_FRAC;

    temp &= ~(0xfff << 20);
    temp |= (val & 0xfff) << 20;

    reg->PMU_PLLCTRL_FRAC = temp;
}

static void pll_frac_set_div1(uint8_t val)
{
    pmu_reg_t *reg = YUN_PMU;

    uint32_t temp = reg->PMU_PLLCTRL_FRAC;
    temp &= ~(0x7 << 4);
    temp |= (val & 0x7) << 4;

    reg->PMU_PLLCTRL = temp;
}

static void pll_frac_set_div2(uint8_t val)
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL_FRAC;
    temp &= ~(0x7 << 8);
    temp |= (val & 0x7) << 8;

    reg->PMU_PLLCTRL = temp;
}

static void pll_set_frac(uint32_t val)
{
    pmu_reg_t *reg = YUN_PMU;

    reg->PMU_FRACPLLFRAC = val & 0xffffff;
}

static uint32_t get_pll_frac_prescal()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL_FRAC;
    temp &= 0x3f << 12;
    temp >>= 12;

    return temp;
}

static uint32_t get_pll_frac_freq_doubling()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL_FRAC;

    temp &= 0xfff << 20;
    temp >>= 20;

    return temp;
}

static uint32_t get_pll_frac_div1()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL_FRAC;
    temp &= 0x7 << 4;
    temp >>= 4;

    return temp;
}

static uint32_t get_pll_frac_div2()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL_FRAC;
    temp &= 0x7 << 8;
    temp >>= 8;

    return temp;
}

static uint32_t get_pll_frca()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_FRACPLLFRAC;
    temp &= 0xffffff;

    return temp;
}

#define PLL_FRAC_CONSTANT 0X1000000

static uint32_t get_pll_frac_freq()
{
    pmu_reg_t *reg = YUN_PMU;
    if (reg->PMU_PLLCTRL_FRAC & (1 << 2)) {
        return EHS_VALUE;
    }

    uint32_t freq = 0;
    uint32_t prescal = get_pll_frac_prescal();
    uint32_t doubling = get_pll_frac_freq_doubling();
    uint32_t div1 = get_pll_frac_div1();
    uint32_t div2 = get_pll_frac_div2();

    double frca = EHS_VALUE * (double)get_pll_frca();
    frca /= PLL_FRAC_CONSTANT;
    freq = (EHS_VALUE * doubling + frca) / prescal / div1 / div2;

    return freq;
}

static uint32_t get_pll_prescal()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL;
    temp &= 0x3f << 12;
    temp >>= 12;

    return temp;
}

static uint32_t get_pll_freq_doubling()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL;

    temp &= 0xfff << 20;
    temp >>= 20;

    return temp;
}

static uint32_t get_pll_div1()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL;
    temp &= 0x7 << 4;
    temp >>= 4;

    return temp;
}

static uint32_t get_pll_div2()
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_PLLCTRL;
    temp &= 0x7 << 8;
    temp >>= 8;

    return temp;
}

static uint32_t get_pll_freq()
{
    pmu_reg_t *reg = YUN_PMU;
    if (reg->PMU_PLLINTRCTRL & (1 << 2)) {
        return EHS_VALUE;
    }

    uint32_t freq = 0;
    uint32_t prescal = get_pll_prescal();
    uint32_t doubling = get_pll_freq_doubling();
    uint32_t div1 = get_pll_div1();
    uint32_t div2 = get_pll_div2();

    freq = EHS_VALUE * doubling / prescal / div1 / div2;

    return freq;
}

static uint32_t get_pll_cpr0_div()
{
    pmu_reg_t *reg = YUN_PMU;
    if (reg->PMU_SYSCLKDIVEN & 1) {
        uint32_t temp = reg->PMU_SYSCLKDIVEN;
        temp &= 0xf << 4;
        temp >>= 4;
        return temp + 2;
    }

    return 1;
}

static uint32_t get_pll_cpr1_div()
{
    pmu_reg_t *reg = YUN_PMU;
    if (reg->PMU_SYSCLKDIVEN & (1 << 8)) {
        uint32_t temp = reg->PMU_SYSCLKDIVEN;
        temp &= 0xf << 12;
        temp >>= 12;
        return temp + 2;
    }

    return 1;
}

static uint32_t get_ahb0_sub1_clk_div()
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t temp = reg->CPR0_SYSCLK0_DIV_CTL;
    temp &= 0x3;
    temp *= 2;

    if (temp == 0) {
        temp = 1;
    }

    return temp;
}

static uint32_t get_ahb0_sub2_clk_div()
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t temp = reg->CPR0_SYSCLK0_DIV_CTL;
    temp &= 0x3 << 4;
    temp >>= 4;
    temp *= 2;

    if (temp == 0) {
        temp = 1;
    }

    return temp;
}

static uint32_t get_apb0_clk_div()
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t temp = reg->CPR0_SYSCLK0_DIV_CTL;
    temp &= 0xf << 8;
    temp >>= 8;
    temp += 1;

    return temp;
}

static uint32_t get_apb1_clk_div()
{
    cpr1_reg_t *reg = YUN_CPR1;
    uint32_t temp = reg->CPR1_SYSCLK1_DIV_CTL & 0xf;
    temp += 1;

    return temp;
}

static void pll_cpr0_clk_enable(int en)
{
    pmu_reg_t *reg = YUN_PMU;

    if (en) {
        reg->PMU_SYSCLKDIVEN |= 1;
    } else {
        reg->PMU_SYSCLKDIVEN &= ~1;
    }
}

static void pll_cpr1_clk_enable(int en)
{
    pmu_reg_t *reg = YUN_PMU;

    if (en) {
        reg->PMU_SYSCLKDIVEN |= 1 << 8;
    } else {
        reg->PMU_SYSCLKDIVEN &= ~(1 << 8);
    }
}

/*value 2~17*/
static void pll_cpr0_clk_div(uint8_t val)
{
    if (val < 2) {
        val = 2;
    }

    val -= 2;
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_SYSCLKDIVEN;

    val &=0xf;
    temp &= ~(0xf << 4);
    temp |= val << 4;

    reg->PMU_SYSCLKDIVEN = temp;
}

/*value 2~17*/
static void pll_cpr1_clk_div(uint8_t val)
{
    if (val < 2) {
        val = 2;
    }

    val -= 2;
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_SYSCLKDIVEN;

    val &= 0xf;
    temp &= ~(0xf << 12);
    temp |= val << 12;

    reg->PMU_SYSCLKDIVEN = temp;
}

static void cpr0_ahb0_sub1_clk_div(uint8_t val)
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t temp = reg->CPR0_SYSCLK0_DIV_CTL;
    temp &= ~0x3;
    temp |= val & 0x3;

    reg->CPR0_SYSCLK0_DIV_CTL = temp;
}

static void cpr0_ahb0_sub2_clk_div(uint8_t val)
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t temp = reg->CPR0_SYSCLK0_DIV_CTL;
    temp &= ~(0x3 << 4);
    temp |= (val & 0x3) << 4;

    reg->CPR0_SYSCLK0_DIV_CTL = temp;
}

/*value 2~9*/
static void cpr0_apb0_clk_div(uint8_t val)
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t temp = reg->CPR0_SYSCLK0_DIV_CTL;
    temp &= ~(0xf << 8);
    temp |= (val & 0xf) << 8;

    reg->CPR0_SYSCLK0_DIV_CTL = temp;
}

/*value 2~9*/
static void cpr1_apb1_clk_div(uint8_t val)
{
    cpr1_reg_t *reg = YUN_CPR1;
    uint32_t temp = reg->CPR1_SYSCLK1_DIV_CTL;
    temp &= ~(0xf << 8);
    temp |= (val & 0xf) << 8;

    reg->CPR1_SYSCLK1_DIV_CTL = temp;
}

static uint32_t get_cpu0_cpu1_freq()
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t div = get_pll_cpr0_div();

    if (reg->CPR0_CPU0CLK_SEL) {
        return EHS_VALUE;
    }

    return get_pll_freq() / div;
}

static uint32_t get_cpu2_freq()
{
    cpr1_reg_t *reg = YUN_CPR1;
    uint32_t div = get_pll_cpr1_div();

    if (reg->CPR1_CPU2CLK_SEL) {
        return EHS_VALUE;
    }

    return get_pll_freq() / div;
}

void drv_lclk_select_src(clk_src_e src)
{
    pmu_reg_t *reg = YUN_PMU;

    if(src == ELS_CLK) {
        reg->PMU_CLKSRCSEL = 1;
    } else {
        reg->PMU_CLKSRCSEL = 0;
    }
}

boot_reson_t drv_get_boot_reason(void)
{
    pmu_reg_t *reg = YUN_PMU;
    uint32_t val = reg->PMU_RSTSTA;
    reg->PMU_RSTSTA = val;

    if (val == 8) {
        return CPU0_RESET;
    } else if (val == 4) {
        return WDG_RESET;
    } else if (val == 2) {
        return MCU_RESET;
    } else if (val == 1) {
        return POWER_RESET;
    }

    return val;
}

void yun_pll_config(pll_config_t *param)
{
    pmu_reg_t *reg = YUN_PMU;

    if (param->bypass_ehs_en) {
        pll_bypass(1);
        return;
    }

    pll_bypass(0);
    pll_set_prescale(param->prescale);
    pll_set_doubling(param->freq_doubling);
    pll_set_div1(param->pll_clk_div1);
    pll_set_div2(param->pll_clk_div2);
    reg->PMU_QSPI_CNT = 0x1e;
    pll_change_sw_trigger();

    while((reg->PMU_PLLCTRL & 1) == 0);
}

void yun_set_cpu0_cpu1_clk_div(uint8_t val)
{
    pll_cpr0_clk_div(val);
}

void yun_set_cpu2_clk_div(uint8_t val)
{
    pll_cpr1_clk_div(val);
}

void yun_cpu0_cpu1_clk_enable(int en)
{
    pll_cpr0_clk_enable(en);
}

void yun_cpu2_clk_enable(int en)
{
    pll_cpr1_clk_enable(en);
}

void yun_qspi_clock_en(int en)
{
    pmu_reg_t *reg = YUN_PMU;

    if (en) {
        reg->PMU_SYSCLKDIVEN |= 1 << 16;
    } else {
        reg->PMU_SYSCLKDIVEN &= ~(1 << 16);
    }
}

void yun_qspi_clk_div(uint8_t val)
{
    if (val < 2) {
        val = 2;
    }

    val -= 2;
    pmu_reg_t *reg = YUN_PMU;
    uint32_t temp = reg->PMU_SYSCLKDIVEN;

    val &=0x7;
    temp &= ~(0x7 << 20);
    temp |= val << 20;

    reg->PMU_SYSCLKDIVEN = temp;
}

int yun_get_cpu_freq(int idx)
{
    if (idx == 0 || idx == 1) {
        return get_cpu0_cpu1_freq();
    } else if (idx == 2) {
        return get_cpu2_freq();
    }

    return 0;
}

void yun_set_ahb0_sub1_clk_div(uint8_t val)
{
     cpr0_ahb0_sub1_clk_div(val);
}

uint32_t yun_get_ahb0_sub1_freq()
{
    uint32_t div = get_ahb0_sub1_clk_div();
    return get_cpu0_cpu1_freq() / div;
}

void yun_set_ahb0_sub2_clk_div(uint8_t val)
{
     cpr0_ahb0_sub2_clk_div(val);
}

uint32_t yun_get_ahb0_sub2_freq()
{
    uint32_t div = get_ahb0_sub2_clk_div();
    return get_cpu0_cpu1_freq() / div;
}

void yun_set_apb0_clk_div(uint8_t val)
{
    cpr0_apb0_clk_div(val);
}

uint32_t yun_get_apb0_freq()
{
    uint32_t div = get_apb0_clk_div();
    return get_cpu0_cpu1_freq() / div;
}

void yun_set_apb1_clk_div(uint8_t val)
{
    cpr1_apb1_clk_div(val);
}

uint32_t yun_get_apb1_freq()
{
    uint32_t div = get_apb1_clk_div();

    return get_cpu2_freq() / div;
}

void yun_audio_clk_config(pll_frac_config_t *param)
{
    pmu_reg_t *reg = YUN_PMU;

    if(param->bypass_ehs_en) {
        pll_frac_bypass(1);
        return;
    }

    pll_frac_bypass(0);
    pll_frac_set_prescale(param->prescale);
    pll_frac_set_doubling(param->freq_doubling);
    pll_frac_set_div1(param->pll_clk_div1);
    pll_frac_set_div2(param->pll_clk_div2);
    pll_set_frac(param->frac_val);

    reg->PMU_QSPI_CNT = 0x1e;
    pll_change_sw_trigger();

    while((reg->PMU_PLLCTRL_FRAC & 1) == 0);
}

uint32_t yun_get_audio_freq()
{
    return get_pll_frac_freq();
}

void yun_set_sdio_div(int idx, int div)
{
    cpr0_reg_t *reg = YUN_CPR0;
    div -= 1;
    uint32_t temp = reg->CPR0_SDIO_CLK_CTL;
    temp &= ~(0x7 << 8);
    temp |= (div << 8);

    reg->CPR0_SDIO_CLK_CTL = temp;
}

int drv_set_sdio_freq(int idx, int freq)
{
    uint32_t div = (yun_get_cpu_freq(0) + (yun_get_cpu_freq(0) % freq)) / freq;
    if (div > 8) {
        return -1;
    }

    yun_set_sdio_div(idx, div);
    return 0;
}

int32_t drv_get_sdio_freq(int idx)
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t div = reg->CPR0_SDIO_CLK_CTL;
    div &= 0x7 << 8;
    div = (div >> 8) + 1;

    return yun_get_cpu_freq(0) / div;
}

int32_t drv_set_cpu_freq(int idx, cpu_freq_t freq)
{
    if (idx == 0 || idx == 1) {
        if (freq != CPU_BYPASS_EHS) {
            uint32_t div = get_pll_freq() / freq;
            yun_set_cpu0_clk_src(PLL_CLK);
            yun_set_cpu0_cpu1_clk_div(div);
        } else {
            yun_set_cpu0_clk_src(EHS_CLK);
        }
        yun_set_apb0_clk_div(1);
    } else {
        return -1;
    }

    return 0;
}

ATTRIBUTE_DATA inline  void yun_sdram_auto_flash_enable(int en)
{
    #define PREG32(addr) *((volatile unsigned int *)addr)
    if (en) {
        csi_dcache_clean_invalid();
        PREG32(0x1a00000c) |= 0x2;
        while( (PREG32(0x1a00000c) & (1 << 11)) == 0);
    } else {
        PREG32(0x1a00000c) &= (~0x2);
        while( (PREG32(0x1a00000c) & (1 << 11)) == 1);
    }
}

ATTRIBUTE_DATA static void delay(int ms)
{
    volatile int i, j;

    for (i = 0; i < ms; i++) {
        for (j = 0; j < i; j++) {
            ;
        }
    }
}

ATTRIBUTE_DATA int yun_enter_lpm(void)
{
    #define PMU_WKUPMASK 0x30000018
    #define PMU_LPCR     0x30000014
    #define PMU_DLC_IFR  0x30000210
    PREG32(PMU_WKUPMASK) = 0xF;
    PREG32(PMU_LPCR) = 0x1;
    PREG32(PMU_DLC_IFR) = 0x1f;
    delay(1000);
    PREG32(0xe000e280) = 0x40;
    PREG32(0x90000200) = 0x1;
    delay(1000);

    uint32_t state = csi_irq_save();

    csi_dcache_clean_invalid();

    PREG32(0x1a00000c) |= 0x2;
    while( (PREG32(0x1a00000c) & (1 << 11)) == 0);
    asm("wait");

    PREG32(0x1a00000c) &= (~0x2);
    while( (PREG32(0x1a00000c) & (1 << 11)) == 1);

    PREG32(0x31000014) = 0x1;
    PREG32(0x1a000100) = 0x0;
    PREG32(0x1a000054) = 0x8;
    PREG32(0x1a000000) = 0x1c0f68;
    PREG32(0x1a000004) = 0x029e949f;
    PREG32(0x1a00000c) = 0x3009;
    csi_irq_restore(state);
    while(PREG32(0x1a00000c) & 1){;}

    return 0;
}

