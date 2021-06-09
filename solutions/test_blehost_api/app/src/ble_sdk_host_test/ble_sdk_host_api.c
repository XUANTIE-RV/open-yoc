/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/ble.h>
#include <aos/kernel.h>
#include <yoc/dis.h>
#include <yoc/bas.h>
#include <yoc/hrs.h>
#include <yoc/hids.h>
#include <yoc/ibeacons.h>
#include <yoc/ota_server.h>
#include <yoc/uart_client.h>
#include <yoc/uart_server.h>
#include <bluetooth/bluetooth.h>

#include "../yunit.h"
#include <errno.h>

#define DEV_ADDR {0xCC,0x3B,0xE3,0x82,0xBA,0xC0}
#define DEV_ADDR2 {0xCC,0x3B,0xE3,0x82,0xBA,0xC2}
#define DEV_NAME "BLE_INIT"
#define VALUE_LEN 100

static dev_addr_t g_addr = {DEV_ADDR_LE_RANDOM, DEV_ADDR2};
static conn_param_t g_param = {
    CONN_INT_MIN_INTERVAL,
    CONN_INT_MAX_INTERVAL,
    0,
    400,
};

static uint8_t adv_flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;
static uint8_t adv_uuid16_list[] = {0x0d, 0x18};
static uint8_t adv_manu_data[10] = {0x01, 0x02, 0x3, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10};
static ad_data_t adv_ad[3] = {
    {
        .type = AD_DATA_TYPE_FLAGS,
        .data = (uint8_t *)&adv_flag,
        .len = 1
    },
    {
        .type = AD_DATA_TYPE_UUID16_ALL,
        .data = (uint8_t *)adv_uuid16_list,
        .len = sizeof(adv_uuid16_list)
    },
    {
        .type = AD_DATA_TYPE_NAME_COMPLETE,
        .data = (uint8_t *)DEV_NAME,
        .len = 8
    }
};
static ad_data_t adv_sd[1] = {
    {
        .type = AD_DATA_TYPE_MANUFACTURER_DATA,
        .data = (uint8_t *)adv_manu_data,
        .len = 10
    }
};
static adv_param_t g_adv_param = {
        ADV_NONCONN_IND,
        adv_ad,
        adv_sd,
        BLE_ARRAY_NUM(adv_ad),
        BLE_ARRAY_NUM(adv_sd),
        ADV_FAST_INT_MIN_2,
        ADV_FAST_INT_MAX_2,
        ADV_FILTER_POLICY_ALL_REQ,
        ADV_DEFAULT_CHAN_MAP,
        {0}
};

static struct test_svc_t {
    int16_t conn_handle;
    uint16_t svc_handle;
    uint16_t svc2_handle;
    ccc_value_en ccc;
    uint16_t mtu;
    char value1[VALUE_LEN];
    char value2[VALUE_LEN];
    char value3[8];
    char value3_cud[30];
    char value4[VALUE_LEN];
} test_svc = {
    .conn_handle = -1,
    .svc_handle = 0,
    .svc2_handle = 0,
    .mtu = VALUE_LEN,
    .value1 = "test value 1",
    .value2 = "test value 2",
    .value3 = "test",
    .value3_cud = "this is value3 cud",
    .value4 = "test value 4",
};

static ble_uart_client_t g_uart_client;
static ble_uart_server_t g_uart_server;

#define TEST_SERVICE_UUID UUID128_DECLARE(0xF0,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR1_UUID UUID128_DECLARE(0xF1,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR2_UUID UUID128_DECLARE(0xF2,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR3_UUID UUID128_DECLARE(0xF3,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR4_UUID UUID128_DECLARE(0xF4,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR5_UUID UUID128_DECLARE(0xF5,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR6_UUID UUID128_DECLARE(0xF6,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR7_UUID UUID128_DECLARE(0xF7,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR8_UUID UUID128_DECLARE(0xF8,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)
#define TEST_CHAR9_UUID UUID128_DECLARE(0xF9,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)

#define TEST_SERVICE_UUID2 UUID128_DECLARE(0xFA,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)


enum {
    TEST_IDX_SVC,
    TEST_IDX_CHAR1,
    TEST_IDX_CHAR1_VAL,
    TEST_IDX_CHAR2,
    TEST_IDX_CHAR2_VAL,
    TEST_IDX_CHAR5,
    TEST_IDX_CHAR5_VAL,
    TEST_IDX_CHAR3,
    TEST_IDX_CHAR3_VAL,
    TEST_IDX_CHAR3_CUD,
    TEST_IDX_CHAR3_CCC,
    TEST_IDX_CHAR4,
    TEST_IDX_CHAR4_VAL,
    TEST_IDX_CHAR6,
    TEST_IDX_CHAR6_VAL,
    TEST_IDX_CHAR7,
    TEST_IDX_CHAR7_VAL,
    TEST_IDX_SVC2,
    TEST_IDX_CHAR8,
    TEST_IDX_CHAR8_CDD,
    TEST_IDX_CHAR8_CPF,


    TEST_IDX_MAX,
};

gatt_service test_service;
static gatt_attr_t test_attrs[] = {
    [TEST_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(TEST_SERVICE_UUID),

    [TEST_IDX_CHAR1] = GATT_CHAR_DEFINE(TEST_CHAR1_UUID, GATT_CHRC_PROP_READ),
    [TEST_IDX_CHAR1_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR1_UUID, GATT_PERM_READ),

    [TEST_IDX_CHAR2] = GATT_CHAR_DEFINE(TEST_CHAR2_UUID, GATT_CHRC_PROP_READ),
    [TEST_IDX_CHAR2_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR2_UUID, GATT_PERM_READ | GATT_PERM_READ_AUTHEN),

    [TEST_IDX_CHAR5] = GATT_CHAR_DEFINE(TEST_CHAR5_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR5_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR5_UUID, GATT_PERM_READ | GATT_PERM_WRITE),

    [TEST_IDX_CHAR3] = GATT_CHAR_DEFINE(TEST_CHAR3_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_NOTIFY | GATT_CHRC_PROP_INDICATE | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR3_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR3_UUID,  GATT_PERM_READ | GATT_PERM_WRITE),
    [TEST_IDX_CHAR3_CUD] = GATT_CHAR_CUD_DEFINE(test_svc.value3_cud,  GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE),
    [TEST_IDX_CHAR3_CCC] = GATT_CHAR_CCC_DEFINE(),

    [TEST_IDX_CHAR4] = GATT_CHAR_DEFINE(TEST_CHAR4_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR4_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR4_UUID,  GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN),

    [TEST_IDX_CHAR6] = GATT_CHAR_DEFINE(TEST_CHAR6_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP),
    [TEST_IDX_CHAR6_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR6_UUID,  GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE),

    [TEST_IDX_CHAR7] = GATT_CHAR_DEFINE(TEST_CHAR7_UUID, GATT_CHRC_PROP_READ | GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP),
    [TEST_IDX_CHAR7_VAL] = GATT_CHAR_VAL_DEFINE(TEST_CHAR7_UUID,  GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN),

    [TEST_IDX_SVC2] = GATT_SECONDARY_SERVICE_DEFINE(TEST_SERVICE_UUID2),
    [TEST_IDX_CHAR8] = GATT_CHAR_DEFINE(TEST_CHAR8_UUID, GATT_CHRC_PROP_WRITE | GATT_CHRC_PROP_WRITE_WITHOUT_RESP | GATT_CHRC_PROP_EXT_PROP | GATT_CHRC_PROP_AUTH),
    [TEST_IDX_CHAR8_CDD] = GATT_CHAR_DESCRIPTOR_DEFINE(TEST_CHAR8_UUID,  GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN),
    [TEST_IDX_CHAR8_CPF] = GATT_CHAR_CPF_DEFINE(0x03, GATT_PERM_READ | GATT_PERM_WRITE)

};


static void CASE_blesdk_gap_before_init() {
    int err;
    ble_event_cb_t callback;
    err = ble_stack_event_register(&callback);
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_INIT, "ret = %d", err, "YOC_GAP_REG_1");

    adv_param_t par;
    err = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_INIT, "ret = %d", err, "YOC_GAP_ADV_START_1");

    err = ble_stack_adv_stop();
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_INIT, "ret = %d", err, "YOC_GAP_ADV_STOP_1");

    scan_param_t param;
    err = ble_stack_scan_start(&param);
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_INIT, "ret = %d", err, "YOC_GAP_SCAN_START_1");

    err = ble_stack_scan_stop();
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_INIT, "ret = %d", err, "YOC_GAP_SCAN_STOP_1");

    gatt_service ser;
    gatt_attr_t attrs[1] = {0};
    err = ble_stack_gatt_registe_service(&ser, attrs, 1);
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_INIT, "ret = %d", err, "YOC_GATT_REG_1");

    // uint8_t _id[2] = {0x01, 0x02};
    // uint8_t _uuid[16] = {0}; 
    // uint8_t _major[2] = {0x03, 0x04};
    // uint8_t _minor[2] = {0x04, 0x05};
    // uint8_t _measure_power = 0x00;
    // char *_sd = "apple_dev";
    // err = ibeacon_start(_id, _uuid, _major, _minor, _measure_power, _sd);
    // YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_INIT, "ret = %d", err, "YOC_IBEACON_START_1");

    // err = ibeacon_stop();
    // YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_INIT, "ret = %d", err, "YOC_IBEACON_STOP_1");

}

static void CASE_blesdk_gap_init()
{
    int err;
    init_param_t init = {NULL, NULL, 1};
    dev_addr_t addr2 = {DEV_ADDR_LE_RANDOM, DEV_ADDR2};
    init_param_t demo = {
        .dev_name = DEV_NAME,
        .dev_addr = &addr2,
        .conn_num_max = 1,
    };

    err = ble_stack_init(NULL);
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_NULL, "ret = %d", err, "YOC_GAP_INIT_1");

    //测试设备的type
    addr2.type = 4;
    demo.dev_addr = &addr2;
    err = ble_stack_init(&demo);
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_PARAM, "ret = %d", err, "YOC_GAP_INIT_2");

    //dev_name = NULL
    addr2.type = DEV_ADDR_LE_RANDOM;
    init.dev_addr = &addr2;
    err = ble_stack_init(&init);
    YUNIT_ASSERT_MSG_QA(err == BLE_STACK_OK, "ret = %d", err, "YOC_GAP_INIT_3");

    //dev_addr = NULL
    init.dev_name = DEV_NAME;
    init.dev_addr = NULL;
    err = ble_stack_init(&init);
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_ALREADY, "ret = %d", err, "YOC_GAP_INIT_4");

}

static void CASE_blesdk_gap_reg()
{
    int err;
    err = ble_stack_event_register(NULL);
    YUNIT_ASSERT_MSG_QA(err == -BLE_STACK_ERR_NULL, "ret = %d", err, "YOC_GAP_REG_2");
    ble_event_cb_t callback;
    err = ble_stack_event_register(&callback);
    YUNIT_ASSERT_MSG_QA(err == 0, "ret = %d", err, "YOC_GAP_REG_3");

}

static void CASE_blesdk_gap_adv()
{
    int ret;
    ad_data_t ad[3] = {0};
    ad_data_t sd[1] = {0};

    uint8_t flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;

    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&flag;
    ad[0].len = 1;

    uint8_t uuid16_list[] = {0x0d, 0x18};
    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data = (uint8_t *)uuid16_list;
    ad[1].len = sizeof(uuid16_list);

    ad[2].type = AD_DATA_TYPE_NAME_COMPLETE;
    ad[2].data = (uint8_t *)DEV_NAME;
    ad[2].len = strlen(DEV_NAME);

    dev_addr_t peer_any_addr = {0};
    dev_addr_t peer_pub_addr = {DEV_ADDR_LE_PUBLIC, DEV_ADDR2};


    uint8_t manu_data[10] = {0x01, 0x02, 0x3, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10};
    sd[0].type = AD_DATA_TYPE_MANUFACTURER_DATA;
    sd[0].data = (uint8_t *)manu_data;
    sd[0].len = sizeof(manu_data);

    adv_param_t par = {
        ADV_NONCONN_IND,
        ad,
        sd,
        BLE_ARRAY_NUM(ad),
        BLE_ARRAY_NUM(sd),
        ADV_FAST_INT_MIN_2,
        ADV_FAST_INT_MAX_2,
        ADV_FILTER_POLICY_ALL_REQ,
        ADV_DEFAULT_CHAN_MAP,
        peer_any_addr
    };

    ret = ble_stack_adv_start(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GAP_ADV_START_2");

    //ad and ad_num mismatch 1
    par.ad = NULL;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_3");

    //ad and ad_num mismatch 2
    par.ad = ad;
    par.ad_num = 0;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_4");

    //sd and sd_num mismatch 1
    par.ad_num = BLE_ARRAY_NUM(ad);
    par.sd = NULL;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_5");

    //sd and sd_num mismatch 2
    par.sd = sd;
    par.sd_num = 0;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_6");

    //invalid channel_map
    par.sd_num = BLE_ARRAY_NUM(sd);
    par.channel_map = 8;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_7");

    //invalid filter_policy
    par.channel_map = 7;
    par.filter_policy = 4;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_8");

    //invalid type
    par.type = 5;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_9");

    //peer_addr == BT_ADDR_LE_ANY
    par.filter_policy = 3;
    par.type = ADV_DIRECT_IND_LOW_DUTY;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_10");

    par.type = ADV_DIRECT_IND;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_ADV_START_11");

    //min < 0x20, avoid setting ad to null
    par.type = ADV_IND;
    par.direct_peer_addr = peer_pub_addr;
    par.interval_min = 0x001f; 
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_ADV_START_12");

    //max > 0x4000
    par.interval_min = ADV_FAST_INT_MIN_1;
    par.interval_max = 0x4001;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_ADV_START_13");

    //min > max
    par.interval_min = 0x00f0;
    par.interval_max = 0x00a0;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_ADV_START_14");

    // data len > 27 + 1 + 4
    // set ad fail
    par.interval_min = ADV_FAST_INT_MIN_1;
    par.interval_max = ADV_FAST_INT_MAX_1;
    uint8_t ad_data[] = {0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x01, 0x02, 0x3, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x01, 0x02, 0x3, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12};
    ad[1].data = (uint8_t *)ad_data;
    ad[1].len = sizeof(ad_data);
    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    par.ad_num = BLE_ARRAY_NUM(ad);
    par.ad = ad;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_ADV_START_15");
    ret = ble_stack_adv_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_ADV_STOP_2");

    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data = (uint8_t *)uuid16_list;
    ad[1].len = sizeof(uuid16_list);
    par.ad = ad;
    ret = ble_stack_adv_start(&par);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_ADV_START_16");
    if (ret == 0) {
        ret = ble_stack_adv_stop();
        YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_ADV_STOP_3");
    }
}

static void CASE_blesdk_gap_scan()
{
    int ret;

    scan_param_t params = {
        SCAN_PASSIVE,
        SCAN_FILTER_DUP_DISABLE,
        SCAN_FAST_INTERVAL,
        SCAN_FAST_WINDOW,
    };

    ret = ble_stack_scan_start(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GAP_SCAN_START_2");

    params.scan_filter = SCAN_FILTER_POLICY_WHITE_LIST + 1;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_SCAN_START_3");
    // stop when not start
    ret = ble_stack_scan_stop();
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_GAP_SCAN_STOP_2");

    params.scan_filter = SCAN_FILTER_POLICY_WHITE_LIST;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_START_4");
    if (ret == 0) {
        ret = ble_stack_scan_stop();
        YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_STOP_3");
    }

    params.type = SCAN_ACTIVE;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_START_5");

    //start twice
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_GAP_SCAN_START_6");

    ret = ble_stack_scan_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_STOP_3");

    //invalid type
    params.type = 2;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_SCAN_START_7");

    params.type = SCAN_PASSIVE;
    params.filter_dup = SCAN_FILTER_DUP_ENABLE;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_START_8");
    ret = ble_stack_scan_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_STOP_4");

    params.filter_dup = 2;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_START_9");

    //interval too small
    params.filter_dup = SCAN_FILTER_DUP_DISABLE;
    params.interval = 3;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_SCAN_START_10");

    params.interval = 4;
    params.window = params.interval;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_GAP_SCAN_START_11");
    ret = ble_stack_scan_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_STOP_5");

    params.interval = 0x4000;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_START_12");
    ret = ble_stack_scan_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_STOP_6");

    //interval too big
    params.interval = 0x4001;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_SCAN_START_13");

    params.interval = SCAN_FAST_INTERVAL;
    //window too small
    params.window = 3;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_SCAN_START_14");

    params.window = 4;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_START_15");
    ret = ble_stack_scan_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_STOP_7");

    params.window = 0x4000;
    params.interval = params.window;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_START_16");
    ret = ble_stack_scan_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SCAN_STOP_8");

    //window too big
    params.window = 0x4001;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_SCAN_START_17");

    // window > interval
    params.interval = SCAN_FAST_INTERVAL;
    params.window = params.interval + 1;
    ret = ble_stack_scan_start(&params);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_GAP_SCAN_START_18");

}

static void CASE_blesdk_gap_get_addr()
{
    int ret;
    dev_addr_t addr;

    ret = ble_stack_get_local_addr(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GAP_GET_ADDR_1");

    ret = ble_stack_get_local_addr(&addr);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_GET_ADDR_2");
}


static void CASE_blesdk_gap_connect()
{
    int ret;
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEV_ADDR};
    conn_param_t param = {
        CONN_INT_MIN_INTERVAL,
        CONN_INT_MAX_INTERVAL,
        0,
        400,
    };

    //NULL peer_addr
    ret = ble_stack_connect(NULL, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GAP_CONN_1");

    //disconnect negative handle
    ret = ble_stack_disconnect(ret);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_DISCONN_1");

    //NULL param, auto_connect true
    ret = ble_stack_connect(&addr, NULL, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_CONN_2");

    //NULL param, auto_connect false
    ret = ble_stack_connect(&addr, NULL, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_3");

    //too big auto_connect
    ret = ble_stack_connect(&addr, &param, 2);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_CONN_4");


    // bt_le_conn_params_valid return false
    //too small param.interval_min
    param.interval_min = 0x0005;  
    param.interval_max = 0x0005;
    ret = ble_stack_connect(&addr, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_5");

    ret = ble_stack_check_conn_params(&param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_CHECK_CONN_1");


    //too big param.interval_min
    param.interval_min = 0x0C81;  
    param.interval_max = 0x0C81;
    ret = ble_stack_connect(&addr, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_6");

    ret = ble_stack_check_conn_params(&param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_CHECK_CONN_2");

    //interval_min > interval_max
    param.interval_min = 0x0C80;  
    param.interval_max = 0x0C00;
    ret = ble_stack_connect(&addr, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_7");

    ret = ble_stack_check_conn_params(&param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_CHECK_CONN_3");

    //valid interval
    param.interval_min = CONN_INT_MIN_INTERVAL;
    param.interval_max = CONN_INT_MAX_INTERVAL;

    //too big latency
    param.latency = 500;
    ret = ble_stack_connect(&addr, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_8");

    ret = ble_stack_check_conn_params(&param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_CHECK_CONN_4");

    param.latency = 0;
    param.interval_max = 35;
    //too small timeout
    param.timeout = 9;
    ret = ble_stack_connect(&addr, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_9");

    ret = ble_stack_check_conn_params(&param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_CHECK_CONN_5");

    //too big timeout
    param.timeout = 0x0C81;
    ret = ble_stack_connect(&addr, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_10");

    ret = ble_stack_check_conn_params(&param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_CHECK_CONN_6");

    param.timeout = 10;
    param.interval_max = CONN_INT_MAX_INTERVAL;
    //4*timeout <= (1+latency) * interval_max
    ret = ble_stack_connect(&addr, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_11");

    ret = ble_stack_check_conn_params(&param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_CHECK_CONN_7");


    //param.interval_min
    param.interval_min = 0x0006;  
    param.interval_max = 0x0006;
    ret = ble_stack_check_conn_params(&param);
    YUNIT_ASSERT_MSG_QA(ret == 1, "ret = %d", ret, "YOC_GAP_CHECK_CONN_8");

    ret = ble_stack_connect(&addr, &param, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GAP_CONN_12");

    ret = ble_stack_disconnect(0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_DISCONN_2");

    //comment out for JIRA: BLETC-218
    ret = ble_stack_connect(&addr, &param, 0);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_CONN_13");

    // ret = ble_stack_disconnect(ret);
    // YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GAP_DISCONN_3");
    // add stop scan to avoid case interference
    bt_le_scan_stop();

}

static void CASE_blesdk_gap_get_connect_info()
{
    int ret;
    connect_info_t info;

    ret = ble_stack_connect_info_get(-1, &info);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_GET_CONN_1");

    ret = ble_stack_connect_info_get(0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_GET_CONN_2");

    //conn_handle not exist
    ret = ble_stack_connect_info_get(50, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_GET_CONN_3");

    ret = ble_stack_connect_info_get(0, &info);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_GET_CONN_4");
}

static void CASE_blesdk_gap_update_param()
{
    int ret;
    conn_param_t param = {
        CONN_INT_MIN_INTERVAL,
        CONN_INT_MAX_INTERVAL,
        0,
        400,
    };

    ret = ble_stack_connect_param_update(-1, &param);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_GET_CONN_1");

    ret = ble_stack_connect_param_update(0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_GET_CONN_2");

    //conn_handle not exist
    ret = ble_stack_connect_param_update(50, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GAP_GET_CONN_3");

    ret = ble_stack_connect_param_update(0, &param);
    YUNIT_ASSERT_MSG_QA(ret != 0, "ret = %d", ret, "YOC_GAP_GET_CONN_4");

    //TODO, add case for bt_conn_le_param_update
}


static void CASE_blesdk_gap_white_list()
{
    int ret;
    int size = ble_stack_white_list_size();

    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEV_ADDR};
    YUNIT_ASSERT_MSG_QA( size > 0, "size = %d", size, "YOC_GAP_WHITE_SIZE_1");

    ret = ble_stack_white_list_add(&addr);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_ADD_WHITE_1");

    //add twice
    ret = ble_stack_white_list_add(&addr);
    YUNIT_ASSERT_MSG_QA(ret < 0, "ret = %d", ret, "YOC_GAP_ADD_WHITE_2");

    ret = ble_stack_white_list_size();
    YUNIT_ASSERT_MSG_QA(ret == size, "ret = %d", ret, "YOC_GAP_WHITE_SIZE_2");

    ret = ble_stack_white_list_remove(&addr);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_WHITE_REM_1");

    ret = ble_stack_white_list_size();
    YUNIT_ASSERT_MSG_QA(ret == size, "ret = %d", ret, "YOC_GAP_WHITE_SIZE_3");

    //remove non-exist
    ret = ble_stack_white_list_remove(&addr);
    YUNIT_ASSERT_MSG_QA(ret < 0, "ret = %d", ret, "YOC_GAP_WHITE_REM_2");

    ret = ble_stack_white_list_clear();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_WHITE_CLR_1");

    //clear twice
    ret = ble_stack_white_list_clear();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_WHITE_CLR_2");
}

static void CASE_blesdk_gap_set_name()
{
    int ret;
    const char name[20] = "BLE_NEW_NAME";

    ret = ble_stack_set_name(name);
    //TODO: why CONFIG_BT_DEVICE_NAME_DYNAMIC not defined explicitly but still has effect?
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GAP_SET_NAME_1");
    //TODO: add case for len = 0 and len > 28

}

static void CASE_blesdk_gatt_reg()
{
    int ret;
    ret = ble_stack_gatt_registe_service(&test_service, NULL, 3);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GATT_REG_2");


    ret = ble_stack_gatt_registe_service(&test_service, test_attrs, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GATT_REG_3");

    //TODO: why ret == 12?
    ret = ble_stack_gatt_registe_service(&test_service, test_attrs, BLE_ARRAY_NUM(test_attrs));
    YUNIT_ASSERT_MSG_QA(ret > 10, "ret = %d", ret, "YOC_GATT_REG_4");

}

static void CASE_blesdk_gatt_notify()
{
    int ret;
    uint8_t data[2] = {1, 2};

    ret = ble_stack_gatt_notificate(-1, 0, data, 2);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_NOTIFY_1");

    ret = ble_stack_gatt_notificate(0, 0, NULL, 2);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_NOTIFY_2");

    ret = ble_stack_gatt_notificate(0, 0, data, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_NOTIFY_3");

    ret = ble_stack_gatt_notificate(50, 0, data, 2);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_NOTIFY_4");

    ret = ble_stack_gatt_notificate(0, 0, data, 2);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GATT_NOTIFY_5");
}

static void CASE_blesdk_gatt_indicate()
{
    int ret;
    uint8_t data[2] = {1, 2};

    ret = ble_stack_gatt_indicate(-1, 0, data, 2);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_INDICATE_1");

    ret = ble_stack_gatt_indicate(0, 0, NULL, 2);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_INDICATE_2");

    ret = ble_stack_gatt_indicate(0, 0, data, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_INDICATE_3");

    ret = ble_stack_gatt_indicate(50, 0, data, 2);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_INDICATE_4");

    ret = ble_stack_gatt_indicate(0, 0, data, 2);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GATT_INDICATE_5");
}

static void CASE_blesdk_gatt_get_mtu()
{
    int ret;

    ret = ble_stack_gatt_mtu_get(-1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_GET_MTU_1");

    ret = ble_stack_gatt_mtu_get(50);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_GET_MTU_2");

    //not connect, att_chan_get return NULL
    ret = ble_stack_gatt_mtu_get(0);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_GATT_GET_MTU_3");
}

static void CASE_blesdk_gatt_exchange_mtu()
{
    int ret;

    ret = ble_stack_gatt_mtu_exchange(-1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_EXCH_MTU_1");

    ret = ble_stack_gatt_mtu_exchange(50);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_EXCH_MTU_2");

    ret = ble_stack_gatt_mtu_exchange(0);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_EXCH_MTU_3");
}

static void CASE_blesdk_gatt_discovery()
{
    int ret;
    uuid_t uuid = {0};

    ret = ble_stack_gatt_discovery(-1, 0, &uuid, 1, 10);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_DISCOV_1");

    ret = ble_stack_gatt_discovery_all(-1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_DISCOV_2");

    ret = ble_stack_gatt_discovery_primary(-1, &uuid, 3, 8);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_DISCOV_3");

    ret = ble_stack_gatt_discovery_include(-1, &uuid, 4, 7);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_DISCOV_4");

    ret = ble_stack_gatt_discovery_char_all(-1, 2, 9);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_DISCOV_5");

    ret = ble_stack_gatt_discovery_char(-1, &uuid, 3, 8);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_DISCOV_6");

    ret = ble_stack_gatt_discovery_descriptor_all(-1, 2, 9);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_DISCOV_7");

    ret = ble_stack_gatt_discovery_descriptor(-1, &uuid, 3, 8);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_DISCOV_8");



    ret = ble_stack_gatt_discovery(50, 0, &uuid, 1, 10);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_DISCOV_9");

    ret = ble_stack_gatt_discovery_all(50);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_DISCOV_10");

    ret = ble_stack_gatt_discovery_primary(50, &uuid, 3, 8);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_DISCOV_11");

    ret = ble_stack_gatt_discovery_include(50, &uuid, 4, 7);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_DISCOV_12");

    ret = ble_stack_gatt_discovery_char_all(50, 2, 9);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_DISCOV_13");

    ret = ble_stack_gatt_discovery_char(50, &uuid, 3, 8);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_DISCOV_14");

    ret = ble_stack_gatt_discovery_descriptor_all(50, 2, 9);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_DISCOV_15");

    ret = ble_stack_gatt_discovery_descriptor(50, &uuid, 3, 8);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_DISCOV_16");


    ret = ble_stack_gatt_discovery(0, 0, &uuid, 1, 10);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_DISCOV_17");

    //discover twice
    ret = ble_stack_gatt_discovery(0, 0, &uuid, 1, 10);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_DISCOV_18");

    //TODO: clear ble_param_pool.disc_params[0].func, and try other handy format of ble_stack_gatt_discovery

}

static void CASE_blesdk_gatt_write()
{
    int ret;
    gatt_write_en type = 0;
    uint8_t data[2] = {1, 0};

    ret = ble_stack_gatt_write(0, 1, NULL, 2, 1, type);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_WRITE_1");

    ret = ble_stack_gatt_write_response(0, 1, NULL, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_WRITE_2");

    ret = ble_stack_gatt_write_no_response(0, 1, NULL, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_WRITE_3");

    ret = ble_stack_gatt_write_signed(0, 1, NULL, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_WRITE_4");



    ret = ble_stack_gatt_write(0, 1, data, 0, 1, type);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_WRITE_5");

    ret = ble_stack_gatt_write_response(0, 1, data, 0, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_WRITE_6");

    ret = ble_stack_gatt_write_no_response(0, 1, data, 0, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_WRITE_7");

    ret = ble_stack_gatt_write_signed(0, 1, data, 0, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_WRITE_8");


    ret = ble_stack_gatt_write(-1, 1, data, 2, 1, type);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_WRITE_9");

    ret = ble_stack_gatt_write_response(-1, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_WRITE_10");

    ret = ble_stack_gatt_write_no_response(-1, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_WRITE_11");

    ret = ble_stack_gatt_write_signed(-1, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_WRITE_12");


    ret = ble_stack_gatt_write(0, 0, data, 2, 1, type);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GATT_WRITE_13");

    ret = ble_stack_gatt_write_response(0, 0, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GATT_WRITE_14");

    ret = ble_stack_gatt_write_no_response(0, 0, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GATT_WRITE_15");

    ret = ble_stack_gatt_write_signed(0, 0, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_GATT_WRITE_16");


    ret = ble_stack_gatt_write(50, 1, data, 2, 1, type);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_WRITE_17");

    ret = ble_stack_gatt_write_response(50, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_WRITE_18");

    ret = ble_stack_gatt_write_no_response(50, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_WRITE_19");

    ret = ble_stack_gatt_write_signed(50, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_WRITE_20");



    ret = ble_stack_gatt_write(0, 1, data, 2, 1, type);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_WRITE_21");

    ret = ble_stack_gatt_write_response(0, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_WRITE_22");

    ret = ble_stack_gatt_write_no_response(0, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_WRITE_23");

    ret = ble_stack_gatt_write_signed(0, 1, data, 2, 1);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_WRITE_24");

    //TODO: need to forge a connected connection
}

static void CASE_blesdk_gatt_read()
{
    int ret;
    uint16_t attr_handle[2] = {2, 3};

    ret = ble_stack_gatt_read(-1, 0, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_READ_1");

    ret = ble_stack_gatt_read(50, 0, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_READ_2");

    ret = ble_stack_gatt_read(0, 0, 0);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_READ_3");

    ret = ble_stack_gatt_read_multiple(-1, 2, attr_handle);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_READ_4");

    ret = ble_stack_gatt_read_multiple(0, 2, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_READ_5");

    ret = ble_stack_gatt_read_multiple(0, 1, attr_handle);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_GATT_READ_6");

    ret = ble_stack_gatt_read_multiple(50, 2, attr_handle);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_GATT_READ_7");

    ret = ble_stack_gatt_read_multiple(0, 2, attr_handle);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_GATT_READ_8");


    //TODO: need to forge a connected connection
}



static void CASE_blesdk_smp_before_set_cap()
{
    int ret;

    ret = ble_stack_smp_passkey_entry(0, 0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_SMP_KEY_ENTRY_1");

    ret = ble_stack_smp_cancel(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_SMP_CANCEL_1");

    ret = ble_stack_smp_passkey_confirm(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_SMP_KEY_CONFIRM_1");

    ret = ble_stack_smp_pairing_confirm(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_SMP_PAIR_CONFIRM_1");

}

static void CASE_blesdk_smp_set_cap()
{
    int ret;
    uint8_t io_cap = IO_CAP_OUT_DISPLAY;

    //output cap only
    ret = ble_stack_iocapability_set(io_cap);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_SET_CAP_1");

    //input cap only 
    io_cap = IO_CAP_IN_NONE;
    ret = ble_stack_iocapability_set(io_cap);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_SET_CAP_2");

    //valid cap
    io_cap = IO_CAP_IN_NONE | IO_CAP_OUT_DISPLAY;
    ret = ble_stack_iocapability_set(io_cap);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_SMP_SET_CAP_3");

    //set twice
    io_cap = IO_CAP_IN_KEYBOARD | IO_CAP_OUT_NONE;
    ret = ble_stack_iocapability_set(io_cap);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_SMP_SET_CAP_4");
}

static void CASE_blesdk_smp_key_entry() 
{
    int ret;

    ret = ble_stack_smp_passkey_entry(-1, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_KEY_ENTRY_2");

    ret = ble_stack_smp_passkey_entry(50, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_KEY_ENTRY_3");

    ret = ble_stack_smp_passkey_entry(0, 0);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_SMP_KEY_ENTRY_4");
}

static void CASE_blesdk_smp_cancel() 
{
    int ret;

    ret = ble_stack_smp_cancel(-1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_CANCEL_2");

    ret = ble_stack_smp_cancel(50);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_CANCEL_3");

    ret = ble_stack_smp_cancel(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_SMP_CANCEL_4");

    //TODO: add forge connected case
}

static void CASE_blesdk_smp_key_confirm() 
{
    int ret;

    ret = ble_stack_smp_passkey_confirm(-1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_KEY_CONFIRM_2");

    ret = ble_stack_smp_passkey_confirm(50);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_KEY_CONFIRM_3");

    ret = ble_stack_smp_passkey_confirm(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_SMP_KEY_CONFIRM_4");

    //TODO: add forge connected case
}

static void CASE_blesdk_smp_pair_confirm() 
{
    int ret;

    ret = ble_stack_smp_pairing_confirm(-1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_PAIR_CONFIRM_2");

    ret = ble_stack_smp_pairing_confirm(50);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_PAIR_CONFIRM_3");

    ret = ble_stack_smp_pairing_confirm(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_SMP_PAIR_CONFIRM_4");

    //TODO: add forge connected case
}

static void CASE_blesdk_smp_load_setting()
{
    int ret;

    ret = ble_stack_setting_load();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_SMP_LOAD_SETTING_1");
}

static void CASE_blesdk_smp_unpair()
{
    int ret;

    dev_addr_t addr2 = {DEV_ADDR_LE_RANDOM, DEV_ADDR2};

    ret = ble_stack_dev_unpair(NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_SMP_UNPAIR_1");

    ret = ble_stack_dev_unpair(&addr2);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_SMP_UNPAIR_2");

    //TODO: add forge connected case

}

static void CASE_blesdk_smp_get_key_size() 
{
    int ret;

    ret = ble_stack_enc_key_size_get(-1);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_GET_KEY_SIZE_1");

    ret = ble_stack_enc_key_size_get(50);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_GET_KEY_SIZE_2");

    ret = ble_stack_enc_key_size_get(0);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_SMP_GET_KEY_SIZE_3");

    //TODO: add forge encrypt connected case
}


static void CASE_blesdk_smp_security() 
{
    int ret;

    ret = ble_stack_security(-1, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_SECURITY_1");

    ret = ble_stack_security(50, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_PARAM, "ret = %d", ret, "YOC_SMP_SECURITY_2");

    ret = ble_stack_security(0, 0);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTCONN, "ret = %d", ret, "YOC_SMP_SECURITY_3");

    //TODO: add forge encrypt connected case
}

static void CASE_blesdk_profile_define()
{
    int ret, i;

    //GATT_PRIMARY_SERVICE_DEFINE
    ret = uuid_compare(test_attrs[0].uuid, UUID_GATT_PRIMARY);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_1");
    YUNIT_ASSERT_MSG_QA(test_attrs[0].perm == GATT_PERM_READ, "attr.perm = %d", test_attrs[0].perm, "YOC_PROFILE_DEF_2");
    YUNIT_ASSERT_MSG_QA(test_attrs[0].read == NULL, "attr.read = %x", test_attrs[0].read, "YOC_PROFILE_DEF_3");
    YUNIT_ASSERT_MSG_QA(test_attrs[0].write == NULL, "attr.perm = %x", test_attrs[0].write, "YOC_PROFILE_DEF_4");
    ret = uuid_compare(test_attrs[0].user_data, TEST_SERVICE_UUID);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_5");

    //GATT_CHAR_DEFINE
    ret = uuid_compare(test_attrs[1].uuid, UUID_GATT_CHRC);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_6");
    YUNIT_ASSERT_MSG_QA(test_attrs[1].perm == GATT_PERM_READ, "attr.perm = %d", test_attrs[1].perm, "YOC_PROFILE_DEF_7");
    YUNIT_ASSERT_MSG_QA(test_attrs[1].read == NULL, "attr.read = %x", test_attrs[1].read, "YOC_PROFILE_DEF_8");
    YUNIT_ASSERT_MSG_QA(test_attrs[1].write == NULL, "attr.perm = %x", test_attrs[1].write, "YOC_PROFILE_DEF_9");
    struct gatt_char_t *gattr_char = (struct gatt_char_t *)test_attrs[1].user_data;
    ret = uuid_compare(gattr_char->uuid, TEST_CHAR1_UUID);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_10");
    YUNIT_ASSERT_MSG_QA(gattr_char->properties == GATT_CHRC_PROP_READ, "gattr_char->properties = %d", gattr_char->properties, "YOC_PROFILE_DEF_11");


    //GATT_CHAR_VAL_DEFINE
    ret = uuid_compare(test_attrs[4].uuid, TEST_CHAR2_UUID);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_12");
    YUNIT_ASSERT_MSG_QA(test_attrs[4].perm == (GATT_PERM_READ | GATT_PERM_READ_AUTHEN), "attr.perm = %d", test_attrs[4].perm, "YOC_PROFILE_DEF_13");
    YUNIT_ASSERT_MSG_QA(test_attrs[4].read == NULL, "attr.read = %x", test_attrs[4].read, "YOC_PROFILE_DEF_14");
    YUNIT_ASSERT_MSG_QA(test_attrs[4].write == NULL, "attr.perm = %x", test_attrs[4].write, "YOC_PROFILE_DEF_15");
    YUNIT_ASSERT_MSG_QA(test_attrs[4].user_data == NULL, "attr.user_data = %x", test_attrs[4].user_data, "YOC_PROFILE_DEF_16");

    //GATT_CHAR_CCC_DEFINE
    ret = uuid_compare(test_attrs[10].uuid, UUID_GATT_CCC);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_17");
    YUNIT_ASSERT_MSG_QA(test_attrs[10].perm == (GATT_PERM_READ | GATT_PERM_WRITE), "attr.perm = %d", test_attrs[10].perm, "YOC_PROFILE_DEF_18");
    YUNIT_ASSERT_MSG_QA(test_attrs[10].read == NULL, "attr.read = %x", test_attrs[10].read, "YOC_PROFILE_DEF_19");
    YUNIT_ASSERT_MSG_QA(test_attrs[10].write == NULL, "attr.perm = %x", test_attrs[10].write, "YOC_PROFILE_DEF_20");
    struct bt_gatt_ccc_t *gattr_ccc = (struct bt_gatt_ccc_t *)test_attrs[10].user_data;
    YUNIT_ASSERT_MSG_QA(gattr_ccc->cfg[0].id == 0, "gattr_ccc->cfg[0].id = %d", gattr_ccc->cfg[0].id, "YOC_PROFILE_DEF_21");
    YUNIT_ASSERT_MSG_QA(gattr_ccc->cfg[0].value == 0, "gattr_ccc->cfg[0].value = %d", gattr_ccc->cfg[0].value, "YOC_PROFILE_DEF_22");

    //GATT_CHAR_CUD_DEFINE
    i = TEST_IDX_CHAR3_CUD;
    ret = uuid_compare(test_attrs[i].uuid, UUID_GATT_CUD);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_23");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].perm == (GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE), "attr.perm = %d", test_attrs[i].perm, "YOC_PROFILE_DEF_24");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].read == NULL, "attr.read = %x", test_attrs[i].read, "YOC_PROFILE_DEF_25");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].write == NULL, "attr.perm = %x", test_attrs[i].write, "YOC_PROFILE_DEF_26");
    gatt_cud_t *cud = (gatt_cud_t *)test_attrs[i].user_data;
    ret = strcmp(cud->user_des, test_svc.value3_cud);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_27");

    //GATT_SECONDARY_SERVICE_DEFINE
    i = TEST_IDX_SVC2;
    ret = uuid_compare(test_attrs[i].uuid, UUID_GATT_SECONDARY);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_28");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].perm == GATT_PERM_READ, "attr.perm = %d", test_attrs[i].perm, "YOC_PROFILE_DEF_29");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].read == NULL, "attr.read = %x", test_attrs[i].read, "YOC_PROFILE_DEF_30");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].write == NULL, "attr.perm = %x", test_attrs[i].write, "YOC_PROFILE_DEF_31");
    ret = uuid_compare(test_attrs[i].user_data, TEST_SERVICE_UUID2);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_32");

    //GATT_CHAR_DESCRIPTOR_DEFINE
    i = TEST_IDX_CHAR8_CDD;
    ret = uuid_compare(test_attrs[i].uuid, TEST_CHAR8_UUID);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_33");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].perm == (GATT_PERM_READ | GATT_PERM_WRITE | GATT_PERM_PREPARE_WRITE | GATT_PERM_WRITE_AUTHEN), "attr.perm = %d", test_attrs[i].perm, "YOC_PROFILE_DEF_34");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].read == NULL, "attr.read = %x", test_attrs[i].read, "YOC_PROFILE_DEF_35");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].write == NULL, "attr.perm = %x", test_attrs[i].write, "YOC_PROFILE_DEF_36");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].user_data == NULL, "attr.user_data = %x", test_attrs[i].user_data, "YOC_PROFILE_DEF_37");

    //GATT_CHAR_CPF_DEFINE
    i = TEST_IDX_CHAR8_CPF;
    ret = uuid_compare(test_attrs[i].uuid, UUID_GATT_CPF);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_PROFILE_DEF_38");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].perm == (GATT_PERM_READ | GATT_PERM_WRITE), "attr.perm = %d", test_attrs[i].perm, "YOC_PROFILE_DEF_39");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].read == NULL, "attr.read = %x", test_attrs[i].read, "YOC_PROFILE_DEF_40");
    YUNIT_ASSERT_MSG_QA(test_attrs[i].write == NULL, "attr.perm = %x", test_attrs[i].write, "YOC_PROFILE_DEF_41");
    gatt_cep_t *prop = (gatt_cep_t *)test_attrs[i].user_data;
    YUNIT_ASSERT_MSG_QA(prop->ext_props == 3, "prop = %d", *prop, "YOC_PROFILE_DEF_42");

}

static void CASE_blesdk_dis_init()
{
    dis_handle_t ret;
    dis_info_t info;

    ret = dis_init(NULL);
    YUNIT_ASSERT_MSG_QA(ret != NULL, "ret = %x", ret, "YOC_DIS_INIT_1");

    ret = dis_init(&info);
    YUNIT_ASSERT_MSG_QA(ret == NULL, "ret = %x", ret, "YOC_DIS_INIT_2");
}

static void CASE_blesdk_bas_init()
{
    bas_handle_t handle;
    bas_t bas;

    handle = bas_init(NULL);
    YUNIT_ASSERT_MSG_QA(handle == NULL, "handle = %x", handle, "YOC_BAS_INIT_1");

    handle = bas_init(&bas);
    YUNIT_ASSERT_MSG_QA(handle != NULL, "handle = %x", handle, "YOC_BAS_INIT_2");

    int ret;
    ret = bas_level_update(NULL, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "handle = %x", handle, "YOC_BAS_UPDATE_1");

    ret = bas_level_update(handle, 0);
    YUNIT_ASSERT_MSG_QA(ret == 0, "handle = %x", handle, "YOC_BAS_UPDATE_2");

    //TODO: condition for ble_stack_gatt_notificate get called

    handle = bas_init(&bas);
    YUNIT_ASSERT_MSG_QA(handle == NULL, "handle = %x", handle, "YOC_BAS_INIT_3");

}

static void CASE_blesdk_hrs_init()
{
    hrs_handle_t handle;
    hrs_t hrs;

    handle = hrs_init(NULL);
    YUNIT_ASSERT_MSG_QA(handle == NULL, "handle = %x", handle, "YOC_HRS_INIT_1");

    handle = hrs_init(&hrs);
    YUNIT_ASSERT_MSG_QA(handle != NULL, "handle = %x", handle, "YOC_HRS_INIT_2");

    int ret;
    uint8_t data[] = {0x01, 0x02};
    ret = hrs_measure_level_update(NULL, data, 2);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_HRS_UPDATE_1");

    ret = hrs_measure_level_update(handle, data, 2);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_HRS_UPDATE_2");

    //TODO: condition for ble_stack_gatt_notificate get called

    handle = hrs_init(&hrs);
    YUNIT_ASSERT_MSG_QA(handle == NULL, "handle = %x", handle, "YOC_HRS_INIT_3");

}

static void CASE_blesdk_hids_init()
{
    hids_handle_t handle;
    int ret;


    handle = hids_init(0);
    YUNIT_ASSERT_MSG_QA(handle != NULL, "handle = %x", handle, "YOC_HIDS_INIT_1");

    ret = hids_key_send(NULL, NULL, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_HIDS_SEND_KEY_1");

    ret = hids_key_send(handle, NULL, 0);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_HIDS_SEND_KEY_2");

    ret = hids_notify_send(NULL, NULL, 0);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_NULL, "ret = %d", ret, "YOC_HIDS_SEND_NOTIFY_1");

    ret = hids_notify_send(handle, NULL, 0);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_HIDS_SEND_NOTIFY_2");


    handle = hids_init(2);
    YUNIT_ASSERT_MSG_QA(handle == NULL, "handle = %x", handle, "YOC_HIDS_INIT_2");

    uint8_t u_data[] = {0x01, 0x02};
    ret = set_data_map(u_data, 2, 3);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_HIDS_SET_MAP_1");

    ret = set_data_map(NULL, 3, 2);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_HIDS_SET_MAP_2");

    ret = init_hids_call_func(-1, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_HIDS_INIT_CALL_1");

    ret = init_hids_call_func(-1, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_HIDS_INIT_CALL_1");

    ret = init_hids_call_func(HIDS_IDX_MAX, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_HIDS_INIT_CALL_2");

    ret = init_hids_call_func(0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_HIDS_INIT_CALL_3");

}

static void CASE_blesdk_ibeacon_init()
{
    int ret;

    uint8_t _id[2] = {0x01, 0x02};
    uint8_t _uuid[16] = {0}; 
    uint8_t _major[2] = {0x03, 0x04};
    uint8_t _minor[2] = {0x04, 0x05};
    uint8_t _measure_power = 0x00;
    char *_sd = "apple_dev";

    ret = ibeacon_start(_id, _uuid, _major, _minor, _measure_power, _sd);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_IBEACON_START_2");

    ret = ibeacon_start(_id, _uuid, _major, _minor, _measure_power, _sd);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_IBEACON_START_3");

    ret = ibeacon_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_IBEACON_STOP_2");

    ret = ibeacon_stop();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_IBEACON_STOP_3");

}

static void ota_event_callback (ota_state_en ota_state) {
    printf("ota_event_callback called\n");
}

static void CASE_blesdk_ota_init()
{
    int ret;

    ret = ble_ota_init(NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_BLE_OTA_INIT_1");

    ret = ble_ota_init(ota_event_callback);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_BLE_OTA_INIT_2");
}

static void CASE_blesdk_ota_process()
{
    int ret = -1;

    ble_ota_process();
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_OTA_PROCESS_1");
}


static int uart_client_profile_recv(const uint8_t *data, int length)
{
    printf("client data received.\n");
    return 0;
}
static int uart_server_profile_recv(const uint8_t *data, int length)
{
    printf("server data received.\n");
    return 0;
}

static int user_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        //common event
        case EVENT_GAP_CONN_CHANGE:
            printf("recv conn change evt\n");
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            printf("recv mtu exchange evt\n");
            break;

        default:
            break;
    }

    return 0;
}

//before init
static void CASE_blesdk_uart_client_before_init() 
{
    int ret;

    ret = uart_client_conn(&g_addr, &g_param);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_SERVER_CONN_1");


}

static void CASE_blesdk_uart_client_init()
{
    uart_handle_t ret;

    ret = uart_client_init(NULL);
    YUNIT_ASSERT_MSG_QA(ret == NULL, "ret = %x", ret, "YOC_BLE_UART_CLIENT_INIT_1");

    g_uart_client.uart_recv = uart_client_profile_recv;
    g_uart_client.uart_event_callback = user_event_callback;
    g_uart_client.client_data.client_conf.conn_def_on = 1;

    ret = uart_client_init(&g_uart_client);
    YUNIT_ASSERT_MSG_QA(ret != NULL, "ret = %x", ret, "YOC_BLE_UART_CLIENT_INIT_2");

}

static void CASE_blesdk_uart_server_init()
{
    uart_handle_t ret;

    ret = uart_server_init(NULL);
    YUNIT_ASSERT_MSG_QA(ret == NULL, "ret = %x", ret, "YOC_BLE_UART_SERVER_INIT_1");

    g_uart_server.uart_recv = uart_server_profile_recv;
    g_uart_server.uart_event_callback = user_event_callback;

    ret = uart_server_init(&g_uart_server);
    YUNIT_ASSERT_MSG_QA(ret != NULL, "ret = %x", ret, "YOC_BLE_UART_SERVER_INIT_2");

}

static void CASE_blesdk_uart_client_scan()
{
    #if 0
    int ret;

    ret = uart_client_scan_start();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_BLE_UART_CLIENT_SCAN_1");

    ret = uart_client_scan_start();
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_BLE_UART_CLIENT_SCAN_2");
    #endif
}

static void CASE_blesdk_uart_server_adv()
{
    int ret;

    ret = uart_server_adv_control(1, &g_adv_param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_BLE_UART_SERVER_ADV_1");

    ret = uart_server_adv_control(0, &g_adv_param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_BLE_UART_SERVER_ADV_2");

    ret = uart_server_adv_control(1, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_SERVER_ADV_3");
}

static void CASE_blesdk_dev_get()
{
    dev_addr_t *ret;

    ret = found_dev_get();
    YUNIT_ASSERT_MSG_QA(ret == NULL, "ret = %x", ret, "YOC_BLE_UART_CLIENT_DEV_GET_1");
}

static void CASE_blesdk_uart_client_conn() 
{
    int ret;

    ret = uart_client_conn(&g_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_BLE_UART_CLIENT_CONN_2");

    ret = uart_client_conn(&g_addr, &g_param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_BLE_UART_CLIENT_CONN_3");
}

static void CASE_blesdk_uart_client_disconn() 
{
    int ret;

    ret = uart_client_disconn(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_CLIENT_DISCONN_1");

    ret = uart_client_disconn(&g_uart_client);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_CLIENT_DISCONN_2");
}

static void CASE_blesdk_uart_server_disconn() 
{
    int ret;

    ret = uart_server_disconn(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_SERVER_DISCONN_1");

    ret = uart_server_disconn(&g_uart_server);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_SERVER_DISCONN_2");
}

static void CASE_blesdk_uart_client_send() 
{
    int ret;
    const char data[5] = {0x1, 0x2, 0x3, 0x4, 0x5};

    ret = uart_client_send(NULL, data, 5, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_CLIENT_SEND_1");

    ret = uart_client_send(&g_uart_client, data, 0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_CLIENT_SEND_2");

    g_uart_client.conn_handle = 0;
    ret = uart_client_send(&g_uart_client, data, 5, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_CLIENT_SEND_3");
}

static void CASE_blesdk_uart_server_send()
{
    #if 0
    int ret;
    const char data[5] = {0x1, 0x2, 0x3, 0x4, 0x5};

    ret = uart_server_send(NULL, data, 5, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_SERVER_SEND_1");

    ret = uart_server_send(&g_uart_server, data, 0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_SERVER_SEND_2");

    g_uart_server.conn_handle = 0;
    ret = uart_server_send(&g_uart_server, data, 5, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_SERVER_SEND_3");
    #endif
}


static void CASE_blesdk_uart_client_param_update() 
{
    int ret;

    ret = uart_client_conn_param_update(NULL, &g_param);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_CLIENT_PARA_UPDATE_1");

    ret = uart_client_conn_param_update(&g_uart_client, &g_param);
    YUNIT_ASSERT_MSG_QA(ret == -BLE_STACK_ERR_CONN, "ret = %d", ret, "YOC_BLE_UART_CLIENT_PARA_UPDATE_2");
}

static void CASE_blesdk_uart_server_param_update() 
{
    int ret;

    ret = uart_server_conn_param_update(NULL, &g_param);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret = %d", ret, "YOC_BLE_UART_SERVER_PARA_UPDATE_1");

    ret = uart_server_conn_param_update(&g_uart_server, &g_param);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_BLE_UART_SERVER_PARA_UPDATE_2");
}


void ble_sdk_host_test_entry(yunit_test_suite_t *suite)
{

    yunit_add_test_case(suite, "ble.sdk.sys.profile.define", CASE_blesdk_profile_define);

    yunit_add_test_case(suite, "ble.sdk.sys.gap.before_init", CASE_blesdk_gap_before_init);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.init", CASE_blesdk_gap_init);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.reg", CASE_blesdk_gap_reg);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.adv", CASE_blesdk_gap_adv);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.scan", CASE_blesdk_gap_scan);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.get_addr", CASE_blesdk_gap_get_addr);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.connect", CASE_blesdk_gap_connect);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.get_connect_info", CASE_blesdk_gap_get_connect_info);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.update_param", CASE_blesdk_gap_update_param);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.white_list", CASE_blesdk_gap_white_list);
    yunit_add_test_case(suite, "ble.sdk.sys.gap.set_name", CASE_blesdk_gap_set_name);


    yunit_add_test_case(suite, "ble.sdk.sys.gatt.reg", CASE_blesdk_gatt_reg);
    yunit_add_test_case(suite, "ble.sdk.sys.gatt.notify", CASE_blesdk_gatt_notify);
    yunit_add_test_case(suite, "ble.sdk.sys.gatt.indicate", CASE_blesdk_gatt_indicate);
    yunit_add_test_case(suite, "ble.sdk.sys.gatt.get_mtu", CASE_blesdk_gatt_get_mtu);
    yunit_add_test_case(suite, "ble.sdk.sys.gatt.exchange_mtu", CASE_blesdk_gatt_exchange_mtu);
    yunit_add_test_case(suite, "ble.sdk.sys.gatt.discovery", CASE_blesdk_gatt_discovery);
    yunit_add_test_case(suite, "ble.sdk.sys.gatt.write", CASE_blesdk_gatt_write);
    yunit_add_test_case(suite, "ble.sdk.sys.gatt.read", CASE_blesdk_gatt_read);


    yunit_add_test_case(suite, "ble.sdk.sys.smp.set_cap", CASE_blesdk_smp_set_cap);
    yunit_add_test_case(suite, "ble.sdk.sys.smp.key_entry", CASE_blesdk_smp_key_entry);
    yunit_add_test_case(suite, "ble.sdk.sys.smp.cancel", CASE_blesdk_smp_cancel);
    yunit_add_test_case(suite, "ble.sdk.sys.smp.key_confirm", CASE_blesdk_smp_key_confirm);
    yunit_add_test_case(suite, "ble.sdk.sys.smp.pair_confirm", CASE_blesdk_smp_pair_confirm);
    yunit_add_test_case(suite, "ble.sdk.sys.smp.load_setting", CASE_blesdk_smp_load_setting);
    yunit_add_test_case(suite, "ble.sdk.sys.smp.unpair", CASE_blesdk_smp_unpair);
    yunit_add_test_case(suite, "ble.sdk.sys.smp.get_key_size", CASE_blesdk_smp_get_key_size);
    yunit_add_test_case(suite, "ble.sdk.sys.smp.security", CASE_blesdk_smp_security);

    yunit_add_test_case(suite, "ble.sdk.app.dis.init", CASE_blesdk_dis_init);
    yunit_add_test_case(suite, "ble.sdk.app.bas.init", CASE_blesdk_bas_init);
    yunit_add_test_case(suite, "ble.sdk.app.hrs.init", CASE_blesdk_hrs_init);
    yunit_add_test_case(suite, "ble.sdk.app.hids.init", CASE_blesdk_hids_init);
    yunit_add_test_case(suite, "ble.sdk.app.ibeacon.init", CASE_blesdk_ibeacon_init);
    yunit_add_test_case(suite, "ble.sdk.app.ota.init", CASE_blesdk_ota_init);
    yunit_add_test_case(suite, "ble.sdk.app.ota.process", CASE_blesdk_ota_process);

    yunit_add_test_case(suite, "ble.sdk.app.uart_client.init", CASE_blesdk_uart_client_init);
    yunit_add_test_case(suite, "ble.sdk.app.uart_client.scan", CASE_blesdk_uart_client_scan);
    yunit_add_test_case(suite, "ble.sdk.app.uart_client.dev_get", CASE_blesdk_dev_get);
    yunit_add_test_case(suite, "ble.sdk.app.uart_client.conn", CASE_blesdk_uart_client_conn);
    yunit_add_test_case(suite, "ble.sdk.app.uart_client.disconn", CASE_blesdk_uart_client_disconn);
    yunit_add_test_case(suite, "ble.sdk.app.uart_client.send", CASE_blesdk_uart_client_send);
    yunit_add_test_case(suite, "ble.sdk.app.uart_client.param_update", CASE_blesdk_uart_client_param_update);

    yunit_add_test_case(suite, "ble.sdk.app.uart_server.init", CASE_blesdk_uart_server_init);
    yunit_add_test_case(suite, "ble.sdk.app.uart_server.send", CASE_blesdk_uart_server_send);
    yunit_add_test_case(suite, "ble.sdk.app.uart_server.disconn", CASE_blesdk_uart_server_disconn);
    yunit_add_test_case(suite, "ble.sdk.app.uart_server.adv", CASE_blesdk_uart_server_adv);
    yunit_add_test_case(suite, "ble.sdk.app.uart_server.param_update", CASE_blesdk_uart_server_param_update);
}

