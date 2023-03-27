/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBH_CDC_ACM_H
#define USBH_CDC_ACM_H

#include "usb_cdc.h"
#include "usbh_core.h"

struct usbh_cdc_acm {
    struct usbh_hubport *hport;

    struct cdc_line_coding linecoding;
    uint8_t ctrl_intf; /* Control interface number */
    uint8_t data_intf; /* Data interface number */
    bool dtr;
    bool rts;
    uint8_t minor;
    usbh_pipe_t bulkin;  /* Bulk IN endpoint */
    usbh_pipe_t bulkout; /* Bulk OUT endpoint */
#ifdef CONFIG_USBHOST_CDC_ACM_NOTIFY
    usbh_pipe_t intin; /* Interrupt IN endpoint (optional) */
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

extern const struct usbh_class_info cdc_acm_class_info;
#define USBH_REGISTER_CDCACM_CLASS()    usbh_register_class_driver(&cdc_acm_class_info);

int usbh_cdc_acm_set_line_coding(struct usbh_cdc_acm *cdc_acm_class, struct cdc_line_coding *line_coding);
int usbh_cdc_acm_get_line_coding(struct usbh_cdc_acm *cdc_acm_class, struct cdc_line_coding *line_coding);
int usbh_cdc_acm_set_line_state(struct usbh_cdc_acm *cdc_acm_class, bool dtr, bool rts);

#ifdef __cplusplus
}
#endif

#endif /* USBH_CDC_ACM_H */
