#ifndef REG_LSGPIO_H_
#define REG_LSGPIO_H_
#include "reg_lsgpio_type.h"
#include "reg_lsexti_type.h"

#define LSGPIOA ((reg_lsgpio_t *)(0x48000000))
#define LSGPIOB ((reg_lsgpio_t *)(0x48000400))
#define LSGPIOC ((reg_lsgpio_t *)(0x48000800))
#define EXTI    ((reg_lsexti_t *)(0x40010400))


#endif //(REG_LSGPIO_H_)
