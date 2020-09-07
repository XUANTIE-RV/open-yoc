/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"
#include <media.h>
#include <yoc/mic.h>

/******************************************************
 * 测试音播放
 *****************************************************/
#include "avutil/named_straightfifo.h"
static int nsfifo_is_reof(nsfifo_t *fifo)
{
    int ret;
    uint8_t reof;
    uint8_t weof;
    ret = nsfifo_get_eof(fifo, &reof, &weof);

    return (ret == 0) ? reof : 1;
}

static void play_sin(int second)
{
    int                reof = 0;
    char *             pos  = NULL;
    int                len  = 0;
    nsfifo_t *fifo = NULL;
    int16_t *sin_data;
    int sin_data_len;

    uint8_t wav_head[] = {0x52, 0x49, 0x46, 0x46, 0x24, 0xA6, 0x0E, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
                          0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x00, 0x7D, 0x00, 0x00,
                          0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x00, 0xA6, 0x0E, 0x00};

    int wav_head_int[11];

    int16_t sin_1k[16] = {
        0, 12539,  23169,  30272,  32767,  30272,  23169,  12539,
        0, -12540, -23170, -30273, -32767, -30273, -23170, -12540,
    };


    memcpy(wav_head_int, wav_head, sizeof(wav_head));

    if (second < 1) {
        second = 30;
    }

    /*每秒32000 + 36*/
    wav_head_int[1] = 32000 * second + 36; /*wav 文件大小 - 8*/
    wav_head_int[10] = 32000 * second;     /* PCM数据字节数 */

    sin_data = sin_1k;
    sin_data_len = sizeof(sin_1k);

    fifo = nsfifo_open("fifo://sintest", O_CREAT, 16000 / 1000 * 16 / 8 * 10 * 40);

    aui_player_stop(MEDIA_SYSTEM);
    aui_player_play(MEDIA_SYSTEM, "fifo://sintest", 1);

    len = nsfifo_get_wpos(fifo, &pos, 4000);
    if (len > sizeof(wav_head)) {
        memcpy(pos, wav_head_int, sizeof(wav_head));
        nsfifo_set_wpos(fifo, sizeof(wav_head));
    } else {
        nsfifo_close(fifo);
        return;
    }

    int wlen = 0;
    while (wlen < wav_head_int[10]) {
        reof = nsfifo_is_reof(fifo); /** peer read reach to end */
        if (reof) {
            break;
        }

        len = nsfifo_get_wpos(fifo, &pos, 4000);
        if (len <= 0) {
            /* If available space is too small, wait player eat the buffer */
            aos_msleep(20);
            continue;
        }

        for (int j = 0; j < len / sin_data_len; j++) {
            memcpy(pos, sin_data, sin_data_len);
            pos += sin_data_len;
            wlen += sin_data_len;
        }

        for (int j = 0; j < (len % sin_data_len); j++) {
            *pos = 0;
            pos++;
            wlen += sin_data_len;
        }

        nsfifo_set_wpos(fifo, len);
    }

    /* set write eof */
    nsfifo_set_eof(fifo, 0, 1);

    while (0 == nsfifo_is_reof(fifo)) {
        aos_msleep(100); //wait peer drain fifo
    }
    nsfifo_close(fifo);
}

static void task_play_sin(void *arg)
{
    static int task_play_sin_is_run = 0;

    if (task_play_sin_is_run) {
        return;
    }

    task_play_sin_is_run = 1;

    int type = (int)arg;
    play_sin(type);

    task_play_sin_is_run = 0;
}

/******************************************************
 * eq测试命令
 *****************************************************/
#if APP_EQ_EN
#define EQ_USAGE "usage: ai eqparam \"{100,-20.0,1.0}{200,-20.0,1.0}{400,-20.0,1.0}{1000,3.0,1.0}\"\n"
static void eq_set_params(char *eq_params)
{
    if (strlen(eq_params) < 5) {
        printf(EQ_USAGE);
        aui_player_eq_config(NULL, 0);
        return;
    }

    char *start = eq_params, *end;
    char *end_of_str = eq_params + strlen(eq_params);
    eqfp_t filter_params[15];
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
            eqfp_t *pparam = &filter_params[param_index];

            while (start < end && i < 3) {
                while (*delim != ',' && delim < end) delim++;

                if (delim-start > 31 || delim-start == 0) {
                    err = 1;
                    break;
                }
                memcpy(snum, start, delim-start);
                snum[delim-start] = '\0';

                switch(i) {
                    case 0:
                        pparam->rate = atoi(snum);
                        break;
                    case 1:
                        pparam->gain = atof(snum);
                        break;
                    case 2:
                        pparam->q = atof(snum);
                        break;
                    default:
                        ;
                }
                i++;

                start = ++delim;
            }

            if (i != 3) {
                err = 3;
                break;
            }
            param_index++;

            start = end + 1;
        }
    } while (start < end_of_str && start != NULL && end != NULL && !err && param_index < 15);

    if (err || start == NULL || end == NULL) {
        printf(EQ_USAGE);
        aui_player_eq_config(NULL, 0);
    } else {
        for (int i = 0; i < param_index; ++i) {
            printf("{%d, %f, %f}\n", filter_params[i].rate, filter_params[i].gain, filter_params[i].q);
            if (!(filter_params[i].gain >= -24.0 && filter_params[i].gain <= 24.0
              && filter_params[i].q >= 0.1 && filter_params[i].q <= 12.0
              && filter_params[i].rate >= 0)) {
                printf("Eq param Error!\n\
                Param scope: frequency >= 0; q value: 0.1 ~ 12.0; gain: -24.0 ~ 24.0");
                return;
            }

            filter_params[i].enable = 1;
            filter_params[i].type = EQF_TYPE_PEAK;
        }
        aui_player_eq_config(filter_params, param_index);
        printf("eq set params successfully:\n");
    }
} 
#endif

/******************************************************
 * 命令解析
 *****************************************************/
static char *get_text(int argc, char **argv, int start)
{
    int   i;
    int   content_len = 0;
    char *content     = NULL;
    char *ptr;
    for (i = start; i < argc; i++) {
        content_len += strlen(argv[i]);
        content_len++;
    }
    content = malloc(content_len);
    if (content == NULL) {
        return NULL;
    }
    ptr = content;
    for (i = start; i < argc; i++) {
        int len = strlen(argv[i]);
        memcpy(ptr, argv[i], len);
        ptr += len;
        *ptr = ' ';
        ptr++;
    }
    ptr--;
    *ptr = '\0';

    return content;
}

static int cli_aui_proc(int argc, char **argv)
{
    /* ai play url */
    if (argc < 3) {
        return -1;
    }

    if (strcmp(argv[1], "talk") == 0) {
        char *text = get_text(argc, argv, 2);
        if (text) {
            app_aui_cloud_push_text(text);
            free(text);
        }
    } else if (strcmp(argv[1], "tts") == 0) {
        char *text = get_text(argc, argv, 2);
        if (text) {
            aui_player_stop(MEDIA_SYSTEM);
            app_aui_cloud_tts_run(text, 1);
            free(text);
        }
    } else if (strcmp(argv[1], "filepcm") == 0) {
        /* automatic dialog test */
        extern int pcm_file_to_dsp(char *filename);
        pcm_file_to_dsp(argv[2]);
    } else if (strcmp(argv[1], "wake") == 0) {
        aui_mic_control(MIC_CTRL_START_SESSION);
    } else {
        return -1;
    }

    return 0;
}

static int cli_player_proc(int argc, char **argv)
{
    if (argc < 3) {
        return -1;
    }

    if (strcmp(argv[1], "music") == 0 ||  strcmp(argv[1], "play") == 0) {
        if (!app_player_get_mute_state()) {
            aui_player_play(MEDIA_MUSIC, argv[2], 0);
        } else {
            printf("Device is mute\n");
        }
    } else if (strcmp(argv[1], "notify") == 0) {
        if (!app_player_get_mute_state()) {
            int nid = atoi(argv[2]);
            if (nid >= 0 && nid < LOCAL_AUDIO_END) {
                extern int local_audio_play_imp(local_audio_name_t name, int resume);
                local_audio_play_imp(nid, 1);
            }
        } else {
            printf("Device is mute\n");
        }
    } else if (strcmp(argv[1], "vol") == 0) {
        if (argv[2][0] == '+') {
            app_volume_inc(0);
        } else if (argv[2][0] == '-') {
            app_volume_dec(0);
        } else {
            int vol = atoi(argv[2]);
            app_volume_set(vol, 0);
        }
    } else if (strcmp(argv[1], "db") == 0) {
        //int voldb = atoi(argv[2]);
        //silan_codec_dac_set_gain(0, voldb, voldb);
    } else if (strcmp(argv[1], "asr") == 0) {
        int en = atoi(argv[2]);
        aui_mic_set_wake_enable(en);
    } else if (strcmp(argv[1], "stop") == 0) {
        aui_player_stop(atoi(argv[2]));
    } else if (strcmp(argv[1], "pause") == 0) {
        aui_player_pause(atoi(argv[2]));
    } else if (strcmp(argv[1], "resume") == 0) {
        aui_player_resume(atoi(argv[2]));
    } else if (strcmp(argv[1], "sin") == 0) {
        if (!app_player_get_mute_state()) {
            int second = atoi(argv[2]);
            aos_task_t task_handle;
            aos_task_new_ext(&task_handle, "sin", task_play_sin, (void *)second, 2048, AOS_DEFAULT_APP_PRI);
        } else {
            printf("Device is mute\n");
        }
#if APP_EQ_EN
    } else if (strcmp(argv[1], "eqparam") == 0) {
        eq_set_params(argv[2]);
#endif
    } else {
        return -1;
    }
    return 0;
}

static void cmd_ai_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc >= 2) {
        if (cli_player_proc(argc, argv) == 0) {
            return;
        }

        if (cli_aui_proc(argc, argv) == 0) {
            return;
        }
    } else {
        printf("\tai tts text\n");
//        printf("\tai talk music:singer+songname\n");
//        printf("\tai talk text\n");
        printf("\tai play|music url[http://]\n");
        printf("\tai stop|pause|resume 0|1|255\n");
        printf("\tai notify n\n");
        printf("\tai vol +|-|[0-100]\n");
        printf("\tai wake 0\n");
    }
}

void cli_reg_cmd_aui(void)
{
    static const struct cli_command cmd_info = {"ai", "media cmd test", cmd_ai_func};

    aos_cli_register_command(&cmd_info);
}
