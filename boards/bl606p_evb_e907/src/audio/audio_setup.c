/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO
#include <stdint.h>
#include <devices/drv_snd_bl606p.h>
#include <alsa/mixer.h>

#include "eq_config.h"
#include "board_audio.h"
#include <yoc/partition.h>

/* 默认采集增益 */
static int g_audio_in_gain[]  = { AUIDO_IN_GAIN_MIC, AUIDO_IN_GAIN_MIC, AUIDO_IN_GAIN_REF };
static int g_audio_out_gain[] = { AUIDO_OUT_GAIN, AUIDO_OUT_GAIN };

/*采集增益配置*/
extern int _aui_analog_gain(int id, int32_t val);

int board_audio_in_set_gain(int id, int gain)
{
    if (id < 0 || id >= sizeof(g_audio_in_gain) / sizeof(int)) {
        return -1;
    }
    _aui_analog_gain(id, gain);
    g_audio_in_gain[id] = gain;
    return 0;
}

int board_audio_in_get_gain(int id)
{
    if (id < 0 || id >= sizeof(g_audio_in_gain) / sizeof(int)) {
        return -1;
    }
    return g_audio_in_gain[id];
}

/*播放增益配置*/
extern int auo_analog_gain(void *context, float val);
int board_audio_out_set_gain(int id, int gain)
{
    //预留2作为全部通道一起配置
    if (id < 0 || id > sizeof(g_audio_out_gain) / sizeof(int)) {
        return -1;
    }

    //设置播放模拟增益
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

    return 0;
}

int board_audio_out_get_gain(int id)
{
    // if (id < 0 || id >= sizeof(g_audio_out_gain) / sizeof(int)) {
    //     return -1;
    // }

    return g_audio_out_gain[0];
}

#ifdef CONFIG_SMART_AUDIO
extern audio_vol_config_t g_vol_config;

audio_vol_config_t *board_audio_out_get_vol_config()
{
    return &g_vol_config;
}

/*EQ参数配置*/
extern int auo_eq_set(uint32_t *eq_filt_coef, int bytes);
extern int auo_eq_show(void);

int board_eq_set_param(void *data, int byte)
{
    if (data == NULL) {
        /*设置默认参数*/
        auo_eq_set((uint32_t *)eq_filt_coef, sizeof(eq_filt_coef));
        return 0;
    }
    return auo_eq_set((uint32_t *)data, byte);
}

int board_eq_show_param(void)
{
    return auo_eq_show();
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

extern int auo_drc_set(uint32_t *drc_filt_coef, int bytes);
int board_drc_set_param(void *data, int byte)
{
    if (data == NULL) {
        /*设置默认参数*/
        auo_drc_set((uint32_t *)drc_filt_coef, sizeof(drc_filt_coef));
        return 0;
    }

    return auo_drc_set((uint32_t *)data, byte);
}
#endif /*CONFIG_SMART_AUDIO*/

/*PA控制引脚*/
int board_audio_get_pa_mute_pin(void)
{
    return AUIDO_PA_MUTE_PIN;
}


static int audio_fw_load(void)
{
#if defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW
    partition_t partition;
    unsigned long run_address;
    const char *name = "prim";

    partition = partition_open(name);
    if (partition_split_and_get(partition, 1, NULL, NULL, &run_address)) {
        printf("get [%s] run_address failed.\n", name);
        return -1;
    }
    partition_close(partition);
    return board_load_amp_fw(0, (void*)run_address, NULL, 0);
# else
    return -1;
#endif
}

void board_audio_init(void)
{
    snd_bl606p_config_t snd_config;
    snd_config.audio_in_gain_list[0] = board_audio_in_get_gain(0);
    snd_config.audio_in_gain_list[1] = board_audio_in_get_gain(1);
    snd_config.audio_in_gain_list[2] = board_audio_in_get_gain(2);
    snd_config.audio_out_gain_list[0] = board_audio_out_get_gain(0);
    snd_config.audio_out_gain_list[1] = board_audio_out_get_gain(1);

    snd_card_bl606p_register(&snd_config);

#ifdef CONFIG_SMART_AUDIO
    /* 播放器解码单路输出 */
    extern void av_set_ao_channel_num(int num);
    av_set_ao_channel_num(1);
#endif

    audio_fw_load();

}
#endif
