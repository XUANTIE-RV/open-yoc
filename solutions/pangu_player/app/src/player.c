/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <avutil/common.h>
#include <avutil/misc.h>
#include <player.h>
#include "avutil/socket_rw.h"
#include "avutil/web.h"
#include "avutil/named_straightfifo.h"
//#include "aoicore.h"
//#include "AudioVolume.h"
// #include "fft_tmall.h"
#include "audio/pingfan16k.c"
#include "sona_aef_config.h"
//#include "result_ac2_mp3.h"
//#include "result_flac.h"
//#include "result_ac2_flac.h"
#include "fft_test_bin.h"
//#include "result_aac_fast_mp4.h"
//#include "test_m4a.h"
//#include "test_m4a_s30.h"
//#include "result_wma_30s.h"
//#include "result_mp2_20s.h"
//#include "chengdu_mp4_30s.h"
//#include "hello_wav.h"
//#include "result_cenc.h"
#include "result_enc_mp3.h"
//#include "result_amrnb_30s.h"
//#include "result_amrwb_30s.h"
//#include "result_f32le_wav_30s.h"
//#include "result_f64le_wav_15s.h"
//#include "result_adpcm_ms.h"
//#include "result_adpcm_ms_ch2.h"
// #include "audio/test2k.wav.c"
// #include "audio/80Hz_0dB_single.wav.c"
// #include "audio/SPK_1K_0dB.wav.c"
//
#include "mca/cmca.h"
#include "mca/mca_all.h"

#define EQ_SEGMENT 8
eqfp_t bqfparams[EQ_SEGMENT] = {
    {1, EQF_TYPE_PEAK,  3.0, 0.667, 100},
    {1, EQF_TYPE_PEAK, -13.0, 0.667, 200 },
    {1, EQF_TYPE_PEAK, 3.0, 0.667, 1000 }
};

#define TAG "ap"

static player_t* g_player;
static nsfifo_t* g_tts_fifo;
static char* g_url;

typedef struct {
    float cf;     /* 中心频率 */
    float gain;   /* 段增益 */
    float qvalue; /* 宽广度 */
} eq_filter_setting_t;

static void _eq_set_params(char *eq_params)
{
    int rc;
    if (strlen(eq_params) < 5) {
        printf("eqparam command format: ai eqparam \"{30, -10, 1.0} {100, -10, 3.0}\"\n");
        return;
    }

    char *start = eq_params, *end;
    char *end_of_str = eq_params + strlen(eq_params);
    eq_filter_setting_t filter_params[EQ_SEGMENT];
    char snum[32];
    int param_index = 0;
    int err = 0;

    do {
        end = strchr(start, '}');
        start = strchr(start, '{');

        if (start != NULL && end != NULL) {
            start++;

            char *delim = start;
            int i = 0;      // 3 params, fc/gain/qvalue
            float *pparam = &filter_params[param_index].cf;

            while (start < end && i < 3) {
                while (*delim != ',' && delim < end) delim++;
                if (delim-start > 31 || delim-start == 0) {
                    err = 1;
                    break;
                }
                memcpy(snum, start, delim-start);
                snum[delim-start] = '\0';

                float m = atof(snum);
                pparam[i++] = m;

                start = ++delim;
            }

            if (i != 3) {
                err = 3;
                break;
            }
            param_index++;

            start = end + 1;
        }
    } while (start < end_of_str && start != NULL && end != NULL && !err && param_index < EQ_SEGMENT);

    if (err || start == NULL || end == NULL) {
        printf("eqparam command format: player eqparam \"{30, -10, 1.0} {100, -10, 3.0}\"\n");
    } else {
        //printf("eq set params successfully:\n");
        peq_setpa_t eqpa;
        for (int i = 0; i < param_index; ++i) {
            eqpa.segid        = i;
            eqpa.param.enable = 1;
            eqpa.param.type   = EQF_TYPE_PEAK;
            eqpa.param.gain   = filter_params[i].gain;
            eqpa.param.q      = filter_params[i].qvalue;
            eqpa.param.rate   = filter_params[i].cf;
            rc = player_ioctl(g_player, PLAYER_CMD_EQ_SET_PARAM, &eqpa);
            if (rc < 0) {
                printf("err param: %5d=> {%f, %f, %f}\n", i, filter_params[i].cf, filter_params[i].gain, filter_params[i].qvalue);
            } else {
                memcpy((void*)&bqfparams[i], &eqpa.param, sizeof(eqfp_t));
            }
        }
    }
}

static void _player_event(player_t *player, uint8_t type, const void *data, uint32_t len)
{
    int rc;
    UNUSED(len);
    UNUSED(data);
    UNUSED(handle);
    LOGD(TAG, "=====%s, %d, type = %d", __FUNCTION__, __LINE__, type);

    switch (type) {
    case PLAYER_EVENT_ERROR:
        rc = player_stop(player);
        break;

    case PLAYER_EVENT_START: {
        media_info_t minfo;
        memset(&minfo, 0, sizeof(media_info_t));
        rc = player_get_media_info(player, &minfo);
        LOGD(TAG, "=====rc = %d, duration = %llums, bps = %llu, size = %u", rc, minfo.duration, minfo.bps, minfo.size);
        break;
    }

    case PLAYER_EVENT_FINISH:
        player_stop(player);
        break;

    default:
        break;
    }
}

int _player(const char *url)
{
    int rc;

    if (NULL != g_player) {
        rc = player_play(g_player, url, 0);
        LOGD(TAG, "%s, %d, rc = %d. play", __FUNCTION__, __LINE__, rc);
    }

    return 0;
}

int _player_stop()
{
    if (NULL != g_player) {
        LOGD(TAG, "==>%s, %d", __FUNCTION__, __LINE__);
        player_stop(g_player);
    }

    return 0;
}

int _player_pause()
{
    if (NULL != g_player) {
        player_pause(g_player);
    }

    return 0;
}

int _player_resume()
{
    if (NULL != g_player) {
        player_resume(g_player);
    }

    return 0;
}

int _player_get_vol()
{
    int rc;
    if (NULL != g_player) {
        uint8_t vol;
        rc = player_get_vol(g_player, &vol);
        LOGD(TAG, "==>%s, %d, rc = %d, vol = %u", __FUNCTION__, __LINE__, rc, vol);
    }

    return 0;
}

int _player_set_vol(uint8_t vol)
{
    int rc;
    if (NULL != g_player) {
        rc = player_set_vol(g_player, vol);
        LOGD(TAG, "==>%s, %d, rc = %d, vol = %u", __FUNCTION__, __LINE__, rc, vol);
    }

    return 0;
}
//extern uint32_t g_pcm_data[];
//extern uint32_t g_pcm_len;

static void _ptask(void *arg)
{
    int fd;
    int cnt = 0, rc, wlen;
    char *val, *pos;
    uint8_t reof = 0;
    wsession_t *session;
    session = wsession_create();

    rc = wsession_get(session, "http://www.srcbin.net/ai/result.mp3", 3);
    if (rc) {
        LOGE(TAG, "wsession_get fail. rc = %d, code = %d, phrase = %s", rc, session->code, session->phrase);
        goto err;
    }

    val = (char*)dict_get_val(&session->hdrs, "Content-Length");
    CHECK_RET_TAG_WITH_GOTO(val != NULL, err);
    fd = session->fd;
    LOGD(TAG, "content len = %d", atoi(val));

    for (;;) {
        wlen = nsfifo_get_wpos(g_tts_fifo, &pos, 8*1000);
        nsfifo_get_eof(g_tts_fifo, &reof, NULL);
        if (wlen <= 0 || reof) {
            LOGE(TAG, "get wpos err. wlen = %d, reof = %d", wlen, reof);
            break;
        }
        rc = sock_readn(fd, pos, wlen, 6*1000);
        if (rc <= 0) {
            LOGE(TAG, "readn err. rc = %d", rc);
            break;
        }
        nsfifo_set_wpos(g_tts_fifo, rc);
        cnt += rc;
    }
    LOGD(TAG, "rc = %8d, cnt = %8d", rc, cnt);

err:
    wsession_destroy(session);
    return;
}

static void _webtask(void *arg)
{
    int cnt = 0, rc;
    char *val, buf[1024];
    wsession_t *session;
    session = wsession_create();

    rc = wsession_get(session, g_url, 3);
    if (rc) {
        LOGE(TAG, "wsession_get fail. rc = %d, code = %d, phrase = %s", rc, session->code, session->phrase);
        goto err;
    }

    val = (char*)dict_get_val(&session->hdrs, "Content-Length");
    CHECK_RET_TAG_WITH_GOTO(val != NULL, err);
    LOGD(TAG, "content len = %d", atoi(val));

    for (;;) {
        rc = wsession_read(session, buf, sizeof(buf), 6*1000);
        if (rc <= 0) {
            LOGE(TAG, "readn err. rc = %d", rc);
            break;
        }
        cnt += rc;
    }
    LOGD(TAG, "rc = %8d, cnt = %8d", rc, cnt);

err:
    wsession_destroy(session);
    aos_freep(&g_url);
    return;
}

// int fft_test()
// {
//     int i = 0;
// #if 0
//     for (i = 0; i < 1; i++) {
//         float dataOut[2304] = {0};
//         void* table = SpxKissFftInit(2304);
//         if (!table)
//             return -1;

//         SpxFftFloatForward(table, (float *)indata_bin, dataOut);
//         SpxFftDestroy(table);
//     }
// #else
//     for (i = 0; i < 1; i++) {
//         static int16_t dataOut[2304] = {0};
//         void* table = SpxKissFftInit(2304);
//         if (!table)
//             return -1;

//         SpxFftForward(table, (int16_t *)indata_bin, dataOut);
//         SpxFftDestroy(table);
//     }
// #endif
//     printf("test finish!\n");

//     return 0;
// }

int mca_test()
{
#if 1
    mcax_register_ipc();
    {
        int rc;
        fxp32_t coeff[5] = { 0xffae2458, 0x006c3bdc, 0xff5acd7a, 0x00d97320, 0xff09b684 };
        static fxp32_t input[16] = { 0xda374d1e, 0x3fe6b88d, 0xa66f8f14, 0x7ba0c273, 0xeffffa6a, 0x477e84b9, 0xf9f5c520, 0xcefd255f,
                                     0xfbcf8536, 0x745a2a65, 0xfb5d30ac, 0x79dd79cb, 0xffff1d82, 0x757f5991, 0x7adb01b8, 0xfb7e6fb7
                                   };
        static fxp32_t output[16 - 2];

        memset(output, 0, sizeof(output));

        rc = cmca_iir_fxp32_coeff32_config(coeff);
        rc = cmca_iir_fxp32(input, 16, 0, 0, output);
        printf("====>>>rc = %d\n", rc);
    }
#else
    mcaicore_cp_init();
#endif

    printf("test finish!\n");

    return 0;
}

static void cmd_ipc_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int rc;
    char url[128];

    if (argc == 3 && strcmp(argv[1], "start") == 0) {
        printf("player %s\n", argv[1]);

        if (strcmp(argv[2], "inter") == 0) {
            //char url[128];

            //snprintf(url, 128, "mem://addr=%u&size=%u&avformat=rawaudio&avcodec=pcm_s16le&channel=1&rate=16000", g_pcm_data, g_pcm_len);
            //_player(url);
        } else if (strcmp(argv[2], "pf") == 0) {
            snprintf(url, 128, "mem://addr=%u&size=%u", (uint32_t)&pingfan16k, sizeof(pingfan16k));
            _player(url);
        } else if (strcmp(argv[2], "bin") == 0) {
#if 0
            snprintf(url, sizeof(url), "mem://addr=%u&size=%u", (uint32_t)&_test_bin, _test_bin_len);
#else
            //snprintf(url, sizeof(url), "mem://addr=%u&size=%u&decryption_key=6abed02448b8ffd2224ff54619935526",
            //		(uint32_t)&_test_bin, _test_bin_len);
            snprintf(url, sizeof(url),
                     "crypto://mem://addr=%u&size=%u&key=527930d9bab56a1ed763991c159d41fa&iv=a084a4e635e5b32830bdb901089ac0c3",
                     (uint32_t)&_test_bin, _test_bin_len);
#endif
            _player(url);
        }
        // else if (strcmp(argv[2], "tt") == 0) {
        //     char url[128];
        //     snprintf(url, 128, "mem://addr=%u&size=%u", (uint32_t)&local_audio_test2k, sizeof(local_audio_test2k));
        //     _player(url);
        // }
        // else if (strcmp(argv[2], "tt") == 0) {
        //     char url[128];
        //     // snprintf(url, 128, "mem://addr=%u&size=%u", (uint32_t)&local_audio_SPK_1K_0dB, sizeof(local_audio_SPK_1K_0dB));
        //     snprintf(url, 128, "mem://addr=%u&size=%u", (uint32_t)&local_audio_80Hz_0dB_single, sizeof(local_audio_80Hz_0dB_single));
        //     _player(url);
        // }
        else {
            _player(argv[2]);
        }

    } else if (argc == 2 && strcmp(argv[1], "stop") == 0) {
        _player_stop();
    } else if (argc == 2 && strcmp(argv[1], "pause") == 0) {
        _player_pause();
    } else if (argc == 2 && strcmp(argv[1], "resume") == 0) {
        _player_resume();
    } else if (argc == 4 && strcmp(argv[1], "start") == 0) {
        if (NULL != g_player) {
            rc = player_play(g_player, argv[2], atoi(argv[3]));
            LOGD(TAG, "%s, %d, rc = %d. play", __FUNCTION__, __LINE__, rc);
        }
    } else if (argc == 2 && strcmp(argv[1], "tts") == 0) {
        g_tts_fifo = nsfifo_open("fifo://tts/1", O_CREAT, 64*1024);
        if (g_tts_fifo) {
            aos_task_new("xx_task", _ptask, NULL, 6*1024);
            _player("fifo://tts/1");
        }
    } else if (argc == 2 && strcmp(argv[1], "tts_stop") == 0) {
        if (g_tts_fifo) {
            nsfifo_set_eof(g_tts_fifo, 0, 1);
            _player_stop();
            aos_msleep(2000);
            nsfifo_close(g_tts_fifo);
            g_tts_fifo = NULL;
        }
    } else if (argc == 3 && strcmp(argv[1], "web") == 0) {
        g_url = strdup(argv[2]);
        LOGD(TAG, "g_url = %s", g_url);
        aos_task_new("web_task", _webtask, NULL, 6*1024);
    } else if (argc == 3 && strcmp(argv[1], "eqparam") == 0) {
        if (g_player)
            _eq_set_params(argv[2]);
    } else if (argc == 3 && strcmp(argv[1], "vol") == 0) {
        int vol = argv[2] ? atoi(argv[2]) : 0;
        _player_set_vol(vol);
        //AGSetVolume(vol);
        //snd_mixer_selem_set_playback_volume_all(g_media.elem, vol);
        //volicore_set(vol);
    } else if (argc == 2 && strcmp(argv[1], "vol_get") == 0) {
        //int8_t vol = 0;
        //AGGetVolume(&vol);
        //printf("vol = %d\n", vol);
        _player_get_vol();
    } else if (argc == 3 && strcmp(argv[1], "mute") == 0) {
        //int mute = argv[2] ? atoi(argv[2]) : 0;
        //AGSetMute(mute);
    } else if (argc == 2 && strcmp(argv[1], "mute_get") == 0) {
        //bool mute = false;
        //AGGetMute(&mute);
        //printf("mute = %d\n", mute);
    } else if (argc == 2 && strcmp(argv[1], "time") == 0) {
        play_time_t ptime;
        memset(&ptime, 0, sizeof(play_time_t));
        rc = player_get_cur_ptime(g_player, &ptime);
        LOGD(TAG, "=====rc = %d, duration = %llums, cur = %llums", rc, ptime.duration, ptime.curtime);
    } else if (argc == 3 && strcmp(argv[1], "seek") == 0) {
        uint64_t timestamp = atoi(argv[2]) * 1000;
        rc = player_seek(g_player, timestamp);
        LOGD(TAG, "=====rc = %d, timestamp = %llums", rc, timestamp);
    } else if (argc == 2 && strcmp(argv[1], "fft") == 0) {
        // fft_test();
    } else if (argc == 2 && strcmp(argv[1], "mca") == 0) {
        mca_test();
    } else {
        printf("player: invaild argv");
    }
}

static const char *g_player_cenc_key = "6abed02448b8ffd2224ff54619935526";
static int _get_decrypt(const void *in, size_t ilen, void *out, size_t *olen)
{
    int rc;

    CHECK_PARAM(in && ilen && out && olen && olen, -1);
    if (*olen < 16) {
        LOGE(TAG, "olen is too small for cenc-str!");
        return -1;
    }
    rc = bytes_from_hex(g_player_cenc_key , out, *olen);
    CHECK_RET_TAG_WITH_RET(rc == 0, -1);
    *olen = 16;

    return 0;
}

int cli_reg_cmd_player(void)
{
    ply_conf_t ply_cnf;
    uint32_t resample_rate = 48000;
    static const struct cli_command cmd_info = {
        "player",
        "player url",
        cmd_ipc_func,
    };

    aos_cli_register_command(&cmd_info);

    player_conf_init(&ply_cnf);
    //ply_cnf.rcv_timeout = 0;
    //ply_cnf.cache_size  = 800*1024;
    ply_cnf.vol_en      = 1;
    ply_cnf.vol_index   = 160; // 0~255
    ply_cnf.get_dec_cb  = _get_decrypt;
    ply_cnf.event_cb    = _player_event;
#if 1
    ply_cnf.resample_rate = resample_rate;
    /* config the aef param */
    //ply_cnf.aef_conf      = sona_aef_config;
    //ply_cnf.aef_conf_size = sona_aef_config_len;
#else
    /* aef debug on after lwip init, need resample to the sample rate must */
    aef_debug_init(resample_rate, sona_aef_config, sona_aef_config_len);
    ply_cnf.resample_rate = resample_rate;
#endif

    g_player = player_new(&ply_cnf);
#if 0
    {
        int rc, i;
        peq_seten_t eqen;
        peq_setpa_t eqpa;

        for (i = 0; i < EQ_SEGMENT; i++) {
            eqpa.segid = i;
            memcpy(&eqpa.param, (void*)&bqfparams[i], sizeof(eqfp_t));
            if (bqfparams[i].type != EQF_TYPE_UNKNOWN) {
                rc = player_ioctl(g_player, PLAYER_CMD_EQ_SET_PARAM, &eqpa);
                CHECK_RET_TAG_WITH_RET(rc == 0, -1);
            }
        }

        eqen.enable = 1;
        rc = player_ioctl(g_player, PLAYER_CMD_EQ_ENABLE, &eqen);
        CHECK_RET_TAG_WITH_RET(rc == 0, -1);
    }
#endif

    return 0;
}



