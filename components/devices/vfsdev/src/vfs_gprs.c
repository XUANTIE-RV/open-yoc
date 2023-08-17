/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_SAL)
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/gprs.h>
#include <string.h>

static aos_status_t _devfs_gprs_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case GPRS_IOC_SET_MODE:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_gprs_mode_t mode;
        memcpy(&mode, (void *)arg, sizeof(rvm_hal_gprs_mode_t));
        if (rvm_hal_gprs_set_mode(dev, mode)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_GET_MODE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_gprs_mode_t mode;
        if (rvm_hal_gprs_get_mode(dev, &mode)) {
            return -EIO;
        }
        memcpy((void *)arg, &mode, sizeof(rvm_hal_gprs_mode_t));
        break;
    }
    case GPRS_IOC_RESET:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_gprs_reset(dev)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_START:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_gprs_start(dev)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_STOP:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_gprs_stop(dev)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_SET_IF_GONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_gprs_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_gprs_dev_msg_t));
        if (rvm_hal_gprs_set_if_config(dev, msg.baud, msg.flow_control)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_MODULE_INIT_CHECK:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_gprs_module_init_check(dev)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_CONNECT_TO_GPRS:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_gprs_connect_to_gprs(dev)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_DISCONNECT_FROM_GPRS:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_gprs_disconnect_from_gprs(dev)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_GET_LINK_STATUS:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_hal_gprs_status_link_t link_status;
        if (rvm_hal_gprs_get_link_status(dev, &link_status)) {
            return -EIO;
        }
        memcpy((void *)arg, &link_status, sizeof(rvm_hal_gprs_status_link_t));
        break;
    }
    case GPRS_IOC_GET_IPADDR:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        char ip[16];
        memcpy(ip, (const void *)arg, sizeof(char) * 16);
        if (rvm_hal_gprs_get_ipaddr(dev, ip)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_SMS_SEND:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_gprs_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_gprs_dev_msg_t));
        if (rvm_hal_gprs_sms_send(dev, msg.sca, msg.da, msg.content)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_SMS_SET_CB:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        recv_sms_cb cb;
        memcpy(&cb, (const void *)arg, sizeof(recv_sms_cb));
        if (rvm_hal_gprs_sms_set_cb(dev, cb)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_GET_IMSI:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_gprs_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_gprs_dev_msg_t));
        if (rvm_hal_gprs_get_imsi(dev, msg.imsi)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_GET_IMEI:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_gprs_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_gprs_dev_msg_t));
        if (rvm_hal_gprs_get_imei(dev, msg.imei)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_GET_CSQ:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_gprs_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_gprs_dev_msg_t));
        if (rvm_hal_gprs_get_csq(dev, msg.csq)) {
            return -EIO;
        }
        break;
    }
    case GPRS_IOC_GET_SIMCARD_INFO:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_gprs_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_gprs_dev_msg_t));
        if (rvm_hal_gprs_get_simcard_info(dev, msg.ccid, msg.insert)) {
            return -EIO;
        }
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    if ((cmd & DEV_NETDRV_IOCTL_CMD_BASE) == DEV_NETDRV_IOCTL_CMD_BASE) {
        extern aos_status_t _devfs_netdrv_ioctl(devfs_file_t *file, int cmd, uintptr_t arg);
        // netdrv的逻辑
        ret = _devfs_netdrv_ioctl(file, cmd, arg);
    } else if (cmd > DEV_WIFI_IOCTL_CMD_BASE) {
        // 上面switch的逻辑
        return ret;
    } else {
        // 错误
        ret = -EINVAL;
    }

    return ret;
}

static const devfs_file_ops_t devfs_gprs_ops = {
    .ioctl      = _devfs_gprs_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t gprs_ops_node = {
    .name = "gprs",
    .ops = (devfs_file_ops_t *)&devfs_gprs_ops,
};

void gprs_devfs_init(void)
{
    devices_add_devfs_ops_node(&gprs_ops_node);
}
VFS_DEV_DRIVER_ENTRY(gprs_devfs_init);

#endif

#endif

