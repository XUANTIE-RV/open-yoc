#include "taurus.h"
#include "core_rv32.h"
#include "swint_call_asm.h"

__attribute__((weak)) void SystemInit(){}

void sys_init_itf()
{
    
    
}

void ble_pkt_irq_mask()
{
    csi_vic_disable_irq(MAC1_IRQn);
}

void ble_pkt_irq_unmask()
{
    csi_vic_enable_irq(MAC1_IRQn);
}

void ble_pkt_irq_clr()
{
    csi_vic_clear_pending_irq(MAC1_IRQn);
}

void swint2_set()
{
    csi_vic_set_pending_irq(SWINT2_IRQn);
}

void flash_swint_set()
{
    csi_vic_set_pending_irq(SWINT3_IRQn);
}

void ll_swint_set()
{
    SWINT_SET_INLINE_ASM(SWINT1_IRQn);
}