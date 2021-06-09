/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <app_config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "app_init.h"
#ifdef CONFIG_CSI_V1
#include <pin_name.h>
#else
#include <soc.h>
#endif
#include <devices/devicelist.h>
#include "app_main.h"
#include "app_init.h"
#include <board.h>
#include <aos/cli.h>
#include "ulog/ulog.h"


static void cmd_secure_demo_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    LOGI(TAG, "start %s, %d\n", __FUNCTION__, __LINE__);
    if (argc == 2)
    {
        if (strcmp(argv[1], "km") == 0)
        {
            LOGI(TAG, "start %s, %d\n", __FUNCTION__, __LINE__);
            key_mgr_demo_main();
        }
        else if (strcmp(argv[1], "crypto") == 0)
        {
            sec_crypto_demo_main();
        }
        else if (strcmp(argv[1], "mbedtls") == 0)
        {
            mbedtls_demo_main();
        }
        else if (strcmp(argv[1], "ecc") == 0)
        {
            sec_crypto_ecc_demo();
        }
        else if (strcmp(argv[1], "aes") == 0)
        {
#ifdef CONFIG_SECURITY_DEMO_TEST
            aes_test_main();
#endif
        }
        else if (strcmp(argv[1], "alimqtt") == 0)
        {
            sec_alimqtt_demo_main(ALIMQTT_CMD_NET);
        }
        else if (strcmp(argv[1], "push") == 0)
        {
            sec_alimqtt_demo_main(ALIMQTT_CMD_PUSH);
        }
        else
        {
            printf("invalid cmd!%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
        }
    }
}

void cli_reg_cmd_secure_demo(void)
{
    static const struct cli_command cmd_info = {
        "sdemo",
        "secure demo commands",
        cmd_secure_demo_func,
    };

    aos_cli_register_command(&cmd_info);
    LOGE(TAG, "\n%s\n", "cmd:");
}

int main(void)
{
    extern int posix_init(void);
    extern void cxx_system_init(void);

    board_yoc_init();

    printf("%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    LOGI(TAG, "\n%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    LOGD(TAG, "%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    LOGE(TAG, "%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
    cli_reg_cmd_secure_demo();
}
