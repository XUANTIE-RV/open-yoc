/*
 * Copyright (C) 2019 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/gnss.h>
#include <string.h>

static aos_status_t _devfs_gnss_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case GNSS_IOC_GETINFO:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_gnss_info_t info;
        memcpy(&info, (void *)arg, sizeof(rvm_hal_gnss_info_t));
        rvm_hal_gnss_getinfo(dev, &info);
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const devfs_file_ops_t devfs_gnss_ops = {
    .ioctl      = _devfs_gnss_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t gnss_ops_node = {
    .name = "gnss",
    .ops = (devfs_file_ops_t *)&devfs_gnss_ops,
};

void gnss_devfs_init(void)
{
    devices_add_devfs_ops_node(&gnss_ops_node);
}
VFS_DEV_DRIVER_ENTRY(gnss_devfs_init);

#endif