#include "app_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <aos/cli.h>
#include <aos/log.h>
#include "tee_test.h"

static const char *TAG = "app_cli";

static int cli_tee_proc(int argc, char **argv)
{
    if (argc < 3) {
        return -1;
    }
    if (strcmp(argv[2], "all") == 0) {
        tee_test();
        return 0;
    }
    printf("app tee <all>\n");
    return -1;
}

static void cmd_apps_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc >= 2) {
        if (strcmp(argv[1], "tee") == 0) {
            if (cli_tee_proc(argc, argv) == 0) {
                return;
            }            
        }
    } else {
        printf("\tapp tee all\n");
    }
}

void cli_reg_cmd_apps(void)
{
    static const struct cli_command cmd_info = {"app", "app info", cmd_apps_func};

    aos_cli_register_command(&cmd_info);
}
