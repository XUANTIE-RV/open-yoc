/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <vfs_inode.h>
#include <vfs_file.h>
#include <vfs_register.h>
#include <devices/device.h>
#include "devfs.h"

static const char path_prefix[] = "/dev/";

static int devfs_open(inode_t *inode, file_t *file)
{
    devfs_node_t *node = (devfs_node_t *)inode->i_arg;

    if (node) {
        // printf("%s, %d, node->name:%s\n", __func__, __LINE__, node->name);
        rvm_dev_t *dev = rvm_hal_device_open(node->name);
        // printf("%s, %d, dev:0x%p\n", __func__, __LINE__, dev);
        if (!dev) {
            return -1;
        }
        file->f_arg = dev;
        return 0;
    }
    return -1;
}

static int devfs_close(file_t *file)
{
    int ret;
    rvm_dev_t *dev = (rvm_dev_t *)file->f_arg;
    ret = rvm_hal_device_close(dev);
    file->f_arg = NULL;
    return ret;
}

static ssize_t devfs_read(file_t *file, void *buf, size_t count)
{
    devfs_node_t *node = (devfs_node_t *)file->node->i_arg;

    if (!node->ops->read)
        return -ENOTSUP;

    if (!devfs_file_is_readable(file))
        return -EPERM;

    if (!buf || !count)
        return -EINVAL;

    return node->ops->read(file, buf, count);
}

static ssize_t devfs_write(file_t *file, const void *buf, size_t count)
{
    devfs_node_t *node = (devfs_node_t *)file->node->i_arg;

    if (!node->ops->write)
        return -ENOTSUP;

    if (!devfs_file_is_writable(file))
        return -EPERM;

    if (!buf || !count)
        return -EINVAL;

    return node->ops->write(file, buf, count);
}

static int devfs_ioctl(file_t *file, int cmd, unsigned long arg)
{
    devfs_node_t *node = (devfs_node_t *)file->node->i_arg;

    if (!node->ops->ioctl)
        return -ENOTSUP;

    return node->ops->ioctl(file, cmd, arg);
}

extern unsigned long cpu_intrpt_save();
extern void cpu_intrpt_restore(unsigned long flags);

static int devfs_poll(file_t *file, int flag, poll_notify_t notify, void *fd, void *arg)
{
    devfs_node_t *node = (devfs_node_t *)file->node->i_arg;
    struct pollfd *poll_fd = (struct pollfd *)fd;
    devfs_poll_request_t req = 0;
    int revents;
    unsigned long flags;

    revents = node->ops->poll ? node->ops->poll(file, &req) : -ENOTSUP;
    revents = (revents < 0) ? POLLERR : revents;

    if (poll_fd) {
        if (poll_fd->events & revents & (POLLIN | POLLOUT | POLLERR)) {
            poll_fd->revents |= revents & (POLLIN | POLLOUT | POLLERR);
            notify(poll_fd, arg);
        } else {
            flags = cpu_intrpt_save();
            node->poll_notify = notify;
            node->poll_fd = poll_fd;
            node->poll_arg = arg;
            cpu_intrpt_restore(flags);
        }
    } else {
        flags = cpu_intrpt_save();
        if (node->poll_fd)
            node->poll_fd->revents |= revents & (POLLIN | POLLOUT | POLLERR);
        node->poll_notify = NULL;
        node->poll_fd = NULL;
        node->poll_arg = NULL;
        cpu_intrpt_restore(flags);
    }

    return 0;
}

static uint32_t devfs_lseek(file_t *file, int64_t offset, int32_t whence)
{
    devfs_node_t *node = (devfs_node_t *)file->node->i_arg;
    int64_t pos;

    if (!node->ops->lseek)
        return (uint32_t)(-ENOTSUP);

    switch (whence) {
    case SEEK_SET:
    case SEEK_CUR:
    case SEEK_END:
        break;
    default:
        return (uint32_t)(-EINVAL);
    }

    pos = node->ops->lseek(file, offset, whence);
    if (pos < 0)
        return (uint32_t)(int32_t)pos;

    devfs_file_set_position(file, pos);

    return (uint32_t)devfs_file_get_position(file);
}

static file_ops_t devfs_fops = {
    .open       = devfs_open,
    .close      = devfs_close,
    .read       = devfs_read,
    .write      = devfs_write,
    .ioctl      = devfs_ioctl,
    .poll       = devfs_poll,
    .lseek      = devfs_lseek,
#ifdef AOS_PROCESS_SUPPORT
    .mmap       = NULL,
#endif
};

int devfs_add_node(devfs_node_t *node)
{
    char path[sizeof(path_prefix) - 1 + DEVFS_NODE_NAME_MAX_LEN + 1];
    int path_len;

    if (!node || !devfs_node_is_valid(node))
        return -EINVAL;

    path_len = snprintf(path, sizeof(path), "%s%s", path_prefix, node->name);
    if (path_len < 0 || path_len >= sizeof(path))
        return -EINVAL;

    // aos_spin_lock_init(&node->poll_lock);
    node->poll_notify = NULL;
    node->poll_fd = NULL;
    node->poll_arg = NULL;

    return vfs_register_driver(path, (vfs_file_ops_t *)&devfs_fops, node);
}

int devfs_remove_node(devfs_node_t *node)
{
    char path[sizeof(path_prefix) - 1 + DEVFS_NODE_NAME_MAX_LEN + 1];
    int path_len;

    if (!node || !devfs_node_is_valid(node))
        return -EINVAL;

    path_len = snprintf(path, sizeof(path), "%s%s", path_prefix, node->name);
    if (path_len < 0 || path_len >= sizeof(path))
        return -EINVAL;

    (void)vfs_unregister_driver(path);

    return 0;
}

void *devfs_file2dev(devfs_file_t *file)
{
    rvm_dev_t *dev = (rvm_dev_t *)file->f_arg;

    return dev;
}

mode_t devfs_file_get_mode(devfs_file_t *file)
{
    return file->node->i_flags;
}

uint64_t devfs_file_get_position(devfs_file_t *file)
{
    return file->offset;
}

void devfs_file_set_position(devfs_file_t *file, uint64_t pos)
{
    file->offset = pos;
}

void devfs_poll_add(devfs_node_t *node, devfs_wait_queue_t wq, devfs_poll_request_t *req)
{
}

void devfs_poll_wakeup(devfs_node_t *node, devfs_wait_queue_t wq)
{
    unsigned long flags;

    flags = cpu_intrpt_save();
    if (node->poll_notify)
        node->poll_notify(node->poll_fd, node->poll_arg);
    node->poll_notify = NULL;
    node->poll_fd = NULL;
    node->poll_arg = NULL;
    cpu_intrpt_restore(flags);
}
