/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __IOT_OCEANCON_H_
#define __IOT_OCEANCON_H_
#include <aos/aos.h>
#include <aos/debug.h>
#include <yoc/udata.h>

#include <liblwm2m.h>
#include <connection.h>
#include <std_object.h>

#define DEFAULT_SERVER_IPV4 "180.101.147.115"

#define MAX_PACKET_SIZE 1024
#define OBJ_COUNT 6

struct oceancon_context {
    uint8_t    task_run;
    aos_sem_t  task_sem;
    aos_event_t event;
    uint8_t   *recv_buffer;
    lwm2m_context_t *lwm2m_ctx;
    lwm2m_object_t  *objArray[OBJ_COUNT];
    int (*udata_to_buffer)(uData *data, void *buffer);
    int (*buffer_to_udata)(uData *data, void *buffer, int len);
    uint8_t *inter_buffer;
    int inter_buffer_size;
};

lwm2m_object_t *get_test_object(iot_channel_t *ch);
void free_test_object(lwm2m_object_t *object);

#endif