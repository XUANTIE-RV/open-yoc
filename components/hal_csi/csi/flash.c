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

//extern const hal_logic_partition_t hal_partitions[];
/*===========implement for aiot ugly, fix me in the future===============*/
__attribute__((weak)) const hal_logic_partition_t hal_partitions[1];
static eflash_handle_t eflash_handle;
static spiflash_handle_t spiflash_handle;

#if CONFIG_FIX_ALIOS_THINGS_KV
int g_erase_buf[EFLASH_SECTOR_SIZE / 4];
#endif

static void _hal_flash_init(void)
{
    static int init;

    if (!init) {
        init = 1;
        eflash_handle = csi_eflash_initialize(0, NULL);
        spiflash_handle = csi_spiflash_initialize(0, NULL);
    }
}

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{

    _hal_flash_init();
    if (partition == NULL){
        return -1;
    }

    if (in_partition == HAL_ALL_FLASH_0) {
        if (eflash_handle != NULL) {
            eflash_info_t *flash_info;
            flash_info = csi_eflash_get_info(eflash_handle);

            //FIXME: structure multiplex, attention!
            partition->partition_start_addr = flash_info->start;
            partition->partition_length     = (flash_info->end - flash_info->start + 1);
            partition->partition_options    = (flash_info->end - flash_info->start + 1) / flash_info->sector_size;
        } else {
            spiflash_info_t  *info = csi_spiflash_get_info(spiflash_handle);
            
            partition->partition_start_addr = info->start;
            partition->partition_length     = info->sector_size * info->sector_count;
            partition->partition_options    = info->sector_count;
        }
    } else {
        hal_logic_partition_t *logic_partition;

        logic_partition = (hal_logic_partition_t *)&hal_partitions[in_partition];
        memcpy(partition, logic_partition, sizeof(hal_logic_partition_t));
    }

    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    int i;
    uint32_t addr,flash_size;
    hal_logic_partition_t partition_info;
    hal_flash_t owner;

    _hal_flash_init();

    if (in_partition == HAL_ALL_FLASH_0) {
        if (eflash_handle != NULL) {
            eflash_info_t *flash_info;
            flash_info = csi_eflash_get_info(eflash_handle);
            int ret;

            uint32_t sector_size = flash_info->sector_size;
            uint32_t erase_count = (size + sector_size - 1) / sector_size;
            addr = flash_info->start + off_set;

            for (i = 0; i < erase_count; i++) {
                ret = csi_eflash_erase_sector(eflash_handle, addr);
                if (ret != 0) {
                    return -1;
                }
                addr += sector_size;
            }
        } else {
            spiflash_info_t *info = csi_spiflash_get_info(spiflash_handle);
            uint32_t sector_size = info->sector_size;
            uint32_t erase_count = (size + sector_size - 1) / sector_size;
            int ret;

            addr = info->start + off_set;

            for (i = 0; i < erase_count; i++) {
                ret = csi_spiflash_erase_sector(spiflash_handle, addr);
                if (ret != 0) {
                    return -1;
                }
                addr += sector_size;
            }
        }
    } else {
        if (hal_flash_info_get(in_partition, &partition_info) != 0) {
            return -1;
        }

        addr = partition_info.partition_start_addr + off_set;
        owner = partition_info.partition_owner;
        flash_size = partition_info.partition_length;

        if(off_set + 1 > flash_size || size > flash_size || size + off_set > flash_size) {
            return -1;
        }
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
            uint32_t erase_count = (size + sector_size - 1) / sector_size;
            for (i = 0; i < erase_count; i++) {
                csi_spiflash_erase_sector(spiflash_handle, addr);
                addr += sector_size;
            }
        }        
    }

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_size)
{
    uint32_t addr,flash_size;
    hal_logic_partition_t partition_info;
    hal_flash_t owner;
    if (off_set == NULL){
        return -1;
    }

    if (in_buf == NULL){
        return -1;
    }
    
    _hal_flash_init();
    if (hal_flash_info_get(in_partition, &partition_info) != 0) {
        return -1;
    }

    if (in_partition == HAL_ALL_FLASH_0) {
        int ret;
        if (eflash_handle != NULL) {
            eflash_info_t *flash_info;
            flash_info = csi_eflash_get_info(eflash_handle);

            ret = csi_eflash_program(eflash_handle, *off_set + flash_info->start, in_buf, in_buf_size);
        } else {
            spiflash_info_t *info = csi_spiflash_get_info(spiflash_handle);

            ret = csi_spiflash_program(spiflash_handle, *off_set + info->start, in_buf, in_buf_size);
        }
        return ret;
    }

    addr = partition_info.partition_start_addr + *off_set;
    owner = partition_info.partition_owner;
    flash_size = partition_info.partition_length;

    if(*off_set + 1 > flash_size || in_buf_size > flash_size || in_buf_size + *off_set > flash_size) {
        return -1;
    }

    if (owner == HAL_FLASH_EMBEDDED) {
#if CONFIG_FIX_ALIOS_THINGS_KV
        memset(g_erase_buf, 0xFF, EFLASH_SECTOR_SIZE);
        if ((addr + in_buf_size) <= ((addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE)) {
            csi_eflash_read(eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);
            memcpy((void *)((uint32_t)g_erase_buf + addr % EFLASH_SECTOR_SIZE), in_buf, in_buf_size);
            csi_eflash_erase_sector(eflash_handle, addr & 0xFFFFFE00);
            csi_eflash_program(eflash_handle, addr & 0xFFFFFE00, g_erase_buf, EFLASH_SECTOR_SIZE);
        } else {
            uint32_t remain_len = addr + in_buf_size - ((addr & 0xFFFFFE00) + EFLASH_SECTOR_SIZE);
            uint32_t len = in_buf_size - remain_len;

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
        csi_eflash_program(eflash_handle, addr, in_buf, in_buf_size);
#endif
    } else if (owner == HAL_FLASH_QSPI || owner == HAL_FLASH_SPI) {
        csi_spiflash_program(spiflash_handle, addr, in_buf, in_buf_size);
    }

    return 0;
}

int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_size)
{
    uint32_t ret;

    _hal_flash_init();
    ret =  hal_flash_erase(in_partition, *off_set, in_buf_size);
    ret |=  hal_flash_write(in_partition, off_set, in_buf , in_buf_size);

    return ret;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_size)
{
    uint32_t addr, flash_size;
    hal_logic_partition_t partition_info;
    hal_flash_t owner;

    _hal_flash_init();
    if (in_partition == HAL_ALL_FLASH_0) {
        if (eflash_handle != NULL) {
            int ret;
            eflash_info_t *flash_info;
            flash_info = csi_eflash_get_info(eflash_handle);

            ret = csi_eflash_read(eflash_handle, *off_set + flash_info->start, out_buf, out_buf_size);
            if (ret != out_buf_size) {
                return -1;
            }
        } else {
            spiflash_info_t *info = csi_spiflash_get_info(spiflash_handle);

            int ret = csi_spiflash_read(spiflash_handle, *off_set + info->start, out_buf, out_buf_size);

            if (ret != out_buf_size) {
                return -1;
            }
        }
    } else {
        if (off_set == NULL){
            return -1;
        }

        if (out_buf == NULL){
            return -1;
        }

        if (hal_flash_info_get(in_partition, &partition_info) != 0) {
            return -1;
        }

        addr = partition_info.partition_start_addr + *off_set;
        owner = partition_info.partition_owner;
        flash_size = partition_info.partition_length;

        if(*off_set + 1 > flash_size || out_buf_size > flash_size || out_buf_size + *off_set > flash_size) {
            return -1;
        }

        if (owner == HAL_FLASH_EMBEDDED) {
            csi_eflash_read(eflash_handle, addr, out_buf, out_buf_size);
        } else if (owner == HAL_FLASH_QSPI || owner == HAL_FLASH_SPI) {
            csi_spiflash_read(spiflash_handle, addr, out_buf, out_buf_size);
        }        
    }

    return 0;
}


