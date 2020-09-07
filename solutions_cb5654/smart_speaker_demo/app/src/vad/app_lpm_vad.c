/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "silan_voice_adc.h"

#include "hw_config.h"
#include "app_config.h"

#include <drv/gpio.h>
#include <pin_name.h>
#include "app_lpm.h"

extern int sram_adc_val_read(int channel, int *vol);
extern int32_t sram_drv_gpio_read(int32_t gpio_pin);

extern SRAM_DATA lpm_pin_src_t wake_srcs_sleep[MAX_LPM_WAKESRC_COUNT];

SRAM_DATA uint32_t g_sram_timer_wakeup = 0;

SRAM_TEXT int check_pin_wakeup()
{
    for (int i = 0; i < MAX_LPM_WAKESRC_COUNT; ++i) {
        if (wake_srcs_sleep[i].type == LPM_PIN_SRC_GPIO) {
            int gpio_state = sram_drv_gpio_read(wake_srcs_sleep[i].pin);
            if (wake_srcs_sleep[i].mode == GPIO_IRQ_MODE_LOW_LEVEL && gpio_state == 0) {
                return 1;
            } else if (wake_srcs_sleep[i].mode == GPIO_IRQ_MODE_HIGH_LEVEL && gpio_state == 1) {
                return 1;
            }
        } else if (wake_srcs_sleep[i].type == LPM_PIN_SRC_ADC) {
            int val = 0;
            if (sram_adc_val_read(wake_srcs_sleep[i].pin, &val) == 0) {
                if (val < VAD_ADC_VAL_MAX && val > VAD_ADC_VAL_MIN) {
                    return 1;
                }
            }
        }
    }

    return 0;
}


#if defined(CONFIG_TEST_LPMVAD) && CONFIG_TEST_LPMVAD

SRAM_TEXT int sram_vad1(void)
{
    uint32_t size;
    int16_t buff[100]; /* 不能进行初始化, 编译器会把初始化变成memset,导致访问sdram */
    int i;

    if (check_pin_wakeup()) {
        return 1;
    }

    SRAM_DATA static int wake_times = 0;
    SRAM_DATA static int call_times = 0;

    codec_adrb_t* rb_mic = voice_get_mic_rb_sram();

    if (codec_adrb_available_read_space(rb_mic) >= 64) {
        call_times ++;
        
        size = codec_adrb_read(rb_mic, (uint8_t*)buff, 64);
        for (i = 0; i < size / 2; i++) {
            if (buff[i] >= 640) {
                wake_times ++;
                break;
            }
        }
    }

    if (call_times >= 40) {
        if (wake_times > 15 && wake_times < 30) {

            #if SDRAM_ACCESS_EN
                printf("vad wake %d\n", wake_times);
            #endif

            wake_times = call_times = 0;
#if SRAM_TIMER_EN
            g_sram_timer_wakeup = sram_timer_get_us();
#endif
            return 1;
        }
        wake_times = call_times = 0;
    }

    return 0;
}

int app_softvad_init(void)
{
    voice_register_vadalg(sram_vad1);
    return 0;
}
#endif

#if defined(CONFIG_MIT_LPMVAD) && CONFIG_MIT_LPMVAD
#include "lpm_mit_vad.h"
SRAM_TEXT int sram_vad_mit(void)
{
#define	READ_BYTE_PER_CHANNEL	64
    int i;
    int16_t buff[READ_BYTE_PER_CHANNEL];
    int16_t buff2[READ_BYTE_PER_CHANNEL / 2];

    lpm_VadDetectResult ret_vad_detect = kVadDetectResultVoice;

    if (check_pin_wakeup()) {
        return 1;
    }
    
    codec_adrb_t* rb_mic = voice_get_mic_rb_sram();

    if (codec_adrb_available_read_space(rb_mic) >= READ_BYTE_PER_CHANNEL) {
        codec_adrb_read(rb_mic, (uint8_t*)buff, READ_BYTE_PER_CHANNEL);

        for(i = 0; i < READ_BYTE_PER_CHANNEL / 2; i++) {
            buff2[i] = buff[i*2];
        }

        ret_vad_detect = lpm_vad_detect((const char *)buff2,READ_BYTE_PER_CHANNEL/2);

        if(ret_vad_detect==kVadDetectResultStartpoint) {
        #if SDRAM_ACCESS_EN
            //vad start
            printf("vad start");
        #endif
            return 1;
        }
        else if(ret_vad_detect==kVadDetectResultEndpoint) {
        #if SDRAM_ACCESS_EN
            //vad end
            printf("vad end");
        #endif
        }
        else {
        #if SDRAM_ACCESS_EN
            //printf("ret_vad_detect=%d", ret_vad_detect);
        #endif
        }
    }
    return 0;
}

int app_softvad_init(void)
{
    lpm_vad_init();
    voice_register_vadalg(sram_vad_mit);
    return 0;
}
#endif
