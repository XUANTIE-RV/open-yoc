/**
 * File: mdm_ws_format.c
 * Brief: Implementation of mdm_ws_format
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author: Zhao Hao
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* Format for wireshark */

#include <stdio.h>
#include <aos/cli.h>

#include "lwip/sys.h"
#include "lwip/pbuf.h"
#include "lwip/opt.h"

#include "mdm_netif_si.h"

u8_t wireshark_set = 1;

static void wireshark_format_print_data(const unsigned char *buf, size_t len)
{
    while (len--) {
        LWIP_DEBUGF(NETDOG_DEBUG, ("|%02x", *buf++));
    }
}

static void wireshark_format_print_time(void)
{
    //u16_t hour = 0;
    //u16_t min  = 0;
    //u16_t sec  = 0;
    //u16_t ms = 0;
    u32_t mss = sys_now();

    //hour = (mss % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60);
    //min  = (mss % (1000 * 60 * 60)) / (1000 * 60);
    //sec  = (mss % (1000 * 60)) / 1000;
    //ms   = (mss % 1000);

    LWIP_DEBUGF(NETDOG_DEBUG, ("%02d:%02d:%02d,%03d,000	ETHER\n",
                                (mss % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60),
                                (mss % (1000 * 60 * 60)) / (1000 * 60),
                                (mss % (1000 * 60)) / 1000,
                                (mss % 1000) ));
}

void wireshark_format_print(struct pbuf *buf, bool type)
{
    struct pbuf *buffer = buf;
    struct pbuf *temp_buffer = NULL;

    if (!wireshark_set) {
        return;
    }

    LWIP_DEBUGF(NETDOG_DEBUG, ("+---------+---------------+----------+\n"));
    wireshark_format_print_time();

    if (type == MDM_PACKET_RECV) {
        LWIP_DEBUGF(NETDOG_DEBUG, ("|0   |00|00|00|11|00|00|00|00|00|11|00|00|08|00"));
    } else if (type == MDM_PACKET_SEND) {
        LWIP_DEBUGF(NETDOG_DEBUG, ("|0   |00|00|00|00|00|00|00|00|00|00|00|00|08|00"));
    } else {
        LWIP_DEBUGF(NETDOG_DEBUG, ("ip packet is not recv,nor send type!/n"));
        return;
    }

    do {
        temp_buffer = buffer->next;

        if (temp_buffer == NULL) {
            wireshark_format_print_data((const unsigned char *)(buffer->payload), buffer->len + 1) ;
        } else {
            wireshark_format_print_data((const unsigned char *)(buffer->payload), buffer->len) ;
        }

        buffer = buffer->next;
    } while (buffer);

    LWIP_DEBUGF(NETDOG_DEBUG, ("\n\n\n"));
}

#if 0
static uint32_t netdog_shell_cmd_wireshark(cmd_item_t *item, struct cli_command *info)
{
    int set_test = 0;
    LWIP_DEBUGF(NETDOG_DEBUG, ("wireshark ori:%d \n", wireshark_set));

    set_test = atoi(item->items[1]);

    if (set_test == 1 || set_test == 0) {
        wireshark_set = set_test;
        LWIP_DEBUGF(NETDOG_DEBUG, ("wireshark set to:%d \n", wireshark_set));
    } else {
        LWIP_DEBUGF(NETDOG_DEBUG, ("input parameter must be 1 or 0, wireshark now :%d \n", wireshark_set));
    }

    return OK;
}

void ysh_reg_cmd_wireshark(void)
{
    static const struct cli_command cmd_info =
    {
        "wireshark",
        "wireshark command.",
        "wireshark :\r\n wireshark 0 or 1 : .",
        netdog_shell_cmd_wireshark
    };

    cli_service_reg_cmd(&cmd_info);

    return;
}
#endif
