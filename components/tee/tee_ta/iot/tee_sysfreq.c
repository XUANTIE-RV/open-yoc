/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_msg_cmd.h>
#include <soc.h>

#if (CONFIG_SYS_FREQ > 0)

#define PMU_MCLK_SEL  (CSKY_CLKGEN_BASE + 0x4)
#define MCLK_REG_VAL  0x8UL

#define PMU_CLK_STABLE  (CSKY_CLKGEN_BASE + 0x18)
#define PMU_PLL_CTRL  (CSKY_CLKGEN_BASE + 0x2c)

typedef enum {
    SET_FREQ    = 0,
    GET_FREQ    = 1
} opr_type_e;

typedef enum {
    IHS_CLK       = 0,         /* internal high speed clock */
    EHS_CLK       = 1          /* external high speed clock */
} clk_src_e;

typedef enum {
    OSR_8M_CLK_16M      = 0x80204,
    OSR_8M_CLK_24M      = 0x80206,
    OSR_8M_CLK_32M      = 0x80208,
    OSR_8M_CLK_40M      = 0x8020a,
    OSR_8M_CLK_48M      = 0x8020c
} clk_val_e;

#define CK_EFLASH_TRC     0x4003f020
#define CK_EFLASH_TNVS    0x4003f024
#define CK_EFLASH_TPGS    0x4003f028
#define CK_EFLASH_TPROG   0x4003f02c
#define CK_EFLASH_TRCV    0x4003f030
#define CK_EFLASH_TERASE  0x4003f034

typedef struct {
    uint8_t trc;
    uint16_t tnvs;
    uint8_t tpgs;
    uint16_t tprog;
    uint16_t trcv_erase;
} eflash_opt_time_t;

static const eflash_opt_time_t eflash_opt_time[] = {
    {0x0, 0x35, 0x16, 0x35, 0x1b9},
    {0x0, 0x6a, 0x2d, 0x6b, 0x371},
    {0x0, 0x9f, 0x44, 0xa1, 0x528},
    {0x1, 0xd4, 0x5a, 0xd7, 0x6e1},
    {0x1, 0x109, 0x71, 0x10c, 0x89b},
    {0x1, 0x13e, 0x88, 0x141, 0xa56},
};

static inline uint32_t getreg32(volatile uint32_t *addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void putreg32(uint32_t val, volatile uint32_t *addr)
{
    *(volatile uint32_t *)addr = val;
}

static int set_sys_freq(clk_src_e source, clk_val_e val)
{
    /* calculate the pllout frequence */
    uint8_t osr_freq = val >> 16;
    uint8_t pllm = val & 0x3f;
    uint8_t plln = (val >> 8) & 0x3f;
    uint8_t pllout = osr_freq * (pllm / plln);

    if (osr_freq != (EHS_VALUE / 1000000) || pllout < 16 || pllout >= 60 || source == IHS_CLK) {
        return TEE_ERROR_BAD_PARAMETERS;
    }


    val = val & 0xfff;
    int timeout = 10000;

    /* config pll and wait until stable */
    if (source == EHS_CLK) {
        val |= (3 << 18);
    }

    putreg32(val, (uint32_t *)PMU_PLL_CTRL);

    while (timeout--) {
        if (getreg32((uint32_t *)PMU_CLK_STABLE) & 0x00000010) {
            break;
        }
    }

    putreg32(MCLK_REG_VAL, (uint32_t *)PMU_MCLK_SEL);

    /* set eflash control operation time */
    uint8_t index = ((pllout + 7) / 8) - 1;
    if (index >= sizeof(eflash_opt_time)/sizeof(eflash_opt_time_t)) {
        return TEE_ERROR_GENERIC;
    }
    eflash_opt_time_t *opt_time = (eflash_opt_time_t *)&eflash_opt_time[index];

    putreg32(opt_time->trc, (uint32_t *)CK_EFLASH_TRC);
    putreg32(opt_time->tnvs, (uint32_t *)CK_EFLASH_TNVS);
    putreg32(opt_time->tpgs, (uint32_t *)CK_EFLASH_TPGS);
    putreg32(opt_time->tprog, (uint32_t *)CK_EFLASH_TPROG);
    putreg32(opt_time->trcv_erase, (uint32_t *)CK_EFLASH_TRCV);

    // reinitialize uart clock
    extern void console_init();

    console_init();

    return TEE_SUCCESS;
}

static int get_sys_freq(uint32_t src_val)
{
    int sysclk;
    int pllclk;

    pllclk = *(uint32_t *)PMU_PLL_CTRL;
    pllclk = pllclk  & 0x0000000F;
    if (pllclk % 2) {
        sysclk = SYSTEM_CLOCK;
    } else {
        sysclk = pllclk * 4000000;
    }

    *(uint32_t *)src_val = sysclk;

    return TEE_SUCCESS;
}

int tee_core_sys_freq(tee_param params[4])
{
    uint32_t opr = params[0].value.a;
    uint32_t clk_src;
    uint32_t clk_val;
    int ret = TEE_SUCCESS;

    if (opr == SET_FREQ) {
        clk_src = params[0].value.b;
        clk_val = params[1].value.a;

        ret =  set_sys_freq(clk_src, clk_val);
    } else if (opr == GET_FREQ) {
        clk_val = params[0].value.b;

        ret = get_sys_freq(clk_val);

    } else {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    return ret;
}

#endif

