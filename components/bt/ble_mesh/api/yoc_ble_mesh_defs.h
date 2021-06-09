// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _BLE_MESH_BLE_MESH_DEFS_H_
#define _BLE_MESH_BLE_MESH_DEFS_H_

#include <stdint.h>

#include "yoc_bt_defs.h"
#include "bt_config.h"

// #include "mesh_proxy.h"
// #include "mesh_access.h"
// #include "mesh_main.h"

// #include "mesh.h"
// #include "proxy.h"
// #include "foundation.h"
// #include "provisioner_main.h"

// #include "model_opcode.h"
// #include "mesh_common.h"

/*!< The maximum length of a BLE Mesh message, including Opcode, Payload and TransMIC */
#ifndef BIT
#define BIT(n)      (1UL << (n))
#endif

#ifndef BIT_MASK
#define BIT_MASK(n) (BIT(n) - 1)
#endif

#define ZERO_OR_COMPILE_ERROR(cond) ((int) sizeof(char[1 - 2 * !(cond)]) - 1)

/* Evaluates to 0 if array is an array; compile error if not array (e.g.
 * pointer)
 */
#define IS_ARRAY(array)                                     \
    ZERO_OR_COMPILE_ERROR(                                  \
        !__builtin_types_compatible_p(__typeof__(array),    \
                          __typeof__(&(array)[0])))

#define ARRAY_SIZE(array)               \
    ((unsigned long) (IS_ARRAY(array) + \
        (sizeof(array) / sizeof((array)[0]))))


#define DEVICE_NAME_SIZE        29


#define BLE_MESH_KEY_PRIMARY 0x0000
#define BLE_MESH_KEY_ANY     0xffff


#define BLE_MESH_ADDR_UNASSIGNED   0x0000
#define BLE_MESH_ADDR_ALL_NODES    0xffff
#define BLE_MESH_ADDR_PROXIES      0xfffc
#define BLE_MESH_ADDR_FRIENDS      0xfffd
#define BLE_MESH_ADDR_RELAYS       0xfffe

#define BLE_MESH_KEY_UNUSED        0xffff
#define BLE_MESH_KEY_DEV           0xfffe

#define BLE_MESH_MODEL_OP_1(b0)         (b0)
#define BLE_MESH_MODEL_OP_2(b0, b1)     (((b0) << 8) | (b1))
#define BLE_MESH_MODEL_OP_3(b0, cid)    ((((b0) << 16) | 0xc00000) | (cid))

#define BLE_MESH_MODEL_OP_END           { 0, 0, NULL }
#define BLE_MESH_MODEL_NO_OPS           ((struct bt_mesh_model_op []) \
                                        { BLE_MESH_MODEL_OP_END })

/** Helper to define an empty model array */
#define BLE_MESH_MODEL_NONE ((struct bt_mesh_model []){})

#define BLE_MESH_MODEL(_id, _op, _pub, _user_data)                  \
{                                                                   \
    .id = (_id),                                                    \
    .op = _op,                                                      \
    .keys = { [0 ... (CONFIG_BLE_MESH_MODEL_KEY_COUNT - 1)] =       \
            BLE_MESH_KEY_UNUSED },                                  \
    .pub = _pub,                                                    \
    .groups = { [0 ... (CONFIG_BLE_MESH_MODEL_GROUP_COUNT - 1)] =   \
            BLE_MESH_ADDR_UNASSIGNED },                             \
    .user_data = _user_data,                                        \
}

#define BLE_MESH_MODEL_VND(_company, _id, _op, _pub, _user_data)    \
{                                                                   \
    .vnd.company = (_company),                                      \
    .vnd.id = (_id),                                                \
    .op = _op,                                                      \
    .pub = _pub,                                                    \
    .keys = { [0 ... (CONFIG_BLE_MESH_MODEL_KEY_COUNT - 1)] =       \
            BLE_MESH_KEY_UNUSED },                                  \
    .groups = { [0 ... (CONFIG_BLE_MESH_MODEL_GROUP_COUNT - 1)] =   \
            BLE_MESH_ADDR_UNASSIGNED },                             \
    .user_data = _user_data,                                        \
}

/** @def BLE_MESH_TRANSMIT
 *
 *  @brief Encode transmission count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0
 *                 and a multiple of 10.
 *
 *  @return Mesh transmit value that can be used e.g. for the default
 *          values of the configuration model data.
 */
#define BLE_MESH_TRANSMIT(count, int_ms) ((count) | (((int_ms / 10) - 1) << 3))

/** @def BLE_MESH_TRANSMIT_COUNT
 *
 *  @brief Decode transmit count from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission count (actual transmissions is N + 1).
 */
#define BLE_MESH_TRANSMIT_COUNT(transmit) (((transmit) & (u8_t)BIT_MASK(3)))

/** @def BLE_MESH_TRANSMIT_INT
 *
 *  @brief Decode transmit interval from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define BLE_MESH_TRANSMIT_INT(transmit) ((((transmit) >> 3) + 1) * 10)

/** @def BLE_MESH_PUB_TRANSMIT
 *
 *  @brief Encode Publish Retransmit count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0
 *                 and a multiple of 50.
 *
 *  @return Mesh transmit value that can be used e.g. for the default
 *          values of the configuration model data.
 */
#define BLE_MESH_PUB_TRANSMIT(count, int_ms) BLE_MESH_TRANSMIT(count, (int_ms) / 5)

/** @def BLE_MESH_PUB_TRANSMIT_COUNT
 *
 *  @brief Decode Pubhlish Retransmit count from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Retransmission count (actual transmissions is N + 1).
 */
#define BLE_MESH_PUB_TRANSMIT_COUNT(transmit) BLE_MESH_TRANSMIT_COUNT(transmit)

/** @def BLE_MESH_PUB_TRANSMIT_INT
 *
 *  @brief Decode Publish Retransmit interval from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define BLE_MESH_PUB_TRANSMIT_INT(transmit) ((((transmit) >> 3) + 1) * 50)

#define BLE_MESH_MODEL_OP_1(b0)         (b0)
#define BLE_MESH_MODEL_OP_2(b0, b1)     (((b0) << 8) | (b1))
#define BLE_MESH_MODEL_OP_3(b0, cid)    ((((b0) << 16) | 0xc00000) | (cid))

#define BLE_MESH_NET_PRIMARY                 0x000

#define BLE_MESH_RELAY_DISABLED              0x00
#define BLE_MESH_RELAY_ENABLED               0x01
#define BLE_MESH_RELAY_NOT_SUPPORTED         0x02

#define BLE_MESH_BEACON_DISABLED             0x00
#define BLE_MESH_BEACON_ENABLED              0x01

#define BLE_MESH_GATT_PROXY_DISABLED         0x00
#define BLE_MESH_GATT_PROXY_ENABLED          0x01
#define BLE_MESH_GATT_PROXY_NOT_SUPPORTED    0x02

#define BLE_MESH_FRIEND_DISABLED             0x00
#define BLE_MESH_FRIEND_ENABLED              0x01
#define BLE_MESH_FRIEND_NOT_SUPPORTED        0x02

#define BLE_MESH_NODE_IDENTITY_STOPPED       0x00
#define BLE_MESH_NODE_IDENTITY_RUNNING       0x01
#define BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED 0x02

/* Features */
#define BLE_MESH_FEAT_RELAY                  BIT(0)
#define BLE_MESH_FEAT_PROXY                  BIT(1)
#define BLE_MESH_FEAT_FRIEND                 BIT(2)
#define BLE_MESH_FEAT_LOW_POWER              BIT(3)
#define BLE_MESH_FEAT_SUPPORTED              (BLE_MESH_FEAT_RELAY |     \
                                              BLE_MESH_FEAT_PROXY |     \
                                              BLE_MESH_FEAT_FRIEND |    \
                                              BLE_MESH_FEAT_LOW_POWER)

#define OP_APP_KEY_ADD                     BLE_MESH_MODEL_OP_1(0x00)
#define OP_APP_KEY_UPDATE                  BLE_MESH_MODEL_OP_1(0x01)
#define OP_DEV_COMP_DATA_STATUS            BLE_MESH_MODEL_OP_1(0x02)
#define OP_MOD_PUB_SET                     BLE_MESH_MODEL_OP_1(0x03)
#define OP_HEALTH_CURRENT_STATUS           BLE_MESH_MODEL_OP_1(0x04)
#define OP_HEALTH_FAULT_STATUS             BLE_MESH_MODEL_OP_1(0x05)
#define OP_HEARTBEAT_PUB_STATUS            BLE_MESH_MODEL_OP_1(0x06)
#define OP_APP_KEY_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x00)
#define OP_APP_KEY_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x01)
#define OP_APP_KEY_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x02)
#define OP_APP_KEY_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x03)
#define OP_ATTENTION_GET                   BLE_MESH_MODEL_OP_2(0x80, 0x04)
#define OP_ATTENTION_SET                   BLE_MESH_MODEL_OP_2(0x80, 0x05)
#define OP_ATTENTION_SET_UNREL             BLE_MESH_MODEL_OP_2(0x80, 0x06)
#define OP_ATTENTION_STATUS                BLE_MESH_MODEL_OP_2(0x80, 0x07)
#define OP_DEV_COMP_DATA_GET               BLE_MESH_MODEL_OP_2(0x80, 0x08)
#define OP_BEACON_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x09)
#define OP_BEACON_SET                      BLE_MESH_MODEL_OP_2(0x80, 0x0a)
#define OP_BEACON_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x0b)
#define OP_DEFAULT_TTL_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x0c)
#define OP_DEFAULT_TTL_SET                 BLE_MESH_MODEL_OP_2(0x80, 0x0d)
#define OP_DEFAULT_TTL_STATUS              BLE_MESH_MODEL_OP_2(0x80, 0x0e)
#define OP_FRIEND_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x0f)
#define OP_FRIEND_SET                      BLE_MESH_MODEL_OP_2(0x80, 0x10)
#define OP_FRIEND_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x11)
#define OP_GATT_PROXY_GET                  BLE_MESH_MODEL_OP_2(0x80, 0x12)
#define OP_GATT_PROXY_SET                  BLE_MESH_MODEL_OP_2(0x80, 0x13)
#define OP_GATT_PROXY_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0x14)
#define OP_KRP_GET                         BLE_MESH_MODEL_OP_2(0x80, 0x15)
#define OP_KRP_SET                         BLE_MESH_MODEL_OP_2(0x80, 0x16)
#define OP_KRP_STATUS                      BLE_MESH_MODEL_OP_2(0x80, 0x17)
#define OP_MOD_PUB_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x18)
#define OP_MOD_PUB_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x19)
#define OP_MOD_PUB_VA_SET                  BLE_MESH_MODEL_OP_2(0x80, 0x1a)
#define OP_MOD_SUB_ADD                     BLE_MESH_MODEL_OP_2(0x80, 0x1b)
#define OP_MOD_SUB_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x1c)
#define OP_MOD_SUB_DEL_ALL                 BLE_MESH_MODEL_OP_2(0x80, 0x1d)
#define OP_MOD_SUB_OVERWRITE               BLE_MESH_MODEL_OP_2(0x80, 0x1e)
#define OP_MOD_SUB_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x1f)
#define OP_MOD_SUB_VA_ADD                  BLE_MESH_MODEL_OP_2(0x80, 0x20)
#define OP_MOD_SUB_VA_DEL                  BLE_MESH_MODEL_OP_2(0x80, 0x21)
#define OP_MOD_SUB_VA_OVERWRITE            BLE_MESH_MODEL_OP_2(0x80, 0x22)
#define OP_NET_TRANSMIT_GET                BLE_MESH_MODEL_OP_2(0x80, 0x23)
#define OP_NET_TRANSMIT_SET                BLE_MESH_MODEL_OP_2(0x80, 0x24)
#define OP_NET_TRANSMIT_STATUS             BLE_MESH_MODEL_OP_2(0x80, 0x25)
#define OP_RELAY_GET                       BLE_MESH_MODEL_OP_2(0x80, 0x26)
#define OP_RELAY_SET                       BLE_MESH_MODEL_OP_2(0x80, 0x27)
#define OP_RELAY_STATUS                    BLE_MESH_MODEL_OP_2(0x80, 0x28)
#define OP_MOD_SUB_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x29)
#define OP_MOD_SUB_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x2a)
#define OP_MOD_SUB_GET_VND                 BLE_MESH_MODEL_OP_2(0x80, 0x2b)
#define OP_MOD_SUB_LIST_VND                BLE_MESH_MODEL_OP_2(0x80, 0x2c)
#define OP_LPN_TIMEOUT_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x2d)
#define OP_LPN_TIMEOUT_STATUS              BLE_MESH_MODEL_OP_2(0x80, 0x2e)
#define OP_HEALTH_FAULT_CLEAR              BLE_MESH_MODEL_OP_2(0x80, 0x2f)
#define OP_HEALTH_FAULT_CLEAR_UNREL        BLE_MESH_MODEL_OP_2(0x80, 0x30)
#define OP_HEALTH_FAULT_GET                BLE_MESH_MODEL_OP_2(0x80, 0x31)
#define OP_HEALTH_FAULT_TEST               BLE_MESH_MODEL_OP_2(0x80, 0x32)
#define OP_HEALTH_FAULT_TEST_UNREL         BLE_MESH_MODEL_OP_2(0x80, 0x33)
#define OP_HEALTH_PERIOD_GET               BLE_MESH_MODEL_OP_2(0x80, 0x34)
#define OP_HEALTH_PERIOD_SET               BLE_MESH_MODEL_OP_2(0x80, 0x35)
#define OP_HEALTH_PERIOD_SET_UNREL         BLE_MESH_MODEL_OP_2(0x80, 0x36)
#define OP_HEALTH_PERIOD_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x37)
#define OP_HEARTBEAT_PUB_GET               BLE_MESH_MODEL_OP_2(0x80, 0x38)
#define OP_HEARTBEAT_PUB_SET               BLE_MESH_MODEL_OP_2(0x80, 0x39)
#define OP_HEARTBEAT_SUB_GET               BLE_MESH_MODEL_OP_2(0x80, 0x3a)
#define OP_HEARTBEAT_SUB_SET               BLE_MESH_MODEL_OP_2(0x80, 0x3b)
#define OP_HEARTBEAT_SUB_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x3c)
#define OP_MOD_APP_BIND                    BLE_MESH_MODEL_OP_2(0x80, 0x3d)
#define OP_MOD_APP_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x3e)
#define OP_MOD_APP_UNBIND                  BLE_MESH_MODEL_OP_2(0x80, 0x3f)
#define OP_NET_KEY_ADD                     BLE_MESH_MODEL_OP_2(0x80, 0x40)
#define OP_NET_KEY_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x41)
#define OP_NET_KEY_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x42)
#define OP_NET_KEY_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x43)
#define OP_NET_KEY_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x44)
#define OP_NET_KEY_UPDATE                  BLE_MESH_MODEL_OP_2(0x80, 0x45)
#define OP_NODE_IDENTITY_GET               BLE_MESH_MODEL_OP_2(0x80, 0x46)
#define OP_NODE_IDENTITY_SET               BLE_MESH_MODEL_OP_2(0x80, 0x47)
#define OP_NODE_IDENTITY_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x48)
#define OP_NODE_RESET                      BLE_MESH_MODEL_OP_2(0x80, 0x49)
#define OP_NODE_RESET_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0x4a)
#define OP_SIG_MOD_APP_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x4b)
#define OP_SIG_MOD_APP_LIST                BLE_MESH_MODEL_OP_2(0x80, 0x4c)
#define OP_VND_MOD_APP_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x4d)
#define OP_VND_MOD_APP_LIST                BLE_MESH_MODEL_OP_2(0x80, 0x4e)

/* Generic OnOff Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_ONOFF_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x01)
#define BLE_MESH_MODEL_OP_GEN_ONOFF_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x02)
#define BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x03)
#define BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x04)

/* Generic Level Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_LEVEL_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x05)
#define BLE_MESH_MODEL_OP_GEN_LEVEL_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x06)
#define BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x07)
#define BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x08)
#define BLE_MESH_MODEL_OP_GEN_DELTA_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x09)
#define BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x0A)
#define BLE_MESH_MODEL_OP_GEN_MOVE_SET                           BLE_MESH_MODEL_OP_2(0x82, 0x0B)
#define BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK                     BLE_MESH_MODEL_OP_2(0x82, 0x0C)

/* Generic Default Transition Time Message Opcode*/
#define BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET                 BLE_MESH_MODEL_OP_2(0x82, 0x0D)
#define BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET                 BLE_MESH_MODEL_OP_2(0x82, 0x0E)
#define BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK           BLE_MESH_MODEL_OP_2(0x82, 0x0F)
#define BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS              BLE_MESH_MODEL_OP_2(0x82, 0x10)

/* Generic Power OnOff Message Opcode*/
#define BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x11)
#define BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x12)

/* Generic Power OnOff Setup Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x13)
#define BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK                BLE_MESH_MODEL_OP_2(0x82, 0x14)

/* Generic Power Level Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x15)
#define BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x16)
#define BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x17)
#define BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x18)
#define BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x19)
#define BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS                  BLE_MESH_MODEL_OP_2(0x82, 0x1A)
#define BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x1B)
#define BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS               BLE_MESH_MODEL_OP_2(0x82, 0x1C)
#define BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x1D)
#define BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x1E)

/* Generic Power Level Setup Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET                  BLE_MESH_MODEL_OP_2(0x82, 0x1F)
#define BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK            BLE_MESH_MODEL_OP_2(0x82, 0x20)
#define BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x21)
#define BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x22)

/* Generic Battery Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_BATTERY_GET                        BLE_MESH_MODEL_OP_2(0x82, 0x23)
#define BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS                     BLE_MESH_MODEL_OP_2(0x82, 0x24)

/* Generic Location Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x25)
#define BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS                  BLE_MESH_MODEL_OP_1(0x40)
#define BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x26)
#define BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x27)

/* Generic Location Setup Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET                     BLE_MESH_MODEL_OP_1(0x41)
#define BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK               BLE_MESH_MODEL_OP_1(0x42)
#define BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x28)
#define BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK                BLE_MESH_MODEL_OP_2(0x82, 0x29)

/* Generic Manufacturer Property Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET                BLE_MESH_MODEL_OP_2(0x82, 0x2A)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS             BLE_MESH_MODEL_OP_1(0x43)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x2B)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET                  BLE_MESH_MODEL_OP_1(0x44)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK            BLE_MESH_MODEL_OP_1(0x45)
#define BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS               BLE_MESH_MODEL_OP_1(0x46)

/* Generic Admin Property Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET               BLE_MESH_MODEL_OP_2(0x82, 0x2C)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS            BLE_MESH_MODEL_OP_1(0x47)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET                 BLE_MESH_MODEL_OP_2(0x82, 0x2D)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET                 BLE_MESH_MODEL_OP_1(0x48)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK           BLE_MESH_MODEL_OP_1(0x49)
#define BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS              BLE_MESH_MODEL_OP_1(0x4A)

/* Generic User Property Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET                BLE_MESH_MODEL_OP_2(0x82, 0x2E)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS             BLE_MESH_MODEL_OP_1(0x4B)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x2F)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET                  BLE_MESH_MODEL_OP_1(0x4C)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK            BLE_MESH_MODEL_OP_1(0x4D)
#define BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS               BLE_MESH_MODEL_OP_1(0x4E)

/* Generic Client Property Message Opcode */
#define BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET              BLE_MESH_MODEL_OP_1(0x4F)
#define BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS           BLE_MESH_MODEL_OP_1(0x50)

/* Sensor Message Opcode */
#define BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x30)
#define BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS               BLE_MESH_MODEL_OP_1(0x51)
#define BLE_MESH_MODEL_OP_SENSOR_GET                             BLE_MESH_MODEL_OP_2(0x82, 0x31)
#define BLE_MESH_MODEL_OP_SENSOR_STATUS                          BLE_MESH_MODEL_OP_1(0x52)
#define BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x32)
#define BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS                   BLE_MESH_MODEL_OP_1(0x53)
#define BLE_MESH_MODEL_OP_SENSOR_SERIES_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x33)
#define BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS                   BLE_MESH_MODEL_OP_1(0x54)

/* Sensor Setup Message Opcode */
#define BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x34)
#define BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET                     BLE_MESH_MODEL_OP_1(0x55)
#define BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK               BLE_MESH_MODEL_OP_1(0x56)
#define BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS                  BLE_MESH_MODEL_OP_1(0x57)
#define BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x35)
#define BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS                 BLE_MESH_MODEL_OP_1(0x58)
#define BLE_MESH_MODEL_OP_SENSOR_SETTING_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x36)
#define BLE_MESH_MODEL_OP_SENSOR_SETTING_SET                     BLE_MESH_MODEL_OP_1(0x59)
#define BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK               BLE_MESH_MODEL_OP_1(0x5A)
#define BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS                  BLE_MESH_MODEL_OP_1(0x5B)

/* Time Message Opcode */
#define BLE_MESH_MODEL_OP_TIME_GET                               BLE_MESH_MODEL_OP_2(0x82, 0x37)
#define BLE_MESH_MODEL_OP_TIME_SET                               BLE_MESH_MODEL_OP_1(0x5C)
#define BLE_MESH_MODEL_OP_TIME_STATUS                            BLE_MESH_MODEL_OP_1(0x5D)
#define BLE_MESH_MODEL_OP_TIME_ROLE_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x38)
#define BLE_MESH_MODEL_OP_TIME_ROLE_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x39)
#define BLE_MESH_MODEL_OP_TIME_ROLE_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x3A)
#define BLE_MESH_MODEL_OP_TIME_ZONE_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x3B)
#define BLE_MESH_MODEL_OP_TIME_ZONE_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x3C)
#define BLE_MESH_MODEL_OP_TIME_ZONE_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x3D)
#define BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x3E)
#define BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x3F)
#define BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x40)

/* Scene Message Opcode */
#define BLE_MESH_MODEL_OP_SCENE_GET                              BLE_MESH_MODEL_OP_2(0x82, 0x41)
#define BLE_MESH_MODEL_OP_SCENE_RECALL                           BLE_MESH_MODEL_OP_2(0x82, 0x42)
#define BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK                     BLE_MESH_MODEL_OP_2(0x82, 0x43)
#define BLE_MESH_MODEL_OP_SCENE_STATUS                           BLE_MESH_MODEL_OP_1(0x5E)
#define BLE_MESH_MODEL_OP_SCENE_REGISTER_GET                     BLE_MESH_MODEL_OP_2(0x82, 0x44)
#define BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS                  BLE_MESH_MODEL_OP_2(0x82, 0x45)

/* Scene Setup Message Opcode */
#define BLE_MESH_MODEL_OP_SCENE_STORE                            BLE_MESH_MODEL_OP_2(0x82, 0x46)
#define BLE_MESH_MODEL_OP_SCENE_STORE_UNACK                      BLE_MESH_MODEL_OP_2(0x82, 0x47)
#define BLE_MESH_MODEL_OP_SCENE_DELETE                           BLE_MESH_MODEL_OP_2(0x82, 0x9E)
#define BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK                     BLE_MESH_MODEL_OP_2(0x82, 0x9F)

/* Scheduler Message Opcode */
#define BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x48)
#define BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS                   BLE_MESH_MODEL_OP_1(0x5F)
#define BLE_MESH_MODEL_OP_SCHEDULER_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x49)
#define BLE_MESH_MODEL_OP_SCHEDULER_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x4A)

/* Scheduler Setup Message Opcode */
#define BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET                      BLE_MESH_MODEL_OP_1(0x60)
#define BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK                BLE_MESH_MODEL_OP_1(0x61)

/* Light Lightness Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x4B)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x4C)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x4D)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x4E)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET             BLE_MESH_MODEL_OP_2(0x82, 0x4F)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET             BLE_MESH_MODEL_OP_2(0x82, 0x50)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK       BLE_MESH_MODEL_OP_2(0x82, 0x51)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS          BLE_MESH_MODEL_OP_2(0x82, 0x52)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET               BLE_MESH_MODEL_OP_2(0x82, 0x53)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS            BLE_MESH_MODEL_OP_2(0x82, 0x54)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET            BLE_MESH_MODEL_OP_2(0x82, 0x55)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS         BLE_MESH_MODEL_OP_2(0x82, 0x56)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET              BLE_MESH_MODEL_OP_2(0x82, 0x57)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS           BLE_MESH_MODEL_OP_2(0x82, 0x58)

/* Light Lightness Setup Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET            BLE_MESH_MODEL_OP_2(0x82, 0x59)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK      BLE_MESH_MODEL_OP_2(0x82, 0x5A)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET              BLE_MESH_MODEL_OP_2(0x82, 0x5B)
#define BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK        BLE_MESH_MODEL_OP_2(0x82, 0x5C)

/* Light CTL Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_CTL_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x5D)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x5E)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x5F)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x60)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET              BLE_MESH_MODEL_OP_2(0x82, 0x61)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET        BLE_MESH_MODEL_OP_2(0x82, 0x62)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS     BLE_MESH_MODEL_OP_2(0x82, 0x63)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET              BLE_MESH_MODEL_OP_2(0x82, 0x64)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK        BLE_MESH_MODEL_OP_2(0x82, 0x65)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS           BLE_MESH_MODEL_OP_2(0x82, 0x66)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x67)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS               BLE_MESH_MODEL_OP_2(0x82, 0x68)

/* Light CTL Setup Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET                  BLE_MESH_MODEL_OP_2(0x82, 0x69)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK            BLE_MESH_MODEL_OP_2(0x82, 0x6A)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET        BLE_MESH_MODEL_OP_2(0x82, 0x6B)
#define BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK  BLE_MESH_MODEL_OP_2(0x82, 0x6C)

/* Light HSL Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_HSL_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x6D)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x6E)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x6F)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK                BLE_MESH_MODEL_OP_2(0x82, 0x70)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x71)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET               BLE_MESH_MODEL_OP_2(0x82, 0x72)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET               BLE_MESH_MODEL_OP_2(0x82, 0x73)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK         BLE_MESH_MODEL_OP_2(0x82, 0x74)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS            BLE_MESH_MODEL_OP_2(0x82, 0x75)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x76)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x77)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x78)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET                   BLE_MESH_MODEL_OP_2(0x82, 0x79)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS                BLE_MESH_MODEL_OP_2(0x82, 0x7A)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x7B)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS               BLE_MESH_MODEL_OP_2(0x82, 0x7C)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x7D)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x7E)

/* Light HSL Setup Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET                  BLE_MESH_MODEL_OP_2(0x82, 0x7F)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK            BLE_MESH_MODEL_OP_2(0x82, 0x80)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x81)
#define BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x82) /* Model spec is wrong */

/* Light xyL Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_XYL_GET                          BLE_MESH_MODEL_OP_2(0x82, 0x83)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_SET                          BLE_MESH_MODEL_OP_2(0x82, 0x84)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK                    BLE_MESH_MODEL_OP_2(0x82, 0x85)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS                       BLE_MESH_MODEL_OP_2(0x82, 0x86)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET                   BLE_MESH_MODEL_OP_2(0x82, 0x87)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS                BLE_MESH_MODEL_OP_2(0x82, 0x88)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x89)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS               BLE_MESH_MODEL_OP_2(0x82, 0x8A)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET                    BLE_MESH_MODEL_OP_2(0x82, 0x8B)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS                 BLE_MESH_MODEL_OP_2(0x82, 0x8C)

/* Light xyL Setup Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET                  BLE_MESH_MODEL_OP_2(0x82, 0x8D)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK            BLE_MESH_MODEL_OP_2(0x82, 0x8E)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET                    BLE_MESH_MODEL_OP_2(0x82, 0x8F)
#define BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK              BLE_MESH_MODEL_OP_2(0x82, 0x90)

/* Light Control Message Opcode */
#define BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET                      BLE_MESH_MODEL_OP_2(0x82, 0x91)
#define BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET                      BLE_MESH_MODEL_OP_2(0x82, 0x92)
#define BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK                BLE_MESH_MODEL_OP_2(0x82, 0x93)
#define BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS                   BLE_MESH_MODEL_OP_2(0x82, 0x94)
#define BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET                        BLE_MESH_MODEL_OP_2(0x82, 0x95)
#define BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET                        BLE_MESH_MODEL_OP_2(0x82, 0x96)
#define BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK                  BLE_MESH_MODEL_OP_2(0x82, 0x97)
#define BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS                     BLE_MESH_MODEL_OP_2(0x82, 0x98)
#define BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET               BLE_MESH_MODEL_OP_2(0x82, 0x99)
#define BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET               BLE_MESH_MODEL_OP_2(0x82, 0x9A)
#define BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK         BLE_MESH_MODEL_OP_2(0x82, 0x9B)
#define BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS            BLE_MESH_MODEL_OP_2(0x82, 0x9C)
#define BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET                  BLE_MESH_MODEL_OP_2(0x82, 0x9D)
#define BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET                  BLE_MESH_MODEL_OP_1(0x62)
#define BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK            BLE_MESH_MODEL_OP_1(0x63)
#define BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS               BLE_MESH_MODEL_OP_1(0x64)

#define BLE_MESH_BLE_MESH_SDU_MAX_LEN            384

/*!< The maximum length of a BLE Mesh provisioned node name */
#define BLE_MESH_BLE_MESH_NODE_NAME_MAX_LEN      31

/*!< The maximum length of a BLE Mesh unprovisioned device name */
#define BLE_MESH_BLE_MESH_DEVICE_NAME_MAX_LEN    DEVICE_NAME_SIZE

/*!< Define the BLE Mesh octet 16 bytes size */
#define BLE_MESH_BLE_MESH_OCTET16_LEN    16
typedef uint8_t ble_mesh_octet16_t[BLE_MESH_BLE_MESH_OCTET16_LEN];

/*!< Define the BLE Mesh octet 8 bytes size */
#define BLE_MESH_BLE_MESH_OCTET8_LEN     8
typedef uint8_t ble_mesh_octet8_t[BLE_MESH_BLE_MESH_OCTET8_LEN];

#define BLE_MESH_BLE_MESH_ADDR_UNASSIGNED              BLE_MESH_ADDR_UNASSIGNED
#define BLE_MESH_BLE_MESH_ADDR_ALL_NODES               BLE_MESH_ADDR_ALL_NODES
#define BLE_MESH_BLE_MESH_ADDR_PROXIES                 BLE_MESH_ADDR_PROXIES
#define BLE_MESH_BLE_MESH_ADDR_FRIENDS                 BLE_MESH_ADDR_FRIENDS
#define BLE_MESH_BLE_MESH_ADDR_RELAYS                  BLE_MESH_ADDR_RELAYS

#define BLE_MESH_BLE_MESH_KEY_UNUSED                   BLE_MESH_KEY_UNUSED
#define BLE_MESH_BLE_MESH_KEY_DEV                      BLE_MESH_KEY_DEV

#define BLE_MESH_BLE_MESH_KEY_PRIMARY                  BLE_MESH_KEY_PRIMARY
#define BLE_MESH_BLE_MESH_KEY_ANY                      BLE_MESH_KEY_ANY

/*!< Primary Network Key index */
#define BLE_MESH_BLE_MESH_NET_PRIMARY                  BLE_MESH_NET_PRIMARY

/*!< Relay state value */
#define BLE_MESH_BLE_MESH_RELAY_DISABLED               BLE_MESH_RELAY_DISABLED
#define BLE_MESH_BLE_MESH_RELAY_ENABLED                BLE_MESH_RELAY_ENABLED
#define BLE_MESH_BLE_MESH_RELAY_NOT_SUPPORTED          BLE_MESH_RELAY_NOT_SUPPORTED

/*!< Beacon state value */
#define BLE_MESH_BLE_MESH_BEACON_DISABLED              BLE_MESH_BEACON_DISABLED
#define BLE_MESH_BLE_MESH_BEACON_ENABLED               BLE_MESH_BEACON_ENABLED

/*!< GATT Proxy state value */
#define BLE_MESH_BLE_MESH_GATT_PROXY_DISABLED          BLE_MESH_GATT_PROXY_DISABLED
#define BLE_MESH_BLE_MESH_GATT_PROXY_ENABLED           BLE_MESH_GATT_PROXY_ENABLED
#define BLE_MESH_BLE_MESH_GATT_PROXY_NOT_SUPPORTED     BLE_MESH_GATT_PROXY_NOT_SUPPORTED

/*!< Friend state value */
#define BLE_MESH_BLE_MESH_FRIEND_DISABLED              BLE_MESH_FRIEND_DISABLED
#define BLE_MESH_BLE_MESH_FRIEND_ENABLED               BLE_MESH_FRIEND_ENABLED
#define BLE_MESH_BLE_MESH_FRIEND_NOT_SUPPORTED         BLE_MESH_FRIEND_NOT_SUPPORTED

/*!< Node identity state value */
#define BLE_MESH_BLE_MESH_NODE_IDENTITY_STOPPED        BLE_MESH_NODE_IDENTITY_STOPPED
#define BLE_MESH_BLE_MESH_NODE_IDENTITY_RUNNING        BLE_MESH_NODE_IDENTITY_RUNNING
#define BLE_MESH_BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED  BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED

/*!< Supported features */
#define BLE_MESH_BLE_MESH_FEATURE_RELAY                BLE_MESH_FEAT_RELAY
#define BLE_MESH_BLE_MESH_FEATURE_PROXY                BLE_MESH_FEAT_PROXY
#define BLE_MESH_BLE_MESH_FEATURE_FRIEND               BLE_MESH_FEAT_FRIEND
#define BLE_MESH_BLE_MESH_FEATURE_LOW_POWER            BLE_MESH_FEAT_LOW_POWER
#define BLE_MESH_BLE_MESH_FEATURE_ALL_SUPPORTED        BLE_MESH_FEAT_SUPPORTED

#define BLE_MESH_ADDR_IS_UNICAST(addr) ((addr) && (addr) < 0x8000)
#define BLE_MESH_ADDR_IS_GROUP(addr) ((addr) >= 0xc000 && (addr) <= 0xff00)
#define BLE_MESH_ADDR_IS_VIRTUAL(addr) ((addr) >= 0x8000 && (addr) < 0xc000)
#define BLE_MESH_ADDR_IS_RFU(addr) ((addr) >= 0xff00 && (addr) <= 0xfffb)

#define BLE_MESH_BLE_MESH_ADDR_IS_UNICAST(addr)        BLE_MESH_ADDR_IS_UNICAST(addr)
#define BLE_MESH_BLE_MESH_ADDR_IS_GROUP(addr)          BLE_MESH_ADDR_IS_GROUP(addr)
#define BLE_MESH_BLE_MESH_ADDR_IS_VIRTUAL(addr)        BLE_MESH_ADDR_IS_VIRTUAL(addr)
#define BLE_MESH_BLE_MESH_ADDR_IS_RFU(addr)            BLE_MESH_ADDR_IS_RFU(addr)

#define BLE_MESH_BLE_MESH_INVALID_NODE_INDEX          (-1)

/* Foundation Models */
#define BLE_MESH_MODEL_ID_CFG_SRV                   0x0000
#define BLE_MESH_MODEL_ID_CFG_CLI                   0x0001
#define BLE_MESH_MODEL_ID_HEALTH_SRV                0x0002
#define BLE_MESH_MODEL_ID_HEALTH_CLI                0x0003

/* Models from the Mesh Model Specification */
#define BLE_MESH_MODEL_ID_GEN_ONOFF_SRV             0x1000
#define BLE_MESH_MODEL_ID_GEN_ONOFF_CLI             0x1001
#define BLE_MESH_MODEL_ID_GEN_LEVEL_SRV             0x1002
#define BLE_MESH_MODEL_ID_GEN_LEVEL_CLI             0x1003
#define BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV    0x1004
#define BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI    0x1005
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV       0x1006
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV 0x1007
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI       0x1008
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV       0x1009
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV 0x100a
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI       0x100b
#define BLE_MESH_MODEL_ID_GEN_BATTERY_SRV           0x100c
#define BLE_MESH_MODEL_ID_GEN_BATTERY_CLI           0x100d
#define BLE_MESH_MODEL_ID_GEN_LOCATION_SRV          0x100e
#define BLE_MESH_MODEL_ID_GEN_LOCATION_SETUPSRV     0x100f
#define BLE_MESH_MODEL_ID_GEN_LOCATION_CLI          0x1010
#define BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV        0x1011
#define BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV 0x1012
#define BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV         0x1013
#define BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV       0x1014
#define BLE_MESH_MODEL_ID_GEN_PROP_CLI              0x1015
#define BLE_MESH_MODEL_ID_SENSOR_SRV                0x1100
#define BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV          0x1101
#define BLE_MESH_MODEL_ID_SENSOR_CLI                0x1102
#define BLE_MESH_MODEL_ID_TIME_SRV                  0x1200
#define BLE_MESH_MODEL_ID_TIME_SETUP_SRV            0x1201
#define BLE_MESH_MODEL_ID_TIME_CLI                  0x1202
#define BLE_MESH_MODEL_ID_SCENE_SRV                 0x1203
#define BLE_MESH_MODEL_ID_SCENE_SETUP_SRV           0x1204
#define BLE_MESH_MODEL_ID_SCENE_CLI                 0x1205
#define BLE_MESH_MODEL_ID_SCHEDULER_SRV             0x1206
#define BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV       0x1207
#define BLE_MESH_MODEL_ID_SCHEDULER_CLI             0x1208
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV       0x1300
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV 0x1301
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI       0x1302
#define BLE_MESH_MODEL_ID_LIGHT_CTL_SRV             0x1303
#define BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV       0x1304
#define BLE_MESH_MODEL_ID_LIGHT_CTL_CLI             0x1305
#define BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV        0x1306
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SRV             0x1307
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV       0x1308
#define BLE_MESH_MODEL_ID_LIGHT_HSL_CLI             0x1309
#define BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV         0x130a
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV         0x130b
#define BLE_MESH_MODEL_ID_LIGHT_XYL_SRV             0x130c
#define BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV       0x130d
#define BLE_MESH_MODEL_ID_LIGHT_XYL_CLI             0x130e
#define BLE_MESH_MODEL_ID_LIGHT_LC_SRV              0x130f
#define BLE_MESH_MODEL_ID_LIGHT_LC_SETUPSRV         0x1310
#define BLE_MESH_MODEL_ID_LIGHT_LC_CLI              0x1311

/*!< Foundation Models */
#define BLE_MESH_BLE_MESH_MODEL_ID_CONFIG_SRV                      BLE_MESH_MODEL_ID_CFG_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_CONFIG_CLI                      BLE_MESH_MODEL_ID_CFG_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_HEALTH_SRV                      BLE_MESH_MODEL_ID_HEALTH_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_HEALTH_CLI                      BLE_MESH_MODEL_ID_HEALTH_CLI

/*!< Models from the Mesh Model Specification */
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV                   BLE_MESH_MODEL_ID_GEN_ONOFF_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI                   BLE_MESH_MODEL_ID_GEN_ONOFF_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV                   BLE_MESH_MODEL_ID_GEN_LEVEL_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_LEVEL_CLI                   BLE_MESH_MODEL_ID_GEN_LEVEL_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV          BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI          BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV             BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV       BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI             BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV             BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV       BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI             BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_BATTERY_SRV                 BLE_MESH_MODEL_ID_GEN_BATTERY_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_BATTERY_CLI                 BLE_MESH_MODEL_ID_GEN_BATTERY_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_LOCATION_SRV                BLE_MESH_MODEL_ID_GEN_LOCATION_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV          BLE_MESH_MODEL_ID_GEN_LOCATION_SETUPSRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_LOCATION_CLI                BLE_MESH_MODEL_ID_GEN_LOCATION_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV              BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV       BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV               BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV             BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_GEN_PROP_CLI                    BLE_MESH_MODEL_ID_GEN_PROP_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_SENSOR_SRV                      BLE_MESH_MODEL_ID_SENSOR_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV                BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_SENSOR_CLI                      BLE_MESH_MODEL_ID_SENSOR_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_TIME_SRV                        BLE_MESH_MODEL_ID_TIME_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_TIME_SETUP_SRV                  BLE_MESH_MODEL_ID_TIME_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_TIME_CLI                        BLE_MESH_MODEL_ID_TIME_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_SCENE_SRV                       BLE_MESH_MODEL_ID_SCENE_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_SCENE_SETUP_SRV                 BLE_MESH_MODEL_ID_SCENE_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_SCENE_CLI                       BLE_MESH_MODEL_ID_SCENE_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_SCHEDULER_SRV                   BLE_MESH_MODEL_ID_SCHEDULER_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV             BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_SCHEDULER_CLI                   BLE_MESH_MODEL_ID_SCHEDULER_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV             BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV       BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI             BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV                   BLE_MESH_MODEL_ID_LIGHT_CTL_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV             BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI                   BLE_MESH_MODEL_ID_LIGHT_CTL_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV              BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV                   BLE_MESH_MODEL_ID_LIGHT_HSL_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV             BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI                   BLE_MESH_MODEL_ID_LIGHT_HSL_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV               BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV               BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_XYL_SRV                   BLE_MESH_MODEL_ID_LIGHT_XYL_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV             BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI                   BLE_MESH_MODEL_ID_LIGHT_XYL_CLI
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_LC_SRV                    BLE_MESH_MODEL_ID_LIGHT_LC_SRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV              BLE_MESH_MODEL_ID_LIGHT_LC_SETUPSRV
#define BLE_MESH_BLE_MESH_MODEL_ID_LIGHT_LC_CLI                    BLE_MESH_MODEL_ID_LIGHT_LC_CLI

/*!< The following opcodes will only be used in the ble_mesh_config_client_get_state function. */
typedef uint32_t ble_mesh_opcode_config_client_get_t;                               /*!< ble_mesh_opcode_config_client_get_t belongs to ble_mesh_opcode_t,
                                                                                          this typedef is only used to locate the opcodes used by ble_mesh_config_client_get_state */
#define BLE_MESH_BLE_MESH_MODEL_OP_BEACON_GET                        OP_BEACON_GET           /*!< To determine the Secure Network Beacon state of a Configuration Server */
#define BLE_MESH_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET              OP_DEV_COMP_DATA_GET    /*!< To determine the Composition Data state of a Configuration Server, a Configuration 
                                                                                          Client shall send a Config Composition Data Get message with the Page field value set
                                                                                          to 0xFF. The response is a Config Composition Data Status message that contains the last
                                                                                          page of the Composition Data state. If the Page field of the Config Composition Data Status
                                                                                          message contains a non-zero value, then the Configuration Client shall send another Composition
                                                                                          Data Get message with the Page field value set to one less than the Page field value of the
                                                                                          Config Composition Data Status message. */
#define BLE_MESH_BLE_MESH_MODEL_OP_DEFAULT_TTL_GET                   OP_DEFAULT_TTL_GET      /*!< To determine the Default TTL state of a Configuration Server */
#define BLE_MESH_BLE_MESH_MODEL_OP_GATT_PROXY_GET                    OP_GATT_PROXY_GET       /*!< To determine the GATT Proxy state of a Configuration Server */
#define BLE_MESH_BLE_MESH_MODEL_OP_RELAY_GET                         OP_RELAY_GET            /*!< To determine the Relay and Relay Retransmit states of a Configuration Server */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_PUB_GET                     OP_MOD_PUB_GET          /*!< To determine the Publish Address, Publish AppKey Index, CredentialFlag,
                                                                                          Publish Period, Publish Retransmit Count, Publish Retransmit Interval Steps,
                                                                                          and Publish TTL states of a particular Model within the element */
#define BLE_MESH_BLE_MESH_MODEL_OP_FRIEND_GET                        OP_FRIEND_GET           /*!< To determine the Friend state of a Configuration Server */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_GET                 OP_HEARTBEAT_PUB_GET    /*!< To determine the Heartbeat Subscription Source, Heartbeat Subscription Destination,
                                                                                          Heartbeat Subscription Count Log, Heartbeat Subscription Period Log, Heartbeat
                                                                                          Subscription Min Hops, and Heartbeat Subscription Max Hops states of a node */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_GET                 OP_HEARTBEAT_SUB_GET    /*!< To determine the Heartbeat Subscription Source, Heartbeat Subscription Destination,
                                                                                          Heartbeat Subscription Count Log, Heartbeat Subscription Period Log, Heartbeat
                                                                                          Subscription Min Hops, and Heartbeat Subscription Max Hops states of a node */
#define BLE_MESH_BLE_MESH_MODEL_OP_NET_KEY_GET                       OP_NET_KEY_GET          /*!< To determine all NetKeys known to the node */
#define BLE_MESH_BLE_MESH_MODEL_OP_APP_KEY_GET                       OP_APP_KEY_GET          /*!< To determine all AppKeys bound to the NetKey */
#define BLE_MESH_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET                 OP_NODE_IDENTITY_GET    /*!< To get the current Node Identity state for a subnet */
#define BLE_MESH_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET                 OP_MOD_SUB_GET          /*!< To get the list of subscription addresses of a model within the element */
#define BLE_MESH_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET              OP_MOD_SUB_GET_VND      /*!< To get the list of subscription addresses of a model within the element */
#define BLE_MESH_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET                 OP_SIG_MOD_APP_GET      /*!< To request report of all AppKeys bound to the SIG Model */
#define BLE_MESH_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET              OP_VND_MOD_APP_GET      /*!< To request report of all AppKeys bound to the Vendor Model */
#define BLE_MESH_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET             OP_KRP_GET              /*!< To get the current Key Refresh Phase state of the identified network key */
#define BLE_MESH_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET               OP_LPN_TIMEOUT_GET      /*!< To get the current value of PollTimeout timer of the Low Power node within a Friend node */
#define BLE_MESH_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_GET              OP_NET_TRANSMIT_GET     /*!< To get the current Network Transmit state of a node */

/*!< The following opcodes will only be used in the ble_mesh_config_client_set_state function. */
typedef uint32_t ble_mesh_opcode_config_client_set_t;                               /*!< ble_mesh_opcode_config_client_set_t belongs to ble_mesh_opcode_t,
                                                                                          this typedef is only used to locate the opcodes used by ble_mesh_config_client_set_state */
#define BLE_MESH_BLE_MESH_MODEL_OP_BEACON_SET                        OP_BEACON_SET           /*!< Set the Secure Network Beacon state of a Configuration Server with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET                   OP_DEFAULT_TTL_SET      /*!< Set the Default TTL state of a Configuration Server with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_GATT_PROXY_SET                    OP_GATT_PROXY_SET       /*!< Determine the GATT Proxy state of a Configuration Server */
#define BLE_MESH_BLE_MESH_MODEL_OP_RELAY_SET                         OP_RELAY_SET            /*!< Set the Relay and Relay Retransmit states of a Configuration Server with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_PUB_SET                     OP_MOD_PUB_SET          /*!< Set the Publish Address, Publish AppKey Index, CredentialFlag, Publish
                                                                                          Period, Publish Retransmit Count, Publish Retransmit Interval Steps, and
                                                                                          Publish TTL states of a particular model within the element with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_SUB_ADD                     OP_MOD_SUB_ADD          /*!< Add the address to the Subscription List state of a particular model
                                                                                          within the element with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD        OP_MOD_SUB_VA_ADD       /*!< Add the Label UUID to the Subscription List state of a particular model
                                                                                          within the element with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE                  OP_MOD_SUB_DEL          /*!< Delete the address from the Subscription List state of a particular
                                                                                          model within the element with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE     OP_MOD_SUB_VA_DEL       /*!< Delete the Label UUID from the Subscription List state of a particular
                                                                                          model within the element with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE               OP_MOD_SUB_OVERWRITE    /*!< Clear the Subscription List and add the address to the Subscription List
                                                                                          state of a particular Model within the element with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE  OP_MOD_SUB_VA_OVERWRITE /*!< Clear the Subscription List and add the Label UUID to the Subscription
                                                                                          List state of a particular model within the element with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_NET_KEY_ADD                       OP_NET_KEY_ADD          /*!< Add the NetKey identified by NetKeyIndex to the NetKey List state with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_APP_KEY_ADD                       OP_APP_KEY_ADD          /*!< Add the AppKey to the AppKey List and bind it to the NetKey identified
                                                                                          by the NetKeyIndex of a Configuration Server with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_APP_BIND                    OP_MOD_APP_BIND         /*!< Bind the AppKey to a model of a particular element of a Configuration Server with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_NODE_RESET                        OP_NODE_RESET           /*!< Reset a node (other than a Provisioner) and remove it from the network */
#define BLE_MESH_BLE_MESH_MODEL_OP_FRIEND_SET                        OP_FRIEND_SET           /*!< Set the Friend state of a Configuration Server with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET                 OP_HEARTBEAT_PUB_SET    /*!< Set the Heartbeat Publication Destination, Heartbeat Publication Count,
                                                                                          Heartbeat Publication Period, Heartbeat Publication TTL, Publication Features,
                                                                                          and Publication NetKey Index of a node with acknowledgment */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET                 OP_HEARTBEAT_SUB_SET    /*!< Determine the Heartbeat Subscription Source, Heartbeat Subscription Destination,
                                                                                          Heartbeat Subscription Count Log, Heartbeat Subscription Period Log, Heartbeat
                                                                                          Subscription Min Hops, and Heartbeat Subscription Max Hops states of a node */
#define BLE_MESH_BLE_MESH_MODEL_OP_NET_KEY_UPDATE                    OP_NET_KEY_UPDATE       /*!< To update a NetKey on a node */
#define BLE_MESH_BLE_MESH_MODEL_OP_NET_KEY_DELETE                    OP_NET_KEY_DEL          /*!< To delete a NetKey on a NetKey List from a node */
#define BLE_MESH_BLE_MESH_MODEL_OP_APP_KEY_UPDATE                    OP_APP_KEY_UPDATE       /*!< To update an AppKey value on the AppKey List on a node */
#define BLE_MESH_BLE_MESH_MODEL_OP_APP_KEY_DELETE                    OP_APP_KEY_DEL          /*!< To delete an AppKey from the AppKey List on a node */
#define BLE_MESH_BLE_MESH_MODEL_OP_NODE_IDENTITY_SET                 OP_NODE_IDENTITY_SET    /*!< To set the current Node Identity state for a subnet */
#define BLE_MESH_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_SET             OP_KRP_SET              /*!< To set the Key Refresh Phase state of the identified network key */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_PUB_VIRTUAL_ADDR_SET        OP_MOD_PUB_VA_SET       /*!< To set the model Publication state of an outgoing message that originates from a model */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL              OP_MOD_SUB_DEL_ALL      /*!< To discard the Subscription List of a model */
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_APP_UNBIND                  OP_MOD_APP_UNBIND       /*!< To remove the binding between an AppKey and a model */
#define BLE_MESH_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_SET              OP_NET_TRANSMIT_SET     /*!< To set the Network Transmit state of a node */

/*!< The following opcodes are used by the BLE Mesh Config Server Model internally to respond to the Config Client Model's request messages */
typedef uint32_t ble_mesh_config_model_status_t;    /*!< ble_mesh_config_model_status_t belongs to ble_mesh_opcode_t, this typedef
                                                          is only used to locate the opcodes used by the Config Model messages */
#define BLE_MESH_BLE_MESH_MODEL_OP_BEACON_STATUS                     OP_BEACON_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_COMPOSITION_DATA_STATUS           OP_DEV_COMP_DATA_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_DEFAULT_TTL_STATUS                OP_DEFAULT_TTL_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GATT_PROXY_STATUS                 OP_GATT_PROXY_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_RELAY_STATUS                      OP_RELAY_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_PUB_STATUS                  OP_MOD_PUB_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_SUB_STATUS                  OP_MOD_SUB_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_LIST                OP_MOD_SUB_LIST
#define BLE_MESH_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_LIST             OP_MOD_SUB_LIST_VND
#define BLE_MESH_BLE_MESH_MODEL_OP_NET_KEY_STATUS                    OP_NET_KEY_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_NET_KEY_LIST                      OP_NET_KEY_LIST
#define BLE_MESH_BLE_MESH_MODEL_OP_APP_KEY_STATUS                    OP_APP_KEY_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_APP_KEY_LIST                      OP_APP_KEY_LIST
#define BLE_MESH_BLE_MESH_MODEL_OP_NODE_IDENTITY_STATUS              OP_NODE_IDENTITY_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_MODEL_APP_STATUS                  OP_MOD_APP_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SIG_MODEL_APP_LIST                OP_SIG_MOD_APP_LIST
#define BLE_MESH_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_LIST             OP_VND_MOD_APP_LIST
#define BLE_MESH_BLE_MESH_MODEL_OP_NODE_RESET_STATUS                 OP_NODE_RESET_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_FRIEND_STATUS                     OP_FRIEND_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_STATUS          OP_KRP_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_STATUS              OP_HEARTBEAT_PUB_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_STATUS              OP_HEARTBEAT_SUB_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_STATUS            OP_LPN_TIMEOUT_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_STATUS           OP_NET_TRANSMIT_STATUS

/*!< The following opcodes will only be used in the ble_mesh_health_client_get_state function. */
typedef uint32_t ble_mesh_opcode_health_client_get_t;                                   /*!< ble_mesh_opcode_health_client_get_t belongs to ble_mesh_opcode_t,
                                                                                              this typedef is only used to locate the opcodes used by ble_mesh_health_client_get_state */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET                  OP_HEALTH_FAULT_GET         /*!< Get the current Registered Fault state */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_GET                 OP_HEALTH_PERIOD_GET        /*!< Get the current Health Period state */
#define BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_GET                     OP_ATTENTION_GET            /*!< Get the current Attention Timer state */

/*!< The following opcodes will only be used in the ble_mesh_health_client_set_state function. */
typedef uint32_t ble_mesh_opcode_health_client_set_t;                                   /*!< ble_mesh_opcode_health_client_set_t belongs to ble_mesh_opcode_t,
                                                                                              this typedef is only used to locate the opcodes used by ble_mesh_health_client_set_state */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR                OP_HEALTH_FAULT_CLEAR       /*!< Clear Health Fault acknowledged */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK          OP_HEALTH_FAULT_CLEAR_UNREL /*!< Clear Health Fault Unacknowledged */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST                 OP_HEALTH_FAULT_TEST        /*!< Invoke Health Fault Test acknowledged */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK           OP_HEALTH_FAULT_TEST_UNREL  /*!< Invoke Health Fault Test unacknowledged */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET                 OP_HEALTH_PERIOD_SET        /*!< Set Health Period acknowledged */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK           OP_HEALTH_PERIOD_SET_UNREL  /*!< Set Health Period unacknowledged */
#define BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_SET                     OP_ATTENTION_SET            /*!< Set Health Attention acknowledged of the Health Server */
#define BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK               OP_ATTENTION_SET_UNREL      /*!< Set Health Attention Unacknowledged of the Health Server */

/*!< The following opcodes are used by the BLE Mesh Health Server Model internally to respond to the Health Client Model's request messages */
typedef uint32_t ble_mesh_health_model_status_t;    /*!< ble_mesh_health_model_status_t belongs to ble_mesh_opcode_t, this typedef
                                                          is only used to locate the opcodes used by the Health Model messages */
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_CURRENT_STATUS             OP_HEALTH_CURRENT_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_STATUS               OP_HEALTH_FAULT_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_STATUS              OP_HEALTH_PERIOD_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_STATUS                  OP_ATTENTION_STATUS

typedef uint32_t ble_mesh_generic_message_opcode_t;     /*!< ble_mesh_generic_message_opcode_t belongs to ble_mesh_opcode_t,
                                                              this typedef is only used to locate the opcodes used by functions
                                                              ble_mesh_generic_client_get_state & ble_mesh_generic_client_set_state  */
/*!< Generic OnOff Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ONOFF_GET                   BLE_MESH_MODEL_OP_GEN_ONOFF_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ONOFF_SET                   BLE_MESH_MODEL_OP_GEN_ONOFF_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK             BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS                BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS

/*!< Generic Level Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LEVEL_GET                   BLE_MESH_MODEL_OP_GEN_LEVEL_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LEVEL_SET                   BLE_MESH_MODEL_OP_GEN_LEVEL_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK             BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS                BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_DELTA_SET                   BLE_MESH_MODEL_OP_GEN_DELTA_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK             BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_MOVE_SET                    BLE_MESH_MODEL_OP_GEN_MOVE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK              BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK

/*!< Generic Default Transition Time Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET          BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET          BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK    BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS       BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS

/*!< Generic Power OnOff Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET               BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS            BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS

/*!< Generic Power OnOff Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET               BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK         BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK

/*!< Generic Power Level Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET             BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET             BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK       BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS          BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET              BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS           BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET           BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS        BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET             BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS          BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS

/*!< Generic Power Level Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET           BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK     BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET             BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK       BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK

/*!< Generic Battery Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_BATTERY_GET                 BLE_MESH_MODEL_OP_GEN_BATTERY_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS              BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS

/*!< Generic Location Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET              BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS           BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET               BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS            BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS

/*!< Generic Location Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET              BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK        BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET               BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK         BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK

/*!< Generic Manufacturer Property Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_GET       BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_STATUS    BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_GET         BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET         BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET_UNACK   BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_STATUS      BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS

/*!< Generic Admin Property Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET        BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS     BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET          BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET          BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK    BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS       BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS

/*!< Generic User Property Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET         BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS      BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET           BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET           BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK     BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS        BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS

/*!< Generic Client Property Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET       BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS    BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS

typedef uint32_t ble_mesh_sensor_message_opcode_t;      /*!< ble_mesh_sensor_message_opcode_t belongs to ble_mesh_opcode_t,
                                                              this typedef is only used to locate the opcodes used by functions
                                                              ble_mesh_sensor_client_get_state & ble_mesh_sensor_client_set_state */
/*!< Sensor Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET           BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS        BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_GET                      BLE_MESH_MODEL_OP_SENSOR_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_STATUS                   BLE_MESH_MODEL_OP_SENSOR_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET               BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS            BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET               BLE_MESH_MODEL_OP_SENSOR_SERIES_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS            BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS

/*!< Sensor Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET              BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET              BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK        BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS           BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET             BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS          BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_SETTING_GET              BLE_MESH_MODEL_OP_SENSOR_SETTING_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET              BLE_MESH_MODEL_OP_SENSOR_SETTING_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK        BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS           BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS

typedef uint32_t ble_mesh_time_scene_message_opcode_t;  /*!< ble_mesh_time_scene_message_opcode_t belongs to ble_mesh_opcode_t,
                                                              this typedef is only used to locate the opcodes used by functions
                                                              ble_mesh_time_scene_client_get_state & ble_mesh_time_scene_client_set_state  */
/*!< Time Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_GET                        BLE_MESH_MODEL_OP_TIME_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_SET                        BLE_MESH_MODEL_OP_TIME_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_STATUS                     BLE_MESH_MODEL_OP_TIME_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_ROLE_GET                   BLE_MESH_MODEL_OP_TIME_ROLE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_ROLE_SET                   BLE_MESH_MODEL_OP_TIME_ROLE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_ROLE_STATUS                BLE_MESH_MODEL_OP_TIME_ROLE_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_ZONE_GET                   BLE_MESH_MODEL_OP_TIME_ZONE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_ZONE_SET                   BLE_MESH_MODEL_OP_TIME_ZONE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_TIME_ZONE_STATUS                BLE_MESH_MODEL_OP_TIME_ZONE_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET               BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET               BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS            BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS

/*!< Scene Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_GET                       BLE_MESH_MODEL_OP_SCENE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_RECALL                    BLE_MESH_MODEL_OP_SCENE_RECALL
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK              BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_STATUS                    BLE_MESH_MODEL_OP_SCENE_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_REGISTER_GET              BLE_MESH_MODEL_OP_SCENE_REGISTER_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS           BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS

/*!< Scene Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_STORE                     BLE_MESH_MODEL_OP_SCENE_STORE
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_STORE_UNACK               BLE_MESH_MODEL_OP_SCENE_STORE_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_DELETE                    BLE_MESH_MODEL_OP_SCENE_DELETE
#define BLE_MESH_BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK              BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK

/*!< Scheduler Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET               BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS            BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_GET                   BLE_MESH_MODEL_OP_SCHEDULER_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_STATUS                BLE_MESH_MODEL_OP_SCHEDULER_STATUS

/*!< Scheduler Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET               BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK         BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK

typedef uint32_t ble_mesh_light_message_opcode_t;      /*!< ble_mesh_light_message_opcode_t belongs to ble_mesh_opcode_t,
                                                              this typedef is only used to locate the opcodes used by functions
                                                              ble_mesh_light_client_get_state & ble_mesh_light_client_set_state  */
/*!< Light Lightness Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET                BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET                BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK          BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS             BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET         BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET         BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK   BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS      BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET           BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS        BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET        BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS     BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET          BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS       BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS

/*!< Light Lightness Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET        BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK  BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET          BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK    BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK

/*!< Light CTL Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_GET                      BLE_MESH_MODEL_OP_LIGHT_CTL_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_SET                      BLE_MESH_MODEL_OP_LIGHT_CTL_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK                BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS                   BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET          BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET    BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET          BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK    BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS       BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET              BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS           BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS

/*!< Light CTL Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET                 BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK           BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET       BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK

/*!< Light HSL Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_GET                         BLE_MESH_MODEL_OP_LIGHT_HSL_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET                     BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET                     BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK               BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS                  BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET              BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET              BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK        BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS           BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_SET                         BLE_MESH_MODEL_OP_LIGHT_HSL_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK                   BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS                      BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET                  BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS               BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET                 BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS              BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET                   BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS                BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS

/*!< Light HSL Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET                 BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK           BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET                   BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK             BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK               /* Model spec is wrong */

/*!< Light xyL Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_GET                         BLE_MESH_MODEL_OP_LIGHT_XYL_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_SET                         BLE_MESH_MODEL_OP_LIGHT_XYL_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK                   BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS                      BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET                  BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS               BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET                 BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS              BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET                   BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS                BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS

/*!< Light xyL Setup Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET                 BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK           BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET                   BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK             BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK

/*!< Light Control Message Opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET                     BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET                     BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK               BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS                  BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET                       BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET                       BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK                 BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS                    BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET              BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET              BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK        BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS           BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET                 BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET                 BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK           BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK
#define BLE_MESH_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS              BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS

typedef uint32_t ble_mesh_opcode_t;
/*!< End of defines of ble_mesh_opcode_t */

#define BLE_MESH_BLE_MESH_CFG_STATUS_SUCCESS                           STATUS_SUCCESS
#define BLE_MESH_BLE_MESH_CFG_STATUS_INVALID_ADDRESS                   STATUS_INVALID_ADDRESS
#define BLE_MESH_BLE_MESH_CFG_STATUS_INVALID_MODEL                     STATUS_INVALID_MODEL
#define BLE_MESH_BLE_MESH_CFG_STATUS_INVALID_APPKEY                    STATUS_INVALID_APPKEY
#define BLE_MESH_BLE_MESH_CFG_STATUS_INVALID_NETKEY                    STATUS_INVALID_NETKEY
#define BLE_MESH_BLE_MESH_CFG_STATUS_INSUFFICIENT_RESOURCES            STATUS_INSUFF_RESOURCES
#define BLE_MESH_BLE_MESH_CFG_STATUS_KEY_INDEX_ALREADY_STORED          STATUS_IDX_ALREADY_STORED
#define BLE_MESH_BLE_MESH_CFG_STATUS_INVALID_PUBLISH_PARAMETERS        STATUS_NVAL_PUB_PARAM
#define BLE_MESH_BLE_MESH_CFG_STATUS_NOT_A_SUBSCRIBE_MODEL             STATUS_NOT_SUB_MOD
#define BLE_MESH_BLE_MESH_CFG_STATUS_STORAGE_FAILURE                   STATUS_STORAGE_FAIL
#define BLE_MESH_BLE_MESH_CFG_STATUS_FEATURE_NOT_SUPPORTED             STATUS_FEAT_NOT_SUPP
#define BLE_MESH_BLE_MESH_CFG_STATUS_CANNOT_UPDATE                     STATUS_CANNOT_UPDATE
#define BLE_MESH_BLE_MESH_CFG_STATUS_CANNOT_REMOVE                     STATUS_CANNOT_REMOVE
#define BLE_MESH_BLE_MESH_CFG_STATUS_CANNOT_BIND                       STATUS_CANNOT_BIND
#define BLE_MESH_BLE_MESH_CFG_STATUS_TEMP_UNABLE_TO_CHANGE_STATE       STATUS_TEMP_STATE_CHG_FAIL
#define BLE_MESH_BLE_MESH_CFG_STATUS_CANNOT_SET                        STATUS_CANNOT_SET
#define BLE_MESH_BLE_MESH_CFG_STATUS_UNSPECIFIED_ERROR                 STATUS_UNSPECIFIED
#define BLE_MESH_BLE_MESH_CFG_STATUS_INVALID_BINDING                   STATUS_INVALID_BINDING
typedef uint8_t ble_mesh_cfg_status_t;      /*!< This typedef is only used to indicate the status code
                                                  contained in some of the Config Server Model status message */

#define BLE_MESH_BLE_MESH_MODEL_STATUS_SUCCESS               0x00
#define BLE_MESH_BLE_MESH_MODEL_STATUS_CANNOT_SET_RANGE_MIN  0x01
#define BLE_MESH_BLE_MESH_MODEL_STATUS_CANNOT_SET_RANGE_MAX  0x02
typedef uint8_t ble_mesh_model_status_t;    /*!< This typedef is only used to indicate the status code contained in
                                                  some of the server model (e.g. Generic Server Model) status message */

/** @def    BLE_MESH_BLE_MESH_TRANSMIT
 *
 *  @brief  Encode transmission count & interval steps.
 *
 *  @note   For example, BLE_MESH_BLE_MESH_TRANSMIT(2, 20) means that the message
 *          will be sent about 90ms(count is 3, step is 1, interval is 30 ms
 *          which includes 10ms of advertising interval random delay).
 *
 *  @param  count   Number of retransmissions (first transmission is excluded).
 *  @param  int_ms  Interval steps in milliseconds. Must be greater than 0
 *                  and a multiple of 10.
 *
 *  @return BLE Mesh transmit value that can be used e.g. for the default
 *          values of the Configuration Model data.
 */
#define BLE_MESH_BLE_MESH_TRANSMIT(count, int_ms) BLE_MESH_TRANSMIT(count, int_ms)

/** @def BLE_MESH_BLE_MESH_GET_TRANSMIT_COUNT
 *
 *  @brief Decode transmit count from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission count (actual transmissions equal to N + 1).
 */
#define BLE_MESH_BLE_MESH_GET_TRANSMIT_COUNT(transmit) BLE_MESH_TRANSMIT_COUNT(transmit)

/** @def BLE_MESH_BLE_MESH_GET_TRANSMIT_INTERVAL
 *
 *  @brief Decode transmit interval from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define BLE_MESH_BLE_MESH_GET_TRANSMIT_INTERVAL(transmit) BLE_MESH_TRANSMIT_INT(transmit)

/** @def BLE_MESH_BLE_MESH_PUBLISH_TRANSMIT
 *
 *  @brief Encode Publish Retransmit count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0
 *                 and a multiple of 50.
 *
 *  @return BLE Mesh transmit value that can be used e.g. for the default
 *          values of the Configuration Model data.
 */
#define BLE_MESH_BLE_MESH_PUBLISH_TRANSMIT(count, int_ms) BLE_MESH_PUB_TRANSMIT(count, int_ms)

/** @def BLE_MESH_BLE_MESH_GET_PUBLISH_TRANSMIT_COUNT
 *
 *  @brief Decode Publish Retransmit count from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Retransmission count (actual transmissions equal to N + 1).
 */
#define BLE_MESH_BLE_MESH_GET_PUBLISH_TRANSMIT_COUNT(transmit) BLE_MESH_PUB_TRANSMIT_COUNT(transmit)

/** @def BLE_MESH_BLE_MESH_GET_PUBLISH_TRANSMIT_INTERVAL
 *
 *  @brief Decode Publish Retransmit interval from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define BLE_MESH_BLE_MESH_GET_PUBLISH_TRANSMIT_INTERVAL(transmit) BLE_MESH_PUB_TRANSMIT_INT(transmit)

/*!< Callbacks which are not needed to be initialized by users (set with 0 and will be initialized internally) */
typedef uint32_t ble_mesh_cb_t;

typedef enum {
    BLE_MESH_BLE_MESH_TYPE_PROV_CB,
    BLE_MESH_BLE_MESH_TYPE_OUTPUT_NUM_CB,
    BLE_MESH_BLE_MESH_TYPE_OUTPUT_STR_CB,
    BLE_MESH_BLE_MESH_TYPE_INTPUT_CB,
    BLE_MESH_BLE_MESH_TYPE_LINK_OPEN_CB,
    BLE_MESH_BLE_MESH_TYPE_LINK_CLOSE_CB,
    BLE_MESH_BLE_MESH_TYPE_COMPLETE_CB,
    BLE_MESH_BLE_MESH_TYPE_RESET_CB,
} ble_mesh_cb_type_t;

/*!< This enum value is provisioning authentication oob method */
typedef enum {
    BLE_MESH_BLE_MESH_NO_OOB,
    BLE_MESH_BLE_MESH_STATIC_OOB,
    BLE_MESH_BLE_MESH_OUTPUT_OOB,
    BLE_MESH_BLE_MESH_INPUT_OOB,
} ble_mesh_oob_method_t;

/*!< This enum value is associated with bt_mesh_output_action_t in mesh_main.h */
typedef enum {
    BLE_MESH_BLE_MESH_NO_OUTPUT       = 0,
    BLE_MESH_BLE_MESH_BLINK           = BIT(0),
    BLE_MESH_BLE_MESH_BEEP            = BIT(1),
    BLE_MESH_BLE_MESH_VIBRATE         = BIT(2),
    BLE_MESH_BLE_MESH_DISPLAY_NUMBER  = BIT(3),
    BLE_MESH_BLE_MESH_DISPLAY_STRING  = BIT(4),
} ble_mesh_output_action_t;

/*!< This enum value is associated with bt_mesh_input_action_t in mesh_main.h */
typedef enum {
    BLE_MESH_BLE_MESH_NO_INPUT      = 0,
    BLE_MESH_BLE_MESH_PUSH          = BIT(0),
    BLE_MESH_BLE_MESH_TWIST         = BIT(1),
    BLE_MESH_BLE_MESH_ENTER_NUMBER  = BIT(2),
    BLE_MESH_BLE_MESH_ENTER_STRING  = BIT(3),
} ble_mesh_input_action_t;

/*!< This enum value is associated with bt_mesh_prov_bearer_t in mesh_main.h */
typedef enum {
    BLE_MESH_BLE_MESH_PROV_ADV   = BIT(0),
    BLE_MESH_BLE_MESH_PROV_GATT  = BIT(1),
} ble_mesh_prov_bearer_t;

/*!< This enum value is associated with bt_mesh_prov_oob_info_t in mesh_main.h */
typedef enum {
    BLE_MESH_BLE_MESH_PROV_OOB_OTHER     = BIT(0),
    BLE_MESH_BLE_MESH_PROV_OOB_URI       = BIT(1),
    BLE_MESH_BLE_MESH_PROV_OOB_2D_CODE   = BIT(2),
    BLE_MESH_BLE_MESH_PROV_OOB_BAR_CODE  = BIT(3),
    BLE_MESH_BLE_MESH_PROV_OOB_NFC       = BIT(4),
    BLE_MESH_BLE_MESH_PROV_OOB_NUMBER    = BIT(5),
    BLE_MESH_BLE_MESH_PROV_OOB_STRING    = BIT(6),
    /* 7 - 10 are reserved */
    BLE_MESH_BLE_MESH_PROV_OOB_ON_BOX    = BIT(11),
    BLE_MESH_BLE_MESH_PROV_OOB_IN_BOX    = BIT(12),
    BLE_MESH_BLE_MESH_PROV_OOB_ON_PAPER  = BIT(13),
    BLE_MESH_BLE_MESH_PROV_OOB_IN_MANUAL = BIT(14),
    BLE_MESH_BLE_MESH_PROV_OOB_ON_DEV    = BIT(15),
} ble_mesh_prov_oob_info_t;

/*!< Macros used to define message opcode */
#define BLE_MESH_BLE_MESH_MODEL_OP_1(b0)            BLE_MESH_MODEL_OP_1(b0)
#define BLE_MESH_BLE_MESH_MODEL_OP_2(b0, b1)        BLE_MESH_MODEL_OP_2(b0, b1)
#define BLE_MESH_BLE_MESH_MODEL_OP_3(b0, cid)       BLE_MESH_MODEL_OP_3(b0, cid)

/*!< This macro is associated with BLE_MESH_MODEL in mesh_access.h */
#define BLE_MESH_BLE_MESH_SIG_MODEL(_id, _op, _pub, _user_data)          \
{                                                                   \
    .model_id = (_id),                                              \
    .op = _op,                                                      \
    .keys = { [0 ... (CONFIG_BLE_MESH_MODEL_KEY_COUNT - 1)] =       \
            BLE_MESH_BLE_MESH_KEY_UNUSED },                              \
    .pub = _pub,                                                    \
    .groups = { [0 ... (CONFIG_BLE_MESH_MODEL_GROUP_COUNT - 1)] =   \
            BLE_MESH_BLE_MESH_ADDR_UNASSIGNED },                         \
    .user_data = _user_data,                                        \
}

/*!< This macro is associated with BLE_MESH_MODEL_VND in mesh_access.h */
#define BLE_MESH_BLE_MESH_VENDOR_MODEL(_company, _id, _op, _pub, _user_data) \
{                                                                       \
    .vnd.company_id = (_company),                                       \
    .vnd.model_id = (_id),                                              \
    .op = _op,                                                          \
    .pub = _pub,                                                        \
    .keys = { [0 ... (CONFIG_BLE_MESH_MODEL_KEY_COUNT - 1)] =           \
            BLE_MESH_BLE_MESH_KEY_UNUSED },                                  \
    .groups = { [0 ... (CONFIG_BLE_MESH_MODEL_GROUP_COUNT - 1)] =       \
            BLE_MESH_BLE_MESH_ADDR_UNASSIGNED },                             \
    .user_data = _user_data,                                            \
}

/** @brief Helper to define a BLE Mesh element within an array.
 *
 *  In case the element has no SIG or Vendor models, the helper
 *  macro BLE_MESH_BLE_MESH_MODEL_NONE can be given instead.
 *
 *  @note This macro is associated with BLE_MESH_ELEM in mesh_access.h
 *
 *  @param _loc       Location Descriptor.
 *  @param _mods      Array of SIG models.
 *  @param _vnd_mods  Array of vendor models.
 */
#define BLE_MESH_BLE_MESH_ELEMENT(_loc, _mods, _vnd_mods)    \
{                                                       \
    .location         = (_loc),                         \
    .sig_model_count  = ARRAY_SIZE(_mods),              \
    .sig_models       = (_mods),                        \
    .vnd_model_count  = ARRAY_SIZE(_vnd_mods),          \
    .vnd_models       = (_vnd_mods),                    \
}

#define BLE_MESH_BLE_MESH_PROV(uuid, sta_val, sta_val_len, out_size, out_act, in_size, in_act) { \
    .uuid           = uuid,         \
    .static_val     = sta_val,      \
    .static_val_len = sta_val_len,  \
    .output_size    = out_size,     \
    .output_action  = out_act,      \
    .input_size     = in_size,      \
    .input_action   = in_act,       \
}

typedef struct ble_mesh_model ble_mesh_model_t;

/** Abstraction that describes a BLE Mesh Element.
 *  This structure is associated with struct bt_mesh_elem in mesh_access.h
 */
typedef struct {
    /** Element Address, assigned during provisioning. */
    uint16_t element_addr;

    /** Location Descriptor (GATT Bluetooth Namespace Descriptors) */
    const uint16_t location;

    const uint8_t sig_model_count;      /*!< SIG Model count */
    const uint8_t vnd_model_count;      /*!< Vendor Model count */

    ble_mesh_model_t *sig_models;   /*!< SIG Models */
    ble_mesh_model_t *vnd_models;   /*!< Vendor Models */
} ble_mesh_elem_t;

/** Abstraction that describes a model publication context.
 *  This structure is associated with struct bt_mesh_model_pub in mesh_access.h
 */
typedef struct {
    /** Pointer to the model to which the context belongs. Initialized by the stack. */
    ble_mesh_model_t *model;

    uint16_t publish_addr; /*!< Publish Address. */
    uint16_t app_idx;      /*!< Publish AppKey Index. */

    uint8_t  ttl;          /*!< Publish Time to Live. */
    uint8_t  retransmit;   /*!< Retransmit Count & Interval Steps. */

    uint8_t  period;        /*!< Publish Period. */
    uint16_t period_div: 4, /*!< Divisor for the Period. */
             cred: 1,       /*!< Friendship Credentials Flag. */
             fast_period: 1, /*!< Use FastPeriodDivisor */
             count: 3;      /*!< Retransmissions left. */

    uint32_t period_start; /*!< Start of the current period. */

    /** @brief Publication buffer, containing the publication message.
     *
     *  This will get correctly created when the publication context
     *  has been defined using the BLE_MESH_BLE_MESH_MODEL_PUB_DEFINE macro.
     *
     *  BLE_MESH_BLE_MESH_MODEL_PUB_DEFINE(name, size);
     */
    struct net_buf_simple *msg;

    /** Callback used to update publish message. Initialized by the stack. */
    ble_mesh_cb_t update;

    /** Role of the device that is going to publish messages */
    uint8_t dev_role;

    /** Publish Period Timer. Initialized by the stack. */
    void* timer;
} ble_mesh_model_pub_t;

/** @def BLE_MESH_BLE_MESH_MODEL_PUB_DEFINE
 *
 *  Define a model publication context.
 *
 *  @param _name    Variable name given to the context.
 *  @param _msg_len Length of the publication message.
 *  @param _role    Role of the device which contains the model.
 */
#define BLE_MESH_BLE_MESH_MODEL_PUB_DEFINE(_name, _msg_len, _role) \
    NET_BUF_SIMPLE_DEFINE_STATIC(bt_mesh_pub_msg_##_name, _msg_len); \
    static ble_mesh_model_pub_t _name = { \
        .update = (uint32_t)NULL, \
        .msg = &bt_mesh_pub_msg_##_name, \
        .dev_role = _role, \
    }

/** @def BLE_MESH_BLE_MESH_MODEL_OP
 *
 *  Define a model operation context.
 *
 *  @param _opcode  Message opcode.
 *  @param _min_len Message minimum length.
 */
#define BLE_MESH_BLE_MESH_MODEL_OP(_opcode, _min_len) \
{ \
    .opcode = _opcode, \
    .min_len = _min_len, \
    .param_cb = (uint32_t)NULL, \
}

/** Abstraction that describes a model operation context.
 *  This structure is associated with struct bt_mesh_model_op in mesh_access.h
 */
typedef struct {
    const uint32_t    opcode;   /*!< Message opcode */
    const size_t      min_len;  /*!< Message minimum length */
    ble_mesh_cb_t param_cb; /*!< Callback used to handle message. Initialized by the stack. */
} ble_mesh_model_op_t;

/** Define the terminator for the model operation table.
 *  Each model operation struct array must use this terminator as
 *  the end tag of the operation unit.
 */
#define BLE_MESH_BLE_MESH_MODEL_OP_END {0, 0, 0}

/** Abstraction that describes a Mesh Model instance.
 *  This structure is associated with struct bt_mesh_model in mesh_access.h
 */
struct ble_mesh_model {
    /** Model ID */
    union {
        const uint16_t model_id;
        struct {
            uint16_t company_id;
            uint16_t model_id;
        } vnd;
    };

    /** Internal information, mainly for persistent storage */
    uint8_t  element_idx;   /*!< Belongs to Nth element */
    uint8_t  model_idx;     /*!< Is the Nth model in the element */
    uint16_t flags;         /*!< Information about what has changed */

    /** The Element to which this Model belongs */
    ble_mesh_elem_t *element;

    /** Model Publication */
    ble_mesh_model_pub_t *const pub;

    /** AppKey List */
    uint16_t keys[CONFIG_BLE_MESH_MODEL_KEY_COUNT];

    /** Subscription List (group or virtual addresses) */
    uint16_t groups[CONFIG_BLE_MESH_MODEL_GROUP_COUNT];

    /** Model operation context */
    ble_mesh_model_op_t *op;

    /** Model-specific user data */
    void *user_data;
};

/** Helper to define an empty model array.
 *  This structure is associated with BLE_MESH_MODEL_NONE in mesh_access.h
 */
#define BLE_MESH_BLE_MESH_MODEL_NONE ((ble_mesh_model_t []){})

/** Message sending context.
 *  This structure is associated with struct bt_mesh_msg_ctx in mesh_access.h
 */
typedef struct {
    /** NetKey Index of the subnet through which to send the message. */
    uint16_t net_idx;

    /** AppKey Index for message encryption. */
    uint16_t app_idx;

    /** Remote address. */
    uint16_t addr;

    /** Destination address of a received message. Not used for sending. */
    uint16_t recv_dst;

    /** Received TTL value. Not used for sending. */
    uint8_t  recv_ttl: 7;

    /** Force sending reliably by using segment acknowledgement */
    uint8_t  send_rel: 1;

    /** TTL, or BLE_MESH_TTL_DEFAULT for default TTL. */
    uint8_t  send_ttl;

    /** Opcode of a received message. Not used for sending message. */
    uint32_t recv_op;

    /** Model corresponding to the message, no need to be initialized before sending message */
    ble_mesh_model_t *model;

    /** Indicate if the message is sent by a node server model, no need to be initialized before sending message */
    bool srv_send;
} ble_mesh_msg_ctx_t;

/** Provisioning properties & capabilities.
 *  This structure is associated with struct bt_mesh_prov in mesh_access.h
 */
typedef struct {
#if CONFIG_BLE_MESH_NODE
    /** The UUID that is used when advertising as an unprovisioned device */
    const uint8_t *uuid;

    /** Optional URI. This will be advertised separately from the
     *  unprovisioned beacon, however the unprovisioned beacon will
     *  contain a hash of it so the two can be associated by the
     *  provisioner.
     */
    const char *uri;

    /** Out of Band information field. */
    ble_mesh_prov_oob_info_t oob_info;

    /** Flag indicates whether unprovisioned devices support OOB public key */
    bool oob_pub_key;

    /** Callback used to notify to set OOB Public Key. Initialized by the stack. */
    ble_mesh_cb_t oob_pub_key_cb;

    /** Static OOB value */
    const uint8_t *static_val;
    /** Static OOB value length */
    uint8_t        static_val_len;

    /** Maximum size of Output OOB supported */
    uint8_t        output_size;
    /** Supported Output OOB Actions */
    uint16_t       output_actions;

    /** Maximum size of Input OOB supported */
    uint8_t        input_size;
    /** Supported Input OOB Actions */
    uint16_t       input_actions;

    /** Callback used to output the number. Initialized by the stack. */
    ble_mesh_cb_t  output_num_cb;
    /** Callback used to output the string. Initialized by the stack. */
    ble_mesh_cb_t  output_str_cb;
    /** Callback used to notify to input number/string. Initialized by the stack. */
    ble_mesh_cb_t  input_cb;
    /** Callback used to indicate that link is opened. Initialized by the stack. */
    ble_mesh_cb_t  link_open_cb;
    /** Callback used to indicate that link is closed. Initialized by the stack. */
    ble_mesh_cb_t  link_close_cb;
    /** Callback used to indicate that provisioning is completed. Initialized by the stack. */
    ble_mesh_cb_t  complete_cb;
    /** Callback used to indicate that node has been reset. Initialized by the stack. */
    ble_mesh_cb_t  reset_cb;
#endif /* CONFIG_BLE_MESH_NODE */

#ifdef CONFIG_BLE_MESH_PROVISIONER
    /** Provisioner device UUID */
    const uint8_t *prov_uuid;

    /** Primary element address of the provisioner */
    const uint16_t prov_unicast_addr;

    /** Pre-incremental unicast address value to be assigned to the first device */
    uint16_t       prov_start_address;

    /** Attention timer contained in Provisioning Invite PDU */
    uint8_t        prov_attention;

    /** Provisioning Algorithm for the Provisioner */
    uint8_t        prov_algorithm;

    /** Provisioner public key oob */
    uint8_t        prov_pub_key_oob;

    /** Callback used to notify to set device OOB Public Key. Initialized by the stack. */
    ble_mesh_cb_t provisioner_prov_read_oob_pub_key;

    /** Provisioner static oob value */
    uint8_t        *prov_static_oob_val;
    /** Provisioner static oob value length */
    uint8_t         prov_static_oob_len;

    /** Callback used to notify to input number/string. Initialized by the stack. */
    ble_mesh_cb_t provisioner_prov_input;
    /** Callback used to output number/string. Initialized by the stack. */
    ble_mesh_cb_t provisioner_prov_output;

    /** Key refresh and IV update flag */
    uint8_t        flags;

    /** IV index */
    uint32_t       iv_index;

    /** Callback used to indicate that link is opened. Initialized by the stack. */
    ble_mesh_cb_t  provisioner_link_open;
    /** Callback used to indicate that link is closed. Initialized by the stack. */
    ble_mesh_cb_t  provisioner_link_close;
    /** Callback used to indicate that a device is provisioned. Initialized by the stack. */
    ble_mesh_cb_t  provisioner_prov_comp;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
} ble_mesh_prov_t;

/** Node Composition data context.
 *  This structure is associated with struct bt_mesh_comp in mesh_access.h
 */
typedef struct {
    uint16_t cid;   /*!< 16-bit SIG-assigned company identifier */
    uint16_t pid;   /*!< 16-bit vendor-assigned product identifier */
    uint16_t vid;   /*!< 16-bit vendor-assigned product version identifier */

    size_t element_count;           /*!< Element count */
    ble_mesh_elem_t *elements;  /*!< A sequence of elements */
} ble_mesh_comp_t;

/*!< This enum value is the role of the device */
typedef enum {
    ROLE_NODE = 0,
    ROLE_PROVISIONER,
    ROLE_FAST_PROV,
} ble_mesh_dev_role_t;

/** Common parameters of the messages sent by Client Model. */
typedef struct {
    ble_mesh_opcode_t opcode;   /*!< Message opcode */
    ble_mesh_model_t *model;    /*!< Pointer to the client model structure */
    ble_mesh_msg_ctx_t ctx;     /*!< The context used to send message */
    int32_t msg_timeout;            /*!< Timeout value (ms) to get response to the sent message */
                                    /*!< Note: if using default timeout value in menuconfig, make sure to set this value to 0 */
    uint8_t msg_role;               /*!< Role of the device - Node/Provisioner */
} ble_mesh_client_common_param_t;

/*!< Flag which will be set when device is going to be added. */
typedef uint8_t ble_mesh_dev_add_flag_t;
#define ADD_DEV_RM_AFTER_PROV_FLAG  BIT(0)  /*!< Device will be removed from queue after provisioned successfully */
#define ADD_DEV_START_PROV_NOW_FLAG BIT(1)  /*!< Start provisioning device immediately */
#define ADD_DEV_FLUSHABLE_DEV_FLAG  BIT(2)  /*!< Device can be remove when queue is full and new device is going to added */

/** Information of the device which is going to be added for provisioning. */
typedef struct {
    yoc_bd_addr_t addr;                 /*!< Device address */
    yoc_ble_addr_type_t addr_type;      /*!< Device address type */
    uint8_t  uuid[16];                  /*!< Device UUID */
    uint16_t oob_info;                  /*!< Device OOB Info */
    /*!< ADD_DEV_START_PROV_NOW_FLAG shall not be set if the bearer has both PB-ADV and PB-GATT enabled */
    ble_mesh_prov_bearer_t bearer;  /*!< Provisioning Bearer */
} ble_mesh_unprov_dev_add_t;

#define DEL_DEV_ADDR_FLAG BIT(0)
#define DEL_DEV_UUID_FLAG BIT(1)
/** Information of the device which is going to be deleted. */
typedef struct {
    union {
        struct {
            yoc_bd_addr_t addr;             /*!< Device address */
            yoc_ble_addr_type_t addr_type;  /*!< Device address type */
        };
        uint8_t uuid[16];                   /*!< Device UUID */
    };
    uint8_t flag;                           /*!< BIT0: device address; BIT1: device UUID */
} ble_mesh_device_delete_t;

#define PROV_DATA_NET_IDX_FLAG  BIT(0)
#define PROV_DATA_FLAGS_FLAG    BIT(1)
#define PROV_DATA_IV_INDEX_FLAG BIT(2)
/** Information of the provisioner which is going to be updated. */
typedef struct {
    union {
        uint16_t net_idx;   /*!< NetKey Index */
        uint8_t  flags;     /*!< Flags */
        uint32_t iv_index;  /*!< IV Index */
    };
    uint8_t flag;           /*!< BIT0: net_idx; BIT1: flags; BIT2: iv_index */
} ble_mesh_prov_data_info_t;

/** Context of fast provisioning which need to be set. */
typedef struct {
    uint16_t unicast_min;   /*!< Minimum unicast address used for fast provisioning */
    uint16_t unicast_max;   /*!< Maximum unicast address used for fast provisioning */
    uint16_t net_idx;       /*!< Netkey index used for fast provisioning */
    uint8_t  flags;         /*!< Flags used for fast provisioning */
    uint32_t iv_index;      /*!< IV Index used for fast provisioning */
    uint8_t  offset;        /*!< Offset of the UUID to be compared */
    uint8_t  match_len;     /*!< Length of the UUID to be compared */
    uint8_t  match_val[16]; /*!< Value of UUID to be compared */
} ble_mesh_fast_prov_info_t;

/*!< This enum value is the action of fast provisioning */
typedef enum {
    FAST_PROV_ACT_NONE,
    FAST_PROV_ACT_ENTER,
    FAST_PROV_ACT_SUSPEND,
    FAST_PROV_ACT_EXIT,
    FAST_PROV_ACT_MAX,
} ble_mesh_fast_prov_action_t;

/*!< This enum value is the event of node/provisioner/fast provisioning */
typedef enum {
    BLE_MESH_BLE_MESH_PROV_REGISTER_COMP_EVT,                        /*!< Initialize BLE Mesh provisioning capabilities and internal data information completion event */
    BLE_MESH_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT,             /*!< Set the unprovisioned device name completion event */
    BLE_MESH_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT,                     /*!< Enable node provisioning functionality completion event */
    BLE_MESH_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT,                    /*!< Disable node provisioning functionality completion event */
    BLE_MESH_BLE_MESH_NODE_PROV_LINK_OPEN_EVT,                       /*!< Establish a BLE Mesh link event */
    BLE_MESH_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT,                      /*!< Close a BLE Mesh link event */
    BLE_MESH_BLE_MESH_NODE_PROV_OOB_PUB_KEY_EVT,                     /*!< Generate Node input OOB public key event */
    BLE_MESH_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT,                   /*!< Generate Node Output Number event */
    BLE_MESH_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT,                   /*!< Generate Node Output String event */
    BLE_MESH_BLE_MESH_NODE_PROV_INPUT_EVT,                           /*!< Event requiring the user to input a number or string */
    BLE_MESH_BLE_MESH_NODE_PROV_COMPLETE_EVT,                        /*!< Provisioning done event */
    BLE_MESH_BLE_MESH_NODE_PROV_RESET_EVT,                           /*!< Provisioning reset event */
    BLE_MESH_BLE_MESH_NODE_PROV_SET_OOB_PUB_KEY_COMP_EVT,            /*!< Node set oob public key completion event */
    BLE_MESH_BLE_MESH_NODE_PROV_INPUT_NUMBER_COMP_EVT,               /*!< Node input number completion event */
    BLE_MESH_BLE_MESH_NODE_PROV_INPUT_STRING_COMP_EVT,               /*!< Node input string completion event */
    BLE_MESH_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT,           /*!< Enable BLE Mesh Proxy Identity advertising completion event */
    BLE_MESH_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT,               /*!< Enable BLE Mesh GATT Proxy Service completion event */
    BLE_MESH_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT,              /*!< Disable BLE Mesh GATT Proxy Service completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT,              /*!< Provisioner enable provisioning functionality completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT,             /*!< Provisioner disable provisioning functionality completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT,           /*!< Provisioner receives unprovisioned device beacon event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_EVT,         /*!< Provisioner read unprovisioned device OOB public key event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_EVT,                    /*!< Provisioner input value for provisioning procedure event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_OUTPUT_EVT,                   /*!< Provisioner output value for provisioning procedure event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT,                /*!< Provisioner establish a BLE Mesh link event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT,               /*!< Provisioner close a BLE Mesh link event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT,                 /*!< Provisioner provisioning done event */
    BLE_MESH_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT,           /*!< Provisioner add a device to the list which contains devices that are waiting/going to be provisioned completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT,               /*!< Provisioner delete a device from the list, close provisioning link with the device if it exists and remove the device from network completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT,       /*!< Provisioner set the value to be compared with part of the unprovisioned device UUID completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT,       /*!< Provisioner set net_idx/flags/iv_index used for provisioning completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT,    /*!< Provisioner read unprovisioned device OOB public key completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT,        /*!< Provisioner input number completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT,        /*!< Provisioner input string completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT,            /*!< Provisioner set node name completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT,        /*!< Provisioner add local app key completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT,    /*!< Provisioner bind local model with local app key completion event */
    BLE_MESH_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT,        /*!< Provisioner add local network key completion event */
    BLE_MESH_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT,                   /*!< Set fast provisioning information (e.g. unicast address range, net_idx, etc.) completion event */
    BLE_MESH_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT,                 /*!< Set fast provisioning action completion event */
    BLE_MESH_BLE_MESH_PROV_EVT_MAX,
} ble_mesh_prov_cb_event_t;

/*!< This enum value is the event of undefined SIG Models and Vendor Models */
typedef enum {
    BLE_MESH_BLE_MESH_MODEL_OPERATION_EVT,               /*!< User-defined models receive messages from peer devices (e.g. get, set, status, etc) event */
    BLE_MESH_BLE_MESH_MODEL_SEND_COMP_EVT,               /*!< User-defined models send messages completion event */
    BLE_MESH_BLE_MESH_MODEL_PUBLISH_COMP_EVT,            /*!< User-defined models publish messages completion event */
    BLE_MESH_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT, /*!< User-defined client models receive publish messages event */
    BLE_MESH_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT,     /*!< Timeout event for the user-defined client models that failed to receive response from peer server models */
    BLE_MESH_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT,          /*!< When a model is configured to publish messages periodically, this event will occur during every publish period */
    BLE_MESH_BLE_MESH_MODEL_EVT_MAX,
} ble_mesh_model_cb_event_t;

/**
 * @brief BLE Mesh Node/Provisioner callback parameters union
 */
typedef union {
    /**
     * @brief BLE_MESH_BLE_MESH_PROV_REGISTER_COMP_EVT
     */
    struct ble_mesh_prov_register_comp_param {
        int err_code;                           /*!< Indicate the result of BLE Mesh initialization */
    } prov_register_comp;                       /*!< Event parameter of BLE_MESH_BLE_MESH_PROV_REGISTER_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT
     */
    struct ble_mesh_set_unprov_dev_name_comp_param {
        int err_code;                           /*!< Indicate the result of setting BLE Mesh device name */
    } node_set_unprov_dev_name_comp;            /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT
     */
    struct ble_mesh_prov_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling BLE Mesh device */
    } node_prov_enable_comp;                    /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT
     */
    struct ble_mesh_prov_disable_comp_param {
        int err_code;                           /*!< Indicate the result of disabling BLE Mesh device */
    } node_prov_disable_comp;                   /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_LINK_OPEN_EVT
     */
    struct ble_mesh_link_open_evt_param {
        ble_mesh_prov_bearer_t bearer;      /*!< Type of the bearer used when device link is open */
    } node_prov_link_open;                      /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_LINK_OPEN_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT
     */
    struct ble_mesh_link_close_evt_param {
        ble_mesh_prov_bearer_t bearer;      /*!< Type of the bearer used when device link is closed */
    } node_prov_link_close;                     /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT
     */
    struct ble_mesh_output_num_evt_param {
        ble_mesh_output_action_t action;    /*!< Action of Output OOB Authentication */
        uint32_t number;                        /*!< Number of Output OOB Authentication  */
    } node_prov_output_num;                     /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT
     */
    struct ble_mesh_output_str_evt_param {
        char string[8];                         /*!< String of Output OOB Authentication */
    } node_prov_output_str;                     /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_INPUT_EVT
     */
    struct ble_mesh_input_evt_param {
        ble_mesh_input_action_t action;     /*!< Action of Input OOB Authentication */
        uint8_t size;                           /*!< Size of Input OOB Authentication */
    } node_prov_input;                          /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_INPUT_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_COMPLETE_EVT
     */
    struct ble_mesh_provision_complete_evt_param {
        uint16_t net_idx;                       /*!< NetKey Index */
        uint16_t addr;                          /*!< Primary address */
        uint8_t  flags;                         /*!< Flags */
        uint32_t iv_index;                      /*!< IV Index */
    } node_prov_complete;                       /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_COMPLETE_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_RESET_EVT
     */
    struct ble_mesh_provision_reset_param {

    } node_prov_reset;                          /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_RESET_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_SET_OOB_PUB_KEY_COMP_EVT
     */
    struct ble_mesh_set_oob_pub_key_comp_param {
        int err_code;                           /*!< Indicate the result of setting OOB Public Key */
    } node_prov_set_oob_pub_key_comp;           /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_SET_OOB_PUB_KEY_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_INPUT_NUM_COMP_EVT
     */
    struct ble_mesh_input_number_comp_param {
        int err_code;                           /*!< Indicate the result of inputting number */
    } node_prov_input_num_comp;                 /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_INPUT_NUM_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROV_INPUT_STR_COMP_EVT
     */
    struct ble_mesh_input_string_comp_param {
        int err_code;                           /*!< Indicate the result of inputting string */
    } node_prov_input_str_comp;                 /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROV_INPUT_STR_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT
     */
    struct ble_mesh_proxy_identity_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling Mesh Proxy advertising */
    } node_proxy_identity_enable_comp;          /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT
     */
    struct ble_mesh_proxy_gatt_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling Mesh Proxy Service */
    } node_proxy_gatt_enable_comp;              /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT
     */
    struct ble_mesh_proxy_gatt_disable_comp_param {
        int err_code;                           /*!< Indicate the result of disabling Mesh Proxy Service */
    } node_proxy_gatt_disable_comp;             /*!< Event parameter of BLE_MESH_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT
     */
    struct ble_mesh_provisioner_recv_unprov_adv_pkt_param {
        uint8_t  dev_uuid[16];                  /*!< Device UUID of the unprovisoned device */
        uint8_t  addr[6];                       /*!< Device address of the unprovisoned device */
        yoc_ble_addr_type_t addr_type;          /*!< Device address type */
        uint16_t oob_info;                      /*!< OOB Info of the unprovisoned device */
        uint8_t  adv_type;                      /*!< Avertising type of the unprovisoned device */
        ble_mesh_prov_bearer_t bearer;      /*!< Bearer of the unprovisoned device */
    } provisioner_recv_unprov_adv_pkt;          /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling BLE Mesh Provisioner */
    } provisioner_prov_enable_comp;             /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_disable_comp_param {
        int err_code;                           /*!< Indicate the result of disabling BLE Mesh Provisioner */
    } provisioner_prov_disable_comp;            /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT
     */
    struct ble_mesh_provisioner_link_open_evt_param {
        ble_mesh_prov_bearer_t bearer;      /*!< Type of the bearer used when Provisioner link is opened */
    } provisioner_prov_link_open;               /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_EVT
     */
    struct ble_mesh_provisioner_prov_read_oob_pub_key_evt_param {
        uint8_t link_idx;                       /*!< Index of the provisioning link */
    } provisioner_prov_read_oob_pub_key;        /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_EVT
     */
    struct ble_mesh_provisioner_prov_input_evt_param {
        ble_mesh_oob_method_t method;       /*!< Method of device Output OOB Authentication */
        ble_mesh_output_action_t action;    /*!< Action of device Output OOB Authentication */
        uint8_t size;                           /*!< Size of device Output OOB Authentication */
        uint8_t link_idx;                       /*!< Index of the provisioning link */
    } provisioner_prov_input;                   /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_OUTPUT_EVT
     */
    struct ble_mesh_provisioner_prov_output_evt_param {
        ble_mesh_oob_method_t method;       /*!< Method of device Input OOB Authentication */
        ble_mesh_input_action_t action;     /*!< Action of device Input OOB Authentication */
        uint8_t size;                           /*!< Size of device Input OOB Authentication */
        uint8_t link_idx;                       /*!< Index of the provisioning link */
        union {
            char string[8];                     /*!< String output by the Provisioner */
            uint32_t number;                    /*!< Number output by the Provisioner */
        };
    } provisioner_prov_output;                  /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_OUTPUT_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT
     */
    struct ble_mesh_provisioner_link_close_evt_param {
        ble_mesh_prov_bearer_t bearer;      /*!< Type of the bearer used when Provisioner link is closed */
        uint8_t reason;                         /*!< Reason of the closed provisioning link */
    } provisioner_prov_link_close;              /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT
     */
    struct ble_mesh_provisioner_prov_comp_param {
        int node_idx;                           /*!< Index of the provisioned device */
        ble_mesh_octet16_t device_uuid;     /*!< Device UUID of the provisioned device */
        uint16_t unicast_addr;                  /*!< Primary address of the provisioned device */
        uint8_t element_num;                    /*!< Element count of the provisioned device */
        uint16_t netkey_idx;                    /*!< NetKey Index of the provisioned device */
    } provisioner_prov_complete;                /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT
     */
    struct ble_mesh_provisioner_add_unprov_dev_comp_param {
        int err_code;                           /*!< Indicate the result of adding device into queue by the Provisioner */
    } provisioner_add_unprov_dev_comp;          /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT
     */
    struct ble_mesh_provisioner_delete_dev_comp_param {
        int err_code;                           /*!< Indicate the result of deleting device by the Provisioner */
    } provisioner_delete_dev_comp;              /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT
     */
    struct ble_mesh_provisioner_set_dev_uuid_match_comp_param {
        int err_code;                           /*!< Indicate the result of setting Device UUID match value by the Provisioner */
    } provisioner_set_dev_uuid_match_comp;      /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT
     */
    struct ble_mesh_provisioner_set_prov_data_info_comp_param {
        int err_code;                           /*!< Indicate the result of setting provisioning info by the Provisioner */
    } provisioner_set_prov_data_info_comp;      /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_read_oob_pub_key_comp_param {
        int err_code;                           /*!< Indicate the result of setting OOB Public Key by the Provisioner */
    } provisioner_prov_read_oob_pub_key_comp;   /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_input_num_comp_param {
        int err_code;                           /*!< Indicate the result of inputting number by the Provisioner */
    } provisioner_prov_input_num_comp;          /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_input_str_comp_param {
        int err_code;                           /*!< Indicate the result of inputting string by the Provisioner */
    } provisioner_prov_input_str_comp;          /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT
     */
    struct ble_mesh_provisioner_set_node_name_comp_param {
        int err_code;                           /*!< Indicate the result of setting provisioned device name by the Provisioner */
        int node_index;                         /*!< Index of the provisioned device */
    } provisioner_set_node_name_comp;           /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_add_local_app_key_comp_param {
        int err_code;                           /*!< Indicate the result of adding local AppKey by the Provisioner */
        uint16_t app_idx;                       /*!< AppKey Index */
    } provisioner_add_app_key_comp;             /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT
     */
    struct ble_mesh_provisioner_bind_local_mod_app_comp_param {
        int err_code;                           /*!< Indicate the result of binding AppKey with model by the Provisioner */
    } provisioner_bind_app_key_to_model_comp;   /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_add_local_net_key_comp_param {
        int err_code;                           /*!< Indicate the result of adding local NetKey by the Provisioner */
        uint16_t net_idx;                       /*!< NetKey Index */
    } provisioner_add_net_key_comp;             /*!< Event parameter of BLE_MESH_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT
     */
    struct ble_mesh_set_fast_prov_info_comp_param {
        uint8_t status_unicast;                 /*!< Indicate the result of setting unicast address range of fast provisioning */
        uint8_t status_net_idx;                 /*!< Indicate the result of setting NetKey Index of fast provisioning */
        uint8_t status_match;                   /*!< Indicate the result of setting matching Device UUID of fast provisioning */
    } set_fast_prov_info_comp;                  /*!< Event parameter of BLE_MESH_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT
     */
    struct ble_mesh_set_fast_prov_action_comp_param {
        uint8_t status_action;                  /*!< Indicate the result of setting action of fast provisioning */
    } set_fast_prov_action_comp;                /*!< Event parameter of BLE_MESH_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT */
} ble_mesh_prov_cb_param_t;

/**
 * @brief BLE Mesh model callback parameters union
 */
typedef union {
    /**
     * @brief BLE_MESH_BLE_MESH_MODEL_OPERATION_EVT
     */
    struct ble_mesh_model_operation_evt_param {
        uint32_t opcode;                /*!< Opcode of the recieved message */
        ble_mesh_model_t *model;    /*!< Pointer to the model which receives the message */
        ble_mesh_msg_ctx_t *ctx;    /*!< Pointer to the context of the received message */
        uint16_t length;                /*!< Length of the received message */
        uint8_t *msg;                   /*!< Value of the received message */
    } model_operation;                  /*!< Event parameter of BLE_MESH_BLE_MESH_MODEL_OPERATION_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_MODEL_SEND_COMP_EVT
     */
    struct ble_mesh_model_send_comp_param {
        int err_code;                   /*!< Indicate the result of sending a message */
        uint32_t opcode;                /*!< Opcode of the message */
        ble_mesh_model_t *model;    /*!< Pointer to the model which sends the message */
        ble_mesh_msg_ctx_t *ctx;    /*!< Context of the message */
    } model_send_comp;                  /*!< Event parameter of BLE_MESH_BLE_MESH_MODEL_SEND_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_MODEL_PUBLISH_COMP_EVT
     */
    struct ble_mesh_model_publish_comp_param {
        int err_code;                   /*!< Indicate the result of publishing a message */
        ble_mesh_model_t *model;    /*!< Pointer to the model which publishes the message */
    } model_publish_comp;               /*!< Event parameter of BLE_MESH_BLE_MESH_MODEL_PUBLISH_COMP_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT
     */
    struct ble_mesh_mod_recv_publish_msg_param {
        uint32_t opcode;                /*!< Opcode of the unsoliciated received message */
        ble_mesh_model_t *model;    /*!< Pointer to the model which receives the message */
        ble_mesh_msg_ctx_t *ctx;    /*!< Pointer to the context of the message */
        uint16_t length;                /*!< Length of the received message */
        uint8_t *msg;                   /*!< Value of the received message */
    } client_recv_publish_msg;          /*!< Event parameter of BLE_MESH_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT
     */
    struct ble_mesh_client_model_send_timeout_param {
        uint32_t opcode;                /*!< Opcode of the previously sent message */
        ble_mesh_model_t *model;    /*!< Pointer to the model which sends the previous message */
        ble_mesh_msg_ctx_t *ctx;    /*!< Pointer to the context of the previous message */
    } client_send_timeout;              /*!< Event parameter of BLE_MESH_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT */
    /**
     * @brief BLE_MESH_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT
     */
    struct ble_mesh_model_publish_update_evt_param {
        ble_mesh_model_t *model;    /*!< Pointer to the model which is going to update its publish message */
    } model_publish_update;             /*!< Event parameter of BLE_MESH_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT */
} ble_mesh_model_cb_param_t;

/** Client Model Get/Set message opcode and corresponding Status message opcode */
typedef struct {
    uint32_t cli_op;        /*!< The client message opcode */
    uint32_t status_op;     /*!< The server status opcode corresponding to the client message opcode */
} ble_mesh_client_op_pair_t;

/** Client Model user data context. */
typedef struct {
    ble_mesh_model_t *model;                    /*!< Pointer to the client model. Initialized by the stack. */
    int op_pair_size;                               /*!< Size of the op_pair */
    const ble_mesh_client_op_pair_t *op_pair;   /*!< Table containing get/set message opcode and corresponding status message opcode */
    uint32_t publish_status;                        /*!< Callback used to handle the received unsoliciated message. Initialized by the stack. */
    void *internal_data;                            /*!< Pointer to the internal data of client model */
    uint8_t msg_role;                               /*!< Role of the device (Node/Provisioner) that is going to send messages */
} ble_mesh_client_t;

#endif /* _BLE_MESH_BLE_MESH_DEFS_H_ */
