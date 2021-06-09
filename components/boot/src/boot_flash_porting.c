#include "boot_flash_porting.h"
#include <yoc/partition_flash.h>

int boot_flash_info_get(boot_flash_info_t *info)
{
    partition_flash_info_t flash_info;

    if (info) {
        partition_flash_info_get(NULL, &flash_info);
        info->sector_count = flash_info.sector_count;
        info->sector_size = flash_info.sector_size;
        info->start_addr = flash_info.start_addr;
        return 0;
    }
    
    return -1;
}

int boot_flash_read(uint32_t addr, void *data, size_t data_len)
{
    return partition_flash_read(NULL, addr, data, data_len);
}

int boot_flash_write(uint32_t addr, void *data, size_t data_len)
{
    return partition_flash_write(NULL, addr, data, data_len);
}

int boot_flash_erase(uint32_t addr, size_t len)
{
    return partition_flash_erase(NULL, addr, len);
}