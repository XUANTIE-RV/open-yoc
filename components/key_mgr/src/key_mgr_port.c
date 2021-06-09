/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "key_mgr_port.h"
#include <sec_crypto_sha.h>
#include <yoc/partition_flash.h>

uint32_t get_data_from_addr(uint32_t addr, uint8_t *data, uint32_t data_len)
{
#ifdef CONFIG_NON_ADDRESS_FLASH
    void *handle;

    handle = partition_flash_open(0);
    partition_flash_read(handle, addr, data, data_len);
    partition_flash_close(handle);
#else
    memcpy(data, (uint8_t *)addr, data_len);
#endif

    return 0;
}

uint32_t get_word_from_addr(uint32_t addr)
{
#ifdef CONFIG_NON_ADDRESS_FLASH
    void *   handle;
    uint32_t v;
    handle = partition_flash_open(0);
    partition_flash_read(handle, addr, (uint8_t *)&v, 4);
    partition_flash_close(handle);
    return v;
#else
    return *(uint32_t *)addr;
#endif
}
