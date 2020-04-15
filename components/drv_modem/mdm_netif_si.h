/**
 * File: mdm_netif_si.h
 * Brief: Public APIs of Sanechips
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

#ifndef _MDM_DEV_H
#define _MDM_DEV_H

#include <arpa/inet.h>
#include <k_api.h>
#include <k_timer.h>
#include <k_list.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/

#define MDM_CMD_SEND  0
#define MDM_CMD_FREE  1

#define MDM_PACKET_RECV 0//统计用
#define MDM_PACKET_SEND 1

//UP模式IP包前cid和modem数据头预留长度
#define CID_RESERVE 1  //给cid预留的1个字节长度
#define IP_HEAD_RESERVE 15  ////ip头前预留的15个字节长度

//激活模式:CP,UP
#define MDM_CP_MOD 0x01
#define MDM_UP_MOD 0x02

#define DATA_VIA_CP     0
#define DATA_VIA_UP     1

#define LOCATION_CID 1

#define ADDR_RECORD_NUM 16  //该值大小与核间rebuf的大小相关，后续考虑将该处值改为和核间通信相关联的值


/*初始值，不进行任何操作*/
#define V4V6_INVALID   0x00
/*仅V4相关的信息有效，例如PDP激活信息*/
#define V4_VALID       0x01
/*仅V6相关的信息有效，例如PDP激活信息*/
#define V6_VALID       0x02
/*V4和V6相关的信息有效，例如PDP激活信息*/
#define V46_VALID      0x03

/*PDP没激活.初始值*/
#define PDP_V4V6_INACTIVE   V4V6_INVALID
/*PDP V4协议激活*/
#define PDP_V4_ACTIVE       V4_VALID
/*PDP V6协议激活*/
#define PDP_V6_ACTIVE       V6_VALID

/* Represents the PDP activation initiated by the local application, eg MMS,VoLTE */
#define PDP_LOCAL     0
#define PDP_NORMAL    2

#ifdef CONFIG_MODEM_DATA_ADDRESS_RECORD
#define MDM_DATA_ADDR_RECORD(data)  mdm_data_addr_record(data)
#define MDM_DATA_ADDR_FIND(data)  mdm_data_addr_find(data)
#else
#define MDM_DATA_ADDR_RECORD(data)
#define MDM_DATA_ADDR_FIND(data)
#endif

#define MDM_PACKET_NUM_PRINT()  mdm_pakcet_num_print()

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
/* PDP activation information,when the argument is 0, no valid value is indicated */
struct pdp_ip_info {

    /* eg V4V6_INVALID.. */
    u16_t ip46flag;
    u32_t ip;
    u32_t gateway;
    u32_t pri_dns;
    u32_t sec_dns;
    struct in6_addr ipv6;
};

//PDP激活信息，专用于PS外网口，其中cid用于识别不同的PS外网口
struct pdp_active_info {
    unsigned char c_id;//PDP唯一标识
    unsigned char mod_flags;
    unsigned char pdp_type;//参见PDP_LOCAL等宏值；
    struct pdp_ip_info act_info;
};

//PDP去激活信息，专用于PS外网口，其中cid用于识别不同的PS外网口
struct pdp_deactive_info {
    int c_id;//PDP唯一标识
    unsigned char ip46flag;
};

struct  modem_msg {

    u32_t cmd: 1;
    u32_t mod: 1;
    u32_t offset: 6;
    u32_t padding: 8;
    u32_t len: 16;
    void *data;
};

struct send_data_node {
    klist_t send_list;
    struct modem_msg *msg;
};

/*******************************************************************************
 *                       Global variable declarations                          *
 ******************************************************************************/

/*******************************************************************************
 *                       Global function declarations                          *
 ******************************************************************************/

int mdm_modflag_set(unsigned char mod_flags);

void mdm_cp_recv(void *data, unsigned int len, unsigned int c_id);

int mdm_netif_pdp_act(struct pdp_active_info *actinfo);

int mdm_netif_pdp_deact(unsigned int c_id, unsigned char ip46flag);

void mdm_psm_ref(unsigned int cmd);

void mdm_psm_unref();

int mdm_at_init(void);

int mdm_api_init(void);
/*******************************************************************************
 *                      Inline function implementations                        *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  // #ifndef _MDM_DEV_H


