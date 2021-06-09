
#define MODULE_NAME            "TIMER"
//#define __DEBUG__

#include "silan_syscfg.h"
#include "silan_irq.h"
#include "silan_errno.h"
#include "silan_timer.h"
#include "silan_printf.h"
#include "silan_pmu.h"
#include "string.h"

#if defined(__CC_ARM)
#define INST_PER_CYCLE		5
static uint32_t delay_calib = 8;
#endif

#if defined(__XCC__)
#define INST_PER_CYCLE		10
static uint32_t delay_calib = 8;
#endif

#if defined(__CSKY__)
#define INST_PER_CYCLE		8
static uint32_t delay_calib = 8;
#endif

static tsk_t   timer_task[TIMER_TASK_NUM];
static uint8_t timer_task_index;
static uint8_t task_timer;

static void timer_task_handler(uint32_t subid)
{
	int32_t i;

	silan_timer_irq_reset(TIMER_ADDR(task_timer));
	//sl_printf("*");

	for(i=0; i<TIMER_TASK_NUM; i++) {
		if(timer_task[i] == NULL)
			break;
		else
			(*timer_task[i])();
	}
}

int32_t silan_timer_task_init(silan_timer_t timer, uint32_t period)
{
	if(timer > TIMER_4) {
		SL_LOG("Task Timer Index Error %d", timer);
		return -EINVAL;
	}
	memset(timer_task, 0, sizeof(timer_task));
	timer_task_index = 0;

	silan_timer_disable(TIMER_ADDR(timer));
	silan_timer_count(TIMER_ADDR(timer), 0);
	silan_timer_compare(TIMER_ADDR(timer), TIMER_1MS_CNT*period);
	silan_pic_request(PIC_IRQID_TIMER, timer, (hdl_t)timer_task_handler);

	task_timer = timer;
	silan_timer_irq_enable(TIMER_ADDR(timer));

	return ENONE;
}

int32_t silan_timer_task_request(tsk_t task)
{
	if(timer_task_index > (TIMER_TASK_NUM - 1)) {
		SL_LOG("No More Timer Task");
		return -ENOSPC;
	}

	timer_task[timer_task_index] = task;

	timer_task_index++;

	return ENONE;
}

void udelay(uint32_t us)
{
	volatile uint32_t i,j;

	for(i=0;i<us;i++)
	{
		for(j=0;j<delay_calib;j++);
	}
}

void silan_delay_calib(void)
{
#if defined(__CC_ARM)
	delay_calib = silan_get_mcu_cclk()/1000000/INST_PER_CYCLE;
#endif
#if defined(__XCC__)
	delay_calib = silan_get_dsp_cclk()/1000000/INST_PER_CYCLE;
#endif
#if defined(__CSKY__)
	delay_calib = silan_get_bus_cclk()/1000000/INST_PER_CYCLE;
#endif

}

