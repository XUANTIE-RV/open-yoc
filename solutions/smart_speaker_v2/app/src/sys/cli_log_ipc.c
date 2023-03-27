/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_COMP_LOG_IPC) && CONFIG_COMP_LOG_IPC
#include <string.h>
#include <aos/cli.h>
#include <log_ipc.h>
#include <board.h>

#define IPC_CMD_LEN    100
static int _cmd_data_generate(int32_t argc, char **argv, char *data)
{
    int len = 0;

    for (int i = 2; i < argc; i++) {
        snprintf(&data[len], IPC_CMD_LEN, " %s", argv[i]);
        len += strlen(data);
    }

    data[len] = '\r';
    len++;
    data[len] = '\n';
    len++;
    data[len] = '\0';
    len++;

    return len;
}

static void task_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    if (argc >= 3 && strcmp(argv[1], "alg") == 0) {
        char data[IPC_CMD_LEN];
        int len = _cmd_data_generate(argc, argv, data);

        log_ipc_cmd_send(board_get_alg_cpuid(), IPC_CMD_DEBUG_CMDINFO, data, len);
    }
}

static struct cli_command cli_cmd[] = {
    { "logipc", "alg cpu console", task_cmd },
};

void cli_reg_cmd_logipc(void)
{
    int32_t num = sizeof(cli_cmd) / sizeof(struct cli_command);
    aos_cli_register_commands(&cli_cmd[0], num);
}
#endif
