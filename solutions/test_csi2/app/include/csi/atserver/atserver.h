/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     atserver.h
 * @brief    head file for AT server parser
 * @version  V1.0
 * @date     2020-02-17
 ******************************************************************************/


#ifndef YOC_AT_SERVER_H
#define YOC_AT_SERVER_H

#include <stdarg.h>
#include <drv/uart.h>


#ifdef __cplusplus
extern "C" {
#endif

/*！
 * \def     LF
 * \brief   default line-feed code
 */

#ifdef LF
#undef LF
#define LF  10
#else
#define LF  10
#endif

/*！
 * \def     CR
 * \brief   default carrige-return code
 */

#ifdef CR
#undef CR
#define CR  13
#else
#define CR  13
#endif

/*！
 * \def     BUFFER_XXX_SIZE
 * \brief   defaut setting for AT line bufer length
 */

#define BUFFER_MIN_SIZE (1024)
#define BUFFER_MAX_SIZE (1024)
#define BUFFER_STEP_SIZE (32)
#define AT_CMD_PARAM_NUM_MAX (16)
#define UART_BUF_SIZE 16

/*！
 * \enum     AT_COMD_TYPE
 * \brief    const type define for AT-CMD
 */

typedef enum
{
    TEST_CMD,                                           ///< test cmd
    READ_CMD,                                           ///< read cmd
    WRITE_CMD,                                          ///< write cmd
    EXECUTE_CMD,                                        ///< execute cmd
    INVAILD_CMD,
} AT_CMD_TYPE;

typedef void (*cmd_callback)(char *cmd, int type, char *data);

typedef struct atserver_cmd {
    const char        *cmd;
    const cmd_callback cb;
} atserver_cmd_t;


/*！
 * \def     atcmd_node
 * \brief   node design for AT cmd single link list
 */

typedef struct atcmd_node {
    const atserver_cmd_t   *at;                         ///< point to user-defined's AT sets
    int                     len;                        ///< at number of user-defined's AT sets
    slist_t                 next;                       ///< single link pointer
} atcmd_node_t;

typedef enum {
    CMD_LINE_INIT,                                      ///< cmd line empty
    CMD_LINE_FEEDING,                                   ///< cmd line buf in feeding status
    CMD_LINE_FEED                                       ///< cmd line feed
}at_cmd_line_st_t;

typedef struct atserver {
    char             *cmd_line_buf;                     ///< line buffer for AT CMD
    uint16_t         cmd_line_sz;                       ///< line buffer length
    uint16_t         cmd_line_valid_cnt;                ///< valid char in cmd line
    at_cmd_line_st_t cmd_line_st;                       ///< cmd line status
    uint8_t          echo_flag;                         ///< AT echo flag
    char             para_delimiter;                    ///< delimiter char for AT parameters
    const char       *output_terminator;                ///< ????
    slist_t          cmd_head;                          ///< list head for user-at cmd link list
} atserver_uservice_t;


int  atserver_init(void);
int  atserver_sendv(const char *command, va_list args);
void atserver_set_output_terminator(const char *terminator);
void atserver_set_para_delimiter(char delimiter);
void atserver_set_timeout(int timeout);
uint8_t atserver_get_echo(void);
void atserver_set_echo(uint8_t flag);


#ifdef __cplusplus
}
#endif


#endif
