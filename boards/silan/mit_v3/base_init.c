/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
 
#include <devices/devicelist.h>
#include "silan_voice_adc.h"
#include "board_config.h"

void board_base_init(void)
{
#ifdef SOC_DSP_LDO_LEVEL
    extern void silan_dsp_ldo_config(int level);
    extern void silan_soc_ldo_config(int level);
    silan_dsp_ldo_config(SOC_DSP_LDO_LEVEL);
    silan_soc_ldo_config(SOC_DSP_LDO_LEVEL);
#endif

    uart_csky_register(0); /* UART1 */
    uart_csky_register(1); /* UART2 */
    uart_csky_register(2); /* UART3 */
    spiflash_csky_register(0);

#ifndef CONFIG_DMAC_DSP_ACQ
    sram_init();
#endif
}

void board_audio_init()
{
#ifndef CONFIG_DMAC_DSP_ACQ
    /* 参考音 增益, 前端反馈，理论 (16)0dB即可，但单端模补偿6dB*/
    voice_ref_init(24, 24); /* 数值单位0.75dB 16 + 6/0.75 = 24 */

    /* 麦克风 增益, boost (3)20dB  模拟增益(8)0dB，伪差分补偿6dB，看信号还较小继续增加12dB */
    int mic_gain_val = 8 + (CONFIG_MIC_GAIN * 2 / 3); 
    voice_mic_init(3, mic_gain_val, mic_gain_val); /*数值单位1.5dB  8 + 18/1.5 = 20*/
#endif
}
