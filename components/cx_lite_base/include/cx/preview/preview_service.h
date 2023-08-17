/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef PREVIEW_SERVICE_H
#define PREVIEW_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void * cx_preview_hdl_t;
typedef enum {
    PREVIEW_MODE_UNKNOWN = 0,
    PREVIEW_MODE_RGB,
    PREVIEW_MODE_IR,
} preview_mode_t;

cx_preview_hdl_t cx_preview_start(const char *preview_name, preview_mode_t mode);
int cx_preview_stop(cx_preview_hdl_t hdl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif