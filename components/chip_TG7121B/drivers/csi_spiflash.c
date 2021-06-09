#include <drv/spiflash.h>
#include "spi_flash.h"

const struct spiflash_instance
{
    spiflash_info_t info;
}spiflash_inst = {
    .info = {
        .start = 0x18000000,              ///< Chip Start address
        .end = 0x1807ffff,                ///< Chip End address (start+size-1)
        .sector_count = 128,       ///< Number of sectors
        .sector_size = 0x1000,        ///< Uniform sector size in bytes (0=sector_info used)
        .page_size = 256,          ///< Optimal programming page size in bytes
        .program_unit = 1,       ///< Smallest programmable unit in bytes
        .erased_value = 0xff,       ///< Contents of erased memory (usually 0xFF)
    },
};
spiflash_event_cb_t spiflash_evt_cb;

spiflash_handle_t csi_spiflash_initialize(int32_t idx, spiflash_event_cb_t cb_event)
{
    spiflash_evt_cb = cb_event;
    return (spiflash_handle_t)&spiflash_inst;
}

int32_t csi_spiflash_uninitialize(spiflash_handle_t handle)
{
    return 0;
}

int32_t csi_spiflash_read(spiflash_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
{
    spi_flash_quad_io_read(addr - FLASH_BASE_ADDR,data,cnt);
    return 0;
}

int32_t csi_spiflash_program(spiflash_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
{
    uint32_t current = addr;
    uint16_t length;
    if(current % 256)
    {
        length = cnt > 256 - current % 256 ? 256 - current % 256 : cnt;
    }else
    {
        length = 0;
    }
    if(length)
    {
        spi_flash_quad_page_program(current - FLASH_BASE_ADDR,(void *)data,length);
        cnt -= length;
        current += length;
        data = (uint8_t *)data + length; 
    }
    while(cnt)
    {
        length = cnt > 256 ? 256 : cnt;
        spi_flash_quad_page_program(current - FLASH_BASE_ADDR,(void *)data,length);
        cnt -= length;
        current += length;
        data = (uint8_t *)data + length; 
    }
    return 0;
}

int32_t csi_spiflash_erase_sector(spiflash_handle_t handle, uint32_t addr)
{
    spi_flash_sector_erase(addr - FLASH_BASE_ADDR);
    return 0;
}

int32_t csi_spiflash_power_down(spiflash_handle_t handle)
{
    spi_flash_deep_power_down();
    return 0;
}

int32_t csi_spiflash_release_power_down(spiflash_handle_t handle)
{
    spi_flash_release_from_deep_power_down();
    return 0;
}

spiflash_info_t *csi_spiflash_get_info(spiflash_handle_t handle)
{
    return (spiflash_info_t *)&spiflash_inst.info;
}
