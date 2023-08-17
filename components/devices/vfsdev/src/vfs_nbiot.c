/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#if defined(CONFIG_SAL)
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/nbiot.h>
#include <string.h>

static aos_status_t _devfs_nbiot_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case NBIOT_IOC_START:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_nbiot_start(dev)) {
            return -EIO;
        }
        break;
    }
    case NBIOT_IOC_STOP:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_nbiot_stop(dev)) {
            return -EIO;
        }
        break;
    }
    case NBIOT_IOC_GET_STATUS:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_nbiot_status_t link_status;
        memcpy(&link_status, (const void *)arg, sizeof(rvm_hal_nbiot_status_t));
        if (rvm_hal_nbiot_get_status(dev, &link_status)) {
            ret = -EIO;
            break;
        }
        break;
    }
    case NBIOT_IOC_GET_IMSI:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_nbiot_imsi_t imsi;
        memcpy(&imsi, (const void *)arg, sizeof(rvm_hal_nbiot_imsi_t));
        if (rvm_hal_nbiot_get_imsi(dev, &imsi)) {
            ret = -EIO;
            break;
        }
        break;
    }
    case NBIOT_IOC_GET_IMEI:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_nbiot_imei_t imei;
        memcpy(&imei, (const void *)arg, sizeof(rvm_hal_nbiot_imei_t));
        if (rvm_hal_nbiot_get_imei(dev, &imei)) {
            ret = -EIO;
            break;
        }
        break;
    }
    case NBIOT_IOC_GET_CSQ:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        int csq;
        memcpy(&csq, (const void *)arg, sizeof(int));
        if (rvm_hal_nbiot_get_csq(dev, &csq)) {
            ret = -EIO;
            break;
        }
        break;
    }
    case NBIOT_IOC_GET_SIMCARD_INFO:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_nbiot_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_nbiot_dev_msg_t));
        if (rvm_hal_nbiot_get_simcard_info(dev, msg.iccid, msg.insert)) {
            ret = -EIO;
            break;
        }
        break;
    }
    case NBIOT_IOC_GET_CELL_INFO:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_nbiot_cell_info_t cellinfo;
        memcpy(&cellinfo, (const void *)arg, sizeof(rvm_hal_nbiot_cell_info_t));
        if (rvm_hal_nbiot_get_cell_info(dev, &cellinfo)) {
            ret = -EIO;
            break;
        }
        break;
    }
    case NBIOT_IOC_SET_STATUS_IND:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        int status;
        memcpy(&status, (const void *)arg, sizeof(int));
        if (rvm_hal_nbiot_set_status_ind(dev, status)) {
            ret = -EIO;
            break;
        }
        break;
    }
    case NBIOT_IOC_SET_SIGNAL_STRENGTH_IND:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        int status;
        memcpy(&status, (const void *)arg, sizeof(int));
        if (rvm_hal_nbiot_set_signal_strength_ind(dev, status)) {
            ret = -EIO;
            break;
        }
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const devfs_file_ops_t devfs_nbiot_ops = {
    .ioctl      = _devfs_nbiot_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t nbiot_ops_node = {
    .name = "nbiot",
    .ops = (devfs_file_ops_t *)&devfs_nbiot_ops,
};

void nbiot_devfs_init(void)
{
    devices_add_devfs_ops_node(&nbiot_ops_node);
}
VFS_DEV_DRIVER_ENTRY(nbiot_devfs_init);

#endif

#endif
