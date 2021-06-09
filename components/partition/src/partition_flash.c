/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <errno.h>
#include <string.h>
#include "yoc/partition_flash.h"
#include "mtb_log.h"

#if defined(CONFIG_KERNEL_NONE)
#warning "Please implement flash operation interface."
void *part_flash_open(int id)
{
    (void)id;
    return NULL;
}

int part_flash_close(void *handle)
{
    (void)handle;
    return 0;
}

int part_flash_info_get(void *handle, partition_flash_info_t *info)
{
    return 0;
}

int part_flash_read(void *handle, uint32_t addr, void *data, size_t data_len)
{
    (void)handle;
    memcpy(data, (void *)addr, data_len);

    return 0;
}

int part_flash_write(void *handle, uint32_t addr, void *data, size_t data_len)
{
    (void)handle;

    return 0;
}

int part_flash_erase(void *handle, uint32_t addr, size_t len)
{
    (void)handle;

    return 0;
}

#else

#include "aos/hal/flash.h"

void *part_flash_open(int id)
{
    return (void*)HAL_ALL_FLASH_0;
}

int part_flash_close(void *handle)
{
    //FXIME
    return 0;
}

int part_flash_info_get(void *handle, partition_flash_info_t *info)
{
    int rc;
    hal_logic_partition_t partition;

    if (handle && info) {
        rc = hal_flash_info_get(HAL_ALL_FLASH_0, &partition);
        if (rc == 0) {
            //FIXME:
            info->start_addr   = partition.partition_start_addr;
            info->sector_size  = partition.partition_length;
            info->sector_count = partition.partition_options;
            MTB_LOGD("info->start_addr:0x%x", info->start_addr);
            MTB_LOGD("info->sector_size:0x%x", info->sector_size);
            MTB_LOGD("info->sector_count:0x%x", info->sector_count);
            return 0;
        }
    }
    return -EINVAL;
}

int part_flash_read(void *handle, uint32_t addr, void *data, size_t data_len)
{
    int rc, offset;

    if (handle && data && data_len > 0) {
        hal_logic_partition_t partition;

        rc = hal_flash_info_get(HAL_ALL_FLASH_0, &partition);
        if (rc == 0) {
            if (addr < partition.partition_start_addr) {
                return -EINVAL;
            }
            offset = addr - partition.partition_start_addr;

            return hal_flash_read(HAL_ALL_FLASH_0, (uint32_t*)&offset, data, data_len);
        }
    }
    return -EINVAL;
}

int part_flash_write(void *handle, uint32_t addr, void *data, size_t data_len)
{
    int rc, offset;

    if (handle && data && data_len > 0) {
        hal_logic_partition_t partition;

        rc = hal_flash_info_get(HAL_ALL_FLASH_0, &partition);
        if (rc == 0) {
            if (addr < partition.partition_start_addr) {
                return -EINVAL;
            }
            offset = addr - partition.partition_start_addr;

            return hal_flash_write(HAL_ALL_FLASH_0, (uint32_t*)&offset, data, data_len);
        }
    }
    return -EINVAL;
}

int part_flash_erase(void *handle, uint32_t addr, size_t len)
{
    int rc, offset;

    if (handle && len > 0) {
        hal_logic_partition_t partition;

        rc = hal_flash_info_get(HAL_ALL_FLASH_0, &partition);
        if (rc == 0) {
            if (addr < partition.partition_start_addr) {
                return -EINVAL;
            }
            offset = addr - partition.partition_start_addr;

            return hal_flash_erase(HAL_ALL_FLASH_0, offset, len);
        }
    }
    return -EINVAL;
}

#endif /* CONFIG_KERNEL_NONE */

static partition_flash_ops_t g_flash_ops = {
    .open     = part_flash_open,
    .close    = part_flash_close,
    .info_get = part_flash_info_get,
    .read     = part_flash_read,
    .write    = part_flash_write,
    .erase    = part_flash_erase
};

void partition_flash_register(partition_flash_ops_t *ops)
{
    if (ops != NULL) {
        memcpy(&g_flash_ops, ops, sizeof(partition_flash_ops_t));
    }
}

void *partition_flash_open(int id)
{
    if (g_flash_ops.open) {
        void *handle = g_flash_ops.open(id);
        return handle;
    }
    return NULL;
}

int partition_flash_close(void *handle)
{
    if (g_flash_ops.close) {
        return g_flash_ops.close(handle);
    }
    return -1;
}

int partition_flash_info_get(void *handle, partition_flash_info_t *info)
{
    if (g_flash_ops.info_get) {
        return g_flash_ops.info_get(handle, info);
    }
    return -1;
}

int partition_flash_read(void *handle, uint32_t addr, void *data, size_t data_len)
{
    int ret;

    ret = -1;
    if (g_flash_ops.read) {
        ret = g_flash_ops.read(handle, addr, data, data_len);
    }
    return ret;
}

int partition_flash_write(void *handle, uint32_t addr, void *data, size_t data_len)
{
    if (g_flash_ops.write)
        return g_flash_ops.write(handle, addr, data, data_len);
    return -1;
}

int partition_flash_erase(void *handle, uint32_t addr, size_t len)
{
    if (g_flash_ops.erase)
        return g_flash_ops.erase(handle, addr, len);
    return -1;
}
