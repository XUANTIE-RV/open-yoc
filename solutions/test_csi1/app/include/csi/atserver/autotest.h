/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     autotest.h
 * @brief    Header File for autotest
 * @version  V1.0
 * @date     2020-02-21
 * @model    common
 ******************************************************************************/

#ifndef _AUTO_TEST_H_
#define _AUTO_TEST_H_

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include <drv/errno.h>
#include "drv/usart.h"
//#include "drv/tick.h"
#include "dev_ringbuf.h"
#include "csi_config.h"
#include "tst_io.h"
#include "list.h"
#include "atserver.h"
#include "at_global_cmd.h"
//#include "board_init.h"
#include "tst_utility.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*
*                        utility api
*
*******************************************************************************/

/**
 * \brief              restart timer
 * \return             none
 */

void tst_timer_restart(void);

/**
 * \brief              get interval from last timer begining
 * \return             interval time unit in mini-second
 */

uint32_t tst_timer_get_interval(void);

/**
 * \brief              do polling delay
 * \param              cnt_ms     time to be delay ,unit in mini-second
 * \return             none
 */

void tst_mdelay(int cnt_ms);

/*******************************************************************************
*
*                        atserver api
*
*******************************************************************************/

/**
 * \brief              init auto test service
 * \param              at_uart        io config
 * \return             err code
 */

int32_t tst_init(tst_uart_config_t* at_uart);

/**
 * \brief              get one test cmd line
 * \return             success return cmd line pointer otherwise return NULL
 */
char* atserver_get_test_cmdline(void);

/**
 * \brief              send response string
 * \param              command   format string for at-response
 * \param              ... variable list
 * \return             0:success  <0: err code
 */

int atserver_send(const char *command, ...);

/**
 * \brief              get echo flag for at-cmd
 * \return             0:not need echo 1:need echo cmd string
 */

int atserver_add_command(const atserver_cmd_t cmd[]);


/**
 * \brief              format AT param from cmd line
 * \param              format   format string
 * \param              ...      variable list read from cmd line string
 * \return             0:success  <0 err code
 */

int atserver_scanf(const char *format, ...);

/**
 * \brief              write data to IO
 * \param              data        data pointer
 * \param              size        data length
 * \return             written bytes
 */

int atserver_write(const void *data, int size);

/**
 * \brief              get device id
 * \                   device id is configed by AT+PINCFGA cmd
 * \return             device id to be test
 */

uint32_t at_cmd_get_tst_dev_id(void);


#define AT_BACK_CMD(cmd)                      atserver_send("%s\r\n", cmd)

#define AT_BACK_OK()                          atserver_send("\r\nOK\r\n")

#define AT_BACK_ERR()                         atserver_send("\r\nERROR\r\n")

#define AT_BACK_ERRNO(errno)                  atserver_send("\r\nERROR: %d\r\n", errno)

#define AT_LOG(fmt,...)  \
        do \
        { \
            atserver_send("+LOG:"); \
            atserver_send(fmt,##__VA_ARGS__); \
            atserver_send("\r\n"); \
        }while(0)


#ifdef __cplusplus
}
#endif

#endif
