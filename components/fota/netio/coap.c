/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc_config.h>
#include <yoc/netio.h>
#include <yoc/fota.h>
#include <aos/network.h>
#include "er-coap-13/er-coap-13.h"
#include <aos/log.h>

// #define BUFFER_SIZE 768
#define COAP_205_CONTENT                (uint8_t)0x45

#define TAG "fota"

typedef struct target_coap {
    size_t block_size;
    char *path;
    int block_num;
    int finish;
    uint8_t *buffer;
} target_coap_t;

// static target_coap_t *coap_ins;
static network_t net;

static int nextMID = 0;

static int coap_message_send(target_coap_t * coap_ins, coap_packet_t *message)
{
    int buffer_len;

    buffer_len = coap_serialize_get_size(message);

    if (buffer_len <= 0) {
        return 0;
    }

    if (buffer_len > BUFFER_SIZE) {
        return -1;
    }

    buffer_len = coap_serialize_message(message, coap_ins->buffer);

    return net.net_write(&net, coap_ins->buffer, buffer_len, 10000);
}

static int coap_message_recv(target_coap_t *coap_ins, uint8_t *buffer, int lenght, int timeoutms)
{
    int rc;
    uint8_t coap_error_code = NO_ERROR;
    coap_packet_t message[1];
    uint8_t *payload;
    uint32_t num, offset;
    uint8_t more = 1;
    uint16_t size;
    int payload_len;

    if (coap_ins->finish == 1) {
        return 0;
    }

    while (1) {
        rc = net.net_read(&net, coap_ins->buffer, BUFFER_SIZE, timeoutms);

        if (rc <= 0) {
            return -1;
        }

        coap_error_code = coap_parse_message(message, coap_ins->buffer, (uint16_t)rc);
        if (coap_error_code != NO_ERROR) {
            LOGD(TAG, "Message parsing failed %u.%2u", coap_error_code >> 5, coap_error_code & 0x1F);
            return -1;
        }

        // LOGD(TAG, "Parsed: ver %u, type %u, tkl %u, code %u.%.2u, mid %u, Content type: %d\r\n",
        //         message->version, message->type, message->token_len, message->code >> 5, message->code & 0x1F, message->mid, message->content_type);

        if (message->mid != (nextMID - 1)) {
            LOGD(TAG, "MID error, drop it");
            continue;
        }

        if (message->code != COAP_205_CONTENT) {
            return -1;
        }

        if (coap_get_header_block2(message, &num, &more, &size, &offset) == 1) {
            if (more == 0)
            coap_ins->finish = 1;
            LOGD(TAG, "coap_get_header_block2 more: %d, size: %d, offset: %d\r\n", more, size, offset);
        }

        payload_len = coap_get_payload(message, (const uint8_t **)&payload);

        if (payload_len > lenght || payload_len <= 0) {
            return -1;
        }

        break;
    }

    memcpy(buffer, payload, payload_len);

    return payload_len;
}

int coap_open(netio_t *io, const char *path)
{
    // LOGD(TAG, "coap_open %s\r\n", path);
    char *ip, *port;
    char *end_point;
    // target_coap_t *coap_ins = (target_coap_t*)io->private;
    target_coap_t *coap_ins = aos_zalloc_check(sizeof(target_coap_t));

    ip = (char *)path + strlen("coap://");

    port = strrchr(ip, ':');

    if (port == NULL) {
        free(coap_ins);
        return -1;
    }

    *port = 0;
    port++;

    network_init(&net);
    net.net_connect(&net, ip, strtol(port, &end_point, 10), SOCK_DGRAM);

    if (end_point != NULL) {
        coap_ins->path = strdup(end_point);
        // LOGD(TAG, "coap_open: path %s\r\n", coap_ins->path);

        if (coap_ins->path == NULL) {
            net.net_disconncet(&net);
            free(coap_ins);
            return -1;
        }
    }

    if ((coap_ins->buffer = malloc(BUFFER_SIZE)) == NULL) {
        free(coap_ins->path);
        net.net_disconncet(&net);
        free(coap_ins);
        return -1;
    }

    coap_ins->block_num = 0;
    coap_ins->finish = 0;
    coap_ins->block_size = 512;

    io->size = -1;

    io->private = coap_ins;
    // LOGD(TAG, "coap_open! %s %x %x %x %x %x", coap_ins->path, coap_ins->block_num, coap_ins->block_size, coap_ins->finish, coap_ins->buffer, io->private);
    return 0;
}

int coap_close(netio_t *io)
{
    // LOGD(TAG, "coap_close\r\n");
    target_coap_t *coap_ins = (target_coap_t*)io->private;
    net.net_disconncet(&net);

    free(coap_ins->path);

    free(coap_ins->buffer);
    free(coap_ins);
    return 0;
}

int coap_read(netio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    coap_packet_t message[1];
    int rc;
    target_coap_t *coap_ins = (target_coap_t*)io->private;

    LOGD(TAG, "coap_read: %p, %d, %d, %d\r\n", buffer, length, timeoutms, coap_ins->block_num);
    // LOGD(TAG, "coap_read! %s %x %x %x %x %x", coap_ins->path, coap_ins->block_num, coap_ins->block_size, coap_ins->finish, coap_ins->buffer, io->private);
    uint8_t temp_token[COAP_TOKEN_LEN];
    time_t tv_sec = aos_now_ms();

    // initialize first 6 bytes, leave the last 2 random
    temp_token[0] = nextMID;
    temp_token[1] = nextMID >> 8;
    temp_token[2] = tv_sec;
    temp_token[3] = tv_sec >> 8;
    temp_token[4] = tv_sec >> 16;
    temp_token[5] = tv_sec >> 24;

    coap_init_message(message, COAP_TYPE_CON, COAP_GET, nextMID ++);
    coap_set_header_uri_path(message, coap_ins->path);
    coap_set_header_token(message, temp_token, 8);
    coap_set_header_block2(message, coap_ins->block_num ++ , 1, length);

    rc = coap_message_send(coap_ins, message);

    if (rc < 0) {
        coap_ins->block_num --;
        LOGD(TAG, "coap_message_send failed\r\n");
        return rc;
    }

    rc = coap_message_recv(coap_ins, buffer, length, timeoutms);

    if (rc < 0) {
        LOGD(TAG, "coap_message_recv failed\r\n");
        coap_ins->block_num --;
    }

    return rc;
}

int coap_seek(netio_t *io, size_t offset, int whence)
{
    target_coap_t *coap_ins = (target_coap_t*)io->private;

    coap_ins->block_num = offset / coap_ins->block_size;
    LOGD(TAG, "coap seek %x %x %x %x", coap_ins->block_num, offset, coap_ins->block_size, io->private);
    return 0;
}


const netio_cls_t coap_cls = {
    .name = "coap",
    .read = coap_read,
    .seek = coap_seek,
    .open = coap_open,
    .close = coap_close,
};

int netio_register_coap(void)
{
    return netio_register(&coap_cls);
}