/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>

#include <aos/cli.h>
#include <aos/kernel.h>
#include <smart_audio.h>

//#include "pcm_test.h"
#include "aui_cloud/app_aui_cloud.h"

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

static void cmd_aui_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 3) {
        return;
    }

    if (strcmp(argv[1], "tts") == 0) {
        char *text = get_text(argc, argv, 2);
        if (text) {
            smtaudio_stop(MEDIA_SYSTEM);
            app_aui_cloud_stop_tts();
            app_aui_cloud_start_tts();
            app_aui_cloud_tts_run(text, 0);
            free(text);
        }
    }

#if 0
    int len = 0;
    app_aui_cloud_start(0);

    while (len < pcm_len) {
        app_aui_cloud_push_audio(pcm + len, 640);
        len += 640;
        aos_msleep(15);
    }

    app_aui_cloud_stop(0);
#endif

}

void cli_reg_cmd_aui(void)
{
    static const struct cli_command cmd_info = {
        "ai",
        "ai tools",
        cmd_aui_func
    };

    aos_cli_register_command(&cmd_info);
}
