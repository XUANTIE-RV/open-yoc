/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MESH_H__
#define __MESH_H__

#include <stdint.h>
#include <aos/models.h>
#include <aos/ble.h>

#ifdef __cplusplus
extern "C" {
#endif



/****** Output OOB Action *****/
typedef enum {
    MESH_NO_OUTPUT       = 0,
    MESH_BLINK           = BIT(0),
    MESH_BEEP            = BIT(1),
    MESH_VIBRATE         = BIT(2),
    MESH_DISPLAY_NUMBER  = BIT(3),
    MESH_DISPLAY_STRING  = BIT(4),
} mesh_output_oob_action_en;

/****** Input OOB Actions *****/
typedef enum {
    MESH_NO_INPUT      = 0,
    MESH_PUSH          = BIT(0),
    MESH_TWIST         = BIT(1),
    MESH_ENTER_NUMBER  = BIT(2),
    MESH_ENTER_STRING  = BIT(3),
} mesh_input_oob_action_en;

/****** provisioning capabilities  *****/
typedef struct _mesh_prov_capabilities_t {
    /* Static OOB value length
       The maximum size of the Static OOB information is 16 octets
    */
    uint8_t        static_val_len;
    /* Static OOB value */
    const uint8_t *static_val;
    /* Maximum size of Output OOB supported
       0x00 :       does not support output OOB
       0x01~ 0x08 : vaild
       0x09–0xFF:   RFU
    */
    uint8_t        output_oob_size;
    /* Output OOB Action, see \ref mesh_output_oob_action_en */
    uint16_t       output_oob_actions;
    /* Maximum size of Input OOB supported
       0x00 :       does not support input OOB
       0x01~ 0x08 : vaild
       0x09–0xFF:   RFU
    */
    uint8_t        input_oob_size;
    /* Input OOB Action, see \ref mesh_input_oob_action_en */
    uint16_t       input_oob_actions;
} mesh_prov_capabilities_t;

/****** OOB Information field *****/
typedef enum {
    MESH_PROV_OOB_NONE      = 0,
    MESH_PROV_OOB_OTHER     = BIT(0),
    MESH_PROV_OOB_URI       = BIT(1),
    MESH_PROV_OOB_2D_CODE   = BIT(2),
    MESH_PROV_OOB_BAR_CODE  = BIT(3),
    MESH_PROV_OOB_NFC       = BIT(4),
    MESH_PROV_OOB_NUMBER    = BIT(5),
    MESH_PROV_OOB_STRING    = BIT(6),
    /* 7 - 10 are reserved */
    MESH_PROV_OOB_ON_BOX    = BIT(11),
    MESH_PROV_OOB_IN_BOX    = BIT(12),
    MESH_PROV_OOB_ON_PAPER  = BIT(13),
    MESH_PROV_OOB_IN_MANUAL = BIT(14),
    MESH_PROV_OOB_ON_DEV    = BIT(15),
} mesh_prov_oob_info_en;

/****** mesh provisioning config *****/
typedef struct _mesh_prov_t {
    /* Device UUID uniquely identifying this device,
       used when unprovisioned device send Unprovisioned Device beacon */
    uint8_t dev_uuid[16];
    /* OOB Information, see \ref mesh_prov_oob_info_en */
    mesh_prov_oob_info_en oob_info;
    /* URI(OPTION)
       Along with the Unprovisioned Device beacon, the device may also advertise a
       separate non-connectable advertising packet with a Uniform Resource Identifier (URI) data type
       (as defined in [7]) that points to out-of-band (OOB) information such as a public key.
       To allow the association of the advertised URI with the Unprovisioned Device beacon,
       the beacon may contain an optional 4-octet URI Hash field.*/
    char *uri;
    /* provisioning capabilities, see \ref mesh_prov_capabilities_t */
    mesh_prov_capabilities_t cap;
} mesh_prov_t;

/****** EVENT_MESH_MSG event data *****/
typedef struct _evt_data_mesh_msg_t {
    /* NetKey Index of the subnet to send the message on. */
    uint16_t net_idx;
    /* AppKey Index to encrypt the message with. */
    uint16_t app_idx;
    /* Remote address. */
    uint16_t remote_addr;
    /* Destination address of a received message. Not used for sending. */
    uint16_t recv_dst;
    /* Received TTL value.*/
    uint16_t recv_ttl;
    /* message data length */
    uint16_t msg_len;
    /* message data */
    uint8_t *msg;
} evt_data_mesh_msg_t;

/****** EVENT_MESH_OOB_OUTPUT event data *****/
typedef struct _evt_data_oob_output_t {
    /* action of outputting the numer, \ref mesh_output_oob_action_en */
    mesh_output_oob_action_en action;
    union {
        /* the output number */
        uint32_t num;
        /* the output staring, when action is MESH_DISPLAY_STRING */
        const char *str;
    };
} evt_data_oob_output_t;

/****** EVENT_MESH_OOB_INPUT event data *****/
typedef struct _evt_data_oob_input_t {
    /* action of inputting the numer, \ref mesh_input_oob_action_en */
    mesh_input_oob_action_en action;
    /* maximum size of the input info  */
    uint32_t size;
} evt_data_oob_input_t;

/****** mesh provisioning bearer type *****/
typedef enum {
    /* PB-ADV is a provisioning bearer used to provision a device using
       Generic Provisioning PDUs over the advertising channels.
    */
    MESH_PROV_ADV   = BIT(0),
    /* PB-GATT is a provisioning bearer used to provision a device using
       Proxy PDUs to encapsulate Provisioning PDUs within the Mesh Provisioning Service.
       PB-GATT is provided for support when a Provisioner does not support PB-ADV
       due to limitations of the application interfaces.
    */
    MESH_PROV_GATT  = BIT(1),
} mesh_prov_en;

/****** EVENT_MESH_PROV_START && EVENT_MESH_PROV_END event data *****/
typedef struct _evt_data_prov_bearer_t {
    /* bearer type, \ref mesh_prov_en */
    mesh_prov_en bearer;
} evt_data_prov_bearer_t;

/****** EVENT_MESH_PROV_COMPLETE event data *****/
typedef struct _evt_data_prov_complete_t {
    /* NetKeyIndex given during provisioning */
    uint16_t net_idx;
    /* primary element address given during provisioning */
    uint16_t addr;
} evt_data_prov_complete_t;

typedef struct _evt_data_friendship_chanage_t {
    /* frinendship established (1) or lost (0) */
    uint8_t established;
    /* frinend node address */
    uint16_t friend_addr;
} evt_data_friendship_chanage_t;

/****** mesh stack event *****/
typedef enum {
    /* mesh stack init event */
    EVENT_MESH_STACK_INIT = EVENT_BLE_MESH,

    /* mesh message event, it is generated when mesh stack recive a mesh messgae.
       event_data \ref evt_data_mesh_msg_t */
    EVENT_MESH_MSG,

    /* OOB output event, it is generated when application need display the auth number or string.
       event_data \ref evt_data_oob_output_t */
    EVENT_MESH_OOB_OUTPUT,
    /* OOB input event, it is generated when application need to notify user to input the auth number or string.
       event_data \ref evt_data_oob_input_t */
    EVENT_MESH_OOB_INPUT,

    /* mesh provisioning start event, it is generated when provisioning process start */
    EVENT_MESH_PROV_START,
    /* mesh provisioning start event, it is generated when provisioning process end */
    EVENT_MESH_PROV_END,
    /* mesh provisioning complete event, it is generated when provisioning process successfully
       event_data \ref evt_data_prov_complete_t */
    EVENT_MESH_PROV_COMPLETE,

    /* device node reset event, it is generated when the node is reset.
       and the node need to be reprovisioned.  */
    EVENT_MESH_NODE_RESET,

    /* LPN node friendship chanage with Friend node.  */
    EVENT_MESH_FRIENDSHIP_CHANGE,

    /* end of mesh event */
    EVENT_MESH_UNKNOWN,
} mesh_event_en;


/****** Default ttl value *****/
#define MESH_TTL_DEFAULT 0xff

/****** mesh message callback *****/
typedef void (*mesh_op_cb_func_t)(evt_data_mesh_msg_t *event_data);

/****** mesh model operation *****/
typedef struct _mesh_model_op_t {
    /* opcode, defined with Macros MESH_MODEL_OP* */
    const uint32_t op;
    /* message callback associatied with opcode * */
    const mesh_op_cb_func_t op_cb;
} mesh_model_op_t;

/****** Relay Mode state *****/
typedef enum {
    MESH_RELAY_DISABLED        =      0x00,
    MESH_RELAY_ENABLED     =          0x01,
    MESH_RELAY_NOT_SUPPORTED    =     0x02,
} mesh_relay_state_en;

/****** Secure Network Beacon state *****/
typedef enum {
    MESH_BEACON_DISABLED     =        0x00,
    MESH_BEACON_ENABLED       =       0x01,
} mesh_beacon_state_en;

/****** GATT Proxy state *****/
typedef enum {
    MESH_GATT_PROXY_DISABLED    =     0x00,
    MESH_GATT_PROXY_ENABLED      =    0x01,
    MESH_GATT_PROXY_NOT_SUPPORTED  =  0x02,
} mesh_gatt_proxy_state_en;

/****** Friend state *****/
typedef enum {
    MESH_FRIEND_DISABLED   =          0x00,
    MESH_FRIEND_ENABLED          =    0x01,
    MESH_FRIEND_NOT_SUPPORTED   =     0x02,
} mesh_friend_state_en;

/** @def MESH_NETWORK_TRANSMIT
 *
 *  @brief Encode transmission count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0,
 *                 less than or equal to 320, and a multiple of 10.
 *
 *  @return Mesh transmit value that can be used e.g. for the default
 *          values of the configuration model data.
 */
#define MESH_NETWORK_TRANSMIT(_count, _int_ms) ((_count) | (((_int_ms / 10) - 1) << 3))
#define MESH_RELAY_RETRANSMIT(_count, _int_ms) MESH_NETWORK_TRANSMIT(_count, _int_ms)

/****** mesh configuration server model config *****/
typedef struct _mesh_cfg_srv_t {
    /* Network Transmit state, value defined with macro MESH_NETWORK_TRANSMIT */
    uint8_t net_transmit;
    /* Relay Mode state, see \ref mesh_relay_state_en */
    uint8_t relay;
    /* Relay Retransmit state, value defined with macro MESH_RELAY_RETRANSMIT  */
    uint8_t relay_retransmit;
    /* Secure Network Beacon state, see \ref mesh_beacon_state_en */
    uint8_t beacon;
    /* GATT Proxy state, see \ref mesh_gatt_proxy_state_en */
    uint8_t gatt_proxy;
    /* Friend state, see \ref mesh_friend_state_en */
    uint8_t frnd;
    /* Default TTL */
    uint8_t default_ttl;
} mesh_cfg_srv_t;

/****** mesh model *****/
typedef  struct _mesh_model_t {
    /* Model identifiers */
    union {
        /* SIG Model ID (16-bit) */
        const uint16_t id;
        /* Vendor Model ID (32-bit). */
        struct {
            /* 16-bit Bluetooth-assigned Company Identifier */
            uint16_t company;
            /* 16-bit vendor-assigned model identifier */
            uint16_t id;
        } vnd;
    };
    /* Length of the publication message */
    uint16_t pub_mesh_len;
    /* number of operations */
    uint16_t op_num;
    /* operation list, see \ref mesh_model_op_t */
    mesh_model_op_t *op_list;

    /* user data */
    void *user_data;
} mesh_model_t;

/****** mesh element *****/
typedef struct _mesh_elem_t {
    /* count of SIG Model IDs in this element */
    const uint8_t model_count;
    /* count of Vendor Model IDs in this element */
    const uint8_t vnd_model_count;

    /* a sequence of model_count SIG Models */
    mesh_model_t *const models;
    /* a sequence of vnd_model_count Vendor Models */
    mesh_model_t *const vnd_models;
} mesh_elem_t;

/****** mesh Composition Data Page *****/
typedef struct _mesh_compostion_data_t {
    /* 16-bit company identifier assigned by the Bluetooth SIG */
    uint16_t cid;
    /* 16-bit vendor-assigned product identifier */
    uint16_t pid;
    /* 16-bit vendor-assigned product version identifier */
    uint16_t vid;

    /* count of elements in this node */
    size_t elem_count;
    /* a sequence of elem_count elements */
    mesh_elem_t *elem;
} mesh_compostion_data_t;

/****** mesh event callback *****/
typedef void (*mesh_event_callback_func_t)(mesh_event_en event, void *event_data);

/****** mesh stack initial param *****/
typedef struct _mesh_param_init_t {
    /* device name */
    char *dev_name;
    /* device address, see \ref dev_addr_t */
    dev_addr_t *dev_addr;

    /* mesh provisioning config, see \ref mesh_prov_t */
    mesh_prov_t *prov;
    /* mesh Composition Data Page, see \ref mesh_compostion_data_t */
    mesh_compostion_data_t *comp;
    /* mesh event callback, see \ref mesh_event_callback_func_t */
    mesh_event_callback_func_t event_cb;
} mesh_param_init_t;

/* Primary Network Key index */
#define MESH_NET_PRIMARY                 0x000

#define MESH_NODE_IDENTITY_STOPPED       0x00
#define MESH_NODE_IDENTITY_RUNNING       0x01
#define MESH_NODE_IDENTITY_NOT_SUPPORTED 0x02

#define MESH_ARRAY_NUM(a) (sizeof(a) / sizeof((a)[0]))

/** @def MESH_MODEL_DEFINE
 *
 *  @brief mesh model define macro
 *
 *  @param _id          Model identifiers
 *  @param _op_list     operation list
 *  @param _op_num      number of operations
 *  @param _pub_msg_len Length of the publication message
 *  @param _user_data   user data
 *
 *  @return mesh model, \ref mesh_model_t
 */
#define MESH_MODEL_DEFINE(_id, _op_list, _op_num, _pub_msg_len, _user_data) \
    {                                                                       \
        .id = (_id),                                                        \
              .op_num = (_op_num),                                                \
                        .op_list = (_op_list),                                              \
                                   .pub_mesh_len = (_pub_msg_len),                                     \
                                           .user_data = (_user_data),                                          \
    }

/** @def MESH_MODEL_CFG_SRV
 *
 *  @brief Configuration Server model define macro
 *
 *  @param _net_transmit     Network Transmit state, value defined with macro MESH_NETWORK_TRANSMIT
 *  @param _relay            Relay Mode state, see \ref mesh_relay_state_en
 *  @param _relay_retransmit Relay Retransmit state, value defined with macro MESH_RELAY_RETRANSMIT
 *  @param _beacon           Secure Network Beacon state, see \ref mesh_beacon_state_en
 *  @param _gatt_proxy       GATT Proxy state, see \ref mesh_gatt_proxy_state_en
 *  @param _frnd             Friend state, see \ref mesh_friend_state_en
 *  @param _default_ttl      default ttl
 *
 *  @return mesh model, \ref mesh_model_t
 */
#define MESH_MODEL_CFG_SRV(_net_transmit,                  \
                           _relay,                         \
                           _relay_retransmit,              \
                           _beacon,                        \
                           _gatt_proxy,                    \
                           _frnd,                          \
                           _default_ttl)                   \
MESH_MODEL_DEFINE(MESH_MODEL_ID_CFG_SRV, NULL, 0, 0,       \
(&(mesh_cfg_srv_t){_net_transmit, _relay, \
    _relay_retransmit, _beacon,              \
    _gatt_proxy, _frnd,                      \
    _default_ttl}))

/** @def MESH_MODEL_CFG_CLI
 *
 *  @brief Configuration Client model define macro
 *
 *  @return mesh model, \ref mesh_model_t
 */
#define MESH_MODEL_CFG_CLI()                                                \
    MESH_MODEL_DEFINE(MESH_MODEL_ID_CFG_CLI, NULL, 0,  0, NULL)

/** @def MESH_MODEL_HEALTH_SRV
 *
 *  @brief Health Server model define macro
 *
 *  @param _faults_num  Maximum number of faults the element can have.
 *
 *  @return mesh model, \ref mesh_model_t
 */
#define MESH_MODEL_HEALTH_SRV(_faults_num)                                   \
    MESH_MODEL_DEFINE(MESH_MODEL_ID_HEALTH_SRV, NULL, 0,  _faults_num, NULL)

/** @def MESH_MODEL_HEALTH_CLI
 *
 *  @brief Health Server model define macro
 *
 *  @param _faults_num  Maximum number of faults the element can have.
 *
 *  @return mesh model, \ref mesh_model_t
 */
#define MESH_MODEL_HEALTH_CLI()                                   \
    MESH_MODEL_DEFINE(MESH_MODEL_ID_HEALTH_CLI, NULL, 0,  0, NULL)

/** @def MESH_MODEL_VND_SVR
 *
 *  @brief vander Server model define macro
 *
 *  @param
 *
 *  @return mesh model, \ref mesh_model_t
 */

#define MESH_MODEL_VND_SVR(_company, _id, _op_list, _user_data)      \
{                                                                        \
	.vnd.company = (_company),                                           \
	.vnd.id = (_id),                                                     \
	.op_list = _op_list,                                                           \
                                                    \
	.user_data = _user_data,                                             \
}


/** @def MESH_ELEMENT_DEFINE
 *
 *  @brief mesh element define macro
 *
 *  @param _models_num     number of SIG Model IDs in this element
 *  @param _models         a sequence of _models_num SIG Models
 *  @param _vnd_models_num number of Vendor Model IDs in this element
 *  @param _vnd_models     a sequence of _vnd_models_num Vendor Models
 *
 *  @return mesh element, \ref mesh_model_t
 */
#define MESH_ELEMENT_DEFINE(_models_num, _models, _vnd_models_num, _vnd_models) \
    {                                                                           \
        .model_count = _models_num,                                             \
                       .vnd_model_count = _vnd_models_num,                                     \
                                          .models = _models,                                                      \
                                                  .vnd_models = _vnd_models,                                              \
    }

/**
 * @brief mesh stack initialize
 *
 * @param param  mesh stack initial param, see \ref mesh_param_init_t
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int ble_stack_mesh_init(mesh_param_init_t *param);

/**
 * @brief Unprovisioned Device beacon enable
 *
 * @param bearers  provisioning bearer, see \ref mesh_prov_en
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int ble_stack_mesh_prov_start(mesh_prov_en bearers);

/**
 * @brief Unprovisioned Device beacon disable
 *
 * @param bearers  provisioning bearer, see \ref mesh_prov_en
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int ble_stack_mesh_prov_stop(mesh_prov_en bearers);

/**
 * @brief Send mesh message.
 *
 * @param net_id      NetKey Index of the subnet to send the message on.
 * @param app_id      AppKey Index to encrypt the message with.
 * @param remote_addr Remote address.
 * @param elem_id     element index in the node's elements
 * @param mod_id      model index in the element
 * @param op          mesh opcode
 * @param msg         message data
 * @param msg_len     message data length
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int ble_stack_mesh_send(uint16_t net_id,
                        uint16_t app_id,
                        uint16_t remote_addr,
                        uint8_t ttl,
                        uint8_t elem_id,
                        uint8_t mod_id,
                        uint32_t op,
                        uint8_t *msg,
                        uint16_t msg_len);

/**
 * @brief Publish mesh message.
 *
 * @param elem_id     element index in the node's elements
 * @param mod_id      model index in the element
 * @param op          mesh opcode
 * @param msg         message data
 * @param msg_len     message data length
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int ble_stack_mesh_pub(uint8_t elem_id,
                       uint8_t mod_id,
                       uint32_t op,
                       uint8_t *msg,
                       uint16_t msg_len);

/**
 * @brief input oob number.
 *
 * @param num   auth number
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int ble_stack_mesh_input_num(uint32_t num);

/**
 * @brief input oob string.
 *
 * @param str   auth string
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int ble_stack_mesh_input_string(char *str);

int ble_stack_mesh_cfg_beacon_get(uint16_t net_idx, uint16_t addr, uint8_t *status);
int ble_stack_mesh_cfg_beacon_set(uint16_t net_idx, uint16_t addr, uint8_t val, uint8_t *status);

int ble_stack_mesh_reset(void);

int ble_stack_mesh_lpn_set(int enable);
int ble_stack_mesh_lpn_poll(void);

int ble_stack_mesh_iv_update_test(int enable);
int ble_stack_mesh_iv_update(void);

#ifdef __cplusplus
}
#endif

#endif   /* __MESH_H__ */

