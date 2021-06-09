#ifndef REG_TIM_TYPE_H_
#define REG_TIM_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t TIM_CTRL;
    volatile uint32_t TIM_TGT;
    volatile uint32_t RESERVED0[2];
    volatile uint32_t TIM_CNT;
    volatile uint32_t TIM_INTR;
}reg_tim_t;

enum TIM_REG_TIM_CTRL_FIELD
{
    TIM_TIM_EN_MASK = (int)0x1,
    TIM_TIM_EN_POS = 0,
    TIM_TIM_INTR_EN_MASK = (int)0x2,
    TIM_TIM_INTR_EN_POS = 1,
    TIM_TIM_INTR_CLR_MASK = (int)0x4,
    TIM_TIM_INTR_CLR_POS = 2,
};

enum TIM_REG_TIM_TGT_FIELD
{
    TIM_TIM_TGT_MASK = (int)0xffffffff,
    TIM_TIM_TGT_POS = 0,
};

enum TIM_REG_TIM_CNT_FIELD
{
    TIM_TIM_CNT_MASK = (int)0xffffffff,
    TIM_TIM_CNT_POS = 0,
};

enum TIM_REG_TIM_INTR_FIELD
{
    TIM_TIM_INTR_MASK = (int)0x1,
    TIM_TIM_INTR_POS = 0,
};

#endif


