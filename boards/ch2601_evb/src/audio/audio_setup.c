/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <board.h>
#include <devices/drv_snd_alkaid.h>

/* 默认采集增益 */


/*采集增益配置*/
int board_audio_in_set_gain(int id, int gain)
{
    return 0;
}

int board_audio_in_get_gain(int id)
{
    return 0;
}

/*播放增益配置*/
extern int auo_digtal_gain(void *context, float val);
int board_audio_out_set_gain(int id, int gain)
{
    return 0;
}

int board_audio_out_get_gain(int id)
{
    return 0;
}

#ifdef CONFIG_SMART_AUDIO
#include <smart_audio.h>
extern audio_vol_config_t g_vol_config;

audio_vol_config_t *board_audio_out_get_vol_config()
{
    return &g_vol_config;
}

/*EQ参数配置*/
int board_eq_set_param(void *data, int byte)
{
    if (data == NULL) {
        /*设置默认参数*/
       //TODO
        return 0;
    }
    return 0;
}

int board_eq_show_param(void)
{
    return 0;
}

int board_eq_get_type(void)
{
    return EQ_TYPE_NONE;
}

uint8_t *board_eq_get_param(size_t *byte)
{
    /*硬件EQ无需实现*/
    return NULL;
}

int board_drc_set_param(void *data, int byte)
{
    if (data == NULL) {
        /*设置默认参数*/
        //TODO
        return 0;
    }

    return 0;
}
#endif /*CONFIG_SMART_AUDIO*/

/*PA控制引脚*/
int board_audio_get_pa_mute_pin(void)
{
    return -1;
}

void board_audio_init(void)
{
    snd_card_alkaid_register(NULL);
}
