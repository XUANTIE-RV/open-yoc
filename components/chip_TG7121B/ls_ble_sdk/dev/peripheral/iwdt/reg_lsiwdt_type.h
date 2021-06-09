#ifndef REG_IWDT_TYPE_H_
#define REG_IWDT_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t IWDT_LOAD;
    volatile uint32_t IWDT_VALUE;
    volatile uint32_t IWDT_CON;
    volatile uint32_t IWDT_INTCLR;
    volatile uint32_t IWDT_RIS;
    volatile uint32_t IWDT_LOCK;
} reg_iwdt_t;

enum IWDT_REG_IWDT_LOAD_FIELD
{
    IWDT_LOAD_MASK = (int)0xffffffff,
    IWDT_LOAD_POS = 0,
};

enum IWDT_REG_IWDT_VALUE_FIELD
{
    IWDT_VALUE_MASK = (int)0xffffffff,
    IWDT_VALUE_POS = 0,
};

enum IWDT_REG_IWDT_CON_FIELD
{
    IWDT_EN_MASK = 0x1,
    IWDT_EN_POS = 0,
    IWDT_IE_MASK = 0x2,
    IWDT_IE_POS = 1,
    IWDT_RSTEN_MASK = 0x4,
    IWDT_RSTEN_POS = 2,
    IWDT_CLKS_MASK = 0x30,
    IWDT_CLKS_POS = 4,
};

enum IWDT_REG_IWDT_INTCLR_FIELD
{
    IWDT_INTCLR_MASK = (int)0xffffffff,
    IWDT_INTCLR_POS = 0,
};

enum IWDT_REG_IWDT_RIS_FIELD
{
    IWDT_WDTIF_MASK = 0x1,
    IWDT_WDTIF_POS = 0,
};

enum IWDT_REG_IWDT_LOCK_FIELD
{
    IWDT_LOCK_MASK = (int)0xffffffff,
    IWDT_LOCK_POS = 0,
};

#endif
