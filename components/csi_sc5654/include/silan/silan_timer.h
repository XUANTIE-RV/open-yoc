#ifndef __SILAN_TIMER_H__
#define __SILAN_TIMER_H__

#include "silan_types.h"
#include "silan_syscfg.h"
#include "silan_timer_regs.h"

#define TIMER_TASK_NUM	8

#define TIMER_1MS_CNT	get_sysclk_val_settled()/2/1000

#define SILAN_TIMER_INIT_DEFAULT_VALUE     (0xffffffff / silan_get_timer_cclk() * 1000000)

typedef enum {
	TIMER_0           = 0,
	TIMER_1,
	TIMER_2,
	TIMER_3,
	TIMER_4,
	TIMER_MS,
	TIMER_S,
}silan_timer_t;

/*************************************************************************/
static void inline silan_timer_stop(uint32_t addr)
{
    __sREG32(addr, TIMER_CTRL) = ~(TIMER_TO|TIMER_EN);
}

static void inline silan_timer_1ms_set(void)
{
	TIMER_1MS_COMPARE = silan_get_timer_cclk() / 1000;
}

static void inline silan_timer_irq_reset(uint32_t addr)
{
   __sREG32(addr, TIMER_CTRL) = TIMER_EN|TIMER_INT;
}

static void inline silan_timer_irq_clear(uint32_t addr)
{
   __sREG32(addr, TIMER_CTRL) = ~TIMER_INT;
}

static void inline silan_timer_count(uint32_t addr, uint32_t count)
{
    __sREG32(addr, TIMER_COUNT) = count;
}

static void inline silan_timer_compare(uint32_t addr, uint32_t compare)
{
    __sREG32(addr, TIMER_COMPARE) = compare;
}

static void inline silan_timer_sel(uint32_t addr, uint32_t sel)
{
    __sREG32(addr, TIMER_SEL) = sel;
}

static void inline silan_timer_enable(uint32_t addr)
{
    __sREG32(addr, TIMER_CTRL) = TIMER_EN;
}

static void inline silan_timer_disable(uint32_t addr)
{
	__sREG32(addr, TIMER_CTRL) &= ~TIMER_EN;
}

static void inline silan_timer_to_clear(uint32_t addr)
{
    __sREG32(addr, TIMER_CTRL) &= ~TIMER_TO;
}

static uint32_t inline silan_timer_to_state(uint32_t addr)
{
	if(__sREG32(addr, TIMER_CTRL)& TIMER_TO)
        return 1;
    else
        return 0;
}

static void inline silan_timer_irq_enable(uint32_t addr)
{
   __sREG32(addr, TIMER_CTRL) = TIMER_EN|TIMER_INT;
}

static void inline silan_timer_irq_disable(uint32_t addr)
{
   __sREG32(addr, TIMER_CTRL) = ~(TIMER_EN|TIMER_INT);
}

static uint32_t inline silan_get_timer_count(uint32_t addr)
{
	return __sREG32(addr, TIMER_COUNT);
}

static void inline silan_timer_start(uint32_t addr)
{
    silan_timer_count(addr, 0x0);
    silan_timer_compare(addr, 0xffffffff);
    silan_timer_enable(addr);
}

static void inline silan_timer_irq_start(uint32_t addr, uint32_t time)
{
    silan_timer_count(addr, 0x0);
    silan_timer_compare(addr, time);
    silan_timer_irq_enable(addr);
}

/*************************************************************************/
void udelay(uint32_t us);
void silan_delay_calib(void);
int32_t silan_timer_task_init(silan_timer_t timer, uint32_t period);
int32_t silan_timer_task_request(tsk_t task);

#endif

