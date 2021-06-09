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

/** @file
 *  @brief Bluetooth Mesh Time and Scene Client Model APIs.
 */

#ifndef _BLE_MESH_BLE_MESH_TIME_SCENE_MODEL_API_H_
#define _BLE_MESH_BLE_MESH_TIME_SCENE_MODEL_API_H_

#include "time_scene_client.h"
#include "yoc_ble_mesh_defs.h"

/** @def    BLE_MESH_BLE_MESH_MODEL_TIME_CLI
 *
 *  @brief  Define a new Time Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Time Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct ble_mesh_client_t.
 *
 *  @return New Time Client Model instance.
 */
#define BLE_MESH_BLE_MESH_MODEL_TIME_CLI(cli_pub, cli_data)         \
        BLE_MESH_BLE_MESH_SIG_MODEL(BLE_MESH_BLE_MESH_MODEL_ID_TIME_CLI, \
                    NULL, cli_pub, cli_data)

/** @def    BLE_MESH_BLE_MESH_MODEL_SCENE_CLI
 *
 *  @brief  Define a new Scene Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Scene Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct ble_mesh_client_t.
 *
 *  @return New Scene Client Model instance.
 */
#define BLE_MESH_BLE_MESH_MODEL_SCENE_CLI(cli_pub, cli_data)         \
        BLE_MESH_BLE_MESH_SIG_MODEL(BLE_MESH_BLE_MESH_MODEL_ID_SCENE_CLI, \
                    NULL, cli_pub, cli_data)

/** @def    BLE_MESH_BLE_MESH_MODEL_SCHEDULER_CLI
 *
 *  @brief  Define a new Scheduler Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Scheduler Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct ble_mesh_client_t.
 *
 *  @return New Scheduler Client Model instance.
 */
#define BLE_MESH_BLE_MESH_MODEL_SCHEDULER_CLI(cli_pub, cli_data)         \
        BLE_MESH_BLE_MESH_SIG_MODEL(BLE_MESH_BLE_MESH_MODEL_ID_SCHEDULER_CLI, \
                    NULL, cli_pub, cli_data)

/**
 *  @brief Bluetooth Mesh Time Scene Client Model Get and Set parameters structure.
 */

/** Parameters of Time Set */
typedef struct {
    u8_t  tai_seconds[5];           /*!< The current TAI time in seconds */
    u8_t  sub_second;               /*!< The sub-second time in units of 1/256 second */
    u8_t  uncertainty;              /*!< The estimated uncertainty in 10-millisecond steps */
    u16_t time_authority : 1;       /*!< 0 = No Time Authority, 1 = Time Authority */
    u16_t tai_utc_delta : 15;       /*!< Current difference between TAI and UTC in seconds */
    u8_t  time_zone_offset;         /*!< The local time zone offset in 15-minute increments */
} ble_mesh_time_set_t;

/** Parameters of Time Zone Set */
typedef struct {
    u8_t time_zone_offset_new;      /*!< Upcoming local time zone offset */
    u8_t tai_zone_change[5];        /*!< TAI Seconds time of the upcoming Time Zone Offset change */
} ble_mesh_time_zone_set_t;

/** Parameters of TAI-UTC Delta Set */
typedef struct {
    u16_t tai_utc_delta_new : 15;   /*!< Upcoming difference between TAI and UTC in seconds */
    u16_t padding : 1;              /*!< Always 0b0. Other values are Prohibited. */
    u8_t tai_delta_change[5];       /*!< TAI Seconds time of the upcoming TAI-UTC Delta change */
} ble_mesh_tai_utc_delta_set_t;

/** Parameter of Time Role Set */
typedef struct {
    u8_t time_role;                 /*!< The Time Role for the element */
} ble_mesh_time_role_set_t;

/** Parameter of Scene Store */
typedef struct {
    u16_t scene_number;             /*!< The number of scenes to be stored */
} ble_mesh_scene_store_t;

/** Parameters of Scene Recall */
typedef struct {
    bool  op_en;                    /*!< Indicate if optional parameters are included */
    u16_t scene_number;             /*!< The number of scenes to be recalled */
    u8_t  tid;                      /*!< Transaction ID */
    u8_t  trans_time;               /*!< Time to complete state transition (optional) */
    u8_t  delay;                    /*!< Indicate message execution delay (C.1) */
} ble_mesh_scene_recall_t;

/** Parameter of Scene Delete */
typedef struct {
    u16_t scene_number;             /*!< The number of scenes to be deleted */
} ble_mesh_scene_delete_t;

/** Parameter of Scheduler Action Get */
typedef struct {
    u8_t index;                     /*!< Index of the Schedule Register entry to get */
} ble_mesh_scheduler_act_get_t;

/** Parameters of Scheduler Action Set */
typedef struct {
    u64_t index : 4;                /*!< Index of the Schedule Register entry to set */
    u64_t year : 7;                 /*!< Scheduled year for the action */
    u64_t month : 12;               /*!< Scheduled month for the action */
    u64_t day : 5;                  /*!< Scheduled day of the month for the action */
    u64_t hour : 5;                 /*!< Scheduled hour for the action */
    u64_t minute : 6;               /*!< Scheduled minute for the action */
    u64_t second : 6;               /*!< Scheduled second for the action */
    u64_t day_of_week : 7;          /*!< Schedule days of the week for the action */
    u64_t action : 4;               /*!< Action to be performed at the scheduled time */
    u64_t trans_time : 8;           /*!< Transition time for this action */
    u16_t scene_number;             /*!< Transition time for this action */
} ble_mesh_scheduler_act_set_t;

/**
 * @brief Time Scene Client Model get message union
 */
typedef union {
    ble_mesh_scheduler_act_get_t scheduler_act_get; /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET */
} ble_mesh_time_scene_client_get_state_t;

/**
 * @brief Time Scene Client Model set message union
 */
typedef union {
    ble_mesh_time_set_t          time_set;          /*!< For BLE_MESH_BLE_MESH_MODEL_OP_TIME_SET */
    ble_mesh_time_zone_set_t     time_zone_set;     /*!< For BLE_MESH_BLE_MESH_MODEL_OP_TIME_ZONE_SET */
    ble_mesh_tai_utc_delta_set_t tai_utc_delta_set; /*!< For BLE_MESH_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET */
    ble_mesh_time_role_set_t     time_role_set;     /*!< For BLE_MESH_BLE_MESH_MODEL_OP_TIME_ROLE_SET */
    ble_mesh_scene_store_t       scene_store;       /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCENE_STORE & BLE_MESH_BLE_MESH_MODEL_OP_SCENE_STORE_UNACK */
    ble_mesh_scene_recall_t      scene_recall;      /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCENE_RECALL & BLE_MESH_BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK */
    ble_mesh_scene_delete_t      scene_delete;      /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCENE_DELETE & BLE_MESH_BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK */
    ble_mesh_scheduler_act_set_t scheduler_act_set; /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET & BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK */
} ble_mesh_time_scene_client_set_state_t;

/**
 *  @brief Bluetooth Mesh Time Scene Client Model Get and Set callback parameters structure.
 */

/** Parameters of Time Status */
typedef struct {
    u8_t  tai_seconds[5];           /*!< The current TAI time in seconds */
    u8_t  sub_second;               /*!< The sub-second time in units of 1/256 second */
    u8_t  uncertainty;              /*!< The estimated uncertainty in 10-millisecond steps */
    u16_t time_authority : 1;       /*!< 0 = No Time Authority, 1 = Time Authority */
    u16_t tai_utc_delta : 15;       /*!< Current difference between TAI and UTC in seconds */
    u8_t  time_zone_offset;         /*!< The local time zone offset in 15-minute increments */
} ble_mesh_time_status_cb_t;

/** Parameters of Time Zone Status */
typedef struct {
    u8_t time_zone_offset_curr;     /*!< Current local time zone offset */
    u8_t time_zone_offset_new;      /*!< Upcoming local time zone offset */
    u8_t tai_zone_change[5];        /*!< TAI Seconds time of the upcoming Time Zone Offset change */
} ble_mesh_time_zone_status_cb_t;

/** Parameters of TAI-UTC Delta Status */
typedef struct {
    u16_t tai_utc_delta_curr : 15;  /*!< Current difference between TAI and UTC in seconds */
    u16_t padding_1 : 1;            /*!< Always 0b0. Other values are Prohibited. */
    u16_t tai_utc_delta_new : 15;   /*!< Upcoming difference between TAI and UTC in seconds */
    u16_t padding_2 : 1;            /*!< Always 0b0. Other values are Prohibited. */
    u8_t tai_delta_change[5];       /*!< TAI Seconds time of the upcoming TAI-UTC Delta change */
} ble_mesh_tai_utc_delta_status_cb_t;

/** Parameter of Time Role Status */
typedef struct {
    u8_t time_role;                 /*!< The Time Role for the element */
} ble_mesh_time_role_status_cb_t;

/** Parameters of Scene Status */
typedef struct {
    bool  op_en;                    /*!< Indicate if optional parameters are included */
    u8_t  status_code;              /*!< Status code of the last operation */
    u16_t current_scene;            /*!< Scene Number of the current scene */
    u16_t target_scene;             /*!< Scene Number of the target scene (optional) */
    u8_t  remain_time;              /*!< Time to complete state transition (C.1) */
} ble_mesh_scene_status_cb_t;

/** Parameters of Scene Register Status */
typedef struct {
    u8_t  status_code;              /*!< Status code for the previous operation */
    u16_t current_scene;            /*!< Scene Number of the current scene */
    struct net_buf_simple *scenes;  /*!< A list of scenes stored within an element */
} ble_mesh_scene_register_status_cb_t;

/** Parameter of Scheduler Status */
typedef struct {
    u16_t schedules;                /*!< Bit field indicating defined Actions in the Schedule Register */
} ble_mesh_scheduler_status_cb_t;

/** Parameters of Scheduler Action Status */
typedef struct {
    u64_t index : 4;                /*!< Enumerates (selects) a Schedule Register entry */
    u64_t year : 7;                 /*!< Scheduled year for the action */
    u64_t month : 12;               /*!< Scheduled month for the action */
    u64_t day : 5;                  /*!< Scheduled day of the month for the action */
    u64_t hour : 5;                 /*!< Scheduled hour for the action */
    u64_t minute : 6;               /*!< Scheduled minute for the action */
    u64_t second : 6;               /*!< Scheduled second for the action */
    u64_t day_of_week : 7;          /*!< Schedule days of the week for the action */
    u64_t action : 4;               /*!< Action to be performed at the scheduled time */
    u64_t trans_time : 8;           /*!< Transition time for this action */
    u16_t scene_number;             /*!< Transition time for this action */
} ble_mesh_scheduler_act_status_cb_t;

/**
 * @brief Time Scene Client Model received message union
 */
typedef union {
    ble_mesh_time_status_cb_t           time_status;            /*!< For BLE_MESH_BLE_MESH_MODEL_OP_TIME_STATUS */
    ble_mesh_time_zone_status_cb_t      time_zone_status;       /*!< For BLE_MESH_BLE_MESH_MODEL_OP_TIME_ZONE_STATUS */
    ble_mesh_tai_utc_delta_status_cb_t  tai_utc_delta_status;   /*!< For BLE_MESH_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS */
    ble_mesh_time_role_status_cb_t      time_role_status;       /*!< For BLE_MESH_BLE_MESH_MODEL_OP_TIME_ROLE_STATUS */
    ble_mesh_scene_status_cb_t          scene_status;           /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCENE_STATUS */
    ble_mesh_scene_register_status_cb_t scene_register_status;  /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS */
    ble_mesh_scheduler_status_cb_t      scheduler_status;       /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_STATUS */
    ble_mesh_scheduler_act_status_cb_t  scheduler_act_status;   /*!< For BLE_MESH_BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS */
} ble_mesh_time_scene_client_status_cb_t;

/** Time Scene Client Model callback parameters */
typedef struct {
    int error_code;                                         /*!< Appropriate error code */
    ble_mesh_client_common_param_t        *params;      /*!< The client common parameters. */
    ble_mesh_time_scene_client_status_cb_t status_cb;   /*!< The scene status message callback values */
} ble_mesh_time_scene_client_cb_param_t;

/** This enum value is the event of Time Scene Client Model */
typedef enum {
    BLE_MESH_BLE_MESH_TIME_SCENE_CLIENT_GET_STATE_EVT,
    BLE_MESH_BLE_MESH_TIME_SCENE_CLIENT_SET_STATE_EVT,
    BLE_MESH_BLE_MESH_TIME_SCENE_CLIENT_PUBLISH_EVT,
    BLE_MESH_BLE_MESH_TIME_SCENE_CLIENT_TIMEOUT_EVT,
    BLE_MESH_BLE_MESH_TIME_SCENE_CLIENT_EVT_MAX,
} ble_mesh_time_scene_client_cb_event_t;

/**
 *  @brief Bluetooth Mesh Time Scene Client Model function.
 */

/**
 * @brief   Time Scene Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* ble_mesh_time_scene_client_cb_t)(ble_mesh_time_scene_client_cb_event_t event,
        ble_mesh_time_scene_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Time Scene Client Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      BT_OK on success or error code otherwise.
 *
 */
bt_err_t ble_mesh_register_time_scene_client_callback(ble_mesh_time_scene_client_cb_t callback);

/**
 * @brief       Get the value of Time Scene Server Model states using the Time Scene Client Model get messages.
 *
 * @note        If you want to know the opcodes and corresponding meanings accepted by this API,
 *              please refer to ble_mesh_time_scene_message_opcode_t in yoc_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   get_state: Pointer to time scene get message value.
 *                         Shall not be set to NULL.
 *
 * @return      BT_OK on success or error code otherwise.
 */
bt_err_t ble_mesh_time_scene_client_get_state(ble_mesh_client_common_param_t *params,
        ble_mesh_time_scene_client_get_state_t *get_state);

/**
 * @brief       Set the value of Time Scene Server Model states using the Time Scene Client Model set messages.
 *
 * @note        If you want to know the opcodes and corresponding meanings accepted by this API,
 *              please refer to ble_mesh_time_scene_message_opcode_t in yoc_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   set_state: Pointer to time scene set message value.
 *                         Shall not be set to NULL.
 *
 * @return      BT_OK on success or error code otherwise.
 */
bt_err_t ble_mesh_time_scene_client_set_state(ble_mesh_client_common_param_t *params,
        ble_mesh_time_scene_client_set_state_t *set_state);

#endif /* _BLE_MESH_BLE_MESH_TIME_SCENE_MODEL_API_H_ */

