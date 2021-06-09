/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "silan_voice_adc.h"

#include <silan_adc_regs.h>
#include <silan_adc_csi.h>
#include <soc.h>
#include <silan_syscfg.h>
#define silan_adc_regs  ((adc_regs_struct_t *)SILAN_ADC_BASE)
#define ADC_BUSY_TIMEOUT  1000
#define ADC_CON_DATA_NUM 1

/**
  \brief       start adc.
  \param[in]   handle adc handle to operate.
  \return      null
*/
SRAM_TEXT void sram_drv_adc_start(void)
{
    silan_adc_regs->adc_cfg &= ~DISABLE;
}

/**
  \brief       stop adc.
  \param[in]   handle adc handle to operate.
  \return      null
*/
SRAM_TEXT void sram_drv_adc_stop(void)
{
    silan_adc_regs->adc_cfg |= DISABLE;
}

/**
  \brief       receiving data from ADC receiver.
  \param[in]   handle ADC handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from ADC receiver.
  \param[in]   num   Number of data items to receive.
  \return      error code
*/
SRAM_TEXT int32_t sram_drv_adc_read(uint32_t *data, uint32_t num)
{
    int32_t timecount = 0;
    uint32_t int_status = 0;

    silan_adc_regs->adc_cfg |= (START | ADC_START);
    // silan_adc_regs->adc_cfg |= ADC_START;

    int_status = silan_adc_regs->adc_int_rsts;
    while (!(int_status & INT_STAT_SIN_FINISH)) {
        int_status = silan_adc_regs->adc_int_rsts;
        timecount++;

        if (timecount >= ADC_BUSY_TIMEOUT) {
            return -1;
        }
    }

    *data = silan_adc_regs->adc_data;

    silan_adc_regs->adc_int_clr |= INT_STAT_SIN_FINISH;
    silan_adc_regs->adc_cfg &= ~ADC_START;
    return 0;
}

SRAM_TEXT void sram_adc_cclk_config(int clksel, silan_clk_onff_t onoff)
{
    misc_clk_reg2_t misc_clk_reg;

    misc_clk_reg.d32 = __REG32(SILAN_SYSCFG_REG3);
    misc_clk_reg.b.adc_clksel = clksel;
    misc_clk_reg.b.adc_clkon = onoff;
    __REG32(SILAN_SYSCFG_REG3) = misc_clk_reg.d32;
}

SRAM_TEXT int sram_adc_val_read(int channel, int *vol)
{
    int i, ret;
    uint32_t recv_data[ADC_CON_DATA_NUM];

    for (i = 0; i < ADC_CON_DATA_NUM; i++) {
        recv_data[i] = 0;
    }

    sram_adc_cclk_config(ADC_CCLK_PLLREF_DIV2, CLK_ON);
    __REG32(SILAN_INTR_STS_BASE + 0x200 + SARADC_IRQn * 4) |= 1;

    __REG32(SILAN_SYSCFG_REG4) |= ((1<<channel));
    silan_adc_regs->adc_cfg |= BYPASS;
    silan_adc_regs->adc_cfg &= ~CSEL(0x7);
    silan_adc_regs->adc_cfg |= CSEL(channel);

    sram_drv_adc_start();

    ret = sram_drv_adc_read(&recv_data[0], ADC_CON_DATA_NUM);
    if (ret < 0) {
        return -2;
    }

    sram_drv_adc_stop();

    sram_adc_cclk_config(ADC_CCLK_PLLREF_DIV2, CLK_OFF);
    __REG32(SILAN_INTR_STS_BASE + 0x200 + SARADC_IRQn * 4) &= ~1;

    *vol = recv_data[0];

    return 0;
}