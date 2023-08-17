/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO
#include <stdint.h>
#include <cv181x_snd.h>
#include <alsa/mixer.h>

#include "eq_config.h"
#include "board_audio.h"

/*
 默认采集增益 */
static int g_audio_in_gain[]  = { AUIDO_IN_GAIN_MIC, AUIDO_IN_GAIN_MIC, AUIDO_IN_GAIN_REF };
static int g_audio_out_gain[] = { AUIDO_OUT_GAIN, AUIDO_OUT_GAIN };

int board_audio_in_set_gain(int id, int gain)
{
    if (id < 0 || id >= sizeof(g_audio_in_gain) / sizeof(int)) {
        return -1;
    }

    printf("dmic unspport set gain\r\n");

    g_audio_in_gain[id] = gain;
    return 0;
}

int board_audio_in_get_gain(int id)
{
    if (id < 0 || id >= sizeof(g_audio_in_gain) / sizeof(int)) {
        return -1;
    }

    printf("dmic unspport set gain\r\n");

    return g_audio_in_gain[id];
}


int board_audio_out_set_gain(int id, int gain)
{
    /*0\1:对应左右声道, 2:全部配置*/
    if (id < 0 || id > sizeof(g_audio_out_gain) / sizeof(int)) {
        return -1;
    }

    //TODO: 设置播放模拟增益
    aos_mixer_t *mixer_hdl;
    aos_mixer_elem_t *elem;

    aos_mixer_open(&mixer_hdl, 0);  //申请mixer
    aos_mixer_attach(mixer_hdl, "card0"); //查找声卡
    aos_mixer_load(mixer_hdl); // 装载mxier
    elem = aos_mixer_first_elem(mixer_hdl); //查找第一个元素

    if (id == 0) {
        aos_mixer_selem_set_playback_db(elem, AOS_MIXER_SCHN_FRONT_LEFT, gain);
        g_audio_out_gain[id] = gain;
    } else if (id == 1) {
        aos_mixer_selem_set_playback_db(elem, AOS_MIXER_SCHN_FRONT_RIGHT, gain);
        g_audio_out_gain[id] = gain;
    } else if (id == 2) {
        aos_mixer_selem_set_playback_db_all(elem, gain); //设置音量为
        g_audio_out_gain[0] = gain;
        g_audio_out_gain[1] = gain;
    } else {
        ;
    }

    aos_mixer_close(mixer_hdl);

    return 0;
}

int board_audio_out_get_gain(int id)
{
    if (id < 0 || id >= sizeof(g_audio_out_gain) / sizeof(int)) {
        return -1;
    }

    return g_audio_out_gain[0];
}

#ifdef CONFIG_SMART_AUDIO
extern audio_vol_config_t g_vol_config;

audio_vol_config_t *board_audio_out_get_vol_config()
{
    return &g_vol_config;
}

int board_eq_set_param(void *data, int byte)
{
    if (data == NULL) {
        //TODO:设置默认EQ参数
        //xxx_eq_set((uint32_t *)eq_filt_coef, sizeof(eq_filt_coef));
        return 0;
    }

    //TODO:设置EQ参数
    //return xxx_eq_set((uint32_t *)data, byte);
    return 0;
}

int board_eq_show_param(void)
{
    //打印EQ参数
    //return xxx_eq_show();
    return 0;
}

int board_eq_get_type(void)
{
    return EQ_TYPE_HARDWARE;
}

uint8_t *board_eq_get_param(size_t *byte)
{
    /*硬件EQ无需实现*/
    return NULL;
}

int board_drc_set_param(void *data, int byte)
{
    if (data == NULL) {
        //TODO: 设置默认DRC参数
        //xxx_drc_set((uint32_t *)drc_filt_coef, sizeof(drc_filt_coef));
        return 0;
    }

    //TODO: 设置DRC参数
    //return xxx_drc_set((uint32_t *)data, byte);
    return 0;
}
#endif /*CONFIG_SMART_AUDIO*/

/*PA控制引脚*/
#if BOARD_AUDIO_SUPPORT_AMP
void board_audio_amplifier_onoff(int on)
{
    extern void PLATFORM_SpkMute(int value);
    PLATFORM_SpkMute(on);
}
#else
int board_audio_get_pa_mute_pin(void)
{
    return GPIO_SPKEN_NUM;
}
#endif

#endif
