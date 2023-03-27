/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include <devices/impl/blockdev_impl.h>
#include <drv/sdif.h>
#include <errno.h>
#include <sd.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <usbh_core.h>
#include <usbh_msc.h>

#define TAG "msc_drv"

typedef struct {
    rvm_dev_t               device;
    struct usbh_msc *       msc_class;
    rvm_hal_blockdev_info_t info;
} usb_dev_t;

#define _usb_uninit rvm_hal_device_free

static rvm_dev_t *_usb_init(driver_t *drv, void *config, int id)
{
    usb_dev_t *dev = (usb_dev_t *)rvm_hal_device_new(drv, sizeof(usb_dev_t), id);

    return (rvm_dev_t *)dev;
}

static int _usb_open(rvm_dev_t *dev)
{
    usb_dev_t *usb = (usb_dev_t *)dev;
    if (!usb)
        return -EINVAL;

    usb->msc_class = (struct usbh_msc *)usbh_find_class_instance("/dev/sda");

    if (usb->msc_class == NULL) {
        LOGE(TAG, "no usb msc");
        return -1;
    }

    memset(&usb->info, 0, sizeof(usb->info));
    usb->info.block_size = usb->msc_class->blocksize;
    usb->info.erase_blks = 1;
    usb->info.user_area_blks = usb->msc_class->blocknum;

    return 0;
}

static int _usb_close(rvm_dev_t *dev)
{
    usb_dev_t *usb = (usb_dev_t *)dev;

    if (!usb)
        return -EINVAL;

    return 0;
}

static int _usb_read(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt)
{
    int ret = -1;
    usb_dev_t *usb = (usb_dev_t *)dev;

    if (!usb || !buffer)
        return -EINVAL;

    if (usb->msc_class) {
        ret = usbh_msc_scsi_read10(usb->msc_class, start_block, buffer, block_cnt);
    }

    return ret;
}

static int _usb_write(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt)
{
    int ret = -1;
    usb_dev_t *usb = (usb_dev_t *)dev;

    if (!usb || !buffer)
        return -EINVAL;

    if (usb->msc_class) {
        ret = usbh_msc_scsi_write10(usb->msc_class, start_block, buffer, block_cnt);
    }

    return ret;
}

static int _usb_erase(rvm_dev_t *dev, uint32_t start_block, uint32_t block_cnt)
{
    usb_dev_t *usb = (usb_dev_t *)dev;

    if (!usb)
        return -EINVAL;

    LOGE(TAG, "not support erase");
    return 0;
}

static int _usb_get_info(rvm_dev_t *dev, rvm_hal_blockdev_info_t *info)
{
    usb_dev_t *usb = (usb_dev_t *)dev;

    if (!usb || !info)
        return -EINVAL;

    info->block_size     = usb->info.block_size;
    info->erase_blks     = usb->info.erase_blks;
    info->user_area_blks = usb->info.user_area_blks;
    info->boot_area_blks = usb->info.boot_area_blks;

    return 0;
}

static blockdev_driver_t usb_driver = {
    .drv =
        {
            .name = "usb_mass",
            .init = _usb_init,
            .uninit = _usb_uninit,
            .open = _usb_open,
            .close = _usb_close,
        },
    .read_blks = _usb_read,
    .write_blks = _usb_write,
    .erase_blks = _usb_erase,
    .get_info = _usb_get_info,
};

void rvm_usb_drv_register(int idx)
{
    rvm_driver_register(&usb_driver.drv, NULL, idx);
}
