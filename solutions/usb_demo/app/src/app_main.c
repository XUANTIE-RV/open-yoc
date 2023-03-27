/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <ulog/ulog.h>

#include "app_main.h"

#define TAG "main"

#include "usbh_core.h"
#include "usbd_core.h"
#include "usbh_cdc_acm.h"
#include "usbh_hid.h"
#include "usbh_msc.h"
#include "usbh_video.h"

#define HELP_INFO "usage:\tusbtest usbh\r\n    \
                   \tusbtest usbd uac/uvc\r\n"

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc_buffer[512];

struct usbh_urb cdc_bulkin_urb;
struct usbh_urb cdc_bulkout_urb;

void usbh_cdc_acm_callback(void *arg, int nbytes)
{
    //struct usbh_cdc_acm *cdc_acm_class = (struct usbh_cdc_acm *)arg;

    if (nbytes > 0) {
        for (size_t i = 0; i < nbytes; i++) {
            LOGD(TAG, "0x%02x ", cdc_buffer[i]);
        }
    }

    LOGD(TAG, "nbytes:%d\r\n", nbytes);
}

int cdc_acm_test(void)
{
    int ret;

    struct usbh_cdc_acm *cdc_acm_class = (struct usbh_cdc_acm *)usbh_find_class_instance("/dev/ttyACM0");

    if (cdc_acm_class == NULL) {
        LOGD(TAG, "do not find /dev/ttyACM0\r\n");
        return -1;
    }

    memset(cdc_buffer, 0, 512);

    usbh_bulk_urb_fill(&cdc_bulkin_urb, cdc_acm_class->bulkin, cdc_buffer, 64, 3000, NULL, NULL);
    ret = usbh_submit_urb(&cdc_bulkin_urb);
    if (ret < 0) {
        LOGD(TAG, "bulk in error,ret:%d\r\n", ret);
    } else {
        LOGD(TAG, "recv over:%d\r\n", cdc_bulkin_urb.actual_length);
        for (size_t i = 0; i < cdc_bulkin_urb.actual_length; i++) {
            LOGD(TAG, "0x%02x ", cdc_buffer[i]);
        }
        LOGD(TAG, "\r\n");
        LOGD(TAG, "recv %.*s\r\n", cdc_bulkin_urb.actual_length, cdc_buffer);
        LOGD(TAG, "recv end\r\n");
    }

    LOGD(TAG, "\r\n");
    // const uint8_t data1[10] = { 0x02, 0x00, 0x00, 0x00, 0x02, 0x02, 0x08, 0x14 };

    // memcpy(cdc_buffer, data1, 8);
    // usbh_bulk_urb_fill(&cdc_bulkout_urb, cdc_acm_class->bulkout, cdc_buffer, 8, 3000, NULL, NULL);
    // ret = usbh_submit_urb(&cdc_bulkout_urb);
    // if (ret < 0) {
    //     LOGD(TAG, "bulk out error,ret:%d\r\n", ret);
    // } else {
    //     LOGD(TAG, "send over:%d\r\n", cdc_bulkout_urb.actual_length);
    // }

    // usbh_bulk_urb_fill(&cdc_bulkin_urb, cdc_acm_class->bulkin, cdc_buffer, 64, 3000, usbh_cdc_acm_callback, cdc_acm_class);
    // ret = usbh_submit_urb(&cdc_bulkin_urb);
    // if (ret < 0) {
    //     LOGD(TAG, "bulk in error,ret:%d\r\n", ret);
    // } else {
    // }

    return ret;
}
#if 0
#include "ff.h"

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_write_buffer[25 * 100];

USB_NOCACHE_RAM_SECTION FATFS fs;
USB_NOCACHE_RAM_SECTION FIL fnew;
UINT fnum;
FRESULT res_sd = 0;

int usb_msc_fatfs_test()
{
    const char *tmp_data = "cherryusb fatfs demo...\r\n";

    LOGD(TAG, "data len:%d\r\n", strlen(tmp_data));
    for (uint32_t i = 0; i < 100; i++) {
        memcpy(&read_write_buffer[i * 25], tmp_data, strlen(tmp_data));
    }

    res_sd = f_mount(&fs, "2:", 1);
    if (res_sd != FR_OK) {
        LOGD(TAG, "mount fail,res:%d\r\n", res_sd);
        return -1;
    }

    LOGD(TAG, "test fatfs write\r\n");
    res_sd = f_open(&fnew, "2:test.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_sd == FR_OK) {
        res_sd = f_write(&fnew, read_write_buffer, sizeof(read_write_buffer), &fnum);
        if (res_sd == FR_OK) {
            LOGD(TAG, "write success, write len：%d\n", fnum);
        } else {
            LOGD(TAG, "write fail\r\n");
            goto unmount;
        }
        f_close(&fnew);
    } else {
        LOGD(TAG, "open fail\r\n");
        goto unmount;
    }
    LOGD(TAG, "test fatfs read\r\n");

    res_sd = f_open(&fnew, "2:test.txt", FA_OPEN_EXISTING | FA_READ);
    if (res_sd == FR_OK) {
        res_sd = f_read(&fnew, read_write_buffer, sizeof(read_write_buffer), &fnum);
        if (res_sd == FR_OK) {
            LOGD(TAG, "read success, read len：%d\n", fnum);
        } else {
            LOGD(TAG, "read fail\r\n");
            goto unmount;
        }
        f_close(&fnew);
    } else {
        LOGD(TAG, "open fail\r\n");
        goto unmount;
    }
    f_mount(NULL, "2:", 1);
    return 0;
unmount:
    f_mount(NULL, "2:", 1);
    return -1;
}
#endif

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t partition_table[512];

int msc_test(void)
{
    int ret;
    struct usbh_msc *msc_class = (struct usbh_msc *)usbh_find_class_instance("/dev/sda");
    if (msc_class == NULL) {
        LOGD(TAG, "do not find /dev/sda\r\n");
        return -1;
    }
#if 1
    /* get the partition table */
    ret = usbh_msc_scsi_read10(msc_class, 0, partition_table, 1);
    if (ret < 0) {
        LOGD(TAG, "scsi_read10 error,ret:%d\r\n", ret);
        return ret;
    }
    for (uint32_t i = 0; i < 512; i++) {
        if (i % 16 == 0) {
            LOGD(TAG, "\r\n");
        }
        LOGD(TAG, "%02x ", partition_table[i]);
    }
    LOGD(TAG, "\r\n");
#endif

#if 0
    usb_msc_fatfs_test();
#endif
    return ret;
}

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_buffer[128];

struct usbh_urb hid_intin_urb;

void usbh_hid_callback(void *arg, int nbytes)
{
    //struct usbh_hid *hid_class = (struct usbh_hid *)arg;

    if (nbytes > 0) {
        for (size_t i = 0; i < nbytes; i++) {
            printf("0x%02x ", hid_buffer[i]);
        }
    }

    printf("nbytes:%d\r\n", nbytes);
    usbh_submit_urb(&hid_intin_urb);
}

int hid_test(void)
{
    int ret;
    struct usbh_hid *hid_class = (struct usbh_hid *)usbh_find_class_instance("/dev/input0");
    if (hid_class == NULL) {
        LOGD(TAG, "do not find /dev/input0\r\n");
        return -1;
    }

    usbh_int_urb_fill(&hid_intin_urb, hid_class->intin, hid_buffer, 8, 0, usbh_hid_callback, hid_class);
    ret = usbh_submit_urb(&hid_intin_urb);
    return ret;
}

void usbh_device_mount_done_callback(struct usbh_hubport *hport)
{
    struct usb_interface_descriptor *intf_desc;

    for (uint8_t i = 0; i < hport->config.config_desc.bNumInterfaces; i++) {
        intf_desc = &hport->config.intf[i].altsetting[0].intf_desc;
        LOGD(TAG, "%s Class:0x%02x,Subclass:0x%02x,Protocl:0x%02x", __func__,
                    intf_desc->bInterfaceClass,
                    intf_desc->bInterfaceSubClass,
                    intf_desc->bInterfaceProtocol);
        if (hport->config.intf[i].class_driver)
            LOGD(TAG, "class driver: %s", hport->config.intf[i].class_driver->driver_name);
    }
}

void usbh_device_unmount_done_callback(struct usbh_hubport *hport)
{
    struct usb_interface_descriptor *intf_desc;

    for (uint8_t i = 0; i < hport->config.config_desc.bNumInterfaces; i++) {
        intf_desc = &hport->config.intf[i].altsetting[0].intf_desc;
        LOGD(TAG, "%s Class:0x%02x,Subclass:0x%02x,Protocl:0x%02x", __func__,
                    intf_desc->bInterfaceClass,
                    intf_desc->bInterfaceSubClass,
                    intf_desc->bInterfaceProtocol);
        if (hport->config.intf[i].class_driver)
            LOGD(TAG, "class driver: %s", hport->config.intf[i].class_driver->driver_name);
    }
}

void usbd_configure_done_callback(void)
{

}

static void usbh_class_test_thread(void *argument)
{
    usb_osal_msleep(3000);
    while (1) {
        // printf("helloworld\r\n");
        usb_osal_msleep(10);
        // cdc_acm_test();
        // msc_test();
        // hid_test();
    }
}

void usbh_class_test(void)
{
    USBH_REGISTER_MSC_CLASS();
    USBH_REGISTER_CDCACM_CLASS();
    USBH_REGISTER_HID_MOUSE_CLASS();
    usbh_initialize();

    usb_osal_thread_create("usbh_test", 4096, CONFIG_USBHOST_PSC_PRIO + 1, usbh_class_test_thread, NULL);
}

static void cmd_usbtest_func(char *wbuf, int wbuf_len, int argc, char **argv)
{

    if (argc < 2) {
        printf("%s\n", HELP_INFO);
        return;
    }

    if (0 == strcmp(argv[1], "usbh") ) {
        usbh_class_test();

    } else if (0 == strcmp(argv[1], "usbd") ) {

        if (argc < 3) {
            printf("%s\n", HELP_INFO);
            return;
        }
        if (0 == strcmp(argv[2], "uvc")) {
            printf("usbd uvc inited, please goto https://webcam-test.com/zh-CN to test it!\r\n");
            extern void video_init(void);
            video_init();

            // Wait until configured
            while (!usb_device_is_configured()) {
                aos_msleep(100);
            }

            // Everything is interrupt driven so just loop here
            while (1) {
                extern void video_test(void);
                video_test();
            }
        } else if (0 == strcmp(argv[2], "uac")) {
            extern void audio_init(void);
            printf("usbd uac inited, please use Audacity to test it!\r\n");
            audio_init();
            // Wait until configured
            while (!usb_device_is_configured()) {
                aos_msleep(100);
            }

            // Everything is interrupt driven so just loop here
            while (1) {
                extern void audio_test(void);
                audio_test();
            }
        } else if (0 == strcmp(argv[2], "rndis")) {

            extern void cdc_rndis_init(void);
            printf("rndis test start, please plug the USB on pc\r\n");
            cdc_rndis_init();

        } else {
            printf("%s\n", HELP_INFO);
            return;
        }
    } else {
        printf("%s\n", HELP_INFO);
        return;
    }
}

void cli_reg_cmd_usbtest(void)
{
    static const struct cli_command cmd_info = {
        "usbtest",
        "usbtest",
        cmd_usbtest_func
    };

    aos_cli_register_command(&cmd_info);
}

int main(int argc, char *argv[])
{
    board_yoc_init();

    LOGD(TAG, "build time: %s, %s\r\n", __DATE__, __TIME__);

    cli_reg_cmd_usbtest();
}
