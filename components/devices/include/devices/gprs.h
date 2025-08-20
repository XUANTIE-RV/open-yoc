 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _DEVICE_GPRS_MODULE_H
#define _DEVICE_GPRS_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

/* Define platform endianness */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

#define SIM800_MAX_LINK_NUM  5

typedef struct {
    int rssi;
    int ber;
} rvm_hal_sig_qual_resp_t;

typedef struct {
    int n;
    int stat;
} rvm_hal_regs_stat_resp_t;

/* Change to include data slink for each link id respectively. <TODO> */

typedef enum {
    AT_TYPE_NULL = 0,
    AT_TYPE_TCP_SERVER,
    AT_TYPE_TCP_CLIENT,
    AT_TYPE_UDP_UNICAST,
    AT_TYPE_MAX
} rvm_hal_at_conn_e;

typedef enum {
    SAL_INIT_CMD_RETRY = 21,
    SAL_INIT_CMD_SIMCARD,
    SAL_INIT_CMD_QUELITY,
    SAL_INIT_CMD_GPRS_CLOSE,
    SAL_INIT_CMD_PDPC,
    SAL_INIT_CMD_GPRSMODE,
    SAL_INIT_CMD_CREG,
    SAL_INIT_CMD_GATT,
    SAL_INIT_CMD_MUX,
    SAL_INIT_CMD_CSTT,
    SAL_INIT_CMD_CIICR,
    SAL_INIT_CMD_END,
} rvm_hal_enum_init_cmd_e;

#define SAL_MODULE_RESET  0x55

typedef enum {
    GPRS_STATUS_LINK_DISCONNECTED = 0,
    GPRS_STATUS_LINK_CONNECTED,
} rvm_hal_gprs_status_link_t;

typedef enum {
    CONNECT_MODE_CSD = 0,    /**< CSD mode */
    CONNECT_MODE_GPRS,       /**< GPRS mode */
} rvm_hal_gprs_mode_t;

#define rvm_hal_gprs_open(name) rvm_hal_device_open(name)
#define rvm_hal_gprs_close(dev) rvm_hal_device_close(dev)

typedef void (*recv_sms_cb)(char *oa, char *content, int length);

int rvm_hal_gprs_set_mode(rvm_dev_t *dev, rvm_hal_gprs_mode_t mode);
int rvm_hal_gprs_get_mode(rvm_dev_t *dev, rvm_hal_gprs_mode_t *mode);
int rvm_hal_gprs_reset(rvm_dev_t *dev);
int rvm_hal_gprs_start(rvm_dev_t *dev);
int rvm_hal_gprs_stop(rvm_dev_t *dev);
/*configuration*/
int rvm_hal_gprs_set_if_config(rvm_dev_t *dev, uint32_t baud, uint8_t flow_control);
/*connection*/
int rvm_hal_gprs_module_init_check(rvm_dev_t *dev);
int rvm_hal_gprs_connect_to_gprs(rvm_dev_t *dev);
int rvm_hal_gprs_disconnect_from_gprs(rvm_dev_t *dev);
int rvm_hal_gprs_get_link_status(rvm_dev_t *dev, rvm_hal_gprs_status_link_t *link_status);
int rvm_hal_gprs_get_ipaddr(rvm_dev_t *dev, char ip[16]);

int rvm_hal_gprs_sms_send(rvm_dev_t *dev, char *sca, char *da, char *content);
int rvm_hal_gprs_sms_set_cb(rvm_dev_t *dev, recv_sms_cb cb);

int rvm_hal_gprs_get_imsi(rvm_dev_t *dev, char *imsi);
int rvm_hal_gprs_get_imei(rvm_dev_t *dev, char *imei);

int rvm_hal_gprs_get_csq(rvm_dev_t *dev, int *csq);
int rvm_hal_gprs_get_simcard_info(rvm_dev_t *dev, char ccid[21], int *insert);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_gprs.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /*_WIFI_MODULE*/
