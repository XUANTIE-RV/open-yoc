/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <aos/cli.h>
#include <yoc/mic.h>
#include <board.h>

#include <sys/stat.h>
#include <vfs.h>
#include <dirent.h>

#include "app_voice.h"

#define TAG "voice"

static int cli_voice_proc(int argc, char **argv)
{
    if (argc < 2) {
        return -1;
    }

    if (strcmp(argv[1], "wakeup") == 0 || strcmp(argv[1], "p2t") == 0) {
        int enable = atoi(argv[2]);
        aui_mic_control(MIC_CTRL_START_SESSION, enable);
    } else if (strcmp(argv[1], "mute") == 0) {
        int enable = atoi(argv[2]);
        aui_mic_control(MIC_CTRL_VOICE_MUTE, enable);
    } else if (strcmp(argv[1], "level") == 0) {
        if (argc == 4) {
            char *word = argv[2];
            int level = atoi(argv[3]);
            aui_mic_control(MIC_CTRL_WAKEUP_LEVEL, word, level);
        }
    } else if (strcmp(argv[1], "plst") == 0) {
        int delay = atoi(argv[2]);
        aui_mic_control(MIC_CTRL_NOTIFY_PLAYER_STATUS, 1, delay);
        aos_msleep(100);
        aui_mic_control(MIC_CTRL_NOTIFY_PLAYER_STATUS, 0, delay);
    } else if (strcmp(argv[1], "wwv") == 0) {
        if ((strcmp(argv[2], "save") == 0)) {
            if (app_wwv_get_confirm()) {
                uint8_t *data = NULL;
                int      size = 0;
                app_wwv_get_data(&data, &size);
                if (data && size > 0) {
                    printf("save file /wwvdata.bin size=%d\r\n", size);
                    int fd = aos_open("/wwvdata.bin", (O_WRONLY | O_CREAT | O_TRUNC));
                    aos_write(fd, data, size);
                    aos_close(fd);
                }
            } else {
                printf("run the 'voice wwv 1' cmd first\r\n");
            }
        } else if ((strcmp(argv[2], "wake") == 0)) {
            aui_mic_send_wakeup_check(1);
        } else if ((strcmp(argv[2], "fail") == 0)) {
            aui_mic_send_wakeup_check(0);
        } else {
            int confirm = atoi(argv[2]);
            app_wwv_set_confirm(confirm);
            printf("set wwv en %d\r\n", confirm);
        }
    } else if (strcmp(argv[1], "doa") == 0) {
        aui_mic_control(MIC_CTRL_START_DOA);
        printf("start doa ctrl\r\n");
    } else if (strcmp(argv[1], "laec") == 0) {
        int enable = atoi(argv[2]);
        aui_mic_control(MIC_CTRL_ENABLE_LINEAR_AEC_DATA, enable);
        printf("start laec ctrl en=%d\r\n", enable);
    } else if (strcmp(argv[1], "asr") == 0) {
        int enable = atoi(argv[2]);
        aui_mic_control(MIC_CTRL_ENABLE_ASR, enable);
        printf("start asr ctrl en=%d\r\n", enable);
#ifdef CONFIG_BOARD_AUDIO
    } else if (strcmp(argv[1], "gain") == 0) {
        if (argc == 4) {
            int id   = atoi(argv[2]);
            int gain = atoi(argv[3]);
            if (board_audio_in_set_gain(id, gain) == 0) {
                printf("id=%d gain=%ddb\r\n", id, gain);
            } else {
                printf("set gain error or not support\r\n");
            }
        } else {
            printf("gain [%d, %d, %d]\r\n",
                   board_audio_in_get_gain(0),
                   board_audio_in_get_gain(1),
                   board_audio_in_get_gain(2));
        }
#endif
    } else {
        return -1;
    }

    return 0;
}

static void cmd_voice_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc >= 2) {
        if (cli_voice_proc(argc, argv) == 0) {
            return;
        }
    } else {
        printf("\tvoice p2t 1\n");
        printf("\tvoice mute 1\n");
        printf("\tvoice doa\n");
        printf("\tvoice laec 0|1\n");
    }
}

void cli_reg_cmd_voice(void)
{
    static const struct cli_command cmd_info = { "voice", "voice cmd test", cmd_voice_func };

    aos_cli_register_command(&cmd_info);
}
