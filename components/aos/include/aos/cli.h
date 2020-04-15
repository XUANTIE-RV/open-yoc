/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_CLI_H
#define YOC_CLI_H

#include <aos/list.h>
#include <yoc/uservice.h>


#ifdef __cplusplus
extern "C" {
#endif


#define CLI_CMD_LEN_MAX 256

#define CLI_CMD_MAX_ARG_NUM 30

/* Structure for registering CLI commands */
struct cli_command {
    const char *name;
    const char *help;
    void (*function)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
};

typedef struct cmd_list {
    const struct cli_command *cmd;
    slist_t                   next;
} cmd_list_t;

void    cli_service_init(utask_t *task);
int     cli_service_stop(void);
int     cli_service_reg_cmd(const struct cli_command *info);
int     cli_service_unreg_cmd(const struct cli_command *info);
int     cli_service_reg_cmds(const struct cli_command commands[], int num_commands);
int     cli_service_unreg_cmds(const struct cli_command commands[], int num_commands);
slist_t cli_service_get_cmd_list(void);


/**
 * This function registers a command with the command-line interface.
 *
 * @param[in]  command  The structure to register one CLI command
 *
 * @return  0 on success, error code otherwise.
 */
int aos_cli_register_command(const struct cli_command *cmd);

/**
 * This function unregisters a command from the command-line interface.
 *
 * @param[in]  command  The structure to unregister one CLI command
 *
 * @return  0 on success,  error code otherwise.
 */
int aos_cli_unregister_command(const struct cli_command *cmd);

/**
 * Register a batch of CLI commands
 * Often, a module will want to register several commands.
 *
 * @param[in]  commands      Pointer to an array of commands.
 * @param[in]  num_commands  Number of commands in the array.
 *
 * @return  0 on successÃ¯Â¼Å’ error code otherwise.
 */
int aos_cli_register_commands(const struct cli_command commands[], int num_commands);

/**
 * Unregister a batch of CLI commands
 *
 * @param[in]  commands      Pointer to an array of commands.
 * @param[in]  num_commands  Number of commands in the array.
 *
 * @return  0 on success, error code otherwise.
 */
int aos_cli_unregister_commands(const struct cli_command commands[], int num_commands);

/**
 * CLI initial function
 * @param void
 * @return  0 on success, error code otherwise
 */
int aos_cli_init(void);

/**
 * Stop the CLI thread and carry out the cleanup
 *
 * @return  0 on success, error code otherwise.
 *
 */
#define aos_cli_stop  cli_service_stop

#define aos_cli_printf printf


#ifdef __cplusplus
}
#endif


#endif
