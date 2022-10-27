#ifndef __BL_EFUSE_H__
#define __BL_EFUSE_H__
#include <stdint.h>

int bl_efuse_get_mac(uint8_t mac[6]);

// 0 success, other invalid
int bl_efuse_get_mac_byslot(uint8_t slot, uint8_t mac[6]);

#endif
