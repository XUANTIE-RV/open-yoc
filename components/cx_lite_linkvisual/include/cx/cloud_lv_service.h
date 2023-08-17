/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CLOUD_LV_SERVICE_H
#define CLOUD_LV_SERVICE_H

typedef enum {
    CXLV_EVENT_CONNECT,         /* connection event */
    CXLV_EVENT_DISCONNECT,      /* disconnection event */
    CXLV_EVENT_LIVE_START,      /* live start event */
    CXLV_EVENT_LIVE_STOP,       /* live stop event */
    CXLV_EVENT_ITERCOMSTART,    /* inter-communication start event */
    CXLV_EVENT_ITERCOMSTOP,     /* inter-communication stop event */
} cx_lv_event_e;

typedef enum {
    CX_MEIDA_NONE = 0,
    CX_MEIDA_H264,
    CX_MEIDA_H265,
    CX_MEIDA_BMP,
    CX_MEIDA_JPEG,
    CX_MEIDA_PNG,
}cx_media_type_e;

typedef enum {
    CX_LV_MOTION_DETECTED_EVENT, //移动侦测
} cx_lv_alarm_event_type_e;

typedef struct {
    cx_lv_alarm_event_type_e event_type;
    cx_media_type_e media_format;
    struct {
        char *p;
        unsigned int len;
    } media;
    char *event_description;
} cx_lv_alarm_param_t;

/* link visual service handler */
typedef void * cxlv_hdl_t;

/* link visual callback type */
typedef void (*cx_lv_callback_cb)(cx_lv_event_e event, void *data);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @description: start link visual service
 * @param [in] lv_name:     lv name, in the form "linkvisual#0"
 * @param [in] cb:          service callback
 * @return                  NULL: error, others: recorder handler.
 */
cxlv_hdl_t cx_lv_start(const char *lv_name, cx_lv_callback_cb cb);

/**
 * @description: stop link visual service
 * @param [in] hdl:         link visual service handler
 * @return                  0: Success, < 0: Error code.
 */
int cx_lv_stop(cxlv_hdl_t hdl);

/**
 * @description: connect to the cloud
 * @param [in] hdl:         link visual service handler
 * @return                  0: Success, < 0: Error code.
 */
int cx_lv_connect(cxlv_hdl_t hdl);

/**
 * @description: disconnect from the cloud
 * @param [in] hdl:         link visual service handler
 * @return                  0: Success, < 0: Error code.
 */
int cx_lv_disconnect(cxlv_hdl_t hdl);

/**
 * @description: post alarm event to cloud
 * @param [in] hdl:         link visual service handler
 * @param [in] param:       alarm param struction
 * @return                  0: Success, < 0: Error code.
 */
int cx_lv_post_alarm_event(cxlv_hdl_t hdl, cx_lv_alarm_param_t *param);

#ifdef __cplusplus
}
#endif

#endif