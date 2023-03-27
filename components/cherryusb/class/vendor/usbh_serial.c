/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "usbh_core.h"
#include "usbh_serial.h"

#define DEV_FORMAT "/dev/serial"

// static uint32_t g_devinuse = 0;
int usbh_serial_connect(struct usbh_hubport *hport, uint8_t intf)
{
    struct usb_endpoint_descriptor *ep_desc;

    if (intf != 3) {
        USB_LOG_WRN("ignore intf:%d\r\n", intf);
        return 0;
    }
    struct usbh_cdc_custom_serial *cdc_custom_class = usb_malloc(sizeof(struct usbh_cdc_custom_serial));
    if (cdc_custom_class == NULL) {
        USB_LOG_ERR("Fail to alloc cdc_custom_class\r\n");
        return -ENOMEM;
    }

    memset(cdc_custom_class, 0, sizeof(struct usbh_cdc_custom_serial));
    cdc_custom_class->hport = hport;

    strncpy(hport->config.intf[intf].devname, DEV_FORMAT, CONFIG_USBHOST_DEV_NAMELEN);

    hport->config.intf[intf].priv = cdc_custom_class;

    for (uint8_t i = 0; i < hport->config.intf[intf].altsetting[0].intf_desc.bNumEndpoints; i++) {
        ep_desc = &hport->config.intf[intf].altsetting[0].ep[i].ep_desc;

        if (ep_desc->bEndpointAddress & 0x80) {
            usbh_hport_activate_epx(&cdc_custom_class->bulkin, hport, ep_desc);
        } else {
            usbh_hport_activate_epx(&cdc_custom_class->bulkout, hport, ep_desc);
        }
    }

    USB_LOG_INFO("Register serial Class:%s\r\n", hport->config.intf[intf].devname);

    return 0;
}

int usbh_serial_disconnect(struct usbh_hubport *hport, uint8_t intf)
{
    int ret = 0;

    struct usbh_cdc_custom_serial *rndis_class = (struct usbh_cdc_custom_serial *)hport->config.intf[intf].priv;

    if (rndis_class) {
        if (rndis_class->bulkin) {
            usbh_pipe_free(rndis_class->bulkin);
        }

        if (rndis_class->bulkout) {
            usbh_pipe_free(rndis_class->bulkout);
        }

        usb_free(rndis_class);

        if (hport->config.intf[intf].devname[0] != '\0')
            USB_LOG_INFO("Unregister Class:%s\r\n", hport->config.intf[intf].devname);
    }

    return ret;
}

const struct usbh_class_driver usb_serial_class_driver = {
    .driver_name = "usb_serial",
    .connect = usbh_serial_connect,
    .disconnect = usbh_serial_disconnect
};

CLASS_INFO_DEFINE const struct usbh_class_info usb_serial_class_info = {
    .match_flags = USB_CLASS_MATCH_INTF_CLASS | USB_CLASS_MATCH_INTF_SUBCLASS | USB_CLASS_MATCH_INTF_PROTOCOL | USB_CLASS_MATCH_VENDOR | USB_CLASS_MATCH_PRODUCT,
    .class = 0xff,
    .subclass = 0,
    .protocol = 0,
    .vid = 0x2c7c,
    .pid = 0x6005,
    .class_driver = &usb_serial_class_driver
};
