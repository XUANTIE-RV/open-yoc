/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef BIT
#define BIT(n) (1UL << (n))
#endif

#define GW_DEV_ADDR_LEN     6
#define GW_DEV_NAME_LEN_MAX 16

    typedef int16_t gw_subdev_t;
    typedef int16_t gw_subgrp_t;
    typedef void *  gw_evt_param_t;
    typedef void *  gw_status_param_t;
    typedef int32_t gw_status_t;
    typedef uint8_t addr_t;

#define GW_DEV_DISCOVER_NO_TIMEOUT 0xFFFFFFFF

    /**
     * @brief subdev iter type
     */
    enum
    {
        GW_SUBDEV_ITER_STOP = 0,
        GW_SUBDEV_ITER_CONTINUE,
    };

    /**
     * @enum gw_dev_protocal_e
     * @brief dev protocal type
     */
    typedef enum
    {
        GW_NODE_BLE_MESH = 0x0, /* Ble Mesh sub dev */
    } gw_subdev_protocol_e;

    /**
     * @struct subdev_info_t
     * @brief subdev context that contains the device addr and the name of each subdev
     */
    typedef struct {
        uint8_t  active_status;
        addr_t   dev_addr[GW_DEV_ADDR_LEN];
        char     name[GW_DEV_NAME_LEN_MAX];
        uint32_t version;
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
        uint8_t occ_cid[33];
#endif
    } subdev_info_t;

    /**
     * @struct gw_mesh_node_prov_ctx
     * @brief mesh prov ctx
     */
    typedef struct {
        uint8_t  dev_addr[6];
        uint8_t  addr_type;
        uint8_t  uuid[16];
        uint8_t  bearer;
        uint16_t oob_info;
    } gw_ble_mesh_info_t;

    /**
     * @union scan_protocal_spec_ctx
     * @brief scan dev info for special protocal
     */
    typedef union
    {
        gw_ble_mesh_info_t ble_mesh_info;
    } gw_discovered_dev_protocol_info_t;

    /**
     * @struct gw_scan_msg_t
     * @brief scan dev info
     */
    typedef struct {
        gw_subdev_protocol_e              protocol;
        gw_discovered_dev_protocol_info_t protocol_info;
    } gw_evt_discovered_info_t;

    /**@enum gw_subdev_bind_failed_reason_e
     * @brief dev protocal type
     */
    typedef enum
    {
        GW_NODE_BIND_SUCCESS = 0x00,
        GW_NODE_BIND_FAILED_REASON_PROTOCOL_NOT_SUPPORT,
        GW_NODE_BIND_FAILED_REASON_NO_BUFFER,
        GW_NODE_BIND_FAILED_REASON_PROTOCOL_INTERNAL, /* Internal failed reason */
        GW_NODE_BIND_FAILED_REASON_OCC_AUTH_FAILED,
        GW_NODE_BIND_FAILED_REASON_GET_JS_FAILED,
    } gw_subdev_bind_status_e;

    /**
     * @enum gw_ota_target_en
     * @brief dev protocal type
     */
    typedef enum
    {
        GW_OTA_TARGET_LOCAL  = 0x00, /* OTA self*/
        GW_OTA_TARGET_REMOTE = 0x01, /* OTA for remote device*/
    } gw_ota_target_en;

    typedef gw_evt_discovered_info_t gw_discovered_dev_t;

    /**
     * @struct gw_evt_subdev_add_t
     * @brief event that add subdev
     */
    typedef struct {
        uint8_t     status; /* the result of adding subdev，0：success others：fail*/
        gw_subdev_t subdev; /* the handle of the addded subdev, valid only if status is 0*/
    } gw_evt_subdev_add_t;

    /**
     * @struct gw_evt_subdev_add_t
     * @brief event that add subdev
     */
    typedef struct {
        gw_subdev_protocol_e              protocol;      /* protocol */
        gw_discovered_dev_protocol_info_t protocol_info; /* protocol info*/
        uint8_t                           failed_reason; /* failed reason*/
    } gw_evt_subdev_add_fail_t;

    /**
     * @struct gw_evt_subdev_del_t
     * @brief event that delete subdev
     */
    typedef struct {
        uint8_t     status; /* the result of deleting subdev，0：success others：fail*/
        gw_subdev_t subdev; /* the handle of the deleted subdev*/
    } gw_evt_subdev_del_t;

    /**
     * @struct gw_evt_subdev_set_name_t
     * @brief event that set name of the subdev
     */
    typedef struct {
        uint8_t     status; /* the result of setting name，0：success others：fail*/
        gw_subdev_t subdev; /* the handle of the subdev*/
    } gw_evt_subdev_set_name_t;

    /**
     * @struct gw_evt_subgrp_create_t
     * @brief event that create a sub-group
     */
    typedef struct {
        uint8_t     status; /* the result of creating subgrp，0：success others：fail*/
        gw_subgrp_t subgrp; /* the handle of the created subgrp*/
    } gw_evt_subgrp_create_t;

    /**
     * @struct gw_evt_subgrp_del_t
     * @brief event that delete a sub-group
     */
    typedef struct {
        uint8_t     status; /* the result of deleting the subgrp，0：success others：fail*/
        gw_subgrp_t subgrp; /* the handle of the deleted subgrp*/
    } gw_evt_subgrp_del_t;

    /**
     * @struct gw_evt_subgrp_set_name_t
     * @brief event that set name for a sub-group
     */
    typedef struct {
        uint8_t     status; /* the result of setting name for the subgrp，0：success others：fail*/
        gw_subgrp_t subgrp; /* the handle of the subgrp*/
    } gw_evt_subgrp_set_name_t;

    /**
     * @struct gw_evt_subgrp_add_subdev_t
     * @brief event that add subdev to the sub-group
     */
    typedef struct {
        uint8_t     status; /* the result of adding subdev to the subgrp，0：success others：fail*/
        gw_subgrp_t subgrp; /* the handle of the subgrp*/
        gw_subdev_t subdev; /* the handle of the subdev that wanted to join the group*/
    } gw_evt_subgrp_add_subdev_t;

    /**
     * @struct gw_evt_subgrp_del_subdev_t
     * @brief event that delete subdev from a sub-group
     */
    typedef struct {
        uint8_t     status; /* the result of deleting the subdev from the subgrp，0：success others：fail*/
        gw_subgrp_t subgrp; /* the handle of the subgrp*/
        gw_subdev_t subdev; /* the handle of the subdev that wanted to delete from the group*/
    } gw_evt_subgrp_del_subdev_t;

    /**
     * @struct gw_evt_ota_firmware_t
     * @brief event of the ota firmware
     */
    typedef struct {
        uint8_t status; /* the fw status*/
        uint8_t index;  /* the index of the firmaware */
    } gw_evt_ota_firmware_t;

    /**
     * @struct gw_status_ota_t
     * @brief status of the ota node
     */
    typedef struct {
        gw_subdev_t subdev;         /* the handle of the subdev that join the ota*/
        uint8_t     status;         /* the dev ota status*/
        uint8_t     firmware_index; /* firmare index*/
        uint32_t    old_version;    /* the dev old version*/
        uint32_t    new_version;    /* the dev new version */
        uint32_t    cost_time;      /* the cost time*/
    } gw_status_ota_t;

    /**
     * @struct gw_subdev_hb_mgmt_enable_status_e
     * @brief status of the hb mgmt enable status
     */
    typedef enum
    {
        GW_HB_MGMT_ENABLE_SUCCESS = 0x0,
        GW_HB_MGMT_ENABLE_FAILED,
        GW_HB_MGMT_DISABLE_SUCCESS,
        GW_HB_MGMT_DISABLE_FAILED,
    } gw_subdev_hb_mgmt_set_status_e;

    /**
     * @struct gw_subdev_hb_mgmt_enable_status_t
     * @brief status of the hb mgmt enable status
     */
    typedef struct {
        uint8_t status; /* the status of the hb set status*/
    } gw_subdev_hb_mgmt_set_status_t;

    /**
     * @struct gw_subdev_hb_set_status_e
     * @brief status of the hb set
     */
    typedef enum
    {
        GW_HB_ENABLE_SUCCESS = 0x0,
        GW_HB_ENABLE_FAILED,
        GW_HB_DISABLE_SUCCESS,
        GW_HB_DISABLE_FAILED,
    } gw_subdev_hb_set_status_e;

    /**
     * @struct gw_subdev_hb_set_status_t
     * @brief status of the hb set
     */
    typedef struct {
        uint8_t  status;   /* the status of the hb set status*/
        uint16_t period_s; /* the period of the hb in s*/
    } gw_subdev_hb_set_status_t;

    /**
     * @struct gw_status_subdev_t
     * @brief status of the subdev
     */
    typedef struct {
        gw_subdev_t subdev;     /* the handle of the subdev */
        int8_t      old_status; /* the dev old status */
        int8_t      new_status; /* the dev new status */
        uint8_t     reason;     /* status change reason */
    } gw_status_subdev_t;

    /**
     * @enum gw_event_type_e
     * @brief event that send to gateway
     */
    typedef enum
    {
        GW_EVT_FOUND_DEV = 0x00,
        GW_EVT_FOUND_DEV_TIMEOUT,
        GW_SUBDEV_EVT_ADD,
        GW_SUBDEV_EVT_ADD_FAIL,
        GW_SUBDEV_EVT_DEL,
        GW_SUBDEV_EVT_SET_NAME,
        GW_SUBDEV_HB_MGMT_SET,
        GW_SUBGRP_EVT_CREATE,
        GW_SUBGRP_EVT_DEL,
        GW_SUBGRP_EVT_SET_NAME,
        GW_SUBDEV_EVT_OTA_FW,
        GW_EVT_MAX
    } gw_event_type_e;

    /**
     * @enum gw_event_ota_firmware_state_en
     * @brief status of ota firmware
     */
    typedef enum
    {
        GW_FIRMWARE_STATE_IDLE    = 0x00,
        GW_FIRMWARE_STATE_IN_USE  = 0x01,
        GW_FIRMWARE_STATE_END_USE = 0x02,
        GW_FIRMWARE_STATE_REMOVED = 0x03,
    } gw_event_ota_firmware_state_e;

    /**
     * @enum gw_event_ota_firmware_state_en
     * @brief status of ota firmware
     */
    typedef enum
    {
        GW_OTA_STATE_IDLE    = 0x00,
        GW_OTA_STATE_ONGOING = 0x01,
        GW_OTA_STATE_SUCCESS = 0x02,
        GW_OTA_STATE_FAILED  = 0x03,
    } gw_subdev_ota_state_e;

    typedef enum
    {
        GW_SUBDEV_NOT_ACTIVE = 0x00,
        GW_SUBDEV_ACTIVE,
    } gw_subdev_atcive_state_en;

    /**
     * @enum gw_subdev_status_change_reason_e
     * @brief status subdev status change reason
     */
    typedef enum
    {
        GW_SUBDEV_STATUS_CHANGE_BY_NODE_ADD = 0x00,
        GW_SUBDEV_STATUS_CHANGE_BY_NODE_RST,
        GW_SUBDEV_STATUS_CHANGE_BY_ACTIVE_CHECK_FAIL,
        GW_SUBDEV_STATUS_CHANGE_BY_ACTIVE_CHECK_SUCCESS,
        GW_SUBDEV_STATUS_CHANGE_BY_HB_NOT_ENABLE,
        GW_SUBDEV_STATUS_CHANGE_BY_HB_ERR_PARAM,
        GW_SUBDEV_STATUS_CHANGE_BY_HB_RECV,
        GW_SUBDEV_STATUS_CHANGE_BY_VERSION_RECV,
        GW_SUBDEV_STATUS_CHANGE_BY_HB_LOST,
    } gw_subdev_active_status_change_reason_e;

    /**
     * @enum gw_subdev_status_type_e
     * @brief status of subdev
     */
    typedef enum
    {
        GW_SUBDEV_STATUS_HB_SET,
        GW_SUBDEV_STATUS_CHANGE,
        GW_SUBDEV_STATUS_ONOFF,
        GW_SUBDEV_STATUS_BRI,
        GW_SUBDEV_STATUS_LEVEL,
        GW_SUBDEV_STATUS_CCT,
        GW_SUBDEV_STATUS_RAW_DATA,
        GW_SUBDEV_STATUS_TRIPLES_INFO,
        GW_SUBDEV_STATUS_VERSION,
        GW_SUBDEV_STATUS_OTA,
        GW_SUBDEV_STATUS_MODEL,
        GW_SUBDEV_STATUS_MAX
    } gw_subdev_status_type_e;

    /**
     * @enum gw_config_e
     * @brief config gateway
     */
    typedef enum
    {
        GW_CONFIG_SUPPORT_OCC_AUTH   = BIT(0),
        GW_CONFIG_SUPPORT_TSL_ENGINE = BIT(1),
        GW_CONFIG_SUPPORT_MAX        = BIT(0) | BIT(1),
    } gw_config_e;

    /**
     * @brief This is the callback function type of subdev status reporting
     * @param[in]  subdev the handle of the subdev that report status
     * @param[in]  gw_status different status type
     * @param[in]  status the status that subdev wants to report
     * @param[in]  status_len the length of status
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    typedef gw_status_t (*gw_subdev_status_cb_t)(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                                 gw_status_param_t status, int status_len);

    /**
     * @brief This is the callback function type of gateway event reporting
     * @param[in]  gw_evt the reporting event type
     * @param[in]  gw_evt_param the parameter of the event
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    typedef gw_status_t (*gw_event_cb_t)(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param);

    /**
     * @brief This function will initialize the gateway service
     * @param[in]  void
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_init(void);

    /**
     * @brief This function will configure the gateway service
     * @param[in]  config config gateway, such as support occ auth etc.
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_config(gw_config_e config);

    /**
     * @brief This function will start discover
     * @param[in]  dev show timeout
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_discover_start(uint32_t timeout);

    /**
     * @brief This function will start or stop discover
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_discover_stop();

    /**
     * @brief This function will add the discovered device to the subnet of the gateway
     * @param[in]  subdev the information of the discovered device
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_add(gw_discovered_dev_t subdev);

    /**
     * @brief This function will delete the subdev
     * @param[in]  subdev the handle of the subdev
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_del(gw_subdev_t subdev);

    /**
     * @brief This function will set the subdev name
     * @param[in]  subdev the handle of the subdev that needs to be named
     * @param[in]  name the name wanted to be set to the subdev
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_set_name(gw_subdev_t subdev, const char *name);

    /**
     * @brief This function will create a group
     * @param[in]  name name of the group
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subgrp_create(const char *name);

    /**
     * @brief This function will delete a group
     * @param[in]  subgrp the handle of the group that wanted to be deleted
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subgrp_del(gw_subgrp_t subgrp);

    /**
     * @brief This function will set name for a group
     * @param[in]  subgrp the handle of the subgrp that needs to be named
     * @param[in]  name the name for the subgrp
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subgrp_set_name(gw_subgrp_t subgrp, const char *name);

    /**
     * @brief This function will add a subdev to the dedicated group
     * @param[in]  subgrp the handle of the group that the subdev wants to join
     * @param[in]  subdev the handle of the subdev that wants to join the dedicated group
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subgrp_add_subdev(gw_subgrp_t subgrp, gw_subdev_t subdev);

    /**
     * @brief This function will add several subdevs to the dedicated group
     * @param[in]  subgrp the handle of the group that the subdevs want to join
     * @param[in]  subdev_list contains the handles of the subdevs that want to join the dedicated group
     * @param[in]  subdev_num the number of the subdevs that want to join the group
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subgrp_add_subdev_list(gw_subgrp_t subgrp, gw_subdev_t *subdev_list, uint8_t subdev_num);

    /**
     * @brief This function will delete a subdev from the dedicated group
     * @param[in]  subgrp the handle of the group that the subdev wants to delete from
     * @param[in]  subdev the handle of the subdev that wants to delete from the dedicated group
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subgrp_del_subdev(gw_subgrp_t subgrp, gw_subdev_t subdev);

    /**
     * @brief This function will delete several subdevs from the dedicated group
     * @param[in]  subgrp the handle of the group that the subdevs want to delete from
     * @param[in]  subdev_list the handle of the subdevs that want to delete from the dedicated group
     * @param[in]  subdev_num the number of the subdevs that want to delete from the group
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subgrp_del_subdev_list(gw_subgrp_t subgrp, gw_subdev_t *subdev_list, uint8_t subdev_num);

    /**
     * @brief This function will find all the subdevs that belong to the dedicated group
     * @param[in]  subgrp the handle of the group
     * @param[in]  func the callback that returns each info of the subdev
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subgrp_belonged_subdev_foreach(gw_subgrp_t subgrp,
                                                       void (*func)(gw_subgrp_t subgrp, gw_subdev_t subdev, void *data),
                                                       void *data);

    gw_status_t gateway_subgrp_set_onoff(gw_subgrp_t subgrp, uint8_t onoff);

    gw_status_t gateway_subgrp_set_brightness(gw_subgrp_t subgrp, uint16_t brightness);
    /**
     * @brief This function will control a subdev
     * @param[in]  subdev the handle of the subdev that wants to be controlled
     * @param[in]  onoff the on/off value
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_set_onoff(gw_subdev_t subdev, uint8_t onoff);

    /**
     * @brief This function will control a subdev
     * @param[in]  subdev the handle of the subdev that wants to be controlled
     * @param[in]  bri the brightness value
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_set_brightness(gw_subdev_t subdev, uint16_t brightness);

    /**
     * @brief This function will send raw data to the subdev
     * @param[in]  subdev the handle of the subdev
     * @param[in]  raw_data the data wants to be sent
     * @param[in]  data_len the length of data
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_send_rawdata(gw_subdev_t subdev, uint8_t *raw_data, int data_len);

    /**
     * @brief This function will get the on/off status of the dedicated subdev
     * @param[in]  subdev the handle of the subdev
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_get_onoff(gw_subdev_t subdev);

    /**
     * @brief This function will get the brightness status of the dedicated subdev
     * @param[in]  subdev the handle of the subdev
     * @param[out] bri the brightness status of the subdev
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_get_brightness(gw_subdev_t subdev);

    /**
     * @brief This function will traverse all the subdev that managed by the gateway
     * @param[in]  func the callback that output the handle of each subdev
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_foreach(uint8_t (*func)(gw_subdev_t subdev, void *data), void *data);

    /**
     * @brief This function will get information of the subdev
     * @param[in]  subdev the handle of the subdev
     * @param[out] info the pointer of the information of the subdev
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_get_info(gw_subdev_t subdev, subdev_info_t *info);

    /*
    group0、group1、group2、group3
    场景一：子设备可以同时属于0,1,2,3，加入新的分组时，遍历没有，push_back/有序插入(提高遍历效率)：2->2,3->0,2,3
    场景二：子设备不能同时属于01，也不能同时属于23，subgrp[0]= -1/group0/group1 ，subgrp[1]= -1/group2/group3，
    */
    gw_subgrp_t gateway_subdev_get_subgrpx(gw_subdev_t subdev,int index);

    int gateway_subdev_set_subgrpx(gw_subdev_t subdev,int index,gw_subgrp_t newgrp);

    /**
     * @brief This function will get triples of the subdev local
     * @param[in]  subdev the handle of the subdev
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_get_cloud_info(gw_subdev_t subdev);

    /**
     * @brief This function will start ota for subdevs
     * @param[in/out]  to get the firmware index
     * @param[in]  subdev_list the handles of the subdevs that need to ota
     * @param[in]  subdev_num the num of ota subdevs
     * @param[in]  image the ota image
     * @param[in]  image_size the size of the ota image
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_ota(int *fw_index, gw_subdev_t *subdev_list, uint8_t subdev_num, const void *image,
                                   uint32_t image_size);

    /**
     * @brief This function will stop ota for subdevs
     * @param[in]  the firmware index
     * @param[in]  subdev_list the handles of the subdevs that need to ota
     * @param[in]  subdev_num the num of ota subdevs
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_ota_stop(int fw_index, gw_subdev_t *subdev_list, uint8_t subdev_num);

    /**
     * @brief This function will control subdev by js modeling method
     * @param[in]  subdev the handle of the subdev that will be controlled
     * @param[in]  model the data model of this control, usually a json string
     * @param[in]  model_len the length of data model
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_model_ctrl(gw_subdev_t subdev, void *model, int model_len);

    /**
     * @brief This function is used to enable or disable offline check
     * @param[in]
     * - 0 disable
     * - 1 enable
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_heartbeat_mgmt_enable(uint8_t enable);

    /**
     * @brief This function is used to config subdev hb pub period
     * @param[in]
     * - 0 disable
     * - 1 enable
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_heartbeat_period_set(gw_subdev_t subdev, uint16_t period_s);

    /**
     * @brief This function will register the subdev status callback
     * @param[in]  subdev the handle of the subdev that wants to be controlled
     * @param[in]  hb pub period in s
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_subdev_status_cb_register(gw_subdev_status_cb_t cb);

    /**
     * @brief This function will register the gateway event callback
     * @param[in]  cb the handle of the callback function
     *
     * @return return value
     * - 0 success
     * - others fail
     */
    gw_status_t gateway_event_cb_register(gw_event_cb_t cb);

#ifdef __cplusplus
}
#endif
#endif
