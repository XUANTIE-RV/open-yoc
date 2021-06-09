#ifndef REG_LSCACHE_TYPE_H_
#define REG_LSCACHE_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t CCR;             // 0x00
    volatile uint32_t SR;            // 0x04
    volatile uint32_t IRQMASK;        // 0x08
    volatile uint32_t IRQSTAT;        // 0x0c
    volatile uint32_t HWPARAMS;        // 0x10
    volatile uint32_t CSHR;            // 0x14
    volatile uint32_t CSMR;            // 0x18
}reg_lscache_t;

enum LSCACHE_REG_CCR_FIELD
{
    LSCACHE_EN_MASK = 0x1,
    LSCACHE_EN_POS = 0,
    LSCACHE_INV_REQ_MASK = 0x2,
    LSCACHE_INV_REQ_POS = 1,
    LSCACHE_POW_REQ_MASK = 0x4,
    LSCACHE_POW_REQ_POS = 2,
    LSCACHE_SET_MAN_POW_MASK = 0x8,
    LSCACHE_SET_MAN_POW_POS = 3,
    LSCACHE_SET_MAN_INV_MASK = 0x10,
    LSCACHE_SET_MAN_INV_POS = 4,
    LSCACHE_SET_PREFETCH_MASK = 0x20,
    LSCACHE_SET_PREFETCH_POS = 5,
    LSCACHE_STATISTIC_EN_MASK = 0x40,
    LSCACHE_STATISTIC_EN_POS = 6,
};

#endif
