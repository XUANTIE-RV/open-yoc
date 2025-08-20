 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <aos/debug.h>
#include <aos/ringbuffer.h>

#define MIN(a, b) (a)<(b)? (a) : (b)

int ringbuffer_create(dev_ringbuf_t *ringbuffer, char *buffer, int length)
{
    aos_check_param(ringbuffer && buffer);

    memset(buffer, 0, length);

    ringbuffer->length = length - 1;
    ringbuffer->ridx = 0;
    ringbuffer->widx = 0;
    ringbuffer->buffer = (uint8_t *)buffer;

    return 0;
}

void ringbuffer_destroy(dev_ringbuf_t *ringbuffer)
{
    aos_check_param(ringbuffer);
    ringbuffer->buffer = NULL;
    ringbuffer->length = ringbuffer->ridx = ringbuffer->widx = 0;
}

int ringbuffer_available_read_space(dev_ringbuf_t *ringbuffer)
{
    aos_check_param(ringbuffer);

    uint32_t ridx = ringbuffer->ridx;
    uint32_t widx = ringbuffer->widx;

    if (ridx == widx) {
        return 0;
    } else if (ridx < widx) {
        return widx - ridx;
    } else {
        return ringbuffer->length - (ridx - widx - 1);
    }
}

int ringbuffer_write(dev_ringbuf_t *ringbuffer, uint8_t *data, uint32_t length)
{
    aos_check_param(ringbuffer && data);

    int i = 0;

    if (ringbuffer_available_write_space(ringbuffer) < length) {
        length = ringbuffer_available_write_space(ringbuffer);
    }

    for (i = 0; i < length; i++) {
        ringbuffer->buffer[ringbuffer->widx] = data[i];

        ringbuffer->widx++;

        if (ringbuffer->widx >= ringbuffer->length + 1) {
            ringbuffer->widx = 0;
        }
    }

    /* return real write len */
    return i;
}

int ringbuffer_read(dev_ringbuf_t *ringbuffer, uint8_t *target, uint32_t amount)
{
    aos_check_param(ringbuffer && target);

    int copy_sz = 0;
    int i;

    if (amount == 0) {
        return -1;
    }

    if (ringbuffer_empty(ringbuffer)) {
        return 0;
    }

    /* get real read size */
    int buffer_size = ringbuffer_available_read_space(ringbuffer);
    copy_sz = MIN(amount, buffer_size);

    /* cp data to user buffer */
    for (i = 0; i < copy_sz; i++) {
        target[i] = ringbuffer->buffer[ringbuffer->ridx];

        ringbuffer->ridx++;

        if (ringbuffer->ridx >= ringbuffer->length + 1) {
            ringbuffer->ridx = 0;
        }
    }

    return copy_sz;
}
