
#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void ipcflash_csky_register(int idx);
extern void ipcflash_csky_unregister(int idx);

#ifdef __cplusplus
}
#endif

#endif
