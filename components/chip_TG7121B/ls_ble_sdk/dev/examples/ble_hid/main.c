#define LOG_TAG "MAIN"
#include "ls_ble.h"
#include "platform.h"
#include "prf_hid.h"
#include "prf_bass.h"
#include "log.h"
#include "ls_dbg.h"
#include "cpu.h"
#include "lsuart.h"
#include "builtin_timer.h"
#include <string.h>
#include "co_math.h"
#include "io_config.h"
#include "SEGGER_RTT.h"
#include "tinyfs.h"

#define APP_HID_DEV_NAME ("LS_HID_Demo")
#define APP_HID_DEV_NAME_LEN (sizeof(APP_HID_DEV_NAME))
#define DIS_SVC_PNPID_LEN 0x07

/// PNP ID Value
#define APP_DIS_PNP_ID                  ("\x02\x5E\x04\x40\x00\x00\x03")
#define APP_DIS_PNP_ID_LEN              (7)

#define DIR_NAME    7
#define RECORD_KEY1 1
tinyfs_dir_t hid_dir;
uint8_t ble_device_name[DEV_NAME_MAX_LEN] = "LS_HID_Demo";

static const uint8_t dis_svc_uuid[] ={0x0A,0x18};
static const uint8_t dis_char_pnpid_uuid[] ={0x50,0x2A};
static const uint8_t att_decl_char_array[] = {0x03,0x28};

enum dis_svc_att_db_hdl
{
    DIS_SVC_IDX_PNP_ID_CHAR,
    DIS_SVC_IDX_PNP_ID_VAL,
    DIS_SVC_ATT_NUM,
};

static const struct att_decl dis_server_att_decl[DIS_SVC_ATT_NUM] =
{
    [DIS_SVC_IDX_PNP_ID_CHAR] = {
        .uuid = att_decl_char_array,
        .s.max_len = 0,
        .s.uuid_len = UUID_LEN_16BIT,
        .s.read_indication = 1,   
        .char_prop.rd_en = 1,
    },
    [DIS_SVC_IDX_PNP_ID_VAL] = {
        .uuid = dis_char_pnpid_uuid,
        .s.max_len = DIS_SVC_PNPID_LEN,
        .s.uuid_len = UUID_LEN_16BIT,
        .s.read_indication = 1,
        .char_prop.rd_en = 1,
    },
};

static const struct svc_decl dis_server_svc =
{
    .uuid = dis_svc_uuid,
    .att = (struct att_decl*)dis_server_att_decl,
    .nb_att = DIS_SVC_ATT_NUM,
    .uuid_len = UUID_LEN_16BIT,
    .sec_lvl = 1,
};
static struct gatt_svc_env dis_server_svc_env;

static uint8_t connect_id = 0xff;
static uint8_t adv_obj_hdl;
static uint8_t advertising_data[28];
static uint8_t scan_response_data[31];
static struct builtin_timer *update_batt_timer_inst = NULL;

struct gap_slave_security_req sec_mode_auth =
{
    .auth = SEC_MODE2_LEVEL2
};

//set the default security parameters.
struct pair_feature feat_param =
{
    .iocap = BLE_GAP_IO_CAPS_NONE,
    .oob = OOB_DATA_FLAG,
    .auth = AUTHREQ,
    .key_size = KEY_SIZE,
    .ikey_dist = INIT_KEY_DIST,
    .rkey_dist = RESP_KEY_DIST
};

struct gap_pin_str passkey =
{
    .pin = passkey_number,
    .str_pad = 0
};

static void at_start_adv(void);
static void hid_server_get_dev_name(struct gap_dev_info_dev_name *dev_name_ptr, uint8_t con_idx);

const uint8_t hid_report_map[] =
{
    0x05, 0x01,       // Usage Page (Generic Desktop)
    0x09, 0x06,       // Usage (Keyboard)
    0xA1, 0x01,       // Collection (Application)
    0x05, 0x07,       // Usage Page (Key Codes)
    0x19, 0xe0,       // Usage Minimum (224)
    0x29, 0xe7,       // Usage Maximum (231)
    0x15, 0x00,       // Logical Minimum (0)
    0x25, 0x01,       // Logical Maximum (1)
    0x75, 0x01,       // Report Size (1)
    0x95, 0x08,       // Report Count (8)
    0x81, 0x02,       // Input (Data, Variable, Absolute)

    0x95, 0x01,       // Report Count (1)
    0x75, 0x08,       // Report Size (8)
    0x81, 0x01,       // Input (Constant) reserved byte(1)

    0x95, 0x05,       // Report Count (5)
    0x75, 0x01,       // Report Size (1)
    0x05, 0x08,       // Usage Page (Page# for LEDs)
    0x19, 0x01,       // Usage Minimum (1)
    0x29, 0x05,       // Usage Maximum (5)
    0x91, 0x02,       // Output (Data, Variable, Absolute), Led report
    0x95, 0x01,       // Report Count (1)
    0x75, 0x03,       // Report Size (3)
    0x91, 0x01,       // Output (Data, Variable, Absolute), Led report padding

    0x95, 0x06,       // Report Count (6)
    0x75, 0x08,       // Report Size (8)
    0x15, 0x00,       // Logical Minimum (0)
    0x25, 0x65,       // Logical Maximum (101)
    0x05, 0x07,       // Usage Page (Key codes)
    0x19, 0x00,       // Usage Minimum (0)
    0x29, 0x65,       // Usage Maximum (101)
    0x81, 0x00,       // Input (Data, Array) Key array(6 bytes)

    0x09, 0x05,       // Usage (Vendor Defined)
    0x15, 0x00,       // Logical Minimum (0)
    0x26, 0xFF, 0x00, // Logical Maximum (255)
    0x75, 0x08,       // Report Size (8 bit)
    0x95, 0x02,       // Report Count (2)
    0xB1, 0x02,       // Feature (Data, Variable, Absolute)

    0xC0              // End Collection (Application)
};
#define HID_REPORT_MAP_LEN (sizeof(hid_report_map))

static void dis_server_read_req_ind(uint8_t att_idx, uint8_t con_idx)
{
    uint16_t handle = 0;
    if(att_idx == DIS_SVC_IDX_PNP_ID_VAL)
    {
        handle = gatt_manager_get_svc_att_handle(&dis_server_svc_env, att_idx);
        gatt_manager_server_read_req_reply(con_idx, handle, 0, (void*)APP_DIS_PNP_ID, APP_DIS_PNP_ID_LEN);
    }
}

static void update_batt_timer_cb(void* arg)
{
    bas_batt_lvl_update(0,100);
    builtin_timer_start(update_batt_timer_inst, 1000, NULL);
}

static void uptate_batt_timer_init(void)
{
    update_batt_timer_inst = builtin_timer_create(update_batt_timer_cb);
}

static void gap_manager_callback(enum gap_evt_type type, union gap_evt_u *evt, uint8_t con_idx)
{
    uint16_t ntf_cfg;
    uint16_t len = sizeof(ntf_cfg);
    uint8_t ret;
    switch (type)
    {
    case CONNECTED:
        connect_id = con_idx;
        LOG_I("connected!");
        
        ret = tinyfs_read(hid_dir, RECORD_KEY1, (uint8_t*)&ntf_cfg, &len);
        if(ret != TINYFS_NO_ERROR)
        {
            LOG_I("hid tinyfs read:%d",ret);
        }
        hid_ntf_cfg_init(ntf_cfg, con_idx, evt->connected.peer_id);
        builtin_timer_start(update_batt_timer_inst, 1000, NULL);
        break;
    case DISCONNECTED:
        LOG_I("disconnected!");
        connect_id = 0xff;
        at_start_adv();
        builtin_timer_stop(update_batt_timer_inst);
        break;
    case CONN_PARAM_REQ:

        break;
    case CONN_PARAM_UPDATED:

        break;
    case MASTER_PAIR_REQ: //4
        connect_id = con_idx;
        gap_manager_slave_pair_response_send(connect_id, true, &feat_param);
        break;

    case SLAVE_SECURITY_REQ: //5
        LOG_I("SLAVE_SECURITY_REQ");
        connect_id = con_idx;

        break;

    case PAIR_DONE: //6
        connect_id = con_idx;
        LOG_I("PAIR_DONE");
        break;

    case ENCRYPT_DONE: //7
        connect_id = con_idx;

        break;

    case DISPLAY_PASSKEY: //8
        connect_id = con_idx;
        LOG_I("DISPLAY_PASSKEY");
        break;

    case REQUEST_PASSKEY: //9
        LOG_I("REQUEST_PASSKEY");
        connect_id = con_idx;
        break;

    case NUMERIC_COMPARE: //10
        LOG_I("NUMERIC_COMPARE");
        connect_id = con_idx;
        break;
    case GET_DEV_INFO_DEV_NAME:
        hid_server_get_dev_name((struct gap_dev_info_dev_name *)evt, con_idx);
        break;
    default:

        break;
    }
}

static void gatt_manager_callback(enum gatt_evt_type type, union gatt_evt_u *evt, uint8_t con_idx)
{
    LOG_I("gatt evt:%d",type);
    switch (type)
    {
    case SERVER_READ_REQ:
        LOG_I("read req");
        dis_server_read_req_ind(evt->server_read_req.att_idx, con_idx);
        break;
    case SERVER_WRITE_REQ:
        LOG_I("write req");
        break;
    case SERVER_NOTIFICATION_DONE:
        LOG_I("ntf done");
        break;
    case MTU_CHANGED_INDICATION:
        LOG_I("MTU:%d",evt->mtu_changed_ind.mtu);
        break;
    case CLIENT_RECV_INDICATION:
        LOG_I("clinent recv ind,hdl:%d",evt->client_recv_notify_indicate.handle);
        //LOG_I(evt->client_recv_notify_indicate.value, evt->client_recv_notify_indicate.length);
        break;
    default:
        LOG_I("Event not handled!");
        break;
    }
}

static void hid_server_get_dev_name(struct gap_dev_info_dev_name *dev_name_ptr, uint8_t con_idx)
{
    LS_ASSERT(dev_name_ptr);
    dev_name_ptr->value = (uint8_t *)APP_HID_DEV_NAME;
    dev_name_ptr->length = APP_HID_DEV_NAME_LEN;
}

static void prf_hid_server_callback(enum hid_evt_type type, union hid_evt_u *evt, uint8_t con_idx)
{
    uint16_t ntf_cfg;
    uint8_t ret;
    switch (type)
    {
    case HID_REPORT_READ:
        evt->read_report_req.value = (uint8_t *)hid_report_map;
        evt->read_report_req.length = HID_REPORT_MAP_LEN;
        break;
    case HID_NTF_CFG:
        LOG_I("ntf_cfg save 2:%08x",evt->ntf_cfg.value);
        ntf_cfg = evt->ntf_cfg.value;
        ret = tinyfs_write(hid_dir, RECORD_KEY1, (uint8_t*)&ntf_cfg, sizeof(ntf_cfg));
        if(ret != TINYFS_NO_ERROR)
        {
            LOG_I("hid tinyfs write:%d",ret);
        }
        tinyfs_write_through();
        break;
    default:
        break;
    }
}

static void prf_batt_server_callback(enum bass_evt_type type, union bass_evt_u *evt, uint8_t con_idx)
{

}

static void at_start_adv(void)
{
    uint8_t *pos;
    uint8_t adv_data_len;
    uint8_t scan_rsp_data_len;

    pos = &scan_response_data[0];
    *pos++ = strlen((const char *)ble_device_name) + 1;
    *pos++ = '\x08';
    memcpy(pos, ble_device_name, strlen((const char *)ble_device_name));
    pos += strlen((const char *)ble_device_name);
    scan_rsp_data_len = ((uint32_t)pos - (uint32_t)(&scan_response_data[0]));

    pos = &advertising_data[0];

    uint8_t manufacturer_value[] = {0xB8, 0x08};
    *pos++ = sizeof(manufacturer_value) + 1;
    *pos++ = '\xff';
    memcpy(pos, manufacturer_value, sizeof(manufacturer_value));
    pos += sizeof(manufacturer_value);

    uint16_t uuid_value = 0x1812;
    *pos++ = sizeof(uuid_value) + 1;
    *pos++ = '\x03';
    memcpy(pos, (uint8_t *)&uuid_value, sizeof(uuid_value));
    pos += sizeof(uuid_value);

    adv_data_len = ((uint32_t)pos - (uint32_t)(&advertising_data[0]));

    LOG_I("start adv");
    dev_manager_start_adv(adv_obj_hdl, advertising_data, adv_data_len, scan_response_data, scan_rsp_data_len);
}
static void create_adv_obj()
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = 0x20,
        .adv_intv_max = 0x20,
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
    LOG_I("profile:%d, start handle:0x%x\n", evt->id, evt->start_hdl);
    switch (evt->id)
    {
    case PRF_HID:
    {
        prf_hid_server_callback_init(prf_hid_server_callback);
        uint8_t ret = tinyfs_mkdir(&hid_dir, ROOT_DIR, DIR_NAME);
        if(ret != TINYFS_NO_ERROR)
        {
            LOG_I("hid tinyfs mkdir:%d",ret);
        }
        create_adv_obj();
    }break;
    case PRF_BASS:
    {
        struct hid_db_cfg db_cfg;   
        db_cfg.hids_nb = 1;
        db_cfg.cfg[0].svc_features = HID_KEYBOARD;
        db_cfg.cfg[0].report_nb = 1;
        db_cfg.cfg[0].report_id[0] = 0;
        db_cfg.cfg[0].report_cfg[0] = HID_REPORT_IN;
        db_cfg.cfg[0].info.bcdHID = 0X0111;
        db_cfg.cfg[0].info.bCountryCode = 0;
        db_cfg.cfg[0].info.flags = HID_WKUP_FOR_REMOTE | HID_NORM_CONN;
        dev_manager_prf_hid_server_add(NO_SEC, &db_cfg, sizeof(db_cfg)); 
        prf_bass_server_callback_init(prf_batt_server_callback); 
    }break;
    default:
        break;
    }
}

static void dev_manager_callback(enum dev_evt_type type, union dev_evt_u *evt)
{
    switch (type)
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
        dev_manager_get_identity_bdaddr(addr, &type);
        LOG_I("type:%d,addr:", type);
        LOG_HEX(addr, sizeof(addr));
        
        dev_manager_add_service((struct svc_decl *)&dis_server_svc);
        uptate_batt_timer_init();
    }
    break;
    case SERVICE_ADDED:
    {
        gatt_manager_svc_register(evt->service_added.start_hdl, DIS_SVC_ATT_NUM, &dis_server_svc_env);

        struct bas_db_cfg db_cfg = {
            .ins_num = 1,
            .ntf_enable[0] = 1,
        };
        dev_manager_prf_bass_server_add(NO_SEC,&db_cfg,sizeof(db_cfg));
    }
        break;
    case PROFILE_ADDED:
        prf_added_handler(&evt->profile_added);
        break;
    case ADV_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        adv_obj_hdl = evt->obj_created.handle;
        at_start_adv();
        break;
    case ADV_STOPPED:
        break;
    case SCAN_STOPPED:

        break;
    default:

        break;
    }
}

void gpio_exit_init(void)
{
    io_cfg_input(PA00);
    io_exti_config(PA00,INT_EDGE_RISING);
    io_exti_enable(PA00,true);

    io_cfg_input(PA07);
    io_exti_config(PA07,INT_EDGE_RISING);
    io_exti_enable(PA07,true);

    io_cfg_input(PB11);
    io_exti_config(PB11,INT_EDGE_RISING);
    io_exti_enable(PB11,true);

    io_cfg_input(PB15);
    io_exti_config(PB15,INT_EDGE_RISING);
    io_exti_enable(PB15,true);
}

void io_exti_callback(uint8_t pin)
{
    switch(pin)
    {
    case PA00:
    {
        uint8_t key[8]= {0};
        key[2] =0x0B;
        app_hid_send_keyboard_report(0,key,8,0);
        key[2] =0x00;
        app_hid_send_keyboard_report(0,key,8,0);
    }
    break;
    case PA07:
    {
        uint8_t key[8]= {0};
        key[2] =0x04;
        app_hid_send_keyboard_report(0,key,8,0);
        key[2] =0x00;
        app_hid_send_keyboard_report(0,key,8,0);
    }
    break;
    case PB11:
    {
        uint8_t key[8]= {0};
        key[2] =0x05;
        app_hid_send_keyboard_report(0,key,8,0);
        key[2] =0x00;
        app_hid_send_keyboard_report(0,key,8,0);
    }
    break;
    case PB15:
    {
        uint8_t key[8]= {0};
        key[2] =0x06;
        app_hid_send_keyboard_report(0,key,8,0);
        key[2] =0x00;
        app_hid_send_keyboard_report(0,key,8,0);
    }
    break;
    default:

        break;
    }
}

int main()
{
    sys_init_app();
    gpio_exit_init();
    ble_init();
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
}
