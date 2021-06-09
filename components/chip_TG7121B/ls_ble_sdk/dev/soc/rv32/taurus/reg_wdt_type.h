#ifndef REG_WDT_TYPE_H_
#define REG_WDT_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t WDT_LOAD;
    volatile uint32_t WDT_CTRL;
    volatile uint32_t WDT_CNT;
    volatile uint32_t WDT_INTR;
}reg_wdt_t;

enum WDT_REG_WDT_LOAD_FIELD
{
    WDT_WDT_LOAD_MASK = (int)0xffffffff,
    WDT_WDT_LOAD_POS = 0,
};

enum WDT_REG_WDT_CTRL_FIELD
{
    WDT_WDT_EN_MASK = (int)0x1,
    WDT_WDT_EN_POS = 0,
    WDT_WDT_RST_EN_MASK = (int)0x2,
    WDT_WDT_RST_EN_POS = 1,
    WDT_WDT_INTR_CLR_MASK = (int)0x4,
    WDT_WDT_INTR_CLR_POS = 2,
};

enum WDT_REG_WDT_CNT_FIELD
{
    WDT_WDT_CNT_MASK = (int)0xffffffff,
    WDT_WDT_CNT_POS = 0,
};

enum WDT_REG_WDT_INTR_FIELD
{
    WDT_WDT_INTR_MASK = (int)0x1,
    WDT_WDT_INTR_POS = 0,
};

#endif


