/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __AT_EVENT_H__
#define __AT_EVENT_H__

#include <sys/types.h>
#include "aos/yloop.h"

typedef enum {
    AT_EVENT_CMD_UART = 0,       /**< at cmd from uart */
    AT_EVENT_CMD_BACK,           /**< at cmd from mdm */
    AT_EVENT_CMD_FOTA,           /**< at cmd from fota */
    AT_EVENT_ELOOP_WIFI,         /**< wifi event */
    AT_EVENT_ELOOP_ETH,          /**< eth event */
    AT_EVENT_SOCK,               /**< socket event */
    AT_EVENT_LPM,                /**< sleep event */
    AT_EVENT_YIO,                /**< yio event */
    AT_EVENT_MAX
} at_event_type_t;

typedef enum {
    AT_MSG_CMD = 0,                 /**< msg is external cmd */
    AT_MSG_EVENT                    /**< msg is internal event */
} at_msg_type_t;

typedef struct {
    at_msg_type_t      msg_type;    /**< msg type   (Level 1)  */
    at_event_type_t    evt_type;    /**< event type (Level 2)  */
    uint16_t           evt_code;    /**< event code (Level 3)  */
    uint32_t           evt_value;   /**< event value */
    void              *argv;        /**< event argv  */
} at_event_t;

typedef int32_t (*at_event_handle_fn_t)(at_event_t *e);

typedef struct {
    at_event_type_t evt_type;
    at_event_handle_fn_t evt_hdl;
} at_event_handle_t;

int32_t at_event_send(at_msg_type_t msg_type, at_event_type_t type,
                      uint16_t code, uint32_t val, void *argv);
#endif
