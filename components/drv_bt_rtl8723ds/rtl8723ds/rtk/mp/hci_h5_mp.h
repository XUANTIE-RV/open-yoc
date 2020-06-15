/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include <stdint.h>


//HCI Command opcodes
#define HCI_LE_READ_BUFFER_SIZE     0x2002
#define DATA_TYPE_H5                0x05

//HCI VENDOR Command opcode
#define HCI_VSC_H5_INIT                 0xFCEE
#define HCI_VSC_UPDATE_BAUDRATE         0xFC17
#define HCI_VSC_DOWNLOAD_FW_PATCH       0xFC20
#define HCI_VSC_READ_ROM_VERSION        0xFC6D
#define HCI_VSC_READ_CHIP_TYPE          0xFC61
#define HCI_VSC_SET_WAKE_UP_DEVICE      0xFC7B
#define HCI_VSC_BT_OFF                  0xFC28

//HCI Event codes
#define HCI_CONNECTION_COMP_EVT             0x03
#define HCI_DISCONNECTION_COMP_EVT          0x05
#define HCI_COMMAND_COMPLETE_EVT            0x0E
#define HCI_COMMAND_STATUS_EVT              0x0F
#define HCI_NUM_OF_CMP_PKTS_EVT             0x13
#define HCI_BLE_EVT                         0x3E

#define BT_HCI_EVT_LE_ADVERTISING_REPORT        0x02

typedef enum {
    DATA_TYPE_NONE    = 0,
    DATA_TYPE_COMMAND = 1,
    DATA_TYPE_ACL     = 2,
    DATA_TYPE_SCO     = 3,
    DATA_TYPE_EVENT   = 4
} hci_data_type_t;

typedef void (*packet_recv)(hci_data_type_t type, uint8_t *data, uint32_t len);

typedef struct h5_mp_t {
     void     (*h5_int_init)(packet_recv h5_callbacks);
     void     (*h5_int_cleanup)(void);
     uint16_t (*h5_send_cmd)(hci_data_type_t type, uint8_t *data, uint16_t length);
     uint8_t  (*h5_send_sync_cmd)(uint16_t opcode, uint8_t *data, uint16_t length);
     uint16_t (*h5_send_acl_data)(hci_data_type_t type, uint8_t *data, uint16_t length);
} h5_mp_t;

const h5_mp_t *get_h5_mp_interface(void);


