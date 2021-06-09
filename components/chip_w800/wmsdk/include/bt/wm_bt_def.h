/**
 * @file    wm_bt_def.h
 *
 * @brief   Bluetooth Define
 *
 * @author  WinnerMicro
 *
 * Copyright (c) 2020 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_BT_DEF_H
#define WM_BT_DEF_H

/** Bluetooth Error Status */
typedef enum
{
    TLS_BT_STATUS_SUCCESS, /**< success */
    TLS_BT_STATUS_FAIL,
    TLS_BT_STATUS_NOT_READY,
    TLS_BT_STATUS_NOMEM, /**< alloc memory failed */
    TLS_BT_STATUS_BUSY,
    TLS_BT_STATUS_DONE,        /**< request already completed */
    TLS_BT_STATUS_UNSUPPORTED,
    TLS_BT_STATUS_PARM_INVALID,
    TLS_BT_STATUS_UNHANDLED,
    TLS_BT_STATUS_AUTH_FAILURE,
    TLS_BT_STATUS_RMT_DEV_DOWN,
    TLS_BT_STATUS_AUTH_REJECTED,
    TLS_BT_STATUS_THREAD_FAILED, /**< create internal thread failed */
    TLS_BT_STATUS_INTERNAL_ERROR, /**< controller stack internal error */
    TLS_BT_STATUS_CTRL_ENABLE_FAILED,
    TLS_BT_STATUS_HOST_ENABLE_FAILED,
    TLS_BT_STATUS_CTRL_DISABLE_FAILED,
    TLS_BT_STATUS_HOST_DISABLE_FAILED,

} tls_bt_status_t;

typedef enum
{
    TLS_BT_CTRL_IDLE = 0,
    TLS_BT_CTRL_ENABLED,
    TLS_BT_CTRL_SLEEPING,
} tls_bt_ctrl_status_t;

/** Bluetooth Adapter State */
typedef enum
{
    WM_BT_STATE_OFF,
    WM_BT_STATE_ON
} tls_bt_state_t;

/** bluetooth host statck events */
typedef enum
{
    WM_BT_ADAPTER_STATE_CHG_EVT,        
    WM_BT_ADAPTER_PROP_CHG_EVT,        
    WM_BT_RMT_DEVICE_PROP_EVT,               
    WM_BT_DEVICE_FOUND_EVT, 
    WM_BT_DISCOVERY_STATE_CHG_EVT,
    WM_BT_REQUEST_EVT,
    WM_BT_SSP_REQUEST_EVT,
    WM_BT_PIN_REQUEST_EVT,
    WM_BT_BOND_STATE_CHG_EVT,
    WM_BT_ACL_STATE_CHG_EVT,
    WM_BT_ENERGY_INFO_EVT,
    WM_BT_LE_TEST_EVT
} tls_bt_host_evt_t;

typedef struct
{
    tls_bt_state_t         status; /**< bluetooth adapter state */
} tls_bt_adapter_state_change_msg_t;

/** Bluetooth Adapter and Remote Device property types */
typedef enum
{
    /* Properties common to both adapter and remote device */
    /**
     * Description - Bluetooth Device Name
     * Access mode - Adapter name can be GET/SET. Remote device can be GET
     * Data type   - bt_bdname_t
     */
    WM_BT_PROPERTY_BDNAME = 0x1,
    /**
     * Description - Bluetooth Device Address
     * Access mode - Only GET.
     * Data type   - bt_bdaddr_t
     */
    WM_BT_PROPERTY_BDADDR,
    /**
     * Description - Bluetooth Service 128-bit UUIDs
     * Access mode - Only GET.
     * Data type   - Array of bt_uuid_t (Array size inferred from property length).
     */
    WM_BT_PROPERTY_UUIDS,
    /**
     * Description - Bluetooth Class of Device as found in Assigned Numbers
     * Access mode - Only GET.
     * Data type   - uint32_t.
     */
    WM_BT_PROPERTY_CLASS_OF_DEVICE,
    /**
     * Description - Device Type - BREDR, BLE or DUAL Mode
     * Access mode - Only GET.
     * Data type   - bt_device_type_t
     */
    WM_BT_PROPERTY_TYPE_OF_DEVICE,
    /**
     * Description - Bluetooth Service Record
     * Access mode - Only GET.
     * Data type   - bt_service_record_t
     */
    WM_BT_PROPERTY_SERVICE_RECORD,

    /* Properties unique to adapter */
    /**
     * Description - Bluetooth Adapter scan mode
     * Access mode - GET and SET
     * Data type   - bt_scan_mode_t.
     */
    WM_BT_PROPERTY_ADAPTER_SCAN_MODE,
    /**
     * Description - List of bonded devices
     * Access mode - Only GET.
     * Data type   - Array of bt_bdaddr_t of the bonded remote devices
     *               (Array size inferred from property length).
     */
    WM_BT_PROPERTY_ADAPTER_BONDED_DEVICES,
    /**
     * Description - Bluetooth Adapter Discovery timeout (in seconds)
     * Access mode - GET and SET
     * Data type   - uint32_t
     */
    WM_BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT,

    /* Properties unique to remote device */
    /**
     * Description - User defined friendly name of the remote device
     * Access mode - GET and SET
     * Data type   - bt_bdname_t.
     */
    WM_BT_PROPERTY_REMOTE_FRIENDLY_NAME,
    /**
     * Description - RSSI value of the inquired remote device
     * Access mode - Only GET.
     * Data type   - int32_t.
     */
    WM_BT_PROPERTY_REMOTE_RSSI,
    /**
     * Description - Remote version info
     * Access mode - SET/GET.
     * Data type   - bt_remote_version_t.
     */

    WM_BT_PROPERTY_REMOTE_VERSION_INFO,

    /**
     * Description - Local LE features
     * Access mode - GET.
     * Data type   - bt_local_le_features_t.
     */
    WM_BT_PROPERTY_LOCAL_LE_FEATURES,

    WM_BT_PROPERTY_REMOTE_DEVICE_TIMESTAMP = 0xFF,
} tls_bt_property_type_t;

/** Bluetooth Adapter Property data structure */
typedef struct
{
    tls_bt_property_type_t type;
    int len;
    void *val;
} tls_bt_property_t;

typedef struct
{
    tls_bt_status_t     status; 
	int num_properties;
	tls_bt_property_t *properties; /**< bluetooth adapter property data */
} tls_bt_adapter_prop_msg_t; 

/** Bluetooth Address */
typedef struct
{
    uint8_t address[6];
} __attribute__((packed))tls_bt_addr_t;

typedef struct
{
    tls_bt_status_t     status;
	tls_bt_addr_t     *address;
	int num_properties;
	tls_bt_property_t *properties; /**< bluetooth adapter property data */
} tls_bt_remote_dev_prop_msg_t;

typedef struct
{
	int num_properties;
	tls_bt_property_t *properties; /**< bluetooth adapter property data */
} tls_bt_device_found_msg_t;

/** Bluetooth Adapter Discovery state */
typedef enum
{
    WM_BT_DISCOVERY_STOPPED,
    WM_BT_DISCOVERY_STARTED
} tls_bt_discovery_state_t;

typedef struct
{
	tls_bt_discovery_state_t state;
} tls_bt_discovery_state_chg_msg_t;

/** Bluetooth Device Name */
typedef struct
{
    uint8_t name[249];
} __attribute__((packed))tls_bt_bdname_t;

typedef struct
{
	tls_bt_addr_t *remote_bd_addr;
	tls_bt_bdname_t *bd_name;
	uint32_t cod;
	uint8_t  min_16_digit;
} tls_bt_pin_request_msg_t;

/** Bluetooth SSP Bonding Variant */
typedef enum
{
    WM_BT_SSP_VARIANT_PASSKEY_CONFIRMATION,
    WM_BT_SSP_VARIANT_PASSKEY_ENTRY,
    WM_BT_SSP_VARIANT_CONSENT,
    WM_BT_SSP_VARIANT_PASSKEY_NOTIFICATION
} tls_bt_ssp_variant_t;

/** Bluetooth PinKey Code */
typedef struct
{
    uint8_t pin[16];
} __attribute__((packed))tls_bt_pin_code_t;

typedef struct
{
	tls_bt_addr_t *remote_bd_addr;
	tls_bt_bdname_t *bd_name;
	uint32_t cod;
	tls_bt_ssp_variant_t pairing_variant;
	uint32_t pass_key;
} tls_bt_ssp_request_msg_t;

/** Bluetooth Bond state */
typedef enum
{
    WM_BT_BOND_STATE_NONE,
    WM_BT_BOND_STATE_BONDING,
    WM_BT_BOND_STATE_BONDED
} tls_bt_bond_state_t;

typedef struct
{
	tls_bt_status_t status;
	tls_bt_addr_t *remote_bd_addr;
	tls_bt_bond_state_t state;
} tls_bt_bond_state_chg_msg_t;

/** Bluetooth ACL connection state */
typedef enum
{
    WM_BT_ACL_STATE_CONNECTED,
    WM_BT_ACL_STATE_DISCONNECTED
} tls_bt_acl_state_t;

typedef struct
{
	tls_bt_status_t status;
	tls_bt_addr_t *remote_address;
	tls_bt_acl_state_t state;
} tls_bt_acl_state_chg_msg_t;

typedef struct
{
    uint8_t status;
    uint8_t ctrl_state;     /* stack reported state */
    uint64_t tx_time;       /* in ms */
    uint64_t rx_time;       /* in ms */
    uint64_t idle_time;     /* in ms */
    uint64_t energy_used;   /* a product of mA, V and ms */
} __attribute__((packed))tls_bt_activity_energy_info;

typedef struct
{
	tls_bt_activity_energy_info *energy_info;
} tls_bt_energy_info_msg_t;

typedef struct
{
	uint8_t status;
	uint32_t count;
} tls_bt_ble_test_msg_t;

typedef union
{
	tls_bt_adapter_state_change_msg_t	     adapter_state_change;	  
	tls_bt_adapter_prop_msg_t	             adapter_prop;	  
	tls_bt_remote_dev_prop_msg_t		     remote_device_prop;
	tls_bt_device_found_msg_t                device_found;
	tls_bt_discovery_state_chg_msg_t         discovery_state;
	tls_bt_pin_request_msg_t                 pin_request;
	tls_bt_ssp_request_msg_t                 ssp_request;
	tls_bt_bond_state_chg_msg_t              bond_state;
	tls_bt_acl_state_chg_msg_t               acl_state;
	tls_bt_energy_info_msg_t                 energy_info;
	tls_bt_ble_test_msg_t                    ble_test;
} tls_bt_host_msg_t;

/** BT host callback function */
typedef void (*tls_bt_host_callback_t)(tls_bt_host_evt_t event, tls_bt_host_msg_t *p_data);


typedef enum
{
	WM_AUDIO_OVER_HCI = 0,
	WM_AUDIO_INTERNAL_MODE,
} tls_sco_data_path_t;

typedef enum
{
	TLS_BT_LOG_NONE = 0,
	TLS_BT_LOG_ERROR = 1,
	TLS_BT_LOG_WARNING = 2,
	TLS_BT_LOG_API = 3,
	TLS_BT_LOG_EVENT = 4,
	TLS_BT_LOG_DEBUG = 5,
	TLS_BT_LOG_VERBOSE = 6,
} tls_bt_log_level_t;

typedef struct
{
  uint8_t  uart_index;  /**< uart port index, 1~4 */
  uint32_t band_rate;   /**< band rate: 115200 ~ 2M */
  uint8_t data_bit;     /**< data bit:5 ~ 8 */
  uint8_t verify_bit;   /**< 0:NONE, 1 ODD, 2 EVEN */
  uint8_t stop_bit;     /**< 0:1bit; 1:1.5bit; 2:2bits */
} tls_bt_hci_if_t;

typedef enum
{
	TLS_BLE_PWR_TYPE_CONN_HDL0,
	TLS_BLE_PWR_TYPE_CONN_HDL1,
	TLS_BLE_PWR_TYPE_CONN_HDL2,
	TLS_BLE_PWR_TYPE_CONN_HDL3,
	TLS_BLE_PWR_TYPE_CONN_HDL4,
	TLS_BLE_PWR_TYPE_CONN_HDL5,
	TLS_BLE_PWR_TYPE_CONN_HDL6,
	TLS_BLE_PWR_TYPE_CONN_HDL7,
	TLS_BLE_PWR_TYPE_CONN_HDL8,
	TLS_BLE_PWR_TYPE_ADV,
	TLS_BLE_PWR_TYPE_SCAN,
	TLS_BLE_PWR_TYPE_DEFAULT,
} tls_ble_power_type_t;

typedef struct
{
	void (*notify_controller_avaiable_hci_buffer)(int cnt);
	void (*notify_host_recv_h4)(uint8_t *ptr, uint16_t length);
} tls_bt_host_if_t;


/*****************************************************************************
 **  Constants and Type Definitions
 *****************************************************************************/


/** Attribute permissions */
#define WM_GATT_PERM_READ              (1 << 0) /**< bit 0 -  0x0001 */
#define WM_GATT_PERM_READ_ENCRYPTED    (1 << 1) /**< bit 1 -  0x0002 */
#define WM_GATT_PERM_READ_ENC_MITM     (1 << 2) /**< bit 2 -  0x0004 */
#define WM_GATT_PERM_WRITE             (1 << 4) /**< bit 4 -  0x0010 */
#define WM_GATT_PERM_WRITE_ENCRYPTED   (1 << 5) /**< bit 5 -  0x0020 */
#define WM_GATT_PERM_WRITE_ENC_MITM    (1 << 6) /**< bit 6 -  0x0040 */
#define WM_GATT_PERM_WRITE_SIGNED      (1 << 7) /**< bit 7 -  0x0080 */
#define WM_GATT_PERM_WRITE_SIGNED_MITM (1 << 8) /**< bit 8 -  0x0100 */

/** definition of characteristic properties */
#define WM_GATT_CHAR_PROP_BIT_BROADCAST    (1 << 0)   /**< 0x01 */
#define WM_GATT_CHAR_PROP_BIT_READ         (1 << 1)   /**< 0x02 */
#define WM_GATT_CHAR_PROP_BIT_WRITE_NR     (1 << 2)   /**< 0x04 */
#define WM_GATT_CHAR_PROP_BIT_WRITE        (1 << 3)   /**< 0x08 */
#define WM_GATT_CHAR_PROP_BIT_NOTIFY       (1 << 4)   /**< 0x10 */
#define WM_GATT_CHAR_PROP_BIT_INDICATE     (1 << 5)   /**< 0x20 */
#define WM_GATT_CHAR_PROP_BIT_AUTH         (1 << 6)   /**< 0x40 */
#define WM_GATT_CHAR_PROP_BIT_EXT_PROP     (1 << 7)   /**< 0x80 */

#define WM_BLE_MAX_ATTR_LEN    600



/** max client application WM BLE Client can support */
#ifndef WM_BLE_CLIENT_MAX
    #define WM_BLE_CLIENT_MAX              3
#endif

/** max server application WM BLE Server can support */
#define WM_BLE_SERVER_MAX              4
#define WM_BLE_ATTRIBUTE_MAX           50

#ifndef WM_BLE_SERVER_SECURITY
    #define WM_BLE_SERVER_SECURITY         BTA_DM_BLE_SEC_NONE
#endif

#define WM_BLE_INVALID_IF              0xFF
#define WM_BLE_INVALID_CONN            0xFFFF

#define WM_BLE_GATT_TRANSPORT_LE                           0x02
#define WM_BLE_GATT_TRANSPORT_BR_EDR                       0x01
#define WM_BLE_GATT_TRANSPORT_LE_BR_EDR                    0x03

#define WM_BLE_MAX_PDU_LENGTH                              251

/** BLE events */
typedef enum
{
    /** BLE Client events */
    WM_BLE_CL_REGISTER_EVT,      /**< BLE client is registered. */
    WM_BLE_CL_DEREGISTER_EVT,    /**< BLE client is deregistered. */
    WM_BLE_CL_READ_CHAR_EVT,
    WM_BLE_CL_WRITE_CHAR_EVT,
    WM_BLE_CL_PREP_WRITE_EVT,
    WM_BLE_CL_EXEC_CMPL_EVT,     /**< Execute complete event */
    WM_BLE_CL_SEARCH_CMPL_EVT,   /**< GATT discovery complete event */
    WM_BLE_CL_SEARCH_RES_EVT,    /**< GATT discovery result event */
    WM_BLE_CL_READ_DESCR_EVT,
    WM_BLE_CL_WRITE_DESCR_EVT,
    WM_BLE_CL_NOTIF_EVT,         /**< GATT attribute notification event */
    WM_BLE_CL_OPEN_EVT,          /**< BLE open request status  event */
    WM_BLE_CL_CLOSE_EVT,         /**< GATTC  close request status event */
    WM_BLE_CL_LISTEN_EVT,
    WM_BLE_CL_CFG_MTU_EVT,       /**< configure MTU complete event */
    WM_BLE_CL_CONGEST_EVT,       /**< GATT congestion/uncongestion event */
    WM_BLE_CL_REPORT_DB_EVT,
    WM_BLE_CL_REG_NOTIFY_EVT,
    WM_BLE_CL_DEREG_NOTIFY_EVT,


    /** BLE Server events */
    WM_BLE_SE_REGISTER_EVT,      /**< BLE Server is registered */
    WM_BLE_SE_DEREGISTER_EVT,    /**< BLE Server is deregistered */
    WM_BLE_SE_CONNECT_EVT,
    WM_BLE_SE_DISCONNECT_EVT,
    WM_BLE_SE_CREATE_EVT,        /**< Service is created */
    WM_BLE_SE_ADD_INCL_SRVC_EVT,
    WM_BLE_SE_ADD_CHAR_EVT,       /**< char data is added */
    WM_BLE_SE_ADD_CHAR_DESCR_EVT,
    WM_BLE_SE_START_EVT,         /**< Service is started */
    WM_BLE_SE_STOP_EVT,          /**< Service is stopped */
    WM_BLE_SE_DELETE_EVT,
    WM_BLE_SE_READ_EVT,          /**< Read request from client */
    WM_BLE_SE_WRITE_EVT,         /**< Write request from client */
    WM_BLE_SE_EXEC_WRITE_EVT,    /**< Execute Write request from client */
    WM_BLE_SE_CONFIRM_EVT,       /**< Confirm event */
    WM_BLE_SE_RESP_EVT,
    WM_BLE_SE_CONGEST_EVT,       /**< Congestion event */
    WM_BLE_SE_MTU_EVT,

} tls_ble_evt_t;

/* WM BLE Client Host callback events */
/* Client callback function events */

/** Bluetooth 128-bit UUID */
typedef struct
{
    uint8_t uu[16];
} tls_bt_uuid_t;

/* callback event data for WM_BLE_CL_REGISTER_EVT/ event */
typedef struct
{
    uint8_t         status; /**< operation status */
    uint8_t         client_if; /**< Client interface ID */
    tls_bt_uuid_t          app_uuid;  /**< Client uuid*/

} tls_ble_cl_register_msg_t;

/** callback event data for WM_BLE_CL_READ_CHAR_EVT /WM_BLE_CL_READ_CHAR_EVTevent */
typedef struct
{
    uint16_t              conn_id;
    uint8_t         status;
    uint16_t              handle;
    uint16_t              len;
    uint8_t               *value;
    uint16_t              value_type;
} tls_ble_cl_read_msg_t;

/** callback event data for WM_BLE_CL_WRITE_CHAR_EVT/WM_BLE_CL_PREP_WRITE_EVT/WM_BLE_CL_WRITE_DESCR_EVT event */
typedef struct
{
    uint16_t              conn_id;
    uint8_t         status;
    uint16_t            handle;
} tls_ble_cl_write_msg_t;

/** callback event data for WM_BLE_CL_EXEC_CMPL_EVT event */
typedef struct
{
    uint16_t              conn_id;
    uint8_t         status;
} tls_ble_cl_exec_cmpl_msg_t;

/** callback event data for WM_BLE_CL_SEARCH_CMPL_EVT event */
typedef struct
{
    uint16_t              conn_id;
    uint8_t         status; /**< operation status */
} tls_ble_cl_search_cmpl_msg_t;

/** callback event data for WM_BLE_CL_SEARCH_RES_EVT event */
typedef struct
{
    uint16_t              conn_id;
    tls_bt_uuid_t             uuid;
    uint8_t               inst_id;
} tls_ble_cl_search_res_msg_t;

/** callback event data for WM_BLE_CL_NOTIF_EVT event */
typedef struct
{
    uint16_t              conn_id;
    uint8_t               *value;
    uint8_t             bda[6];
    uint16_t            handle;
    uint16_t              len;
    bool             is_notify;
} tls_ble_cl_notify_msg_t;

/** callback event data for WM_BLE_CL_OPEN_EVT event */
typedef struct
{
    uint16_t              conn_id;
    uint8_t         status; /**< operation status */
    uint8_t         client_if; /**< Client interface ID */
    uint8_t             bd_addr[6];
} tls_ble_cl_open_msg_t;

/** callback event data for WM_BLE_CL_CLOSE_EVT event */
typedef struct
{
    uint16_t              conn_id;
    uint8_t         status;
    uint8_t         client_if;
    uint8_t             remote_bda[6];
    uint16_t     reason;         /**< disconnect reason code, not useful when connect event is reported */
} tls_ble_cl_close_msg_t;

/** callback event data for WM_BLE_CL_LISTEN_EVT event */
typedef struct
{
    uint8_t         status;
    uint8_t        client_if;
} tls_ble_cl_listen_msg_t;

/** callback event data for WM_BLE_CL_CFG_MTU_EVT event */
typedef struct
{
    uint16_t conn_id;
    uint8_t         status;
    uint16_t mtu;
} tls_ble_cl_cfg_mtu_msg_t;

typedef struct
{
    uint16_t              conn_id;
    bool             congested; /**< congestion indicator */
} tls_ble_cl_congest_msg_t;

typedef enum
{
    WM_BTGATT_DB_PRIMARY_SERVICE,
    WM_BTGATT_DB_SECONDARY_SERVICE,
    WM_BTGATT_DB_INCLUDED_SERVICE,
    WM_BTGATT_DB_CHARACTERISTIC,
    WM_BTGATT_DB_DESCRIPTOR,
} tls_bt_gatt_db_attribute_type_t;

typedef struct
{
    uint16_t             id;
    tls_bt_uuid_t           uuid;
    tls_bt_gatt_db_attribute_type_t type;
    uint16_t            attribute_handle;

    /*
     * If |type| is |BTGATT_DB_PRIMARY_SERVICE|, or
     * |BTGATT_DB_SECONDARY_SERVICE|, this contains the start and end attribute
     * handles.
     */
    uint16_t            start_handle;
    uint16_t            end_handle;

    /*
     * If |type| is |BTGATT_DB_CHARACTERISTIC|, this contains the properties of
     * the characteristic.
     */
    uint8_t             properties;
} tls_btgatt_db_element_t;

typedef struct
{
	uint16_t conn_id;
	tls_btgatt_db_element_t *db;
	int count;
	uint8_t status;
} tls_ble_cl_gatt_db_msg_t;

typedef struct
{
	uint16_t conn_id;
	uint8_t status;
	bool reg;
	uint16_t handle;
	
} tls_ble_cl_reg_notify_msg_t;


/* WM BLE Server Host callback events */
/* Server callback function events */

/** callback event data for WM_BLE_SE_REGISTER_EVT/WM_BLE_SE_DEREGISTER_EVT event */
typedef struct
{
    uint8_t         status; /* operation status */
    uint8_t         server_if; /* Server interface ID */
    tls_bt_uuid_t app_uuid;
} tls_ble_se_register_msg_t;

/** callback event data for WM_BLE_SE_CONNECT_EVT/WM_BLE_SE_DISCONNECT_EVT event */
typedef struct
{
    uint16_t conn_id;
    uint8_t         server_if; /**< Server interface ID */
    bool connected;
    uint8_t addr[6];
} tls_ble_se_connect_msg_t;

typedef tls_ble_se_connect_msg_t tls_ble_se_disconnect_msg_t;

/** callback event data for WM_BLE_SE_CREATE_EVT event */
typedef struct
{
    uint8_t         status; /**< operation status */
    uint8_t         server_if;
    bool is_primary;
    uint8_t inst_id;
    tls_bt_uuid_t uuid;
    uint16_t              service_id;
} tls_ble_se_create_msg_t;

/** callback event data for WM_BLE_SE_ADD_INCL_SRVC_EVT event */
typedef struct
{
    uint8_t         status; /**< operation status */
    uint8_t         server_if;
    uint16_t           service_id;
    uint16_t           attr_id;
} tls_ble_se_add_incl_srvc_msg_t;

/** callback event data for WM_BLE_SE_ADDCHAR_EVT event */
typedef struct
{
    uint8_t         status; /**< operation status */
    uint8_t         server_if;
    tls_bt_uuid_t uuid;
    uint16_t              service_id;
    uint16_t              attr_id;
} tls_ble_se_add_char_msg_t;

typedef tls_ble_se_add_char_msg_t tls_ble_se_add_char_descr_msg_t;



/** callback event data for WM_BLE_SE_START_EVT event */
typedef struct
{
    uint8_t         status; /**< operation status */
    uint8_t         server_if;
    uint16_t              service_id;
} tls_ble_se_start_msg_t;

typedef tls_ble_se_start_msg_t tls_ble_se_stop_msg_t;

typedef tls_ble_se_start_msg_t tls_ble_se_delete_msg_t;

typedef struct
{
    uint16_t        conn_id;
    uint32_t        trans_id;
    uint8_t       remote_bda[6];
    uint16_t        handle;
    uint16_t        offset;
    bool       is_long;
} tls_ble_se_read_msg_t;

typedef struct
{
    uint16_t              conn_id;
    uint32_t              trans_id;
    uint8_t             remote_bda[6];
    uint16_t              handle;     /**< attribute handle */
    uint16_t              offset;    /**< attribute value offset, if no offset is needed for the command, ignore it */
    uint16_t              len;        /**< length of attribute value */
    bool             need_rsp;   /**< need write response */
    bool             is_prep;    /**< is prepare write */
    uint8_t               *value;  /**< the actual attribute value */
} tls_ble_se_write_msg_t;

typedef struct
{
    uint16_t              conn_id;
    uint32_t              trans_id;
    uint8_t             remote_bda[6];
    uint8_t exec_write;
} tls_ble_se_exec_write_msg_t;

typedef struct
{
    uint16_t              conn_id;
    uint8_t         status; /**< operation status */

} tls_ble_se_confirm_msg_t;

typedef struct
{
    
    uint8_t         status; /* operation status */
	uint16_t        conn_id;
	uint16_t        trans_id;

} tls_ble_se_response_msg_t;

typedef struct
{
    uint16_t              conn_id;
    bool             congested; /**< congestion indicator */
} tls_ble_se_congest_msg_t;

typedef struct
{
    uint16_t              conn_id;
    uint16_t            mtu;
} tls_ble_se_mtu_msg_t;


/** Union of data associated with HD callback */
typedef union
{
    tls_ble_cl_register_msg_t    cli_register;      /**< WM_BLE_CL_REGISTER_EVT */
    tls_ble_cl_read_msg_t        cli_read;          /**< WM_BLE_CL_READ_EVT */
    tls_ble_cl_write_msg_t       cli_write;         /**< WM_BLE_CL_WRITE_EVT */
    tls_ble_cl_exec_cmpl_msg_t   cli_exec_cmpl;     /**< WM_BLE_CL_EXEC_CMPL_EVT */
    tls_ble_cl_search_cmpl_msg_t cli_search_cmpl;   /**< WM_BLE_CL_SEARCH_CMPL_EVT */
    tls_ble_cl_search_res_msg_t  cli_search_res;    /**< WM_BLE_CL_SEARCH_RES_EVT */
    tls_ble_cl_notify_msg_t      cli_notif;         /**< WM_BLE_CL_NOTIF_EVT */
    tls_ble_cl_open_msg_t        cli_open;          /**< WM_BLE_CL_OPEN_EVT */
    tls_ble_cl_close_msg_t       cli_close;         /**< WM_BLE_CL_CLOSE_EVT */
    tls_ble_cl_listen_msg_t      cli_listen;        /**< WM_BLE_CL_LISTEN_EVT */
    tls_ble_cl_cfg_mtu_msg_t     cli_cfg_mtu;       /**< WM_BLE_CL_CFG_MTU_EVT */
    tls_ble_cl_congest_msg_t     cli_congest;       /**< WM_BLE_CL_CONGEST_EVT */
	tls_ble_cl_gatt_db_msg_t     cli_db;            /* WM_BLE_CL_REPORT_DB_EVT*/
	tls_ble_cl_reg_notify_msg_t  cli_reg_notify;


    tls_ble_se_register_msg_t    ser_register;     /**< WM_BLE_SE_REGISTER_EVT */
    tls_ble_se_connect_msg_t     ser_connect;      /**< WM_BLE_SE_OPEN_EVT */
    tls_ble_se_disconnect_msg_t  ser_disconnect;   /**< WM_BLE_SE_CLOSE_EVT */
    tls_ble_se_create_msg_t      ser_create;       /**< WM_BLE_SE_CREATE_EVT */
    tls_ble_se_add_incl_srvc_msg_t ser_add_incl_srvc;
    tls_ble_se_add_char_msg_t    ser_add_char;     /**< WM_BLE_SE_ADDCHAR_EVT */
    tls_ble_se_add_char_descr_msg_t ser_add_char_descr;
    tls_ble_se_start_msg_t       ser_start_srvc;   /**< WM_BLE_SE_START_EVT */
    tls_ble_se_stop_msg_t        ser_stop_srvc;    /**< WM_BLE_SE_STOP_EVT */
    tls_ble_se_delete_msg_t      ser_delete_srvc;
    tls_ble_se_read_msg_t        ser_read;         /**< WM_BLE_SE_READ_EVT */
    tls_ble_se_write_msg_t       ser_write;        /**< WM_BLE_SE_WRITE_EVT */
    tls_ble_se_exec_write_msg_t  ser_exec_write;   /**< WM_BLE_SE_EXEC_WRITE_EVT */
    tls_ble_se_confirm_msg_t     ser_confirm;      /**< WM_BLE_SE_CONFIRM_EVT */
    tls_ble_se_congest_msg_t     ser_congest;      /**< WM_BLE_CL_CONGEST_EVT */
    tls_ble_se_mtu_msg_t         ser_mtu;
	tls_ble_se_response_msg_t    ser_resp;

} tls_ble_msg_t;

/** WM BLE Client callback function */
typedef void (*tls_ble_callback_t)(tls_ble_evt_t event, tls_ble_msg_t *p_data);



/** BLE dm events */
typedef enum
{
    WM_BLE_DM_SET_ADV_DATA_CMPL_EVT,    /**< BLE DM set advertisement data completed*/
    WM_BLE_DM_TIMER_EXPIRED_EVT,        /**< BLE DM timer expired event. */
    WM_BLE_DM_TRIGER_EVT,               /**< BLE DM event trigered event, async processing*/
    WM_BLE_DM_SCAN_RES_EVT,             /**< BLE DM scan result evt*/
	WM_BLE_DM_SET_SCAN_PARAM_CMPL_EVT,
} tls_ble_dm_evt_t;


/** callback event data for WM_BLE_DM_SET_ADV_DATA */
typedef struct
{
    uint8_t         status; /**< operation status */
} tls_ble_dm_set_adv_data_cmpl_msg_t;

typedef struct
{
	uint8_t status;
	uint8_t dm_id; //dummy value; who care this value;
} tls_ble_dm_set_scan_param_cmpl_msg_t;

typedef struct
{
    uint8_t id;
    int32_t func_ptr;
} tls_ble_dm_timer_expired_msg_t;

typedef tls_ble_dm_timer_expired_msg_t tls_ble_dm_evt_triger_msg_t;

typedef struct
{
    uint8_t address[6];                    /**< device address */
    int8_t rssi;                        /**< signal strength */
    uint8_t *value; /**< adv /scan resp value */
} tls_ble_dm_scan_res_msg_t;

typedef union
{
    tls_ble_dm_set_adv_data_cmpl_msg_t   dm_set_adv_data_cmpl;
    tls_ble_dm_timer_expired_msg_t       dm_timer_expired;
    tls_ble_dm_evt_triger_msg_t          dm_evt_trigered;
    tls_ble_dm_scan_res_msg_t            dm_scan_result;
	tls_ble_dm_set_scan_param_cmpl_msg_t dm_set_scan_param_cmpl;
} tls_ble_dm_msg_t;

typedef struct
{
    bool set_scan_rsp;
    bool include_name;
    bool include_txpower;
    int min_interval;
    int max_interval;
    int appearance;
    uint16_t manufacturer_len;
    uint8_t manufacturer_data[31];
    uint16_t service_data_len;
    uint8_t service_data[31];
    uint16_t service_uuid_len;
    uint8_t service_uuid[31];
} __attribute__((packed)) tls_ble_dm_adv_data_t;

typedef struct
{
    uint16_t      adv_int_min;            /* minimum adv interval */
    uint16_t      adv_int_max;            /* maximum adv interval */
    tls_bt_addr_t   *dir_addr;
} __attribute__((packed)) tls_ble_dm_adv_param_t;

/** WM BLE device manager callback function */
typedef void (*tls_ble_dm_callback_t)(tls_ble_dm_evt_t event, tls_ble_dm_msg_t *p_data);

/** WM BLE dm timer callback function */
typedef void (*tls_ble_dm_timer_callback_t)(uint8_t timer_id);

/** WM BLE device evt triger callback function */
typedef void (*tls_ble_dm_triger_callback_t)(int32_t evt_id);

typedef void (*tls_ble_scan_res_notify_t)(tls_ble_dm_scan_res_msg_t *msg);


#define TLS_HAL_CBACK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        assert(0);  \
    }


#endif /* WM_BT_DEF_H */

