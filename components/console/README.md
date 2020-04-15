# 概述

CLI(命令行服务)是基于uService的一种服务。可在用户提示符下键入可执行指令，实现对功能命令的维护。

# 示例代码

## 命令行服务接口

```c
#ifndef YOC_CLI_H
#define YOC_CLI_H

#include <aos/list.h>
#include <yoc/uservice.h>

#define CLI_CMD_LEN_MAX 256

#define CLI_CMD_MAX_ARG_NUM 10

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
#endif
```

## 命令行服务初始化

```c
#include <yoc/cli.h>
extern const char *console_get_devname(void);

int aos_cli_init(void)
{
    cli_service_init(NULL);
    return 0;
}
```

## 命令行服务注册

```c
static uint32_t help_func(void)
{
    slist_t cmd_lists = cli_service_get_cmd_list();

    cmd_list_t *node;

    slist_for_each_entry(&cmd_lists, node, cmd_list_t, next) {
        printf("%-15s : %s\n", node->cmd->name, node->cmd->help);
    }

    return 0;
}

static void cmd_help_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (NULL == argv[1]) {
        help_func();
    }
}

void cli_reg_cmd_help(void)
{
    static const struct cli_command cmd_info = {
        "help",
        "show commands",
        cmd_help_func,
    };

    aos_cli_register_command(&cmd_info);
}
```
