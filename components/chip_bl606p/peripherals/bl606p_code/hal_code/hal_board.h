#ifndef __HAL_BOARD_H__
#define __HAL_BOARD_H__
int hal_board_cfg(uint8_t board_code);
uint32_t hal_board_get_factory_addr(void);
static int hal_board_load_fdt_info(const void *dtb);
#endif
