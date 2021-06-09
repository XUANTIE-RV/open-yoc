/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     atserver.c
 * @brief    C file for AT server parser
 * @version  V1.0
 * @date     2020-02-17
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/common.h>
#include <drv/errno.h>
#include "drv/usart.h"
#include "dev_ringbuf.h"
#include "tst_io.h"
#include "atserver.h"
#include "autotest.h"

#define CHUNKSIZE (256)
static atserver_uservice_t g_atserver;
static char g_cmd_line_buf[BUFFER_MIN_SIZE] = {0};

int atserver_init(void)
{
    memset(&g_atserver, 0, sizeof(atserver_uservice_t));
    g_atserver.cmd_line_st = CMD_LINE_INIT;
    g_atserver.cmd_line_buf = g_cmd_line_buf;
    g_atserver.cmd_line_sz = BUFFER_MIN_SIZE;

    g_atserver.output_terminator = NULL;
    g_atserver.para_delimiter = ',';
    g_atserver.echo_flag = 0;
    slist_init(&g_atserver.cmd_head);

    return 0;
}

/**
 * \brief              check whether an at-cmd has exist in current at-cmd link list
 * \param[in]          cmd at-cmd string
 * \return             0: not exist -EINVAL: exist in current link-list,so the cmd
 *                     can't be added in the link list.
 */

static int atserver_cmd_check(const char *cmd)
{
    atcmd_node_t *node;

    slist_for_each_entry(&g_atserver.cmd_head, node, atcmd_node_t, next) {
        atserver_cmd_t *atcmd = (atserver_cmd_t *)node->at;

        for (int i = 0; i < node->len; i++) {
            if (strcmp(cmd, atcmd->cmd) == 0) {
                return (-EINVAL);
            }

            atcmd++;
        }
    }
    return 0;
}

/**
 * \brief              check whether each at-cmd in groups exist in current global cmd link list
 * \param[in]          cmd point to cmd array
 * \return             n:total cmd number in the cmd[],and every cmd doesn't duplicate with the glogal cmd link list
                       -EINVAL: exist in current link-list,so the cmd
 *                     can't be added in the link list.
 */

static int atserver_cmd_check_group(const atserver_cmd_t cmd[])
{
    atserver_cmd_t *at_cmd = (atserver_cmd_t *)cmd;
    int len = 0;

    while (at_cmd->cmd != NULL) {
        int ret = atserver_cmd_check(at_cmd->cmd);

        if (ret < 0) {
            return ret;
        }

        at_cmd++;
        len++;
    }

    return len;
}

/**
 * \brief              set echo flag for at-cmd
 * \param[in]          flag  0 or 1
 * \return             none
 */

void atserver_set_echo(uint8_t flag)
{
    g_atserver.echo_flag = flag;
}

/**
 * \brief              geit echo flag for at-cmd
 * \return             0:not need echo 1:need echo cmd string
 */

uint8_t atserver_get_echo(void)
{
    return g_atserver.echo_flag;
}

/**
 * \brief              get echo flag for at-cmd
 * \return             0:not need echo 1:need echo cmd string
 */

int atserver_add_command(const atserver_cmd_t cmd[])
{
    int len;

    if(cmd == NULL)
        return -1;

    len = atserver_cmd_check_group(cmd);

    if (len < 0) {
        return (-EINVAL);
    }

    atcmd_node_t *node = (atcmd_node_t *)malloc(sizeof(atcmd_node_t));
    if (node) {
        node->at = cmd;
        node->len = len;
        slist_add(&node->next, &g_atserver.cmd_head);
        return 0;
    } else {
        return -ENOMEM;
    }
}

/**
 * \brief              get cmd line
 * \return             if exist one cmd line in io-buff,then return line buffer pointer
 *                     otherwise reutrn NULL
 */

static char* atserver_read_cmd_line(void)
{

    char           c = 0;
    uint32_t       bValidCmdLine = 0;
    char*          pstr = NULL;
    int            read_cnt;

    while (g_atserver.cmd_line_st <= CMD_LINE_FEED)
    {
        /* get char  if need feeding */
        if(g_atserver.cmd_line_st > CMD_LINE_INIT)
        {
            read_cnt = tst_io_rcv(&c,1);
            if (read_cnt == 0)
            {
                break;
            }
            //printf("get byte:%02x\n",c);
        }

        /*  AT line process */
        switch (g_atserver.cmd_line_st)
        {
            case CMD_LINE_INIT:
                /* init process for AT FSM */

                memset(g_atserver.cmd_line_buf,0,g_atserver.cmd_line_sz);
                g_atserver.cmd_line_valid_cnt = 0;
                g_atserver.cmd_line_st = CMD_LINE_FEEDING;
                break;

            case CMD_LINE_FEEDING:
                /*feed char */

                if ((c == CR_T || c == LF) && g_atserver.cmd_line_valid_cnt > 0 )
                {
                    g_atserver.cmd_line_buf[g_atserver.cmd_line_valid_cnt++] = '\0';
                    g_atserver.cmd_line_st = CMD_LINE_FEED;
                }
                else if ((c == CR_T || c == LF) && g_atserver.cmd_line_valid_cnt == 0 )
                {
                    //discard
                    break;
                }
                else
                {
                     g_atserver.cmd_line_buf[g_atserver.cmd_line_valid_cnt++] = c;
                }
                break;
            case CMD_LINE_FEED:

                /*
                 *  check current line  sometimes device will receive ATATxxx like string,
                 *  we need ignore redundant char
                 */
                #if 0
                pstr = NULL;
                pstr_temp = g_atserver.cmd_line_buf;
                do
                {
                    pstr_temp = strstr(pstr_temp,"AT");
                    if(pstr_temp)
                    {
                        pstr = pstr_temp;
                        pstr_temp +=2;
                    }

                }while(pstr_temp);
                #else
                pstr = g_atserver.cmd_line_buf;
                #endif

                if(pstr == g_atserver.cmd_line_buf)
                {
                    bValidCmdLine = 1;
                }
                else if (pstr != NULL && pstr != g_atserver.cmd_line_buf)
                {
                    uint32_t len = strlen(pstr);
                    memmove(g_atserver.cmd_line_buf,pstr,len);
                    g_atserver.cmd_line_buf[len] = '\0';
                    g_atserver.cmd_line_valid_cnt = len;
                    bValidCmdLine = 1;
                }
                else
                {
                    AT_BACK_ERR();
                    bValidCmdLine = 0;
                }
                g_atserver.cmd_line_st++;
                break;
        }
    }

    /*reset cmd line status */
    if(g_atserver.cmd_line_st > CMD_LINE_FEED)
    {
        /* exit from CMD_LINE_FEED ,so we get one line */
        g_atserver.cmd_line_st = CMD_LINE_INIT;
    }
    return (bValidCmdLine)?g_atserver.cmd_line_buf:NULL;

}

/**
 * \brief              get cmd line type and cmd tag length
 * \param              type parser cmd string and get cmd line type
 *                     there are four cmd type in toal as follow
 *                     TEST_CMD/WRITE_CMD/READ_CMD/EXCUTE_CMD
 * \return             n: cmd tag length -1: err
 */

static int atserver_get_cmdlen(AT_CMD_TYPE *type,char* pcmdline)
{
    int i;
    char *str = pcmdline;

    for (i = 0; i <= strlen(pcmdline); i++) {
        if (*str == '=') {
            if (*(str + 1) == '?') {
                *type = TEST_CMD;
            } else {
                *type = WRITE_CMD;
            }

            return i;
        } else if (*str == '?') {
            *type = READ_CMD;
            return i;
        } else if (*str == '\0') {
            *type = EXECUTE_CMD;
            return i;
        } else {
            str++;
        }
    }

    return -1;
}

/**
 * \brief              get param number from write-cmd
 * \param              type parser cmd string and get cmd line type
 *                     there are four cmd type in toal as follow
 *                     TEST_CMD/WRITE_CMD/READ_CMD/EXCUTE_CMD
 * \return             n: cmd param number -1 :err
 */

static int get_para_num(void)
{
    int argc = 0;
    char *p;
    char para_delimiter = g_atserver.para_delimiter;

    p = strstr(g_atserver.cmd_line_buf, "=");

    if (p != NULL) {
        if (*(p + 1) != '\0') {
            argc = 1;

            while (*p) {
                if (*p == para_delimiter) {
                    argc++;
                }

                p++;
            }
        } else {
            return -1;
        }
    }

    return argc;
}

static int para_split_by_delimiter(char *buf, char **argv)
{
    int argc = 0;
    char *p;
    int i;
    char para_delimiter = g_atserver.para_delimiter;

    if (buf == NULL) {
        return argc;
    }

    argv[argc++] = buf;

    while (*buf != '\0') {
        if (*buf == para_delimiter) {
            *buf = '\0';

            if ((*(buf + 1) == '\0') || (*(buf + 1) == para_delimiter)) {
                argv[argc++] = NULL;
            } else {
                argv[argc++] = buf + 1;
            }
        }

        buf++;
    } //cut para

    for (i = 0; i < argc; i++) {
        p = argv[i];

        while (*p != '\0') {
            if (*p != ' ') {
                break;
            } else {
                p++;
            }
        }

        argv[i] = p;
    } //remove head space

    for (i = 0; i < argc; i++) {
        p = argv[i] + strlen(argv[i]) - 1;

        while (*p != '\0') {
            if (*p == ' ') {
                //*p = '\0';
                p--;
            } else {
                p[1] = '\0';
                break;
            }
        }
    } //remove tail space

    return argc;
}

static int atserver_def_cmd_process(char* pcmdline)
{
    int cmd_tag_len;
    AT_CMD_TYPE type = INVAILD_CMD;
    atcmd_node_t *node;

    /* get cmd tag length and cmd type */

    cmd_tag_len = atserver_get_cmdlen(&type,pcmdline);

    if (cmd_tag_len <= 0) {
        return DRV_ERROR_UNSUPPORTED;
    }

    /* search global cmd link-list and if find matched call-back function then process the cmd */

    slist_for_each_entry(&g_atserver.cmd_head, node, atcmd_node_t, next)
    {
        atserver_cmd_t *atcmd = (atserver_cmd_t *)node->at;

        for (int i = 0; i < node->len; i++)
        {
            if ((strncmp(pcmdline, atcmd->cmd, cmd_tag_len) == 0) && (cmd_tag_len == strlen(atcmd->cmd)))
            {
                if (atcmd->cb != NULL)
                {
                    char *param = NULL;

                    if (type == WRITE_CMD)
                    {
                        /* param point to cmd param string in case of write cmd */
                        param = strstr(pcmdline, "=");
                        param++;
                    }
                    /*  pass cmd tag/cmd type/ cmd param to user call-back function */
                    atcmd->cb((char *)atcmd->cmd, type, param);
                    return 0;
                }
            }

            atcmd++;
        }
   }

    return DRV_ERROR_UNSUPPORTED;
}

/**
 * \brief              get one test cmd line
 * \return             success return cmd line pointer otherwise return NULL
 */

char* atserver_get_test_cmdline(void)
{
    char* pCmdLine = NULL;
    int   ret;

    /* try to get a new AT cmd line from IO-buf
     * a new AT cmd line exit with '\0'
     */

    do
    {
        pCmdLine = atserver_read_cmd_line();
        if(!pCmdLine)
        {
            return NULL;
        }

        /* check whether need to echo cmd line */

        if(g_atserver.echo_flag)
        {
            atserver_send("%s\r\n",pCmdLine);
        }

        /* get one new AT and send it to default-process-handler */
        ret = atserver_def_cmd_process(pCmdLine);
    }while(ret == 0);

    /* if not processed by default-process-handler ,then send cmdlint to user */

    return pCmdLine;

}

/**
 * \brief              format AT param from cmd line
 * \param              format   format string
 * \param              ...      variable list read from cmd line string
 * \return             0:success  <0 err code
 */

int atserver_scanf(const char *format, ...)
{
    va_list args;
    int argc;
    char * argv[AT_CMD_PARAM_NUM_MAX];
    int ret = -1;
    uint32_t len;
    char temp_buf[BUFFER_MIN_SIZE];

    HANDLE_PARAM_CHK(format,-1);
    va_start(args, format);

    argc = get_para_num();

    if (argc > 0) {
        len = strlen(g_atserver.cmd_line_buf);
        if(len > (BUFFER_MIN_SIZE+1) )
        {
            return -ENOMEM;
        }
        memcpy(temp_buf, g_atserver.cmd_line_buf, len);
        temp_buf[len]='\0';

        if (argc > AT_CMD_PARAM_NUM_MAX) {

            return -ENOMEM;
        }

        char *param = NULL;

        param = strstr(temp_buf, "=");
        param++;
        argc = para_split_by_delimiter(param, argv);

        int len = 0;
        int offset = 0;

        if (argv[0] != NULL) {
            len = strlen(argv[0]);
            memmove(temp_buf,argv[0],len);
            temp_buf[len] = 0;
            offset += len;
        }

        for (int i = 1; i < argc; i++) {
            temp_buf[offset] = g_atserver.para_delimiter;
            offset++;
            len = 0;

            if (argv[i] != NULL) {
                len = strlen(argv[i]);
                memmove(temp_buf + offset, argv[i],len);
                offset += len;
                temp_buf[offset] = 0;
            }
        }

        temp_buf[offset] = '\0';
        ret = vsscanf(temp_buf, format, args);

    }

    va_end(args);

    if (ret < 0) {
        return -EINVAL;
    } else {
        return ret;
    }
}

/**
 * \brief              write data to IO
 * \param              data        data pointer
 * \param              size        data length
 * \return             written bytes
 */

int atserver_write(const void *data, int size)
{

    return tst_io_send((char*)data, size);

}

int atserver_sendv(const char *command, va_list args)
{
    int ret = -EINVAL;
    char send_buf[256];
    int  len;

    len = vsnprintf(send_buf, 256, command, args);
    if (len >= 0) {
        ret = tst_io_send((char*)send_buf, strlen(send_buf));

        if (ret == 0 && g_atserver.output_terminator != NULL) {
            ret = tst_io_send((char*)g_atserver.output_terminator, strlen(g_atserver.output_terminator));
        }

    }

    return 0;
}

/**
 * \brief              send response string
 * \param              command   format string for at-response
 * \param              ... variable list
 * \return             success return cmd line pointer otherwise return NULL
 */

int atserver_send(const char *command, ...)
{
    int ret;
    va_list args;

    va_start(args, command);

    ret = atserver_sendv(command, args);

    va_end(args);

    return ret;
}



