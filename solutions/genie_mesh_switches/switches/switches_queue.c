/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <aos/hal/gpio.h>

#include "common/log.h"
#include "pinmux.h"
#include "genie_service.h"
#include "switches_queue.h"

static aos_queue_t smart_button_queue;

int queue_send_data(queue_mesg_t *pdata)
{
    int ret = 0;

    ret = aos_queue_send(&smart_button_queue, (void *)pdata, sizeof(queue_mesg_t));
    if (0 != ret)
    {
        GENIE_LOG_ERR("sb send mesg fail:%d", ret);
        return -1;
    }

    return 0;
}

int queue_recv_data(queue_mesg_t *pdata)
{
    int ret = 0;
    unsigned int read_size = 0;

    ret = aos_queue_recv(&smart_button_queue, AOS_WAIT_FOREVER, (void *)pdata, &read_size);
    if (ret == 0 && read_size > 0)
    {
        return 0;
    }

    return -1;
}

int queue_init(void)
{
    int ret = 0;
    uint8_t *queue_buff = NULL;

    queue_buff = (uint8_t *)aos_malloc(sizeof(queue_mesg_t) * QUEUE_MAX_MSG_SIZE);
    if (queue_buff == NULL)
    {
        GENIE_LOG_ERR("input event no mem");
        return -1;
    }

    memset(queue_buff, 0, sizeof(queue_mesg_t));
    ret = aos_queue_new(&smart_button_queue, (void *)queue_buff, sizeof(queue_mesg_t), sizeof(queue_mesg_t) * QUEUE_MAX_MSG_SIZE);
    if (ret != 0)
    {
        GENIE_LOG_ERR("input event queue init fail");
        return ret;
    }

    return ret;
}
