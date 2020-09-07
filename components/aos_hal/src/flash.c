/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/eflash.h>
#include <drv/spiflash.h>

#include "soc.h"
#include "aos/hal/flash.h"

#define EFLASH_SECTOR_SIZE 512

#ifndef CONFIG_FIX_ALIOS_THINGS_KV
#define CONFIG_FIX_ALIOS_THINGS_KV 0
#endif

extern const hal_logic_partition_t hal_partitions[];
static eflash_handle_t eflash_handle;
static spiflash_handle_t spiflash_handle;

#if CONFIG_FIX_ALIOS_THINGS_KV
int g_erase_buf[EFLASH_SECTOR_SIZE / 4];
#endif

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];
    memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));

    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    int i;
    uint32_t addr;
    hal_logic_partition_t partition_info;
    hal_flash_t owner;

    if (hal_flash_info_get(in_partition, &partition_info) != 0) {
        return -1;
    }

    addr = partition_info.partition_start_addr + off_set;
    owner = partition_info.partition_owner;


    if (owner == HAL_FLASH_EMBEDDED) {
#if CONFIG_FIX_ALIOS_THINGS_KV
        memset(g_erase_buf, 0xFF, EFLASH_SECTOR_SIZE);
#endif
        for (i = 0; i < size / EFLASH_SECTOR_SIZE; i++) {
            csi_eflash_erase_sector(eflash_handle, addr + EFLASH_SECTOR_SIZE * i);
#if CONFIG_FIX_ALIOS_THINGS_KV
            csi_eflash_program(eflash_handle, addr + EFLASH_SECTOR_SIZE * i, g_erase_buf, EFLASH_SECTOR_SIZE);
#endif
        }
    } else if (owner == HAL_FLASH_QSPI || owner == HAL_FLASH_SPI) {
        spiflash_info_t *info = csi_spiflash_get_info(spiflash_handle);
        uint32_t sector_size = info->sector_size;
        for (i = 0; i < size / sector_size; i++) {
            csi_spiflash_erase_sector(spiflash_handle, addr);
            addr += sector_size;
        }
    }

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
    uint32_t addr;
    hal_logic_partition_t partition_info;
    hal_flash_t owner;

    if (hal_flash_info_get(in_partition, &partition_info) != 0) {
        return -1;
    }

    addr = partition_info.partition_start_addr + *off_set;
    owner = partition_info.partition_owner;

    if (owner == HAL_FLASH_EMBEDDED) {
#if CONFIG_FIX_ALIOS_THINGS_KV
        memset(g_erase_buf, 0xFF, EFLASH_SECTOR_SIZE);
        if ((addr + in_buf_len) <= ((addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE)) {
            csi_eflash_read(eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);
            memcpy((void *)((uint32_t)g_erase_buf + addr % EFLASH_SECTOR_SIZE), in_buf, in_buf_len);
            csi_eflash_erase_sector(eflash_handle, addr & 0xFFFFFE00);
            csi_eflash_program(eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);
        } else {
            uint32_t remain_len = addr + in_buf_len - ((addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE);
            uint32_t len = in_buf_len - remain_len;

            csi_eflash_read(eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);
            memcpy((void *)((uint32_t)g_erase_buf + addr % EFLASH_SECTOR_SIZE), in_buf, len);
            csi_eflash_erase_sector(eflash_handle, addr & 0xFFFFFE00);
            csi_eflash_program(eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);

            memset(g_erase_buf, 0xFF, EFLASH_SECTOR_SIZE);
            csi_eflash_read(eflash_handle, (addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE, g_erase_buf, EFLASH_SECTOR_SIZE);
            memcpy((void *)g_erase_buf, (void *)((uint32_t)in_buf + len), remain_len);
            csi_eflash_erase_sector(eflash_handle, (addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE);
            csi_eflash_program(eflash_handle, (addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE, g_erase_buf, EFLASH_SECTOR_SIZE);
        }
#else
        csi_eflash_program(eflash_handle, addr, in_buf, in_buf_len);
#endif
    } else if (owner == HAL_FLASH_QSPI || owner == HAL_FLASH_SPI) {
        csi_spiflash_program(spiflash_handle, addr, in_buf, in_buf_len);
    }

    return 0;
}

int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
    hal_flash_erase(in_partition, *off_set, in_buf_len);
    hal_flash_write(in_partition, off_set, in_buf , in_buf_len);

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t addr;
    hal_logic_partition_t partition_info;
    hal_flash_t owner;

    if (hal_flash_info_get(in_partition, &partition_info) != 0) {
        return -1;
    }

    addr = partition_info.partition_start_addr + *off_set;
    owner = partition_info.partition_owner;

    if (owner == HAL_FLASH_EMBEDDED) {
        csi_eflash_read(eflash_handle, addr, out_buf, out_buf_len);
    } else if (owner == HAL_FLASH_QSPI || owner == HAL_FLASH_SPI) {
        csi_spiflash_read(spiflash_handle, addr, out_buf, out_buf_len);
    }

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

void hal_flash_init(void)
{
    eflash_handle = csi_eflash_initialize(0, NULL);

    spiflash_handle = csi_spiflash_initialize(0, NULL);
}
