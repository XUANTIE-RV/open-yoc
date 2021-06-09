#ifndef CPU_H_
#define CPU_H_
#include <stdbool.h>

void enter_critical(void);

void exit_critical(void);

bool in_interrupt(void);

void disable_global_irq(void);

void enable_global_irq(void);

#endif

