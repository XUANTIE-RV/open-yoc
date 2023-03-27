/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBH_HID_H
#define USBH_HID_H

#include "usb_hid.h"
#include "usbh_core.h"

extern const struct usbh_class_info hid_mouse_class_info;
#define USBH_REGISTER_HID_MOUSE_CLASS()    usbh_register_class_driver(&hid_mouse_class_info);

extern const struct usbh_class_info hid_keyboard_class_info;
#define USBH_REGISTER_HID_KEYBOARD_CLASS()    usbh_register_class_driver(&hid_keyboard_class_info);

struct usbh_hid {
    struct usbh_hubport *hport;

    uint8_t report_desc[128];
    uint8_t intf; /* interface number */
    uint8_t minor;
    usbh_pipe_t intin;  /* INTR IN endpoint */
    usbh_pipe_t intout; /* INTR OUT endpoint */
};

#ifdef __cplusplus
extern "C" {
#endif

int usbh_hid_set_idle(struct usbh_hid *hid_class, uint8_t report_id, uint8_t duration);
int usbh_hid_get_idle(struct usbh_hid *hid_class, uint8_t *buffer);

#ifdef __cplusplus
}
#endif

#endif /* USBH_HID_H */
