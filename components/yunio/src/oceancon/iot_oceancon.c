/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/log.h>
#include <aos/list.h>
#include <yoc/sysinfo.h>

#include <yoc/iot.h>
#include "iot_oceancon.h"

static const char *TAG = "OCEANCON";

static void lwm2m_start(void *argv)
{
    int result;
    uint8_t first_time_ready = 1;

    iot_channel_t *ch = (iot_channel_t *)argv;

    struct oceancon_context *ctx = (struct oceancon_context *)ch->priv;

    lwm2m_context_t *lwm2mH = (lwm2m_context_t *)ctx->lwm2m_ctx;

    /* clear the flag */
    aos_event_set(&ctx->event, 0x0, 1);

    while (ctx->task_run) {
        /*
        * This function does two things:
        *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
        *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
        *    (eg. retransmission) and the time between the next operation
        */
        time_t timeout = get_server_lifetime(ctx->objArray[1], 0);

        result = lwm2m_step(lwm2mH, &timeout);

        LOGI(TAG, "lwm2m_step time %d", timeout);

        if (result != 0) {
            LOGE(TAG, "lwm2m_step() failed: 0x%X\r\n", result);
            event_publish(EVENT_IOT_DISCONNECTED, (void*)ch);
            break;
        }

        lwm2m_recv(lwm2mH, ctx->recv_buffer, MAX_PACKET_SIZE, timeout);

        if (lwm2mH->serverList->status == STATE_REG_FAILED) {
            event_publish(EVENT_IOT_DISCONNECTED, (void*)ch);
            break;
        } else if (lwm2mH->serverList->status == STATE_REGISTERED) {
            if (first_time_ready) {
                event_publish(EVENT_IOT_CONNECT_SUCCESS, (void*)ch);
                first_time_ready = 0;
            }
        }
    }

    aos_event_set(&ctx->event, 0x2, 0);
    aos_sem_signal(&ctx->task_sem);

    aos_task_exit(0);
}

int oceancon_channel_open(iot_channel_t *ch)
{
    int result;

    iot_channel_t *channel = ch;
    lwm2m_context_t *lwm2mH = NULL;
    struct oceancon_context *ctx = NULL;

    const char *endpoint = aos_get_imei();

    if (endpoint == NULL) {
         LOGE(TAG, "set imei first");
        return -1;
    }

    iot_oceancon_config_t *cfg = (iot_oceancon_config_t *)ch->iot->user_cfg;

    ctx = aos_malloc(sizeof(struct oceancon_context));

    if (ctx == NULL) {
        goto fatal;
    }

    ctx->inter_buffer = aos_malloc(cfg->buffer_size);

    if (ctx->inter_buffer == NULL) {
        goto fatal;
    }

    ctx->inter_buffer_size = cfg->buffer_size;

    ctx->buffer_to_udata = cfg->buffer_to_udata;
    ctx->udata_to_buffer = cfg->udata_to_buffer;

        /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    lwm2mH = lwm2m_init();

    if (NULL == lwm2mH) {
        LOGE(TAG, "lwm2m_init() failed\r\n");
        goto fatal;
    }


    /*
     * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
    int serverId = 123;

    ctx->objArray[0] = get_security_object(serverId, cfg->server_addr, NULL, NULL, -1, false);

    if (NULL == ctx->objArray[0]) {
        LOGE(TAG, "Failed to create security object\r\n");
        goto fatal;
    }

    ctx->objArray[1] = get_server_object(serverId, "U", cfg->lifetime, false);

    if (NULL == ctx->objArray[1]) {
        LOGE(TAG, "Failed to create server object\r\n");
        goto fatal;
    }

    ctx->objArray[2] = get_object_device();

    if (NULL == ctx->objArray[2]) {
        LOGE(TAG, "Failed to create Device object\r\n");
        goto fatal;
    }

    ctx->objArray[3] = get_object_firmware(lwm2mH);

    if (NULL == ctx->objArray[3]) {
        LOGE(TAG, "Failed to create Firmware object\r\n");
        goto fatal;
    }

    ctx->objArray[4] = get_object_conn_m();

    if (NULL == ctx->objArray[4]) {
        LOGE(TAG, "Failed to create connectivity monitoring object\r\n");
        goto fatal;
    }

    ctx->objArray[5] = get_test_object(channel);

    if (NULL == ctx->objArray[5]) {
        LOGE(TAG, "Failed to create test object\r\n");
        goto fatal;
    }

    /*
     * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    result = lwm2m_configure(lwm2mH, endpoint, NULL, NULL, OBJ_COUNT, ctx->objArray);

    if (result != 0) {
        LOGE(TAG, "lwm2m_configure() failed: 0x%X\r\n", result);
        goto fatal;
    }

    uint8_t *recv_buffer = aos_malloc(MAX_PACKET_SIZE);

    if (recv_buffer == NULL) {
        goto fatal;
    }

    if (aos_sem_new(&ctx->task_sem, 0) < 0) {
        goto fatal;
    }

    if (aos_event_new(&ctx->event, 0) < 0) {
        goto fatal;
    }

    ctx->recv_buffer = recv_buffer;
    ctx->task_run = 0;
    ctx->lwm2m_ctx = lwm2mH;


    channel->priv = ctx;

    return 0;

fatal:

    if (lwm2mH) {
        lwm2m_close(lwm2mH);
    }

    if (channel) {
        if (channel->uData) {
            yoc_udata_free(channel->uData);
        }

        aos_free(channel);
    }

    if (ctx) {
        if (ctx->objArray[0]) {
            clean_security_object(ctx->objArray[0]);
        }

        if (ctx->objArray[1]) {
            clean_server_object(ctx->objArray[1]);
        }

        if (ctx->objArray[2]) {
            free_object_device(ctx->objArray[2]);
        }

        if (ctx->objArray[3]) {
            free_object_firmware(ctx->objArray[3]);
        }

        if (ctx->objArray[4]) {
            free_object_conn_m(ctx->objArray[4]);
        }

        if (ctx->objArray[5]) {
            free_test_object(ctx->objArray[5]);
        }

        if (ctx->recv_buffer) {
            aos_free(ctx->recv_buffer);
        }

        if (ctx->inter_buffer) {
            aos_free(ctx->inter_buffer);
        }

        if (aos_sem_is_valid(&ctx->task_sem)) {
            aos_sem_free(&ctx->task_sem);
        }

        aos_free(ctx);
    }

    return -1;
}

int oceancon_channel_send(iot_channel_t *ch)
{
    struct oceancon_context *ctx = (struct oceancon_context *)ch->priv;
    lwm2m_uri_t uri;
    uint32_t flag;

    if (aos_event_get(&ctx->event, 0x3, 1, &flag, -1)) {
        return -1;
    }

    lwm2m_stringToUri("/19/0/0", 7, &uri);

    if (lwm2m_resource_value_changed(ctx->lwm2m_ctx, &uri)) {
        return -1;
    }

    lwm2m_break_recv(ctx->lwm2m_ctx);

    return 0;
}

int oceancon_channel_start(iot_channel_t *ch)
{
    struct oceancon_context *ctx = (struct oceancon_context *)ch->priv;

    ctx->task_run = 1;
    return aos_task_new("lwm2m", lwm2m_start, ch, 1280);
}

void oceancon_channel_close(iot_channel_t *ch)
{
    struct oceancon_context *ctx = (struct oceancon_context *)ch->priv;

    ctx->task_run = 0;
    lwm2m_break_recv(ctx->lwm2m_ctx);

    aos_sem_wait(&ctx->task_sem, AOS_WAIT_FOREVER);

    aos_sem_free(&ctx->task_sem);

    aos_event_free(&ctx->event);

    lwm2m_close(ctx->lwm2m_ctx);

    aos_free(ctx->recv_buffer);

    aos_free(ctx->inter_buffer);

    if (ctx->objArray[0]) {
        clean_security_object(ctx->objArray[0]);
    }

    if (ctx->objArray[1]) {
        clean_server_object(ctx->objArray[1]);
    }

    if (ctx->objArray[2]) {
        free_object_device(ctx->objArray[2]);
    }

    if (ctx->objArray[3]) {
        free_object_firmware(ctx->objArray[3]);
    }

    if (ctx->objArray[4]) {
        free_object_conn_m(ctx->objArray[4]);
    }

    if (ctx->objArray[5]) {
        free_test_object(ctx->objArray[5]);
    }

    aos_free(ctx);
}

int oceancon_destroy(iot_t *iot)
{
    aos_free(iot->user_cfg);
    aos_free(iot);
    return 0;
}

iot_t *iot_new_oceancon(iot_oceancon_config_t *config)
{
    aos_check_param(config);

    iot_t *oceancon_iot = aos_malloc(sizeof(iot_t));

    if (oceancon_iot) {
        memset(oceancon_iot, 0, sizeof(iot_t));
        oceancon_iot->channel_open  = oceancon_channel_open;
        oceancon_iot->channel_start = oceancon_channel_start;
        oceancon_iot->channel_send  = oceancon_channel_send;
        oceancon_iot->channel_close = oceancon_channel_close;
        oceancon_iot->destroy       = oceancon_destroy;

        iot_oceancon_config_t *cfg = aos_malloc(sizeof(iot_oceancon_config_t));

        if (cfg == NULL) {
            aos_free(oceancon_iot);
            return NULL;
        }

        memcpy(cfg, config, sizeof(iot_oceancon_config_t));
        oceancon_iot->user_cfg = cfg;
    }

    return oceancon_iot;
}
