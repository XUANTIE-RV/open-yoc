/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBD_VIDEO_H
#define USBD_VIDEO_H

#include "usb_video.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uvc_event_callbacks {
    void (*uvc_event_stream_on)(int is_on);
    void (*uvc_event_setup_class_control)(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len);
    void (*uvc_event_setup_class_streaming)(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len);
    void (*uvc_event_data_out)(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len);
    void (*uvc_event_tx_complete)(uint8_t ep, uint32_t nbytesn);
} uvc_event_callbacks_t;

/* Init video interface driver */
struct usbd_interface *usbd_video_init_intf(struct usbd_interface *intf,
                                            uint32_t dwFrameInterval,
                                            uint32_t dwMaxVideoFrameSize,
                                            uint32_t dwMaxPayloadTransferSize);

void usbd_video_open(uint8_t intf);
void usbd_video_close(uint8_t intf);
uint32_t usbd_video_mjpeg_payload_fill(uint8_t *input, uint32_t input_len, uint8_t *output, uint32_t *out_len);

#ifdef __cplusplus
}
#endif

#endif /* USBD_VIDEO_H */
