#ifndef REG_CALC_TYPE_H_
#define REG_CALC_TYPE_H_
#include <stdint.h>

typedef struct{
    volatile uint32_t SQRTSR;
    volatile uint32_t RDCND;
    volatile uint32_t SQRTRES;
    volatile uint32_t RESERVED0[5];
    volatile uint32_t DIVD;
    volatile uint32_t DIVS;
    volatile uint32_t DIVQ;
    volatile uint32_t DIVR;
    volatile uint32_t DIVCSR;
}reg_calc_t;

enum CALC_REG_DIVCSR_FIELD
{
    CALC_DIV_BUSY_MASK = 0x1,
    CALC_DIV_BUSY_POS = 0,
    CALC_DIV_DZ_MASK = 0x2,
    CALC_DIV_DZ_POS = 1,
    CALC_DIV_SIGN_MASK = 0x100,
    CALC_DIV_SIGN_POS = 8,
    CALC_DIV_TRM_MASK = 0x200,
    CALC_DIV_TRM_POS = 9,
};
#endif

