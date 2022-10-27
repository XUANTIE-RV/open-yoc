#ifndef COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_MBOX_BL_IPC_H_
#define COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_MBOX_BL_IPC_H_
#include "../../chip_bl606p.h"


#define BL_IPC_BASE_M0         ((bl_ipc_regs_t *)BL_IPC0_BASE)
#define BL_IPC_M0_IRQn         IPC_M0_IRQn

#define BL_IPC_BASE_LP         ((bl_ipc_regs_t *)BL_IPC1_BASE)
#define BL_IPC_LP_IRQn         IPC_LP_IRQn

#define BL_IPC_BASE_D0         ((bl_ipc_regs_t *)BL_IPC2_BASE)
#define BL_IPC_D0_IRQn         IPC_D0_IRQn


#define BL_IPC_SHARERAM_BASE   (0x40000000)                        // share mem
#define BL_IPC_SHARERAM        (0x100)                            // >4 4K 0x1000 256 0x100

#define BL_GET_U32_0(val)      ((val>>0 ) & 0xFF)
#define BL_GET_U32_1(val)      ((val>>8 ) & 0xFF)
#define BL_GET_U32_2(val)      ((val>>16) & 0xFF)
#define BL_GET_U32_3(val)      ((val>>24) & 0xFF)

#define BL_INTRRUPT_UMASK_FLAG     (0xFFFFFFFF)

#define BL_IPC_INTRRUPT_FIELD_ACK_MASK (0x80)
#define BL_IPC_INTRRUPT_SEND_DATA_MASK (0x01)
#define BL_IPC_INTRRUPT_RECEIVED_MASK  (0x03)

typedef struct {
    volatile uint32_t trigger_int;
    volatile uint32_t a[7];
    volatile uint32_t trigger_int_res;
    volatile uint32_t int_rawstatus;
    volatile uint32_t int_clear;
    volatile uint32_t int_umaskset;
    volatile uint32_t int_umaskclear;

} bl_ipc_regs_t;

typedef struct {
    bl_ipc_regs_t* bl_ipc_base;
    uint32_t       bl_ipc_irq_num;

    uint8_t        *pui8_send_buf;
    uint8_t        *pui8_recv_buf;

    uint32_t       ui32_bufflen;
    uint32_t       ui32_local_id;
    uint32_t       ui32_channel_id;
} bl_ipc_priv_t;
typedef bl_ipc_priv_t* pbl_ipc_priv_t;

void bl_ipc_mask_enable(uint32_t  ui32_cpuid);
void bl_ipc_mask_disable(uint32_t  ui32_cpuid);
void bl_ipc_clear_pend(uint32_t  ui32_cpuid, uint32_t ui32_rawstatus);
uint32_t bl_ipc_get_isr_status(uint32_t  ui32_cpuid);
int bl_ipc_trigger(int i32_src_id, int i32_dest_id, uint32_t ui32_value);
uint32_t bl_ipc_get_base(uint32_t ui32_cpuid);

uint32_t blyoc_ipc_init(uint32_t ui32_cpuid);
void blyoc_ipc_uninit(uint32_t ui32_cpuid);

#endif /* COMPONENTS_CHIP_BL606_DRIVERS_BLYOC_MBOX_BL_IPC_H_ */
