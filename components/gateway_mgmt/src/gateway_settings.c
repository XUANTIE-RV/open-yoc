/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <aos/kv.h>
#include "ulog/ulog.h"
#include "gateway_mgmt.h"

#define TAG "GW_SETTINGS"

static inline int16_t _gateway_subdev_get_index(gw_subdev_t subdev)
{
    return (int16_t)subdev;
}

static inline int16_t _gateway_subgrp_get_index(gw_subgrp_t subgrp)
{
    return (int16_t)subgrp;
}

int gateway_subdev_ctx_store(gw_subdev_ctx_t *subdev_ctx)
{
    char key_string[14] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "subdev-%05d", _gateway_subdev_get_index(subdev_ctx->subdev));

    ret = aos_kv_set(key_string, subdev_ctx, sizeof(gw_subdev_ctx_t), 1);
    if (ret != 0) {
        LOGE(TAG, "save subdev[%d] info fail", _gateway_subdev_get_index(subdev_ctx->subdev));
    }

    return ret;
}

int gateway_subdev_ctx_load(gw_subdev_ctx_t *subdev_ctx)
{
    char key_string[14] = { 0 };
    int  buffer_len     = sizeof(gw_subdev_ctx_t);
    int  ret            = 0;

    sprintf(key_string, "subdev-%05d", _gateway_subdev_get_index(subdev_ctx->subdev));

    ret = aos_kv_get(key_string, subdev_ctx, &buffer_len);
    if (ret != 0) {
        // LOGD(TAG, "load subdev[%d] info fail", _gateway_subdev_get_index(subdev_ctx->subdev));
    }
    return ret;
}

int gateway_subdev_ctx_delete(gw_subdev_ctx_t *subdev_ctx)
{
    char key_string[14] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "subdev-%05d", _gateway_subdev_get_index(subdev_ctx->subdev));

    ret = aos_kv_del(key_string);
    if (ret != 0) {
        LOGE(TAG, "del subdev[%d] info fail", _gateway_subdev_get_index(subdev_ctx->subdev));
    }
    memset(subdev_ctx, 0, sizeof(gw_subdev_ctx_t));
    subdev_ctx->state = GW_NODE_INVAL;
    return ret;
}

int gateway_subdev_name_store(gw_subdev_ctx_t *subdev_ctx)
{
    char key_string[16] = { 0 };
    int  ret            = 0;

    if (!subdev_ctx->name || !strlen(subdev_ctx->name)) {
        LOGE(TAG, "Invalid name len ");
        return -EINVAL;
    }

    sprintf(key_string, "dev-name-%03d", _gateway_subdev_get_index(subdev_ctx->subdev));

    ret = aos_kv_set(key_string, (void *)subdev_ctx->name, strlen(subdev_ctx->name), 1);
    if (ret != 0) {
        LOGE(TAG, "save subdev[%d] name fail", _gateway_subdev_get_index(subdev_ctx->subdev));
    }

    return ret;
}

int gateway_subdev_name_load(gw_subdev_ctx_t *subdev_ctx)
{
    char key_string[16] = { 0 };
    int  ret            = 0;
    int  buffer_len     = GW_DEV_NAME_LEN_MAX;

    sprintf(key_string, "dev-name-%03d", _gateway_subdev_get_index(subdev_ctx->subdev));

    ret = aos_kv_get(key_string, (void *)subdev_ctx->name, &buffer_len);
    if (ret != 0) {
        LOGD(TAG, "subdev[%d] has no name", _gateway_subdev_get_index(subdev_ctx->subdev));
    }

    return ret;
}

int gateway_subdev_name_delete(gw_subdev_ctx_t *subdev_ctx)
{
    char key_string[16] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "dev-name-%03d", _gateway_subdev_get_index(subdev_ctx->subdev));

    ret = aos_kv_del(key_string);
    if (ret != 0) {
        LOGE(TAG, "del subdev[%d] name fail", _gateway_subdev_get_index(subdev_ctx->subdev));
    }

    return ret;
}

int gateway_subgrp_ctx_store(gw_subgrp_ctx_t *subgrp_ctx)
{
    char key_string[14] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "subgrp-%05d", _gateway_subgrp_get_index(subgrp_ctx->subgrp));

    ret = aos_kv_set(key_string, subgrp_ctx, sizeof(gw_subgrp_ctx_t), 1);
    if (ret != 0) {
        LOGE(TAG, "save subgrp[%d] info fail", _gateway_subgrp_get_index(subgrp_ctx->subgrp));
    }

    return ret;
}

int gateway_subgrp_ctx_load(gw_subgrp_ctx_t *subgrp_ctx)
{
    char key_string[14] = { 0 };
    int  ret            = 0;
    int  buffer_len     = sizeof(gw_subdev_ctx_t);

    sprintf(key_string, "subgrp-%05d", _gateway_subgrp_get_index(subgrp_ctx->subgrp));

    ret = aos_kv_get(key_string, subgrp_ctx, &buffer_len);
    if (ret != 0) {
        LOGE(TAG, "save subgrp[%d] info fail", _gateway_subgrp_get_index(subgrp_ctx->subgrp));
    }

    return ret;
}

int gateway_subgrp_ctx_delete(gw_subgrp_ctx_t *subgrp_ctx)
{
    char key_string[14] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "subgrp-%05d", _gateway_subgrp_get_index(subgrp_ctx->subgrp));

    ret = aos_kv_del(key_string);
    if (ret != 0) {
        LOGE(TAG, "del subgrp[%d] info fail", _gateway_subgrp_get_index(subgrp_ctx->subgrp));
    }

    return ret;
}

int gateway_subgrp_name_store(gw_subgrp_ctx_t *subgrp_ctx)
{
    char key_string[16] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "grp-name-%03d", _gateway_subgrp_get_index(subgrp_ctx->subgrp));

    ret = aos_kv_set(key_string, (void *)subgrp_ctx->name, strlen(subgrp_ctx->name), 1);
    if (ret != 0) {
        LOGE(TAG, "save subgrp[%d] name fail", _gateway_subgrp_get_index(subgrp_ctx->subgrp));
    }

    return ret;
}

int gateway_subgrp_name_load(gw_subgrp_ctx_t *subgrp_ctx)
{
    char key_string[16] = { 0 };
    int  ret            = 0;
    int  buffer_len     = GW_DEV_NAME_LEN_MAX;

    sprintf(key_string, "grp-name-%03d", _gateway_subgrp_get_index(subgrp_ctx->subgrp));

    ret = aos_kv_get(key_string, (void *)subgrp_ctx->name, &buffer_len);
    if (ret != 0) {
        LOGE(TAG, "save subgrp[%d] name fail", _gateway_subgrp_get_index(subgrp_ctx->subgrp));
    }

    return ret;
}

int gateway_subgrp_name_delete(gw_subgrp_ctx_t *subgrp_ctx)
{
    char key_string[16] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "grp-name-%03d", _gateway_subgrp_get_index(subgrp_ctx->subgrp));

    ret = aos_kv_del(key_string);
    if (ret != 0) {
        LOGE(TAG, "del subgrp[%d] name fail", _gateway_subgrp_get_index(subgrp_ctx->subgrp));
    }
    return ret;
}
