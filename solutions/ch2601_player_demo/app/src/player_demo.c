/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/aos.h>
#include <aos/cli.h>
#include "player_demo.h"
#include "welcome_mp3.h"
#include "raw_example_little.h"

#define TAG "player_demo"
static player_t *g_player;

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

static void cmd_ipc_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "play") == 0) {
        char url[128];

        if (strcmp(argv[2], "welcom") == 0) {
            snprintf(url, sizeof(url), "mem://addr=%u&size=%u", (uint32_t)&_welcome_mp3, _welcome_mp3_len);
            player_play(get_player_demo(), url, 0);
        } else if (strcmp(argv[2], "raw") == 0) {
            snprintf(url, sizeof(url), "mem://addr=%u&size=%u&avformat=rawaudio&avcodec=pcm_s8&channel=1&rate=8000", (uint32_t)&_example_little_raw, _example_little_raw_len);
            player_play(get_player_demo(), url, 0);
        } else {
            player_play(get_player_demo(), argv[2], 0);
        }
    } else if (argc == 2 && strcmp(argv[1], "stop") == 0) {
        player_stop(get_player_demo());
    } else if (argc == 2 && strcmp(argv[1], "pause") == 0) {
        player_pause(get_player_demo());
    } else if (argc == 2 && strcmp(argv[1], "resume") == 0) {
        player_resume(get_player_demo());
    } else {
        printf("\tplayer play welcom/raw\n");
        printf("\tplayer pause\n");
        printf("\tplayer resume\n");
        printf("\tplayer stop\n");
        printf("\tplayer help");
    }
}

player_t *get_player_demo()
{
    if (!g_player) {
        ply_conf_t ply_cnf;

        player_conf_init(&ply_cnf);
        ply_cnf.vol_en         = 1;
        ply_cnf.vol_index      = 160; // 0~255
        ply_cnf.event_cb       = _player_event;
        ply_cnf.period_num     = 12;  // 12 * 5 period_ms = 60ms

        g_player = player_new(&ply_cnf);
    }

    return g_player;
}

int cli_reg_cmd_player(void)
{
    char url[128];
    static const struct cli_command cmd_info = {
        "player",
        "player example",
        cmd_ipc_func,
    };

    aos_cli_register_command(&cmd_info);

    snprintf(url, sizeof(url), "mem://addr=%u&size=%u", (uint32_t)&_welcome_mp3, _welcome_mp3_len);
    player_play(get_player_demo(), url, 0);

    return 0;
}



