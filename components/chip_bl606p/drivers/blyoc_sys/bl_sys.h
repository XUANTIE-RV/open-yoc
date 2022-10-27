#ifndef COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_SYS_BL_SYS_H_
#define COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_SYS_BL_SYS_H_


/*****************************cpu id*********************************/
#define BL_REG_CORE_ID_BASE    (0xF0000000)                        // Reg for cpu id

#define BL_REG_CORE_ID_M0      (0xE9070000)                        // Reg value for cpu id
#define BL_REG_CORE_ID_D0      (0xDEAD5500)
#define BL_REG_CORE_ID_LP      (0xDEADE902)

#define BL_CPUID_D0  0                                             // user defineed cpu id
#define BL_CPUID_M0  1
#define BL_CPUID_LP  2
#define BL_CPUID_MAX 3

uint32_t bl_cpuid_get(void);
void bl_enable_cpu0(void);

void l2_sram_vram_config(void);
void bl_sys_psram_init(void);

#endif /* COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_SYS_BL_SYS_H_ */
