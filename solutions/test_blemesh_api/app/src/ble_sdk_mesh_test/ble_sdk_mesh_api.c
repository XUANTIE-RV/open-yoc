/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include <api/mesh.h>
#include "mesh_node.h"
#include "mesh_model/mesh_model.h"

#include "../yunit.h"
#include "net.h"
#include <errno.h>

#define BLEMESH_TEST_CASE_STR "YOC_BLEMESH_"
typedef enum {
    BLEMESH_AT_MESH_INIT = 0,

    BLEMESH_PROVISIONER_INIT,
    BLEMESH_PROVISIONER_ENABLE,
    BLEMESH_PROVISIONER_DISABLE,
    BLEMESH_PROVISIONER_DEV_FILTER,
    BLEMESH_PROVISIONER_DEV_ADD,
    BLEMESH_PROVISIONER_DEV_DEL,
    BLEMESH_NODE_OOB_INPUT_NUM,
    BLEMESH_NODE_OOB_INPUT_STR,
    BLEMESH_PROVISIONER_STATIC_OOB_SET,
    BLEMESH_PROVISIONER_GET_PROVISIONER_DATA,

    BLEMESH_NODE_INIT,

    BLEMESH_MODEL_INIT,
    BLEMESH_MODEL_GET_COMP_DATA,
    BLEMESH_MODEL_SET_CB,
    BLEMESH_MODEL_STATUS_GET,
    BLEMESH_MODEL_APPKEY_ADD,
    BLEMESH_MODEL_AUTOCONFIG,

    BLEMESH_GENERIC_ONOFF_SET,
    BLEMESH_GENERIC_ONOFF_GET,
    BLEMESH_GENERIC_MOVE_SET,
    BLEMESH_GENERIC_DELTA_SET,
    BLEMESH_GENERIC_LEVEL_SET,
    BLEMESH_GENERIC_LEVEL_GET,
    BLEMESH_LIGHT_LIGHTNESS_GET,
    BLEMESH_LIGHT_LIGHTNESS_SET,
    BLEMESH_LIGHT_LIGHTNESS_LINEAR_GET,
    BLEMESH_LIGHT_LIGHTNESS_LINEAR_SET,
    BLEMESH_LIGHT_LIGHTNESS_LAST_GET,
    BLEMESH_LIGHT_LIGHTNESS_DEF_GET,
    BLEMESH_LIGHT_DEF_SET,
    BLEMESH_LIGHT_LIGHTNESS_RANGE_GET,
    BLEMESH_LIGHT_LIGHTNESS_RANGE_SET,
    BLEMESH_LIGHT_CTL_GET,
    BLEMESH_LIGHT_CTL_SET,
    BLEMESH_LIGHT_CTL_TEMP_SET,
    BLEMESH_LIGHT_CTL_TEMP_GET,
    BLEMESH_LIGHT_CTL_DEF_SET,
    BLEMESH_LIGHT_CTL_DEF_GET,
    BLEMESH_LIGHT_CTL_TEMP_RANGE_SET,
    BLEMESH_LIGHT_CTL_TEMP_RANGE_GET,
    BLEMESH_VENDOR_CLI_MODEL_MSG_SEND,
    BLEMESH_VENDOR_SRV_MODEL_MSG_SEND,
} BLEMESH_API_TEST_CASE_e;

#define DEV_ADDR                                                                                   \
    {                                                                                              \
        0xCC, 0x3B, 0xE3, 0x82, 0xBA, 0xC0                                                         \
    }
#define DEV_ADDR2                                                                                  \
    {                                                                                              \
        0xCC, 0x3B, 0xE3, 0x82, 0xBA, 0xC2                                                         \
    }
#define DEV_NAME "MESH_INIT"
#define VALUE_LEN 100

#define CUR_FAULTS_MAX 4

static u8_t cur_faults[CUR_FAULTS_MAX];
static u8_t reg_faults[CUR_FAULTS_MAX * 2];

#define BT_COMP_ID_LF 0x05f1
#define OP_VENDOR_BUTTON BT_MESH_MODEL_OP_3(0x00, BT_COMP_ID_LF)

static void vnd_func(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                     struct net_buf_simple *buf)
{
    printf("enter vnd_func\r\n");
}

static struct bt_mesh_model_op vnd_ops[] = {
    {OP_VENDOR_BUTTON, 0, vnd_func},
    {0, 0, NULL},
};

static struct bt_mesh_model vnd_models[] = {
    BT_MESH_MODEL_VND(BT_COMP_ID_LF, 0x0, vnd_ops, NULL, NULL),
};

static void get_faults(u8_t *faults, u8_t faults_size, u8_t *dst, u8_t *count)
{
    u8_t i, limit = *count;

    for (i = 0, *count = 0; i < faults_size && *count < limit; i++) {
        if (faults[i]) {
            *dst++ = faults[i];
            (*count)++;
        }
    }
}

static int fault_get_cur(struct bt_mesh_model *model, u8_t *test_id, u16_t *company_id,
                         u8_t *faults, u8_t *fault_count)
{
    printf("Sending current faults\n");

    *test_id    = 0x00;
    *company_id = BT_COMP_ID_LF;

    get_faults(cur_faults, sizeof(cur_faults), faults, fault_count);

    return 0;
}

static int fault_get_reg(struct bt_mesh_model *model, u16_t cid, u8_t *test_id, u8_t *faults,
                         u8_t *fault_count)
{
    if (cid != BT_COMP_ID_LF) {
        printf("Faults requested for unknown Company ID 0x%04x\n", cid);
        return -EINVAL;
    }

    printf("Sending registered faults\n");

    *test_id = 0x00;

    get_faults(reg_faults, sizeof(reg_faults), faults, fault_count);

    return 0;
}

static int fault_clear(struct bt_mesh_model *model, uint16_t cid)
{
    if (cid != BT_COMP_ID_LF) {
        return -EINVAL;
    }

    memset(reg_faults, 0, sizeof(reg_faults));

    return 0;
}

static int fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t cid)
{
    if (cid != BT_COMP_ID_LF) {
        return -EINVAL;
    }

    if (test_id != 0x00) {
        return -EINVAL;
    }

    return 0;
}

static const struct bt_mesh_health_srv_cb my_health_srv_cb = {
    .fault_get_cur = fault_get_cur,
    .fault_get_reg = fault_get_reg,
    .fault_clear   = fault_clear,
    .fault_test    = fault_test,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &my_health_srv_cb,
};

static uint8_t g_onoff = 0;
static uint8_t tid;
static int     on_off_update(struct bt_mesh_model *mod);

BT_MESH_HEALTH_PUB_DEFINE(health_pub, CUR_FAULTS_MAX);
BT_MESH_MODEL_PUB_DEFINE(gen_onoff_pub, on_off_update, 2);

static int on_off_update(struct bt_mesh_model *mod)
{
    uint8_t msg[2];
    g_onoff = !g_onoff;
    msg[0]  = g_onoff;
    msg[1]  = tid;

    struct net_buf_simple *msg_buf = gen_onoff_pub.msg;
    bt_mesh_model_msg_init(msg_buf, BT_MESH_MODEL_OP_2(0x82, 0x03));
    net_buf_simple_add_mem(msg_buf, msg, 2);
    printf("PUB LED %s, TID %d\n", g_onoff ? "ON" : "OFF", tid);
    tid++;
    return 0;
}

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay  = BT_MESH_RELAY_DISABLED,
    .beacon = BT_MESH_BEACON_DISABLED,
#if defined(CONFIG_BT_MESH_FRIEND)
    .frnd = BT_MESH_FRIEND_DISABLED,
#else
    .frnd       = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
    .gatt_proxy = BT_MESH_GATT_PROXY_DISABLED,
#else
    .gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif

    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit     = BT_MESH_TRANSMIT(2, 20),
    .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

static struct bt_mesh_cfg_cli cfg_cli = {};

static void show_faults(u8_t test_id, u16_t cid, u8_t *faults, size_t fault_count)
{
    size_t i;

    if (!fault_count) {
        printf("Health Test ID 0x%02x Company ID 0x%04x: no faults\n", test_id, cid);
        return;
    }

    printf("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:\n", test_id, cid, fault_count);

    for (i = 0; i < fault_count; i++) {
        printf("\t0x%02x\n", faults[i]);
    }
}

static void health_current_status(struct bt_mesh_health_cli *cli, u16_t addr, u8_t test_id,
                                  u16_t cid, u8_t *faults, size_t fault_count)
{
    printf("Health Current Status from 0x%04x\n", addr);
    show_faults(test_id, cid, faults, fault_count);
}

static struct bt_mesh_health_cli health_cli = {
    .current_status = health_current_status,
};

static void gen_onoff_status(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    g_onoff = buf->data[0];
    printf("src addr:%04x ,LED %s\n", ctx->addr, buf->data[0] ? "ON" : "OFF");
}

static const struct bt_mesh_model_op gen_onoff_op[] = {
    {BT_MESH_MODEL_OP_2(0x82, 0x04), 0, gen_onoff_status},
    BT_MESH_MODEL_OP_END,
};

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_CFG_CLI(&cfg_cli),
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
    BT_MESH_MODEL_HEALTH_CLI(&health_cli),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_op, &gen_onoff_pub, NULL),
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models, 0),
};

static const struct bt_mesh_comp g_comp = {
    .cid        = BT_COMP_ID_LF,
    .elem       = elements,
    .elem_count = ARRAY_SIZE(elements),
};

extern struct bt_mesh_net bt_mesh;

static const u8_t default_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
};

static struct bt_mesh_prov        prov;
static struct bt_mesh_comp        comp;
static struct bt_mesh_provisioner provisioner;

static const uint8_t prov_uuid[] = {0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0x93};

static void my_test_adv_pkt_notify(const u8_t addr[6], const u8_t addr_type, const u8_t adv_type,
                                   const u8_t dev_uuid[16], u16_t oob_info,
                                   bt_mesh_prov_bearer_t bearer)
{
    printf("my_test_adv_pkt_notify get called!\n");
}

static void CASE_blesdk_mesh_before_init()
{
    int ret;

    ret = bt_mesh_prov_enable(BT_MESH_PROV_ADV);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROV_EN_1");

    ret = bt_mesh_prov_enable(BT_MESH_PROV_GATT);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROV_EN_2");

    ret = bt_mesh_prov_disable(BT_MESH_PROV_ADV);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROV_DIS_1");

    ret = bt_mesh_prov_disable(BT_MESH_PROV_GATT);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROV_DIS_2");

    struct bt_mesh_elem *elem;
    elem = bt_mesh_model_elem(&root_models[0]);
    YUNIT_ASSERT_MSG_QA(elem == NULL, "ret = %d", ret, "YOC_MESH_MODEL_ELEM_1");

    elem = bt_mesh_model_elem(NULL);
    YUNIT_ASSERT_MSG_QA(elem == NULL, "ret = %d", ret, "YOC_MESH_MODEL_ELEM_1");
}

static void CASE_blesdk_mesh_init()
{
    int ret = 0;

    bt_mesh_reset();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_RESET_1");

    ret = bt_mesh_init(NULL, &comp, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INIT_1");

    ret = bt_mesh_init(&prov, NULL, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INIT_2");

    prov.uuid = NULL;
    ret       = bt_mesh_init(&prov, &comp, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INIT_3");

    prov.uuid     = prov_uuid;
    prov.oob_info = (BT_MESH_PROV_OOB_ON_DEV << 1);
    ret           = bt_mesh_init(&prov, &comp, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INIT_4");

    prov.oob_info       = BT_MESH_PROV_OOB_ON_DEV;
    prov.output_actions = (BT_MESH_DISPLAY_STRING << 1);
    ret                 = bt_mesh_init(&prov, &comp, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INIT_5");

    prov.output_actions = BT_MESH_DISPLAY_STRING;
    prov.input_actions  = (BT_MESH_ENTER_STRING << 1);
    ret                 = bt_mesh_init(&prov, &comp, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INIT_6");

    prov.input_actions = BT_MESH_ENTER_STRING;
    //deal with bt_mesh_comp_register
    comp.elem_count = 0;
    ret             = bt_mesh_init(&prov, &comp, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INIT_7");

    ret = bt_mesh_init(&prov, &g_comp, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_INIT_8");

    //init twice
    ret = bt_mesh_init(&prov, &g_comp, &provisioner);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_MESH_INIT_9");

    ret = bt_mesh_prov_enable(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROV_EN_3");

    ret = bt_mesh_prov_enable(BT_MESH_PROV_GATT << 1);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROV_EN_4");

    ret = bt_mesh_prov_disable(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROV_DIS_3");

    ret = bt_mesh_prov_disable(BT_MESH_PROV_GATT << 1);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROV_DIS_4");

    atomic_set_bit(bt_mesh.flags, BT_MESH_VALID);

    ret = bt_mesh_suspend(0);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUSPEND_1");

    ret = bt_mesh_suspend(0);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_MESH_SUSPEND_2");

    ret = bt_mesh_resume();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_RESUME_1");

    ret = bt_mesh_resume();
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_MESH_RESUME_2");
}

static void CASE_blesdk_mesh_provision()
{
    int ret;

    uint16_t net_idx  = 0;
    uint16_t addr     = 0;
    uint8_t  flags    = 0;
    uint32_t iv_index = 0;

    //valid set
    ret = bt_mesh_is_provisioned();
    YUNIT_ASSERT_MSG_QA(ret == 1, "ret = %d", ret, "YOC_MESH_IS_PROV_1");

    ret = bt_mesh_iv_update();
    YUNIT_ASSERT_MSG_QA(ret == 1, "ret = %d", ret, "YOC_MESH_IV_UPDATE_1");

    bt_mesh.ivu_duration = BT_MESH_IVU_MIN_HOURS;
    ret                  = bt_mesh_iv_update();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_IV_UPDATE_2");

    ret = bt_mesh_iv_update();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_IV_UPDATE_3");

    ret = bt_mesh_prov_enable(BT_MESH_PROV_ADV);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_MESH_PROV_EN_5");

    ret = bt_mesh_prov_enable(BT_MESH_PROV_GATT);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_MESH_PROV_EN_6");

    ret = bt_mesh_prov_disable(BT_MESH_PROV_ADV);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_MESH_PROV_DIS_5");

    ret = bt_mesh_prov_disable(BT_MESH_PROV_GATT);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_MESH_PROV_DIS_6");

    ret = bt_mesh_provision(default_key, net_idx, flags, iv_index, addr, default_key);
    YUNIT_ASSERT_MSG_QA(ret == -EALREADY, "ret = %d", ret, "YOC_MESH_PROV_1");

    atomic_clear_bit(bt_mesh.flags, BT_MESH_VALID);
    //valid clear
    ret = bt_mesh_is_provisioned();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_IS_PROV_2");

    ret = bt_mesh_iv_update();
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_IV_UPDATE_4");

    ret = bt_mesh_prov_disable(BT_MESH_PROV_ADV);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_PROV_DIS_7");

    ret = bt_mesh_prov_disable(BT_MESH_PROV_GATT);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_PROV_DIS_8");

    ret = bt_mesh_prov_enable(BT_MESH_PROV_ADV);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_PROV_EN_7");

    ret = bt_mesh_prov_enable(BT_MESH_PROV_GATT);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_PROV_EN_8");

    ret = bt_mesh_provision(default_key, net_idx, flags, iv_index, addr, default_key);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_PROV_2");
}

static void CASE_blesdk_mesh_input()
{
    int ret;

    const char *str = "hello, world!";

    ret = bt_mesh_input_string(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INPUT_STR_1");

    ret = bt_mesh_input_string(str);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INPUT_STR_2");

    ret = bt_mesh_input_number(0);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_INPUT_NUM_1");

    //TODO: set link flags
}

static void CASE_blesdk_mesh_model_send()
{
    int ret;

    struct bt_mesh_msg_ctx ctx     = {.net_idx = 0xffff, .app_idx = 0xffff, .addr = 0x5};
    uint16_t               msg_len = 20;
    NET_BUF_SIMPLE_DEFINE(msg_buf, 2 + msg_len + 4);

    ret = bt_mesh_model_send(NULL, &ctx, &msg_buf, NULL, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_MODEL_SEND_1");

    ret = bt_mesh_model_send(vnd_models, NULL, &msg_buf, NULL, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_MODEL_SEND_2");

    ret = bt_mesh_model_send(vnd_models, &ctx, NULL, NULL, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_MODEL_SEND_3");

    //bt_mesh_trans_send return EINVAL
    ret = bt_mesh_model_send(&root_models[0], &ctx, &msg_buf, NULL, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_MODEL_SEND_4");

    ret = bt_mesh_model_publish(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_MODEL_PUB_1");

    ret = bt_mesh_model_publish(&root_models[0]);
    YUNIT_ASSERT_MSG_QA(ret == -ENOTSUP, "ret = %d", ret, "YOC_MESH_MODEL_PUB_2");

    ret = bt_mesh_model_publish(&root_models[4]);
    YUNIT_ASSERT_MSG_QA(ret == -EADDRNOTAVAIL, "ret = %d", ret, "YOC_MESH_MODEL_PUB_3");

    struct bt_mesh_elem *elem;
    elem = bt_mesh_model_elem(&root_models[0]);
    YUNIT_ASSERT_MSG_QA(elem != NULL, "ret = %d", ret, "YOC_MESH_MODEL_ELEM_3");
}

static void CASE_blesdk_mesh_comp_get()
{
    NET_BUF_SIMPLE_DEFINE(comp, 32);
    u8_t status, page = 0x00;
    int  ret = 0;

    ret = bt_mesh_cfg_comp_data_get(0, 0, page, &status, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_COMP_GET_1");

    ret = bt_mesh_cfg_comp_data_get(0, 0, page, NULL, &comp);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_COMP_GET_2");

    //TODO: more cases..
}

static void CASE_blesdk_mesh_beacon()
{
    u8_t status = 0x00;
    int  ret;

    ret = bt_mesh_cfg_beacon_set(0, 0, BT_MESH_BEACON_ENABLED + 1, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_BEACON_SET_1");

    ret = bt_mesh_cfg_beacon_set(0, 0, BT_MESH_BEACON_ENABLED, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_BEACON_SET_2");

    ret = bt_mesh_cfg_beacon_get(0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_BEACON_GET_1");
}

static void CASE_blesdk_mesh_ttl()
{
    u8_t ttl, val = 0x00;
    int  ret;

    ret = bt_mesh_cfg_ttl_set(0, 0, val, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_TTL_SET_1");

    val = 0x01;
    ret = bt_mesh_cfg_ttl_set(0, 0, val, &ttl);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_TTL_SET_2");

    val = BT_MESH_TTL_MAX + 1;
    ret = bt_mesh_cfg_ttl_set(0, 0, val, &ttl);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_TTL_SET_3");

    val = BT_MESH_TTL_MAX - 1;
    ret = bt_mesh_cfg_ttl_set(0, 0, val, &ttl);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_TTL_SET_4");

    ret = bt_mesh_cfg_ttl_get(0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_TTL_GET_1");

    ret = bt_mesh_cfg_ttl_get(0, 0, &ttl);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_TTL_GET_2");
}

static void CASE_blesdk_mesh_friend()
{
    u8_t status, val = 0x00;
    int  ret;

    ret = bt_mesh_cfg_friend_set(0, 0, val, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_FRIEND_SET_1");

    val = BT_MESH_FRIEND_ENABLED + 1;
    ret = bt_mesh_cfg_friend_set(0, 0, val, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_FRIEND_SET_2");

    ret = bt_mesh_cfg_friend_get(0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_FRIEND_GET_1");
}

static void CASE_blesdk_mesh_proxy()
{
    u8_t status, val = 0x00;
    int  ret;

    ret = bt_mesh_cfg_gatt_proxy_set(0, 0, val, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_PROXY_SET_1");

    val = BT_MESH_GATT_PROXY_ENABLED + 1;
    ret = bt_mesh_cfg_gatt_proxy_set(0, 0, val, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PROXY_SET_2");

    ret = bt_mesh_cfg_gatt_proxy_get(0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_PROXY_GET_1");
}

static void CASE_blesdk_mesh_relay()
{
    u8_t status, transmit = 0x00;
    int  ret;

    ret = bt_mesh_cfg_relay_set(0, 0, 0, 0, &status, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_RELAY_SET_1");

    ret = bt_mesh_cfg_relay_set(0, 0, BT_MESH_RELAY_ENABLED + 1, 0, &status, &transmit);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_RELAY_SET_2");

    ret = bt_mesh_cfg_relay_set(0, 0, BT_MESH_RELAY_ENABLED, 0, &status, &transmit);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_RELAY_SET_3");

    ret = bt_mesh_cfg_relay_get(0, 0, &status, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_RELAY_GET_1");

    ret = bt_mesh_cfg_relay_get(0, 0, &status, &transmit);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_RELAY_GET_2");
}

static void CASE_blesdk_mesh_pub()
{
    struct bt_mesh_cfg_mod_pub pub    = {0};
    u8_t                       status = 0x00;
    u16_t                      cid    = CID_NVAL;
    int                        ret;

    ret = bt_mesh_cfg_mod_pub_set(0, 0, 0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PUB_SET_1");

    ret = bt_mesh_cfg_mod_pub_set_vnd(0, 0, 0, 0, cid, &pub, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PUB_VND_SET_1");

    ret = bt_mesh_cfg_mod_pub_set_vnd(0, 0, 0, 0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PUB_VND_SET_2");

    //cli_wait()
    ret = bt_mesh_cfg_mod_pub_set(0, 0, 0, 0, &pub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_PUB_SET_2");

    //bt_mesh_model_send->model_send->bt_mesh_trans_send->send_seg
    ret = bt_mesh_cfg_mod_pub_set_vnd(0, 0, 0, 0, 0, &pub, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_PUB_VND_SET_3");

    ret = bt_mesh_cfg_mod_pub_get(0, 0, 0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PUB_GET_1");

    ret = bt_mesh_cfg_mod_pub_get_vnd(0, 0, 0, 0, cid, &pub, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PUB_VND_GET_1");

    ret = bt_mesh_cfg_mod_pub_get(0, 0, 0, 0, &pub, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PUB_GET_2");

    ret = bt_mesh_cfg_mod_pub_get_vnd(0, 0, 0, 0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PUB_VND_GET_2");

    ret = bt_mesh_cfg_mod_pub_get(0, 0, 0, 0, NULL, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_PUB_GET_3");

    ret = bt_mesh_cfg_mod_pub_get_vnd(0, 0, 0, 0, 0, &pub, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_PUB_VND_GET_3");

    ret = bt_mesh_cfg_mod_pub_get(0, 0, 0, 0, &pub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_PUB_GET_4");

    ret = bt_mesh_cfg_mod_pub_get_vnd(0, 0, 0, 0, 0, NULL, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_PUB_VND_GET_4");

    ret = bt_mesh_cfg_mod_pub_get_vnd(0, 0, 0, 0, 0, &pub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_PUB_VND_GET_5");
}

static void CASE_blesdk_mesh_hb_pub()
{
    struct bt_mesh_cfg_hb_pub pub    = {0};
    u8_t                      status = 0x00;
    int                       ret;

    ret = bt_mesh_cfg_hb_pub_set(0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HB_PUB_SET_1");

    ret = bt_mesh_cfg_hb_pub_set(0, 0, &pub, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_HB_PUB_SET_2");

    ret = bt_mesh_cfg_hb_pub_set(0, 0, &pub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_HB_PUB_SET_3");

    ret = bt_mesh_cfg_hb_pub_get(0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HB_PUB_GET_1");

    ret = bt_mesh_cfg_hb_pub_get(0, 0, &pub, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_HB_PUB_GET_2");

    ret = bt_mesh_cfg_hb_pub_set(0, 0, &pub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_HB_PUB_GET_3");
}

static void CASE_blesdk_mesh_hb_sub()
{
    struct bt_mesh_cfg_hb_sub sub    = {0};
    u8_t                      status = 0x00;
    int                       ret;

    ret = bt_mesh_cfg_hb_sub_set(0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HB_SUB_SET_1");

    ret = bt_mesh_cfg_hb_sub_set(0, 0, &sub, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_HB_SUB_SET_2");

    ret = bt_mesh_cfg_hb_sub_set(0, 0, &sub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_HB_SUB_SET_3");

    ret = bt_mesh_cfg_hb_sub_get(0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HB_SUB_GET_1");

    ret = bt_mesh_cfg_hb_sub_get(0, 0, &sub, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_HB_SUB_GET_2");

    ret = bt_mesh_cfg_hb_sub_set(0, 0, &sub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_HB_SUB_GET_3");
}

static void CASE_blesdk_mesh_sub()
{
    uint16_t sub, cid = 0x00;
    u8_t     status = 0x00;
    int      ret;

    ret = bt_mesh_cfg_mod_sub_get(0, 0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_GET_1");

    ret = bt_mesh_cfg_mod_sub_get_vnd(0, 0, 0, cid, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VND_GET_1");

    ret = bt_mesh_cfg_mod_sub_get(0, 0, 0, &sub, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_GET_2");

    ret = bt_mesh_cfg_mod_sub_get_vnd(0, 0, 0, cid, &sub, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VND_GET_2");

    ret = bt_mesh_cfg_mod_sub_get(0, 0, 0, NULL, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_SUB_GET_3");

    ret = bt_mesh_cfg_mod_sub_get_vnd(0, 0, 0, cid, NULL, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VND_GET_3");

    ret = bt_mesh_cfg_mod_sub_get(0, 0, 0, &sub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_SUB_GET_4");

    ret = bt_mesh_cfg_mod_sub_get_vnd(0, 0, 0, cid, &sub, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_SUB_VND_GET_4");
}

static void CASE_blesdk_mesh_krp()
{
    u8_t phase, status = 0x00;
    int  ret;

    ret = bt_mesh_cfg_krp_set(0, 0, 0, &phase, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_KRP_SET_1");

    ret = bt_mesh_cfg_krp_set(0, 0, 0, &phase, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_KRP_SET_2");

    ret = bt_mesh_cfg_krp_get(0, 0, 0, &phase, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_KRP_GET_1");

    ret = bt_mesh_cfg_krp_get(0, 0, 0, &phase, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_KRP_GET_2");
}

static void CASE_blesdk_mesh_timeout()
{
    s32_t ret;

    ret = bt_mesh_cfg_cli_timeout_get();
    YUNIT_ASSERT_MSG_QA(ret == 2, "ret = %d", ret, "YOC_MESH_TIMEOUT_GET_1");

    bt_mesh_cfg_cli_timeout_set(1);
    YUNIT_ASSERT_MSG_QA(ret == 2, "ret = %d", ret, "YOC_MESH_TIMEOUT_SET_1");

    ret = bt_mesh_cfg_cli_timeout_get();
    YUNIT_ASSERT_MSG_QA(ret == 1, "ret = %d", ret, "YOC_MESH_TIMEOUT_GET_2");
}

static void CASE_blesdk_mesh_sub_add_del()
{
    u8_t status = 0x00;
    int  ret;

    ret = bt_mesh_cfg_mod_sub_add(0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_SUB_ADD_1");

    ret = bt_mesh_cfg_mod_sub_add(0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_SUB_ADD_2");

    ret = bt_mesh_cfg_mod_sub_del(0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_SUB_DEL_1");

    ret = bt_mesh_cfg_mod_sub_del(0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_SUB_DEL_2");
}

static void CASE_blesdk_mesh_sub_overwrite()
{
    u8_t status = 0x00;
    int  ret;

    ret = bt_mesh_cfg_mod_sub_overwrite(0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_SUB_OVERWRITE_1");

    ret = bt_mesh_cfg_mod_sub_overwrite(0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_SUB_OVERWRITE_2");

    ret = bt_mesh_cfg_mod_sub_overwrite_vnd(0, 0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VND_OVERWRITE_1");

    ret = bt_mesh_cfg_mod_sub_overwrite_vnd(0, 0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_SUB_VND_OVERWRITE_2");

    ret = bt_mesh_cfg_mod_sub_overwrite_vnd(0, 0, 0, 0, 0, CID_NVAL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VND_OVERWRITE_3");
}

static void CASE_blesdk_mesh_sub_va_overwrite()
{
    u8_t       status    = 0x00;
    u16_t      virt_addr = 0x00;
    const u8_t label[16] = {0};
    int        ret;

    // msg->len > 11, send_seg
    ret = bt_mesh_cfg_mod_sub_va_overwrite(0, 0, 0, label, 0, &virt_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VA_OVERWRITE_1");

    ret = bt_mesh_cfg_mod_sub_va_overwrite(0, 0, 0, label, 0, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_SUB_VA_OVERWRITE_2");

    ret = bt_mesh_cfg_mod_sub_va_overwrite(0, 0, 0, label, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_OVERWRITE_3");

    ret = bt_mesh_cfg_mod_sub_va_overwrite_vnd(0, 0, 0, label, 0, 0, &virt_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_OVERWRITE_1");

    ret = bt_mesh_cfg_mod_sub_va_overwrite_vnd(0, 0, 0, label, 0, 0, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_OVERWRITE_2");

    ret = bt_mesh_cfg_mod_sub_va_overwrite_vnd(0, 0, 0, label, 0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_OVERWRITE_3");

    ret = bt_mesh_cfg_mod_sub_va_overwrite_vnd(0, 0, 0, label, 0, CID_NVAL, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_OVERWRITE_4");
}

static void CASE_blesdk_mesh_sub_va_add_del()
{
    u8_t       status    = 0x00;
    u16_t      virt_addr = 0x00;
    const u8_t label[16] = {0};
    int        ret;

    ret = bt_mesh_cfg_mod_sub_va_add(0, 0, 0, label, 0, &virt_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VA_ADD_1");

    ret = bt_mesh_cfg_mod_sub_va_add(0, 0, 0, label, 0, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_SUB_VA_ADD_2");

    ret = bt_mesh_cfg_mod_sub_va_add(0, 0, 0, label, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_ADD_3");

    ret = bt_mesh_cfg_mod_sub_va_del(0, 0, 0, label, 0, &virt_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VA_DEL_1");

    ret = bt_mesh_cfg_mod_sub_va_del(0, 0, 0, label, 0, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_SUB_VA_DEL_2");

    ret = bt_mesh_cfg_mod_sub_va_del(0, 0, 0, label, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_DEL_3");
}

static void CASE_blesdk_mesh_sub_va_vnd_add_del()
{
    u8_t       status    = 0x00;
    u16_t      virt_addr = 0x00;
    const u8_t label[16] = {0};
    int        ret;

    ret = bt_mesh_cfg_mod_sub_va_add_vnd(0, 0, 0, label, 0, 0, &virt_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_ADD_1");

    ret = bt_mesh_cfg_mod_sub_va_add_vnd(0, 0, 0, label, 0, 0, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_ADD_2");

    ret = bt_mesh_cfg_mod_sub_va_add_vnd(0, 0, 0, label, 0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_ADD_3");

    ret = bt_mesh_cfg_mod_sub_va_add_vnd(0, 0, 0, label, 0, CID_NVAL, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_ADD_4");

    ret = bt_mesh_cfg_mod_sub_va_del_vnd(0, 0, 0, label, 0, 0, &virt_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_DEL_1");

    ret = bt_mesh_cfg_mod_sub_va_del_vnd(0, 0, 0, label, 0, 0, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_DEL_2");

    ret = bt_mesh_cfg_mod_sub_va_del_vnd(0, 0, 0, label, 0, 0, NULL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_DEL_3");

    ret = bt_mesh_cfg_mod_sub_va_del_vnd(0, 0, 0, label, 0, CID_NVAL, &virt_addr, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VA_VND_DEL_4");
}

static void CASE_blesdk_mesh_sub_vnd_add_del()
{
    u8_t status = 0x00;
    int  ret;

    ret = bt_mesh_cfg_mod_sub_add_vnd(0, 0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VND_ADD_1");

    ret = bt_mesh_cfg_mod_sub_add_vnd(0, 0, 0, 0, 0, CID_NVAL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VND_ADD_2");

    ret = bt_mesh_cfg_mod_sub_add_vnd(0, 0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_SUB_VND_ADD_3");

    ret = bt_mesh_cfg_mod_sub_del_vnd(0, 0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_SUB_VND_DEL_1");

    ret = bt_mesh_cfg_mod_sub_del_vnd(0, 0, 0, 0, 0, CID_NVAL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_SUB_VND_DEL_2");

    ret = bt_mesh_cfg_mod_sub_del_vnd(0, 0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_SUB_VND_DEL_3");
}

static void CASE_blesdk_mesh_app_bind()
{
    u8_t status = 0x00;
    int  ret;

    ret = bt_mesh_cfg_mod_app_bind(0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_APP_BIND_1");

    ret = bt_mesh_cfg_mod_app_bind(0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_APP_BIND_2");

    ret = bt_mesh_cfg_mod_app_bind_vnd(0, 0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_APP_VND_BIND_1");

    ret = bt_mesh_cfg_mod_app_bind_vnd(0, 0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_APP_VND_BIND_2");

    ret = bt_mesh_cfg_mod_app_bind_vnd(0, 0, 0, 0, 0, CID_NVAL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_APP_VND_BIND_3");
}

static void CASE_blesdk_mesh_app_unbind()
{
    u8_t status = 0x00;
    int  ret;

    ret = bt_mesh_cfg_mod_app_unbind(0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_APP_UNBIND_1");

    ret = bt_mesh_cfg_mod_app_unbind(0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_APP_UNBIND_2");

    ret = bt_mesh_cfg_mod_app_unbind_vnd(0, 0, 0, 0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(-EINVAL, "ret = %d", ret, "YOC_MESH_APP_VND_UNBIND_1");

    ret = bt_mesh_cfg_mod_app_unbind_vnd(0, 0, 0, 0, 0, 0, &status);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_APP_VND_UNBIND_2");

    ret = bt_mesh_cfg_mod_app_unbind_vnd(0, 0, 0, 0, 0, CID_NVAL, &status);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_APP_VND_UNBIND_3");
}

static void CASE_blesdk_mesh_net_key_update()
{
    u8_t net_key[16] = {0};
    u8_t status      = 0x00;
    int  ret;

    // send_seg
    ret = bt_mesh_cfg_net_key_add(0, 0, 0, net_key, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_NET_KEY_ADD_1");

    ret = bt_mesh_cfg_net_key_add(0, 0, 0, net_key, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_NET_KEY_ADD_2");

    ret = bt_mesh_cfg_net_key_update(0, 0, 0, net_key, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_NET_KEY_UPDATE_1");

    ret = bt_mesh_cfg_net_key_update(0, 0, 0, net_key, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_NET_KEY_UPDATE_2");
}

static void CASE_blesdk_mesh_app_key_update()
{
    u8_t app_key[16] = {0};
    u8_t status      = 0x00;
    int  ret;

    ret = bt_mesh_cfg_app_key_add(0, 0, 0, 0, app_key, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_APP_KEY_ADD_1");

    ret = bt_mesh_cfg_app_key_add(0, 0, 0, 0, app_key, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_APP_KEY_ADD_2");

    ret = bt_mesh_cfg_app_key_update(0, 0, 0, 0, app_key, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_APP_KEY_UPDATE_1");

    ret = bt_mesh_cfg_app_key_update(0, 0, 0, 0, app_key, &status);
    YUNIT_ASSERT_MSG_QA(ret == 402, "ret = %d", ret, "YOC_MESH_APP_KEY_UPDATE_2");
}

static void CASE_blesdk_mesh_node_reset()
{
    int ret;

    //cli_wait
    ret = bt_mesh_cfg_node_reset(0, 0);
    YUNIT_ASSERT_MSG_QA(ret == RHINO_BLK_TIMEOUT, "ret = %d", ret, "YOC_MESH_NODE_RESET_1");
}

static void CASE_blesdk_mesh_health_cli()
{
    int ret;

    ret = bt_mesh_health_cli_set(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_CLI_1");

    ret = bt_mesh_health_cli_set(&root_models[4]);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_CLI_2");

    ret = bt_mesh_health_cli_set(&root_models[0]);
    YUNIT_ASSERT_MSG_QA(ret == 0, "ret = %d", ret, "YOC_MESH_HEALTH_CLI_3");
}

static void CASE_blesdk_mesh_health_cli_timeout()
{
    s32_t ret;

    ret = bt_mesh_health_cli_timeout_get();
    YUNIT_ASSERT_MSG_QA(ret == 2000, "ret = %d", ret, "YOC_MESH_HEALTH_TIMEOUT_GET_1");

    bt_mesh_health_cli_timeout_set(1000);
    YUNIT_ASSERT_MSG_QA(ret == 2000, "ret = %d", ret, "YOC_MESH_HEALTH_TIMEOUT_SET_1");

    ret = bt_mesh_health_cli_timeout_get();
    YUNIT_ASSERT_MSG_QA(ret == 1000, "ret = %d", ret, "YOC_MESH_HEALTH_TIMEOUT_GET_2");
}

static void CASE_blesdk_mesh_health_fault()
{
    u8_t   test_id, faults = 0x00;
    size_t fault_count = 0;
    int    ret;

    ret = bt_mesh_health_fault_get(0, 0, 0, 0, NULL, &faults, &fault_count);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_GET_1");

    ret = bt_mesh_health_fault_get(0, 0, 0, 0, &test_id, NULL, &fault_count);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_GET_2");

    ret = bt_mesh_health_fault_get(0, 0, 0, 0, &test_id, &faults, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_GET_3");

    ret = bt_mesh_health_fault_get(0, 0, 0, 0, &test_id, &faults, &fault_count);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_GET_4");

    ret = bt_mesh_health_fault_test(0, 0, 0, 0, test_id, &faults, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_TEST_1");

    ret = bt_mesh_health_fault_test(0, 0, 0, 0, test_id, NULL, &fault_count);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_TEST_2");

    ret = bt_mesh_health_fault_test(0, 0, 0, 0, test_id, &faults, &fault_count);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_TEST_3");

    ret = bt_mesh_health_fault_clear(0, 0, 0, 0, NULL, &faults, &fault_count);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_CLR_1");

    ret = bt_mesh_health_fault_clear(0, 0, 0, 0, &test_id, NULL, &fault_count);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_CLR_2");

    ret = bt_mesh_health_fault_clear(0, 0, 0, 0, &test_id, &faults, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_CLR_3");

    ret = bt_mesh_health_fault_clear(0, 0, 0, 0, &test_id, &faults, &fault_count);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_FAULT_CLR_4");
}

static void CASE_blesdk_mesh_health_period()
{
    u8_t divisor, updated_divisor = 0;
    int  ret;

    ret = bt_mesh_health_period_set(0, 0, 0, 16, &updated_divisor);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_PERIOD_SET_1");

    ret = bt_mesh_health_period_set(0, 0, 0, 1, &updated_divisor);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_PERIOD_SET_2");

    ret = bt_mesh_health_period_set(0, 0, 0, 1, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_PERIOD_SET_3");

    ret = bt_mesh_health_period_get(0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_PERIOD_GET_1");

    ret = bt_mesh_health_period_get(0, 0, 0, &divisor);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_PERIOD_GET_2");
}

static void CASE_blesdk_mesh_health_attention()
{
    u8_t attention, updated_attention = 0;
    int  ret;

    ret = bt_mesh_health_attention_set(0, 0, 0, attention, &updated_attention);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_ATTENTION_SET_1");

    ret = bt_mesh_health_attention_set(0, 0, 0, attention, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_ATTENTION_SET_2");

    ret = bt_mesh_health_attention_get(0, 0, 0, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_ATTENTION_GET_1");

    ret = bt_mesh_health_attention_get(0, 0, 0, &attention);
    YUNIT_ASSERT_MSG_QA(ret == -EINVAL, "ret = %d", ret, "YOC_MESH_HEALTH_ATTENTION_GET_2");
}

/* BLE MESH NODE API */
static void CASE_ble_mesh_node_init()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    node_config_t config = {0};

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_INIT, i++);
    ret = ble_mesh_node_init(NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_INIT, i++);
    ret = ble_mesh_node_init(&config);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    config.role = PROVISIONER;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_INIT, i++);
    ret = ble_mesh_node_init(&config);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    config.role = NODE;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_INIT, i++);
    ret = ble_mesh_node_init(&config);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_node_oob_input_num()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_OOB_INPUT_NUM, i++);
    ret = ble_mesh_node_OOB_input_num(32);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_OOB_INPUT_NUM, i++);
    ret = ble_mesh_node_OOB_input_num(32);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_node_oob_input_str()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};
    char str[]          = "0123456";

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_OOB_INPUT_STR, i++);
    ret = ble_mesh_node_OOB_input_string(NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_OOB_INPUT_STR, i++);
    ret = ble_mesh_node_OOB_input_string(str);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_NODE_OOB_INPUT_STR, i++);
    ret = ble_mesh_node_OOB_input_string(str);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);
}

/* BLE MESH MODEL API */
static void CASE_ble_mesh_model_init()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    static struct bt_mesh_model elem0_root_models[] = {
        MESH_MODEL_CFG_SRV_NULL(),       MESH_MODEL_CFG_CLI_NULL(),
        MESH_MODEL_HEALTH_SRV_NULL(),    MESH_MODEL_HEALTH_CLI_NULL(),
        MESH_MODEL_GEN_ONOFF_CLI_NULL(), MESH_MODEL_GEN_LEVEL_CLI_NULL(),
        MESH_MODEL_LIGHTNESS_CLI_NULL(), MESH_MODEL_LIGHT_CTL_CLI_NULL(),
    };

    static struct bt_mesh_model elem0_vnd_models[] = {
        MESH_MODEL_VENDOR_CLI_NULL(),
    };

    static struct bt_mesh_elem elements[] = {
        BT_MESH_ELEM(0, elem0_root_models, elem0_vnd_models, 0),
    };

    /*comp*/
    static struct bt_mesh_comp mesh_comp = {
        .cid        = 0x01A8,
        .elem       = elements,
        .elem_count = ARRAY_SIZE(elements),
    };

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_MODEL_INIT, i++);
    ret = ble_mesh_model_init(NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_MODEL_INIT, i++);
    ret = ble_mesh_model_init(&mesh_comp);
    YUNIT_ASSERT_MSG_QA(ret == (0), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_model_get_comp_data()
{
    int  i              = 0;
    char result_str[30] = {0};

    const struct bt_mesh_comp *mesh_comp = NULL;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_MODEL_GET_COMP_DATA, i++);
    mesh_comp = ble_mesh_model_get_comp_data();
    YUNIT_ASSERT_MSG_QA(mesh_comp != (NULL), "ret = %s", (mesh_comp == NULL) ? "NULL" : "!NULL",
                        result_str);
}

static void node_model_message_cb(mesh_model_event_en event, void *p_arg)
{
    printf("node_model_message_cb called, with event %x\n", event);
}

static void CASE_ble_mesh_model_set_cb()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_MODEL_SET_CB, i++);
    ret = ble_mesh_model_set_cb(NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_MODEL_SET_CB, i++);
    ret = ble_mesh_model_set_cb(node_model_message_cb);
    YUNIT_ASSERT_MSG_QA(ret == (0), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_model_status_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    uint16_t             op_code      = 0x8001;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_MODEL_STATUS_GET, i++);
    ret = ble_mesh_model_status_get(netkey_idx, appkey_idx, unicast_addr, NULL, op_code);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_MODEL_STATUS_GET, i++);
    ret = ble_mesh_model_status_get(netkey_idx, appkey_idx, unicast_addr, &model, op_code);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_MODEL_STATUS_GET, i++);
    ret = ble_mesh_model_status_get(netkey_idx, appkey_idx, unicast_addr, &model, op_code);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_generic_onoff_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_onoff_arg        set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_ONOFF_SET, i++);
    ret = ble_mesh_generic_onoff_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_ONOFF_SET, i++);
    ret = ble_mesh_generic_onoff_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_ONOFF_SET, i++);
    ret = ble_mesh_generic_onoff_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_ONOFF_SET, i++);
    ret = ble_mesh_generic_onoff_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_generic_onoff_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_ONOFF_GET, i++);
    ret = ble_mesh_generic_onoff_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_ONOFF_GET, i++);
    ret = ble_mesh_generic_onoff_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_ONOFF_GET, i++);
    ret = ble_mesh_generic_onoff_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_generic_move_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_level_arg        set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_MOVE_SET, i++);
    ret =
        ble_mesh_generic_level_move_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_MOVE_SET, i++);
    ret = ble_mesh_generic_level_move_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_MOVE_SET, i++);
    ret = ble_mesh_generic_level_move_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                          ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_MOVE_SET, i++);
    ret = ble_mesh_generic_level_move_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                          ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_generic_delta_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_level_arg        set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_DELTA_SET, i++);
    ret =
        ble_mesh_generic_level_delta_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_DELTA_SET, i++);
    ret = ble_mesh_generic_level_delta_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_DELTA_SET, i++);
    ret = ble_mesh_generic_level_delta_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                           ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_DELTA_SET, i++);
    ret = ble_mesh_generic_level_delta_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                           ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_generic_level_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_level_arg        set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_LEVEL_SET, i++);
    ret = ble_mesh_generic_level_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_LEVEL_SET, i++);
    ret = ble_mesh_generic_level_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_LEVEL_SET, i++);
    ret = ble_mesh_generic_level_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_LEVEL_SET, i++);
    ret = ble_mesh_generic_level_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_generic_level_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_LEVEL_GET, i++);
    ret = ble_mesh_generic_level_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_LEVEL_GET, i++);
    ret = ble_mesh_generic_level_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_GENERIC_LEVEL_GET, i++);
    ret = ble_mesh_generic_level_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_lightness_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_GET, i++);
    ret = ble_mesh_light_lightness_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_GET, i++);
    ret = ble_mesh_light_lightness_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_GET, i++);
    ret = ble_mesh_light_lightness_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_lightness_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_lightness_arg    set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_SET, i++);
    ret = ble_mesh_light_lightness_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_SET, i++);
    ret = ble_mesh_light_lightness_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_SET, i++);
    ret = ble_mesh_light_lightness_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_SET, i++);
    ret = ble_mesh_light_lightness_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_lightness_linear_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LINEAR_GET, i++);
    ret = ble_mesh_light_lightness_linear_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LINEAR_GET, i++);
    ret = ble_mesh_light_lightness_linear_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LINEAR_GET, i++);
    ret = ble_mesh_light_lightness_linear_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_lightness_linear_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_lightness_arg    set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LINEAR_SET, i++);
    ret = ble_mesh_light_lightness_linear_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg,
                                              ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LINEAR_SET, i++);
    ret = ble_mesh_light_lightness_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LINEAR_SET, i++);
    ret = ble_mesh_light_lightness_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LINEAR_SET, i++);
    ret = ble_mesh_light_lightness_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_lightness_last_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LAST_GET, i++);
    ret = ble_mesh_light_lightness_last_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LAST_GET, i++);
    ret = ble_mesh_light_lightness_last_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_LAST_GET, i++);
    ret = ble_mesh_light_lightness_last_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_lightness_def_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_DEF_GET, i++);
    ret = ble_mesh_light_lightness_def_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_DEF_GET, i++);
    ret = ble_mesh_light_lightness_def_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_DEF_GET, i++);
    ret = ble_mesh_light_lightness_def_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_def_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_lightness_arg    set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_DEF_SET, i++);
    ret =
        ble_mesh_light_lightness_def_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_DEF_SET, i++);
    ret = ble_mesh_light_lightness_def_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_DEF_SET, i++);
    ret = ble_mesh_light_lightness_def_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                           ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_DEF_SET, i++);
    ret = ble_mesh_light_lightness_def_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                           ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_lightness_range_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_RANGE_GET, i++);
    ret = ble_mesh_light_lightness_range_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_RANGE_GET, i++);
    ret = ble_mesh_light_lightness_range_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_RANGE_GET, i++);
    ret = ble_mesh_light_lightness_range_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_lightness_range_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_lightness_arg    set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_RANGE_SET, i++);
    ret = ble_mesh_light_lightness_range_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg,
                                             ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_RANGE_SET, i++);
    ret =
        ble_mesh_light_lightness_range_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_RANGE_SET, i++);
    ret = ble_mesh_light_lightness_range_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                             ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_LIGHTNESS_RANGE_SET, i++);
    ret = ble_mesh_light_lightness_range_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                             ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_ctl_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_GET, i++);
    ret = ble_mesh_light_ctl_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_GET, i++);
    ret = ble_mesh_light_ctl_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_GET, i++);
    ret = ble_mesh_light_ctl_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_ctl_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_light_ctl_arg    set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_SET, i++);
    ret = ble_mesh_light_ctl_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_SET, i++);
    ret = ble_mesh_light_ctl_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_SET, i++);
    ret = ble_mesh_light_ctl_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_SET, i++);
    ret = ble_mesh_light_ctl_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_ctl_temp_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_light_ctl_arg    set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_SET, i++);
    ret = ble_mesh_light_ctl_temp_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_SET, i++);
    ret = ble_mesh_light_ctl_temp_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_SET, i++);
    ret = ble_mesh_light_ctl_temp_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_SET, i++);
    ret = ble_mesh_light_ctl_temp_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_ctl_temp_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_GET, i++);
    ret = ble_mesh_light_ctl_temp_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_GET, i++);
    ret = ble_mesh_light_ctl_temp_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_GET, i++);
    ret = ble_mesh_light_ctl_temp_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_ctl_def_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_light_ctl_arg    set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_DEF_SET, i++);
    ret = ble_mesh_light_ctl_def_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_DEF_SET, i++);
    ret = ble_mesh_light_ctl_def_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_DEF_SET, i++);
    ret = ble_mesh_light_ctl_def_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_DEF_SET, i++);
    ret = ble_mesh_light_ctl_def_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_ctl_def_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_DEF_GET, i++);
    ret = ble_mesh_light_ctl_def_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_DEF_GET, i++);
    ret = ble_mesh_light_ctl_def_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_DEF_GET, i++);
    ret = ble_mesh_light_ctl_def_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_ctl_temp_range_set()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;
    set_light_ctl_arg    set_arg      = {};
    bool                 ack          = false;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_RANGE_SET, i++);
    ret = ble_mesh_light_ctl_temp_range_set(netkey_idx, appkey_idx, unicast_addr, NULL, &set_arg,
                                            ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_RANGE_SET, i++);
    ret =
        ble_mesh_light_ctl_temp_range_set(netkey_idx, appkey_idx, unicast_addr, &model, NULL, ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_RANGE_SET, i++);
    ret = ble_mesh_light_ctl_temp_range_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                            ack);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_RANGE_SET, i++);
    ret = ble_mesh_light_ctl_temp_range_set(netkey_idx, appkey_idx, unicast_addr, &model, &set_arg,
                                            ack);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_light_ctl_temp_range_get()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             netkey_idx   = 0;
    uint16_t             appkey_idx   = 0;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_RANGE_GET, i++);
    ret = ble_mesh_light_ctl_temp_range_get(netkey_idx, appkey_idx, unicast_addr, NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-EINVAL), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_RANGE_GET, i++);
    ret = ble_mesh_light_ctl_temp_range_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    unicast_addr = 0x0000;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_LIGHT_CTL_TEMP_RANGE_GET, i++);
    ret = ble_mesh_light_ctl_temp_range_get(netkey_idx, appkey_idx, unicast_addr, &model);
    YUNIT_ASSERT_MSG_QA(ret == (-EADDRNOTAVAIL), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_vendor_cli_model_msg_send()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             appkey_idx   = 0;
    vnd_model_msg        msg          = {};
    msg.netkey_idx                    = 0;
    msg.appkey_idx                    = appkey_idx;
    msg.dst_addr                      = unicast_addr;

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_VENDOR_CLI_MODEL_MSG_SEND, i++);
    ret = ble_mesh_vendor_cli_model_msg_send(&msg);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    msg.model = &model;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_VENDOR_CLI_MODEL_MSG_SEND, i++);
    ret = ble_mesh_vendor_cli_model_msg_send(&msg);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_VENDOR_CLI_MODEL_MSG_SEND, i++);
    ret = ble_mesh_vendor_cli_model_msg_send(NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);
}

static void CASE_ble_mesh_vendor_srv_model_msg_send()
{
    int  ret;
    int  i              = 0;
    char result_str[30] = {0};

    struct bt_mesh_model model        = {};
    uint16_t             unicast_addr = 0x0001;
    uint16_t             appkey_idx   = 0;
    vnd_model_msg        msg          = {};
    msg.netkey_idx                    = 0;
    msg.appkey_idx                    = appkey_idx;
    msg.dst_addr                      = unicast_addr;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_VENDOR_SRV_MODEL_MSG_SEND, i++);
    ret = ble_mesh_vendor_srv_model_msg_send(&msg);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);

    msg.model = &model;
    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_VENDOR_CLI_MODEL_MSG_SEND, i++);
    ret = ble_mesh_vendor_cli_model_msg_send(&msg);
    YUNIT_ASSERT_MSG_QA(ret == (-22), "ret = %d", ret, result_str);

    memset(result_str, 0, sizeof(result_str));
    sprintf(result_str, "%s%d_%d", BLEMESH_TEST_CASE_STR, BLEMESH_VENDOR_SRV_MODEL_MSG_SEND, i++);
    ret = ble_mesh_vendor_srv_model_msg_send(NULL);
    YUNIT_ASSERT_MSG_QA(ret == (-1), "ret = %d", ret, result_str);
}

void ble_sdk_mesh_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "ble.sdk.mesh.access.before_init", CASE_blesdk_mesh_before_init);
    yunit_add_test_case(suite, "ble.sdk.mesh.access.init", CASE_blesdk_mesh_init);
    yunit_add_test_case(suite, "ble.sdk.mesh.access.provision", CASE_blesdk_mesh_provision);
    yunit_add_test_case(suite, "ble.sdk.mesh.access.input", CASE_blesdk_mesh_input);
    yunit_add_test_case(suite, "ble.sdk.mesh.access.model_send", CASE_blesdk_mesh_model_send);

    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.timeout", CASE_blesdk_mesh_timeout);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.comp_get", CASE_blesdk_mesh_comp_get);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.beacon", CASE_blesdk_mesh_beacon);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.ttl", CASE_blesdk_mesh_ttl);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.friend", CASE_blesdk_mesh_friend);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.proxy", CASE_blesdk_mesh_proxy);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.relay", CASE_blesdk_mesh_relay);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.pub", CASE_blesdk_mesh_pub);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.hb_pub", CASE_blesdk_mesh_hb_pub);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.sub", CASE_blesdk_mesh_sub);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.hb_sub", CASE_blesdk_mesh_hb_sub);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.krp", CASE_blesdk_mesh_krp);

    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.sub_add_del", CASE_blesdk_mesh_sub_add_del);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.sub_vnd_add_del",
                        CASE_blesdk_mesh_sub_vnd_add_del);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.sub_va_add_del", CASE_blesdk_mesh_sub_va_add_del);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.sub_va_vnd_add_del",
                        CASE_blesdk_mesh_sub_va_vnd_add_del);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.app_bind", CASE_blesdk_mesh_app_bind);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.app_unbind", CASE_blesdk_mesh_app_unbind);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.overwrite", CASE_blesdk_mesh_sub_overwrite);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.va_overwrite", CASE_blesdk_mesh_sub_va_overwrite);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.net_key_update", CASE_blesdk_mesh_net_key_update);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.app_key_update", CASE_blesdk_mesh_app_key_update);
    yunit_add_test_case(suite, "ble.sdk.mesh.cfg.node_reset", CASE_blesdk_mesh_node_reset);

    yunit_add_test_case(suite, "ble.sdk.mesh.health.cli", CASE_blesdk_mesh_health_cli);
    yunit_add_test_case(suite, "ble.sdk.mesh.health.cli_timeout",
                        CASE_blesdk_mesh_health_cli_timeout);
    yunit_add_test_case(suite, "ble.sdk.mesh.health.fault", CASE_blesdk_mesh_health_fault);
    yunit_add_test_case(suite, "ble.sdk.mesh.health.period", CASE_blesdk_mesh_health_period);
    yunit_add_test_case(suite, "ble.sdk.mesh.health.attention", CASE_blesdk_mesh_health_attention);

    yunit_add_test_case(suite, "blemesh.ble_mesh_node_init", CASE_ble_mesh_node_init);
    yunit_add_test_case(suite, "blemesh.ble_mesh_node_OOB_input_num",
                        CASE_ble_mesh_node_oob_input_num);
    yunit_add_test_case(suite, "blemesh.ble_mesh_node_OOB_input_string",
                        CASE_ble_mesh_node_oob_input_str);

    yunit_add_test_case(suite, "blemesh.ble_mesh_model_init", CASE_ble_mesh_model_init);
    yunit_add_test_case(suite, "blemesh.ble_mesh_model_get_comp_data",
                        CASE_ble_mesh_model_get_comp_data);
    yunit_add_test_case(suite, "blemesh.ble_mesh_model_set_cb", CASE_ble_mesh_model_set_cb);
    yunit_add_test_case(suite, "blemesh.ble_mesh_model_status_get", CASE_ble_mesh_model_status_get);

    yunit_add_test_case(suite, "blemesh.ble_mesh_generic_onoff_get",
                        CASE_ble_mesh_generic_onoff_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_generic_onoff_set",
                        CASE_ble_mesh_generic_onoff_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_generic_move_set", CASE_ble_mesh_generic_move_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_generic_delta_set",
                        CASE_ble_mesh_generic_delta_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_generic_level_set",
                        CASE_ble_mesh_generic_level_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_generic_level_get",
                        CASE_ble_mesh_generic_level_get);

    yunit_add_test_case(suite, "blemesh.ble_mesh_light_lightness_get",
                        CASE_ble_mesh_light_lightness_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_lightness_set",
                        CASE_ble_mesh_light_lightness_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_lightness_linear_get",
                        CASE_ble_mesh_light_lightness_linear_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_lightness_linear_set",
                        CASE_ble_mesh_light_lightness_linear_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_lightness_last_get",
                        CASE_ble_mesh_light_lightness_last_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_lightness_def_get",
                        CASE_ble_mesh_light_lightness_def_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_def_set", CASE_ble_mesh_light_def_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_lightness_range_get",
                        CASE_ble_mesh_light_lightness_range_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_lightness_range_set",
                        CASE_ble_mesh_light_lightness_range_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_ctl_set", CASE_ble_mesh_light_ctl_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_ctl_get", CASE_ble_mesh_light_ctl_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_ctl_temp_set",
                        CASE_ble_mesh_light_ctl_temp_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_ctl_temp_get",
                        CASE_ble_mesh_light_ctl_temp_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_ctl_def_set",
                        CASE_ble_mesh_light_ctl_def_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_ctl_def_get",
                        CASE_ble_mesh_light_ctl_def_get);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_ctl_temp_range_set",
                        CASE_ble_mesh_light_ctl_temp_range_set);
    yunit_add_test_case(suite, "blemesh.ble_mesh_light_ctl_temp_range_get",
                        CASE_ble_mesh_light_ctl_temp_range_get);

    yunit_add_test_case(suite, "blemesh.ble_mesh_vendor_cli_model_msg_send",
                        CASE_ble_mesh_vendor_cli_model_msg_send);
    yunit_add_test_case(suite, "blemesh.ble_mesh_vendor_srv_model_msg_send",
                        CASE_ble_mesh_vendor_srv_model_msg_send);
}
