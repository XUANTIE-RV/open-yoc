/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "key_mgr_port.h"
#include <sec_crypto_sha.h>
#include <yoc/partition_device.h>

int get_data_from_addr(unsigned long addr, uint8_t *data, size_t data_len, partition_info_t *part_info)
{
#ifdef CONFIG_NON_ADDRESS_FLASH
    void *handle = partition_device_find(&part_info->storage_info);
    return partition_device_read(handle, addr - part_info->base_addr, data, data_len);
#else
    memcpy(data, (uint8_t *)addr, data_len);
#endif /* CONFIG_NON_ADDRESS_FLASH */

    return 0;
}

