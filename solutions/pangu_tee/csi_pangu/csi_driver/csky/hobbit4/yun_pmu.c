/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
#include "yun_pmu.h"
#include "cpr_regs.h"
#include "pmu_regs.h"
#include "soc.h"

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

static uint32_t get_apb0_clk_div()
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t temp = reg->CPR0_SYSCLK0_DIV_CTL;
    temp &= 0xf << 8;
    temp >>= 8;
    temp += 1;

    return temp;
}

// static uint32_t get_apb1_clk_div()
// {
//     cpr1_reg_t *reg = YUN_CPR1;
//     uint32_t temp = reg->CPR1_SYSCLK1_DIV_CTL & 0xf;
//     temp += 1;

//     return temp;
// }

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

// static uint32_t get_pll_cpr1_div()
// {
//     pmu_reg_t *reg = YUN_PMU;
//     if (reg->PMU_SYSCLKDIVEN & (1 << 8)) {
//         uint32_t temp = reg->PMU_SYSCLKDIVEN;
//         temp &= 0xf << 12;
//         temp >>= 12;
//         return temp + 2;
//     }

//     return 1;
// }

static uint32_t get_cpu0_cpu1_freq()
{
    cpr0_reg_t *reg = YUN_CPR0;
    uint32_t div = get_pll_cpr0_div();

    if (reg->CPR0_CPU0CLK_SEL) {
        return EHS_VALUE;
    }

    return get_pll_freq() / div;
}

uint32_t yun_get_apb0_freq()
{
    uint32_t div = get_apb0_clk_div();
    return get_cpu0_cpu1_freq() / div;
}

// uint32_t yun_get_apb1_freq()
// {
//     uint32_t div = get_apb1_clk_div();

//     return get_cpu2_freq() / div;
// }