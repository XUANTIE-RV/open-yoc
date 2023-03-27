#include "boot_flash_porting.h"
#include <yoc/partition_device.h>
#include <yoc/partition.h>

int boot_device_info_get(storage_info_t *storage_info, boot_device_info_t *info)
{
    partition_device_info_t flash_info;

    if (info && storage_info) {
        void *handle = partition_device_find(storage_info);
        if (partition_device_info_get(handle, &flash_info)) {
            return -1;
        }
        info->sector_size = flash_info.sector_size;
        info->block_size = flash_info.block_size;
        info->erase_size = flash_info.erase_size;
        info->device_size = flash_info.device_size;
        info->base_addr = flash_info.base_addr;
        return 0;
    }
    
    return -1;
}

int boot_device_read(storage_info_t *storage_info, uint32_t offset, void *data, size_t data_len)
{
    int ret;

    void *handle = partition_device_find(storage_info);
    ret = partition_device_read(handle, offset, data, data_len);
    return ret;
}

int boot_device_write(storage_info_t *storage_info, uint32_t offset, void *data, size_t data_len)
{
    int ret;

    void *handle = partition_device_find(storage_info);
    ret = partition_device_write(handle, offset, data, data_len);
    return ret;
}

int boot_device_erase(storage_info_t *storage_info, uint32_t offset, size_t len)
{
    int ret;

    void *handle = partition_device_find(storage_info);
    ret = partition_device_erase(handle, offset, len);
    return ret;
}