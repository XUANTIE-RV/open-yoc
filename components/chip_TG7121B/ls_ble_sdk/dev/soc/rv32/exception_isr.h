#ifndef EXCEPTION_ISR_H_
#define EXCEPTION_ISR_H_

extern void (*interrupt_vector[])();

void rv_set_int_isr(uint8_t type,void (*isr)());

#endif
