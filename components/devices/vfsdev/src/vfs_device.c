/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <aos/kernel.h>
#include <devices/device.h>
#include <devices/vfs_device.h>

static slist_t devfs_node_list;

void devices_add_devfs_ops_node(const devfs_ops_node_t *ops_node)
{
    slist_add_tail((slist_t *)&ops_node->node, &devfs_node_list);
}

int devices_fs_init(rvm_dev_t *dev, const char *name, int id)
{
    devfs_file_ops_t *ops = NULL;

    if (name && dev) {
        devfs_ops_node_t *node;
        slist_for_each_entry(&devfs_node_list, node, devfs_ops_node_t, node) {
            if (strcmp(name, node->name) == 0) {
                ops = node->ops;
                break;
            }
        }

        if (ops) {
            devfs_node_init(&dev->devfs_node);
            dev->devfs_node.ops = ops;
            size_t name_len = snprintf(dev->devfs_node.name, sizeof(dev->devfs_node.name), "%s%d", name, id);
            if (name_len >= sizeof(dev->devfs_node.name))
                return -EINVAL;
            int ret = devfs_add_node(&dev->devfs_node);
            return ret;
        }
    }
    return -1;
}

int rvm_vfs_device_drv_init(OS_DRIVER_ENTRY drv_init_entry)
{
    if (drv_init_entry)
        drv_init_entry();
    return 0;
}
#endif
