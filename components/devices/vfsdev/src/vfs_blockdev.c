/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/blockdev.h>
#include <string.h>
#include <inttypes.h>
#include <devices/impl/blockdev_impl.h>

#define BLOCKDEV_DRIVER(dev)  ((blockdev_driver_t*)(dev->drv))

static aos_status_t _devfs_blkdev_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    // printf("%s, %d, arg:0x%lx\n", __func__, __LINE__, arg);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case BLOCKDEV_IOC_GET_INFO:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            return -EPERM;
        }
        rvm_hal_blockdev_info_t blkdev_info;
        if (rvm_hal_blockdev_get_info(dev, &blkdev_info)) {
            ret = -EIO;
            break;
        }
        memcpy((void *)arg, &blkdev_info, sizeof(rvm_hal_blockdev_info_t));
        break;
    }
    case BLOCKDEV_IOC_ERASE:
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
        uint32_t blkcnt = *(uintptr_t *)arg;
        if (rvm_hal_blockdev_erase_blks(dev, offset, blkcnt)) {
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

ssize_t _devfs_blkdev_write(devfs_file_t *file, const void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    size_t offset = devfs_file_get_position(file);

    // printf("%s, %d, offset:0x%"PRIX64", count:%"PRId64"\n", __func__, __LINE__, offset, count);

    if (!devfs_file_is_writable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_blockdev_write_blks(dev, (void *)buf, offset, count)) {
        return -EIO;
    }

    devfs_file_set_position(file, offset + count);

    return count;
}

ssize_t _devfs_blkdev_read(devfs_file_t *file, void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    size_t offset = devfs_file_get_position(file);

    // printf("%s, %d, offset:0x%"PRIX64", count:%"PRId64"\n", __func__, __LINE__, offset, count);

    if (!devfs_file_is_readable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_blockdev_read_blks(dev, buf, offset, count)) {
        return -EIO;
    }

    devfs_file_set_position(file, offset + count);

    return count;
}

static int64_t _devfs_blkdev_lseek(devfs_file_t *file, int64_t offset, int whence)
{
    uint64_t size;
    rvm_dev_t *dev = devfs_file2dev(file);
    rvm_hal_blockdev_info_t blkdev_info;
    if (rvm_hal_blockdev_get_info(dev, &blkdev_info)) {
        return -EIO;
    }
    size = blkdev_info.user_area_blks;
#if defined(CONFIG_COMP_SDMMC)
    rvm_hal_mmc_access_area_t cur_area;
    if (BLOCKDEV_DRIVER(dev)->current_area) {
        if (BLOCKDEV_DRIVER(dev)->current_area(dev, &cur_area)) {
            return -EIO;
        }
        if (cur_area == MMC_AccessPartitionBoot1 || cur_area == MMC_AccessPartitionBoot2) {
            size = blkdev_info.boot_area_blks;
        }
    }
#endif
    // printf("%s, %d, offset:0x%"PRIX64", size:%"PRId64"\n", __func__, __LINE__, offset, size);
    return devfs_file_lseek_sized(file, size, offset, whence);
}

static const devfs_file_ops_t devfs_blckdev_ops = {
    .ioctl      = _devfs_blkdev_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = _devfs_blkdev_read,
    .write      = _devfs_blkdev_write,
    .lseek      = _devfs_blkdev_lseek,
};

static devfs_ops_node_t mmc_ops_node = {
    .name = "mmc",
    .ops = (devfs_file_ops_t *)&devfs_blckdev_ops,
};
void mmc_devfs_init(void)
{
    devices_add_devfs_ops_node(&mmc_ops_node);
}
VFS_DEV_DRIVER_ENTRY(mmc_devfs_init);

static devfs_ops_node_t sd_ops_node = {
    .name = "sd",
    .ops = (devfs_file_ops_t *)&devfs_blckdev_ops,
};
void sd_devfs_init(void)
{
    devices_add_devfs_ops_node(&sd_ops_node);
}
VFS_DEV_DRIVER_ENTRY(sd_devfs_init);

static devfs_ops_node_t usb_ops_node = {
    .name = "usb_mass",
    .ops = (devfs_file_ops_t *)&devfs_blckdev_ops,
};
void usb_devfs_init(void)
{
    devices_add_devfs_ops_node(&usb_ops_node);
}
VFS_DEV_DRIVER_ENTRY(usb_devfs_init);

#endif