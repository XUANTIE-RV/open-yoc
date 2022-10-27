#ifndef COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_SYS_H_
#define COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_SYS_H_

/**************************read/write reg ***************************/
#define BLYOC_GET_REG32(reg)      (*((volatile uint32_t *)(uintptr_t)(reg)))
#define BLYOC_SET_REG32(reg,val)  ((*((volatile uint32_t *)(uintptr_t)(reg))) = val)

/*****************************cpu id*********************************/
#define BLYOC_REG_CORE_ID_BASE    (0xF0000000)                        // Reg for cpu id

#define BLYOC_REG_CORE_ID_M0      (0xE9070000)                        // Reg value for cpu id
#define BLYOC_REG_CORE_ID_D0      (0xDEAD5500)
#define BLYOC_REG_CORE_ID_LP      (0xDEADE902)

#define BLYOC_CPUID_D0  0                                             // user defineed cpu id
#define BLYOC_CPUID_M0  1
#define BLYOC_CPUID_LP  2
#define BLYOC_CPUID_MAX 3

void blyoc_enable_cpu0(void);
void blyoc_boot_cpu0(uint32_t start_addr);
uint32_t blyoc_cpuid_get(void);

#endif /* COMPONENTS_CHIP_BL606_INCLUDE_BLYOC_SYS_H_ */
