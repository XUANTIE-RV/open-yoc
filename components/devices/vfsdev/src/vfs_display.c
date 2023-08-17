/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/display.h>
#include <string.h>

static aos_status_t _devfs_display_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case DISPLAY_IOC_SET_EVENT:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_display_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_display_dev_msg_t));
        if (rvm_hal_display_set_event(dev, msg.event_cb, msg.priv)) {
            return -EIO;
        }
        break;
    }
    case DISPLAY_IOC_GET_INFO:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_display_info_t info;
        if (rvm_hal_display_get_info(dev, &info)) {
            return -EIO;
        }
        memcpy((void *)arg, &info, sizeof(rvm_hal_display_info_t));
        break;
    }
    case DISPLAY_IOC_GET_FRAMEBUFFER:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_display_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_display_dev_msg_t));
        if (rvm_hal_display_get_framebuffer(dev, msg.smem_start, msg.smem_len)) {
            return -EIO;
        }
        break;
    }
    case DISPLAY_IOC_SET_BRIGHTNESS:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint8_t brightness;
        memcpy(&brightness, (void *)arg, sizeof(uint8_t));
        if (rvm_hal_display_set_brightness(dev, brightness)) {
            return -EIO;
        }
        break;
    }
    case DISPLAY_IOC_GET_BRIGHTNESS:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_display_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_display_dev_msg_t));
        if (rvm_hal_display_get_brightness(dev, msg.brightness)) {
            return -EIO;
        }
        break;
    }
    case DISPLAY_IOC_WRITE_AREA:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }

        rvm_display_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_display_dev_msg_t));
        if (rvm_hal_display_write_area(dev, msg.area, msg.data)) {
            return -EIO;
        }
        break;
    }
    case DISPLAY_IOC_WRITE_AREA_ASYNC:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }

        rvm_display_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_display_dev_msg_t));
        if (rvm_hal_display_write_area_async(dev, msg.area, msg.data)) {
            return -EIO;
        }
        break;
    }
    case DISPLAY_IOC_READ_AREA:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_display_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_display_dev_msg_t));
        if (rvm_hal_display_read_area(dev, msg.area, msg.data)) {
            return -EIO;
        }
        break;
    }
    case DISPLAY_IOC_PAN_DISPLAY:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_display_pan_display(dev)) {
            return -EIO;
        }
        break;
    }
    case DISPLAY_IOC_BLANK_ON_OFF:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        uint8_t on_off;
        memcpy(&on_off, (const void *)arg, sizeof(uint8_t));
        if (rvm_hal_display_blank_on_off(dev, on_off)) {
            return -EIO;
        }
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const devfs_file_ops_t devfs_display_ops = {
    .ioctl      = _devfs_display_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t display_ops_node = {
    .name = "disp",
    .ops = (devfs_file_ops_t *)&devfs_display_ops,
};

void display_devfs_init(void)
{
    devices_add_devfs_ops_node(&display_ops_node);
}
VFS_DEV_DRIVER_ENTRY(display_devfs_init);

#endif
