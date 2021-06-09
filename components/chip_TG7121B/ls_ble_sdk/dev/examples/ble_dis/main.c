#define LOG_TAG "MAIN"
#include "ls_ble.h"
#include "platform.h"
#include "prf_diss.h"
#include "log.h"
#include "ls_dbg.h"
#include <string.h>

#define APP_DIS_DEV_NAME                ("LS Dis Server")
#define APP_DIS_DEV_NAME_LEN            (sizeof(APP_DIS_DEV_NAME))

/// Manufacturer Name Value
#define APP_DIS_MANUFACTURER_NAME       ("LinkedSemi")
#define APP_DIS_MANUFACTURER_NAME_LEN   (sizeof(APP_DIS_MANUFACTURER_NAME))

/// ADV Data
#define APP_DIS_ADV_DATA                ("LS Dis Server")

/// Model Number String Value
#define APP_DIS_MODEL_NB_STR            ("LS-BLE-1.0")
#define APP_DIS_MODEL_NB_STR_LEN        (10)

/// Serial Number
#define APP_DIS_SERIAL_NB_STR           ("1.0.0.0-LE")
#define APP_DIS_SERIAL_NB_STR_LEN       (10)

/// Firmware Revision
#define APP_DIS_FIRM_REV_STR            ("1.0.0")
#define APP_DIS_FIRM_REV_STR_LEN        (5)

/// System ID Value - LSB -> MSB
#define APP_DIS_SYSTEM_ID               ("\x12\x34\x56\xFF\xFE\x9A\xBC\xDE")
#define APP_DIS_SYSTEM_ID_LEN           (8)

/// Hardware Revision String
#define APP_DIS_HARD_REV_STR           ("1.0.0")
#define APP_DIS_HARD_REV_STR_LEN       (5)

/// Software Revision String
#define APP_DIS_SW_REV_STR              ("1.0.0")
#define APP_DIS_SW_REV_STR_LEN          (5)

/// IEEE
#define APP_DIS_IEEE                    ("\xFF\xEE\xDD\xCC\xBB\xAA")
#define APP_DIS_IEEE_LEN                (6)

/// PNP ID Value
#define APP_DIS_PNP_ID                  ("\x02\x5E\x04\x40\x00\x00\x03")
#define APP_DIS_PNP_ID_LEN              (7)

static uint8_t adv_obj_hdl;
static uint8_t advertising_data[28] = {11,0x08,'L','i','n','k','e','d','s','e','m','i'};;
static uint8_t scan_response_data[31];

static void dis_server_get_dev_name(struct gap_dev_info_dev_name *dev_name_ptr, uint8_t con_idx);
static void dis_server_get_appearance(struct gap_dev_info_appearance *dev_appearance_ptr, uint8_t con_idx);
static void dis_server_get_slv_pref_param(struct gap_dev_info_slave_pref_param *dev_slv_pref_param_ptr, uint8_t con_idx);

static void gap_manager_callback(enum gap_evt_type type,union gap_evt_u *evt,uint8_t con_idx)
{
    switch(type)
    {
    case CONNECTED:

    break;
    case DISCONNECTED:
        dev_manager_start_adv(adv_obj_hdl,advertising_data,sizeof(advertising_data),scan_response_data,sizeof(scan_response_data));
    break;
    case CONN_PARAM_REQ:
        //LOG_I
    break;
    case CONN_PARAM_UPDATED:

    break;
    case GET_DEV_INFO_DEV_NAME:
        dis_server_get_dev_name((struct gap_dev_info_dev_name*)evt, con_idx);
    break;
    case GET_DEV_INFO_APPEARANCE:
        dis_server_get_appearance((struct gap_dev_info_appearance*)evt, con_idx);
    break;
    case GET_DEV_INFO_SLV_PRE_PARAM:
        dis_server_get_slv_pref_param((struct gap_dev_info_slave_pref_param*)evt, con_idx);
    break;
    default:

    break;
    }
}

static void gatt_manager_callback(enum gatt_evt_type type,union gatt_evt_u *evt,uint8_t con_idx)
{
    
}
static void dis_server_get_dev_name(struct gap_dev_info_dev_name *dev_name_ptr, uint8_t con_idx)
{
    LS_ASSERT(dev_name_ptr);
    dev_name_ptr->value = (uint8_t*)APP_DIS_DEV_NAME;
    dev_name_ptr->length = APP_DIS_DEV_NAME_LEN;
}
static void dis_server_get_appearance(struct gap_dev_info_appearance *dev_appearance_ptr, uint8_t con_idx)
{
    LS_ASSERT(dev_appearance_ptr);
    dev_appearance_ptr->appearance = 0;
}
static void dis_server_get_slv_pref_param(struct gap_dev_info_slave_pref_param *dev_slv_pref_param_ptr, uint8_t con_idx)
{
    LS_ASSERT(dev_slv_pref_param_ptr);
    dev_slv_pref_param_ptr->con_intv_min  = 8;
    dev_slv_pref_param_ptr->con_intv_max  = 20;
    dev_slv_pref_param_ptr->slave_latency =  0;
    dev_slv_pref_param_ptr->conn_timeout  = 200;
}

static void dis_server_get_value(uint8_t value, uint8_t con_idx)
{
    // Initialize length
    uint8_t len = 0;
    // Pointer to the data
    uint8_t *data = NULL;
    switch (value)
    {
    case DIS_SERVER_MANUFACTURER_NAME_CHAR:
        len = APP_DIS_MANUFACTURER_NAME_LEN;
        data = (uint8_t *)APP_DIS_MANUFACTURER_NAME;
        break;
    case DIS_SERVER_MODEL_NB_STR_CHAR:
        len = APP_DIS_MODEL_NB_STR_LEN;
        data = (uint8_t *)APP_DIS_MODEL_NB_STR;
        break;
    case DIS_SERVER_SERIAL_NB_STR_CHAR:
        len = APP_DIS_SERIAL_NB_STR_LEN;
        data = (uint8_t *)APP_DIS_SERIAL_NB_STR;
        break;
    case DIS_SERVER_HARD_REV_STR_CHAR:
        len = APP_DIS_HARD_REV_STR_LEN;
        data = (uint8_t *)APP_DIS_HARD_REV_STR;
        break;
    case DIS_SERVER_FIRM_REV_STR_CHAR:
        len = APP_DIS_FIRM_REV_STR_LEN;
        data = (uint8_t *)APP_DIS_FIRM_REV_STR;
        break;
    case DIS_SERVER_SW_REV_STR_CHAR:
        len = APP_DIS_SW_REV_STR_LEN;
        data = (uint8_t *)APP_DIS_SW_REV_STR;
        break;
    case DIS_SERVER_SYSTEM_ID_CHAR:
        len = APP_DIS_SYSTEM_ID_LEN;
        data = (uint8_t *)APP_DIS_SYSTEM_ID;
        break;
    case DIS_SERVER_IEEE_CHAR:
        len = APP_DIS_IEEE_LEN;
        data = (uint8_t *)APP_DIS_IEEE;
        break;
    case DIS_SERVER_PNP_ID_CHAR:
        len = APP_DIS_PNP_ID_LEN;
        data = (uint8_t *)APP_DIS_PNP_ID;
        break;
    default:
        LOG_I("Unsupported dis value!");
        break;
    }
    if(data)
    {
        prf_diss_value_confirm(con_idx, value, len, data);
    }   
}

static void prf_dis_server_callback(enum diss_evt_type type,union diss_evt_u *evt,uint8_t con_idx)
{
    switch(type)
    {
    case DIS_SERVER_SET_VALUE_RSP:

    break;
    case DIS_SERVER_GET_VALUE_IND:
        dis_server_get_value(evt->get_value_ind.value, con_idx);
    break;
    default:
        LS_ASSERT(0);
    break;
    }
}

static void create_adv_obj()
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = 1600,
        .adv_intv_max = 1600,
        .own_addr_type = PUBLIC_OR_RANDOM_STATIC_ADDR,
        .filter_policy = 0,
        .ch_map = 0x7,
        .disc_mode = ADV_MODE_GEN_DISC,
        .prop = {
            .connectable = 1,
            .scannable = 1,
            .directed = 0,
            .high_duty_cycle = 0,
        },
    };
    dev_manager_create_legacy_adv_object(&adv_param);
}

static void prf_added_handler(struct profile_added_evt *evt)
{
    LOG_I("profile:%d, start handle:0x%x\n",evt->id,evt->start_hdl);
    switch(evt->id)
    {
    case PRF_DIS_SERVER:
        prf_dis_server_callback_init(prf_dis_server_callback);
        create_adv_obj();
    break;
    default:

    break;
    }
}

static void dev_manager_callback(enum dev_evt_type type,union dev_evt_u *evt)
{
    switch(type)
    {
    case STACK_INIT:
    {
        struct ble_stack_cfg cfg = {
            .private_addr = false,
            .controller_privacy = false,
        };
        dev_manager_stack_init(&cfg);
    }
    break;
    case STACK_READY:
    {
        uint8_t addr[6];
        bool type;
        dev_manager_get_identity_bdaddr(addr,&type);
        LOG_I("type:%d,addr:",type);
        LOG_HEX(addr,sizeof(addr));
        dev_manager_prf_dis_server_add(NO_SEC,0xffff);
    }break;
    case SERVICE_ADDED:

    break;
    case PROFILE_ADDED:
        prf_added_handler(&evt->profile_added);
    break;
    case ADV_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        adv_obj_hdl = evt->obj_created.handle;
        dev_manager_start_adv(adv_obj_hdl,advertising_data,sizeof(advertising_data),scan_response_data,sizeof(scan_response_data));
    break;
    case ADV_STOPPED:
    
    break;
    case SCAN_STOPPED:
    
    break;
    default:

    break;
    }
    
}


int main()
{
    sys_init_app();
    ble_init();
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
}
