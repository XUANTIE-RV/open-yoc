#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "aos/hal/flash.h"
#include "spi_flash.h"
#include "reg_lscache.h"
#include "compile_flag.h"
#ifndef CONFIG_KERNEL_NONE
#include <k_default_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_sys.h>
#include <k_list.h>
#include <k_ringbuf.h>
#include <k_obj.h>
#include <k_sem.h>
#include <k_queue.h>
#include <k_buf_queue.h>
#include <k_stats.h>
#include <k_time.h>
#include <k_task.h>
#include <port.h>
#include "k_critical.h"
#endif

#define FLASH_SECTOR_SIZE (4096)

static const hal_logic_partition_t *hal_partitions(hal_partition_t in_partition)
{
    static const hal_logic_partition_t all = {
                .partition_owner = HAL_FLASH_SPI,
                .partition_description = NULL,
                .partition_start_addr = FLASH_BASE_ADDR,
                .partition_length = 0x1000,
                .partition_options = 128,
            };
    static const hal_logic_partition_t app = {
                .partition_owner = HAL_FLASH_SPI,
                .partition_description = "prim",
                .partition_start_addr = 0x1800b000,
                //.partition_length = 0x35000,
                .partition_length = 0x28000,
                .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
            };
    static const hal_logic_partition_t otp = {
                .partition_owner = HAL_FLASH_SPI,
                .partition_description = "otp",
                .partition_start_addr = 0x1807f000,
                .partition_length = 0x1000, //4k bytes
                .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
            };
    static const hal_logic_partition_t kv = {
                .partition_owner = HAL_FLASH_SPI,
                .partition_description = "kv",
                .partition_start_addr = 0x18007000,
                .partition_length = 0x2000,
                .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
            };
    static const hal_logic_partition_t misc = {
                .partition_owner = HAL_FLASH_SPI,
                .partition_description = "misc",
                .partition_start_addr = 0x18033000,
                .partition_length = 0x2b000,
                //.partition_start_addr = 0x18040000,
                //.partition_length = 0x1e000,
                .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
            };
    const hal_logic_partition_t *ptr;
    switch(in_partition)
    {
    case HAL_ALL_FLASH_0:
        ptr = &all;
    break;
    case HAL_PARTITION_APPLICATION:
        ptr = &app;
    break;
    case HAL_PARTITION_BT_FIRMWARE:
        ptr = &otp;
    break;
    case HAL_PARTITION_PARAMETER_2:
        ptr = &kv;
    break;
    case HAL_PARTITION_OTA_TEMP:
        ptr = &misc;
    break;
    default:
        ptr = NULL;
    break;  
    }
    return ptr;
}

int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    *partition = *hal_partitions(in_partition);
    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    uint32_t erase_offset = 0;

    if (in_partition != HAL_ALL_FLASH_0)
    {
        if (off_set + size > hal_partitions(in_partition)->partition_length)
        {
            return -1;
        }
    }

    erase_offset = hal_partitions(in_partition)->partition_start_addr + off_set - FLASH_BASE_ADDR;

    while (size > 0)
    {
        spi_flash_sector_erase(erase_offset);

        if (size > FLASH_SECTOR_SIZE)
        {
            size -= FLASH_SECTOR_SIZE;
            erase_offset += FLASH_SECTOR_SIZE;
        }
        else
        {
            break;
        }
    };

    return 0;
}

XIP_BANNED void cache_restart()
{
    #ifndef CONFIG_KERNEL_NONE
    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();
    /*
    *(volatile uint32_t *)0x18000000;
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");

    *(volatile uint32_t *)0x18000000;
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    */
    LSCACHE->CCR = 0;
    LSCACHE->CCR = 1;
    while(LSCACHE->SR&0x4);
    /*
    *(volatile uint32_t *)0x18000000;
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    */
    RHINO_CRITICAL_EXIT();
    #endif
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_size)
{
    if(!in_buf)
    {
        return -1;
    }
    uint32_t current = *off_set;
    uint16_t length;
    if(current % 256)
    {
        length = in_buf_size > 256 - current % 256 ? 256 - current % 256 : in_buf_size;
    }else
    {
        length = 0;
    }
    if(length)
    {
        spi_flash_quad_page_program(hal_partitions(in_partition)->partition_start_addr + current - FLASH_BASE_ADDR,(void *)in_buf,length);
        in_buf_size -= length;
        current += length;
        in_buf = (uint8_t *)in_buf + length; 
    }
    while(in_buf_size)
    {
        length = in_buf_size > 256 ? 256 : in_buf_size;
        spi_flash_quad_page_program(hal_partitions(in_partition)->partition_start_addr + current - FLASH_BASE_ADDR,(void *)in_buf,length);
        in_buf_size -= length;
        current += length;
        in_buf = (uint8_t *)in_buf + length; 
    }
    *off_set = current;
    cache_restart();
    return 0;
}

int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_size)
{
    int32_t ret = hal_flash_erase(in_partition,*off_set,in_buf_size);
    if(ret == 0)
    {
        ret = hal_flash_write(in_partition,off_set,in_buf,in_buf_size);
    }
    return ret;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t out_buf_size)
{
    uint32_t current = *off_set;
    spi_flash_quad_io_read(hal_partitions(in_partition)->partition_start_addr + current - FLASH_BASE_ADDR,out_buf,out_buf_size);
    current += out_buf_size;
    *off_set = current;
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

void hal_flash_init()
{

}