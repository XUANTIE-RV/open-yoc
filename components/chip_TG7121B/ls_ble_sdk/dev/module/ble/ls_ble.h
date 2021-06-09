#ifndef LS_BLE_H_
#define LS_BLE_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#define INVALID_CON_IDX (0xff)
#define INVALID_PEER_ID (0xff)

#define BLE_ADDR_LEN 6
#define BLE_KEY_LEN 16

#define DEV_NAME_MAX_LEN 0x20

/**defgroup BLE_GAP_IO_CAPS GAP IO Capabilities**/
#define BLE_GAP_IO_CAPS_DISPLAY_ONLY      0x0   /**< Display Only. */
#define BLE_GAP_IO_CAPS_DISPLAY_YESNO     0x1   /**< Display and Yes/No entry. */
#define BLE_GAP_IO_CAPS_KEYBOARD_ONLY     0x2   /**< Keyboard Only. */
#define BLE_GAP_IO_CAPS_NONE              0x3   /**< No I/O capabilities. */
#define BLE_GAP_IO_CAPS_KEYBOARD_DISPLAY  0x4   /**< Keyboard and Display. */

/**defgroup SEC_AUTH_FLAG  SEC Auth Flag**/
#define AUTH_NONE               0                 /**< No auth requirement. */
#define AUTH_BOND              (1 << 0)           /**< Bond flag. */
#define AUTH_MITM              (1 << 2)           /**< MITM flag. */
#define AUTH_SEC_CON           (1 << 3)           /**< Security connection flag. */
#define AUTH_KEY_PRESS_NOTIFY  (1 << 4)           /**< Key press notify flag. */
#define AUTH_ALL               (AUTH_BOND | AUTH_MITM | AUTH_SEC_CON | AUTH_KEY_PRESS_NOTIFY)  /**< All authentication flags are on. */

/**defgroup SEC_KEY_DIST_FLAG  SEC Key Distribution Flag**/
#define KDIST_NONE      0            /**< No key needs to be distributed. */
#define KDIST_ENCKEY   (1 << 0)      /**< Distribute encryption and master identification info. */
#define KDIST_IDKEY    (1 << 1)      /**< Distribute identity and address info. */
#define KDIST_SIGNKEY  (1 << 2)      /**< Distribute signing info. */
#define KDIST_ALL      (KDIST_ENCKEY | KDIST_IDKEY | KDIST_SIGNKEY)  /**< Distribute all info. */

/**defgroup SEC mode and level**/
#define SEC_MODE1_LEVEL1  0x00    /**< No security is needed. */
#define SEC_MODE1_LEVEL2  0x01    /**< Encrypted link is required. Unnecessary: MITM and SC. */
#define SEC_MODE1_LEVEL3  0x02    /**< Encrypted link is required. Necessary: MITM; unnecessary: SC. */
#define SEC_MODE1_LEVEL4  0x03    /**< Encrypted link is required. Necessary: MITM and SC. */
#define SEC_MODE2_LEVEL1  0x04    /**< Data signing is required. Unnecessary: MITM and SC. */
#define SEC_MODE2_LEVEL2  0x05    /**< Data signing is required. Necessary: MITM; unnecessary: SC. */

/**defgroup SEC Parameter**/
#define OOB_DATA_FLAG                     0x0                               /**< Indicate whether OOB is supported. */
#define AUTHREQ                           (AUTH_MITM | AUTH_BOND)           /**< Set the auth. */
#define KEY_SIZE                          0x10                              /**< Indicate the supported maximum LTK size (range: 7-16). */
#define INIT_KEY_DIST                     (KDIST_ENCKEY | KDIST_IDKEY)      /**< Set the initial key distribution. */
#define RESP_KEY_DIST                     (KDIST_ENCKEY | KDIST_IDKEY)      /**< Set the response key distribution. */
#define passkey_number                    {'1','2','3','4','5','6'}         /**< Set the passkey,size(6). */

enum gap_own_addr_type
{
    PUBLIC_OR_RANDOM_STATIC_ADDR,
    RESOLVABLE_PRIVATE_ADDR,
    NON_RESOLVABLE_PRIVATE_ADDR,
};

enum gap_peer_addr_type
{
    PUBLIC_ADDR,
    RANDOM_ADDR,
};

enum gap_adv_type
{
    /// Flag
    GAP_ADV_TYPE_FLAGS                      = 0x01,//!< GAP_ADV_TYPE_FLAGS
    /// Use of more than 16 bits UUID
    GAP_ADV_TYPE_MORE_16_BIT_UUID           = 0x02,//!< GAP_ADV_TYPE_MORE_16_BIT_UUID
    /// Complete list of 16 bit UUID
    GAP_ADV_TYPE_COMPLETE_LIST_16_BIT_UUID  = 0x03,//!< GAP_ADV_TYPE_COMPLETE_LIST_16_BIT_UUID
    /// Use of more than 32 bit UUD
    GAP_ADV_TYPE_MORE_32_BIT_UUID           = 0x04,//!< GAP_ADV_TYPE_MORE_32_BIT_UUID
    /// Complete list of 32 bit UUID
    GAP_ADV_TYPE_COMPLETE_LIST_32_BIT_UUID  = 0x05,//!< GAP_ADV_TYPE_COMPLETE_LIST_32_BIT_UUID
    /// Use of more than 128 bit UUID
    GAP_ADV_TYPE_MORE_128_BIT_UUID          = 0x06,//!< GAP_ADV_TYPE_MORE_128_BIT_UUID
    /// Complete list of 128 bit UUID
    GAP_ADV_TYPE_COMPLETE_LIST_128_BIT_UUID = 0x07,//!< GAP_ADV_TYPE_COMPLETE_LIST_128_BIT_UUID
    /// Shortened device name
    GAP_ADV_TYPE_SHORTENED_NAME             = 0x08,//!< GAP_ADV_TYPE_SHORTENED_NAME
    /// Complete device name
    GAP_ADV_TYPE_COMPLETE_NAME              = 0x09,//!< GAP_ADV_TYPE_COMPLETE_NAME
    /// Transmit power
    GAP_ADV_TYPE_TRANSMIT_POWER             = 0x0A,//!< GAP_ADV_TYPE_TRANSMIT_POWER
    /// Class of device
    GAP_ADV_TYPE_CLASS_OF_DEVICE            = 0x0D,//!< GAP_ADV_TYPE_CLASS_OF_DEVICE
    /// Simple Pairing Hash C
    GAP_ADV_TYPE_SP_HASH_C                  = 0x0E,//!< GAP_ADV_TYPE_SP_HASH_C
    /// Simple Pairing Randomizer
    GAP_ADV_TYPE_SP_RANDOMIZER_R            = 0x0F,//!< GAP_ADV_TYPE_SP_RANDOMIZER_R
    /// Temporary key value
    GAP_ADV_TYPE_TK_VALUE                   = 0x10,//!< GAP_ADV_TYPE_TK_VALUE
    /// Out of Band Flag
    GAP_ADV_TYPE_OOB_FLAGS                  = 0x11,//!< GAP_ADV_TYPE_OOB_FLAGS
    /// Slave connection interval range
    GAP_ADV_TYPE_SLAVE_CONN_INT_RANGE       = 0x12,//!< GAP_ADV_TYPE_SLAVE_CONN_INT_RANGE
    /// Require 16 bit service UUID
    GAP_ADV_TYPE_RQRD_16_BIT_SVC_UUID       = 0x14,//!< GAP_ADV_TYPE_RQRD_16_BIT_SVC_UUID
    /// Require 32 bit service UUID
    GAP_ADV_TYPE_RQRD_32_BIT_SVC_UUID       = 0x1F,//!< GAP_ADV_TYPE_RQRD_32_BIT_SVC_UUID
    /// Require 128 bit service UUID
    GAP_ADV_TYPE_RQRD_128_BIT_SVC_UUID      = 0x15,//!< GAP_ADV_TYPE_RQRD_128_BIT_SVC_UUID
    /// Service data 16-bit UUID
    GAP_ADV_TYPE_SERVICE_16_BIT_DATA        = 0x16,//!< GAP_ADV_TYPE_SERVICE_16_BIT_DATA
    /// Service data 32-bit UUID
    GAP_ADV_TYPE_SERVICE_32_BIT_DATA        = 0x20,//!< GAP_ADV_TYPE_SERVICE_32_BIT_DATA
    /// Service data 128-bit UUID
    GAP_ADV_TYPE_SERVICE_128_BIT_DATA       = 0x21,//!< GAP_ADV_TYPE_SERVICE_128_BIT_DATA
    /// Public Target Address
    GAP_ADV_TYPE_PUB_TGT_ADDR               = 0x17,//!< GAP_ADV_TYPE_PUB_TGT_ADDR
    /// Random Target Address
    GAP_ADV_TYPE_RAND_TGT_ADDR              = 0x18,//!< GAP_ADV_TYPE_RAND_TGT_ADDR
    /// Appearance
    GAP_ADV_TYPE_APPEARANCE                 = 0x19,//!< GAP_ADV_TYPE_APPEARANCE
    /// Advertising Interval
    GAP_ADV_TYPE_ADV_INTV                   = 0x1A,//!< GAP_ADV_TYPE_ADV_INTV
    /// LE Bluetooth Device Address
    GAP_ADV_TYPE_LE_BT_ADDR                 = 0x1B,//!< GAP_ADV_TYPE_LE_BT_ADDR
    /// LE Role
    GAP_ADV_TYPE_LE_ROLE                    = 0x1C,//!< GAP_ADV_TYPE_LE_ROLE
    /// Simple Pairing Hash C-256
    GAP_ADV_TYPE_SPAIR_HASH                 = 0x1D,//!< GAP_ADV_TYPE_SPAIR_HASH
    /// Simple Pairing Randomizer R-256
    GAP_ADV_TYPE_SPAIR_RAND                 = 0x1E,//!< GAP_ADV_TYPE_SPAIR_RAND
    /// 3D Information Data
    GAP_ADV_TYPE_3D_INFO                    = 0x3D,//!< GAP_ADV_TYPE_3D_INFO

    /// Manufacturer specific data
    GAP_ADV_TYPE_MANU_SPECIFIC_DATA         = 0xFF,//!< GAP_ADV_TYPE_MANU_SPECIFIC_DATA
};

struct dev_addr
{
    uint8_t addr[BLE_ADDR_LEN];
};

struct ble_addr
{
    struct dev_addr addr;
    uint8_t type;
};

enum sec_lvl_type
{
    NO_SEC,
    UNAUTH_SEC,
    AUTH_SEC,
    SEC_CON_SEC,
};

struct legacy_adv_prop
{
    uint8_t connectable:1,
            scannable:1,
            directed:1,
            high_duty_cycle:1;
};

enum adv_disc_mode
{
    /// Mode in non-discoverable
    ADV_MODE_NON_DISC = 0,
    /// Mode in general discoverable
    ADV_MODE_GEN_DISC,
    /// Mode in limited discoverable
    ADV_MODE_LIM_DISC,
    /// Broadcast mode without presence of AD_TYPE_FLAG in advertising data
    ADV_MODE_BEACON,
    ADV_MODE_MAX,
};

struct legacy_adv_obj_param
{
    struct dev_addr *peer_addr;
    uint16_t adv_intv_min;
    uint16_t adv_intv_max;
    enum gap_own_addr_type own_addr_type;
    enum gap_peer_addr_type peer_addr_type;
    uint8_t filter_policy;
    uint8_t ch_map;
    enum adv_disc_mode disc_mode;
    struct legacy_adv_prop prop;
};

enum scan_type
{
    GENERAL_DISCOVERABLE,
    LIMITED_DISCOVERABLE,
    OBSERVER,
    OBSERVER_WHITELIST,
    CONNECTABLE,
    CONNECTABLE_WHITELIST,
};

struct start_scan_param
{
    uint16_t scan_intv;
    uint16_t scan_window;
    uint16_t duration;
    uint16_t period;
    enum scan_type type;
    uint8_t active:1,
            filter_duplicates:2;
};

enum init_type
{
    DIRECT_CONNECTION,
    AUTO_CONNECTION_WHITELIST,
};

struct start_init_param
{
    struct dev_addr *peer_addr;
    uint16_t scan_intv;
    uint16_t scan_window;
    /// Timeout for automatic connection establishment (in unit of 10ms). Cancel the procedure if not all
    /// indicated devices have been connected when the timeout occurs. 0 means there is no timeout
    uint16_t conn_to;
    /// Minimum value for the connection interval (in unit of 1.25ms). Shall be less than or equal to
    /// conn_intv_max value. Allowed range is 7.5ms to 4s.
    uint16_t conn_intv_min;
    /// Maximum value for the connection interval (in unit of 1.25ms). Shall be greater than or equal to
    /// conn_intv_min value. Allowed range is 7.5ms to 4s.
    uint16_t conn_intv_max;
    /// Slave latency. Number of events that can be missed by a connected slave device
    uint16_t conn_latency;
    /// Link supervision timeout (in unit of 10ms). Allowed range is 100ms to 32s
    uint16_t supervision_to;
    uint8_t peer_addr_type;
    enum init_type type;
};


enum dev_evt_type
{
    STACK_INIT,
    STACK_READY,
    PROFILE_ADDED,
    SERVICE_ADDED,
    ADV_OBJ_CREATED,
    SCAN_OBJ_CREATED,
    INIT_OBJ_CREATED,
    ADV_STOPPED,
    SCAN_STOPPED,
    INIT_STOPPED,
    OBJ_DELETED,
    ADV_REPORT,
};

enum prf_id
{
    PRF_DIS_SERVER,
    PRF_MESH,
    PRF_FOTA_SERVER,
    PRF_HID,
    PRF_BASS,
};

struct profile_added_evt
{
    uint16_t start_hdl;
    enum prf_id id;
    
};

struct service_added_evt
{
    uint16_t start_hdl;
    uint8_t status;
};

struct obj_created_evt
{
    uint8_t handle;
    uint8_t status;
};

struct stopped_evt
{
    uint8_t handle;
    uint8_t reason;
};

struct obj_deleted_evt
{
    uint8_t handle;
    uint8_t status;
};

struct adv_report_info
{
    uint8_t evt_type:3,
            complete:1,
            connectable:1,
            scannable:1,
            directed:1;    
};

struct adv_report_evt
{
    uint8_t *data;
    struct dev_addr *adv_addr;
    uint16_t length;
    uint8_t adv_addr_type;
    int8_t rssi;
    struct adv_report_info info;
};

union dev_evt_u
{
    struct profile_added_evt profile_added;
    struct service_added_evt service_added;
    struct obj_created_evt obj_created;
    struct stopped_evt stopped;
    struct obj_deleted_evt deleted;
    struct adv_report_evt adv_report;
};

struct ble_stack_cfg
{
    bool private_addr;
    bool controller_privacy;
};

enum uuid_length
{
    UUID_LEN_16BIT = 0,
    UUID_LEN_32BIT,
    UUID_LEN_128BIT,
};

struct char_properties
{
    uint8_t broadcast:1,
            rd_en:1,
            wr_cmd:1,
            wr_req:1,
            ntf_en:1,
            ind_en:1,            
            wr_signed:1,
            ext_prop:1;
};

struct char_permissions
{
    uint8_t rd_perm:2,
            wr_perm:2,
            ind_perm:2,
            ntf_perm:2;
};

struct att_decl
{
    const uint8_t *uuid;
    struct{
        uint16_t max_len:12,
                eks:1,
                uuid_len:2,
                read_indication:1;        
    }s;
    struct char_permissions char_perm;
    struct char_properties char_prop; 
};

struct svc_decl
{
    const uint8_t *uuid;
    struct att_decl *att;
    uint8_t nb_att;
    uint8_t    sec_lvl:2,
            uuid_len:2,
            secondary:1;
};

enum gap_evt_type
{
    CONNECTED,
    DISCONNECTED,
    CONN_PARAM_REQ,
    CONN_PARAM_UPDATED,
    MASTER_PAIR_REQ,
    SLAVE_SECURITY_REQ,
    PAIR_DONE,
    ENCRYPT_FAIL,
    ENCRYPT_DONE,
    DISPLAY_PASSKEY,
    REQUEST_PASSKEY,
    NUMERIC_COMPARE,
    REQUEST_LEGACY_OOB,
    REQUEST_SC_OOB,
    GET_DEV_INFO_DEV_NAME,
    GET_DEV_INFO_APPEARANCE,
    GET_DEV_INFO_SLV_PRE_PARAM,
};
enum
{
    LS_BLE_ROLE_MASTER,
    LS_BLE_ROLE_SLAVE,
};
struct gap_conn_param
{
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
};

struct gap_conn_param_req
{
    struct gap_conn_param const *conn_param;
    bool *accept;
};

struct gap_conn_param_updated
{
    ///Connection interval value
    uint16_t            con_interval;
    ///Connection latency value
    uint16_t            con_latency;
    ///Supervision timeout
    uint16_t            sup_to;
};

struct gap_sc_oob
{
    /// Confirm Value
    uint8_t conf[BLE_KEY_LEN];
    /// Random Number
    uint8_t rand[BLE_KEY_LEN];
};

struct pair_feature
{
    uint8_t iocap;
    uint8_t oob;
    uint8_t auth;
    uint8_t key_size;
    uint8_t ikey_dist;
    uint8_t rkey_dist;
};

struct gap_connected
{
    uint16_t con_interval;
    uint16_t con_latency;
    uint16_t sup_to;
    uint8_t peer_id;
};

struct gap_disconnected
{
    uint8_t reason;
};

struct gap_master_pair_req
{
    uint8_t auth;
};

struct gap_slave_security_req
{
    uint8_t auth;
};

struct gap_pair_done
{
    bool succeed;
    union{
        uint8_t auth;
        uint8_t fail_reason;
    }u;
};

struct gap_encrypt_fail
{
    uint8_t reason;
};

struct gap_encrypt_done
{
    uint8_t auth;
};

struct gap_pin_str
{
    char pin[6];
    char str_pad;
};

struct gap_display_passkey
{
    struct gap_pin_str passkey;
};

struct gap_numeric_compare
{
    struct gap_pin_str number;
};

struct gap_dev_info_dev_name
{
    uint16_t length;
    uint8_t* value;
};

struct gap_dev_info_appearance
{
    uint16_t appearance;
};

struct gap_dev_info_slave_pref_param
{
    uint16_t con_intv_min;
    uint16_t con_intv_max;
    uint16_t slave_latency;
    uint16_t conn_timeout;
};

union gap_evt_u
{
    struct gap_connected connected;
    struct gap_disconnected disconnected;
    struct gap_conn_param_req conn_param_req;
    struct gap_conn_param_updated conn_param_updated;
    struct gap_master_pair_req master_pair_req;
    struct gap_slave_security_req slave_security_req;
    struct gap_pair_done pair_done;
    struct gap_encrypt_fail encrypt_fail;
    struct gap_encrypt_done encrypt_done;
    struct gap_display_passkey display_passkey;
    struct gap_numeric_compare numeric_compare;
    struct gap_dev_info_dev_name get_dev_name;
    struct gap_dev_info_appearance get_appearance;
    struct gap_dev_info_slave_pref_param slv_pref_param;
};

struct gap_update_conn_param
{
    uint16_t intv_min;
    uint16_t intv_max;
    uint16_t latency;
    uint16_t sup_timeout;
    uint16_t ce_len_min;
    uint16_t ce_len_max;
};

struct gap_set_pkt_size
{
    uint16_t pkt_size;
};

enum gatt_evt_type
{
    SERVER_READ_REQ = 0,
    SERVER_WRITE_REQ,
    SERVER_NOTIFICATION_DONE,
    SERVER_INDICATION_DONE,

    CLIENT_RECV_NOTIFICATION,
    CLIENT_RECV_INDICATION,
    CLIENT_PRIMARY_SVC_DIS_IND,       // Primary service discovery indication
    CLIENT_INCL_SVC_DIS_IND,          // Included service discovery indication
    CLIENT_CHAR_DIS_BY_UUID_IND,      // Characteristic discovery by UUID indication 
    CLIENT_CHAR_DESC_DIS_BY_UUID_IND, // Characteristic descriptor discovery by UUID indication
    CLIENT_RD_CHAR_VAL_BY_UUID_IND,   // Read characteristic value by UUID indication
    CLIENT_WRITE_WITH_RSP_DONE,             // Write response indication
    CLIENT_WRITE_NO_RSP_DONE,          // Write with no response indication

    MTU_CHANGED_INDICATION,
    GATT_EVT_MAX
};

enum svc_set_value_status
{
    SVC_SET_VAL_NO_ERROR,
    SVC_SET_VAL_NOT_SUPPORTED,
    SVC_SET_VAL_INVALID_HANDLE,
    SVC_SET_VAL_INVALID_OFFSET,
    SVC_SET_VAL_INVALID_LENGTH,
};

enum svc_get_value_status
{
    SVC_GET_VAL_NO_ERROR,
    SVC_GET_VAL_NOT_SUPPORTED,
    SVC_GET_VAL_INVALID_HANDLE,
    SVC_GET_VAL_APP_ERROR,
};

struct gatt_svc_env
{
    void *hdr;
    uint16_t start_hdl;
    uint8_t att_num;
};

struct gatt_server_read_req
{
    struct gatt_svc_env const *svc;
    uint8_t att_idx;
};

struct gatt_server_write_req
{
    struct gatt_svc_env const *svc;
    uint8_t const *value;
    uint8_t *return_status;
    uint16_t offset;
    uint16_t length;
    uint8_t att_idx;
};

struct gatt_server_notify_indicate_done
{
    uint16_t transaction_id;
    uint8_t status;
};

struct gatt_client_recv_notify_indicate
{
    uint16_t handle;
    uint16_t length;
    uint8_t const *value;
};

struct gatt_mtu_changed_indicate
{
    uint16_t mtu;
};

struct gatt_handle_range
{
    uint16_t begin_handle;
    uint16_t end_handle;
};

struct gatt_client_svc_disc_indicate
{
    const uint8_t *uuid;
    struct gatt_handle_range handle_range;  
    enum uuid_length uuid_len;
};

struct gatt_client_svc_disc_include_indicate
{
    const uint8_t *uuid;
    struct gatt_handle_range handle_range;
    uint16_t attr_handle;
    enum uuid_length uuid_len;
};

struct gatt_client_disc_char_indicate
{
    const uint8_t *uuid;
    uint16_t attr_handle;
    uint16_t pointer_handle;
    uint8_t properties;
    enum uuid_length uuid_len;
};

struct gatt_client_disc_char_desc_indicate
{
    const uint8_t *uuid;
    uint16_t attr_handle;
    enum uuid_length uuid_len;
};

struct gatt_read_indicate
{
    uint8_t const *value;
    uint16_t handle;
    uint16_t offset;
    uint16_t length;
};

struct gatt_write_rsp
{
    uint16_t transaction_id;
    uint8_t status;
};

struct gatt_write_no_rsp
{
    uint16_t transaction_id;
    uint8_t status;
};

union gatt_evt_u
{
    struct gatt_server_read_req server_read_req;
    struct gatt_server_write_req server_write_req;
    struct gatt_server_notify_indicate_done server_notify_indicate_done;
    struct gatt_client_recv_notify_indicate client_recv_notify_indicate;
    struct gatt_mtu_changed_indicate mtu_changed_ind;
    struct gatt_client_svc_disc_indicate client_svc_disc_indicate;
    struct gatt_client_svc_disc_include_indicate client_svc_disc_include_indicate;
    struct gatt_client_disc_char_indicate client_disc_char_indicate;
    struct gatt_client_disc_char_desc_indicate client_disc_char_desc_indicate;
    struct gatt_read_indicate client_read_indicate;
    struct gatt_write_rsp client_write_rsp;
    struct gatt_write_no_rsp client_write_no_rsp;
};

uint8_t *adv_data_pack(uint8_t *buf,uint8_t field_nums,...);
#define ADV_DATA_PACK(buf,field_nums,...) (adv_data_pack((buf),(field_nums),__VA_ARGS__) - (buf))

void ble_init(void);

void ble_loop(void);

void dev_manager_init(void (*cb)(enum dev_evt_type,union dev_evt_u *));

void dev_manager_stack_init(struct ble_stack_cfg *cfg);

void dev_manager_get_identity_bdaddr(uint8_t *addr,bool *random);

void dev_manager_add_service(struct svc_decl *svc);

uint8_t dev_manager_svc_set_value(uint16_t handle, uint16_t length, uint8_t *value);

uint8_t dev_manager_svc_get_value(uint16_t handle, uint8_t *value, uint16_t *length);

void dev_manager_create_legacy_adv_object(struct legacy_adv_obj_param *p_param);

void dev_manager_create_scan_object(enum gap_own_addr_type own_addr_type);

void dev_manager_create_init_object(enum gap_own_addr_type own_addr_type);

void dev_manager_start_adv(uint8_t adv_handle,uint8_t *adv_data,uint8_t adv_data_length,uint8_t *scan_rsp_data,uint8_t scan_rsp_data_length);

void dev_manager_set_adv_duration(uint16_t duration);

void dev_manager_update_adv_data(uint8_t adv_handle,uint8_t *adv_data,uint8_t adv_data_length,uint8_t *scan_rsp_data,uint8_t scan_rsp_data_length);

void dev_manager_stop_adv(uint8_t adv_handle);

void dev_manager_start_scan(uint8_t scan_handle, struct start_scan_param * param);

void dev_manager_stop_scan(uint8_t scan_handle);

void dev_manager_start_init(uint8_t init_handle, struct start_init_param * param);

void dev_manager_stop_init(uint8_t init_handle);

void dev_manager_delete_activity(uint8_t obj_hdl);

void dev_manager_set_mac_addr(uint8_t *addr);

uint8_t dev_manager_update_adv_interval(uint8_t adv_handle, uint32_t new_intv_min, uint32_t new_intv_max);

void gap_manager_init(void (*evt_cb)(enum gap_evt_type,union gap_evt_u *,uint8_t));

void gap_manager_disconnect(uint8_t con_idx,uint8_t reason);

void gap_manager_master_bond(uint8_t con_idx, struct pair_feature * pair_feat);

void gap_manager_master_encrypt(uint8_t con_idx);

void gap_manager_slave_security_req(uint8_t con_idx, uint8_t auth);

void gap_manager_slave_pair_response_send(uint8_t con_idx,uint8_t accept,struct pair_feature *feat);

void gap_manager_passkey_input(uint8_t con_idx,struct gap_pin_str *passkey);

void gap_manager_numeric_compare_set(uint8_t con_idx,bool equal);

void gap_manager_sc_oob_set(uint8_t con_idx,struct gap_sc_oob *sc_oob);

void gap_manager_tk_set(uint8_t con_idx,uint8_t key[BLE_KEY_LEN]);

uint8_t gap_manager_get_role(uint8_t con_idx);

uint8_t gap_manager_get_sec_lvl(uint8_t con_idx);

void gap_manager_get_peer_addr(uint8_t conidx,struct ble_addr *addr);

void gap_manager_get_identity_addr(uint8_t peer_id,struct ble_addr *addr);

void gap_manager_update_conn_param(uint8_t con_idx,struct gap_update_conn_param *p_param);

void gap_manager_set_pkt_size(uint8_t con_idx, struct gap_set_pkt_size *p_param);

void gap_manager_delete_bonding(uint8_t peer_id);

uint8_t gap_manager_get_bonding_peer_id(uint8_t link_id);

uint8_t gap_manager_get_bonded_dev_num(void);

void gatt_manager_init(void (*evt_cb)(enum gatt_evt_type,union gatt_evt_u *,uint8_t));

void gatt_manager_svc_register(uint16_t start_hdl,uint8_t att_num,struct gatt_svc_env *svc);

void gatt_manager_server_read_req_reply(uint8_t con_idx,uint16_t handle,uint8_t status,uint8_t *data,uint16_t length);

void gatt_manager_server_send_indication(uint8_t con_idx,uint16_t handle,uint8_t *data,uint16_t length,uint16_t *transaction_id);

void gatt_manager_server_send_notification(uint8_t con_idx,uint16_t handle,uint8_t *data,uint16_t length,uint16_t *transaction_id);

void gatt_manager_client_indication_confirm(uint8_t con_idx,uint16_t handle);

uint16_t gatt_manager_get_svc_att_handle(struct gatt_svc_env *svc,uint8_t att_idx);

void gatt_manager_client_write_no_rsp(uint8_t con_idx,uint16_t handle,uint8_t *data,uint16_t length);

void gatt_manager_client_write_with_rsp(uint8_t con_idx,uint16_t handle,uint8_t *data,uint16_t length);

void gatt_manager_client_cccd_enable(uint8_t con_idx,uint16_t handle,bool notification_en, bool indication_en);

void gatt_manager_client_svc_discover_by_uuid(uint8_t con_idx,uint8_t *uuid,enum uuid_length uuid_len,uint16_t start_hdl,uint16_t end_hdl);

void gatt_manager_client_char_discover_by_uuid(uint8_t con_idx,uint8_t *uuid,enum uuid_length uuid_len,uint16_t start_hdl,uint16_t end_hdl);

void gatt_manager_client_desc_char_discover(uint8_t con_idx,uint16_t start_hdl,uint16_t end_hdl);

void gatt_manager_client_mtu_exch_send(uint8_t con_idx);

void gatt_manager_client_read(uint8_t con_idx,uint16_t handle);
#endif

