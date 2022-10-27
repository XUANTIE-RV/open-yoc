/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __MESH_NODE
#define __MESH_NODE

#include <stdio.h>
#include <mesh_model/mesh_model.h>
#include "mesh_provisioner.h"

#define DEVICE_NAME_MAX_LENGTH             28
#define RSSI_BEACON_REPORT_PERIOD          1000 // ms
#define DEF_ADD_APPKEY_DELAY               (300)
#define DEF_ADD_APPKEY_TIMEOUT             (5 * 1000)
#define DEF_AUTOCONFIG_STORE_CHECK_TIMEOUT (1 * 1000)
#define DEF_AUTOCONFIG_STORE_CHECK_RETRY   40
#define DEF_ADD_APPKEY_RETRY               10
#define DEF_PROV_AUTOCONFIG_TIMEOUT        1000 // ms
#define DEF_UNICAST_ADDR_AUTO_CONFIG_RETRY (5)  // s
#define DEF_GROUP_ADDR_AUTO_CONFIG_RETRY   (20) // s
#define CONFIG_MAX_SUBLIST_OVERWRITE_SIZE  10
#define HB_PUB_PERIOD_NOT_SET              0xFF
#define DEF_GET_VERSION_RETRY_TIMEOUT      (1000)
#define TTL_NOT_SET                        0xFF
#define NODE_OTA_NOT_SUPPORT               0x00
#define DEF_VERSION_REPORT_TTL             0x07

#ifndef CONFIG_BT_MESH_AUTOCONFIG_DST_SUB_ADDR
#define CONFIG_BT_MESH_AUTOCONFIG_DST_SUB_ADDR 0xF000
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_DST_PUB_ADDR
#define CONFIG_BT_MESH_AUTOCONFIG_DST_PUB_ADDR 0xF001
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_LOCAL_SUB_ADDR
#define CONFIG_BT_MESH_AUTOCONFIG_LOCAL_SUB_ADDR CONFIG_BT_MESH_AUTOCONFIG_DST_PUB_ADDR
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_LOCAL_PUB_ADDR
#define CONFIG_BT_MESH_AUTOCONFIG_LOCAL_PUB_ADDR CONFIG_BT_MESH_AUTOCONFIG_DST_SUB_ADDR
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_NET_IDX
#define CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_NET_IDX 0x0
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_STATUS_REPORT_ADDR
#define CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_STATUS_REPORT_ADDR CONFIG_BT_MESH_AUTOCONFIG_DST_PUB_ADDR
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_DST_ADDR
#define CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_DST_ADDR 0xF003
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_COUNT_LOG
#define CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_COUNT_LOG 0xff
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_PERIOD_LOG
#define CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_PERIOD_LOG 0x3 // 4s
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_TTL
#define CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_TTL 0x4
#endif

#ifndef CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_FEAD
#define CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_FEAD 0x0F // RELAY/PROXY/FRIEND/LPN
#endif

#ifndef CONFIG_BT_MESH_CUR_FAULTS_MAX
#define CONFIG_BT_MESH_CUR_FAULTS_MAX 4
#endif

#ifndef CONFIG_BT_MESH_CUR_TEST_ID_MAX
#define CONFIG_BT_MESH_CUR_TEST_ID_MAX 4
#endif

typedef struct {
    uint16_t prim_unicast;
    uint8_t  element_num;
} mesh_node_local_t;

typedef enum
{
    BT_MESH_EVENT_NODE_REST,
    BT_MESH_EVENT_NODE_PROV_COMP,
    BT_MESH_EVENT_NODE_OOB_INPUT_NUM,
    BT_MESH_EVENT_NODE_OOB_INPUT_STRING,
    BT_MESH_EVENT_NODE_AUTOCONFIG_SUCCESS,
} mesh_prov_event_en;

typedef enum
{
    BT_MESH_AUTO_CONFIG_SUCCEED,
    BT_MESH_AUTO_CONFIG_NOT_START,
    BT_MESH_AUTO_CONFIG_START,
    BT_MESH_AUTO_CONFIG_FAILED,
} meah_auto_config_status_en;

typedef enum
{
    BT_MESH_NODE_NO_LPM,
    BT_MESH_NODE_LPM_TX_RX,
    BT_MESH_NODE_LPM_RX_TX,
    BT_MESH_NODE_LPM_NO_RX,
    BT_MESH_NODE_LPM_NO_TX,
    BT_MESH_NODE_LPM_NO_RX_TX,
} mesh_node_lpm_type_en;

typedef enum
{
    NODE_OTA_AIS_BY_GATT     = 0x00,
    NODE_OTA_AIS_BY_UART     = 0x01,
    NODE_OTA_AIS_BY_HCI_UART = 0x02,
    NODE_OTA_MESH_BY_EXT_1M  = 0x03,
    NODE_OTA_MESH_BY_EXT_2M  = 0x04,
    NODE_OTA_MESH_BY_LEGACY  = 0x05,
} mesh_node_ota_type_en;

typedef enum
{
    BT_MESH_STATUS_SUCCEED,
    BT_MESH_STATUS_ERR,
} status_failed_en;

enum
{
    NODE_HB_NOT_ENABLE = 0x00,
    NODE_HB_ENABLE     = 0x01,
};

enum
{
    NODE_HB_SUB_PUB_MISMATCH = 0x00,
    NODE_HB_SUB_PUB_MATCH    = 0x01,
};

enum
{
    NODE_HB_TTL_INVALID = 0x00,
    NODE_HB_TTL_VAILD   = 0x01,
};

enum
{
    NODE_HB_COUNT_INVALID = 0x00,
    NODE_HB_COUNT_VALID   = 0x01,
};

enum
{
    NODE_HB_PERIOD_NO_CHECK = 0x00,
    NODE_HB_PERIOD_MISMATCH = 0x01,
    NODE_HB_PERIOD_MATCH    = 0x02,
};

typedef void (*prov_event_cb)(mesh_prov_event_en event, void *p_arg);

typedef void (*attn_cb)(void);

typedef int (*fault_test)(u8_t test_id, u16_t company_id);

typedef struct {
    uint8_t test_id;
    uint8_t faults_num;
    uint8_t faults[CONFIG_BT_MESH_CUR_FAULTS_MAX];
} health_faults_t;

typedef struct {
    fault_test faults_test;
    attn_cb    att_on;
    attn_cb    att_off;
} health_srv_cb;

typedef enum
{
    ACTION_NULL,
    ACTION_NUM,
    ACTION_STR,
} oob_action_en;

typedef enum
{
    PROVISIONER = 0x01,
    NODE        = 0x02,
} node_role_en;

typedef struct {
    uint8_t *     static_oob_data;
    oob_action_en input_action;
    uint8_t       input_max_size;
    oob_action_en output_action;
    uint8_t       output_max_size;
} oob_method;

typedef struct {
    uint8_t        dev_uuid[16];
    uint8_t        dev_name[DEVICE_NAME_MAX_LENGTH];
    model_event_cb user_model_cb;
    prov_event_cb  user_prov_cb;
#ifdef BT_MESH_BEACON_RSSI_REPORT
    uint8_t rssi_report_enable;
#endif
    health_srv_cb *health_cb;
    oob_method     node_oob;
} node_config_t;

#ifdef CONFIG_BT_MESH_PROVISIONER
typedef struct {
    uint16_t unicast_start_addr;
    uint16_t unicast_end_addr;
    uint8_t  attention_time;
} provisioner_node_config_t;

#endif

typedef struct {
    uint16_t sub_addr;
} model_auto_config_t;

typedef struct {
    prov_event_cb  _prov_cb;
    model_event_cb _model_cb;
#ifdef CONFIG_BT_MESH_PROVISIONER
    provisioner_cb _provisioner_cb;
#endif
} node_at_cb;

typedef struct __attribute__((__packed__)) {
    uint8_t hb_enable          : 1;
    uint8_t hb_sub_pub_check   : 1;
    uint8_t hb_ttl_check       : 1;
    uint8_t hb_pub_count_check : 1;
    uint8_t hb_period_check    : 2;
    uint8_t reseed             : 2;
} hb_check_status;

typedef struct {
    hb_check_status check_status;
    uint8_t         period_log;
} hb_set_status;

int ble_mesh_node_init(node_config_t *param);
int ble_mesh_node_reset();
int ble_mesh_node_OOB_input_num(uint32_t num);
int ble_mesh_node_OOB_input_string(const char *str);
int ble_mesh_vendor_srv_model_version_report(uint16_t dst_addr, uint8_t ack_ttl);

#ifdef CONFIG_BT_MESH_PROVISIONER
int  ble_mesh_node_provisioner_init(provisioner_cb cb);
int  ble_mesh_node_provisioner_config(provisioner_node_config_t *config);
int  ble_mesh_node_provisioner_get_config(provisioner_node_config_t *config);
int  ble_mesh_node_provisioner_autoprov();
int  ble_mesh_node_provisioner_enable();
int  ble_mesh_node_provisioner_disable();
bool ble_mesh_node_provisioner_is_enabled();
int  ble_mesh_node_appkey_add(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr);
int  ble_mesh_node_model_autoconfig(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr,
                                    model_auto_config_t auto_conf);
int  ble_mesh_node_get_node_app_version(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t addr);
int  ble_mesh_node_active_check(uint16_t net_idx, uint16_t appkey_idx, uint16_t addr, uint8_t period_log);
int  ble_mesh_node_at_cb_register(node_at_cb cb);
int  ble_mesh_node_at_cb_unregister();
int  ble_mesh_node_overwrite_node_sublist(uint16_t net_idx, uint16_t appkey_idx, uint16_t addr, uint16_t mod_id,
                                          uint8_t sublist_size, uint16_t *subaddr, uint16_t CID);

#endif

#endif
