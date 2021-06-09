# 命令行服务（cli）

## 概述

CLI(命令行服务)是基于uService的一种服务。可在用户提示符下键入可执行指令，实现对功能命令的维护。

## 组件安装

```bash
yoc init
yoc install cli
```

## 接口列表

| 函数                   | 说明                         |
| : --------------------- | : --------------------------- |
| cli_service_init       | 初始化命令行服务             |
| cli_service_stop       | 停止命令行服务               |
| cli_service_reg_cmd    | 注册命令行服务               |
| cli_service_unreg_cmd  | 注销可以执行的命令行服务     |
| cli_service_reg_cmds   | 注册可以执行的命令行服务队列 |
| cli_service_unreg_cmds | 注销可以执行的命令行服务队列 |

## 接口详细说明

### cli_service_init
`void cli_service_init(utask_t *task);`

- 功能描述: 
  - 初始化命令行服务。

- 参数: 
  - `task`: 微服务任务名称。

- 返回值: 
  - 无。

### cli_service_stop

`int cli_service_stop(void);`

- 功能描述: 
  - 停止命令行服务。

- 参数: 
  - 无。

- 返回值: 
  - 0: 成功。
  - -1: 失败。

### cli_service_reg_cmd

`int cli_service_reg_cmd(const struct cli_command *info);`

- 功能描述: 
  - 注册命令行服务。

- 参数: 
  - `info`: 需要注册的可以执行命令行服务。

- 返回值: 
  - 0: 成功。
  - <0: 失败。

### cli_service_unreg_cmd

`int cli_service_unreg_cmd(const struct cli_command *info);`

- 功能描述: 
  - 注销可以执行的命令行服务。

- 参数: 
  - `info`: 需要注销的可以执行命令行服务。

- 返回值: 
  - 0: 成功。
  - <0: 失败。

### cli_service_reg_cmds

`int cli_service_reg_cmds(const struct cli_command commands[],int num_commands);`

- 功能描述: 
  - 注册可以执行的命令行服务队列。

- 参数: 
  - `commands`: 需要注册的可以执行命令行服务队列指针。
  - `num_commands`: 需要注册的可以执行命令行服务队列命令数。

- 返回值: 
  - 0: 成功。
  - <0: 失败。

### cli_service_unreg_cmds

`int cli_service_unreg_cmds(const struct cli_command commands[], int num_commands);`

- 功能描述: 
  - 注销可以执行的命令行服务队列。

- 参数: 
  - `commands`: 需要注销的可以执行命令行服务队列指针。
  - `num_commands`: 需要注销的可以执行命令行服务队列命令数。

- 返回值: 
  - 0: 成功。
  - <0: 失败。

## 示例

### 命令行服务接口

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

### 命令行服务初始化

```c
#include <yoc/cli.h>
extern const char *console_get_devname(void);

int aos_cli_init(void)
{
    cli_service_init(NULL);
    return 0;
}
```

### 命令行服务注册

```c
static uint32_t help_func(void)
{
    printf("%s, %d\n", __func__, __LINE__);
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



## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

无。

## 组件参考

无。

