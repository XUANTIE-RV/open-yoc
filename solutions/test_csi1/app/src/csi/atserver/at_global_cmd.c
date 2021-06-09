/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     at_global_cmd.c
 * @brief    C file for global AT define and handler
 * @version  V1.0
 * @date     2020-02-17
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/errno.h>
#include "at_global_cmd.h"
#include "atserver.h"
#include "autotest.h"

uint32_t      g_tst_dev_idx;


aux_bord_pin_t aux_switch_pin_map[] = {
{"UART_RX",      0},
{"UART_TX",      1},
{"UART_RTS",     2},
{"UART_CTS",     3},
{"",0},

};

/**< test Called when recv cmd is AT+<x>=? TEST_CMD*/
/**< query Called when recv cmd is AT+<x>? READ_CMD*/
/**< setup Called when recv cmd is AT+<x>=<...> WRITE_CMD*/
/**< exec Called when recv cmd is AT+<x> EXECUTE_CMD*/

const atserver_cmd_t common_at_cmd[] = {
    AT,
    AT_HELP,
/*    AT_PINCFGA, */
/*    AT_PINCFGB, */
    AT_NULL,
};




void at_cmd_at(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        AT_BACK_OK();
    }
}

void at_cmd_help(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        atserver_send("\r\n%s:", cmd + 2);
        int i = 0;

        while (common_at_cmd[i++].cmd) {
            if (common_at_cmd[i].cmd[2] == '+') {
                atserver_send("%s;", common_at_cmd[i].cmd + 3);
            }
        }

        atserver_send("\r\nOK\r\n");
    }
}

#if 0
void at_cmd_pincofig_dut(char *cmd, int type, char *data)
{
    uint32_t         dev_idx;
    uint32_t         pin_port;
    uint32_t         pin_offset;
    uint32_t         pin_func;
    int param_num            ;
    int ret;
    g_tst_dev_idx = 0;

    if (type == WRITE_CMD ) {
        param_num = atserver_scanf("%d,%d,%d,%d",&dev_idx,&pin_port,&pin_offset,&pin_func);
        if(param_num < 4)
        {
            AT_LOG("[%s=%s]:param error",cmd,data);
            AT_BACK_ERR();
            return;
        }
        AT_LOG("AT+PINCFGA:dev_idx[%d]port[%d]pinindex[%d]pin_func[%d]",dev_idx,pin_port,pin_offset,pin_func);

        ret = csi_pin_set_mux(pin_port, pin_offset, pin_func);
        if(ret == CSI_OK)
        {
            AT_BACK_OK();
        }
        else
        {
            AT_BACK_ERR();
        }
    }
    g_tst_dev_idx = dev_idx;
    return;
}

uint32_t at_cmd_get_tst_dev_id(void)
{
    return g_tst_dev_idx;
}


void at_cmd_pincofig_aux(char *cmd, int type, char *data)
{
    char pin_name_str[32] = "";
    uint32_t         pin_port;
    uint32_t         pin_offset;
    uint32_t         pin_func;
    int param_num            ;
    int ret;
    if (type == WRITE_CMD) {
         param_num = atserver_scanf("%[^,],%d,%d,%d",pin_name_str,&pin_port,&pin_offset,&pin_func);
         if(param_num < 4)
         {
             AT_LOG("[%s=%s]:param error param_num:%d",cmd,data,param_num);
             AT_BACK_ERR();
             return;
         }

         AT_LOG("AT+PINCFGB:pinname[%s]port[%d]pinindex[%d]pin_func[%d]",pin_name_str,pin_port,pin_offset,pin_func);
         ret = csi_pin_set_mux(pin_port, pin_offset, pin_func);
         if(ret == CSI_OK)
         {
             AT_BACK_OK();
         }
         else
         {
             AT_BACK_ERR();
         }

     }

}

#endif
