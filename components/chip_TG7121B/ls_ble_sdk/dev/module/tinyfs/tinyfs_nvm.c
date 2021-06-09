#include "tinyfs_nvm.h"
#include "spi_flash.h"

void (* const nvm_program)(uint32_t,uint8_t *,uint16_t) = spi_flash_quad_page_program;
void (* const nvm_read)(uint32_t, uint8_t *, uint16_t) = spi_flash_quad_io_read;
void (* const nvm_sector_ease)(uint32_t) = spi_flash_sector_erase;
