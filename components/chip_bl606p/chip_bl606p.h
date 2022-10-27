#ifndef COMPONENTS_CHIP_BL606_CHIP_BL606_H_
#define COMPONENTS_CHIP_BL606_CHIP_BL606_H_
#include <bl606p.h>
/*************************** REG base *******************************/
#define BL_CORE_ID_BASE                      ((uint32_t)0xF0000000)                // Reg for cpu id

#define BL_UART0_BASE                        ((uint32_t)0x30002000)                // Reg for UART0
#define BL_UART1_BASE                        ((uint32_t)0x2000a100)                // Reg for UART1

#define BL_IPC0_BASE                         ((uint32_t)0x2000a800)                // Reg for IPC0
#define BL_IPC1_BASE                         ((uint32_t)0x2000a840)                // Reg for IPC1
#define BL_IPC2_BASE                         ((uint32_t)0x30005000)                // Reg for IPC2

/*************************** memory base *****************************/
//typedef struct {
//    volatile uint32_t  mem_start;
//    volatile uint32_t  mem_lth;
//    volatile uint32_t  mem_end;
//
//} MEM_CODE_TYPE;
//
//MEM_CODE_TYPE BL_FPGA_DDR1[] = {
//    {
//        0x80000000,                      // cpu1
//        0x02000000,
//        0x82000000
//    },
//    {
//        0x82000000,                      // codec
//        48 * 10000,
//        0x82075300
//    },
//    {
//        0x8F000000,                      // mbox
//        0x10000,
//        0x8F010000
//    },
//    {
//        0,
//        0,
//        0
//    }
//};

/*************************** RRQ base ********************************/
//#if defined(CPU_M0)   TODO： 这一块代码由于没有对宏重命名与 bl606p会有重定义的问题，所以当前先注掉
//#include "CPU_IRQ_M0.h"
//#elif defined(CPU_D0)
//#include "CPU_IRQ_D0.h"
//#elif defined(CPU_L0)
//#include "CPU_IRQ_L0.h"
//#else
//
//#endif /* CPU_M0, CPU_D0, CPU_L0  */

/**************************read/write reg ***************************/
#define BL_GET_REG32(reg)      (*((volatile uint32_t *)(uintptr_t)(reg)))
#define BL_SET_REG32(reg,val)  ((*((volatile uint32_t *)(uintptr_t)(reg))) = val)

#endif /* COMPONENTS_CHIP_BL606_CHIP_BL606_H_ */
