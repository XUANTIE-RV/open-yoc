/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/cli.h>

extern const char *console_get_devname(void);

int aos_cli_init(void)
{
    cli_service_init(NULL);
    return 0;
}

/**
 * This function registers a command with the command-line interface.
 *
 * @param[in]  command  The structure to register one CLI command
 *
 * @return  0 on success, error code otherwise.
 */
int aos_cli_register_command(const struct cli_command *cmd) {
    return cli_service_reg_cmd(cmd);
}

/**
 * This function unregisters a command from the command-line interface.
 *
 * @param[in]  command  The structure to unregister one CLI command
 *
 * @return  0 on success,  error code otherwise.
 */
int aos_cli_unregister_command(const struct cli_command *cmd) {
    return cli_service_unreg_cmd(cmd);
}

/**
 * Register a batch of CLI commands
 * Often, a module will want to register several commands.
 *
 * @param[in]  commands      Pointer to an array of commands.
 * @param[in]  num_commands  Number of commands in the array.
 *
 * @return  0 on success， error code otherwise.
 */
int aos_cli_register_commands(const struct cli_command commands[], int num_commands) {
    return cli_service_reg_cmds(commands, num_commands);
}

/**
 * Unregister a batch of CLI commands
 *
 * @param[in]  commands      Pointer to an array of commands.
 * @param[in]  num_commands  Number of commands in the array.
 *
 * @return  0 on success, error code otherwise.
 */
int aos_cli_unregister_commands(const struct cli_command commands[], int num_commands) {
    return cli_service_unreg_cmds(commands, num_commands);
}
