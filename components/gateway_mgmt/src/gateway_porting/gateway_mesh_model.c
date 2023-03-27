/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
 
#if defined(CONFIG_TSL_DEVICER_MESH) && CONFIG_TSL_DEVICER_MESH
#include <tsl_engine/device_mesh.h>
#include <tsl_engine/cloud_device_conv.h>
#include <api/mesh.h>
#include <errno.h>
#include <aos/ble.h>
#include <mesh_model/mesh_model.h>
#include <mesh_node.h>
#include <mesh_provisioner.h>
#include <provisioner_main.h>
#include <smartliving/iot_import.h>
#include <smartliving/exports/iot_export_linkkit.h>
#include "gateway_mesh.h"
#include "gateway_mgmt.h"
#include "gateway_mesh_model.h"

#define TAG "dev_mesh"

gateway_model_send_data_t g_gateway_model_send_data = NULL;

extern aos_sem_t js_sem;

static int _dev_mesh_open(device_t *dev)
{
    // TODO:
    return 0;
}

static void _dev_mesh_close(device_t *dev)
{
    // TODO:
}

static int _dev_mesh_send_to_device(device_t *dev, const uint8_t *data, size_t size)
{
    int                    rc  = -1;
    char *                 buf = NULL;
    gw_subdev_ctx_t *      subdev_ctx;
    int16_t                subdev_idx;
    uint8_t *              mac    = (uint8_t *)dev->mac;
    size_t                 amount = size + 4; // 4 is for TransMIC
    struct net_buf_simple  msg    = { 0 };
    struct bt_mesh_msg_ctx ctx    = { 0 };
    struct bt_mesh_model * model  = NULL;

    buf = aos_zalloc(amount);
    if (!buf) {
        LOGE(TAG, "may be oom, size = %u", size);
        goto err;
    }
    memcpy(buf, data, size);
    msg.data  = (u8_t *)buf;
    msg.len   = size;
    msg.size  = amount;
    msg.__buf = (u8_t *)buf;

    /* restore sundev idx from mac address */
    subdev_idx = mac[0] | (mac[1] << 8);
    subdev_ctx = gateway_subdev_get_ctx((gw_subdev_t)subdev_idx);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev %d is not exist", subdev_idx);
        goto err;
    }

    ctx.net_idx  = 0;
    ctx.app_idx  = 0;
    ctx.addr     = subdev_ctx->info.ble_mesh_info.unicast_addr;
    ctx.send_ttl = BT_MESH_TTL_DEFAULT;

    model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, CID_NVAL);
    if (!model) {
        LOGE(TAG, "No onoff model found");
        goto err;
    }

    rc = bt_mesh_model_send(model, &ctx, &msg, NULL, NULL);
    if (rc) {
        LOGE(TAG, "model send fail %d", rc);
        rc = -1;
        goto err;
    }
    LOGD(TAG, "%s, data = %p, size = %d", __FUNCTION__, data, size);

err:
    aos_free(buf);
    return rc;
}

static int _dev_mesh_send_to_cloud(device_t *dev, const uint8_t *data, size_t size)
{
    gw_subdev_ctx_t *subdev_ctx;
    int16_t          subdev_idx;
    uint8_t *        mac = (uint8_t *)dev->mac;

    /* restore sundev idx from mac address */
    subdev_idx = mac[0] | (mac[1] << 8);
    subdev_ctx = gateway_subdev_get_ctx((gw_subdev_t)subdev_idx);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev %d is not exist", subdev_idx);
        return -ENODEV;
    }

    return gateway_mgmt_modem_status(subdev_ctx, (uint8_t *)data, size);
}

static int _dev_mesh_ioctl(device_t *dev, int cmd, void *arg, size_t *arg_size)
{
    int rc = -1;

    return rc;
}

static struct device_ops _dev_ops_mesh = {
    .open           = _dev_mesh_open,
    .close          = _dev_mesh_close,
    .ioctl          = _dev_mesh_ioctl,
    .send_to_device = _dev_mesh_send_to_device,
    .send_to_cloud  = _dev_mesh_send_to_cloud,
};

const device_mesh_t g_dev_cls_mesh = {
    .type = DEV_TYPE_MESH,
    .name = "mesh",
    .size = sizeof(device_mesh_t),
    .ops  = &_dev_ops_mesh,
};

/**
 * @brief  regist device for mesh
 * @return 0/-1
 */
int device_register_mesh()
{
    return dev_cls_register((const device_t *)&g_dev_cls_mesh);
}

void gateway_model_conv_init(void)
{
    int ret = 0;
    ret     = device_register_mesh();
    if (ret) {
        LOGE(TAG, "Device register mesh failed %d", ret);
        return;
    }
#if defined(CONFIG_SAVE_JS_TO_RAM) && (CONFIG_SAVE_JS_TO_RAM == 1)
    init_js_to_ram();
    aos_sem_new(&js_sem, 1);
#endif
}

#endif
