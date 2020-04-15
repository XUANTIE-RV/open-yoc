/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __SHELL_H__
#define __SHELL_H__
#include <stdint.h>

#define SHELL_LINE_MAX_LEN   32
#define SHELL_PARAM_MAX_NUM  5
#define SHELL_MAX_COUNT      10

#define SHELL_CMD_NAME_LEN   8
#define SHELL_CMD_HELP_LEN   32
#define SHELL_CMD_STRUCT_LEN 44

typedef void (*shell_func)(int argc, char*argv[]);

typedef struct
{
    char name[SHELL_CMD_NAME_LEN];      /* cmd name */
    char help[SHELL_CMD_HELP_LEN];      /* introduce info */
    shell_func func;                    /* cmd func */
}shell_cmd_t;


//注册命令函数
void shell_register_command(shell_cmd_t *new_cmd);

// 初始化函数
void shell_init(void);

// shell主处理函数
uint8_t shell_main(void);

#endif
