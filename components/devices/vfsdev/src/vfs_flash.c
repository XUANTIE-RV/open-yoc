/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/flash.h>
#include <string.h>

static aos_status_t _devfs_flash_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case FLASH_IOC_GET_INFO:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            return -EPERM;
        }
        rvm_hal_flash_dev_info_t flash_info;
        if (rvm_hal_flash_get_info(dev, &flash_info)) {
            ret = -EIO;
            break;
        }
        memcpy((void *)arg, &flash_info, sizeof(rvm_hal_flash_dev_info_t));
        break;
    }
    case FLASH_IOC_ERASE:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        size_t offset = devfs_file_get_position(file);
        int32_t sector_cnt = *(int32_t *)arg;
        if (rvm_hal_flash_erase(dev, offset, sector_cnt)) {
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

ssize_t _devfs_flash_write(devfs_file_t *file, const void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    size_t offset = devfs_file_get_position(file);
    // printf("%s, %d, offset:0x%x, count:%d\n", __func__, __LINE__, offset, count);

    if (!devfs_file_is_writable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_flash_program(dev, offset, buf, count)) {
        return -EIO;
    }

    devfs_file_set_position(file, offset + count);

    return count;
}

ssize_t _devfs_flash_read(devfs_file_t *file, void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    size_t offset = devfs_file_get_position(file);
    // printf("%s, %d, offset:0x%x, count:%d\n", __func__, __LINE__, offset, count);

    if (!devfs_file_is_readable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_flash_read(dev, offset, buf, count)) {
        return -EIO;
    }

    devfs_file_set_position(file, offset + count);

    return count;
}

static int64_t _devfs_flash_lseek(devfs_file_t *file, int64_t offset, int whence)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    rvm_hal_flash_dev_info_t flash_info;
    if (rvm_hal_flash_get_info(dev, &flash_info)) {
        return -EIO;
    }
    // printf("%s, %d, offset:0x%"PRIX64", sector_size:0x%x, sector_count:%d\n", __func__, __LINE__, offset, flash_info.sector_size, flash_info.sector_count);
    return devfs_file_lseek_sized(file, flash_info.sector_size * flash_info.sector_count, offset, whence);
}

static const devfs_file_ops_t devfs_flash_ops = {
    .ioctl      = _devfs_flash_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = _devfs_flash_read,
    .write      = _devfs_flash_write,
    .lseek      = _devfs_flash_lseek,
};

static devfs_ops_node_t flash_ops_node = {
    .name = "flash",
    .ops = (devfs_file_ops_t *)&devfs_flash_ops,
};

void flash_devfs_init(void)
{
    devices_add_devfs_ops_node(&flash_ops_node);
}
VFS_DEV_DRIVER_ENTRY(flash_devfs_init);

#endif