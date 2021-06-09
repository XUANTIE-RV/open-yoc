#ifndef _LS_SIG_MESH_H_
#define _LS_SIG_MESH_H_
#include <stdint.h>
#include "ls_ble.h"
#define __EMPTY
typedef uint8_t SIGMESH_ModelHandle_TypeDef;
typedef uint8_t SIGMESH_NodeInfo_TypeDef;

#define UUID_MESH_DEV_LEN (16)
#define MESH_AUTH_DATA_LEN (16)
#define MAX_MESH_MODEL_NB (14)
#define MAX_AUTO_PROV_MESH_MODEL_NB (10)
//#define MAX_MESH_MODEL_MSG_BUFFER      (380)
#define MAX_MESH_MODEL_MSG_BUFFER (60)

#define UPADTE_GLP_STOP_TYPE 0xfe
#define UPADTE_GLP_STOP_TIMEOUT_TYPE 0xfd

//mesh server model index
#define GENERIC_ONOFF_SERVER             (0x1000)
#define GENERIC_ONOFF_CLIENT             (0x1001)
#define GENERIC_LVL_SERVER               (0x1002)
#define GENERIC_LVL_CLIENT               (0x1003)
#define LIGHTNESS_SERVER                 (0x1300)
#define LIGHTS_CTL_SERVER                (0x1303)
#define LIGHTS_HSL_SERVER                (0x1307)
#define VENDOR_TMALL_SERVER              (0x01A80000)
        
//Generic OnOff        
#define GENERIC_ONOFF_GET                (0x0182)
#define GENERIC_ONOFF_SET                (0x0282)
#define GENERIC_ONOFF_SET_UNAK           (0x0382)
#define GENERIC_ONOFF_STATUS             (0x0482)

//Generic OnOff        
#define GENERIC_LVL_GET                  (0x0582)
#define GENERIC_LVL_SET                  (0x0682)
#define GENERIC_LVL_SET_UNAK             (0x0782)
#define GENERIC_LVL_STATUS               (0x0882)

//Light Lightness        
#define LIGHT_LIGHTNESS_SET              (0x4c82)
#define LIGHT_LIGHTNESS_SET_UNAK         (0x4d82)
#define LIGHT_LIGHTNESS_STATUS           (0x4e82)

//Light HSL
#define LIGHT_HSL_SET                    (0x7682)
#define LIGHT_HSL_SET_UNACK              (0x7782)
#define LIGHT_HSL_STATUS                 (0x7882)

//Light CTL
#define LIGHT_CTL_SET                    (0x5E82)
#define LIGHT_CTL_SET_UNACK              (0x5F82)
#define LIGHT_CTL_STATUS                 (0x6082)

// Vendor        
#define APP_MESH_VENDOR_SET              (0x0001A8d1)
#define APP_MESH_VENDOR_SET_UNAK         (0x0001A8d2)
#define APP_MESH_VENDOR_STATUES          (0x0001A8d3)
#define APP_MESH_VENDOR_INDICATION       (0x0001A8d4)
#define APP_MESH_VENDOR_CONFIRMATION     (0x0001A8d5)
#define APP_MESH_VENDOR_TRANSPARENT_MSG  (0x0001A8cf)

#define VENDOR_OPCODE_LEN (3)

enum mesh_evt_type
{
    MESH_ACTIVE_ENABLE = 0,
    MESH_ACTIVE_DISABLE = 1,
    MESH_ACTIVE_REGISTER_MODEL = 2,
    MESH_ACTIVE_MODEL_PUBLISH = 3,
    MESH_ACTIVE_MODEL_GROUP_MEMBERS = 4,
    MESH_ACTIVE_MODEL_RSP_SENT = 5,
    MESH_ACTIVE_LPN_START = 6,
    MESH_ACTIVE_LPN_STOP = 7,
    MESH_ACTIVE_LPN_SELECT_FRIEND = 8,
    MESH_ACTIVE_PROXY_CTL = 9,
    MESH_ACTIVE_STORAGE_LOAD = 10,
    MESH_ACTIVE_STORAGE_SAVE = 11,
    MESH_ACTIVE_STORAGE_CONFIG = 12,
    MESH_GET_PROV_INFO = 13,
    MESH_GET_PROV_AUTH_INFO = 14,
    MESH_REPORT_ATTENTION_STATE = 15,
    MESH_REPOPT_PROV_RESULT = 16,
    MESH_ACCEPT_MODEL_INFO = 17,
    MESH_RSP_MODEL_INFO = 18,
    MESH_REPORT_TIMER_STATE = 19,
    MESH_GENIE_PROV_COMP = 20,
    MESH_ADV_REPORT = 21,
    MESH_STATE_UPD_IND = 22,
    MESH_ACTIVE_GLP_START = 23,
    MESH_ACTIVE_GLP_STOP = 24,
    MESH_ACTIVE_AUTO_PROV =25,
};

/// Mesh Supported Features
enum mesh_feature
{
    EN_RELAY_NODE = 0x00000001,
    EN_PROXY_NODE = 0x00000002,
    EN_FRIEND_NODE = 0x00000004,
    EN_LOW_POWER_NODE = 0x00000008,
    EN_MSG_API = 0x00010000,
    EN_PB_GATT = 0x00020000,
    EN_DYN_BCN_INTV = 0x00040000,
};

enum mesh_provisioned_state
{
    UNPROVISIONED_KO = 0x00,
    PROVISIONING,
    PROVISIONED_OK,
};

enum mesh_provision_result
{
    MESH_PROV_STARTED = 0x0,
    MESH_PROV_SUCCEED,
    MESH_PROV_FAILED,
};

enum mesh_timer_state
{
    MESH_TIMER_DOING = 0x00,
    MESH_TIMER_DONE = 0x01,
};

enum tmall_glp_stop_reason
{
    NO_STOPPING_GLP_REQ = 0x00,
    APPLICATION_USER_STOPPING_GLP_REQ,
    PROVISIONING_INVITE_SWITCH_GLP_REQ,
    PROVISIONING_COMP_SWITCH_GLP_REQ,
};

enum tmall_glp_state
{
    TMALL_GLP_STATE_IDLE =0x00,
    TMALL_GLP_STATE_SLEEP,
    TMALL_GLP_STATE_ACTIVE,
};

/// State identifier values
enum mesh_state_idx
{
    /// Generic OnOff state
    MESH_STATE_GEN_ONOFF = 0,
    /// Generic Level state
    MESH_STATE_GEN_LVL,
    /// Generic Default Transition Time state
    MESH_STATE_GEN_DTT,
    /// Generic Power Actual state
    MESH_STATE_GEN_POWER_ACTUAL,
    /// Generic Power Last state
    MESH_STATE_GEN_POWER_LAST,
    /// Generic Power Default state
    MESH_STATE_GEN_POWER_DFLT,
    /// Generic Power Range state
    MESH_STATE_GEN_POWER_RANGE,
    /// Generic OnPowerUp state
    MESH_STATE_GEN_ONPOWERUP,

    /// Light Lightness
    MESH_STATE_LIGHT_LN = 50,
    /// Light Lightness Linear
    MESH_STATE_LIGHT_LN_LIN,
    /// Light Lightness Last
    MESH_STATE_LIGHT_LN_LAST,
    /// Light Lightness Default
    MESH_STATE_LIGHT_LN_DFLT,
    /// Light Lightness Range
    MESH_STATE_LIGHT_LN_RANGE,
    /// Light Lightness Range Min
    MESH_STATE_LIGHT_LN_RANGE_MIN,
    /// Light Lightness Range Max
    MESH_STATE_LIGHT_LN_RANGE_MAX,

    /// Light CTL Lightness
    MESH_STATE_LIGHT_CTL_LN = 100,
    /// Light CTL Temperature
    MESH_STATE_LIGHT_CTL_TEMP,
    /// Light CTL Delta UV
    MESH_STATE_LIGHT_CTL_DELTA_UV,
    /// Light CTL Temperature Default
    MESH_STATE_LIGHT_CTL_TEMP_DFLT,
    /// Light CTL Temperature Range
    MESH_STATE_LIGHT_CTL_TEMP_RANGE,
    /// Light CTL Delta UV Default
    MESH_STATE_LIGHT_CTL_DELTA_UV_DFLT,

    /// Light HSL Lightness
    MESH_STATE_LIGHT_HSL_LN = 150,
    /// Light HSL Hue
    MESH_STATE_LIGHT_HSL_HUE,
    /// Light HSL Saturation
    MESH_STATE_LIGHT_HSL_SAT,
    /// Light HSL Target
    MESH_STATE_LIGHT_HSL_TGT,
    /// Light HSL Default (Lightness + Hue + Saturation)
    MESH_STATE_LIGHT_HSL_DFLT,
    /// Light HSL Lightness Default
    MESH_STATE_LIGHT_HSL_DFLT_LN,
    /// Light HSL Hue Default
    MESH_STATE_LIGHT_HSL_DFLT_HUE,
    /// Light HSL Saturation Default
    MESH_STATE_LIGHT_HSL_DFLT_SAT,
    /// Light HSL Hue Range
    MESH_STATE_LIGHT_HSL_RANGE_HUE,
    /// Light HSL Saturation Range
    MESH_STATE_LIGHT_HSL_RANGE_SAT,

    /// Light xyL Lightness
    MESH_STATE_LIGHT_XYL_LN = 200,
    /// Light xyL x and y
    MESH_STATE_LIGHT_XYL_XY,
    /// Light xyL Lightness Target
    MESH_STATE_LIGHT_XYL_LN_TGT,
    /// Light xyL x and y Target
    MESH_STATE_LIGHT_XYL_XY_TGT,
    /// Light xyL Lightness Default
    MESH_STATE_LIGHT_XYL_LN_DFLT,
    /// Light xyL x and y Default
    MESH_STATE_LIGHT_XYL_XY_DFLT,
    /// Light xyL x and y Range
    MESH_STATE_LIGHT_XYL_XY_RANGE
};

struct reqister_model_info
{
    uint32_t model_id;
    uint8_t elmt_idx;
    uint8_t config;
};

struct report_model_loc_id_info
{
    uint8_t nb_model;
    uint16_t model_lid[MAX_MESH_MODEL_NB];
};

struct report_dev_provisioned_state_info
{
    uint8_t proved_state;
    uint8_t proving_success_state;
};

struct mesh_prov_info
{
    uint8_t DevUuid[UUID_MESH_DEV_LEN];
    uint32_t UriHash;
    uint16_t OobInfo;
    uint8_t PubKeyOob;
    uint8_t StaticOob;
    uint8_t OutOobSize;
    uint8_t InOobSize;
    uint16_t OutOobAction;
    uint16_t InOobAction;
    uint8_t Info;
};

struct mesh_prov_auth_info
{
    uint8_t Adopt;
    uint8_t AuthSize;
    uint8_t AuthBuffer[MESH_AUTH_DATA_LEN];
};

struct report_mesh_attention_info
{
    uint8_t state;
};

struct update_state_info
{
    uint8_t upd_type;
    uint8_t len;
    uint8_t data[__EMPTY];
};

struct rsp_model_info
{
    uint8_t ModelHandle;
    uint8_t app_key_lid;
    uint16_t dest_addr;
    uint32_t opcode;
    uint16_t len;
    uint8_t info[MAX_MESH_MODEL_MSG_BUFFER];
};

struct model_cli_set_state_info
{
    uint32_t state_1;
    uint32_t state_2;
    uint16_t dest_addr;
    uint16_t set_info;
    uint8_t mdl_lid;
    uint8_t app_key_lid;
};

struct model_cli_trans_info
{
    uint32_t state_1;
    uint32_t state_2;
    uint32_t trans_time_ms;
    uint16_t trans_info;
    uint16_t dest_addr;
    uint16_t delay_ms;
    uint8_t mdl_lid;
    uint8_t app_key_lid;
};

struct start_glp_info
{
    uint8_t RxDelyMs;
    uint16_t SleepIntvlMs;
};

struct model_rx_info
{
    uint8_t ModelHandle;
    uint8_t app_key_lid;
    int8_t rssi;
    uint16_t source_addr;
    uint8_t not_relayed;
    uint32_t opcode;
    uint16_t rx_info_len;
    uint8_t info[__EMPTY];
};

struct model_state_upd
{
    uint32_t state;
    uint32_t trans_time_ms;
    /// State identifier 
    uint16_t state_id;
    uint8_t elmt_idx;
};

struct report_mesh_prov_result_info
{
    uint8_t state;
    uint16_t status;
};

struct model_rsp_sent_info
{
    uint8_t state;
    uint16_t status;
};


struct report_mesh_timer_state_info
{
    uint8_t timer_id;
    uint8_t status;
};

struct model_id_info
{
    uint8_t element_id;
    uint8_t model_lid;
    uint16_t sig_model_cfg_idx;
    uint32_t model_id;
};

struct mesh_model_info
{
    uint8_t nb_model;
    uint8_t app_key_lid;
    struct model_id_info info[MAX_MESH_MODEL_NB];
};

struct mesh_publish_info_ind
{
    uint8_t  model_lid;
    uint32_t period_ms;
    uint16_t addr;
};

struct mesh_auto_prov_info
{
    uint16_t unicast_addr;
    uint8_t model_nb;
    uint16_t group_addr;
    uint32_t model_id[MAX_AUTO_PROV_MESH_MODEL_NB];
    uint8_t app_key[16];
    uint8_t net_key[16];
};

union ls_sig_mesh_evt_u {
    struct reqister_model_info register_model_param;
    struct report_model_loc_id_info loc_id_param;
    struct report_dev_provisioned_state_info st_proved;
    struct report_mesh_attention_info update_attention;
    struct report_mesh_prov_result_info prov_rslt_sate;
    struct model_rx_info rx_msg;
    struct update_state_info update_state_param;
    struct report_mesh_timer_state_info mesh_timer_state;
    struct adv_report_evt adv_report;
    struct model_state_upd mdl_state_upd_ind;
    struct mesh_model_info sig_mdl_info;
    struct mesh_publish_info_ind mesh_publish_info;
    struct mesh_auto_prov_info mesh_auto_prov_param;
};

struct ls_sig_mesh_cfg
{
    uint32_t MeshFeatures;
    uint16_t MeshCompanyID;
    uint16_t MeshProID;
    uint16_t MeshProVerID;
    uint16_t MeshLocDesc;
    uint16_t NbAddrReplay;
    uint8_t NbCompDataPage;
    uint8_t FrdRxWindowMS;
    uint8_t FrdQueueSize;
    uint8_t FrdSubsListSize;
};

struct bcn_start_unprov_param
{
    uint8_t DevUuid[UUID_MESH_DEV_LEN];
    uint16_t OobInfo;
    uint32_t UriHash;
    bool UriHash_Present;
};

void prf_ls_sig_mesh_callback_init(void (*evt_cb)(enum mesh_evt_type, union ls_sig_mesh_evt_u *));
void dev_manager_prf_ls_sig_mesh_add(uint8_t sec_lvl, struct ls_sig_mesh_cfg *cfg);
void ls_sig_mesh_init(struct mesh_model_info *param);
void ls_sig_mesh_platform_reset(void);
void set_prov_param(struct mesh_prov_info *param);
void set_prov_auth_info(struct mesh_prov_auth_info *param);
void model_subscribe(uint8_t const ModelHandle, uint16_t const Addr);
void rsp_model_info_ind(struct rsp_model_info *param);
void mesh_model_client_set_state_handler(struct model_cli_set_state_info *param);
void mesh_model_client_tx_message_handler(struct model_cli_trans_info *param);
void TIMER_Set(uint8_t TimerID, uint32_t DelayMS);
void TIMER_Cancel(uint8_t TimerID);
void SIGMESH_UnbindAll(void);
void stop_tx_unprov_beacon(void);
void start_tx_unprov_beacon(struct bcn_start_unprov_param *param);
void ls_sig_mesh_con_set_scan_rsp_data(uint8_t *scan_rsp_data, uint8_t *scan_rsp_data_len);
void start_ls_sig_mesh_gatt(void);
void stop_ls_sig_mesh_gatt(void);
void ls_sig_mesh_proxy_adv_ctl(uint8_t enable);
void start_glp_handler(struct start_glp_info *param);
void stop_glp_handler(void);
void ls_sig_mesh_auto_prov_handler(struct mesh_auto_prov_info const *param, bool const auto_prov_mesh_flag);
void prov_succeed_src_addr_ind(uint16_t src);
#endif //(_LS_SIG_MESH_H_
