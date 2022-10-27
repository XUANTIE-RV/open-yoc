#include "boot_flash_porting.h"
#include <yoc/partition_flash.h>
#include <yoc/partition.h>

int boot_flash_info_get(int flashid, boot_flash_info_t *info)
{
    partition_flash_info_t flash_info;

    if (info) {
        void *handle = partition_flash_open(flashid);
        partition_flash_info_get(handle, &flash_info);
        partition_flash_close(handle);
        info->sector_count = flash_info.sector_count;
        info->sector_size = flash_info.sector_size;
        info->start_addr = flash_info.start_addr;
        return 0;
    }
    
    return -1;
}

int boot_flash_read(unsigned long addr, void *data, size_t data_len)
{
    int ret;
    int flashid = 0;

#if CONFIG_MULTI_FLASH_SUPPORT
    flashid = get_flashid_by_abs_addr(addr);
#endif
    void *handle = partition_flash_open(flashid);
    ret = partition_flash_read(handle, addr, data, data_len);
    partition_flash_close(handle);
    return ret;
}

int boot_flash_write(unsigned long addr, void *data, size_t data_len)
{
    int ret;
    int flashid = 0;

#if CONFIG_MULTI_FLASH_SUPPORT
    flashid = get_flashid_by_abs_addr(addr);
#endif
    void *handle = partition_flash_open(flashid);
    ret = partition_flash_write(handle, addr, data, data_len);
    partition_flash_close(handle);
    return ret;
}

int boot_flash_erase(unsigned long addr, size_t len)
{
    int ret;
    int flashid = 0;

#if CONFIG_MULTI_FLASH_SUPPORT
    flashid = get_flashid_by_abs_addr(addr);
#endif
    void *handle = partition_flash_open(flashid);
    ret = partition_flash_erase(handle, addr, len);
    partition_flash_close(handle);
    return ret;
}