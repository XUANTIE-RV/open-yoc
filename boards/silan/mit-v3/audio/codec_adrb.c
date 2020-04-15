/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "codec_adrb.h"
#include "silan_voice_adc.h"

#define MIN(a, b) (a)<(b)? (a) : (b)
SRAM_TEXT int codec_adrb_create(codec_adrb_t *codec_adrb, char *buffer, int length)
{
    if(codec_adrb == NULL || buffer == NULL) {
        return -1;
    }

    memset(buffer, 0, length);

    codec_adrb->length = length;
    codec_adrb->head = 0;
    codec_adrb->tail = 0;
    codec_adrb->buffer = (uint8_t *)buffer;

    return 0;
}

SRAM_TEXT void codec_adrb_destroy(codec_adrb_t *codec_adrb)
{
    if(codec_adrb == NULL){
        return;
    }

    codec_adrb->length = codec_adrb->head = codec_adrb->tail = 0;
}

SRAM_TEXT int codec_adrb_available_read_space(codec_adrb_t *codec_adrb)
{
    if(codec_adrb == NULL){
        return -1;
    }

    uint32_t head = codec_adrb->head;
    uint32_t tail = codec_adrb->tail;

    if (head == tail) {
        return 0;
    } else if (head < tail) {
        return tail - head;
    } else {
        return codec_adrb->length - (head - tail);
    }
}

SRAM_TEXT int codec_adrb_write(codec_adrb_t *codec_adrb, uint8_t *data, uint32_t length)
{
    if(codec_adrb == NULL || data == NULL || length == 0) {
        return -1;
    }

    int i = 0;

    for (i = 0; i < length; i++) {
        if (codec_adrb_full(codec_adrb)) {
            break;
        }

        codec_adrb->buffer[codec_adrb->tail] = data[i];

        codec_adrb->tail++;
        codec_adrb->tail %= codec_adrb->length;
    }

    /* return real write len */
    return i;
}

SRAM_TEXT int codec_adrb_read(codec_adrb_t *codec_adrb, uint8_t *target, uint32_t amount)
{
    if(codec_adrb == NULL || target == NULL || amount == 0) {
        return -1;
    }

    int copy_sz = 0;
    int i;

    if (codec_adrb_empty(codec_adrb)) {
        return 0;
    }

    /* get real read size */
    int buffer_size = codec_adrb_available_read_space(codec_adrb);
    copy_sz = MIN(amount, buffer_size);

    /* cp data to user buffer */
    for (i = 0; i < copy_sz; i++) {
        target[i] = codec_adrb->buffer[codec_adrb->head];

        codec_adrb->head++;
        codec_adrb->head %= codec_adrb->length;
    }

    return copy_sz;
}
