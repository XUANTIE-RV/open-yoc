/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/cli.h>
#include <yoc/adb_server.h>

#if defined(CONFIG_COMP_VOICE_WRAPPER) && CONFIG_COMP_VOICE_WRAPPER
extern void fct_pcm_chk_record(char *play_url, int second, int vol, int pcmchk, int savefile);

static void cmd_factory_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }

    char *fct_cmd = argv[1];

    if (strcmp(fct_cmd, "pcmchk") == 0) {
        if (argc == 6) {
            int rec_sec = atoi(argv[2]);
            int vol = atoi(argv[3]);
            char *play_url = argv[4];
            int savefile = atoi(argv[5]);
            fct_pcm_chk_record(play_url, rec_sec, vol, 1, savefile);
        } else {
            adbserver_send("param error\r\n");
        }
    } else if (strcmp(fct_cmd, "pcmrec") == 0) {
        if (argc >= 3) {
            int rec_sec = atoi(argv[2]);
            fct_pcm_chk_record("none://", rec_sec, -1, 0, 1);
        } else {
            adbserver_send("param error\r\n");
        }
    } else {
        ;
    }
}
#endif

void cli_reg_cmd_factory(void)
{
#if defined(CONFIG_COMP_VOICE_WRAPPER) && CONFIG_COMP_VOICE_WRAPPER
    static const struct cli_command cmd_info = { "factory", "factory test", cmd_factory_func };

    aos_cli_register_command(&cmd_info);
#endif
}

