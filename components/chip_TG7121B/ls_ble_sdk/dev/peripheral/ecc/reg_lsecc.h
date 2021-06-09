#ifndef REG_LSECC_H_
#define REG_LSECC_H_
#include "reg_lsecc_type.h"

#define LSECC ((reg_ecc_t *)(0x40002000))

void lsecc_init(void);
#endif //(REG_LSECC_H_)
