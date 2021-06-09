#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_
#include <stdint.h>
#include <stdbool.h>
#define FLASH_BASE_ADDR (0x18000000)
#define FLASH_PAGE_SIZE (256)

void spi_flash_xip_status_set(bool xip);

void spi_flash_writing_status_set(bool writing);

void spi_flash_drv_var_init(bool xip,bool writing);

void spi_flash_init(void);

void spi_flash_xip_start(void);

void spi_flash_xip_stop(void);

void spi_flash_read_status_register_0(uint8_t *status_reg_0);

void spi_flash_read_status_register_1(uint8_t *status_reg_1);

void spi_flash_write_status_register(uint16_t status);

void spi_flash_quad_page_program(uint32_t offset,uint8_t *data,uint16_t length);

void spi_flash_page_program(uint32_t offset,uint8_t *data,uint16_t length);

void spi_flash_page_erase(uint32_t offset);

void spi_flash_sector_erase(uint32_t offset);

void spi_flash_chip_erase(void);

void spi_flash_quad_io_read(uint32_t offset,uint8_t *data,uint16_t length);

void spi_flash_fast_read(uint32_t offset,uint8_t *data,uint16_t length);

void spi_flash_deep_power_down(void);

void spi_flash_release_from_deep_power_down(void);

void spi_flash_read_id(uint8_t *manufacturer_id,uint8_t *mem_type_id,uint8_t *capacity_id);

void spi_flash_read_unique_id(uint8_t unique_serial_id[16]);

void spi_flash_erase_security_area(uint8_t idx);

void spi_flash_program_security_area(uint8_t idx,uint16_t addr,uint8_t *data,uint16_t length);

void spi_flash_read_security_area(uint8_t idx,uint16_t addr,uint8_t *data,uint16_t length);

void spi_flash_software_reset(void);

void spi_flash_qe_status_read_and_set(void);

void spi_flash_prog_erase_suspend(void);

void spi_flash_prog_erase_resume(void);

bool spi_flash_writing_busy(void);

bool spi_flash_xip_status_get(void);

#endif

