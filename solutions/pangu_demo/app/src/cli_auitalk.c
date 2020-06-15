#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <yoc/mic.h>
#include <media.h>
#include <aos/cli.h>
#include "cloud_baidu.h"
#include "app_main.h"
#include "audio/audio_res.h"

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
            if (strncasecmp(text, MUSIC_PREFIX, strlen(MUSIC_PREFIX)) == 0) {
                baidu_music(&g_aui_handler, text);
            } else {
                aui_cloud_push_text(&g_aui_handler, text);
            }
            free(text);
        }
    } else if (strcmp(argv[1], "tts") == 0) {
        char *text = get_text(argc, argv, 2);
        if (text) {
            aui_player_stop(MEDIA_SYSTEM);
            aui_cloud_req_tts(&g_aui_handler, text, NULL);
            app_player_play(MEDIA_SYSTEM, "fifo://tts/1", 1);
            free(text);
        }
    } else if (strcmp(argv[1], "wake") == 0) {
        //aui_mic_control(MIC_CTRL_START_SESSION);
    } else {
        return -1;
    }

    return 0;
}

static int cli_player_proc(int argc, char **argv)
{
    /* ai play url */
    if (argc < 3) {
        return -1;
    }

    if (strcmp(argv[1], "music") == 0) {
        aui_player_play(MEDIA_MUSIC, argv[2], 0);
    } else if (strcmp(argv[1], "notify") == 0) {
        int nid = atoi(argv[2]);
        if (nid >= 0 && nid < LOCAL_AUDIO_END) {
            local_audio_play(nid);
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
    } else if (strcmp(argv[1], "led") == 0) {
        //int stat = atoi(argv[2]);
        //app_set_led_state(stat);
    } else if (strcmp(argv[1], "pa") == 0) {
        //int en = atoi(argv[2]);
        //app_audio_pa_ctrl(en);
    } else if (strcmp(argv[1], "pause") == 0) {
        aui_player_pause(atoi(argv[2]));
        //aui_player_vol_gradual(MEDIA_MUSIC, 70, 200);
    }  else if (strcmp(argv[1], "pcm") == 0) {
        aui_mic_control(0);
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
        printf("\tai talk music:singer+songname\n");
        printf("\tai talk text\n");
        printf("\tai music url[http://]\n");
        printf("\tai notify [0-6]\n");
        printf("\tai vol +/-/[0-100]\n");
        printf("\tai wake 0\n");
    }
}

void cli_reg_cmd_aui(void)
{
    static const struct cli_command cmd_info = {"ai", "ai info", cmd_ai_func};

    aos_cli_register_command(&cmd_info);
}
