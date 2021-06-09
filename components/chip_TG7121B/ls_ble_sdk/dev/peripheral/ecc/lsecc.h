#ifndef __LSECC_H__
#define __LSECC_H__
#include "stdint.h"
#include "reg_lsecc.h"
#include "reg_lsecc_type.h"

typedef uint8_t  u_int8;
typedef uint16_t u_int16;
typedef uint32_t u_int32;
typedef bool boolean;

void lsecc_init(void);
void lsecc_deinit(void);
#endif //(__LSECC_H__)


