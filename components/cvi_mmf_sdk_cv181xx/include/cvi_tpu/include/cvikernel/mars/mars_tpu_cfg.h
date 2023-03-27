#ifndef __MARS_TPU_CFG__
#define __MARS_TPU_CFG__

#define MARS_VER                                       182202
#define MARS_HW_NPU_SHIFT                              3
#define MARS_HW_EU_SHIFT                               4
#define MARS_HW_LMEM_SHIFT                             15
#define MARS_HW_LMEM_BANKS                             8
#define MARS_HW_LMEM_BANK_SIZE                         0x1000
#define MARS_HW_NODE_CHIP_SHIFT                        0
#define MARS_HW_NPU_NUM                                (1 << MARS_HW_NPU_SHIFT)
#define MARS_HW_EU_NUM                                 (1 << MARS_HW_EU_SHIFT)
#define MARS_HW_LMEM_SIZE                              (1 << MARS_HW_LMEM_SHIFT)
#define MARS_HW_LMEM_START_ADDR                        0x0C000000
#define MARS_HW_NODE_CHIP_NUM                          (1 << MARS_HW_NODE_CHIP_SHIFT)

#if (MARS_HW_LMEM_SIZE != (MARS_HW_LMEM_BANK_SIZE * MARS_HW_LMEM_BANKS))
#error "Set wrong TPU configuration."
#endif

#define MARS_GLOBAL_MEM_START_ADDR                     0x0
#define MARS_GLOBAL_MEM_SIZE                           0x100000000 // 

#define MARS_GLOBAL_TIU_CMDBUF_ADDR                    0x00000000
#define MARS_GLOBAL_TDMA_CMDBUF_ADDR                   0x00800000
#define MARS_GLOBAL_TIU_CMDBUF_RESERVED_SIZE           0x00800000 // 8MB
#define MARS_GLOBAL_TDMA_CMDBUF_RESERVED_SIZE          0x00800000 // 8MB
#define MARS_GLOBAL_POOL_RESERVED_SIZE                (MARS_GLOBAL_MEM_SIZE - MARS_GLOBAL_TIU_CMDBUF_RESERVED_SIZE - MARS_GLOBAL_TDMA_CMDBUF_RESERVED_SIZE)

#define MARS_UART_CTLR_BASE_ADDR                       0x04140000

#define MARS_TDMA_ENGINE_BASE_ADDR                     0x0C100000
#define MARS_TDMA_ENGINE_END_ADDR                      (MARS_TDMA_ENGINE_BASE_ADDR + 0x1000)

#define MARS_TIU_ENGINE_BASE_ADDR                      0x0C101000 //"NPS Register" in memory map?
#define MARS_TIU_ENGINE_END_ADDR                       (MARS_TIU_ENGINE_BASE_ADDR + 0x1000)

#endif
