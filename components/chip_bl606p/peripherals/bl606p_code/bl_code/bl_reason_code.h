#ifndef __BL_REASON_CODE_H__
#define __BL_REASON_CODE_H__

#include <stdint.h>

typedef enum {
    BL_RST_POWER_OFF = 0,
    BL_RST_HARDWARE_WATCHDOG,
    BL_RST_FATAL_EXCEPTION,
    BL_RST_SOFTWARE_WATCHDOG,
    BL_RST_SOFTWARE,
} BL_RST_REASON_E;

BL_RST_REASON_E bl_sys_rstinfo_get(void);
int bl_sys_rstinfo_set(BL_RST_REASON_E val);
int bl_sys_rstinfo_getsting(char *info);
void bl_sys_rstinfo_init(void);
int bl_sys_logall_enable(void);
int bl_sys_logall_disable(void);
void bl_sys_mfg_config(char *outbuf, int len, int argc, char **argv);
int bl_sys_reset_por(void);
void bl_sys_reset_system(void);
int bl_sys_isxipaddr(uint32_t addr);
int bl_sys_early_init(void);
int bl_sys_init(void);
void bl_enable_cpu0(void);
void bl_boot_cpu0(uint32_t start_addr);
void bl_halt_cpu0(void);
void bl_release_cpu0(void);
void bl_sys_enabe_jtag(int cpuid);
void cli_reg_cmd_mfg(void);
#endif
