/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __GATEWAY_UT_H__
#define __GATEWAY_UT_H__
#include "app_main.h"
#include "app_gateway_cmds.h"
#include <gateway.h>

#include "smartliving/iot_import.h"
#include "smartliving/exports/iot_export_linkkit.h"

#define GATEWAY_SUPPORT_GROUP_CTRL  1

// for demo only
#define PRODUCT_KEY "PK_XXXXX"
#define PRODUCT_SECRET "PS_XXXXX"
#define DEVICE_NAME "DN_XXXXX"
#define DEVICE_SECRET "DS_XXXXX"

#define TOPO_LIST_PK "productKey"
#define TOPO_LIST_DN "deviceName"

#define TOPO_CHANGE_STATUS "status"
#define TOPO_CHANGE_SUBLIST "subList"

// for demo currently, later will get these info from occ
#define BLE_MESH_LIGHT_PK "a1faJDbzH2E"
#define BLE_MESH_LIGHT_PS "SLEKF6F3L4xtI8yS"

#define QUEUE_MSG_SIZE sizeof(gateway_msg_t)
#define MAX_QUEUE_SIZE 10 * QUEUE_MSG_SIZE

#define SUB_DEV_QUEUE_MSG_SIZE sizeof(gateway_add_subdev_msg_t)
#define MAX_SUB_DEV_QUEUE_SIZE 10 * SUB_DEV_QUEUE_MSG_SIZE

#define MAX_RETRY_TIME 5


//KV config gw type,VALUE:master mean it is gateway,slave is subdev
#define GATEWAY_DEVICE_TYPE_KEY "gwtype"
#define GATEWAY_DEVICE_TYPE_MASTER "master"
#define GATEWAY_DEVICE_TYPE_SLAVE "slave"
#define GATEWAY_DEVICE_TYPE_LEN (7)

#define GATEWAY_FORBIDDEN_AUTO_ADD_SUBDEV_FLAG_KEY "gwf"
#define GATEWAY_FORBIDDEN_AUTO_ADD_SUBDEV_FLAG_LEN 2

#define GATEWAY_MAGIC_CODE 0x736A

#define GATEWAY_NODE_INVALID -1
#define GATEWAY_NODE_UNREG -2
#define GATEWAY_NODE_UNBIND 0
#define GATEWAY_NODE_BINDED 1

typedef enum _gw_topo_change_status_e
{
    GW_TOPO_CHANGE_STATUS_ADD = 0,
    GW_TOPO_CHANGE_STATUS_DELETE = 1,
    GW_TOPO_CHANGE_STATUS_ENABLE = 2,
    GW_TOPO_CHANGE_STATUS_DISABLE = 8,
    GW_TOPO_CHANGE_STATUS_INVALID
} gw_topo_change_status_e;

typedef enum _gw_device_type_e
{
    GW_DEVICE_MASTER = 0,
    GW_DEVICE_SLAVE,
    GW_DEVICE_INVALID
} gw_device_type_e;

typedef enum _gw_topo_get_reason_e
{
    GW_TOPO_GET_REASON_CONNECT_CLOUD = 0,
    GW_TOPO_GET_REASON_CLI_CMD,
    GW_TOPO_GET_REASON_MAX
} gw_topo_get_reason_e;

typedef enum _gateway_msg_type_e
{
    GATEWAY_MSG_TYPE_ADD,
    GATEWAY_MSG_TYPE_ADD_RANGE,
    GATEWAY_MSG_TYPE_DEL,
    GATEWAY_MSG_TYPE_DEL_RANGE,
    GATEWAY_MSG_TYPE_RESET,
    GATEWAY_MSG_TYPE_UPDATE,
    GATEWAY_MSG_TYPE_ADDALL,
    GATEWAY_MSG_TYPE_DELALL,
    GATEWAY_MSG_TYPE_QUERY_SUBDEV_ID,
    GATEWAY_MSG_TYPE_CLOUD_CONNECT,
    GATEWAY_MSG_TYPE_PERMIT_JOIN,
    GATEWAY_MSG_TYPE_LOGIN_ALL,
    GATEWAY_MSG_TYPE_LOGOUT_ALL,
    GATEWAY_MSG_TYPE_MAX
} gateway_msg_type_e;

typedef struct _gateway_msg_s
{
    gateway_msg_type_e msg_type;
    int devid;
    int devid_end;
    char *payload;
    int payload_len;
} gateway_msg_t;

typedef struct _suddev_cloud_info_s
{
    short cloud_devid;
    gw_subdev_t subdev;
    iotx_linkkit_dev_meta_info_t meta;
    uint8_t bind;
} subdev_cloud_info_t;

typedef struct _gw_subdev_cloud_info_s
{
    subdev_cloud_info_t info[MAX_DEVICES_META_NUM+1];
} gw_subdev_cloud_info_t;


int app_gateway_ut_init(void);
int app_gateway_ut_handle_permit_join(void);
int app_gateway_ut_handle_topolist_reply(const char *payload, const int payload_len);
int app_gateway_ut_send_msg(gateway_msg_t *, int);
int app_gateway_ut_msg_process(int master_devid, int timeout);
void gateway_ut_misc_process(uint64_t time_now_sec);
int app_gateway_ut_update_subdev(gw_topo_get_reason_e reason);
int app_gateway_ut_handle_property_set(const int devid, const char *request, const int request_len);
void app_gateway_subdev_registered_handle(int devid);
void app_gateway_subdev_binded_handle(int devid);
int gateway_subdev_rpt_onoff(char *mac, uint8_t pwrstate);
int gateway_subdev_rpt_brightness(char *mac, uint8_t bri);
gw_status_t app_gateway_ut_event_process(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param);
gw_status_t app_gateway_ut_subdev_status_process(gw_subdev_t subdev, gw_subdev_status_type_e gw_status, gw_status_param_t status, int status_len);


#endif
