/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/hci.h>
#include <string.h>

static aos_status_t _devfs_hci_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case HCI_IOC_SET_EVENT:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hci_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_hci_dev_msg_t));
        rvm_hal_hci_set_event(dev, msg.event, msg.priv);
        break;
    }
    case HCI_IOC_START:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        hci_driver_send_cmd_t send_cmd;
        memcpy(&send_cmd, (void *)arg, sizeof(hci_driver_send_cmd_t));
        if (rvm_hal_hci_start(dev, send_cmd)) {
            return -EIO;
        }
        break;
    }
    case HCI_IOC_SEND:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_hci_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_hci_dev_msg_t));
        if (rvm_hal_hci_send(dev, msg.data, msg.size) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case HCI_IOC_RECV:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_hci_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_hci_dev_msg_t));
        if (rvm_hal_hci_recv(dev, msg.data, msg.size) < 0) {
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

static const devfs_file_ops_t devfs_hci_ops = {
    .ioctl      = _devfs_hci_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t hci_ops_node = {
    .name = "hci",
    .ops = (devfs_file_ops_t *)&devfs_hci_ops,
};

void hci_devfs_init(void)
{
    devices_add_devfs_ops_node(&hci_ops_node);
}
VFS_DEV_DRIVER_ENTRY(hci_devfs_init);

#endif
