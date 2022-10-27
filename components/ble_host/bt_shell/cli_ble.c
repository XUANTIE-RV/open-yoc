/**
 *
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 */

#include <aos/aos.h>
#include <aos/bt.h>
#include <aos/cli.h>

/// Extern Functions from cli_bt.c
extern int         cli_bt_bt_addr_str_to_struct(const char *str, const char *type, bt_dev_addr_t *addr);
extern const char *cli_bt_bt_dev_addr_to_string(bt_dev_addr_t *addr);
extern int  cli_bt_bt_data_parse(uint8_t *data, size_t data_len, int (*cb)(bt_data_t *data, void *arg), void *cb_arg);
extern void cli_bt_hexdump(const u8_t *data, size_t len);
extern const char *cli_bt_hex(const void *buf, size_t len);
extern int         cli_bt_str_to_hex(uint8_t *hex, size_t size, char *s);
extern char *      cli_bt_uuid_str(uuid_t *uuid);

typedef struct _test_sevice_t {
    uint16_t used             : 1;
    uint16_t indicate_ongoing : 1;
    uint16_t paired           : 1;
    uint16_t ccc              : 2;
    uint16_t security         : 3;
    int16_t  conn_handle;
    uint16_t mtu;
} test_service_t;

static uint8_t  ble_init_flag      = 0;
static uint8_t *pscan_ad           = NULL;
static uint8_t *pscan_sd           = NULL;
static int16_t  bt_cur_conn_handle = -1;
static uint8_t  scan_filter        = 0;

static test_service_t test_service_pool[CONFIG_BT_MAX_CONN] = { 0 };

static test_service_t *_test_sevice_find(uint16_t conn_handle, uint8_t alloc)
{
    int             i;
    test_service_t *unused = NULL;
    for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        if (test_service_pool[i].used && test_service_pool[i].conn_handle == conn_handle) {
            /* found the test service */
            return &test_service_pool[i];
        }

        if (!test_service_pool[i].used) {
            unused = &test_service_pool[i];
        }
    }

    /* not found test service, alloc new one */
    if (alloc) {
        unused->used        = 1;
        unused->conn_handle = conn_handle;
        return unused;
    }

    return NULL;
}

static void _test_service_free(uint16_t conn_handle)
{
    test_service_t *test_service = _test_sevice_find(conn_handle, 0);

    memset(test_service, 0, sizeof(*test_service));
}

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    test_service_t *test_service = _test_sevice_find(e->conn_handle, 1);

    if (!test_service) {
        printf("can't find test service, conn_handle %d\n", e->conn_handle);
        return;
    }

    connect_info_t info;
    ble_stack_connect_info_get(e->conn_handle, &info);

    if (e->connected == CONNECTED && e->err == 0) {
        printf("Connected (%d): %s\n", e->conn_handle, cli_bt_bt_dev_addr_to_string(&info.peer_addr));
        test_service->conn_handle = e->conn_handle;
        bt_cur_conn_handle = e->conn_handle;
    } else {
        _test_service_free(e->conn_handle);
        bt_cur_conn_handle = -1;
        printf("Disconected (%d): %s err %d\n", e->conn_handle, cli_bt_bt_dev_addr_to_string(&info.peer_addr), e->err);
    }
}

static void conn_security_change(ble_event_en event, void *event_data)
{
    evt_data_gap_security_change_t *e            = (evt_data_gap_security_change_t *)event_data;
    test_service_t *                test_service = _test_sevice_find(e->conn_handle, 1);

    if (!test_service) {
        printf("can't find test service, conn_handle %d\n", e->conn_handle);
        return;
    }

    test_service->security = e->level;

    printf("conn %d security level change to level%d\n", e->conn_handle, e->level);
}

static int scan_ad_cmp(bt_data_t *ad, void *arg)
{
    bt_data_t *ad2 = arg;

    return (ad->type == ad2->type && ad->len == ad2->len && 0 == memcmp(ad->data, ad2->data, ad->len));
}

static int scan_ad_callback(bt_data_t *ad, void *arg)
{
    evt_data_gap_dev_find_t *e = arg;
    int                      ret;

    ret = cli_bt_bt_data_parse(e->adv_data, e->adv_len, scan_ad_cmp, (void *)ad);
    if (ret < 0) {
        /* adv data invaild, break */
        return 1;
    } else if (ret == 0) {
        /* not match, break */
        return 1;
    } else if (ret == 1) {
        /* matched, continue */
        return 0;
    }

    return 1;
}

static void device_find(ble_event_en event, void *event_data)
{
    evt_data_gap_dev_find_t *e = event_data;
    int                      ret;
    uint8_t *                pattern = { 0 };

    if (e->adv_len) {
        if (e->adv_type == ADV_SCAN_IND) {
            pattern = pscan_sd;
        } else {
            pattern = pscan_ad;
        }
    }

    if (pattern) {
        ret = cli_bt_bt_data_parse(pattern, 31, scan_ad_callback, e);

        if (ret == 0) {
#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV > 0
            printf("[DEVICE]: %s, adv type %d, rssi %d, len %d, sid %d, Raw data:%s",
                   cli_bt_bt_dev_addr_to_string(&e->dev_addr), e->adv_type, e->rssi, e->adv_len, e->sid,
                   cli_bt_hex(e->adv_data, e->adv_len > 31 ? 31 : e->adv_len));
#else
            printf("[DEVICE]: %s, adv type %d, rssi %d, len %d, Raw data:%s",
                   cli_bt_bt_dev_addr_to_string(&e->dev_addr), e->adv_type, e->rssi, e->adv_len,
                   cli_bt_hex(e->adv_data, e->adv_len > 31 ? 31 : e->adv_len));
#endif
            if (e->adv_len > 31) {
                uint8_t len = e->adv_len - 31 < 10 ? e->adv_len - 31 : 10;
                printf(", Tail Data: %s", cli_bt_hex(e->adv_data + e->adv_len - len, len));
            }
            printf("\n");
        }
    } else {
#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV > 0
        printf("[DEVICE]: %s, adv type %d, rssi %d, len %d, sid %d, Raw data:%s",
               cli_bt_bt_dev_addr_to_string(&e->dev_addr), e->adv_type, e->rssi, e->adv_len, e->sid,
               cli_bt_hex(e->adv_data, e->adv_len > 31 ? 31 : e->adv_len));
#else
        printf("[DEVICE]: %s, adv type %d, rssi %d, len %d, Raw data:%s", cli_bt_bt_dev_addr_to_string(&e->dev_addr),
               e->adv_type, e->rssi, e->adv_len, cli_bt_hex(e->adv_data, e->adv_len > 31 ? 31 : e->adv_len));
#endif
        if (e->adv_len > 31) {
            uint8_t len = e->adv_len - 31 < 10 ? e->adv_len - 31 : 10;
            printf(", Tail Data: %s", cli_bt_hex(e->adv_data + e->adv_len - len, len));
        }
        printf("\n");
    }
}

static void conn_param_req(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_param_req_t *e = event_data;
    printf("LE conn  param req: int (0x%04x, 0x%04x) lat %d to %d\n", e->param.interval_min, e->param.interval_max,
           e->param.latency, e->param.timeout);

    e->accept = 1;
}

static void conn_param_update(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_param_update_t *e = event_data;

    printf("LE conn param updated: int 0x%04x lat %d to %d\n", e->interval, e->latency, e->timeout);
}
#if defined(CONFIG_BT_SMP) && CONFIG_BT_SMP

static void smp_passkey_display(evt_data_smp_passkey_display_t *e)
{

    printf("Passkey for %s: %s\n", cli_bt_bt_dev_addr_to_string(&e->peer_addr), e->passkey);
}

static void smp_passkey_confirm(evt_data_smp_passkey_confirm_t *e)
{
    printf("Pairing passkey for %s: %s\n", cli_bt_bt_dev_addr_to_string(&e->peer_addr), e->passkey);
}

static void smp_passkey_entry(evt_data_smp_passkey_enter_t *e)
{
    printf("Enter passkey for %s\n", cli_bt_bt_dev_addr_to_string(&e->peer_addr));
}

static void smp_cancel(evt_data_smp_cancel_t *e)
{
    printf("Pairing cancelled: %s\n", cli_bt_bt_dev_addr_to_string(&e->peer_addr));
}

static void smp_pairing_confirm(evt_data_smp_pairing_confirm_t *e)
{
    printf("Confirm pairing for %s\n", cli_bt_bt_dev_addr_to_string(&e->peer_addr));
}

static void smp_pairing_complete(evt_data_smp_pairing_complete_t *e)
{
    if (e->err) {
        printf("Pairing failed with %s, err %d\n", cli_bt_bt_dev_addr_to_string(&e->peer_addr), e->err);
    } else {
        printf("%s with %s\n", e->bonded ? "Bonded" : "Paired", cli_bt_bt_dev_addr_to_string(&e->peer_addr));
    }
}

static void smp_event(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_SMP_PASSKEY_DISPLAY:
            smp_passkey_display(event_data);
            break;

        case EVENT_SMP_PASSKEY_CONFIRM:
            smp_passkey_confirm(event_data);
            break;

        case EVENT_SMP_PASSKEY_ENTER:
            smp_passkey_entry(event_data);
            break;

        case EVENT_SMP_PAIRING_CONFIRM:
            smp_pairing_confirm(event_data);
            break;

        case EVENT_SMP_PAIRING_COMPLETE:
            smp_pairing_complete(event_data);
            break;

        case EVENT_SMP_CANCEL:
            smp_cancel(event_data);
            break;

        default:
            break;
    }
}

#endif /* CONFIG_BT_SMP */

static int ble_stack_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GAP_CONN_SECURITY_CHANGE:
            conn_security_change(event, event_data);
            break;

        case EVENT_GAP_DEV_FIND:
            device_find(event, event_data);
            break;

        case EVENT_GAP_CONN_PARAM_REQ:
            conn_param_req(event, event_data);
            break;

        case EVENT_GAP_CONN_PARAM_UPDATE:
            conn_param_update(event, event_data);
            break;
#if defined(CONFIG_BT_SMP) && CONFIG_BT_SMP
        case EVENT_SMP_PASSKEY_DISPLAY:
        case EVENT_SMP_PASSKEY_CONFIRM:
        case EVENT_SMP_PASSKEY_ENTER:
        case EVENT_SMP_PAIRING_CONFIRM:
        case EVENT_SMP_PAIRING_COMPLETE:
        case EVENT_SMP_CANCEL:
            smp_event(event, event_data);
            break;
#endif

        default:
            break;
    }

    return 0;
}

static ble_event_cb_t ble_stack_event_cb = {
    .callback = ble_stack_event_callback,
};

static void cmd_ble_stack_init(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int           ret;
    bt_dev_addr_t addr;
    init_param_t  init = { NULL, NULL, 1 };

    if (argc == 3) {
        ret = cli_bt_bt_addr_str_to_struct(argv[1], argv[2], &addr);
        if (ret) {
            printf("Invalid address\n");
            return;
        }

        init.dev_addr = &addr;
    } else if (argc == 1) {
    } else {
        printf("Invalid param\n");
    }

    ret = ble_stack_init(&init);

    ble_stack_setting_load();

    if (ret) {
        printf("Bluetooth init failed (err %d)\n", ret);
        return;
    }

    ret = ble_stack_event_register(&ble_stack_event_cb);
    if (ret) {
        printf("Bluetooth stack init fail\n");
        return;
    } else {
        printf("Bluetooth stack init success\n");
    }

    ble_init_flag = 1;
}

static void cmd_ble_stack_set_name(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret;

    if (argc < 2) {
        printf("Bluetooth Local Name: %s\n", bt_stack_get_name());
        return;
    }

    ret = ble_stack_set_name(argv[1]);

    if (ret) {
        printf("Unable to set name %s (err %d)", argv[1], ret);
    }
}

static void cmd_ble_stack_iocapability_set(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int     ret;
    uint8_t io_cap = 0;

    if (argc < 3) {
        return;
    }

    if (!strcmp(argv[1], "NONE")) {
        io_cap |= IO_CAP_IN_NONE;
    } else if (!strcmp(argv[1], "YESNO")) {
        io_cap |= IO_CAP_IN_YESNO;
    } else if (!strcmp(argv[1], "KEYBOARD")) {
        io_cap |= IO_CAP_IN_KEYBOARD;
    } else {
        return;
    }

    if (!strcmp(argv[2], "NONE")) {
        io_cap |= IO_CAP_OUT_NONE;
    } else if (!strcmp(argv[2], "DISPLAY")) {
        io_cap |= IO_CAP_OUT_DISPLAY;
    } else {
        return;
    }

    ret = ble_stack_iocapability_set(io_cap);
    if (ret) {
        printf("set io capablity %s %s err %d\n", argv[1], argv[2], ret);
        return;
    }

    printf("set io capablity %s %s success\n", argv[1], argv[2]);
}

static inline int _bt_data_set(bt_data_t *data, void *arg)
{
    bt_data_t **pbt_data = arg;

    **pbt_data = *data;

    (*pbt_data)++;

    return 0;
}

static inline int _bt_data_num(bt_data_t *data, void *arg)
{
    int *num = arg;

    (*num)++;

    return 0;
}

static void cmd_ble_stack_adv(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int         ret;
    adv_param_t param      = { 0 };
    uint8_t     ad_hex[31] = { 0 };
    size_t      ad_hex_len = 0;
    uint8_t     sd_hex[31] = { 0 };
    size_t      sd_hex_len = 0;
    size_t      ad_num     = 0;
    size_t      sd_num     = 0;

    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    if (!strcmp(argv[1], "stop")) {
        ret = ble_stack_adv_stop();
        if (ret) {
            printf("Failed to stop advertising\n");
        } else {
            printf("Advertising stopped\n");
        }

        return;
    }

    if (!strcmp(argv[1], "nconn")) {
        param.type = ADV_NONCONN_IND;
    } else if (!strcmp(argv[1], "conn")) {
        param.type = ADV_IND;
    }

    if (argc > 2) {
        if (strlen(argv[2]) > 62 || strlen(argv[2]) % 1) {
            printf("ad data invaild %ld\n", strlen(argv[2]));
            return;
        }

        ad_hex_len = cli_bt_str_to_hex(ad_hex, sizeof(ad_hex), argv[2]);
        if (ad_hex_len == 0) {
            printf("ad data invaild %s\n", argv[2]);
            return;
        }

        ret = cli_bt_bt_data_parse(ad_hex, ad_hex_len, _bt_data_num, &ad_num);
        if (ret < 0) {
            printf("invaild ad data %s\n", argv[2]);
            return;
        }
    }

    if (argc > 3) {
        if (strlen(argv[3]) > 62 || strlen(argv[3]) % 1) {
            printf("sd data invaild %ld\n", strlen(argv[3]));
            return;
        }

        sd_hex_len = cli_bt_str_to_hex(sd_hex, sizeof(sd_hex), argv[3]);
        if (sd_hex_len == 0) {
            printf("sd data invaild %s\n", argv[2]);
            return;
        }

        ret = cli_bt_bt_data_parse(sd_hex, sd_hex_len, _bt_data_num, &sd_num);
        if (ret < 0) {
            printf("invaild sd data %s\n", argv[2]);
            return;
        }
    }

    bt_data_t ad[ad_num];
    bt_data_t sd[sd_num];

    bt_data_t *pbt_data = ad;

    if (ad_num) {
        cli_bt_bt_data_parse(ad_hex, ad_hex_len, _bt_data_set, &pbt_data);
    }

    pbt_data = sd;

    if (sd_num) {
        cli_bt_bt_data_parse(sd_hex, sd_hex_len, _bt_data_set, &pbt_data);
    }

    param.ad           = ad_num > 0 ? ad : NULL;
    param.sd           = sd_num > 0 ? sd : NULL;
    param.ad_num       = (uint8_t)ad_num;
    param.sd_num       = (uint8_t)sd_num;
    param.interval_min = ADV_FAST_INT_MIN_2;
    param.interval_max = ADV_FAST_INT_MAX_2;

    ret = ble_stack_adv_start(&param);

    if (ret) {
        printf("Failed to start advertising (err %d)\n", ret);
    } else {
        printf("adv_type:%x;adv_interval_min:%d (*0.625)ms;adv_interval_max:%d (*0.625)ms\n", param.type,
               (int)param.interval_min, (int)param.interval_max);
        printf("Advertising started\n");
    }

    return;
}

static void cmd_ble_scan_filter(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    scan_filter = atoi(argv[1]);

    if (scan_filter > SCAN_FILTER_POLICY_WHITE_LIST) {
        scan_filter = 0;
        printf("invaild param %s\n", argv[1]);
        return;
    }

    printf("Set scan filter %s\n", scan_filter == 0 ? "SCAN_FILTER_POLICY_ANY_ADV" : "SCAN_FILTER_POLICY_WHITE_LIST");
    return;
}

static void cmd_ble_stack_scan(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int            ret;
    static uint8_t scan_ad[31] = { 0 };
    static uint8_t scan_sd[31] = { 0 };

    scan_param_t params = {
        SCAN_PASSIVE,
        SCAN_FILTER_DUP_DISABLE,
        SCAN_FAST_INTERVAL,
        SCAN_FAST_WINDOW,
    };

    params.scan_filter = scan_filter;

    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    if (argc >= 2) {
        if (!strcmp(argv[1], "off")) {
            pscan_ad = NULL;
            pscan_sd = NULL;
            ret      = ble_stack_scan_stop();
            if (ret) {
                printf("scan stop err %d\n", ret);
                return;
            }

            printf("scan stop success\n");
            return;
        } else if (!strcmp(argv[1], "active")) {
            params.type = SCAN_ACTIVE;
        } else if (!strcmp(argv[1], "passive")) {
            params.type = SCAN_PASSIVE;
        } else {
            printf("invaild param %s\n", argv[1]);
            return;
        }
    }

    if (argc >= 3) {
        if (!strcmp(argv[2], "dups")) {
            params.filter_dup = SCAN_FILTER_DUP_DISABLE;
        } else if (!strcmp(argv[2], "nodups")) {
            params.filter_dup = SCAN_FILTER_DUP_ENABLE;
        } else {
            printf("invaild param %s\n", argv[2]);
            return;
        }
    }

    if (argc >= 4) {
        params.interval = strtol(argv[3], NULL, 10);
    }

    if (argc >= 5) {
        params.window = strtol(argv[4], NULL, 10);
    }

    if (argc >= 6) {
        pscan_ad = scan_ad;
        memset(scan_ad, 0, sizeof(scan_ad));
        cli_bt_str_to_hex(scan_ad, sizeof(scan_ad), argv[5]);
    }

    if (argc >= 7) {
        pscan_sd = scan_sd;
        memset(scan_sd, 0, sizeof(scan_sd));
        cli_bt_str_to_hex(scan_sd, sizeof(scan_sd), argv[6]);
    }

    ret = ble_stack_scan_start(&params);
    if (ret) {
        printf("scan start err %d\n", ret);
        return;
    }

    printf("scan start type %d filter %d interval 0x%02x window 0x%02x success\n", params.type, params.filter_dup,
           params.interval, params.window);
}

static void cmd_ble_stack_get_local_addr(char *wbuf, int wbuf_len, int argc, char **argv)
{
    bt_dev_addr_t addr;

    ble_stack_get_local_addr(&addr);

    printf("Local Device Address: %s\n", cli_bt_bt_dev_addr_to_string(&addr));
}

#if defined(CONFIG_BT_CONN) && CONFIG_BT_CONN

static void cmd_ble_stack_connect(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int           ret;
    bt_dev_addr_t addr;
    conn_param_t  param = {
        CONN_INT_MIN_INTERVAL,
        CONN_INT_MAX_INTERVAL,
        0,
        400,
    };

    if (argc < 3) {
        printf("invaild param\n");
        return;
    }

    if (argc >= 3) {
        ret = cli_bt_bt_addr_str_to_struct(argv[1], argv[2], &addr);

        if (ret) {
            printf("invalid peer address err %d\n", ret);
            return;
        }
    }

    if (argc >= 5) {
        param.interval_min = strtol(argv[3], NULL, 16);
        param.interval_max = strtol(argv[4], NULL, 16);
    }

    if (argc >= 7) {
        param.latency = strtol(argv[5], NULL, 16);
        param.timeout = strtol(argv[6], NULL, 16);
    }

    ret = ble_stack_connect(&addr, &param, 0);

    if (ret) {
        printf("connection failed err %d\n", ret);
        return;
    }

    printf("connect to %s %s success\n", argv[1], argv[2]);
}

static void cmd_ble_stack_check_conn_params(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int          check;
    conn_param_t param;

    if (argc < 5) {
        printf("invaild params\n");
        return;
    }

    param.interval_min = strtol(argv[1], NULL, 16);
    param.interval_max = strtol(argv[2], NULL, 16);
    param.latency      = strtol(argv[3], NULL, 16);
    param.timeout      = strtol(argv[4], NULL, 16);

    check = ble_stack_check_conn_params(&param);

    printf("connection param interval min 0x%02x interval max 0x%02x"
           "latency 0x%02x timeout 0x%02x check %s\n",
           param.interval_min, param.interval_max, param.latency, param.timeout, check ? "vaild" : "invaild");
}

static void cmd_ble_conn_select(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int16_t conn_handle = 0;

    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    conn_handle = strtol(argv[1], NULL, 10);

    bt_cur_conn_handle = conn_handle;

    printf("select connect handle %d\n", bt_cur_conn_handle);

    return;
}

static void cmd_ble_stack_disconnect(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int16_t conn_handle = 0;
    int     ret;

    if (bt_cur_conn_handle != -1 && argc < 2) {
        conn_handle = bt_cur_conn_handle;
    } else {
        if (argc < 2) {
            printf("invaild param\n");
            return;
        }

        conn_handle = strtol(argv[1], NULL, 10);
    }

    ret = ble_stack_disconnect(conn_handle);

    if (ret) {
        printf("Disconnection failed err %d\n", ret);
    }

    printf("Disconnection with conn handle %d success\n", conn_handle);
}

static void cmd_ble_stack_connect_info_get(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int16_t        conn_handle = 0;
    connect_info_t info;
    char           local_str[30]  = { 0 };
    char           remote_str[30] = { 0 };
    int            ret;

    if (bt_cur_conn_handle != -1 && argc < 2) {
        conn_handle = bt_cur_conn_handle;
    } else {
        if (argc < 2) {
            printf("invaild param\n");
            return;
        }

        conn_handle = strtol(argv[1], NULL, 10);
    }

    ret = ble_stack_connect_info_get(conn_handle, &info);
    if (ret) {
        printf("get connect info %d err %d", conn_handle, ret);
        return;
    }

    strcpy(local_str, cli_bt_bt_dev_addr_to_string(&info.local_addr));
    strcpy(remote_str, cli_bt_bt_dev_addr_to_string(&info.peer_addr));

    printf("connect info:\ntype %d, conn hanle %d, role %s\n"
           "interval 0x%02x latency 0x%02x timeout 0x%02x\n"
           "local device %s\n remote device %s\n",
           info.type, info.conn_handle, info.role ? "Slave" : "Master", info.interval, info.latency, info.timeout,
           local_str, remote_str);
}

#if defined(CONFIG_BT_SMP) && CONFIG_BT_SMP
static void cmd_ble_stack_security(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int err, sec;

    if (bt_cur_conn_handle == -1) {
        printf("Not connected\n");
        return;
    }

    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    sec = atoi(argv[1]);

    err = ble_stack_security(bt_cur_conn_handle, sec);

    if (err) {
        printf("Setting security failed err %d\n", err);
    }

    printf("Setting security success\n");

    return;
}

static void cmd_ble_stack_pref_phy_set(char *wbuf, int wbuf_len, int argc, char **argv)
{

    int                ret;
    uint8_t            rx_phy_rate, tx_phy_rate;
    static const char *phy_str[] = { "", "1M PHY", "2M PHY", "", "Coded PHY" };

    if (bt_cur_conn_handle < 0) {
        printf("dev not connected\r\n");
    }

    if (argc < 3) {
        printf("invaild param\n");
        return;
    }

    tx_phy_rate = strtoul(argv[1], NULL, 16);
    if (tx_phy_rate != PREF_PHY_1M && tx_phy_rate != PREF_PHY_2M && tx_phy_rate != PREF_PHY_CODED) {
        printf("invaild param tx phy %s\n", argv[1]);
        return;
    }

    rx_phy_rate = strtoul(argv[2], NULL, 16);
    if (rx_phy_rate != PREF_PHY_1M && rx_phy_rate != PREF_PHY_2M && rx_phy_rate != PREF_PHY_CODED) {
        printf("invaild param rx phy %s\n", argv[2]);
        return;
    }

    ret = ble_stack_pref_phy_set(bt_cur_conn_handle, tx_phy_rate, rx_phy_rate);
    if (ret) {
        printf("set pref phy err %d\n", ret);
        return;
    }

    printf("set pref phy tx phy: %s rx phy: %s success\n", phy_str[tx_phy_rate], phy_str[rx_phy_rate]);
}

static void cmd_ble_stack_connect_param_update(char *wbuf, int wbuf_len, int argc, char **argv)
{
    conn_param_t param;
    int          err;

    if (argc < 5) {
        printf("invaild param\n");
        return;
    }

    param.interval_min = strtoul(argv[1], NULL, 16);
    param.interval_max = strtoul(argv[2], NULL, 16);
    param.latency      = strtoul(argv[3], NULL, 16);
    param.timeout      = strtoul(argv[4], NULL, 16);
    err                = ble_stack_connect_param_update(bt_cur_conn_handle, &param);

    if (err) {
        printf("conn update failed (err %d).\n", err);
    } else {
        printf("conn update initiated.\n");
    }

    return;
}

static void cmd_ble_stack_smp_passkey_entry(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int          ret;
    unsigned int passkey;

    if (bt_cur_conn_handle == -1) {
        printf("Not connected\n");
        return;
    }

    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    passkey = atoi(argv[1]);

    if (passkey < 0 || passkey > 999999) {
        printf("Passkey should be between 0-999999\n");
        return;
    }

    ret = ble_stack_smp_passkey_entry(bt_cur_conn_handle, passkey);
    if (ret) {
        printf("passkey entry err %d\n", ret);
        return;
    }
    printf("passkey entry success\n");
}

static void cmd_ble_stack_smp_cancel(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int     ret;
    int16_t conn_handle;

    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    ret = ble_stack_smp_cancel(conn_handle);
    if (ret) {
        printf("pairing cancel err %d\n", ret);
        return;
    }
    printf("pairing cancel success\n");
}

static void cmd_ble_stack_smp_passkey_confirm(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int             ret;
    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    ret = ble_stack_smp_passkey_confirm(bt_cur_conn_handle);
    if (ret) {
        printf("passkey confirm err %d\n", ret);
        return;
    }
    printf("passkey confirm success\n");
    return;
}

static void cmd_ble_stack_smp_pairing_confirm(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret;

    if (bt_cur_conn_handle == -1) {
        printf("Not connected\n");
        return;
    }

    ret = ble_stack_smp_pairing_confirm(bt_cur_conn_handle);
    if (ret) {
        printf("pairing confirm err %d\n", ret);
        return;
    }
    printf("pairing confirm success\n");
}

static void cmd_ble_stack_dev_unpair(char *wbuf, int wbuf_len, int argc, char **argv)
{
    bt_dev_addr_t addr;
    int           err;

    if (argc < 2) {
        printf("Specify remote address or `all`\n");
        return;
    }

    if (strcmp(argv[1], "all") == 0) {
        err = ble_stack_dev_unpair(NULL);

        if (err) {
            printf("Failed to clear pairings err %d\n", err);
        } else {
            printf("Pairings successfully cleared\n");
        }

        return;
    }

    if (argc < 3) {
        printf("Both address and address type needed\n");
        return;
    }

    err = cli_bt_bt_addr_str_to_struct(argv[1], argv[2], &addr);
    if (err) {
        printf("Invalid address\n");
        return;
    }

    err = ble_stack_dev_unpair(&addr);

    if (err) {
        printf("Failed to clear pairing err %d\n", err);
    } else {
        printf("Pairing successfully cleared\n");
    }

    return;
}

static void cmd_ble_stack_enc_key_size_get(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int     ret;
    int16_t conn_handle;

    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    ret = ble_stack_enc_key_size_get(conn_handle);
    if (ret < 0) {
        printf("enc key size get err %d\n", ret);
        return;
    }
    printf("enc key size %d\n", ret);
}

static inline void paried_devcie(bt_dev_addr_t *addr, void *data)
{
    printf("%s\n", cli_bt_bt_dev_addr_to_string(addr));
}

static void cmd_ble_stack_paired_dev_foreach(char *wbuf, int wbuf_len, int argc, char **argv)
{
    printf("Bonded Device list:\n");
    ble_stack_paired_dev_foreach(paried_devcie, NULL);
}
#endif /* CONFIG_BT_SMP */

#define TEST_IDX_CHAR1_VAL_LEN (100) /* the Test Characteristic 1 value length */
#define TEST_IDX_CHAR3_VAL_LEN (8)   /* the Test Characteristic 5 value length */

static uint16_t    test_service_handle = 0;
static uint8_t     test_service_data[256] = "test";
static char        test_service_cud[30] = "this is test service cud";
static aos_timer_t test_service_report_timer;

#define TEST_SERVICE_UUID                                                                                              \
    UUID128_DECLARE(0xF0, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define TEST_CHAR1_UUID                                                                                                \
    UUID128_DECLARE(0xF1, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define TEST_CHAR2_UUID                                                                                                \
    UUID128_DECLARE(0xF2, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define TEST_CHAR3_UUID                                                                                                \
    UUID128_DECLARE(0xF3, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define TEST_CHAR4_UUID                                                                                                \
    UUID128_DECLARE(0xF4, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define TEST_CHAR5_UUID                                                                                                \
    UUID128_DECLARE(0xF5, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define TEST_CHAR6_UUID                                                                                                \
    UUID128_DECLARE(0xF6, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define TEST_CHAR7_UUID                                                                                                \
    UUID128_DECLARE(0xF7, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)

enum {
    TEST_IDX_SVC,
    TEST_IDX_CHAR1,
    TEST_IDX_CHAR1_VAL,
    TEST_IDX_CHAR2,
    TEST_IDX_CHAR2_VAL,
    TEST_IDX_CHAR3,
    TEST_IDX_CHAR3_VAL,
    TEST_IDX_CHAR4,
    TEST_IDX_CHAR4_VAL,
    TEST_IDX_CHAR4_CUD,
    TEST_IDX_CHAR4_CCC,
    TEST_IDX_CHAR5,
    TEST_IDX_CHAR5_VAL,
    TEST_IDX_CHAR6,
    TEST_IDX_CHAR6_VAL,
    TEST_IDX_CHAR7,
    TEST_IDX_CHAR7_VAL,

    TEST_IDX_MAX,
};

#if defined(CONFIG_BT_HOST_OPTIMIZE) && CONFIG_BT_HOST_OPTIMIZE
GATT_SERVICE_STATIC_DEFINE(
    test_service, [TEST_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(TEST_SERVICE_UUID),

    [TEST_IDX_CHAR1]     = GATT_CHAR_DEFINE(TEST_CHAR1_UUID, GATT_CHRC_PROP_READ),
    [TEST_IDX_CHAR1_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR1_UUID, GATT_PERM_READ),

    [TEST_IDX_CHAR2]     = GATT_CHAR_DEFINE(TEST_CHAR2_UUID, GATT_CHRC_PROP_READ),
    [TEST_IDX_CHAR2_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR2_UUID, GATT_PERM_READ | GATT_PERM_READ_AUTHEN),

    [TEST_IDX_CHAR3]
    = GATT_CHAR_DEFINE(TEST_CHAR5_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                            | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR3_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR5_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [TEST_IDX_CHAR4]     = GATT_CHAR_DEFINE(TEST_CHAR3_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                                             | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_NOTIFY
                                                             | GATT_CHRC_PROP_INDICATE | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR4_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR3_UUID, GATT_PERM_READ | GATT_PERM_WRITE),
    [TEST_IDX_CHAR4_CUD] = GATT_CHAR_CUD_DEFINE(NULL, GATT_PERM_READ), [TEST_IDX_CHAR4_CCC] = GATT_CHAR_CCC_DEFINE(),

    [TEST_IDX_CHAR5] = GATT_CHAR_DEFINE(TEST_CHAR4_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                                             | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR5_VAL]
    = GATT_CHAR_VAL_DEFINE(TEST_CHAR4_UUID, GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN),

    [TEST_IDX_CHAR6]
    = GATT_CHAR_DEFINE(TEST_CHAR6_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                            | GATT_CHRC_PROP_EXT_PROP),
    [TEST_IDX_CHAR6_VAL]
    = GATT_CHAR_VAL_DEFINE(TEST_CHAR6_UUID, GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE),

    [TEST_IDX_CHAR7]
    = GATT_CHAR_DEFINE(TEST_CHAR7_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                            | GATT_CHRC_PROP_EXT_PROP),
    [TEST_IDX_CHAR7_VAL]
    = GATT_CHAR_VAL_DEFINE(TEST_CHAR7_UUID,
                           GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN), );
#else
static gatt_service test_service;
static gatt_attr_t  test_attrs[] = {
    [TEST_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(TEST_SERVICE_UUID),

    [TEST_IDX_CHAR1]     = GATT_CHAR_DEFINE(TEST_CHAR1_UUID, GATT_CHRC_PROP_READ),
    [TEST_IDX_CHAR1_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR1_UUID, GATT_PERM_READ),

    [TEST_IDX_CHAR2]     = GATT_CHAR_DEFINE(TEST_CHAR2_UUID, GATT_CHRC_PROP_READ),
    [TEST_IDX_CHAR2_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR2_UUID, GATT_PERM_READ | GATT_PERM_READ_AUTHEN),

    [TEST_IDX_CHAR3]
    = GATT_CHAR_DEFINE(TEST_CHAR5_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                            | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR3_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR5_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [TEST_IDX_CHAR4]     = GATT_CHAR_DEFINE(TEST_CHAR3_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                                             | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_NOTIFY
                                                             | GATT_CHRC_PROP_INDICATE | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR4_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR3_UUID, GATT_PERM_READ | GATT_PERM_WRITE),
    [TEST_IDX_CHAR4_CUD] = GATT_CHAR_CUD_DEFINE(NULL, GATT_PERM_READ),
    [TEST_IDX_CHAR4_CCC] = GATT_CHAR_CCC_DEFINE(),

    [TEST_IDX_CHAR5] = GATT_CHAR_DEFINE(TEST_CHAR4_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                                             | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR5_VAL]
    = GATT_CHAR_VAL_DEFINE(TEST_CHAR4_UUID, GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN),

    [TEST_IDX_CHAR6]
    = GATT_CHAR_DEFINE(TEST_CHAR6_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                            | GATT_CHRC_PROP_EXT_PROP),
    [TEST_IDX_CHAR6_VAL]
    = GATT_CHAR_VAL_DEFINE(TEST_CHAR6_UUID, GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE),

    [TEST_IDX_CHAR7]
    = GATT_CHAR_DEFINE(TEST_CHAR7_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP
                                            | GATT_CHRC_PROP_EXT_PROP),
    [TEST_IDX_CHAR7_VAL] = GATT_CHAR_VAL_DEFINE(
        TEST_CHAR7_UUID, GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN),
};
#endif /* CONFIG_BT_HOST_OPTIMIZE */

static int _test_servic_event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e             = (evt_data_gatt_char_read_t *)event_data;
    int16_t                    handle_offset = 0;

    test_service_t *test_service = _test_sevice_find(e->conn_handle, 0);

    if (!test_service) {
        printf("can't find test service, conn_handle %d\n", e->conn_handle);
        return 0;
    }

    BLE_CHAR_RANGE_CHECK(test_service_handle, TEST_IDX_MAX, e->char_handle, handle_offset);

    printf("_test_servic_event_char_read conn_handle %d char_handle %d len %d offset %d\n", e->conn_handle,
           e->char_handle, e->len, e->offset);

    switch (handle_offset) {
        case TEST_IDX_CHAR1_VAL:
            if (e->offset > TEST_IDX_CHAR1_VAL_LEN) {
                printf("gatt read offset %d out of Characteristic 1 value", e->offset);
                e->data = NULL;
                e->len  = 0;
                return 0;
            }
            e->data = test_service_data;
            e->len  = TEST_IDX_CHAR1_VAL_LEN;
            break;
        case TEST_IDX_CHAR3_VAL:
            if (e->offset > TEST_IDX_CHAR3_VAL_LEN) {
                printf("gatt read offset %d out of Characteristic 5 value", e->offset);
                e->data = NULL;
                e->len  = 0;
                return 0;
            }
            e->data = test_service_data;
            e->len  = TEST_IDX_CHAR3_VAL_LEN;
            break;
        case TEST_IDX_CHAR2_VAL:
        case TEST_IDX_CHAR4_VAL:
        case TEST_IDX_CHAR6_VAL:
        case TEST_IDX_CHAR7_VAL:
            if (e->offset > sizeof(test_service_data)) {
                printf("gatt read offset %d out of test_service_data", e->offset);
                e->data = NULL;
                e->len  = 0;
                return 0;
            }
            e->data = test_service_data;
            e->len  = sizeof(test_service_data);
            break;
        case TEST_IDX_CHAR4_CUD:
            if (e->offset > sizeof(test_service_cud)) {
                printf("gatt read offset %d out of test_service_cud", e->offset);
                e->data = NULL;
                e->len  = 0;
                return 0;
            }
            e->data = (uint8_t *)test_service_cud;
            e->len  = sizeof(test_service_cud);
            break;

        default:
            e->data = NULL;
            e->len  = 0;
            break;
    }

    return 0;
}

static int _test_servic_event_char_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_write_t *e             = (evt_data_gatt_char_write_t *)event_data;
    int16_t                     handle_offset = 0;

    test_service_t *test_service = _test_sevice_find(e->conn_handle, 0);

    if (!test_service) {
        printf("can't find test service, conn_handle %d\n", e->conn_handle);
        return 0;
    }

    BLE_CHAR_RANGE_CHECK(test_service_handle, TEST_IDX_MAX, e->char_handle, handle_offset);

    printf("_test_servic_event_char_write %s conn_handle %d char_handle %d len %d offset %d\n",
           e->flag == 0 ? "write" : (e->flag == GATT_WRITE_FLAG_PREPARE ? "prepare write" : "write without response"),
           e->conn_handle, e->char_handle, e->len, e->offset);

    cli_bt_hexdump(e->data, e->len);

    switch (handle_offset) {
        case TEST_IDX_CHAR3_VAL:
            /* prepare write request, set e->len = 0 means device is authorized */
            // TODO: Check if correct
            if (e->flag == GATT_WRITE_FLAG_PREPARE) {
                e->len = 0;
                return 0;
            }

            if (e->offset + e->len > TEST_IDX_CHAR3_VAL_LEN) {
                printf("gatt write offest %d len %d out of test_service_data\n", e->offset, e->len);
                e->len = -ATT_ERR_INVALID_OFFSET;
                return 0;
            }

            memcpy(test_service_data + e->offset, e->data, e->len);
            e->len = e->len;
            break;
        case TEST_IDX_CHAR4_VAL:
        case TEST_IDX_CHAR5_VAL:
        case TEST_IDX_CHAR6_VAL:
        case TEST_IDX_CHAR7_VAL:
            /* prepare write request, set e->len = 0 means device is authorized */
            // TODO: Check if correct
            if (e->flag == GATT_WRITE_FLAG_PREPARE) {
                e->len = 0;
                return 0;
            }

            if (e->offset + e->len > sizeof(test_service_data)) {
                printf("gatt write offest %d len %d out of test_service_data\n", e->offset, e->len);
                e->len = -ATT_ERR_INVALID_OFFSET;
                return 0;
            }

            memcpy(test_service_data + e->offset, e->data, e->len);
            e->len = e->len;
            break;
        default:
            e->len = 0;
            break;
    }

    return 0;
}

static void _test_service_report_timer(void *arg1, void *arg2)
{
    int     i;
    int     ret     = 0;
    uint8_t data[2] = { 1, 2 };

    for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        ret = 0;
        if (test_service_pool[i].used) {
            if (test_service_pool[i].ccc == CCC_VALUE_NOTIFY) {
                ret = ble_stack_gatt_notificate(test_service_pool[i].conn_handle,
                                                test_service_handle + TEST_IDX_CHAR4_VAL, data, 2);
            } else if (test_service_pool[i].ccc == CCC_VALUE_INDICATE) {
                ret = ble_stack_gatt_indicate(test_service_pool[i].conn_handle,
                                              test_service_handle + TEST_IDX_CHAR4_VAL, data, 2);
            }
            data[0]++;
            data[1]++;
            if (ret) {
                printf("gatt %s err %d\n", test_service_pool[i].ccc == CCC_VALUE_NOTIFY ? "notify" : "indicate", ret);
            }
        }
    }
}

static void _test_service_report_check(void)
{
    int            i;
    int            ret;
    static uint8_t init = 0;
    if (!init) {
        ret = aos_timer_new_ext(&test_service_report_timer, _test_service_report_timer, NULL, 2000, 2000, 0);
        if (ret != 0) {
            printf("test service timer create failed\r\n");
            return;
        }

        init = 1;
    }

    for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        if (test_service_pool[i].used && test_service_pool[i].ccc) {
            break;
        }
    }

    /* no active connection need report, so stop the timer */
    if (i == CONFIG_BT_MAX_CONN) {
        aos_timer_stop(&test_service_report_timer);
    } else {
        aos_timer_start(&test_service_report_timer);
    }
}

static int _test_servic_event_char_ccc_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_write_t *e             = (evt_data_gatt_char_ccc_write_t *)event_data;
    uint16_t                        handle_offset = 0;

    test_service_t *test_service = _test_sevice_find(e->conn_handle, 0);

    if (!test_service) {
        printf("can't find test service, conn_handle %d\n", e->conn_handle);
        return 0;
    }

    BLE_CHAR_RANGE_CHECK(test_service_handle, TEST_IDX_MAX, e->char_handle, handle_offset);

    if (handle_offset == TEST_IDX_CHAR4_CCC) {
        test_service->ccc = e->ccc_value;

        printf("test service ccc handle %d change %d\n", e->char_handle, e->ccc_value);

        _test_service_report_check();
    }

    return 0;
}

static int _test_servic_event_gatt_indicate_confirm(ble_event_en event, void *event_data)
{
    evt_data_gatt_indicate_cb_t *e = event_data;

    if (e->err) {
        printf("gatt indicate fail, err %d\n", e->err);
    } else {
        printf("gatt indicate char %d success\n", e->char_handle);
    }

    return 0;
}

#if defined(CONFIG_BT_GATT_CLIENT) && CONFIG_BT_GATT_CLIENT

#define GATT_WRITE_BUFF_SIZE 256
static uint8_t gatt_write_buf[GATT_WRITE_BUFF_SIZE];
static uint8_t gatt_read_show_mode = 0;

static void print_chrc_props(u8_t properties)
{
    printf("Properties: ");

    if (properties & GATT_CHRC_PROP_BROADCAST) {
        printf("[bcast]");
    }

    if (properties & GATT_CHRC_PROP_READ) {
        printf("[read]");
    }

    if (properties & GATT_CHRC_PROP_WRITE) {
        printf("[write]");
    }

    if (properties & GATT_CHRC_PROP_WRITE_WITHOUT_RESP) {
        printf("[write w/w rsp]");
    }

    if (properties & GATT_CHRC_PROP_NOTIFY) {
        printf("[notify]");
    }

    if (properties & GATT_CHRC_PROP_INDICATE) {
        printf("[indicate]");
    }

    if (properties & GATT_CHRC_PROP_AUTH) {
        printf("[auth]");
    }

    if (properties & GATT_CHRC_PROP_EXT_PROP) {
        printf("[ext prop]");
    }

    printf("\n");
}

static int _test_servic_event_gatt_discovery(ble_event_en event, void *event_data)
{
    union
    {
        evt_data_gatt_discovery_svc_t      svc;
        evt_data_gatt_discovery_inc_svc_t  svc_inc;
        evt_data_gatt_discovery_char_t     char_c;
        evt_data_gatt_discovery_char_des_t char_des;
    } * e;

    e = event_data;

    switch (event) {
        case EVENT_GATT_DISCOVERY_SVC:
            printf("Service %s found: start handle %x, end_handle %x\n", cli_bt_uuid_str(&e->svc.uuid),
                   e->svc.start_handle, e->svc.end_handle);
            break;

        case EVENT_GATT_DISCOVERY_CHAR:
            printf("Characteristic %s found: handle %x\n", cli_bt_uuid_str(&e->char_c.uuid), e->char_c.attr_handle);
            print_chrc_props(e->char_c.props);
            break;

        case EVENT_GATT_DISCOVERY_INC_SVC:
            printf("Include %s found: handle %x, start %x, end %x\n", cli_bt_uuid_str(&e->svc_inc.uuid),
                   e->svc_inc.attr_handle, e->svc_inc.start_handle, e->svc_inc.end_handle);
            break;

        default:
            printf("Descriptor %s found: handle %x\n", cli_bt_uuid_str(&e->char_des.uuid), e->char_des.attr_handle);
            break;
    }
    return 0;
}

static int _test_servic_event_gatt_read_cb(ble_event_en event, void *event_data)
{
    evt_data_gatt_read_cb_t *e = event_data;

    if (!e->len || !e->data) {
        return 0;
    }

    printf("Read complete: err %u length %u\n", e->err, e->len);

    if (!gatt_read_show_mode) {
        cli_bt_hexdump(e->data, e->len);
    } else {
        for (int i = 0; i < e->len; i++) {
            printf("%02x", *e->data++);
        }
        printf("\r\n");
    }
    return 0;
}

static int _test_servic_event_gatt_write_cb(ble_event_en event, void *event_data)
{
    evt_data_gatt_write_cb_t *e = event_data;
    printf("Write complete: err %u\n", e->err);
    return 0;
}

static int _test_servic_event_gatt_notify(ble_event_en event, void *event_data)
{
    evt_data_gatt_notify_t *e = event_data;

    printf("Notification: char handle %d length %u\n", e->char_handle, e->len);
    cli_bt_hexdump(e->data, e->len);

    return 0;
}

static int _test_servic_event_gatt_mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = event_data;

    test_service_t *test_service = _test_sevice_find(e->conn_handle, 0);

    if (NULL == test_service) {
        printf("conn_handle %d Not Connected\n", e->conn_handle);
    }

    test_service->mtu = ble_stack_gatt_mtu_get(e->conn_handle);
    printf("Exchange %s, MTU %d\n", e->err == 0 ? "successful" : "failed", test_service->mtu);

    return 0;
}

#endif /* CONFIG_BT_GATT_CLIENT */
static int test_service_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GATT_CHAR_READ:
            _test_servic_event_char_read(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE:
            _test_servic_event_char_write(event, event_data);
            break;

        case EVENT_GATT_CHAR_CCC_WRITE:
            _test_servic_event_char_ccc_write(event, event_data);
            break;

        case EVENT_GATT_INDICATE_CB:
            _test_servic_event_gatt_indicate_confirm(event, event_data);
            break;

#if defined(CONFIG_BT_GATT_CLIENT) && CONFIG_BT_GATT_CLIENT
        case EVENT_GATT_DISCOVERY_SVC:
        case EVENT_GATT_DISCOVERY_INC_SVC:
        case EVENT_GATT_DISCOVERY_CHAR:
        case EVENT_GATT_DISCOVERY_CHAR_DES:
            _test_servic_event_gatt_discovery(event, event_data);
            break;

        case EVENT_GATT_CHAR_READ_CB:
            _test_servic_event_gatt_read_cb(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE_CB:
            _test_servic_event_gatt_write_cb(event, event_data);
            break;

        case EVENT_GATT_NOTIFY:
            _test_servic_event_gatt_notify(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            _test_servic_event_gatt_mtu_exchange(event, event_data);
            break;
#endif /* CONFIG_BT_GATT_CLIENT */
        default:
            break;
    }

    return 0;
}

static ble_event_cb_t test_sevice_event = {
    .callback = test_service_callback,
};

static void cmd_ble_stack_gatt_registe_service(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret = 0;

#if defined(CONFIG_BT_HOST_OPTIMIZE) && CONFIG_BT_HOST_OPTIMIZE
    ret = ble_stack_gatt_service_handle(&test_service);
#else
    ret = ble_stack_gatt_registe_service(&test_service, test_attrs, BLE_ARRAY_NUM(test_attrs));
#endif
    if (ret < 0) {
        printf("Registering test services faild (%d)\n", ret);
        return;
    }

    test_service_handle = ret;

    ble_stack_event_register(&test_sevice_event);

    printf("Registe test services success\n");
}

static void cmd_ble_stack_gatt_notificate(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int             ret;
    int16_t         conn_handle;
    test_service_t *service = NULL;
    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    ret = ble_stack_gatt_notificate(conn_handle, test_service_handle + TEST_IDX_CHAR4_VAL, (uint8_t *)argv[2],
                                    strlen(argv[2]));
    if (ret) {
        printf("gatt notify err %d\n", ret);
        return;
    }

    printf("gatt notify success\n");
}

static void cmd_ble_stack_gatt_indicate(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int             ret;
    int16_t         conn_handle;
    test_service_t *service = NULL;
    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);

    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    ret = ble_stack_gatt_indicate(conn_handle, test_service_handle + TEST_IDX_CHAR4_VAL, (uint8_t *)argv[2],
                                  strlen(argv[2]));
    if (ret) {
        printf("gatt indicate err %d\n", ret);
        return;
    }

    printf("gatt indicate success\n");
}

static void cmd_ble_stack_gatt_mtu_get(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int             mtu;
    int16_t         conn_handle;
    test_service_t *service = NULL;
    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    mtu = ble_stack_gatt_mtu_get(conn_handle);
    if (mtu > 0) {
        printf("gatt mtu %d\n", mtu);
    } else {
        printf("gatt mtu get err %d\n", mtu);
    }
}

#if defined(CONFIG_BT_GATT_CLIENT) && CONFIG_BT_GATT_CLIENT

static void cmd_ble_stack_gatt_mtu_exchange(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int             err;
    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    err = ble_stack_gatt_mtu_exchange(bt_cur_conn_handle);

    if (err) {
        printf("gatt mtu exchange failed err %d\n", err);
    } else {
        printf("gatt mtu exchange pending\n");
    }

    return;
}

static void cmd_ble_stack_gatt_discovery(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int                    err;
    gatt_discovery_type_en type;
    union
    {
        struct ut_uuid_16  uuid16;
        struct ut_uuid_32  uuid32;
        struct ut_uuid_128 uuid128;
    } uuid = { 0 };

    uint16_t start_handle = 0x0001;
    uint16_t end_handle   = 0xffff;

    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    if (argc < 2) {
        if (!strcmp(argv[0], "gatt-discover-primary") || !strcmp(argv[0], "gatt-discover-secondary")) {
            printf("invaild param\n");
            return;
        }

        goto done;
    }

    /* Only set the UUID if the value is valid (non zero) */
    if (strlen(argv[1]) <= 4) {
        uuid.uuid16.uuid.type = UUID_TYPE_16;
        uuid.uuid16.val       = strtoul(argv[1], NULL, 16);
    } else if (strlen(argv[1]) == 32) {
        uuid.uuid128.uuid.type = UUID_TYPE_128;
        cli_bt_str_to_hex(uuid.uuid128.val, 16, argv[1]);
    } else {
        printf("invaild uuid\n");
        return;
    }

    if (argc > 2) {
        start_handle = strtoul(argv[2], NULL, 16);

        if (argc > 3) {
            end_handle = strtoul(argv[3], NULL, 16);
        }
    }

done:

    if (!strcmp(argv[0], "gatt-discover-secondary")) {
        type = GATT_FIND_PRIMARY_SERVICE;
    } else if (!strcmp(argv[0], "gatt-discover-include")) {
        type = GATT_FIND_INC_SERVICE;
    } else if (!strcmp(argv[0], "gatt-discover-characteristic")) {
        type = GATT_FIND_CHAR;
    } else if (!strcmp(argv[0], "gatt-discover-descriptor")) {
        type = GATT_FIND_CHAR_DESCRIPTOR;
    } else {
        type = GATT_FIND_PRIMARY_SERVICE;
    }

    err = ble_stack_gatt_discovery(conn_handle, type,
                                   (uuid.uuid16.uuid.type == 0 && uuid.uuid16.val == 0) ? NULL : (uuid_t *)&uuid,
                                   start_handle, end_handle);

    if (err) {
        printf("gatt discovery failed err %d\n", err);
    } else {
        printf("gatt discovery pending\n");
    }

    return;
}

static void cmd_ble_stack_gatt_write(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int      err;
    uint16_t handle, offset, len = 1;

    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    if (argc < 4) {
        printf("invaild params\n");
        return;
    }

    handle = strtoul(argv[1], NULL, 16);
    offset = strtoul(argv[2], NULL, 16);

    gatt_write_buf[0] = strtoul(argv[3], NULL, 16);

    if (argc == 5) {
        int i;

        len = BLE_MIN(strtoul(argv[4], NULL, 16), sizeof(gatt_write_buf));

        for (i = 1; i < len; i++) {
            gatt_write_buf[i] = gatt_write_buf[0];
        }
    }

    err = ble_stack_gatt_write_response(conn_handle, handle, gatt_write_buf, len, offset);

    if (err) {
        printf("Write failed (err %d)\n", err);
    } else {
        printf("Write pending\n");
    }

    return;
}

static void cmd_ble_stack_gatt_write_no_response(char *wbuf, int wbuf_len, int argc, char **argv)
{
    uint16_t        handle;
    uint16_t        repeat;
    int             err = 0;
    uint16_t        len;
    bool            sign;
    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    if (argc < 3) {
        printf("invaild params\n");
        return;
    }

    sign              = !strcmp(argv[0], "gatt-write-signed");
    handle            = strtoul(argv[1], NULL, 16);
    gatt_write_buf[0] = strtoul(argv[2], NULL, 16);
    len               = 1;

    if (argc > 3) {
        int i;

        len = BLE_MIN(strtoul(argv[3], NULL, 16), sizeof(gatt_write_buf));

        for (i = 1; i < len; i++) {
            gatt_write_buf[i] = gatt_write_buf[0];
        }
    }

    repeat = 0;

    if (argc > 4) {
        repeat = strtoul(argv[4], NULL, 16);
    }

    if (!repeat) {
        repeat = 1;
    }

    while (repeat--) {
        if (sign) {
            err = ble_stack_gatt_write_signed(conn_handle, handle, gatt_write_buf, len, 0);
        } else {
            err = ble_stack_gatt_write_no_response(conn_handle, handle, gatt_write_buf, len, 0);
        }

        if (err) {
            break;
        }
    }

    printf("Write Complete (err %d)\n", err);

    return;
}

static void cmd_ble_stack_gatt_read_format(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        printf("invaild param\n");
        return;
    }

    gatt_read_show_mode = strtoul(argv[1], NULL, 16);
    return;
}

static void cmd_ble_stack_gatt_read(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int      err;
    uint16_t handle;
    uint16_t offset = 0;

    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    if (argc < 2) {
        printf("invaild params\n");
        return;
    }

    handle = strtoul(argv[1], NULL, 16);

    if (argc > 2) {
        offset = strtoul(argv[2], NULL, 16);
    }

    err = ble_stack_gatt_read(conn_handle, handle, offset);

    if (err) {
        printf("Read failed (err %d)\n", err);
    } else {
        printf("Read pending\n");
    }
}

static void cmd_ble_stack_gatt_read_multiple(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int             i, err;
    static uint16_t h[8];

    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    if (argc < 3) {
        printf("invaild params\n");
        return;
    }
    if (argc - 1 > BLE_ARRAY_NUM(h)) {
        printf("Enter max %lu handle items to read\n", ARRAY_SIZE(h));
        return;
    }

    for (i = 0; i < argc - 1; i++) {
        h[i] = strtoul(argv[i + 1], NULL, 16);
    }

    err = ble_stack_gatt_read_multiple(conn_handle, argc - 1, h);

    if (err) {
        printf("GATT multiple read request failed (err %d)\n", err);
    } else {
        printf("GATT multiple read request pending\n");
    }
}

static void cmd_gatt_subscribe(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int             err;
    uint16_t        ccc_handle;
    uint16_t        value = 0;
    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    if (argc < 3) {
        printf("invaild params\n");
        return;
    }

    ccc_handle = strtoul(argv[1], NULL, 16);
    value      = CCC_VALUE_NOTIFY;

    if (argc >= 3 && !strcmp(argv[2], "ind")) {
        value = CCC_VALUE_INDICATE;
    }

    err = ble_stack_gatt_write_response(conn_handle, ccc_handle, &value, sizeof(value), 0);

    if (err) {
        printf("Subscribe failed (err %d)\n", err);
    } else {
        printf("Subscribed\n");
    }

    return;
}

static void cmd_gatt_unsubscribe(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int             err;
    uint16_t        ccc_handle;
    uint16_t        value = CCC_VALUE_NONE;
    int16_t         conn_handle;
    test_service_t *service = NULL;

    conn_handle = bt_cur_conn_handle;

    service = _test_sevice_find(conn_handle, 0);
    if (NULL == service) {
        printf("conn handle %d not connected\n", conn_handle);
        return;
    }

    if (argc < 2) {
        printf("invaild params\n");
        return;
    }

    ccc_handle = strtoul(argv[1], NULL, 16);

    err = ble_stack_gatt_write_response(conn_handle, ccc_handle, &value, sizeof(value), 0);

    if (err) {
        printf("Unsubscribe failed (err %d)\n", err);
    } else {
        printf("Unsubscribe success\n");
    }

    return;
}

/// GATT Transport test

enum {
    GATT_TRANS_IDX_SVC,
    GATT_TRANS_IDX_RX,
    GATT_TRANS_IDX_RX_VAL,
    GATT_TRANS_IDX_TX,
    GATT_TRANS_IDX_TX_VAL,
    GATT_TRANS_IDX_TX_CCC,
    GATT_TRANS_IDX_MAX,
};

#define GATT_TRANS_SERVICE_UUID                                                                                        \
    UUID128_DECLARE(0xF0, 0x33, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define GATT_TRANS_RX_UUID                                                                                             \
    UUID128_DECLARE(0xF1, 0x33, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)
#define GATT_TRANS_TX_UUID                                                                                             \
    UUID128_DECLARE(0xF2, 0x33, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93)

#ifndef CONFIG_BT_CLI_BLE_GATT_TRANS_TASK_STACK_SIZE
#define CONFIG_BT_CLI_BLE_GATT_TRANS_TASK_STACK_SIZE (4096)
#endif

static gatt_service gatt_trans_service;

struct {
    aos_task_t task;
    uint8_t    registed;
    uint16_t   svc_handle;
    int16_t    conn_handle;
    uint8_t    rx_buf[247];
    uint16_t   ccc;
    uint8_t    type;        // 0:server 1:client
    uint8_t    mode;        // 0:loop  1:single
    uint8_t    svc_tx_mode; // 0:notify 1:indicate
    uint16_t   svc_rx_handle;
    uint8_t    cli_tx_mode; // 0:write 1:write without response
    uint32_t   data_len;    // 0:stream otherwise data_size
    uint32_t   fail_count;
    uint32_t   missmatch_count;
    uint32_t   trans_data;
    uint32_t   tx_count;
    uint32_t   tx_cb_count;
    uint32_t   tx_retry_count;
    uint32_t   rx_count;
    uint32_t   connect_count;
    uint32_t   disconn_count;
    aos_sem_t  op_sem;
    aos_sem_t  sync_sem;
    uint8_t    op;
} gatt_trans_svc = { 0 };

static gatt_attr_t gatt_trans_attrs0[] = {
    [GATT_TRANS_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(GATT_TRANS_SERVICE_UUID),

    [GATT_TRANS_IDX_RX]     = GATT_CHAR_DEFINE(GATT_TRANS_RX_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE
                                                                   | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [GATT_TRANS_IDX_RX_VAL] = GATT_CHAR_VAL_DEFINE(GATT_TRANS_RX_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [GATT_TRANS_IDX_TX]     = GATT_CHAR_DEFINE(GATT_TRANS_TX_UUID, GATT_CHRC_PROP_NOTIFY),
    [GATT_TRANS_IDX_TX_VAL] = GATT_CHAR_VAL_DEFINE(GATT_TRANS_TX_UUID, GATT_PERM_NONE),
    [GATT_TRANS_IDX_TX_CCC] = GATT_CHAR_CCC_DEFINE(),
};

static gatt_attr_t gatt_trans_attrs1[] = {
    [GATT_TRANS_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(GATT_TRANS_SERVICE_UUID),

    [GATT_TRANS_IDX_RX]     = GATT_CHAR_DEFINE(GATT_TRANS_RX_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE
                                                                   | GATT_CHRC_PROP_WRITE_WITHOUT_RESP),
    [GATT_TRANS_IDX_RX_VAL] = GATT_CHAR_VAL_DEFINE(GATT_TRANS_RX_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [GATT_TRANS_IDX_TX]     = GATT_CHAR_DEFINE(GATT_TRANS_TX_UUID, GATT_CHRC_PROP_INDICATE),
    [GATT_TRANS_IDX_TX_VAL] = GATT_CHAR_VAL_DEFINE(GATT_TRANS_TX_UUID, GATT_PERM_NONE),
    [GATT_TRANS_IDX_TX_CCC] = GATT_CHAR_CCC_DEFINE(),
};

uint8_t test_data[256]
    = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
        0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
        0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b,
        0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d,
        0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1,
        0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
        0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5,
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
        0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
        0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
        0xfc, 0xfd, 0xfe, 0xff };

static int gatt_indicate(const uint8_t *data, uint16_t len)
{
    int ret = 0;

    if (gatt_trans_svc.ccc == CCC_VALUE_INDICATE) {
        ret = aos_sem_wait(&gatt_trans_svc.sync_sem, 10000);

        if (ret) {
            printf("indicate sem wait fail %d\n", ret);
            return ret;
        }

        ret = ble_stack_gatt_indicate(gatt_trans_svc.conn_handle, gatt_trans_svc.svc_handle + GATT_TRANS_IDX_TX_VAL,
                                      data, len);

        if (ret) {
            gatt_trans_svc.fail_count++;
            return ret;
        }

        gatt_trans_svc.trans_data += len;
    } else {
        ret = -1;
        gatt_trans_svc.fail_count++;
        printf("indicate is disabled\n");
    }

    return ret;
}

static int gatt_notification(const uint8_t *data, uint16_t len)
{
    int ret = 0;

    if (gatt_trans_svc.ccc == CCC_VALUE_NOTIFY) {
        ret = ble_stack_gatt_notificate(gatt_trans_svc.conn_handle, gatt_trans_svc.svc_handle + GATT_TRANS_IDX_TX_VAL,
                                        data, len);

        if (ret) {
            return ret;
        }

        gatt_trans_svc.trans_data += len;
    } else {
        ret = -1;
        printf("nofify is disabled\n");
    }

    return ret;
}

static int gatt_write(const uint8_t *data, uint16_t len)
{
    int ret = 0;
    ret     = aos_sem_wait(&gatt_trans_svc.sync_sem, 1000);

    if (ret) {
        printf("write sem wait fail %d\n", ret);
        return ret;
    }

    ret = ble_stack_gatt_write_response(gatt_trans_svc.conn_handle, gatt_trans_svc.svc_rx_handle, data, len, 0);

    if (ret) {
        return ret;
    }

    gatt_trans_svc.trans_data += len;

    return ret;
}

static int gatt_write_no_response(const uint8_t *data, uint16_t len)
{
    int ret = 0;

    ret = ble_stack_gatt_write_no_response(gatt_trans_svc.conn_handle, gatt_trans_svc.svc_rx_handle, data, len, 0);

    if (ret) {
        return ret;
    }

    gatt_trans_svc.trans_data += len;

    return ret;
}

typedef int (*gatt_send_func_t)(const uint8_t *data, uint16_t len);

gatt_send_func_t gatt_send_func_table[2][2]
    = { { gatt_notification, gatt_indicate }, { gatt_write, gatt_write_no_response } };

static void transport_task(void *arg)
{
    int      ret;
    uint32_t trans_start_time;
    uint32_t trans_end_time;
    printf("transport task start\n");

    while (1) {
        aos_sem_wait(&gatt_trans_svc.op_sem, AOS_WAIT_FOREVER);
        trans_start_time = aos_now_ms();
        if (gatt_trans_svc.op == 1) {
            gatt_send_func_t send_func;

            if (gatt_trans_svc.type == 0) {
                send_func = gatt_send_func_table[gatt_trans_svc.type][gatt_trans_svc.svc_tx_mode];
            } else {
                send_func = gatt_send_func_table[gatt_trans_svc.type][gatt_trans_svc.cli_tx_mode];
            }

            uint32_t count = gatt_trans_svc.data_len;
            int      mtu   = ble_stack_gatt_mtu_get(gatt_trans_svc.conn_handle) - 3;
            int      i     = 0;

            if (gatt_trans_svc.data_len == 0) {
                count = 0xFFFFFFFF;
            }

            while (count) {
                uint16_t send_count = mtu < count ? mtu : count;
                test_data[0]        = i;
                ret                 = send_func(test_data, send_count);

                if (ret == -ENOMEM || ret == -ENOBUFS || ret == -EAGAIN) {
                    gatt_trans_svc.tx_retry_count++;
                    aos_msleep(1);
                    continue;
                }

                i++;

                if (ret) {
                    gatt_trans_svc.fail_count++;
                    printf("send fail %d\n", ret);
                }

                count -= send_count;
                gatt_trans_svc.tx_count++;

                if (gatt_trans_svc.op == 0) {
                    printf("op stop\n");
                    break;
                }

                if (gatt_trans_svc.data_len == 0) {
                    count = 0xFFFFFFFF;
                }
            }
            trans_end_time = aos_now_ms();
            float speed    = gatt_trans_svc.data_len / (trans_end_time - trans_start_time) * 1000 / 1024;
            printf("send %d complete in %d ms ,speed %f kB/s\n", gatt_trans_svc.data_len,
                   trans_end_time - trans_start_time, speed);
        }
    }
}

void test3_start_adv(void)
{
    ad_data_t ad[2] = { 0 };
    uint8_t   flag  = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;
    ad[0].type      = AD_DATA_TYPE_FLAGS;
    ad[0].data      = (uint8_t *)&flag;
    ad[0].len       = 1;

    ad[1].type = AD_DATA_TYPE_NAME_COMPLETE;
    ad[1].data = (uint8_t *)"TEST";
    ad[1].len  = strlen("TEST");

    adv_param_t param = {
        ADV_IND, ad, NULL, BLE_ARRAY_NUM(ad), 0, ADV_FAST_INT_MIN_1, ADV_FAST_INT_MAX_1,
    };

    int ret = ble_stack_adv_start(&param);

    if (ret) {
        gatt_trans_svc.fail_count++;
        printf("adv start fail %d!\n", ret);
    } else {
        printf("adv start!\n");
    }
}

static int data_check(uint8_t *data, uint16_t len)
{
    int i = 1;

    while (i < len && data[i] == i) {
        i++;
    };

    return (i == len);
}

static int _gatt_trans_char_write(ble_event_en event, void *event_data)
{
    int                         ret;
    evt_data_gatt_char_write_t *e             = (evt_data_gatt_char_write_t *)event_data;
    int16_t                     handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(gatt_trans_svc.svc_handle, GATT_TRANS_IDX_MAX, e->char_handle, handle_offset);

    // printf("event_char_write conn_handle %d char_handle %d len %d offset %d\n",
    //       e->conn_handle, e->char_handle, e->len, e->offset);
    // hexdump(e->data, e->len);
    handle_offset = e->char_handle - gatt_trans_svc.svc_handle;

    switch (handle_offset) {
        case GATT_TRANS_IDX_RX_VAL:
            memcpy(gatt_trans_svc.rx_buf + e->offset, e->data, BLE_MIN(e->len, sizeof(gatt_trans_svc.rx_buf)));
            e->len = BLE_MIN(e->len, sizeof(gatt_trans_svc.rx_buf));

            gatt_trans_svc.rx_count++;

            if (data_check(gatt_trans_svc.rx_buf, BLE_MIN(e->len, sizeof(gatt_trans_svc.rx_buf)))) {
                gatt_trans_svc.trans_data += e->len;
            } else {
                gatt_trans_svc.missmatch_count++;
            }

            if (gatt_trans_svc.mode == 0) {
                if (gatt_trans_svc.svc_tx_mode) {
                    if (gatt_trans_svc.ccc == CCC_VALUE_INDICATE) {
                        ret = ble_stack_gatt_indicate(gatt_trans_svc.conn_handle,
                                                      gatt_trans_svc.svc_handle + GATT_TRANS_IDX_TX_VAL,
                                                      gatt_trans_svc.rx_buf, e->len);

                        if (ret) {
                            gatt_trans_svc.fail_count++;
                            printf("indicate fail %d\n", ret);
                            return ret;
                        }

                        gatt_trans_svc.trans_data += e->len;
                        gatt_trans_svc.tx_count++;
                    } else {
                        gatt_trans_svc.fail_count++;
                        printf("indicate is disabled\n");
                    }
                } else {
                    if (gatt_trans_svc.ccc == CCC_VALUE_NOTIFY) {
                        ret = ble_stack_gatt_notificate(gatt_trans_svc.conn_handle,
                                                        gatt_trans_svc.svc_handle + GATT_TRANS_IDX_TX_VAL,
                                                        gatt_trans_svc.rx_buf, e->len);

                        if (ret) {
                            gatt_trans_svc.fail_count++;
                            printf("nofify fail %d\n", ret);
                            return ret;
                        }

                        gatt_trans_svc.trans_data += e->len;
                        gatt_trans_svc.tx_count++;
                    } else {
                        gatt_trans_svc.fail_count++;
                        printf("nofify is disabled\n");
                    }
                }
            }

            break;

        default:
            e->len = 0;
            break;
    }

    return 0;
}

static int _gatt_trans_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e             = (evt_data_gatt_char_read_t *)event_data;
    int16_t                    handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(gatt_trans_svc.svc_handle, GATT_TRANS_IDX_MAX, e->char_handle, handle_offset);
    handle_offset = e->char_handle - gatt_trans_svc.svc_handle;
    switch (handle_offset) {
        case GATT_TRANS_IDX_RX_VAL:
            e->data = (uint8_t *)gatt_trans_svc.rx_buf;
            e->len  = sizeof(gatt_trans_svc.rx_buf);
            break;

        default:
            e->data = NULL;
            e->len  = 0;
            break;
    }

    return 0;
}

static int _gatt_trans_ccc_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_ccc_change_t *e = (evt_data_gatt_char_ccc_change_t *)event_data;
    gatt_trans_svc.ccc                 = e->ccc_value;
    printf("CCC change %d\n", e->ccc_value);
    return 0;
}

static void _gatt_trans_conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        gatt_trans_svc.connect_count++;
        gatt_trans_svc.conn_handle = e->conn_handle;
    } else {
        gatt_trans_svc.op          = 0;
        gatt_trans_svc.conn_handle = -1;
        gatt_trans_svc.disconn_count++;
    }
}

static void _gatt_trans_indicate_cb(ble_event_en event, void *event_data)
{
    evt_data_gatt_indicate_cb_t *e = event_data;

    if (e->char_handle == gatt_trans_svc.svc_handle + GATT_TRANS_IDX_TX_VAL) {
        gatt_trans_svc.tx_cb_count++;
        aos_sem_signal(&gatt_trans_svc.sync_sem);
    }
}

static void _gatt_trans_write_cb(ble_event_en event, void *event_data)
{
    evt_data_gatt_write_cb_t *e = event_data;

    if (e->char_handle == gatt_trans_svc.svc_rx_handle) {
        gatt_trans_svc.tx_cb_count++;
        aos_sem_signal(&gatt_trans_svc.sync_sem);
    }
}

static void _gatt_trans_notify(ble_event_en event, void *event_data)
{
    evt_data_gatt_notify_t *e = event_data;
    // printf("_gatt_trans_notify conn_handle %d char_handle %d len %d\n",
    //       e->conn_handle, e->char_handle, e->len);
    // hexdump(e->data, e->len);
    gatt_trans_svc.rx_count++;

    if (data_check((uint8_t *)e->data, e->len)) {
        gatt_trans_svc.trans_data += e->len;
    } else {
        gatt_trans_svc.missmatch_count++;
    }
}

static void _gatt_trans_mtu_change(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = event_data;
    printf("mtu exchange %s, mtu %d\n", e->err ? "fail" : "success", ble_stack_gatt_mtu_get(e->conn_handle));
}

static int gatt_trans_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            _gatt_trans_conn_change(event, event_data);
            break;

        case EVENT_GATT_CHAR_READ:
            _gatt_trans_char_read(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE:
            _gatt_trans_char_write(event, event_data);
            break;

        case EVENT_GATT_CHAR_CCC_CHANGE:
            _gatt_trans_ccc_change(event, event_data);
            break;

        case EVENT_GATT_INDICATE_CB:
            _gatt_trans_indicate_cb(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE_CB:
            _gatt_trans_write_cb(event, event_data);
            break;

        case EVENT_GATT_NOTIFY:
            _gatt_trans_notify(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            _gatt_trans_mtu_change(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}

static ble_event_cb_t gatt_trans_event_cb = {
    .callback = gatt_trans_event_callback,
};

static int gatt_transport_test_config(int argc, char *argv[])
{
    int ret;

    if (argc < 6) {
        printf("params num err");
        return -EINVAL;
    }

    if (!gatt_trans_svc.registed) {
        memset(&gatt_trans_svc, 0, sizeof(gatt_trans_svc));
        gatt_trans_svc.conn_handle = -1;
    }

    gatt_trans_svc.type          = atoi(argv[0]);
    gatt_trans_svc.mode          = atoi(argv[1]);
    gatt_trans_svc.svc_tx_mode   = atoi(argv[2]);
    gatt_trans_svc.svc_rx_handle = atoi(argv[3]);
    gatt_trans_svc.cli_tx_mode   = atoi(argv[4]);
    gatt_trans_svc.data_len      = atoi(argv[5]);

    if (!gatt_trans_svc.registed) {
        if (gatt_trans_svc.type == 0) {
            if (gatt_trans_svc.svc_tx_mode) {
                ret = ble_stack_gatt_registe_service(&gatt_trans_service, gatt_trans_attrs1,
                                                     BLE_ARRAY_NUM(gatt_trans_attrs1));
            } else {
                ret = ble_stack_gatt_registe_service(&gatt_trans_service, gatt_trans_attrs0,
                                                     BLE_ARRAY_NUM(gatt_trans_attrs0));
            }

            if (ret < 0) {
                printf("Registering test services faild (%d)\n", ret);
                return ret;
            }

            gatt_trans_svc.svc_handle = ret;
        }

        ret = ble_stack_event_register(&gatt_trans_event_cb);

        if (ret) {
            return ret;
        }

        ret = aos_sem_new(&gatt_trans_svc.sync_sem, 1);

        if (ret) {
            return ret;
        }

        ret = aos_sem_new(&gatt_trans_svc.op_sem, 0);

        if (ret) {
            return ret;
        }

        ret = aos_task_new_ext(&gatt_trans_svc.task, "gatt test", transport_task, NULL, CONFIG_BT_CLI_BLE_GATT_TRANS_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
        if (ret) {
            printf("gatt trans task create err %d\n", ret);
            return ret;
        }

        gatt_trans_svc.registed = 1;
    }

    printf("gatt-transport-test-config success!\n");

    if (gatt_trans_svc.type == 0) {
        printf("type:server mode:%s tx mode:%s data len:%d\n", gatt_trans_svc.mode ? "single" : "loop",
               gatt_trans_svc.svc_tx_mode ? "indicate" : "notify", gatt_trans_svc.data_len);
        printf("use <ble adv conn> command to start advertising!\n");
    } else {
        printf("type:client mode:%s svc rx handle:0x%x tx mode: %s data len:%d\n",
               gatt_trans_svc.mode ? "single" : "loop", gatt_trans_svc.svc_rx_handle,
               gatt_trans_svc.cli_tx_mode ? "write without response" : "write", gatt_trans_svc.data_len);
        printf("use <ble connect> command to connect test device which in server mode!\n");
    }

    return 0;
}

static int gatt_transport_test_start()
{
    if (gatt_trans_svc.conn_handle == -1) {
        printf("Not Connected\n");
        return -1;
    }

    gatt_trans_svc.op = 1;
    aos_sem_signal(&gatt_trans_svc.op_sem);
    return 0;
}

static int gatt_transport_test_show_result()
{
    printf("TRANSPORT TEST REPORT:\n");
    printf("Type                  : %s\n", gatt_trans_svc.type ? "Client" : "Server");
    printf("Mode                  : %s\n", gatt_trans_svc.mode ? "Single" : "Loop");

    if (gatt_trans_svc.type == 0) {
        printf("TX mode               : %s\n", gatt_trans_svc.svc_tx_mode ? "Indication" : "Notification");
    } else {
        printf("TX mode               : %s\n",
               gatt_trans_svc.cli_tx_mode ? "Write without response" : "Write with response");
    }

    if (gatt_trans_svc.data_len) {
        printf("Data Len              : %d\n", gatt_trans_svc.data_len);
    } else {
        printf("Data Len              : stream\n");
    }

    printf("Transport data amount : %d\n", gatt_trans_svc.trans_data);
    printf("Transport missmatch   : %d\n", gatt_trans_svc.missmatch_count);
    printf("Transport tx count    : %d\n", gatt_trans_svc.tx_count);
    printf("Transport tx cb count : %d\n", gatt_trans_svc.tx_cb_count);
    printf("Transport resend count: %d\n", gatt_trans_svc.tx_retry_count);
    printf("Transport rx count    : %d\n", gatt_trans_svc.rx_count);
    printf("Transport fail count  : %d\n", gatt_trans_svc.fail_count);
    printf("Transport conn count  : %d\n", gatt_trans_svc.connect_count);
    printf("Transport discon count: %d\n", gatt_trans_svc.disconn_count);
    return 0;
}

static int gatt_transport_test_op(int argc, char *argv[])
{
    uint8_t op = 0xFF;

    if (argc < 1) {
        printf("params num err");
        return -EINVAL;
    }

    op                = atoi(argv[0]);
    gatt_trans_svc.op = op;

    if (op == 1) {
        return gatt_transport_test_start();
    } else if (op == 2) {
        return gatt_transport_test_show_result();
    } else if (op == 3) {
        gatt_trans_svc.trans_data      = 0;
        gatt_trans_svc.fail_count      = 0;
        gatt_trans_svc.missmatch_count = 0;
        gatt_trans_svc.tx_count        = 0;
        gatt_trans_svc.tx_cb_count     = 0;
        gatt_trans_svc.rx_count        = 0;
        gatt_trans_svc.tx_retry_count  = 0;
    }

    return 0;
}

static void cmd_gatt_transport_test(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (!strcmp(argv[0], "gatt-transport-test-config")) {
        gatt_transport_test_config(argc - 1, &argv[1]);
    } else if (!strcmp(argv[0], "gatt-transport-test-op")) {
        gatt_transport_test_op(argc - 1, &argv[1]);
    }

    return;
}

#endif /* CONFIG_BT_GATT_CLIENT */

#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_WHITELIST) && CONFIG_BT_WHITELIST
typedef struct {
    bt_dev_addr_t addr;
    uint8_t       set_flag;
} wl_addr_t;

#define MAX_WL_SZIE 10
static wl_addr_t wl_list[MAX_WL_SZIE] = { 0 };

static void _clear_wl_list()
{
    memset(wl_list, 0, sizeof(wl_list));
}

static int _get_wl_size()
{
    int wl_actal_szie;
    wl_actal_szie = ble_stack_white_list_size();
    if (wl_actal_szie <= 0) {
        return -1;
    }
    if (wl_actal_szie > MAX_WL_SZIE) {
        printf("actual wl size is %d but upper wl list size is %d\n", wl_actal_szie, MAX_WL_SZIE);
    }
    return wl_actal_szie < MAX_WL_SZIE ? wl_actal_szie : MAX_WL_SZIE;
}

static int _is_wl_addr_t_exist(bt_dev_addr_t addr)
{
    uint8_t index = 0;
    int     size  = 0;
    size          = _get_wl_size();
    if (size < 0) {
        return -1;
    }

    for (index = 0; index < size; index++) {
        if (wl_list[index].set_flag) {
            if (!memcmp(&wl_list[index].addr, &addr, sizeof(wl_list[index].addr))) {
                return 1;
            }
        }
    }

    if (index >= size) {
        return 0;
    }
    return 0;
}

static int _add_addr_to_wl_list(bt_dev_addr_t addr)
{
    uint8_t index = 0;
    int     size  = 0;
    size          = _get_wl_size();
    if (size < 0) {
        return -1;
    }

    for (index = 0; index < size; index++) {
        if (!wl_list[index].set_flag) {
            memcpy(&wl_list[index].addr, &addr, sizeof(wl_list[index].addr));
            wl_list[index].set_flag = 1;
            break;
        }
    }

    if (index >= size) {
        printf("wl list is full\r\n");
        return -1;
    } else {
        return 0;
    }
}

static int _remove_addr_form_wl_list(bt_dev_addr_t addr)
{
    uint8_t index = 0;
    int     size  = 0;

    size = _get_wl_size();
    if (size < 0) {
        return -1;
    }

    for (index = 0; index < size; index++) {
        if (wl_list[index].set_flag) {
            if (!memcmp(&wl_list[index].addr, &addr, sizeof(addr))) {
                memset(&wl_list[index], 0, sizeof(wl_addr_t));
                break;
            }
        }
    }

    if (index >= size) {
        printf("wl addr not exist\r\n");
        return -1;
    } else {
        return 0;
    }
}

static int _show_wl_list()
{
    int     found_flag = 0;
    uint8_t index      = 0;
    int     size       = 0;

    size = _get_wl_size();
    if (size < 0) {
        return -1;
    }
    for (index = 0; index < size; index++) {
        if (wl_list[index].set_flag) {
            found_flag = 1;
            printf("wl %d: %s\r\n", index, cli_bt_bt_dev_addr_to_string(&wl_list[index].addr));
        }
    }

    if (!found_flag) {
        printf("wl addr not exit\r\n");
        return -1;
    } else {
        return 0;
    }
}
static void cmd_ble_stack_white_list_clear(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int err = 0;

    if (!ble_init_flag) {
        printf("bt stack not init\n");
        return;
    }

    err = ble_stack_white_list_clear();
    if (err) {
        printf("Clear white list err %d\n", err);
    }

    _clear_wl_list();

    printf("Clear white list\n");
    return;
}

static void cmd_ble_stack_white_list_add(char *wbuf, int wbuf_len, int argc, char **argv)
{
    bt_dev_addr_t addr;
    int           err;

    if (!ble_init_flag) {
        printf("bt stack not init\n");
        return;
    }

    if (argc == 3) {
        err = cli_bt_bt_addr_str_to_struct(argv[1], argv[2], &addr);

        if (err) {
            printf("Invalid address\n");
            return;
        }
    } else {
        printf("Invalid param\n");
        return;
    }

    if (_is_wl_addr_t_exist(addr)) {
        printf("wl addr already exist\r\n");
        return;
    }

    err = ble_stack_white_list_add(&addr);
    if (!err) {
        err = _add_addr_to_wl_list(addr);
        if (err) {
            printf("add to upper wl list faild\r\n");
        } else {
            printf("Add %s (%s) to white list\n", argv[1], argv[2]);
        }
    } else {
        printf("add wl addr faild err %d\r\n", err);
    }
    return;
}

static void cmd_ble_stack_white_list_remove(char *wbuf, int wbuf_len, int argc, char **argv)
{
    bt_dev_addr_t addr;
    int           err;

    if (!ble_init_flag) {
        printf("bt stack not init\n");
        return;
    }

    if (argc == 3) {
        err = cli_bt_bt_addr_str_to_struct(argv[1], argv[2], &addr);

        if (err) {
            printf("Invalid address\n");
            return;
        }
    } else {
        printf("Invalid param\n");
        return;
    }

    if (!_is_wl_addr_t_exist(addr)) {
        printf("wl addr not exist\r\n");
        return;
    }

    err = ble_stack_white_list_remove(&addr);
    if (!err) {
        err = _remove_addr_form_wl_list(addr);
        if (err) {
            printf("remove from upper wl list faild\r\n");
        } else {
            printf("Remove %s (%s) to white list\n", argv[1], argv[2]);
        }
    } else {
        printf("add wl addr faild err %d\r\n", err);
    }
    return;
}

static void cmd_ble_stack_white_list_size(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret;
    if (!ble_init_flag) {
        printf("bt stack not init\n");
        return;
    }

    ret = ble_stack_white_list_size();

    printf("white list size is %d\n", ret);
}

static void cmd_ble_white_list_show(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (!ble_init_flag) {
        printf("bt stack not init\n");
        return;
    }

    _show_wl_list();
}

#endif /* CONFIG_BT_WHITELIST */

#define HELP_ADDR_LE "<address: XX:XX:XX:XX:XX:XX> <type: (public|random)>"

static const struct cli_command ble_cmd_info[] = {
    { "init", "ble init " HELP_ADDR_LE, cmd_ble_stack_init },
    { "name", "ble name [name]", cmd_ble_stack_set_name },
    { "io-capability", "<io input: NONE,YESNO, KEYBOARD> <io output: NONE,DISPLAY>", cmd_ble_stack_iocapability_set },
    { "adv", "<type: stop, conn, nconn> <ad(len|adtype|addata ...): 0xxxxxxxx> <sd(len|adtype|addata ...): 0xxxxxxxx>",
      cmd_ble_stack_adv },
    { "scan",
      "<value: active, passive, off> <dup filter: dups, nodups> "
      "<scan interval> <scan window> "
      "<ad(len|adtype|addata ...): 0xxxxxxxx> <sd(len|adtype|addata ...): 0xxxxxxxx>",
      cmd_ble_stack_scan },
    { "scan_filter", "ble scan_filter <filter_policy: (0:any adv, 1:white list)>", cmd_ble_scan_filter },
    { "local_addr", "ble local_addr", cmd_ble_stack_get_local_addr },

#if defined(CONFIG_BT_CONN) && CONFIG_BT_CONN
    { "connect", "ble connect " HELP_ADDR_LE " <interval_min> <interval_max> <latency> <timeout>",
      cmd_ble_stack_connect },
    { "conn-params-check", "ble conn-params-check <min interval> <max interval> <latency> <timeout>",
      cmd_ble_stack_check_conn_params },
    { "conn-select", "ble conn-select [conn_handle]", cmd_ble_conn_select },
    { "disconnect", "ble disconnect [conn_handle]", cmd_ble_stack_disconnect },
    { "conn_info", "ble conn_info [conn_handle]", cmd_ble_stack_connect_info_get },
    { "phy-rate-set",
      "ble phy-rate-set <tx rate(0x01:1M Phy,0x02:2M phy,0x04:Coded Phy)> <rx rate(0x01:1M Phy,0x02:2M phy,0x04:Coded "
      "Phy)>",
      cmd_ble_stack_pref_phy_set },
    { "conn-update", "ble conn-update <min interval> <max interval> <latency> <timeout>",
      cmd_ble_stack_connect_param_update },

#if defined(CONFIG_BT_SMP) && CONFIG_BT_SMP
    { "security", "ble security <security level(0: L0, 1:L1, 2:L2, 3:L3 4:L4 )>", cmd_ble_stack_security },
    { "auth-passkey", "ble auth-passkey <passkey>", cmd_ble_stack_smp_passkey_entry },
    { "auth-cancel", "ble auth-cancel", cmd_ble_stack_smp_cancel },
    { "auth-passkey-confirm", "ble auth-passkey-confirm", cmd_ble_stack_smp_passkey_confirm },
    { "auth-pairing-confirm", "ble auth-pairing-confirm", cmd_ble_stack_smp_pairing_confirm },
    { "clear", "ble clear <peer device:(all, [" HELP_ADDR_LE "])>", cmd_ble_stack_dev_unpair },
    { "enc-key-size", "ble enc-key-size", cmd_ble_stack_enc_key_size_get },
    { "bonded-dev", "ble bonded-dev", cmd_ble_stack_paired_dev_foreach },
#endif /* CONFIG_BT_SMP */

    { "gatt-register-service", "ble gatt-register-service", cmd_ble_stack_gatt_registe_service },
    { "gatt-notify", "ble gatt-notify <conn_handle> <data>", cmd_ble_stack_gatt_notificate },
    { "gatt-indicate", "ble gatt-indicate <conn_handle> <data>", cmd_ble_stack_gatt_indicate },
    { "gatt-get-mtu", "ble gatt-get-mtu <conn_handle>", cmd_ble_stack_gatt_mtu_get },

#if defined(CONFIG_BT_GATT_CLIENT) && CONFIG_BT_GATT_CLIENT
    { "gatt-exchange-mtu", "ble gatt-exchange-mtu", cmd_ble_stack_gatt_mtu_exchange },
    { "gatt-discover-primary", "ble gatt-discover-primary <UUID> [start handle] [end handle]",
      cmd_ble_stack_gatt_discovery },
    { "gatt-discover-secondary", "ble gatt-discover-secondary <UUID> [start handle] [end handle]",
      cmd_ble_stack_gatt_discovery },
    { "gatt-discover-include", "ble gatt-discover-include [UUID] [start handle] [end handle]",
      cmd_ble_stack_gatt_discovery },
    { "gatt-discover-characteristic", "ble gatt-discover-characteristic [UUID] [start handle] [end handle]",
      cmd_ble_stack_gatt_discovery },
    { "gatt-discover-descriptor", "ble gatt-discover-descriptor [UUID] [start handle] [end handle]",
      cmd_ble_stack_gatt_discovery },
    { "gatt-write", "ble gatt-write <handle> <offset> <data> [length]", cmd_ble_stack_gatt_write },
    { "gatt-write-without-response", "ble gatt-write-without-response <handle> <data> [length] [repeat]",
      cmd_ble_stack_gatt_write_no_response },
    { "gatt-write-signed", "ble gatt-write-signed <handle> <data> [length] [repeat]",
      cmd_ble_stack_gatt_write_no_response },
    { "gatt-read-format", "ble gatt-read-format", cmd_ble_stack_gatt_read_format },
    { "gatt-read", "ble gatt-read <handle> [offset]", cmd_ble_stack_gatt_read },
    { "gatt-read-multiple", "ble gatt-read-multiple <handle 1> <handle 2> ...", cmd_ble_stack_gatt_read_multiple },
    { "gatt-subscribe", "ble gatt-subscribe <CCC handle> [ind]", cmd_gatt_subscribe },
    { "gatt-unsubscribe", "ble gatt-unsubscribe <CCC handle>", cmd_gatt_unsubscribe },
    { "gatt-transport-test-config",
      "ble gatt-transport-test-config "
      "<type 0:server 1 client> <mode 0:loop 1:single>"
      "<server tx mode 0:notify 1:indicate> <server rx handle> <client tx mode 0:write 1:write_withoutresponse> "
      "<data_len 0:stream 1~0xFFFFFFFF data size>",
      cmd_gatt_transport_test },
    { "gatt-transport-test-op",
      "ble gatt-transport-test-op"
      "<op 0:stop 1:start 2:show result 3:reset>",
      cmd_gatt_transport_test },
#endif /* CONFIG_BT_GATT_CLIENT */

#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_WHITELIST) && CONFIG_BT_WHITELIST
    { "wl-clear", "ble init", cmd_ble_stack_white_list_clear },
    { "wl-add", "ble wl-add " HELP_ADDR_LE, cmd_ble_stack_white_list_add },
    { "wl-remove", "ble wl-remove " HELP_ADDR_LE, cmd_ble_stack_white_list_remove },
    { "wl-size", "ble wl-size", cmd_ble_stack_white_list_size },
    { "wl-show", "ble wl-show", cmd_ble_white_list_show },
#endif /* CONFIG_BT_WHITELIST */

    { NULL, NULL, NULL }
};

static void cmd_ble_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int i = 0;

    if (argc < 2) {
        printf("================BLE Command List================\n");

        for (i = 0; ble_cmd_info[i].name != NULL; i++) {
            printf("%-30s: %s\n", ble_cmd_info[i].name, ble_cmd_info[i].help);
        }

        return;
    }

    for (i = 0; ble_cmd_info[i].name != NULL; i++) {
        if (strlen(ble_cmd_info[i].name) == strlen(argv[1])
            && !strncmp(ble_cmd_info[i].name, argv[1], strlen(ble_cmd_info[i].name)))
        {
            if (ble_cmd_info[i].function) {
                ble_cmd_info[i].function(wbuf, wbuf_len, argc - 1, &argv[1]);
                return;
            }
        }
    }

    printf("Unsupport BLE Command, type `ble` for help\n");
}

void cli_reg_cmd_ble_stack(void)
{
    static const struct cli_command cmd_info = {
        "ble",
        "ble <commands> [args]",
        cmd_ble_func,
    };

    aos_cli_register_command(&cmd_info);
}
