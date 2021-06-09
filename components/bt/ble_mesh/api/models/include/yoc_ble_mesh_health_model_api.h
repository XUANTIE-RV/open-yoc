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

#ifndef _BLE_MESH_BLE_MESH_HEALTH_MODEL_API_H_
#define _BLE_MESH_BLE_MESH_HEALTH_MODEL_API_H_

#include "yoc_ble_mesh_defs.h"

/** @def    BLE_MESH_BLE_MESH_MODEL_HEALTH_SRV
 *
 *  @brief  Define a new Health Server Model.
 *
 *  @note   The Health Server Model can only be included by a Primary Element.
 *
 *  @param  srv Pointer to the unique struct ble_mesh_health_srv_t.
 *  @param  pub Pointer to the unique struct ble_mesh_model_pub_t.
 *
 *  @return New Health Server Model instance.
 */
#define BLE_MESH_BLE_MESH_MODEL_HEALTH_SRV(srv, pub)                           \
        BLE_MESH_BLE_MESH_SIG_MODEL(BLE_MESH_BLE_MESH_MODEL_ID_HEALTH_SRV,          \
                           NULL, pub, srv)

/** @def    BLE_MESH_BLE_MESH_MODEL_HEALTH_CLI
 *
 *  @brief  Define a new Health Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Health Client Model.
 *
 *  @param  cli_data Pointer to the unique struct ble_mesh_client_t.
 *
 *  @return New Health Client Model instance.
 */
#define BLE_MESH_BLE_MESH_MODEL_HEALTH_CLI(cli_data)                           \
        BLE_MESH_BLE_MESH_SIG_MODEL(BLE_MESH_BLE_MESH_MODEL_ID_HEALTH_CLI,          \
                           NULL, NULL, cli_data)

/** Health Server Model callbacks */
typedef struct {
    /** Fetch current faults */
    int (*fault_get_cur)(ble_mesh_model_t *model, uint8_t *test_id,
                         uint16_t *company_id, uint8_t *faults, uint8_t *fault_count);

    /** Fetch registered faults */
    int (*fault_get_reg)(ble_mesh_model_t *model, uint16_t company_id,
                         uint8_t *test_id, uint8_t *faults, uint8_t *fault_count);

    /** Clear registered faults */
    int (*fault_clear)(ble_mesh_model_t *model, uint16_t company_id);

    /** Run a specific test */
    int (*fault_test)(ble_mesh_model_t *model, uint8_t test_id, uint16_t company_id);

    /** Attention on */
    void (*attn_on)(ble_mesh_model_t *model);

    /** Attention off */
    void (*attn_off)(ble_mesh_model_t *model);
} ble_mesh_health_srv_cb_t;

/** Health Server Model Context */
typedef struct {
    /** Pointer to Health Server Model */
    ble_mesh_model_t *model;

    /** Optional callback struct */
    const ble_mesh_health_srv_cb_t *cb;

    /** Attention Timer state */
    struct k_delayed_work attn_timer;
} ble_mesh_health_srv_t;

/** Parameter of Health Fault Get */
typedef struct {
    uint16_t company_id;    /*!< Bluetooth assigned 16-bit Company ID */
} ble_mesh_health_fault_get_t;

/** Parameter of Health Attention Set */
typedef struct {
    uint8_t attention;      /*!< Value of the Attention Timer state */
} ble_mesh_health_attention_set_t;

/** Parameter of Health Period Set */
typedef struct {
    uint8_t fast_period_divisor;    /*!< Divider for the Publish Period */
} ble_mesh_health_period_set_t;

/** Parameter of Health Fault Test */
typedef struct {
    uint16_t company_id;    /*!< Bluetooth assigned 16-bit Company ID */
    uint8_t  test_id;       /*!< ID of a specific test to be performed */
} ble_mesh_health_fault_test_t;

/** Parameter of Health Fault Clear */
typedef struct {
    uint16_t company_id;    /*!< Bluetooth assigned 16-bit Company ID */
} ble_mesh_health_fault_clear_t;

/**
 * @brief For BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET
 *            BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_GET
 *            BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_GET
 * the get_state parameter in the ble_mesh_health_client_get_state function should not be set to NULL.
 */
typedef union {
    ble_mesh_health_fault_get_t fault_get;          /*!< For BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET. */
} ble_mesh_health_client_get_state_t;

/**
 * @brief For BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR
 *            BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK
 *            BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST
 *            BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK
 *            BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET
 *            BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK
 *            BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_SET
 *            BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK
 * the set_state parameter in the ble_mesh_health_client_set_state function should not be set to NULL.
 */
typedef union {
    ble_mesh_health_attention_set_t attention_set;    /*!< For BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_SET or BLE_MESH_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK. */
    ble_mesh_health_period_set_t    period_set;       /*!< For BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET or BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK. */
    ble_mesh_health_fault_test_t    fault_test;       /*!< For BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST or BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK. */
    ble_mesh_health_fault_clear_t   fault_clear;      /*!< For BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR or BLE_MESH_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK. */
} ble_mesh_health_client_set_state_t;

/** Parameters of Health Current Status */
typedef struct {
    uint8_t  test_id;                       /*!< ID of a most recently performed test */
    uint16_t company_id;                    /*!< Bluetooth assigned 16-bit Company ID */
    struct net_buf_simple *fault_array;     /*!< FaultArray field contains a sequence of 1-octet fault values */
} ble_mesh_health_current_status_cb_t;

/** Parameters of Health Fault Status */
typedef struct {
    uint8_t  test_id;                       /*!< ID of a most recently performed test */
    uint16_t company_id;                    /*!< Bluetooth assigned 16-bit Company ID */
    struct net_buf_simple *fault_array;     /*!< FaultArray field contains a sequence of 1-octet fault values */
} ble_mesh_health_fault_status_cb_t;

/** Parameter of Health Period Status */
typedef struct {
    uint8_t fast_period_divisor;            /*!< Divider for the Publish Period */
} ble_mesh_health_period_status_cb_t;

/** Parameter of Health Attention Status */
typedef struct {
    uint8_t attention;                      /*!< Value of the Attention Timer state */
} ble_mesh_health_attention_status_cb_t;

/**
 * @brief Health Client Model received message union
 */
typedef union {
    ble_mesh_health_current_status_cb_t   current_status;       /*!< The health current status value */
    ble_mesh_health_fault_status_cb_t     fault_status;         /*!< The health fault status value */
    ble_mesh_health_period_status_cb_t    period_status;        /*!< The health period status value */
    ble_mesh_health_attention_status_cb_t attention_status;     /*!< The health attention status value */
} ble_mesh_health_client_common_cb_param_t;

/** Health Client Model callback parameters */
typedef struct {
    int error_code;                                         /*!< Appropriate error code */
    ble_mesh_client_common_param_t          *params;    /*!< The client common parameters. */
    ble_mesh_health_client_common_cb_param_t status_cb; /*!< The health message status callback values */
} ble_mesh_health_client_cb_param_t;

/** This enum value is the event of Health Client Model */
typedef enum {
    BLE_MESH_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT,
    BLE_MESH_BLE_MESH_HEALTH_CLIENT_SET_STATE_EVT,
    BLE_MESH_BLE_MESH_HEALTH_CLIENT_PUBLISH_EVT,
    BLE_MESH_BLE_MESH_HEALTH_CLIENT_TIMEOUT_EVT,
    BLE_MESH_BLE_MESH_HEALTH_CLIENT_EVT_MAX,
} ble_mesh_health_client_cb_event_t;

/** Health Server Model callback parameter */
typedef struct {
    int error_code;                                       /*!< Appropriate error code */
} ble_mesh_health_server_cb_param_t;

/** This enum value is the event of Health Server Model */
typedef enum {
    BLE_MESH_BLE_MESH_HEALTH_SERVER_FAULT_UPDATE_COMPLETE_EVT,
    BLE_MESH_BLE_MESH_HEALTH_SERVER_EVT_MAX,
} ble_mesh_health_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Health Client and Server Model function.
 */

/**
 * @brief   Health Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* ble_mesh_health_client_cb_t)(ble_mesh_health_client_cb_event_t event,
        ble_mesh_health_client_cb_param_t *param);

/**
 * @brief   Health Server Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* ble_mesh_health_server_cb_t)(ble_mesh_health_server_cb_event_t event,
        ble_mesh_health_server_cb_param_t *param);

/**
 * @brief         Register BLE Mesh Health Model callback, the callback will report Health Client & Server Model events.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        BT_OK on success or error code otherwise.
 *
 */
bt_err_t ble_mesh_register_health_client_callback(ble_mesh_health_client_cb_t callback);

/**
 * @brief         Register BLE Mesh Health Server Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        BT_OK on success or error code otherwise.
 *
 */
bt_err_t ble_mesh_register_health_server_callback(ble_mesh_health_server_cb_t callback);

/**
 * @brief         This function is called to get the Health Server states using the Health Client Model get messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to ble_mesh_opcode_health_client_get_t in yoc_ble_mesh_defs.h
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     get_state: Pointer to a union, each kind of opcode corresponds to one structure inside.
 *                           Shall not be set to NULL.
 *
 * @return        BT_OK on success or error code otherwise.
 *
 */
bt_err_t ble_mesh_health_client_get_state(ble_mesh_client_common_param_t *params,
        ble_mesh_health_client_get_state_t *get_state);

/**
 * @brief         This function is called to set the Health Server states using the Health Client Model set messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to ble_mesh_opcode_health_client_set_t in yoc_ble_mesh_defs.h
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     set_state: Pointer to a union, each kind of opcode corresponds to one structure inside.
 *                           Shall not be set to NULL.
 *
 * @return        BT_OK on success or error code otherwise.
 *
 */
bt_err_t ble_mesh_health_client_set_state(ble_mesh_client_common_param_t *params,
        ble_mesh_health_client_set_state_t *set_state);

/**
 * @brief         This function is called by the Health Server Model to start to publish its Current Health Fault.
 *
 * @param[in]     element: The element to which the Health Server Model belongs.
 *
 * @return        BT_OK on success or error code otherwise.
 *
 */
bt_err_t ble_mesh_health_server_fault_update(ble_mesh_elem_t *element);

#endif /** _BLE_MESH_BLE_MESH_HEALTH_MODEL_API_H_ */
