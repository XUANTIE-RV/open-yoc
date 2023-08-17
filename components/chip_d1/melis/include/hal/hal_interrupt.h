#ifndef SUNXI_HAL_INTERRUPT_H
#define SUNXI_HAL_INTERRUPT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <interrupt.h>

void hal_interrupt_enable(void);
void hal_interrupt_disable(void);
unsigned long hal_interrupt_save(void);
void hal_interrupt_restore(unsigned long flag);

uint32_t hal_interrupt_get_nest(void);

#ifdef __cplusplus
}
#endif

#endif
