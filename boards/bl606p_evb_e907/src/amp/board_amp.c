/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#include <stdint.h>
#include <string.h>

#include <csi_core.h>

#if defined(CONFIG_COMP_LOG_IPC) && CONFIG_COMP_LOG_IPC
#include <log_ipc.h>

#define ALG_CPUID 0
int board_logipc_init()
{
    uint8_t cpu_id[] = { ALG_CPUID };

    return log_ipc_ap_init(cpu_id, (int)sizeof(cpu_id));
}

int board_get_alg_cpuid()
{
    return ALG_CPUID;
}
#endif

#if defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW
#define CACHE_ALIGN_MASK    (__riscv_xlen - 1)
#define CACHE_ALIGN_UP(a)   (((a) + CACHE_ALIGN_MASK) & ~CACHE_ALIGN_MASK)
#define CACHE_ALIGN_DOWN(a) ((a) & ~CACHE_ALIGN_MASK)
extern void boot_c906(int boot_addr);
int board_load_amp_fw(uint8_t cpuid, void * loaddr, const uint8_t * fw, uint32_t size)
{
    if (cpuid != 0) {
        return -1;
    }

    if (fw && size) {
        memcpy(loaddr, fw, size);
        csi_dcache_clean_invalid_range((uint32_t *)CACHE_ALIGN_DOWN((uint32_t)loaddr), CACHE_ALIGN_UP(size));
    }

    boot_c906((int)loaddr);
    return 0;
}
#endif
