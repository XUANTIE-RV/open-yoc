/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBH_CDC_ACM_H
#define USBH_CDC_ACM_H

#include "usbh_core.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct usbh_class_info usb_serial_class_info;
#define USBH_REGISTER_USB_SERIAL_CLASS()    usbh_register_class_driver(&usb_serial_class_info);

struct usbh_cdc_custom_serial {
    struct usbh_hubport *hport;

    usbh_pipe_t bulkin;  /* Bulk IN endpoint */
    usbh_pipe_t bulkout; /* Bulk OUT endpoint */
};

#ifdef __cplusplus
}
#endif

#endif /* USBH_CDC_ACM_H */
