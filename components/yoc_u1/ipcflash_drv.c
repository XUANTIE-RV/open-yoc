/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#include <aos/debug.h>
#include <aos/kernel.h>
#include <hostfs.h>
#include <soc.h>
#include <string.h>
#include <vfs.h>

#include "hal/flash_impl.h"

#define IPCFLASH_PATH  "/data"
#define IPCFLASH_FILE  "/data/flash.bin"

#define IPCFLASH_TAG   "ipcflash"

#define START_ADDR     0
#define END_ADDR       CONFIG_IPCFLASH_SIZE
#define SECTOR_SZ      512
#define ERASE_VAL      0xFF

typedef struct {
    uint32_t          start;              ///< Chip Start address
    uint32_t          end;                ///< Chip End address (start+size-1)
    uint32_t          sector_count;       ///< Number of sectors
    uint32_t          sector_size;        ///< Uniform sector size in bytes (0=sector_info used)
    uint8_t           erased_value;       ///< Contents of erased memory (usually 0xFF)
} flash_info_t;

typedef struct {
    aos_dev_t         device;
    int               fd;
    flash_info_t      info;
} flash_dev_t;

static aos_dev_t *yoc_ipcflash_init(driver_t *drv,void *config, int id)
{
    flash_dev_t *dev = (flash_dev_t*)device_new(drv, sizeof(flash_dev_t), id);
    int ret;

    dev->info.start = START_ADDR;
    dev->info.end   = END_ADDR;
    dev->info.sector_count = (END_ADDR - START_ADDR + 1) / SECTOR_SZ;
    dev->info.sector_size = SECTOR_SZ;
    dev->info.erased_value = ERASE_VAL;

    ret = hostfs_register(IPCFLASH_PATH);
    if (ret < 0) {
        device_free((aos_dev_t*)dev);
        return NULL;
    }

    LOGI(IPCFLASH_TAG, "Ipc flash init\n");

    return (aos_dev_t*)dev;
}

void yoc_ipcflash_uninit(aos_dev_t *dev)
{
    hostfs_unregister(IPCFLASH_PATH);
    device_free(dev);
}

static int yoc_ipcflash_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static int yoc_ipcflash_open(aos_dev_t *dev)
{
    int ret, flags;
    struct stat st;
    flash_dev_t *flash = (flash_dev_t*)dev;

    ret = aos_stat(IPCFLASH_FILE, &st);
    if (ret)
        flags = O_CREAT | O_TRUNC | O_RDWR;
    else
        flags = O_RDWR;
    flash->fd = aos_open(IPCFLASH_FILE, flags);
    if (flash->fd < 0) {
        LOGE(IPCFLASH_TAG, "Failed to open ipc flash!\n");
        return flash->fd;
    }

    return 0;
}

static int yoc_ipcflash_close(aos_dev_t *dev)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    aos_close(flash->fd);

    return 0;
}

static int yoc_ipcflash_read(aos_dev_t *dev, uint32_t addroff, void *buff, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    char *buf = (char *)buff;
    ssize_t sz = 0;
    off_t pos;

    if ((addroff > END_ADDR) || (addroff + bytesize > END_ADDR)) {
        LOGE(IPCFLASH_TAG, "read: out of range!\n");
        return -ERANGE;
    }

    pos = aos_lseek(flash->fd, addroff, SEEK_SET);
    if (pos < 0) {
        LOGE(IPCFLASH_TAG, "read: Failed to seek to pos: %d\n", addroff);
        return -EIO;
    }

    while (bytesize > 0) {
        ssize_t tmp = aos_read(flash->fd, buf + sz, bytesize);
        if (tmp < 0) {
            LOGE(IPCFLASH_TAG, "read: Failed to read flash!\n");
            return -EIO;
        } else if (tmp == 0) {
            /* set the remain buf to erased_value, and write to flash */
            memset(buf + sz, flash->info.erased_value, bytesize);
            aos_write(flash->fd, buf + sz, bytesize);
            break;
        }
        sz += tmp;
        bytesize -= (int32_t)tmp;
    }

    return 0;
}

static int yoc_ipcflash_program(aos_dev_t *dev, uint32_t dstaddr, const void *srcbuf, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    const char *buf = (const char *)srcbuf;
    ssize_t sz = 0;
    off_t pos;

    if ((dstaddr > END_ADDR) || (dstaddr + bytesize > END_ADDR)) {
        LOGE(IPCFLASH_TAG, "write: out of range!\n");
        return -ERANGE;
    }

    pos = aos_lseek(flash->fd, dstaddr, SEEK_SET);
    if (pos < 0) {
        LOGE(IPCFLASH_TAG, "write: Failed to seek to pos: %d\n", dstaddr);
        return -EIO;
    }

    while (bytesize > 0) {
        ssize_t tmp = aos_write(flash->fd, buf + sz, bytesize);
        if (tmp < 0) {
            LOGE(IPCFLASH_TAG, "write: Failed to read flash!\n");
            return -EIO;
        }
        sz += tmp;
        bytesize -= (int32_t)tmp;
    }

    aos_sync(flash->fd);

    return 0;
}

static int yoc_ipcflash_erase(aos_dev_t *dev, int32_t addroff, int32_t blkcnt)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    uint8_t buf[SECTOR_SZ];
    off_t pos;
    int ret = 0;
    int i;

    if ((addroff > END_ADDR) || (addroff + blkcnt * flash->info.sector_size > END_ADDR)) {
        LOGE(IPCFLASH_TAG, "read: out of range!\n");
        return -ERANGE;
    }

    memset(buf, flash->info.erased_value, flash->info.sector_size);

    pos = aos_lseek(flash->fd, addroff, SEEK_SET);
    if (pos < 0) {
        LOGE(IPCFLASH_TAG, "erase: Failed to seek to pos: %d\n", addroff);
        return -EIO;
    }

    for (i = 0; i < blkcnt; i++) {
        ssize_t tmp = aos_write(flash->fd, buf, flash->info.sector_size);
        if (tmp < 0) {
            ret = -EIO;
            break;
        }
    }

    aos_sync(flash->fd);

    return ret;
}

static int yoc_ipcflash_get_info(aos_dev_t *dev, flash_dev_info_t *info)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    info->start_addr = flash->info.start;
    info->block_size = flash->info.sector_size;
    info->block_count = (flash->info.end - flash->info.start + 1) / flash->info.sector_size;

    return 0;
}

static flash_driver_t flash_driver = {
    .drv = {
        .name   = "eflash",
        .type   = "flash",
        .init   = yoc_ipcflash_init,
        .uninit = yoc_ipcflash_uninit,
        .lpm    = yoc_ipcflash_lpm,
        .open   = yoc_ipcflash_open,
        .close  = yoc_ipcflash_close,
    },
    .read       = yoc_ipcflash_read,
    .program    = yoc_ipcflash_program,
    .erase      = yoc_ipcflash_erase,
    .get_info   = yoc_ipcflash_get_info,
};

void ipcflash_csky_register(int idx)
{
    driver_register(&flash_driver.drv, NULL, idx);
}

void ipcflash_csky_unregister(int idx)
{
    char name[32];

    sprintf("%s%d", flash_driver.drv.name, idx);
    driver_unregister(name);
}
