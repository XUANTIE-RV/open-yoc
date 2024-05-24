/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/uart.h>
#include <string.h>

static aos_status_t _devfs_uart_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case UART_IOC_GET_DEFAULT_CONFIG:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_uart_config_t config;
        rvm_hal_uart_config_default(&config);
        memcpy((void *)arg, &config, sizeof(rvm_hal_uart_config_t));
        break;
    }
    case UART_IOC_GET_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_uart_config_t config;
        if (rvm_hal_uart_config_get(dev, &config)) {
            return -EIO;
        }
        memcpy((void *)arg, &config, sizeof(rvm_hal_uart_config_t));
        break;
    }
    case UART_IOC_SET_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_uart_config_t config;
        memcpy(&config, (void *)arg, sizeof(rvm_hal_uart_config_t));
        if (rvm_hal_uart_config(dev, &config)) {
            return -EIO;
        }
        break;
    }
    case UART_IOC_DMA_ENABLE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        bool enable;
        memcpy(&enable, (void *)arg, sizeof(bool));
        if (rvm_hal_uart_trans_dma_enable(dev, enable)) {
            return -EIO;
        }
        break;
    }
    case UART_IOC_SET_TYPE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        enum rvm_hal_uart_type_t type;
        memcpy(&type, (void *)arg, sizeof(enum rvm_hal_uart_type_t));
        if (rvm_hal_uart_set_type(dev, type)) {
            return -EIO;
        }
        break;
    }
    case UART_IOC_SET_BUFFER_SIZE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint32_t buf_size;
        memcpy(&buf_size, (void *)arg, sizeof(uint32_t));
        if (rvm_hal_uart_set_buffer_size(dev, buf_size)) {
            return -EIO;
        }
        break;
    }
    case UART_IOC_SET_EVENT:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_uart_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_uart_dev_msg_t));
        rvm_hal_uart_set_event(dev, msg.event, msg.priv);
        break;
    }
    case UART_IOC_SEND_POLL:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_uart_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_uart_dev_msg_t));
        if (rvm_hal_uart_send_poll(dev, msg.buffer, msg.size) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case UART_IOC_RECV_POLL:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_uart_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_uart_dev_msg_t));
        if (rvm_hal_uart_recv_poll(dev, msg.buffer, msg.size) < 0) {
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

ssize_t _devfs_uart_write(devfs_file_t *file, const void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    mode_t mode = devfs_file_get_mode(file);
    // printf("%s, %d\n", __func__, __LINE__);

    if (!devfs_file_is_writable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_uart_send(dev, buf, count, (mode & O_NONBLOCK) ? AOS_NO_WAIT : AOS_WAIT_FOREVER) < 0) {
        return -1;
    }
    return count;
}

ssize_t _devfs_uart_read(devfs_file_t *file, void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    mode_t mode = devfs_file_get_mode(file);

    if (!devfs_file_is_readable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_uart_recv(dev, buf, count, (mode & O_NONBLOCK) ? AOS_NO_WAIT : AOS_WAIT_FOREVER) < 0) {
        return -1;
    }
    return count;
}
static void event(rvm_dev_t *dev, int event_id, void *priv)
{
    if(!((devfs_node_t *)priv)->poll_notify ||  !((devfs_node_t *)priv)->poll_arg) {
        return ;
    }
    vfs_poll_notify_t notify = ((devfs_node_t *)priv)->poll_notify;
    void *args = ((devfs_node_t *)priv)->poll_arg;
    notify(NULL, args);
}

int _devfs_uart_poll(file_t *file, devfs_poll_request_t* req)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    devfs_node_t *node = (devfs_node_t *)file->node->i_arg;
    rvm_hal_uart_state_t state = 0;
    rvm_hal_uart_get_state(dev, &state);
    int revents = 0;
    if (state & RVM_UART_STATE_READABLE){ revents |= POLLIN; }
    if (state & RVM_UART_STATE_WRITABLE){ revents |= POLLOUT; }
    if (state & RVM_UART_STATE_ERROR){ revents |= POLLERR; }
    if (!node->poll_arg) {
        rvm_hal_uart_set_event(dev, event, node);
    } else {
        rvm_hal_uart_set_event(dev, NULL, NULL);
    }
    
    return revents;
}

static const devfs_file_ops_t devfs_uart_ops = {
    .ioctl      = _devfs_uart_ioctl,
    .poll       = _devfs_uart_poll,
    .mmap       = NULL,
    .read       = _devfs_uart_read,
    .write      = _devfs_uart_write,
    .lseek      = NULL,
};

static devfs_ops_node_t uart_ops_node = {
    .name = "uart",
    .ops = (devfs_file_ops_t *)&devfs_uart_ops,
};

void uart_devfs_init(void)
{
    devices_add_devfs_ops_node(&uart_ops_node);
}
VFS_DEV_DRIVER_ENTRY(uart_devfs_init);

static devfs_ops_node_t usb_serial_ops_node = {
    .name = "usb_serial",
    .ops = (devfs_file_ops_t *)&devfs_uart_ops,
};

void usb_serial_devfs_init(void)
{
    devices_add_devfs_ops_node(&usb_serial_ops_node);
}
VFS_DEV_DRIVER_ENTRY(usb_serial_devfs_init);

#endif