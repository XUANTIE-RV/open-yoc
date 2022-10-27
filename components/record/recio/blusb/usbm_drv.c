/**
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include <string.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>

#include <bl_usb.h>
#include <bl606p_usb.h>
#include <usbd_core.h>
#include <usbd_audio.h>

#define TAG "UACDRV"

#define AUDIO_IN_EP 0x81

static usbd_class_t     audio_class;
static usbd_interface_t audio_control_intf;
static usbd_interface_t audio_stream_intf;

static void usbd_audio_iso_callback(uint8_t ep)
{
    LOGI(TAG, "callback ep=%d", ep);
    return;
}

static usbd_endpoint_t audio_in_ep = { .ep_cb = usbd_audio_iso_callback, .ep_addr = AUDIO_IN_EP };
extern const uint8_t audio_descriptor_3ch[];
extern const uint8_t audio_descriptor_5ch[];

int blusbm_init(int chncnt)
{
    if (chncnt != 3 && chncnt != 5) {
        LOGI(TAG, "unsupport chncnt %d\r\n", chncnt);
        return -1;
    }

    LOGI(TAG, "blusbm_init\r\n");
    usb_clock_init();

    if (chncnt == 3) {
        usbd_desc_register(audio_descriptor_3ch);
    } else if (chncnt == 5) {
        usbd_desc_register(audio_descriptor_5ch);
    } else {
        return -1;
    }

    usbd_audio_add_interface(&audio_class, &audio_control_intf);
    usbd_audio_add_interface(&audio_class, &audio_stream_intf);
    usbd_interface_add_endpoint(&audio_stream_intf, &audio_in_ep);
    // usbd_audio_add_entity(0x01, AUDIO_CONTROL_CLOCK_SOURCE);
    usbd_audio_add_entity(0x02, AUDIO_CONTROL_FEATURE_UNIT);

    usbd_initialize();
    return 0;
}

int blusbm_write(const uint8_t *data, uint32_t data_len)
{
    uint32_t ret_bytes = 0;
    usbd_ep_write(AUDIO_IN_EP, data, data_len, &ret_bytes, USB_FOREVER);
    return (int)ret_bytes;
}

/*博流USB协议栈需要应用适配的接口*/
void usbd_audio_open(uint8_t intf)
{
    //LOGI(TAG, "PC Audio open");
    printf("PC Audio open %d\r\n", intf);
}

void usbd_audio_close(uint8_t intf)
{
    //LOGI(TAG, "PC Audio close");
    printf("PC Audio close %d\r\n", intf);
}
