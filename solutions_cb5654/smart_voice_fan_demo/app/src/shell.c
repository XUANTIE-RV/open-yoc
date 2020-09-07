/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "silan_uart.h"
#include "app_printf.h"

extern void mdelay(uint32_t ms);
shell_cmd_t sys_cmds[SHELL_MAX_COUNT] = {0};

char shell_line[SHELL_LINE_MAX_LEN]   = {0}; /* store the strings recved from uart */
char *shell_param[SHELL_LINE_MAX_LEN] = {0}; /* store the param recved (include cmd name) */

extern int shell_fputc(int ch);
extern int shell_fgetc(char *ch);
extern void drv_reboot(int cmd);
extern int get_wakeup_cnt(void);

static int shell_get_char(char *recv_ch)
{
    return shell_fgetc(recv_ch);
}

static void shell_send_char(char ch)
{
    shell_fputc((int)ch);
}

/* parse cmd and param from cmd_string
 * 获取到的paramArry[0]为要允许的命令名
 * 其他的为命令参数
 * 返回值为获取到的参数的个数（包括一个命令名）
 */
static uint8_t shell_get_param(char* line, char *param_arry[], uint8_t arry_len)
{
    uint8_t i;
    char *ptr = NULL;
    ptr = strtok(line, " ");
    for(i = 0; ptr != NULL &&i < arry_len; i++)
    {
        param_arry[i] = ptr;
        ptr = strtok(NULL, " ");
    }
    return i;
}


/*
 * get a cmd with '\r\n' end
 */
static uint8_t shell_get_one_line(char *line, uint8_t max_len)
{
    char get_char;
    static uint8_t count = 0; /*用于记录除特殊字符外的其他有效字符的数量*/
    if (shell_get_char(&get_char))
    {
        if(count >= max_len) /*长度超限*/
        {
            count = 0; /*清零计数器以便后续使用*/
            shell_send_char('\n'); /*把回车字符输出到串口*/
            LOGE("Reach max\r\n");
            return 1;  /*返回有效标志*/
        }
        line[count] = get_char; /*记录数据*/

        switch(get_char)
        {
        case 0x08:
        case 0x7F: /*退格键或者删除键*/
            {
                if(count > 0)
                {
                    count--; /*删除上一个接收到的字符*/
                    shell_send_char(0x08); /* 退格 */
                    shell_send_char(' ');  /* 该字符回显去除 */
                    shell_send_char(0x08); /* 退格 */
                }
                return 0;
            }
            break;
        case '\r':
        case '\n': /*接收到回车换行，证明已经收到一个完整的命令*/
            {
                line[count] = '\0'; /*添加字符串结束符，刚好可以去掉'\r'或者'\n'*/
                count = 0; /*清零计数器以便后续使用*/
                shell_send_char('\n'); /*把回车字符输出到串口*/
                return 1; /*返回有效标志*/
            }
            break;
        default:
            count++;
        }
        shell_send_char(get_char); /*把收到的字符输出到串口*/
    }
    return 0;
}


uint8_t shell_main(void)
{
    uint8_t param_num = 0;

    if (shell_get_one_line(shell_line, SHELL_LINE_MAX_LEN))
    {
        param_num = shell_get_param(shell_line, shell_param, SHELL_PARAM_MAX_NUM);
        if (param_num)
        {
            uint8_t i = 0;
            for (i = 0; i < SHELL_MAX_COUNT; i++) /* find cmd name */
            {
                if(strcmp(sys_cmds[i].name, shell_param[0]) == 0)
                {
                    sys_cmds[i].func(param_num, &shell_param[0]); /* run cmd func */
                    LOGI("shell-> ");
                    return 1;
                }
            }
            LOGE("unknown name \'%s\'\n", shell_line); /* print err info */
        }
        LOGI("shell-> ");
    }
    return 0;
}

static int cmd_cnt = 0;
void shell_register_command(shell_cmd_t *new_cmd)
{
    memcpy(sys_cmds[cmd_cnt].name, new_cmd->name, SHELL_CMD_NAME_LEN);
    memcpy(sys_cmds[cmd_cnt].help, new_cmd->help, SHELL_CMD_HELP_LEN);
    sys_cmds[cmd_cnt].func = new_cmd->func;
    cmd_cnt++;

    if(cmd_cnt >= SHELL_MAX_COUNT)
    {
        LOGE("Regcmds exceed max\n");
    }
}

static void help_cmd_fun(int argc, char*argv[]) /* cmd func */
{
    uint8_t i;
    if (argc > 1)
    {
        LOGE("no arg\n");
        return;
    }

    for (i=0; i < SHELL_MAX_COUNT; i++)
    {
        if(sys_cmds[i].func != NULL)
            LOGI("%s        %s\n", sys_cmds[i].name, sys_cmds[i].help);
    }
}

static void sys_cmd_fun(int argc, char*argv[]) /* cmd func */
{
    if (argc < 2)
    {
        LOGE("sys cmd has error arg.\n");
        return;
    }

    if (0 == strcmp(argv[1], "reboot")) {
        drv_reboot(0);
        return;
    } else if (0 == strcmp(argv[1], "sleep")) {
        if (argc == 3 && argv[2]) {
            mdelay(atoi(argv[2]) * 1000);
            return;
        }
    } else if(0 == strcmp(argv[1], "wakeup")){
        LOGD("system wakeup times: %d\n", get_wakeup_cnt());
    }
}

void shell_init(void)
{
    shell_cmd_t help_cmd = {"help", "show commands", help_cmd_fun};
    shell_register_command(&help_cmd);

    shell_cmd_t sys_cmd = {"sys", "sys command", sys_cmd_fun};
    shell_register_command(&sys_cmd);
}
