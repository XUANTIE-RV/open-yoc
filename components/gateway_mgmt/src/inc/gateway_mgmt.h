/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef __GATEWAY_MGMT_H__
#define __GATEWAY_MGMT_H__

#include "gateway.h"
#include "gateway_occ_auth.h"

#define GW_SUB_DEV_QUEUE_MAX_NUM     10
#define GW_ADD_SUBDEV_MAX_RETRY_TIME 5

#ifndef CONFIG_GW_MAX_DEVICES_META_NUM
#define CONFIG_GW_MAX_DEVICES_META_NUM (32)
#endif

#ifndef GW_MAX_GW_GROUP_NUM
#define GW_MAX_GW_GROUP_NUM (10)
#endif

#ifndef GW_MAX_SUBDEV_JOINED_GRP_NUM
#define GW_MAX_SUBDEV_JOINED_GRP_NUM (3)
#endif

#define GW_MAGIC_CODE 0x736A

#define GW_SUBGRP_INVAL -1
#define GW_SUBGRP_VALID 1

#define GW_GET_OCC_JS_DELAYED_TIME 5000

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
#define GW_DEF_OCC_AUTH_NODE_TIMEOUT (10000) // 10s
#endif

typedef int atomic_t;

typedef enum
{
    GW_MSG_ADD_SUB_DEV,
    GW_MSG_ADD_SUB_DEV_FAIL,
    GW_MSG_DEL_SUB_DEV,
    GW_MSG_DEV_HB_MGMT_SET,
    GW_MSG_SCAN_DEV,
    GW_MSG_SCAN_DEV_TIMEOUT,
    GW_MSG_AUTH_INPUT,
    GW_MSG_MAX
} enum_mgmt_msg_type_e;

typedef enum
{
    GW_SUBDEV_HB_SET_STATUS,
    GW_SUBDEV_HB_STATUS,
    GW_SUBDEV_ACTIVE_CHECK_STATUS,
    GW_SUBDEV_VERSION_STATUS,
    GW_SUBDEV_ONOFF_STATUS,
    GW_SUBDEV_LEVEL_STATUS,
    GW_SUBDEV_BRIGHTNESS_STATUS,
    GW_SUBDEV_RAW_DATA,
    GW_SUBDEV_CCT_STATUS,
    GW_SUBDEV_TRIPLES_STATUS,
    GW_SUBDEV_MAX_STATUS
} enum_subdev_status_type_e;

/* for mgmt event process task */
typedef enum
{
    GW_SUBDEV_ADD_EVENT = 0,
    GW_SUBDEV_ACTIVE_CHECK_EVENT,
    GW_SUBDEV_VERSION_GET_EVENT,
} gateway_mgmt_event_e;

typedef enum
{
    GW_AUTH_INPUT_NUM,
    GW_AUTH_INPUT_STR,
} gw_auth_input_e;

typedef struct {
    uint8_t action;
    uint8_t size;
    uint8_t uuid[16];
    uint8_t dev_addr[6];
    uint8_t addr_type;
} gw_mesh_node_auth_ctx_t;

typedef union
{
    gw_mesh_node_auth_ctx_t mesh_ctx;
} gw_auth_ctx_t;

typedef struct {
    uint8_t       procotol;
    gw_auth_ctx_t auth_ctx;
} gw_auth_input_t;

typedef struct {
    uint8_t             status;
    uint8_t             flag;
    gw_subdev_triples_t status_data;
} gw_occ_subdev_triples_status_t;

typedef enum
{
    GW_NODE_INVAL        = -1,
    GW_NODE_ACTIVE       = 1,
    GW_NODE_ACTIVE_CHECK = 2,
    GW_NODE_NOT_ACTIVE   = 3
} gw_subdev_state_en;

typedef enum
{
    GW_DEV_NO_LPM = 0x0,
    GW_DEV_LPM_RX_TX,
    GW_DEV_LPM_TX_RX,
    GW_DEV_LPM_NO_RX,
    GW_DEV_LPM_NO_TX,
    GW_DEV_LPM_NO_RX_TX,
} gw_subdev_lpm_type_en;

typedef enum
{
    GW_DEV_GATEWAY = 0x0,
    GW_DEV_NODE,
} gw_subdev_node_type_en;

typedef struct {
    uint8_t protocol  : 3;
    uint8_t lpm_type  : 3;
    uint8_t node_type : 2;
} gw_subdev_type_t;

typedef struct {
    uint16_t unicast_addr;
    uint8_t  elem_num;
    uint8_t  dev_addr[6];
    uint8_t  addr_type;
    uint8_t  uuid[16];
    uint16_t oob_info;
} gw_subdev_ble_mesh_info_t;

typedef union
{
    gw_subdev_ble_mesh_info_t ble_mesh_info;
} gw_subdev_info_t;

typedef struct {
    gw_subdev_type_t dev_type;
    gw_subdev_info_t protocol_info;
    uint8_t          retry_time;
} gateway_add_subdev_msg_t;

typedef enum
{
    NODE_STATUS_IN_LPM = 0x0,
    NODE_STATUS_HB_NOT_ENABLE,
    NODE_STATUS_HB_PERIOD_MISS,
    NODE_STATUS_HB_PARAM_ERR,
} active_check_status_en;

typedef struct {
    atomic_t status;
    uint16_t node_period;
} gw_active_check_status_t;

typedef struct {
    uint32_t version;
    uint8_t  support_protocol;
} gw_subdev_version_status;

/**
 * @struct gw_evt_bind_fail_t
 * @brief scan dev info
 */
typedef struct {
    gw_subdev_protocol_e              protocol;
    gw_discovered_dev_protocol_info_t protocol_info;
    uint8_t                           failed_reason;
} gw_evt_bind_fail_t;

typedef union
{
    gw_subdev_ble_mesh_info_t ble_mesh_info;
} gw_subdev_status_protocol_info;

typedef struct {
    gw_subdev_protocol_e           protocol;
    gw_subdev_status_protocol_info protocol_info;
} gw_subdev_status_update_dev_info;

typedef struct {
    gw_subdev_protocol_e           protocol;
    gw_subdev_status_protocol_info protocol_info;
} gw_subdev_rst_status;

typedef struct {
    int8_t           state;
    gw_subdev_t      subdev;
    gw_subdev_type_t dev_type;
    gw_subdev_info_t info;
    // const char *     name;
    char             name[GW_DEV_NAME_LEN_MAX];
    uint32_t         version;
    uint8_t          support_ota_protocol;
    uint16_t         period;
    gw_subgrp_t      subgrp[GW_MAX_SUBDEV_JOINED_GRP_NUM];
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    char cid[33];
    char occ_pid[OCC_PID_MAXLEN];
#endif
    long long hb_last_recv;
} gw_subdev_ctx_t;

typedef struct {
    short           magic_code; /** to indicate whether infor from flash is valid */
    gw_subdev_ctx_t subdevs[CONFIG_GW_MAX_DEVICES_META_NUM + 1];
} gw_subdev_list_t;

/* states such as onoff/brightness/.. of each sub device, revised by customer*/
typedef struct dev_status_s {
    uint8_t  power_state;
    uint16_t bri;
    uint16_t cct;
} gw_subdev_states_t;

typedef struct {
    int8_t      state;
    gw_subgrp_t subgrp;
    const char *name;
} gw_subgrp_ctx_t;

typedef struct {
    gw_subgrp_ctx_t subgrps[GW_MAX_GW_GROUP_NUM];
} gw_subgrp_list_t;

typedef enum
{
    GW_SUBDEV_GET_TRIPLES_BY_OCC,
    GW_SUBDEV_GET_TRIPLES_BY_LOCAL,
    GW_SUBDEV_GET_TRIPLES_MAX
} gw_get_trpiles_method_e;

extern int gateway_mgmt_init(void);
extern int gateway_mgmt_send_process_msg(uint8_t event, void *msg);
extern int gateway_mgmt_modem_status(gw_subdev_ctx_t *subdev_ctx, uint8_t *data, size_t data_len);

extern int   gateway_subdev_offline_check_enable(uint8_t enable);
extern void  gateway_subdev_state_update(gw_subdev_ctx_t *dev, int8_t state, uint8_t reason);
extern int   gateway_subdev_offline_hb_pub_set(gw_subdev_t subdev, uint16_t period_s);
extern void *gateway_subdev_get_protocol_info(gw_subdev_t subdev);
extern int   gateway_occ_getjs(char *pid);
extern int   gateway_occ_get_dev_triples(uint8_t *pcid, gw_subdev_triples_t *gw_subdev_triples);
extern int   gateway_occ_get_gw_mac(void);

extern gw_subdev_ctx_t *gateway_mgmt_get_subdev_ctx_by_mac(char *mac);
extern gw_subdev_ctx_t *gateway_subdev_get_ctx(gw_subdev_t subdev);
extern gw_subdev_ctx_t *gateway_subdev_get_ctx_by_index(uint16_t index);

#endif
