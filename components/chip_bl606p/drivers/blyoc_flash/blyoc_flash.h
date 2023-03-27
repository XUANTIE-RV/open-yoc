#ifndef __BL_FLASH_H__
#define __BL_FLASH_H__
#include <stdint.h>

int bl_flash_init(void);
int bl_flash_deinit(void);

int bl_flash_clear_status_register(void);
int bl_flash_clear_status_register_for_winbond(void);

int bl_flash_erase(uint32_t addr, uint32_t len);
int bl_flash_write(uint32_t addr, void *src, uint32_t len);
int bl_flash_read(uint32_t addr, void *dst, uint32_t len);
int bl_flash_config_update(void);
void* bl_flash_get_flashCfg(void);

int bl_flash_read_byxip(uint32_t addr, uint8_t *dst, uint32_t len);
int bl_flash_write_reg_withcmd(uint8_t writeRegCmd, uint8_t *regValue, uint8_t regLen);
int bl_flash_release_power_down(void);
int bl_flash_power_down(void);
int bl_flash_set_clock(uint32_t clock);
int bl_flash_set_iomode(uint8_t io_mode);
#endif
