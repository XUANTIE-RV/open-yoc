/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#if !(defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <aos/aos.h>
#include <ulog/ulog.h>

#include "mit/debug_mode.h"
#include "mit/mit_rtos.h"
#ifdef CONFIG_VAD_ENABLED
#include "mit/vad_mit.h"
#endif
#include "alg_audio.h"

#define TAG "KWSMinD"

#define MAX_KWS_DATA_LEN   0xC000 // 48 K
#define KWS_DATA_FRAME_LEN (20 * 2 * 16)
#define MAX_VAD_DATA_LEN   (80 * 2 * 16)                // 80 ms pcm, this has to be longer than 20 ms processing slice
#define MAX_VAD_TAG_LEN    (MAX_VAD_DATA_LEN / 10 / 32) // vad tag for each 10 ms pcm
#define MAX_ASR_RESULT_LEN (4096)

static short g_aec_data_out[KWS_DATA_FRAME_LEN]; //算法处理后的数据，2路，默认未交织格式
static char  g_kws_data[MAX_KWS_DATA_LEN] __attribute__((aligned(16))); /* 关键词音频数据 */
static char  g_vad_data[MAX_VAD_DATA_LEN] __attribute__((aligned(16))); /* 上云数据 */
static short g_vad_data_tag[MAX_VAD_TAG_LEN];
static int   g_vad_data_len;
static int   g_vad_data_should_ep = 0;
static char  g_asr_result[MAX_ASR_RESULT_LEN];

//定义存放20ms 2通道数据的全局数组,AEC线性滤波后AGC前多通道数据
#ifdef CONFIG_FEOUTAEC
short g_debug_test_feout_aec_buffer[320*2];//2CH, 20ms 
short g_debug_test_feout_aec_buffer_alternative[320*2];//2CH, 20ms //用于进行交织非交织转换
static int g_linear_aec_data_en_status = 1; /* 线性AEC的使能状态, 0:非使能 1：使能(默认) */
#endif

static mit_rtos_listener g_nuithings_listener;
static MitRtosConfig     g_mit_rtos_config; // mit_rtos 初始化配置参数
static int               wakeup_audio_playing_before, wakeup_audio_playing;
static int               wakeup_audio_hw_delay, wakeup_audio_buf_delay;

#ifdef CONFIG_VAD_ENABLED
static vad_init_config   g_vad_init_config; // 单路vad 初始化配置参数
static vad_process_data  g_vad_process_data; // vad输入参数
static VADHANDLE         vad_handle = NULL; // vad引擎指针
#endif

static int mit_rtos_data_get_main(void *user_data, char *buffer, int len)
{
    int ret = len;

    return ret;
}

static int mit_rtos_event_main(void *user_data, mit_rtos_event_t event, int dialog_finish)
{
    // LOGD(TAG, "mit_rtos_event_main %d %s", event, (char *)user_data);
    // switch (event)
    // {
    // case MIT_RTOS_EVENT_ASRPARTIALRESULT:
    //     /* code */
    //     break;
    // case MIT_RTOS_EVENT_ASRRESULT:
    //     /* code */
    //     strlcpy(g_asr_result, (char *)user_data, MAX_ASR_RESULT_LEN);
    //     break;
    // case MIT_RTOS_EVENT_ASRRERROR:
    //     /* code */
    //     break;
    // default:
    //     break;
    // }
    return 0;
}

static int mit_rtos_data_out(void *user_data, mit_rtos_event_t event, char *buffer, int len)
{
    return 0;
}

void audio_alginit(void)
{

#if defined(CONFIG_CHIP_BL606P) && CONFIG_CHIP_BL606P ||  \
    defined(CONFIG_CHIP_LIGHT) && CONFIG_CHIP_LIGHT
    int ret = 0;

    // app启动，执行初始化等操作。
    g_nuithings_listener.on_event_callback  = mit_rtos_event_main;
    g_nuithings_listener.need_data_callback = mit_rtos_data_get_main;
    g_nuithings_listener.put_data_callback  = mit_rtos_data_out;
    g_nuithings_listener.user_data          = NULL;

    g_mit_rtos_config.listener = &g_nuithings_listener;
    g_mit_rtos_config.alg_type = kMitRtosAlgBSS;

    g_mit_rtos_config.task_enable                = 0;
    g_mit_rtos_config.kws_alwayson_enable        = 1;
    g_mit_rtos_config.fe_enable                  = 1;
    g_mit_rtos_config.kws_enable                 = 1;
    g_mit_rtos_config.vad_enable                 = 0;
    g_mit_rtos_config.vad_endpoint_ignore_enable = 0;
    g_mit_rtos_config.need_data_after_vad        = 1;
    g_mit_rtos_config.wwv_enable                 = 2; // enable wwv or not.
    if (g_mit_rtos_config.wwv_enable == 2) {
      g_mit_rtos_config.local_threshold = -100;
      g_mit_rtos_config.wwv_threshold = -100;
    }

    g_mit_rtos_config.voice_filter_type = kMitRtosVoiceFilterTypeDisable;
    // g_mit_rtos_config.log_in_asr_enable = 1;
    g_mit_rtos_config.is_interleave = 1;

    g_mit_rtos_config.vad_endpoint_delay = 600;
    g_mit_rtos_config.vad_silencetimeout = 10000;
    g_mit_rtos_config.vad_voicetimeout   = 10000;
    g_mit_rtos_config.vad_kws_strategy   = 0;

    g_mit_rtos_config.kws_thres.vad_speech_noise_thres_sp = -0.2;
    g_mit_rtos_config.kws_thres.vad_speech_noise_thres_ep = -0.1;
    g_mit_rtos_config.asr_thres.vad_speech_noise_thres_sp = -0.2;
    g_mit_rtos_config.asr_thres.vad_speech_noise_thres_ep = -0.1;

    g_mit_rtos_config.enable_individual_vad4asr = 1; // enable process of individula_vad for asr function.
    g_mit_rtos_config.vad4asr_enable            = 1; //是否使能ASR专用VAD子模块
    g_mit_rtos_config.fe_tune.mic_num           = 2;
    g_mit_rtos_config.fe_tune.ref_num           = 1;
    g_mit_rtos_config.fe_tune.out_num           = 2;

    g_mit_rtos_config.gain_tune_db       = -100; // gain: 0dB
    g_mit_rtos_config.fe_tune.agc_enable = -1;   // agc enable
    // g_mit_rtos_config.fe_tune.agc_gain = 4.0f;
    g_mit_rtos_config.fe_tune.agc_power = 8;
    g_mit_rtos_config.fe_tune.agc_level = 4;
    // g_mit_rtos_config.add_kwsdetect_enable = 0;

    g_mit_rtos_config.enable_init_more_log = 2;
    // g_mit_rtos_config.localasr_enable = 1;

    // g_silence_log_level = 3;

#ifdef CONFIG_VAD_ENABLED
    mit_rtos_vad_config_default(&g_vad_init_config);
    g_vad_init_config.ch_index = 20;
    g_vad_init_config.pmodel = 0;
    g_vad_init_config.vad_endpoint_delay = 600;
    g_vad_init_config.vad_silencetimeout = 10000;
    g_vad_init_config.vad_voicetimeout = 30000;
	g_vad_init_config.vad_speech_noise_thres_sp = -0.2;
	g_vad_init_config.vad_speech_noise_thres_ep = -0.1;
    g_vad_init_config.fifo_disable = 1;
    g_vad_init_config.micnum = -1;
    g_vad_init_config.refnum = -1;
    g_vad_init_config.delay_out2ref = 60;
#endif

    ret = mit_rtos_init(&g_mit_rtos_config);
    // g_log_enable = 1;   // 底层算法log打印
    LOGD(TAG, "call mit_rtos_init() return %d\n", ret);
#ifdef CONFIG_VAD_ENABLED
    vad_handle = mit_rtos_vad_init(&g_vad_init_config);
    //mit_rtos_vad_debug_set_mode(vad_handle, 2<<4);
    if (NULL == vad_handle) {
      LOGE(TAG, "vad handle is null!!");
      return;
    }
#endif
    ret = mit_rtos_start();
    LOGD(TAG, "call mit_rtos_start() return %d\n", ret);
    // disabled debug info
    mit_rtos_debug_set_mode(0x00);

#ifdef CONFIG_FEOUTAEC
    //通过接口进行存储地址设置，同时开启次功能
    MitRtosSdkOfflineTest g_debug_test_feout_aec;
    g_debug_test_feout_aec.buffer = (char *)g_debug_test_feout_aec_buffer;
    g_debug_test_feout_aec.leng_byte =sizeof(g_debug_test_feout_aec_buffer);
    mit_rtos_set_param_str(kMitRtosParamFEOutAEC, (char *)&g_debug_test_feout_aec);
#endif

#else
    int ret = 0;

    // app启动，执行初始化等操作。
    g_nuithings_listener.on_event_callback  = mit_rtos_event_main;
    g_nuithings_listener.need_data_callback = mit_rtos_data_get_main;
    g_nuithings_listener.put_data_callback  = mit_rtos_data_out;
    g_nuithings_listener.user_data          = NULL;

    g_mit_rtos_config.listener                   = &g_nuithings_listener;
    g_mit_rtos_config.alg_type                   = kMitRtosAlgBSSOpt;
    g_mit_rtos_config.task_enable                = 0;
    g_mit_rtos_config.kws_alwayson_enable        = 1;
    g_mit_rtos_config.fe_enable                  = 1;
    g_mit_rtos_config.kws_enable                 = 1;
    g_mit_rtos_config.vad_enable                 = 1;
    g_mit_rtos_config.vad_endpoint_ignore_enable = 0;
    g_mit_rtos_config.need_data_after_vad        = 1;
    g_mit_rtos_config.wwv_enable                 = 0; // enable wwv or not.
    /*if (2==g_mit_rtos_config.wwv_enable) {
      g_mit_rtos_config.local_threshold = 20;
      g_mit_rtos_config.wwv_threshold = 10;
    }*/

    g_mit_rtos_config.voice_filter_type = kMitRtosVoiceFilterTypeDisable;
    // g_mit_rtos_config.log_in_asr_enable = 1;
    g_mit_rtos_config.is_interleave = 1;

    g_mit_rtos_config.vad_endpoint_delay = 1000;
    g_mit_rtos_config.vad_silencetimeout = 8000;
    g_mit_rtos_config.vad_voicetimeout   = 10000;
    g_mit_rtos_config.vad_kws_strategy   = 0;

    g_mit_rtos_config.kws_thres.vad_speech_noise_thres_sp = -0.2;
    g_mit_rtos_config.kws_thres.vad_speech_noise_thres_ep = -0.1;
    g_mit_rtos_config.asr_thres.vad_speech_noise_thres_sp = -0.2;
    g_mit_rtos_config.asr_thres.vad_speech_noise_thres_ep = -0.1;

    g_mit_rtos_config.enable_individual_vad4asr = 1; // enable process of individula_vad for asr function.
    g_mit_rtos_config.vad4asr_enable            = 1; //是否使能ASR专用VAD子模块
    g_mit_rtos_config.fe_tune.mic_num           = 2;
    g_mit_rtos_config.fe_tune.ref_num           = 1;
    g_mit_rtos_config.fe_tune.out_num           = 2;

    g_mit_rtos_config.gain_tune_db       = 12; // gain: 12dB
    g_mit_rtos_config.fe_tune.agc_enable = 1;  // agc enable
    // g_mit_rtos_config.fe_tune.agc_gain = 4.0f;
    g_mit_rtos_config.fe_tune.agc_power = 8;
    g_mit_rtos_config.fe_tune.agc_level = 4;
    // g_mit_rtos_config.add_kwsdetect_enable = 0;

    g_mit_rtos_config.enable_init_more_log = 2;

    // g_silence_log_level = 3;
    ret = mit_rtos_init(&g_mit_rtos_config);
    // g_log_enable = 1;   // 底层算法log打印
    LOGD(TAG, "call mit_rtos_init() return %d\n", ret);
    ret = mit_rtos_start();
    LOGD(TAG, "call mit_rtos_start() return %d\n", ret);
    // disabled debug info
    mit_rtos_debug_set_mode(0x00);

#endif
}

void audio_wakeup_voice_stat(int stat, int buf_delay, int hw_delay)
{
    wakeup_audio_playing   = stat;
    wakeup_audio_hw_delay  = hw_delay;
    wakeup_audio_buf_delay = buf_delay;
}

void audio_set_wakeup_level(const char *wakeup_word, int level)
{
#if defined(CONFIG_CHIP_BL606P) && CONFIG_CHIP_BL606P ||  \
    defined(CONFIG_CHIP_LIGHT) && CONFIG_CHIP_LIGHT
    int                    kws_count = mit_rtos_get_keyword_count();
    MitRtosSdkKeywordList *kws_list  = mit_rtos_get_keyword_list();
    LOGD(TAG, "kws_count=%d, kws_list=%p", kws_count, kws_list);

    if (kws_list && kws_count > 0) {
        for (int i = 0; i < kws_count; i++) {
            LOGD(TAG,
                 "id=%d type=%d word=%s en=%d level=%.3f level_dnd=%.3f",
                 kws_list[i].index,
                 kws_list[i].keyword_type,
                 kws_list[i].word,
                 kws_list[i].enable,
                 kws_list[i].threshold,
                 kws_list[i].threshold_dnd);

            if (strcmp(wakeup_word, kws_list[i].word) == 0) {
                kws_list[i].threshold = level;
                LOGD(TAG, "set word(%s) level=%d", wakeup_word, level);
            }
        }
        mit_rtos_set_keyword_enable(kws_list, kws_count);
    } else {
        LOGE(TAG, "get list error");
    }
#endif
}

static mit_rtos_voice_data g_voice_data;
static MitRtosAudioResult  audio_result_main;
static int                 should_check_asr_data_finish = 0;

static void mit_status_update(void)
{
    int ret              = -1;
    int asr_data_is_null = 0; // the asr_data is null of this get

    int au_status = audio_get_status();
    if (au_status == AU_STATUS_READY || au_status == AU_STATUS_FINI) {
        return;
    }

    g_voice_data.mode                        = kMitRtosVoiceModeAsr;
    g_voice_data.available                   = kVoiceDataInvalid;
    g_voice_data.data                        = g_vad_data;
    g_voice_data.data_length                 = MAX_VAD_DATA_LEN;
    g_voice_data.data_framestate             = g_vad_data_tag;
    g_voice_data.data_framestate_length_byte = sizeof(g_vad_data_tag);

    ret = mit_rtos_get_voice(&g_voice_data);
    //    LOGE(TAG, "%d %d", ret, g_voice_data.data_length);
    if (0 == ret) {
        g_vad_data_len = g_voice_data.data_length;
        if (g_vad_data_len <= 0) {
            asr_data_is_null = 1;
            if (1 == should_check_asr_data_finish) {
                should_check_asr_data_finish = 0;
                asr_data_is_null             = 0;

                g_vad_data_should_ep = 1;
                if (au_status != AU_STATUS_WAKE) {
                    audio_set_status(AU_STATUS_FINI);
                }
            }
        } else {
            if (g_voice_data.data_framestate_length_byte / 2 != g_vad_data_len / 10 / 32) {
                // LOGE(TAG, "frame tag len error(%d VS %d)",g_voice_data.data_framestate_length_byte / 2,
                // g_vad_data_len / 10 / 32);
            }
        }
    } else {
        LOGE(TAG, "ASR data ret %d", ret);
    }

    if (audio_result_main.vad_state >= 0) {
        switch (audio_result_main.vad_state) {
            case kMitRotsVadResultStartPoint:
                // do nothing
                break;
            case kMitRotsVadResultEndPoint:
                // nls end
                if (1 == asr_data_is_null) {
                    // endpoint and asr data is null. so stop this dialog
                    g_vad_data_should_ep = 1;
                    if (au_status != AU_STATUS_WAKE) {
                        audio_set_status(AU_STATUS_FINI);
                    }
                } else {
                    // start the check of data_end
                    should_check_asr_data_finish = 1;
                }
                break;
            case kMitRotsVadResultTimeout:
                // nls cancel with err
                if (au_status != AU_STATUS_WAKE) {
                    audio_set_status(AU_STATUS_FINI);
                }
                break;
            default:
                LOGE(TAG, "user vad_state(%d) error", audio_result_main.vad_state);
                break;
        }
    }
}

audio_wakeup_info_t audio_process(short *data_in, int sample_count, short **out, size_t *out_len)
{
    audio_wakeup_info_t wakeup_info     = { 0, NULL, 0, 0, 0};
    int                 output_data_len = 0;

    /* 输出两路数据的字节数 */
    output_data_len = sample_count * sizeof(short) * 2;

    /* 通知算法裁剪唤醒提示音 */
    if (wakeup_audio_playing_before != wakeup_audio_playing) {
        mit_rtos_set_playstate(
            wakeup_audio_playing ? kMitRtosPlayOn : kMitRtosPlayOff, wakeup_audio_hw_delay, wakeup_audio_buf_delay);
        LOGD(TAG,
             "set play status=%d hwdelay=%d audiodelay=%d",
             wakeup_audio_playing,
             wakeup_audio_hw_delay,
             wakeup_audio_buf_delay);
        wakeup_audio_playing_before = wakeup_audio_playing;
    }

#ifdef CONFIG_FEOUTAEC
    //每次process前对缓存buffer进行memset，
    //process完成后，获取2通道 AEC线性滤波后AGC前的数据
    memset(g_debug_test_feout_aec_buffer, 0, sizeof(g_debug_test_feout_aec_buffer));
    memset(g_debug_test_feout_aec_buffer_alternative, 0, sizeof(g_debug_test_feout_aec_buffer_alternative));
#endif

    int ret = mit_rtos_update_audio((char *)data_in,
                                    sample_count * 3 * sizeof(short),
                                    (char *)g_aec_data_out,
                                    &output_data_len,
                                    &audio_result_main);

    /* 唤醒处理 */
    if (ret > 10) {
        //命令词命中
        wakeup_info.wakeup_type = AUDIO_WAKEUP_TYPE_CMD;
        LOGD(TAG, "command word ret=%d", ret);
    } else if (ret & 0x01) {
        //主唤醒词唤醒成功。可以获取唤醒附加消息，但二次确认数据还未准备完毕
        audio_set_status(AU_STATUS_WAKE);
        if (audio_result_main.data_kws_count > 0) {
            LOGI(TAG, "kws is %s do_wwv=%d ret=%d", audio_result_main.data_kws[0].word, audio_result_main.data_kws[0].do_wwv, ret);
            wakeup_info.wakeup_type = AUDIO_WAKEUP_TYPE_KEY;
            wakeup_info.kws_score   = audio_result_main.data_kws[0].confidence;
            wakeup_info.kws_word    = audio_result_main.data_kws[0].word;

            /* 关闭唤醒时刻获取DOA, 采用外部主动来获取，即解析到云端有需要的时候再获取 */
            //mit_rtos_get_param_int(kMitRtosParamDoa ,&wakeup_info.kws_doa);
        } else {
            LOGE(TAG, "wakeup, but data_kws_count <=0");
        }
    } else if (ret & 0x02) {
        //主唤醒词二次确认数据准备完毕
    } else {
        ;
    }

    /* 从算法读取处理后数据 */
    mit_status_update();

    /* VAD 检测 */
#ifdef CONFIG_VAD_ENABLED
    if (g_voice_data.data_length >= 640) {
        int count = g_voice_data.data_length / 640;
        int vad_statu = -1;
        for (int i = 0; i < count; i++) {
            g_vad_process_data.length = 640;
            g_vad_process_data.data   = g_voice_data.data + i * 640;
            vad_statu                 = mit_rtos_vad_detect_more(vad_handle, &g_vad_process_data);
            switch (vad_statu) {
                case kVadDetectResultSilence:
                    break;
                case kVadDetectResultStartpoint:
                    //LOGE(TAG, "vad_statu kVadDetectResultStartpoint");
                    wakeup_info.vad_st = 1;
                    break;
                case kVadDetectResultVoice:
                    break;
                case kVadDetectResultEndpoint:
                    //LOGE(TAG, "vad_statu kVadDetectResultEndpoint");
                    wakeup_info.vad_st = 2;
                    break;
                case kVadDetectResultSilenceTimeout:
                    //LOGE(TAG, "vad_statu kVadDetectResultSilenceTimeout");
                    wakeup_info.vad_st = 3;
                    break;
                case kVadDetectResultVoiceTimeout:
                    //LOGE(TAG, "vad_statu kVadDetectResultVoiceTimeout");
                    wakeup_info.vad_st = 4;
                    break;
                default:
                    break;
            }
        }
    }
#endif

    /* 算法后数据输出 */
#ifdef CONFIG_FEOUTAEC
    //转换成交织数据
    if (g_linear_aec_data_en_status) {
        for (int ii = 0; ii < sample_count; ii++){
            g_debug_test_feout_aec_buffer_alternative[2 * ii] = g_debug_test_feout_aec_buffer[ii];
            g_debug_test_feout_aec_buffer_alternative[2 * ii + 1] = g_debug_test_feout_aec_buffer[sample_count + ii];
        }
        memcpy(*out, g_debug_test_feout_aec_buffer_alternative, output_data_len);
        *out_len = (size_t)output_data_len;
    } else {
        memcpy(*out, g_aec_data_out, output_data_len);
        *out_len = (size_t)output_data_len;
    }
#else
    memcpy(*out, g_aec_data_out, output_data_len);
    *out_len = (size_t)output_data_len;
#endif

    return wakeup_info;
}

static int g_audio_status = AU_STATUS_READY;
int        audio_get_status()
{
    return g_audio_status;
}

int audio_set_status(int status)
{
    g_audio_status = status;
    return 0;
}

int audio_get_vad()
{
    /* 适配接口定义与该枚举一致 */
    return audio_result_main.vad_state;
}

int audio_get_vad_data(char **vad_data, int *data_len, int *finished)
{
    *data_len      = g_vad_data_len;
    g_vad_data_len = 0;

    *vad_data  = g_vad_data;

    *finished = g_vad_data_should_ep;
    if (1 == g_vad_data_should_ep) {
        g_vad_data_should_ep = 0;
        LOGD(TAG, "audio_get_vad_data all finished");
    }

    return *data_len == 0 ? -1 : 0;
}

#if defined(CONFIG_VAD_TAG) && CONFIG_VAD_TAG
int audio_get_tagged_vad_data(char **vad_data, int *data_len, short **frame_tag, int *finished)
{
    *data_len      = g_vad_data_len;
    g_vad_data_len = 0;

    *vad_data  = g_vad_data;
    *frame_tag = g_vad_data_tag;

    *finished = g_vad_data_should_ep;
    if (1 == g_vad_data_should_ep) {
        g_vad_data_should_ep = 0;
        LOGD(TAG, "audio_get_vad_data all finished");
    }

    return *data_len == 0 ? -1 : 0;
}
#endif

char *audio_get_kws_data(int *data_len)
{
    char *kws_data = NULL;

    g_voice_data.mode        = kMitRtosVoiceModeKws;
    g_voice_data.available   = kVoiceDataInvalid;
    g_voice_data.data        = g_kws_data;
    g_voice_data.data_end    = 0;
    g_voice_data.data_length = MAX_KWS_DATA_LEN;

    int ret = mit_rtos_get_voice(&g_voice_data);
    if (ret == 0 && g_voice_data.data_length > 0) {
        *data_len = g_voice_data.data_length;
        kws_data  = g_kws_data;
    }

    return kws_data;
}

int audio_set_debug_level(unsigned int mode)
{
    mit_rtos_debug_set_mode(mode);
    return 0;
}

int audio_set_p2t_mode(int mode)
{
    LOGD(TAG, "set asr mode %d", mode);

    mit_rtos_stop();
    switch(mode) {
        case 0:
            mit_rtos_start();
            break;
        case 1:
            mit_rtos_start_mode(kMitRotsModeP2tKws); //默认的p2t模式
            break;
        case 2:
            mit_rtos_start_mode(kMitRotsModeP2t);
            break;
        case 4:
            mit_rtos_start_mode(kMitRotsModeVadKws);
            break;
        case 5:
           mit_rtos_start_mode(kMitRotsModeVad);
            break;
        default: 
            mit_rtos_start_mode(kMitRotsModeKws);
    }

    return 0;
}

int audio_get_doa()
{
    int doa = 361;
    mit_rtos_get_param_int(kMitRtosParamDoa ,&doa);
    return doa;
}

//{"header":{"namespace":"CMDModel","name":"AsrPartialReulst"},"payload":{"result":"da kai kong tiao"}}
//{"header":{"namespace":"CMDModel","name":"AsrResult"},"payload":{"result":"guan bi kong tiao"}}
//{"header":{"namespace":"CMDModel","name":"AsrResult"},"payload":{"result":"san shi du"}}
//{"header":{"namespace":"CMDModel","name":"AsrResult"},"payload":{"result":"shen gao wen du"}}
//{"header":{"namespace":"CMDModel","name":"AsrPartialReulst"},"payload":{"result":"jiang di wen du"}}
//const char dakaikongtiao[]="{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\"},\"payload\":{\"asrresult\":\"打开空调\",\"semantics\":[{\"domain\":\"acc\",\"intent\":\"ctl_acc_power_ctrl_on\"}]}}";
//const char guanbikongtiao[]="{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\"},\"payload\":{\"asrresult\":\"关闭空调\",\"semantics\":[{\"domain\":\"acc\",\"intent\":\"ctl_acc_power_ctrl_off\"}]}}";
//const char sanshidu[]="{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\"},\"payload\":{\"asrresult\":\"三十度\",\"semantics\":[{\"domain\":\"acc\",\"intent\":\"ctl_acc_temp_set\",\"slots\":{\"number\":[{\"value\":30,\"offset\":0}]}}]}}";
//const char shengaowendu[]="{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\"},\"payload\":{\"asrresult\":\"升高温度\",\"semantics\":[{\"domain\":\"acc\",\"intent\":\"ctl_acc_temp_set_increase\"}]}}";
//const char jiangdiwendu[]="{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\"},\"payload\":{\"asrresult\":\"降低温度\",\"semantics\":[{\"domain\":\"acc\",\"intent\":\"ctl_acc_temp_set_decrease\"}]}}";

const char *audio_get_asr_result()
{
    if (g_asr_result[0] == 0) {
        return NULL;
    }
/*
    if (strstr(g_asr_result, "开 空 调")) {
        return dakaikongtiao;
    } else if (strstr(g_asr_result, "闭 空 调")) {
        return guanbikongtiao;
    } else if (strstr(g_asr_result, "三 十 度")) {
        return sanshidu;
    } else if (strstr(g_asr_result, "升 高 温 度")) {
        return shengaowendu;
    } else if (strstr(g_asr_result, "降 低 温 度")) {
        return jiangdiwendu;
    }
*/
    return g_asr_result;
}

void audio_clear_asr_result()
{
    g_asr_result[0] = 0;
}

#ifdef CONFIG_FEOUTAEC
void audio_set_linear_aec_en_status(int status)
{
    if (status > 0) {
        g_linear_aec_data_en_status = 1;
    } else {
        g_linear_aec_data_en_status = 0;
    }
}

int audio_get_linear_aec_en_status(void)
{
    return g_linear_aec_data_en_status;
}
#endif

#endif