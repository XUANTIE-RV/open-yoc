#include "spi_flash.h"
#include "le501x.h"
#include "lscache.h"
#include "compile_flag.h"
#include "reg_rcc.h"
#include "field_manipulate.h"
#include "sdk_config.h"
#include "platform.h"
#include "reg_lsgpio.h"
#include "reg_syscfg.h"
#include "reg_rf.h"
#include "prf_fotas.h"
#include "tinycrypt/ecc_dsa.h"
#include "ls_ble.h"
#include "ls_dbg.h"
#include "log.h"
#include "common.h"
#include "systick.h"
#define FW_ECC_VERIFY (0)

static uint8_t adv_obj_hdl;
static uint8_t advertising_data[28] = {5,8,'F','O','T','A'};
static uint8_t scan_response_data[31];

#if FW_ECC_VERIFY
extern const uint8_t fotas_pub_key[64];
bool fw_signature_check(struct fw_digest *digest,struct fota_signature *signature)
{
    return uECC_verify(fotas_pub_key, digest->data, sizeof(digest->data), signature->data, uECC_secp256r1());
}
#else
bool fw_signature_check(struct fw_digest *digest,struct fota_signature *signature)
{
    return true;
}
#endif

static void prf_fota_server_callback(enum fotas_evt_type type,union fotas_evt_u *evt,uint8_t con_idx)
{
    switch(type)
    {
    case FOTAS_START_REQ_EVT:
    {
        // ota_settings_write(SINGLE_FOREGROUND); 
        ota_settings_write(DOUBLE_FOREGROUND); 
        enum fota_start_cfm_status status;
        if(fw_signature_check(evt->fotas_start_req.digest, evt->fotas_start_req.signature))
        {
            status = FOTA_REQ_ACCEPTED;
        }else
        {
            status = FOTA_REQ_REJECTED;
        }
        prf_fotas_start_confirm(con_idx, status);
    }break;
    case FOTAS_FINISH_EVT:
        if(evt->fotas_finish.integrity_checking_result)
        {
            if(evt->fotas_finish.new_image->base != get_app_image_base())
            {
                ota_copy_info_set(evt->fotas_finish.new_image);
            }
            else
            {
                ota_settings_erase();
            }
            platform_reset(RESET_OTA_SUCCEED);
        }else
        {
            platform_reset(RESET_OTA_FAILED);
        }
    break;
    default:
        LS_ASSERT(0);
    break;
    }
}

static void create_adv_obj()
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = 32,
        .adv_intv_max = 32,
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
    case PRF_FOTA_SERVER:
        prf_fota_server_callback_init(prf_fota_server_callback);
        create_adv_obj();
    break;
    default:

    break;
    }
}

static void dev_manager_callback(enum dev_evt_type type, union dev_evt_u * evt)
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
    }break;
    case STACK_READY:
        dev_manager_prf_fota_server_add(NO_SEC);
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
    default:

    break;
    }
}

static void gap_manager_callback(enum gap_evt_type type, union gap_evt_u * evt, uint8_t con_idx)
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
    case ADV_STOPPED:
        LOG_I("adv stopped");
    break;
    default:

    break;
    }


}

static void gatt_manager_callback(enum gatt_evt_type type, union gatt_evt_u * evt, uint8_t con_idx)
{


}

int main()
{
    sys_init_app();
//    MODIFY_REG(LSGPIOA->MODE,GPIO_MODE4_MASK,1<<GPIO_MODE4_POS);
//    MODIFY_REG(LSGPIOA->AE,GPIO_AE4_MASK,2<<GPIO_AE4_POS);
    ble_init();
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
    return 0;
}
