/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     at_global_cmd.h
 * @brief    head file for global AT define
 * @version  V1.0
 * @date     2020-02-17
 ******************************************************************************/

#ifndef _AT_GLOBAL_CMD_H_
#define _AT_GLOBAL_CMD_H_
/*!
 * \struct aux_bord_pin_t
 * \brief  pin map on auxiliary board
 *  we design a switch matrix for peripheral device test
 *  matrix demetion is An x Bm, A side is connect to hw-pin on dut board(design under test board)
 *  and B side is connect hw-pin on auxiliary board by switching matrix array  hw signal from A side can be pass
 *  to any pin on B side.
 *
 *  for test convenience, all B pin function is pre-defined and fixed
 *        -----------
 *     A0-|---------|-B0-uart_rx
 *     A1-|---------|-B1-uart_tx
 *     A2-|---------|-B2-uart_rts
 *     A3-|---------|-B3-uart_rts
 *    ...-|         |-...
 *     An-|---------|-Bm-xxxx
 */
typedef struct{
    const char*          pin_tag;
    int                  B_idx;
}aux_bord_pin_t;

#define AT              {"AT",                   at_cmd_at}
#define AT_HELP         {"AT+HELP",              at_cmd_help}
#define AT_NULL         {NULL,                   NULL}

void at_cmd_at(char *cmd, int type, char *data);
void at_cmd_help(char *cmd, int type, char *data);
//void at_cmd_pincofig_dut(char *cmd, int type, char *data);
//void at_cmd_pincofig_aux(char *cmd, int type, char *data);




#endif

