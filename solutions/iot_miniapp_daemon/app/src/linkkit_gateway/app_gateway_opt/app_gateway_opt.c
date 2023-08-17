#include "app_gateway_opt.h"

#define SUBNODE_NUMS 10
#define TAG "app_gateway_opt"

gw_evt_discovered_info_t subdevNum[SUBNODE_NUMS];

static gw_status_t app_subdev_status_process_cb(gw_subdev_t subdev, gw_subdev_status_type_e gw_status, gw_status_param_t status, int status_len);
static gw_status_t app_gw_event_process_cb(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param);
extern const char *bt_hex_real(const void *buf, size_t len);

static struct updatestatus{
    gw_subdev_t subdev;
    uint8_t onoff;
}subdevstatus;

static inline int bt_addr_val_to_str(const uint8_t addr[6], char *str, size_t len)
{
    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

static gw_evt_discovered_info_t g_gw_msg;
static gw_subdev_t g_subdev_index;
static gw_status_t app_gw_event_process_cb(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param)
{
    LOGD(TAG, "Gw event:%02x", gw_evt);
    gw_evt_discovered_info_t *msg = (gw_evt_discovered_info_t *)malloc(sizeof(gw_evt_discovered_info_t));
    memcpy(msg,gw_evt_param,sizeof(gw_evt_discovered_info_t));
    uint8_t                  dev_addr_str[20];
    g_gw_msg = (*msg);
    bt_addr_val_to_str(msg->protocol_info.ble_mesh_info.dev_addr, (char *)dev_addr_str, sizeof(dev_addr_str));

    switch (gw_evt) {
        case GW_SUBGRP_EVT_CREATE:
            LOGD(TAG, "%s GW_SUBGRP_EVT_CREATE", state_change);
            break;
        case GW_EVT_FOUND_DEV: {
            LOGD(TAG, "%s GW_EVT_FOUND_DEV", state_change);
            
            if (msg->protocol == GW_NODE_BLE_MESH) {
                add_scan_meshdev(&g_gw_msg);
                LOGI(TAG, "Found mesh dev:%s",  dev_addr_str);
                // jsapi_gateway_publish_provShowDev((char *)dev_addr_str);
            }
        } break;
        case GW_EVT_FOUND_DEV_TIMEOUT: {
            LOGD(TAG, "%s GW_EVT_FOUND_DEV_TIMEOUT", state_change);
        } break;
        case GW_SUBDEV_EVT_ADD:{
            // 发生添加子设备事件
            gw_evt_subdev_add_t *add = (gw_evt_subdev_add_t *)malloc(sizeof(gw_evt_subdev_add_t));
            memcpy(add,gw_evt_param,sizeof(gw_evt_subdev_add_t));
            gw_evt_subdev_add_t *gw_evt_subdev_add = NULL;            //meshdev added indev update
            gw_evt_subdev_add = (gw_evt_subdev_add_t *)gw_evt_param;
            int16_t index = gw_evt_subdev_add->subdev;
            LOGD(TAG, "[[[[[[[index:%d]]]]]]]\r\n", index);
            jsapi_gateway_publish_addNodeRst("success", (void*)gw_evt_param);
        } break;
        case GW_SUBDEV_EVT_ADD_FAIL: {
            LOGD(TAG, "%s GW_SUBDEV_EVT_ADD_FAIL", state_change);
            jsapi_gateway_publish_addNodeRst("fail", (void*)gw_evt_param);
        } break;
        case GW_SUBDEV_EVT_DEL:{
            LOGD(TAG, "%s GW_SUBDEV_EVT_DEL", state_change);
            char kv_index[20];
            char kv_devname[20];
            gw_evt_subdev_add_t *gw_evt_subdev_add = NULL;
            gw_evt_subdev_add = (gw_evt_subdev_add_t *)gw_evt_param;
            int16_t index = gw_evt_subdev_add->subdev;
            snprintf(kv_index, sizeof(kv_index), "index%d", index);
            snprintf(kv_devname, sizeof(kv_devname), "dev-name-00%d", index);

            aos_kv_del(kv_index);
            aos_kv_del(kv_devname);

            jsapi_gateway_publish_delNodeRst("success");
        } break;
        case GW_SUBDEV_EVT_SET_NAME:{
            LOGD(TAG, "%s GW_SUBDEV_EVT_SET_NAME", state_change);
        } break;
        case GW_SUBDEV_HB_MGMT_SET: 
            LOGD(TAG, "%s GW_SUBDEV_HB_MGMT_SET", state_change);
            gw_subdev_hb_mgmt_set_status_t *set_status = (gw_subdev_hb_mgmt_set_status_t *)gw_evt_param;
            LOGD(TAG, "Gw hb mgmt set status:%02x", set_status->status);
            (void)set_status;
            break;
        case GW_SUBGRP_EVT_DEL:
            LOGD(TAG, "%s GW_SUBGRP_EVT_DEL", state_change);
            break;
        case GW_SUBGRP_EVT_SET_NAME:
            LOGD(TAG, "%s GW_SUBGRP_EVT_SET_NAME", state_change);
            break;
        case GW_SUBDEV_EVT_OTA_FW:
            LOGD(TAG, "%s GW_SUBDEV_EVT_OTA_FW", state_change);
            break;
        case GW_EVT_MAX:
            LOGD(TAG, "%s GW_EVT_MAX", state_change);
            break;
        default:
            break;
    }
    return 0;
}

static gw_status_t app_subdev_status_process_cb(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                         gw_status_param_t status, int status_len)
{

    LOGD(TAG, "Subdev[%d] status:%02x ", subdev, gw_status);
    g_subdev_index = gw_status;
    switch (gw_status) {
        case GW_SUBDEV_STATUS_HB_SET: 
            LOGD(TAG, "%s GW_SUBDEV_STATUS_HB_SET", sub_change);
            break;
        case GW_SUBDEV_STATUS_ONOFF: 
            LOGD(TAG, "%s GW_SUBDEV_STATUS_ONOFF", sub_change);
            uint8_t pwrstate             = *(uint8_t *)status;
            subdevstatus.subdev = subdev;
            subdevstatus.onoff = pwrstate;
            jsapi_gateway_publish_nodeStatusRpt((void*)&subdevstatus);
            break;
        case GW_SUBDEV_STATUS_BRI:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_BRI", sub_change);
            break;
        case GW_SUBDEV_STATUS_TRIPLES_INFO:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_TRIPLES_INFO", sub_change);
            break;
        case GW_SUBDEV_STATUS_RAW_DATA:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_RAW_DATA", sub_change);
            break;
        case GW_SUBDEV_STATUS_MODEL: 
            LOGD(TAG, "%s GW_SUBDEV_STATUS_MODEL", sub_change);
            break;
        case GW_SUBDEV_STATUS_CHANGE:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_CHANGE", sub_change);
            // 添加子设备的时候 状态变化
            break;
        case GW_SUBDEV_STATUS_OTA:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_OTA", sub_change);
            break;
        case GW_SUBDEV_STATUS_VERSION:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_VERSION", sub_change);
            break;
        case GW_SUBDEV_STATUS_CCT:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_CCT", sub_change);
            break;
        case GW_SUBDEV_STATUS_LEVEL:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_LEVEL", sub_change);
            break;
        case GW_SUBDEV_STATUS_MAX:
            LOGD(TAG, "%s GW_SUBDEV_STATUS_MAX", sub_change);
            break;
        default:
            break;
    }

    return 0;
}

int app_gateway_init()
{
    LOGD(TAG, "init gateway ");

    /* Register callback for gateway operate event */
    gateway_event_cb_register(app_gw_event_process_cb);

    /* Register callback for subdev status report */
    gateway_subdev_status_cb_register(app_subdev_status_process_cb);

    /* init gateway service */
    gateway_init();
            
    gw_config_e gw_config = 0;

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    /* config if gateway support occ auth/tsl engine and so on*/
    gw_config |= GW_CONFIG_SUPPORT_OCC_AUTH;
#endif

#if defined(CONFIG_TSL_DEVICER_MESH) && CONFIG_TSL_DEVICER_MESH
    /* config if gateway support occ auth/tsl engine and so on*/
    gw_config |= GW_CONFIG_SUPPORT_TSL_ENGINE;
#endif
    gateway_config(gw_config);

    gateway_subgrp_create("NONE");

    // char kv_group[20];
    // char kv_groupname[32];
    // ========================== 房间类型组 ===============================
    // snprintf(kv_group, sizeof(kv_group), "grp-name-00%d", 0);
    // int ret = aos_kv_getstring(kv_group, kv_groupname, sizeof(kv_groupname));
    // LOGD(TAG, "ret %d %s", ret, kv_group);
    // ==== 客厅组 ===
    // if (strcmp(kv_groupname, "LIVING_ROOM") != 0) {
        // aos_kv_del(kv_group);
        gateway_subgrp_create("LIVING_ROOM");
    // }
    // memset(kv_group, 0, sizeof(kv_group));
    // memset(kv_groupname, 0, sizeof(kv_groupname));

    // snprintf(kv_group, sizeof(kv_group), "grp-name-00%d", 1);
    // aos_kv_getstring(kv_group, kv_groupname, sizeof(kv_groupname));
    // LOGD(TAG, "ret %d %s", ret, kv_group);
    // ==== 卧室组 ===
    // if (strcmp(kv_groupname, "BED_ROOM") != 0) {
        // aos_kv_del(kv_group);
        gateway_subgrp_create("BED_ROOM");
    // }
    // memset(kv_group, 0, sizeof(kv_group));
    // memset(kv_groupname, 0, sizeof(kv_groupname));

    // ========================== 设备类型组 ===============================
    // ==== 照明灯组 ====
    // snprintf(kv_group, sizeof(kv_group), "grp-name-00%d", 2);
    // aos_kv_getstring(kv_group, kv_groupname, sizeof(kv_groupname));
    // LOGD(TAG, "ret %d %s", ret, kv_group);
    // if (strcmp(kv_groupname, "OBJ_LIGHT") != 0) {
        // aos_kv_del(kv_group);
        gateway_subgrp_create("OBJ_LIGHT");
    // }
    // ==== 窗帘组 ==== 
    // snprintf(kv_group, sizeof(kv_group), "grp-name-00%d", 3);
    // aos_kv_getstring(kv_group, kv_groupname, sizeof(kv_groupname));
    // LOGD(TAG, "ret %d %s", ret, kv_group);
    // if (strcmp(kv_groupname, "OBJ_SCREEN") != 0) {
        // aos_kv_del(kv_group);
        gateway_subgrp_create("OBJ_SCREEN");
    // }

    return 0;
}