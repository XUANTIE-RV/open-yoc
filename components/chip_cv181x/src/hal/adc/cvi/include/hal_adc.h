/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/

#ifndef __CVI_ADC_H__
#define __CVI_ADC_H__

#include <drv/common.h>
#include "aos/cli.h"

#ifdef __cplusplus
extern "C" {
#endif

// #define CSI_DRV_DEBUG

#ifndef pr_err
#define pr_err(x, args...) aos_cli_printf("[%s|%d] - " x, __func__, __LINE__, ##args)
#endif

#ifdef CSI_DRV_DEBUG
#ifndef pr_debug
#define pr_debug(x, args...) aos_cli_printf("[%s|%d] - " x, __func__, __LINE__, ##args)
#endif

#ifndef pr_warn
#define pr_warn(x, args...) aos_cli_printf("[%s|%d] - " x, __func__, __LINE__, ##args)
#endif

#else

#ifndef pr_debug
#define pr_debug(x, args...)
#endif

#ifndef pr_warn
#define pr_warn(x, args...)
#endif

#endif

#define     __IM     volatile const       /*! Defines 'read only' structure member permissions */
#define     __OM     volatile             /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile             /*! Defines 'read / write' structure member permissions */

/* CTRL REG */
#define ADC_CTRL_ADC_START_Pos                      (0U)
#define ADC_CTRL_ADC_START_Msk                      (1U << ADC_CTRL_ADC_START_Pos)
#define ADC_CTRL_ADC_START                          ADC_CTRL_ADC_START_Msk

#define ADC_CTRL_ADC_SEL_Pos                        (4U)
#define ADC_CHANNEL_SEL_Msk                         (0xF << ADC_CTRL_ADC_SEL_Pos)
#define ADC_CHANNEL_SEL_1		                   (1U << (ADC_CTRL_ADC_SEL_Pos + 1U))
#define ADC_CHANNEL_SEL_2		                   (1U << (ADC_CTRL_ADC_SEL_Pos + 2U))
#define ADC_CHANNEL_SEL_3		                   (1U << (ADC_CTRL_ADC_SEL_Pos + 3U))
#define ADC_CHANNEL_SEL_CH(_ch_)		           (1U << (ADC_CTRL_ADC_SEL_Pos + _ch_))

/* STATUS REG */
#define ADC_STATUS_ADC_BUSY_Pos                      (0U)
#define ADC_STATUS_ADC_BUSY_Msk                      (0x1U << ADC_STATUS_ADC_BUSY_Pos)

/* CYC_SET REG */
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos                     (12U)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_Msk                     (0xF << ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_1	                   (0U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_2	                   (1U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_3	                   (2U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_4	                   (3U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_5	                   (4U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_6	                   (5U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_7	                   (6U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_8	                   (7U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_9	                   (8U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_10	                   (9U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_11	                   (10U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_12	                   (11U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_13	                   (12U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_14	                   (13U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_15	                   (14U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)
#define ADC_CYC_SET_ADC_CYC_CLK_DIV_16	                   (15U<< ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos)

#define ADC_CYC_SET_ADC_CYC_SAMPLE_Pos                  (8U)
#define ADC_CYC_SET_ADC_CYC_SAMPLE_Msk                  (0xF << ADC_CYC_SET_ADC_CYC_SAMPLE_Pos)

/* INT_EN REG */
#define ADC_EN_ALL_CHANNEL_DONE_Pos                 (0U)
#define ADC_EN_ALL_CHANNEL_DONE_Msk                 (1U << ADC_EN_ALL_CHANNEL_DONE_Pos)
#define ADC_EN_ALL_CHANNEL_DONE_EN                  ADC_EN_ALL_CHANNEL_DONE_Msk

/* INT_STATUS, offset: 0x14 */
#define ADC_INT_ALL_CONNNEL_DONE_Pos                 (0U)
#define ADC_INT_ALL_CONNNEL_DONE_Msk                 (1U << ADC_INT_ALL_CONNNEL_DONE_Pos)
#define ADC_INT_ALL_CONNNEL_DONE                     ADC_INT_ALL_CONNNEL_DONE_Msk

/* ADC_DATA, offset: 0x18 */
#define ADC_DATA_Pos                                (0U)
#define ADC_DATA_Msk                                (0xFFFU << ADC_DATA_Pos)

/* ANA_REGISTER, offset: 0x30 */
#define ADC_EN_Pos                             (0U)
#define ADC_EN_Msk                             (0x1U << ADC_EN_Pos)
#define ADC_EN                                 ADC_EN_Msk

#define ADC_INTR_Pos                            (0U)
#define ADC_INTR_Msk                            (1U << ADC_INTR_Pos)
#define ADC_INTR_En                             ADC_INTR_Msk

struct cvi_adc_regs_t {
    uint32_t CTRL;
    uint32_t STATUS;
    uint32_t CYC_SET;
    uint32_t CH1_RESULT;
    uint32_t CH2_RESULT;
    uint32_t CH3_RESULT;
    uint32_t INTR_EN;
    uint32_t INTR_CLR;
    uint32_t INTR_STA;
    uint32_t INTR_RAW;
};

static struct cvi_adc_regs_t cv182x_adc_reg = {
    .CTRL = 0x4,
    .STATUS = 0x8,
    .CYC_SET = 0xc,
    .CH1_RESULT = 0x14,
    .CH2_RESULT = 0x18,
    .CH3_RESULT = 0x1c,
    .INTR_EN = 0x20,
    .INTR_CLR = 0x24,
    .INTR_STA = 0x28,
    .INTR_RAW = 0x2c,
};

static struct cvi_adc_regs_t *cvi_adc_reg = &cv182x_adc_reg;

#define ADC_CTRL(reg_base)         *((__IOM uint32_t *)(reg_base + cvi_adc_reg->CTRL))
#define ADC_STATUS(reg_base)       *((__IM uint32_t *)(reg_base + cvi_adc_reg->STATUS))
#define ADC_CYC_SET(reg_base)      *((__IOM uint32_t *)(reg_base + cvi_adc_reg->CYC_SET))

#define ADC_CH1_RESULT(reg_base)   *((__IM uint32_t *)(reg_base + cvi_adc_reg->CH1_RESULT))
#define ADC_CH2_RESULT(reg_base)   *((__IM uint32_t *)(reg_base + cvi_adc_reg->CH2_RESULT))
#define ADC_CH3_RESULT(reg_base)   *((__IM uint32_t *)(reg_base + cvi_adc_reg->CH3_RESULT))
#define ADC_RESULT_CH(reg_base, _ch_)   *((__IM uint32_t *)(reg_base + cvi_adc_reg->CH1_RESULT + (_ch_ - 1) * 4))

#define ADC_INTR_EN(reg_base)      *((__IOM uint32_t *)(reg_base + cvi_adc_reg->INTR_EN))
#define ADC_INTR_CLR(reg_base)     *((__IOM uint32_t *)(reg_base + cvi_adc_reg->INTR_CLR))
#define ADC_INTR_STA(reg_base)     *((__IM uint32_t *)(reg_base + cvi_adc_reg->INTR_STA))
#define ADC_INTR_RAW(reg_base)     *((__IM uint32_t *)(reg_base + cvi_adc_reg->INTR_RAW))

static inline void adc_cyc_setting(unsigned long reg_base)
{
    pr_debug("write reg %08p value %#x\n", &ADC_CYC_SET(reg_base), ADC_CYC_SET(reg_base) | ADC_CYC_SET_ADC_CYC_CLK_DIV_16);
    ADC_CYC_SET(reg_base) &= ~ADC_CYC_SET_ADC_CYC_CLK_DIV_16;
    ADC_CYC_SET(reg_base) |= ADC_CYC_SET_ADC_CYC_CLK_DIV_16;//set saradc clock cycle=840ns
}

static inline void adc_start(unsigned long reg_base)
{
    pr_debug("write reg %08p value %#x\n", &ADC_CTRL(reg_base), ADC_CTRL(reg_base) | ADC_CTRL_ADC_START);
    ADC_CTRL(reg_base) |= ADC_CTRL_ADC_START;
}

static inline uint32_t adc_get_start(unsigned long reg_base)
{
    return (ADC_CTRL(reg_base) & ADC_CTRL_ADC_START_Msk);
}

static inline void adc_stop(unsigned long reg_base)
{
    ADC_CTRL(reg_base) &= ~ADC_CTRL_ADC_START;
}

static inline uint32_t adc_get_stop(unsigned long reg_base)
{
    return (ADC_CTRL(reg_base) & ADC_CTRL_ADC_START_Msk);
}

static inline uint32_t adc_get_data_ready(unsigned long reg_base)
{
    return !(ADC_STATUS(reg_base) & ADC_STATUS_ADC_BUSY_Msk);
}

static inline uint32_t adc_get_idle(unsigned long reg_base)
{
    return !(ADC_STATUS(reg_base) & ADC_STATUS_ADC_BUSY_Msk);
}

static inline void adc_set_sel_channel(unsigned long reg_base, uint32_t value)
{
    pr_debug("write reg %08p value %#x\n", &ADC_CTRL(reg_base), value);
    ADC_CTRL(reg_base) |= value;
}

static inline void adc_reset_sel_channel(unsigned long reg_base, uint32_t value)
{
    pr_debug("write reg %08p value %#x\n", &ADC_CTRL(reg_base), value);
    ADC_CTRL(reg_base) &= ~value;
}

static inline uint32_t adc_get_sel_channel(unsigned long reg_base)
{
    return (ADC_CTRL(reg_base) & ADC_CHANNEL_SEL_Msk);
}

static inline void adc_set_clk_div(unsigned long reg_base, uint32_t value)
{
    ADC_CYC_SET(reg_base) &= ~ADC_CYC_SET_ADC_CYC_CLK_DIV_Msk;
    ADC_CYC_SET(reg_base) |= (value << ADC_CYC_SET_ADC_CYC_CLK_DIV_Pos);
}
static inline uint32_t adc_get_clk_div(unsigned long reg_base)
{
    pr_debug("read reg %08p value %#x\n", &ADC_CYC_SET(reg_base), (ADC_CYC_SET(reg_base) & ADC_CYC_SET_ADC_CYC_CLK_DIV_Msk));
    return (ADC_CYC_SET(reg_base) & ADC_CYC_SET_ADC_CYC_CLK_DIV_Msk);
}

static inline void adc_set_sample_cycle(unsigned long reg_base, uint32_t value)
{
    ADC_CYC_SET(reg_base) &= ~ADC_CYC_SET_ADC_CYC_SAMPLE_Msk;
    ADC_CYC_SET(reg_base) |= (value << ADC_CYC_SET_ADC_CYC_SAMPLE_Pos);
}

static inline void adc_en_int_all_channel_done(unsigned long reg_base)
{
    ADC_INTR_EN(reg_base) |= ADC_EN_ALL_CHANNEL_DONE_EN;
}

static inline void adc_dis_int_all_channel_done(unsigned long reg_base)
{
    ADC_INTR_EN(reg_base) &= ~ADC_EN_ALL_CHANNEL_DONE_EN;
}

static inline void adc_int_en(unsigned long reg_base)
{
    ADC_INTR_EN(reg_base) |= ADC_INTR_En;
}

static inline void adc_int_dis(unsigned long reg_base)
{
    ADC_INTR_EN(reg_base) &= ~ADC_INTR_En;
}

static inline uint32_t adc_get_int_flag(unsigned long reg_base)
{
    return (ADC_INTR_STA(reg_base) &  ADC_EN_ALL_CHANNEL_DONE_Msk);
}

static inline void adc_clear_int_flag(unsigned long reg_base)
{
    ADC_INTR_CLR(reg_base) |= ADC_EN_ALL_CHANNEL_DONE_EN;
}

static inline uint32_t adc_get_channel1_data(unsigned long reg_base)
{
    pr_debug("read reg %08p value %#x\n", &ADC_CH1_RESULT(reg_base), (ADC_CH1_RESULT(reg_base) & ADC_DATA_Msk));
    return (ADC_CH1_RESULT(reg_base) & ADC_DATA_Msk);
}

static inline uint32_t adc_get_channel2_data(unsigned long reg_base)
{
    return (ADC_CH2_RESULT(reg_base) & ADC_DATA_Msk);
}

static inline uint32_t adc_get_channel3_data(unsigned long reg_base)
{
    return (ADC_CH3_RESULT(reg_base) & ADC_DATA_Msk);
}

static inline uint32_t adc_get_channel_data_ch(unsigned long reg_base, uint32_t ch)
{
    return (ADC_RESULT_CH(reg_base, ch) & ADC_DATA_Msk);
}

#ifdef __cplusplus
}
#endif

#endif  /* _CVI_ADC_LL_H_*/
