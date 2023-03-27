#include "usbh_core.h"
#include "usbh_msc.h"
#include <aos/cli.h>

USB_MEM_ALIGNX uint8_t partition_table[512];

int msc_test(void)
{
    int ret;
    struct usbh_msc *msc_class = (struct usbh_msc *)usbh_find_class_instance("/dev/sda");
    if (msc_class == NULL) {
        USB_LOG_RAW("do not find /dev/sda\r\n");
        return -1;
    }
    /* get the partition table */
    ret = usbh_msc_scsi_read10(msc_class, 0, partition_table, 1);
    if (ret < 0) {
        USB_LOG_RAW("scsi_read10 error,ret:%d\r\n", ret);
        return ret;
    }
    for (uint32_t i = 0; i < 512; i++) {
        if (i % 16 == 0) {
            USB_LOG_RAW("\r\n");
        }
        USB_LOG_RAW("%02x ", partition_table[i]);
    }
    USB_LOG_RAW("\r\n");

    return ret;
}

void usbh_device_mount_done_callback(struct usbh_hubport *hport)
{
}

void usbh_device_unmount_done_callback(struct usbh_hubport *hport)
{
}

static void usbh_class_test_thread(void *argument)
{
    while (1) {
        printf("helloworld\r\n");
        usb_osal_msleep(1000);
        msc_test();
    }
}

void usbh_msc_test()
{
    usb_osal_thread_create("usbh_test", 4096, CONFIG_USBHOST_PSC_PRIO + 1, usbh_class_test_thread, NULL);
}

ALIOS_CLI_CMD_REGISTER(usbh_msc_test, usbh_msc_test, usb host test);
