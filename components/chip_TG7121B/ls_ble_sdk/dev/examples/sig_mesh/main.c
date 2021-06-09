#define LOG_TAG "MAIN"
#include <string.h>
#include <stdio.h>
#include "ls_ble.h"
#include "platform.h"
#include "ls_sig_mesh.h"
#include "log.h"
#include "ls_dbg.h"
#include "lsgpio.h"
#include "spi_flash.h"
#include "tinyfs.h"
#include "tinycrypt/sha256.h"
#include "tinycrypt/constants.h"
#include "lscrypt.h"
#include "field_manipulate.h"
#include "reg_lscrypt.h"
#include "le501x.h"
#include "sig_mesh_ctl.h"
#include "sig_light_cfg.h"

#define COMPANY_ID 0x093A
#define COMPA_DATA_PAGES 1
#define MAX_NB_ADDR_REPLAY 5
#define PROV_AUTH_ACCEPT 1
#define PROV_AUTH_NOACCEPT 0
static bool sent_adv_ready=true;
static bool ls_sig_mesh_auto_prov=false;
uint8_t vendor_tid = 0;
#define RECORD_KEY1 1
#define RECORD_KEY2 2

tinyfs_dir_t mesh_dir;
struct mesh_model_info model_env;
SIGMESH_NodeInfo_TypeDef Node_Get_Proved_State = 0;
SIGMESH_NodeInfo_TypeDef Node_Proved_State = 0;

static uint8_t dev_uuid[16] = {0x92,0x88,0x8A,0xFB,0xF0,0xCA,0x8F,0x45,0xB2,0xDF,0x93,0xEE,0x71,0x12,0x7C,0x68};
static uint8_t auth_data[16] = {0xF4,0xC7,0x0B,0xE8,0x10,0xE1,0x3A,0xAE,0xF8,0x3B,0x05,0xD4,0xA1,0xDA,0x8C,0xF3};

//just for auto prov mode
uint8_t net_key[16] = {0x4D,0xC8,0x86,0xC8,0x0A,0x53,0xEF,0x80,0x5B,0x5C,0x30,0x59,0xF6,0xE3,0xA7,0xAD};
uint8_t app_key[16] = {0x8E,0x40,0x71,0xA5,0x3F,0x7A,0xEA,0xF0,0x81,0xB9,0xCF,0x8B,0x83,0x05,0x12,0x52};


uint8_t rsp_data_info[40] = {0};
struct mesh_publish_info_ind mesh_publish_env[MAX_MESH_MODEL_NB];
uint8_t model_tid = 0;

static uint16_t mesh_src_addr;
static uint8_t adv_obj_hdl;
static bool mesh_node_prov_state = false;
static void create_adv_obj(void);
void app_client_model_tx_message_handler(uint32_t tx_msg, uint16_t model_cfg_idx );

void auto_check_unbind(void)
{
    uint16_t length = 1;
    uint8_t coutinu_power_up_num = 0;
    tinyfs_mkdir(&mesh_dir, ROOT_DIR, 5);
    tinyfs_read(mesh_dir, RECORD_KEY1, &coutinu_power_up_num, &length);
    LOG_I("coutinu_power_up_num:%d", coutinu_power_up_num);

    if (coutinu_power_up_num > 4)
    {
        coutinu_power_up_num = 0;
        tinyfs_write(mesh_dir, RECORD_KEY1, &coutinu_power_up_num, sizeof(coutinu_power_up_num));
        SIGMESH_UnbindAll();
    }
    else
    {
        coutinu_power_up_num++;
        tinyfs_write(mesh_dir, RECORD_KEY1, &coutinu_power_up_num, sizeof(coutinu_power_up_num));
        tinyfs_write_through();
    }
}

void enable_tx_unprov_beacon(void)
{
    struct bcn_start_unprov_param info;
        memcpy(&info.DevUuid[0],&dev_uuid[0],16);
        info.UriHash = 0x00000000;
        info.OobInfo = 0x0000;
        info.UriHash_Present = true;
        start_tx_unprov_beacon(&info);
}

void disable_tx_unprov_beacon(void)
{
    stop_tx_unprov_beacon();
}

void prov_succeed_src_addr_ind(uint16_t src)
{
    tinyfs_write(mesh_dir, RECORD_KEY2, (uint8_t *)&src, sizeof(src));
    tinyfs_write_through();
}

static void gap_manager_callback(enum gap_evt_type type, union gap_evt_u *evt, uint8_t con_idx)
{
    switch (type)
    {
    case CONNECTED:
        LOG_I("connected");
        break;
    case DISCONNECTED:
        LOG_I("disconnected");
        if(mesh_node_prov_state == true)
        {
            ls_sig_mesh_proxy_adv_ctl(true);
        }
        break;
    case CONN_PARAM_REQ:

        break;
    case CONN_PARAM_UPDATED:

        break;
    default:

        break;
    }
}

static void gatt_manager_callback(enum gatt_evt_type type, union gatt_evt_u *evt, uint8_t con_idx)
{
    
}

void app_client_model_tx_message_handler(uint32_t tx_msg, uint16_t model_cfg_idx )
{
    if (model_cfg_idx == MESH_CMDL_CFG_IDX_GENC_LEVEL)
    {
       struct model_cli_trans_info param;
        model_tid++;

       param.mdl_lid = mesh_publish_env[MODEL4_GENERIC_LVL_CLI].model_lid;
       param.app_key_lid = model_env.app_key_lid;
       param.dest_addr =  mesh_publish_env[MODEL4_GENERIC_LVL_CLI].addr;
       param.state_1 = tx_msg;
       param.state_2 = 0x00;
       param.delay_ms=50;
       param.trans_info=(uint16_t)(model_tid<<8);
       param.trans_time_ms=100;
       mesh_model_client_tx_message_handler(&param);
    }

    if (model_cfg_idx == MESH_CMDL_CFG_IDX_GENC_ONOFF)
     {
       struct model_cli_trans_info param;
        model_tid++;

       param.mdl_lid = mesh_publish_env[MODEL3_GENERIC_ONOFF_CLI].model_lid;
       param.app_key_lid = model_env.app_key_lid;
       param.dest_addr =  mesh_publish_env[MODEL3_GENERIC_ONOFF_CLI].addr;
       param.state_1 = tx_msg;
       param.state_2 = 0x00;
       param.delay_ms=50;
       param.trans_info=(uint16_t)(model_tid<<8);
       param.trans_time_ms=100;
       mesh_model_client_tx_message_handler(&param);
    }
}

static void mesh_manager_callback(enum mesh_evt_type type, union ls_sig_mesh_evt_u *evt)
{
    switch (type)
    {
    case MESH_ACTIVE_ENABLE:
    {
        create_adv_obj();
        TIMER_Set(2, 3000); //clear power up num
    }
    break;
    case MESH_ACTIVE_DISABLE:
    {
        SIGMESH_UnbindAll();
        ls_sig_mesh_platform_reset();
    }
    break;
    case MESH_ACTIVE_REGISTER_MODEL:
    {
        for (uint8_t i = 0; i < model_env.nb_model; i++)
        {
            model_env.info[i].model_lid = evt->sig_mdl_info.info[i].model_lid;
        }
        model_env.app_key_lid = evt->sig_mdl_info.app_key_lid;
    }
    break;
    case MESH_ACTIVE_MODEL_PUBLISH:
    {
        for (uint8_t i = 0; i < model_env.nb_model; i++)
        {
            if (evt->mesh_publish_info.model_lid == model_env.info[i].model_lid)
            {
                 mesh_publish_env[i].model_lid = evt->mesh_publish_info.model_lid;
                 mesh_publish_env[i].period_ms = evt->mesh_publish_info.period_ms;
                 mesh_publish_env[i].addr =  evt->mesh_publish_info.addr;
            }
        }
       
    }
    break;
    case MESH_ACTIVE_MODEL_GROUP_MEMBERS:
    {
    }
    break;
    case MESH_ACTIVE_MODEL_RSP_SENT:
    {
    }
    break;
    case MESH_ACTIVE_LPN_START:
    {
    }
    break;
    case MESH_ACTIVE_LPN_STOP:
    {
    }
    break;
    case MESH_ACTIVE_LPN_SELECT_FRIEND:
    {
    }
    break;
    case MESH_ACTIVE_PROXY_CTL:
    {
    }
    break;
    case MESH_ACTIVE_STORAGE_LOAD:
    {
        Node_Get_Proved_State = evt->st_proved.proved_state;
        if (Node_Get_Proved_State == PROVISIONED_OK)
        {
            uint16_t length = sizeof(mesh_src_addr);
            LOG_I("The node is provisioned");
            ls_mesh_light_set_lightness(0xffff,LIGHT_LED_2);
            ls_mesh_light_set_lightness(0xffff,LIGHT_LED_3);
            tinyfs_read(mesh_dir, RECORD_KEY2, (uint8_t*)&mesh_src_addr, &length);
            mesh_node_prov_state = true;
        }
        else
        {
            LOG_I("The node is not provisioned");
            mesh_node_prov_state = false;
        }
        
    }
    break;
    case MESH_ACTIVE_STORAGE_SAVE:
        break;
    case MESH_ACTIVE_STORAGE_CONFIG:
        break;
    case MESH_GET_PROV_INFO:
    {
        struct mesh_prov_info param;
        memcpy(&param.DevUuid[0],&dev_uuid[0],16);
        param.UriHash = 0x00000000;
        param.OobInfo = 0x0000;
        param.PubKeyOob = 0x00;
        param.StaticOob = 0x01;
        param.OutOobSize = 0x00;
        param.InOobSize = 0x00;
        param.OutOobAction = 0x0000;
        param.InOobAction = 0x0000;
        param.Info = 0x00;
        set_prov_param(&param);
    }
    break;
    case MESH_GET_PROV_AUTH_INFO:
    {
        struct mesh_prov_auth_info param;
        param.Adopt = PROV_AUTH_ACCEPT;
        memcpy(&param.AuthBuffer[0], &auth_data[0], 16);
        param.AuthSize = 16;
        set_prov_auth_info(&param);
    }
    break;
    case MESH_REPORT_ATTENTION_STATE:
    {
    }
    break;
    case MESH_REPOPT_PROV_RESULT:
    {
        if(evt->prov_rslt_sate.state == MESH_PROV_STARTED)
        {
            LOG_I("prov started");
        }
        else if(evt->prov_rslt_sate.state == MESH_PROV_SUCCEED)
        {
            LOG_I("prov succeed");
            mesh_node_prov_state = true;
            ls_mesh_light_set_lightness(0xffff,LIGHT_LED_2);
            ls_mesh_light_set_lightness(0xffff,LIGHT_LED_3);
        }
       else if(evt->prov_rslt_sate.state == MESH_PROV_FAILED)
        {
            LOG_I("prov failled:%d",evt->prov_rslt_sate.status);
        }
    }
    break;
    case MESH_ACCEPT_MODEL_INFO:
    {
        
    }
    break;
    case MESH_STATE_UPD_IND:
    {
        sig_mesh_mdl_state_upd_hdl(&evt->mdl_state_upd_ind);
    }
    break;
    case MESH_REPORT_TIMER_STATE:
    {
        if (2 == evt->mesh_timer_state.timer_id)
        {
            uint8_t clear_power_on_num = 0;
            TIMER_Cancel(2);
            tinyfs_write(mesh_dir, RECORD_KEY1, &clear_power_on_num, sizeof(clear_power_on_num));
            tinyfs_write_through();
        }
    }
    break;
    case MESH_ADV_REPORT:
    {
        // if(evt->adv_report.adv_addr->addr[5] == 0x20 && evt->adv_report.adv_addr->addr[4] == 0x17)
        // {
        //     LOG_I("dev addr: %02x:%02x:%02x:%02x:%02x:%02x",evt->adv_report.adv_addr->addr[5],
        //                                                     evt->adv_report.adv_addr->addr[4],
        //                                                     evt->adv_report.adv_addr->addr[3],
        //                                                     evt->adv_report.adv_addr->addr[2],
        //                                                     evt->adv_report.adv_addr->addr[1],
        //                                                     evt->adv_report.adv_addr->addr[0]);
        //     //LOG_HEX(evt->adv_report.adv_addr.addr,6);
        //     LOG_HEX(evt->adv_report.data,evt->adv_report.length);
        // }
        
    }
    break;
    case MESH_ACTIVE_AUTO_PROV:
    {
        if (ls_sig_mesh_auto_prov == true)
        {
           struct mesh_auto_prov_info init_param;
           init_param.model_nb = model_env.nb_model;
           init_param.unicast_addr = 0x0002;  //Recommendation unicast_addr >= 0x0002 
           init_param.group_addr = 0xC000;    //Recommendation group_addr >= 0xC000
           init_param.model_id[0] = model_env.info[0].model_id;
           init_param.model_id[1] = model_env.info[1].model_id;
           init_param.model_id[2] = model_env.info[2].model_id;
           init_param.model_id[3] = model_env.info[3].model_id;
           memcpy(&init_param.app_key[0],&app_key[0],16);
           memcpy(&init_param.net_key[0],&net_key[0],16);
  
           ls_sig_mesh_auto_prov_handler(&init_param,true);
        }
    }
    break;
    default:
        break;
    }
}

char ble_device_name[DEV_NAME_MAX_LEN] = "ls_sig_mesh_node";
void ls_sig_mesh_con_set_scan_rsp_data(uint8_t *scan_rsp_data, uint8_t *scan_rsp_data_len)
{
    uint8_t *pos;
    pos = scan_rsp_data;
    *pos++ = strlen(ble_device_name)+1;
    *pos++ = '\x08';
    memcpy(pos, ble_device_name, strlen((const char *)ble_device_name));
    pos += strlen((const char *)ble_device_name);

    *scan_rsp_data_len = ((uint32_t)pos - (uint32_t)(&scan_rsp_data[0]));
}

static void create_adv_obj()
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = 0x64,
        .adv_intv_max = 0x64,
        .own_addr_type = PUBLIC_OR_RANDOM_STATIC_ADDR,
        .filter_policy = 0,
        .ch_map = 0x7,
        .disc_mode = ADV_MODE_GEN_DISC,
        .prop = {
            .connectable = 0,
            .scannable = 1,
            .directed = 0,
            .high_duty_cycle = 0,
        },
    };
    dev_manager_create_legacy_adv_object(&adv_param);
}

void mesh_send_custom_adv(uint16_t duration,uint8_t *adv_data,uint8_t adv_data_length)
{
    if (sent_adv_ready==true)
    {
        sent_adv_ready = false;
       dev_manager_set_adv_duration(duration);
       dev_manager_start_adv(adv_obj_hdl,adv_data,adv_data_length,NULL,0);
    }
}
uint8_t usr_mac_addr[6]={2,2,3,4,5,6};
static void dev_manager_callback(enum dev_evt_type type, union dev_evt_u *evt)
{
    switch (type)
    {
    case STACK_INIT:
    {
        struct ble_stack_cfg cfg = {
            .private_addr = true,
            .controller_privacy = false,
        };
        dev_manager_stack_init(&cfg);
        dev_manager_set_mac_addr(&usr_mac_addr[0]);
 //       ls_sig_mesh_auto_prov = true;
    }
    break;

    case STACK_READY:
    {
        uint8_t addr[6];
        struct ls_sig_mesh_cfg feature = {
            .MeshFeatures = EN_RELAY_NODE | EN_MSG_API,
            .MeshCompanyID = COMPANY_ID,
            .MeshProID = 0x001A,
            .MeshProVerID = 0x0001,
            .MeshLocDesc = 0x0100,
            .NbAddrReplay  = MAX_NB_ADDR_REPLAY,
            .NbCompDataPage = COMPA_DATA_PAGES,
        };

        if (ls_sig_mesh_auto_prov == false)
        {
          feature.MeshFeatures |= EN_PB_GATT | EN_PROXY_NODE;
        }
        bool type;
        dev_manager_get_identity_bdaddr(addr, &type);
        memcpy(&dev_uuid[0],addr,6);
        LOG_I("stack ready:");
        LOG_HEX(addr,6);
        dev_manager_prf_ls_sig_mesh_add(NO_SEC, &feature);
        ls_button_timer_init();
    }
    break;

    case SERVICE_ADDED:
    {
    }
    break;
    case PROFILE_ADDED:
    {
        prf_ls_sig_mesh_callback_init(mesh_manager_callback);
      
        model_env.nb_model = USR_MODEL_MAX_NUM;
        model_env.app_key_lid = 0;
        model_env.info[MODEL0_GENERIC_ONOFF_SVC].sig_model_cfg_idx = MESH_MDL_CFG_ONOFF;
        model_env.info[MODEL0_GENERIC_ONOFF_SVC].element_id = 0;
        model_env.info[MODEL0_GENERIC_ONOFF_SVC].model_lid =0;
        model_env.info[MODEL0_GENERIC_ONOFF_SVC].model_id =  GENERIC_ONOFF_SERVER;

        model_env.info[MODEL1_GENERIC_LVL_SVC].sig_model_cfg_idx = MESH_MDL_CFG_LEVEL;
        model_env.info[MODEL1_GENERIC_LVL_SVC].element_id = 1;
        model_env.info[MODEL1_GENERIC_LVL_SVC].model_lid =0;
        model_env.info[MODEL1_GENERIC_LVL_SVC].model_id =  GENERIC_LVL_SERVER;
        
        model_env.info[MODEL2_GENERIC_LVL_SVC].sig_model_cfg_idx = MESH_MDL_CFG_LEVEL;
        model_env.info[MODEL2_GENERIC_LVL_SVC].element_id = 2;
        model_env.info[MODEL2_GENERIC_LVL_SVC].model_lid =0;
        model_env.info[MODEL2_GENERIC_LVL_SVC].model_id =  GENERIC_LVL_SERVER;

        model_env.info[MODEL3_GENERIC_ONOFF_CLI].sig_model_cfg_idx = MESH_CMDL_CFG_IDX_GENC_ONOFF;
        model_env.info[MODEL3_GENERIC_ONOFF_CLI].element_id = 3;
        model_env.info[MODEL3_GENERIC_ONOFF_CLI].model_lid =0;
        model_env.info[MODEL3_GENERIC_ONOFF_CLI].model_id  = GENERIC_ONOFF_CLIENT;

        model_env.info[MODEL4_GENERIC_LVL_CLI].sig_model_cfg_idx = MESH_CMDL_CFG_IDX_GENC_LEVEL;
        model_env.info[MODEL4_GENERIC_LVL_CLI].element_id = 4;
        model_env.info[MODEL4_GENERIC_LVL_CLI].model_lid =0;
        model_env.info[MODEL4_GENERIC_LVL_CLI].model_id  = GENERIC_LVL_CLIENT;

        model_env.info[MODEL5_GENERIC_ONOFF_SVC].sig_model_cfg_idx = MESH_MDL_CFG_ONOFF;
        model_env.info[MODEL5_GENERIC_ONOFF_SVC].element_id = 5;
        model_env.info[MODEL5_GENERIC_ONOFF_SVC].model_lid =0;
        model_env.info[MODEL5_GENERIC_ONOFF_SVC].model_id =  GENERIC_ONOFF_SERVER;

        ls_sig_mesh_init(&model_env);
    }
    break;
    case ADV_OBJ_CREATED:
    {
        adv_obj_hdl = evt->obj_created.handle;
        LOG_I("adv obj hdl:%d",adv_obj_hdl);
    }
    break;
    case ADV_STOPPED:
    {
        sent_adv_ready = true;
    }
    break;
    case SCAN_STOPPED:
    {
    }
    break;
    default:

        break;
    }
}

int main()
{
    sys_init_app();
    ls_mesh_pwm_init();
    light_button_init();
    ble_init();
    auto_check_unbind();
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
    return 0;
}
