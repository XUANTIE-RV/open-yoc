/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "aos/ble.h"
#include "app_main.h"
#include "app_init.h"
#include "pin_name.h"
#include <yoc/bas.h>
#include <yoc/hids.h>
#include <yoc/bas.h>
#include <yoc/dis.h>

#define TAG "DEMO"

#define DEVICE_NAME "YoC-KEYBOARD"

/* send_data.Code -> key map
    0x00    Reserved (no event indicated)
    0x01    Keyboard ErrorRollOver
    0x02    Keyboard POSTFail
    0x03    Keyboard ErrorUndefined
    0x04    Keyboard a and A
    0x05    Keyboard b and B
    0x06    Keyboard c and C
    0x07    Keyboard d and D
    0x08    Keyboard e and E
    0x09    Keyboard f and F
    0x0A    Keyboard g and G
    0x0B    Keyboard h and H
    0x0C    Keyboard i and I
    0x0D    Keyboard j and J
    0x0E    Keyboard k and K
    0x0F    Keyboard l and L
    0x10    Keyboard m and M
    0x11    Keyboard n and N
    0x12    Keyboard o and O
    0x13    Keyboard p and P
    0x14    Keyboard q and Q
    0x15    Keyboard r and R
    0x16    Keyboard s and S
    0x17    Keyboard t and T
    0x18    Keyboard u and U
    0x19    Keyboard v and V
    0x1A    Keyboard w and W
    0x1B    Keyboard x and X
    0x1C    Keyboard y and Y
    0x1D    Keyboard z and Z
    0x1E    Keyboard 1 and !
    0x1F    Keyboard 2 and @
    0x20    Keyboard 3 and #
    0x21    Keyboard 4 and $
    0x22    Keyboard 5 and %
    0x23    Keyboard 6 and ^
    0x24    Keyboard 7 and &
    0x25    Keyboard 8 and *
    0x26    Keyboard 9 and (
    0x27    Keyboard 0 and )
    0x28    Keyboard Return (ENTER)
    0x29    Keyboard ESCAPE
    0x2A    Keyboard DELETE (Backspace)
    0x2B    Keyboard Tab
    0x2C    Keyboard Spacebar
    0x2D    Keyboard - and (underscore)
    0x2E    Keyboard = and +
    0x2F    Keyboard [ and {
    0x30    Keyboard ] and }
    0x31    Keyboard \ and |
    0x32    Keyboard Non-US # and ~
    0x33    Keyboard ; and :
    0x34    Keyboard ' and "
    0x35    Keyboard Grave Accent and Tilde
    0x36    Keyboard, and <
    0x37    Keyboard . and >
    0x38    Keyboard / and ?
    0x39    Keyboard Caps Lock
    0x3A    Keyboard F1
    0x3B    Keyboard F2
    0x3C    Keyboard F3
    0x3D    Keyboard F4
    0x3E    Keyboard F5
    0x3F    Keyboard F6
    0x40    Keyboard F7
    0x41    Keyboard F8
    0x42    Keyboard F9
    0x43    Keyboard F10
    0x44    Keyboard F11
    0x45    Keyboard F12
    0x46    Keyboard PrintScreen
    0x47    Keyboard Scroll Lock
    0x48    Keyboard Pause
    0x49    Keyboard Insert
    0x4A    Keyboard Home
    0x4B    Keyboard PageUp
    0x4C    Keyboard Delete Forward
    0x4D    Keyboard End
    0x4E    Keyboard PageDown
    0x4F    Keyboard RightArrow
    0x50    Keyboard LeftArrow
    0x51    Keyboard DownArrow
    0x52    Keyboard UpArrow
    0x53    Keypad Num Lock and Clear
    0x54    Keypad /
    0x55    Keypad *
    0x56    Keypad -
    0x57    Keypad +
    0x58    Keypad ENTER
    0x59    Keypad 1 and End
    0x5A    Keypad 2 and Down Arrow
    0x5B    Keypad 3 and PageDn
    0x5C    Keypad 4 and Left Arrow
    0x5D    Keypad 5
    0x5E    Keypad 6 and Right Arrow
    0x5F    Keypad 7 and Home
    0x60    Keypad 8 and Up Arrow
    0x61    Keypad 9 and PageUp
    0x62    Keypad 0 and Insert
    0x63    Keypad . and Delete
    0x64    Keyboard Non-US \ and |
    0x65    Keyboard Application
    0x66    Keyboard Power
    0x67    Keypad =
    0x68    Keyboard F13
    0x69    Keyboard F14
    0x6A    Keyboard F15
    0x6B    Keyboard F16
    0x6C    Keyboard F17
    0x6D    Keyboard F18
    0x6E    Keyboard F19
    0x6F    Keyboard F20
    0x70    Keyboard F21
    0x71    Keyboard F22
    0x72    Keyboard F23
    0x73    Keyboard F24
    0x74    Keyboard Execute
    0x75    Keyboard Help
    0x76    Keyboard Menu
    0x77    Keyboard Select
    0x78    Keyboard Stop
    0x79    Keyboard Again
    0x7A    Keyboard Undo
    0x7B    Keyboard Cut
    0x7C    Keyboard Copy
    0x7D    Keyboard Paste
    0x7E    Keyboard Find
    0x7F    Keyboard Mute
    0x80    Keyboard Volume Up
    0x81    Keyboard Volume Down
    0x82    Keyboard Locking Caps Lock
    0x83    Keyboard Locking Num Lock
    0x84    Keyboard Locking Scroll Lock
    0x85    Keypad Comma
    0x86    Keypad Equal Sign
    0x87    Keyboard International1
    0x88    Keyboard International2
    0x89    Keyboard International3
    0x8A    Keyboard International4
    0x8B    Keyboard International5
    0x8C    Keyboard International6
    0x8D    Keyboard International7
    0x8E    Keyboard International8
    0x8F    Keyboard International9
    0x90    Keyboard LANG1
    0x91    Keyboard LANG2
    0x92    Keyboard LANG3
    0x93    Keyboard LANG4
    0x94    Keyboard LANG5
    0x95    Keyboard LANG6
    0x96    Keyboard LANG7
    0x97    Keyboard LANG8
    0x98    Keyboard LANG9
    0x99    Keyboard Alternate Erase
    0x9A    Keyboard SysReq/Attention
    0x9B    Keyboard Cancel
    0x9C    Keyboard Clear
    0x9D    Keyboard Prior
    0x9E    Keyboard Return
    0x9F    Keyboard Separator
    0xA0    Keyboard Out
    0xA1    Keyboard Oper
    0xA2    Keyboard Clear/Again
    0xA3    Keyboard CrSel/Props
    0xA4    Keyboard ExSel
    0xE0    Keyboard LeftControl
    0xE1    Keyboard LeftShift
    0xE2    Keyboard LeftAlt
    0xE3    Keyboard Left GUI
    0xE4    Keyboard RightControl
    0xE5    Keyboard RightShift
    0xE6    Keyboard RightAlt
    0xE7    Keyboard Right GUI
*/

static uint8_t report_map[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x06, // Usage (Keyboard)

    0xA1, 0x01, // Collection (Application)
    0x05, 0x07, // Usage Page (Key Codes)
    0x19, 0xe0, // Usage Minimum (0xE0 -> LeftControl)
    0x29, 0xe7, // Usage Maximum (0xE7 -> Right GUI)
    0x15, 0x00, // Logical Minimum (0)
    0x25, 0x01, // Logical Maximum (1)
    0x75, 0x01, // Report Size (1)
    0x95, 0x08, // Report Count (8)
    0x81, 0x02, // Input (Data, Variable, Absolute)
    0x95, 0x01, // Report Count (1)
    0x75, 0x08, // Report Size (8)
    0x81, 0x01, // Input (Constant) reserved byte(1)
    // LED输出报告
    0x95, 0x05, // Report Count (5)
    0x75, 0x01, // Report Size (1)
    0x05, 0x08, // Usage Page (Page# for LEDs)
    0x19, 0x01, // Usage Minimum (1)
    0x29, 0x05, // Usage Maximum (5)
    0x91, 0x02, // Output (Data, Variable, Absolute), Led report
    0x95, 0x01, // Report Count (1)
    0x75, 0x03, // Report Size (3)
    0x91, 0x01, // Output (Data, Variable, Absolute), Led report padding

    0x95, 0x06, // Report Count (6)
    0x75, 0x08, // Report Size (8)
    0x15, 0x00, // Logical Minimum (0)
    0x25, 0x65, // Logical Maximum (101)
    0x05, 0x07, // Usage Page (Key codes)
    0x19, 0x00, // Usage Minimum (0)
    0x29, 0x65, // Usage Maximum (101)  0x65    Keyboard Application
    0x81, 0x00, // Input (Data, Array) Key array(6 bytes)
    /*
        0x09, 0x05,       // Usage (Vendor Defined)
        0x15, 0x00,       // Logical Minimum (0)
        0x26, 0xFF, 0x00, // Logical Maximum (255)
        0x75, 0x08,       // Report Count (2)
        0x95, 0x02,       // Report Size (8 bit)
        0xB1, 0x02,       // Feature (Data, Variable, Absolute)
    */
    0xC0 // End Collection (Application)
};

typedef struct _SPECIAL_KEY_VALUE_ {
    uint8_t Left_Ctrl   : 1;
    uint8_t Left_Shift  : 1;
    uint8_t Left_Alt    : 1;
    uint8_t Left_Gui    : 1;
    uint8_t Right_Ctrl  : 1;
    uint8_t Right_Shift : 1;
    uint8_t Right_Alt   : 1;
    uint8_t Right_Gui   : 1;
} SPECIAL_KEY_VALUE_BIT;

typedef union
{
    SPECIAL_KEY_VALUE_BIT bits;
    uint8_t               data;
} SPCL_KEY;

typedef struct press_key {
    SPCL_KEY keydata;
    uint8_t  Rsv;
    uint8_t  Code1;
    uint8_t  Code2;
    uint8_t  Code3;
    uint8_t  Code4;
    uint8_t  Code5;
    uint8_t  Code6;
} press_key_data;

static uint8_t report_output_data[1] = {
    0x00,
};

typedef struct _KEYBOARD_LED_ {
    uint8_t Num_Lock    : 1;
    uint8_t Cap_Lock    : 1;
    uint8_t Scroll_Lock : 1;
    uint8_t Compose     : 1;
    uint8_t Kana        : 1;
    uint8_t Rsv         : 3;
} KEYBOARD_LED_BIT;

typedef union
{
    KEYBOARD_LED_BIT bits;
    uint8_t          data;
} CTRL_LED;

static press_key_data send_data;

static int16_t    g_conn_handle   = -1;
static int16_t    g_paired_hanlde = -1;
static dev_addr_t g_paired_addr;

static bas_handle_t  g_bas_handle  = NULL;
static dis_handle_t  g_dis_handle  = NULL;
static hids_handle_t g_hids_handle = NULL;
static bas_t         g_bas;
static aos_sem_t     sync_sem;
#define MANUFACTURER_NAME "PINGTOUGE"
#define MODEL_NUMBER      "MODE_KEYBOARD"
#define SERIAL_NUMBER     "00000001"
#define HW_REV            "0.0.1"
#define FW_REV            "0.0.2"
#define SW_REV            "0.0.3"

static pnp_id_t pnp_id = {
    VEND_ID_SOURCE_USB,
    0x1915,
    0xEEEE,
    0x0001,
};

static dis_info_t dis_info = {
    MANUFACTURER_NAME, MODEL_NUMBER, SERIAL_NUMBER, HW_REV, FW_REV, SW_REV, NULL, NULL, &pnp_id,
};

static void start_adv(void)
{
    ad_data_t ad[3] = { 0 };
    uint8_t   flag  = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;
    ad[0].type      = AD_DATA_TYPE_FLAGS;
    ad[0].data      = (uint8_t *)&flag;
    ad[0].len       = 1;

    uint8_t uuid16_list[] = { 0x12, 0x18, 0x0f, 0x18 }; /* UUID_BAS, UUID_HIDS */
    ad[1].type            = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data            = (uint8_t *)uuid16_list;
    ad[1].len             = sizeof(uuid16_list);

    ad[2].type = AD_DATA_TYPE_GAP_APPEARANCE;
    ad[2].data = (uint8_t *)(uint8_t[]){ 0xc1, 0x03 };
    ad[2].len  = 2;

    adv_param_t param = {
        ADV_IND, ad, NULL, BLE_ARRAY_NUM(ad), 0, ADV_FAST_INT_MIN_1, ADV_FAST_INT_MAX_1,
    };

    int ret = ble_stack_adv_start(&param);

    if (ret) {
        LOGE(TAG, "adv start fail %d!", ret);
    } else {
        LOGE(TAG, "adv start!");
    }
}

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED && e->err == 0) {
        g_conn_handle = e->conn_handle;
        LOGI(TAG, "Connected");
    } else {
        g_conn_handle = -1;
        LOGI(TAG, "Disconnected err %d", e->err);
        aos_sem_signal(&sync_sem);
    }
}

static void event_pairing_passkey_display(ble_event_en event, void *event_data)
{
    evt_data_smp_passkey_display_t *e = (evt_data_smp_passkey_display_t *)event_data;

    LOGI(TAG, "passkey is %s", e->passkey);
}

static void event_smp_complete(ble_event_en event, void *event_data)
{
    evt_data_smp_pairing_complete_t *e = (evt_data_smp_pairing_complete_t *)event_data;

    if (e->err == 0) {
        g_paired_addr   = e->peer_addr;
        g_paired_hanlde = e->conn_handle;
    }

    LOGI(TAG, "pairing %s!!!", e->err ? "FAIL" : "SUCCESS");
}

static void event_smp_cancel(ble_event_en event, void *event_data)
{
    LOGI(TAG, "pairing cancel");
}

void Light_Led(CTRL_LED usdata)
{
    if (usdata.bits.Cap_Lock) {
        LOGI(TAG, "Cap_Lock on\r\n");
    }

    if (usdata.bits.Num_Lock) {
        LOGI(TAG, "Num_Lock on\r\n");
    }

    if (usdata.bits.Scroll_Lock) {
        LOGI(TAG, "Scroll_Lock on\r\n");
    }

    if (usdata.bits.Compose) {
        LOGI(TAG, "Compose on\r\n");
    }

    if (usdata.bits.Kana) {
        LOGI(TAG, "Kana on\r\n");
    }
}

static void event_output_write(ble_event_en event, void *event_data)
{
    CTRL_LED led_status;

    evt_data_gatt_char_write_t *e = (evt_data_gatt_char_write_t *)event_data;
    LOGI(TAG, "EVENT_GATT_CHAR_WRITE_CB,%x,%x\r\n", e->len, e->data[0]);

    if (e->len == 1) {
        report_output_data[0] = e->data[0];
        led_status.data       = e->data[0];
        Light_Led(led_status);
    }
}

static void smp_pairing_confirm(evt_data_smp_pairing_confirm_t *e)
{
    LOGD(TAG, "Confirm pairing for");
}

static void conn_security_change(ble_event_en event, void *event_data)
{
    evt_data_gap_security_change_t *e = (evt_data_gap_security_change_t *)event_data;
    LOGD(TAG, "conn %d security level change to level %d\n", e->conn_handle, e->level);
    (void)e;
}

static void conn_param_update(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_param_update_t *e = event_data;

    LOGD(TAG, "LE conn param updated: int 0x%04x lat %d to %d\n", e->interval, e->latency, e->timeout);
    (void)e;
}

static void mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = (evt_data_gatt_mtu_exchange_t *)event_data;

    if (e->err == 0) {
        LOGI(TAG, "mtu exchange, MTU %d", ble_stack_gatt_mtu_get(e->conn_handle));
    } else {
        LOGE(TAG, "mtu exchange fail, %x", e->err);
    }
}

static void identity_address_resolved(ble_event_en event, void *event_data)
{
    evt_data_smp_identity_address_t *e = (evt_data_smp_identity_address_t *)event_data;
    LOGI(TAG, "rpa %02x:%02x:%02x:%02x:%02x:%02x (type %d)", e->rpa.val[5], e->rpa.val[4], e->rpa.val[3], e->rpa.val[2],
         e->rpa.val[1], e->rpa.val[0], e->rpa.type);
    LOGI(TAG, "id  %02x:%02x:%02x:%02x:%02x:%02x (type %d)", e->identity_addr.val[5], e->identity_addr.val[4],
         e->identity_addr.val[3], e->identity_addr.val[2], e->identity_addr.val[1], e->identity_addr.val[0],
         e->identity_addr.type);
}

static int event_callback(ble_event_en event, void *event_data)
{
    LOGD(TAG, "D event %x\n", event);

    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GAP_CONN_PARAM_UPDATE:
            conn_param_update(event, event_data);
            break;

        case EVENT_SMP_PASSKEY_DISPLAY:
            event_pairing_passkey_display(event, event_data);
            break;

        case EVENT_SMP_PAIRING_COMPLETE:
            event_smp_complete(event, event_data);
            break;

        case EVENT_SMP_PAIRING_CONFIRM:
            smp_pairing_confirm(event_data);

            ble_stack_smp_passkey_confirm(g_conn_handle);
            break;

        case EVENT_SMP_CANCEL:
            event_smp_cancel(event, event_data);
            break;

        case EVENT_GAP_CONN_SECURITY_CHANGE:
            conn_security_change(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            mtu_exchange(event, event_data);
            break;

        case EVENT_GAP_ADV_TIMEOUT:
            start_adv();
            break;

        case EVENT_SMP_IDENTITY_ADDRESS_GET:
            identity_address_resolved(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}

int key_board_send(press_key_data *senddata)
{
    int iflag = 0;

    // code data not 0
    if (g_conn_handle != -1) {
        iflag = ble_prf_hids_key_send(g_hids_handle, (uint8_t *)(senddata), sizeof(send_data));
        memset(senddata, 0, sizeof(press_key_data));
        iflag |= ble_prf_hids_key_send(g_hids_handle, (uint8_t *)(senddata), sizeof(send_data));
        return iflag;
    }

    return -1;
}

#ifdef AOS_COMP_CLI
static void cmd_keysend_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 2) {
        if (g_conn_handle != -1) {
            memset(&send_data, 0, sizeof(send_data));
            send_data.Code6 = atoi(argv[1]);
            key_board_send(&send_data);
            LOGD(TAG, "key send %d", atoi(argv[1]));
        }
    }
}

void cli_reg_cmd_keysend(void)
{
    static const struct cli_command cmd_info = {
        "keysend",
        "keysend commands",
        cmd_keysend_func,
    };

    aos_cli_register_command(&cmd_info);
}
#endif

static ble_event_cb_t ble_cb = {
    .callback = event_callback,
};

int main()
{
    int s_flag;
    int ret = 0;

    board_yoc_init();

    init_param_t init = {
        .dev_name     = DEVICE_NAME,
        .dev_addr     = NULL,
        .conn_num_max = 1,
    };

#if defined(AOS_COMP_CLI) || defined(USE_CLI)
    cli_reg_cmd_keysend();
#endif
    aos_sem_new(&sync_sem, 1);

    LOGI(TAG, "Bluetooth HID demo!");

    ble_stack_init(&init);

    ble_stack_setting_load();

    ret = ble_stack_event_register(&ble_cb);

    if (ret) {
        LOGE(TAG, "register event failed");
        return -1;
    }

    ble_stack_iocapability_set(IO_CAP_IN_YESNO | IO_CAP_OUT_NONE);

    g_bas_handle = ble_prf_bas_init(&g_bas);

    if (g_bas_handle == NULL) {
        LOGE(TAG, "BAS init FAIL!!!!");
        return -1;
    }

    g_dis_handle = ble_prf_dis_init(&dis_info);

    if (g_dis_handle == NULL) {
        LOGE(TAG, "DIS init FAIL!!!!");
        return -1;
    }

    g_hids_handle = ble_prf_hids_init(HIDS_REPORT_PROTOCOL_MODE);

    if (g_hids_handle == NULL) {
        LOGE(TAG, "HIDS init FAIL!!!!");
        return -1;
    }

    s_flag = ble_prf_hids_set_data_map(report_map, sizeof(report_map), REPORT_MAP);

    if (s_flag == -1) {
        LOGE(TAG, "set_report_map FAIL!!!!");
        return s_flag;
    }

    s_flag = ble_prf_hids_set_data_map((uint8_t *)(&send_data), sizeof(send_data), REPORT_INPUT);

    if (s_flag == -1) {
        LOGE(TAG, "set_report_input FAIL!!!!");
        return s_flag;
    }

    s_flag = ble_prf_hids_set_data_map(report_output_data, sizeof(report_output_data), REPORT_OUTPUT);

    if (s_flag == -1) {
        LOGE(TAG, "set_report_output FAIL!!!!");
        return s_flag;
    }

    s_flag = ble_prf_hids_regist(HIDS_IDX_REPORT_OUTPUT_VAL, (void *)event_output_write);

    if (s_flag == -1) {
        LOGE(TAG, "ble_prf_hids_regist FAIL!!!!");
        return s_flag;
    }

    while (1) {
        aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);

        if (g_conn_handle == -1) {
            start_adv();
        }
    }

    return 0;
}
