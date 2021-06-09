#ifndef REG_LSECC_TYPE_H_
#define REG_LSECC_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t ARAM[64];
    volatile uint32_t NRAM[64];
    volatile uint32_t ENG[64];
    volatile uint32_t CON;
    volatile uint32_t STAT;
    volatile uint32_t DMACTR;
}reg_ecc_t;

enum ECC_REG_ARAM_FIELD
{
    ECC_ARAMBIT_MASK = (int)0xffffffff,
    ECC_ARAMBIT_POS = 0,
};

enum ECC_REG_NRAM_FIELD
{
    ECC_NRAMBIT_MASK = (int)0xffffffff,
    ECC_NRAMBIT_POS = 0,
};

enum ECC_REG_ENG_FIELD
{
    ECC_ENGBIT_MASK = (int)0xffffffff,
    ECC_ENGBIT_POS = 0,
};

enum ECC_REG_CON_FIELD
{
    ECC_CONBIT_MASK = (int)0xffffffff,
    ECC_CONBIT_POS = 0,
};

enum ECC_REG_STAT_FIELD
{
    ECC_RUNNING_MASK = 0x1,
    ECC_RUNNING_POS = 0,
    ECC_DONE_MASK = 0x2,
    ECC_DONE_POS = 1,
    ECC_STATBIT_MASK = (int)0xfffffffc,
    ECC_STATBIT_POS = 2,
};

enum ECC_REG_DMACTR_FIELD
{
    ECC_DMA_EN_MASK = 0x1,
    ECC_DMA_EN_POS = 0,
};

#endif
