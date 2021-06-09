#include "aos/hal/timer.h"
#include "lstimer.h"
#include <stdio.h>
#include <aos/kernel.h>
#define PORT_TIMER_SIZE    4

typedef struct {
    TIM_HandleTypeDef  htim;
    uint8_t            inited;
    hal_timer_cb_t     timer_handler;
    void              *timer_arg;
} hal_timer_t;

static hal_timer_t hal_tim[PORT_TIMER_SIZE] = {0};
static reg_timer_t *get_timer_reg(timer_dev_t *tim);
static void enable_timer_clk_isr(timer_dev_t *tim);

int32_t hal_timer_init(timer_dev_t *tim)
{
    int32_t ret = -1;
    uint32_t udwcycle;

    if(tim == NULL) {
        return -1;
    }

    if(tim->port > PORT_TIMER_SIZE) {
        return -1;
    }

    if (hal_tim[tim->port].inited == 1){
        return 0;
    }
    reg_timer_t *TIMx = get_timer_reg(tim);
    enable_timer_clk_isr(tim);
    TIM_HandleTypeDef * const psttimhandle = &hal_tim[tim->port].htim;
    psttimhandle->Instance = TIMx;
    hal_tim[tim->port].timer_handler = tim->config.cb;
    hal_tim[tim->port].timer_arg = tim->config.arg;

    udwcycle = tim->config.period;
    if(udwcycle > 6553600) {
        psttimhandle->Init.Period            = (udwcycle / 2000) - 1;
        psttimhandle->Init.Prescaler         = SDK_HCLK_MHZ*2000 -1;
    }
    else {
        psttimhandle->Init.Period            = (udwcycle / 100) - 1;
        psttimhandle->Init.Prescaler         = SDK_HCLK_MHZ*100 -1;
    }
    
    psttimhandle->Init.ClockDivision     = 0;
    psttimhandle->Init.CounterMode       = TIM_COUNTERMODE_UP;
    psttimhandle->Init.RepetitionCounter = 0;

    __HAL_TIM_URS_ENABLE(psttimhandle);

    if(tim->config.reload_mode == TIMER_RELOAD_MANU) {
        ret = HAL_TIM_OnePulse_Init(psttimhandle, TIM_OPMODE_SINGLE);
    }
    else {
        ret = HAL_TIM_Base_Init(psttimhandle);
    }
    hal_tim[tim->port].inited = 1;

    return ret;
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    int32_t ret = -1;

    if(tim != NULL && tim->port < PORT_TIMER_SIZE + 1) {
        if(!hal_tim[tim->port].inited) {
            ret = -1;
        }
        else {
            if(get_timer_reg(tim) != NULL) 
            {
                ret = HAL_TIM_Base_Start_IT(&hal_tim[tim->port].htim);
            }
        }
    }
    return ret;
}

void hal_timer_stop(timer_dev_t *tim)
{
    if(get_timer_reg(tim) != NULL) {
        HAL_TIM_Base_Stop_IT(&hal_tim[tim->port].htim);
    }
}

int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    int32_t ret = -1;
    if(tim == NULL) {
        return -1;
    }
    reg_timer_t *TIMx = get_timer_reg(tim);
    uint32_t udwcycle = para.period;
    enable_timer_clk_isr(tim);
    TIM_HandleTypeDef * const psttimhandle = &hal_tim[tim->port].htim;
    psttimhandle->Instance = TIMx;
    hal_tim[tim->port].timer_handler = tim->config.cb;
    hal_tim[tim->port].timer_arg = tim->config.arg;

    if(udwcycle > 6553600) {
        psttimhandle->Init.Period            = (udwcycle / 2000) - 1;
        psttimhandle->Init.Prescaler         = SDK_HCLK_MHZ*2000 -1;
    }
    else {
        psttimhandle->Init.Period            = (udwcycle / 100) - 1;
        psttimhandle->Init.Prescaler         = SDK_HCLK_MHZ*100 -1;
    }
    
    psttimhandle->Init.ClockDivision     = 0;
    psttimhandle->Init.CounterMode       = TIM_COUNTERMODE_UP;
    psttimhandle->Init.RepetitionCounter = 0;

    __HAL_TIM_URS_ENABLE(psttimhandle);

    if(tim->config.reload_mode == TIMER_RELOAD_MANU) {
        ret = HAL_TIM_OnePulse_Init(psttimhandle, TIM_OPMODE_SINGLE);
    }
    else {
        ret = HAL_TIM_Base_Init(psttimhandle);
    }
    HAL_TIM_Base_Start_IT(&hal_tim[tim->port].htim);
    return ret;
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    int32_t ret = -1;

    if (tim != NULL)
    {
        hal_tim[tim->port].inited = 0;
        ret = HAL_TIM_Base_DeInit(&hal_tim[tim->port].htim);

        switch(tim->port)
        {
        case 0:
            __HAL_RCC_GPTIMA_CLK_DISABLE();
        break;
        case 1:
            __HAL_RCC_GPTIMB_CLK_DISABLE();
        break;
        case 2:
            __HAL_RCC_GPTIMC_CLK_DISABLE();
        break;
        case 3:
            __HAL_RCC_BTIM_CLK_DISABLE();
        break;
        default:
        break;
        }
    }

    return ret;
}

void GPTIMA1_Handler(void)
{
    aos_kernel_intrpt_enter();
    HAL_TIM_IRQHandler(&hal_tim[0].htim);
    aos_kernel_intrpt_exit();
}

void GPTIMB1_Handler(void)
{
    aos_kernel_intrpt_enter();
    HAL_TIM_IRQHandler(&hal_tim[1].htim);   
    aos_kernel_intrpt_exit(); 
}

void GPTIMC1_Handler(void)
{
    aos_kernel_intrpt_enter();
    HAL_TIM_IRQHandler(&hal_tim[2].htim);
    aos_kernel_intrpt_exit();
}

void BSTIM1_Handler(void)
{
    aos_kernel_intrpt_enter();
    HAL_TIM_IRQHandler(&hal_tim[3].htim);
    aos_kernel_intrpt_exit();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    hal_timer_t *timer = (hal_timer_t *)htim;

   if (timer && timer->timer_handler) {
        timer->timer_handler(timer->timer_arg);
    }
}

static reg_timer_t *get_timer_reg(timer_dev_t *tim)
{
    reg_timer_t *reg;
    switch(tim->port)
    {
    case 0:
        reg = LSGPTIMA;
    break;
    case 1:
        reg = LSGPTIMB;
    break;
    case 2:
        reg = LSGPTIMC;
    break;
    case 3:
        reg = LSBSTIM;
    break;
    default:
        reg = NULL;
    break;
    }
    return reg;
}

static void enable_timer_clk_isr(timer_dev_t *tim)
{
    switch(tim->port)
    {
    case 0:
        __HAL_RCC_GPTIMA_CLK_ENABLE();
        arm_cm_set_int_isr(GPTIMA1_IRQn,GPTIMA1_Handler);
        NVIC_EnableIRQ(GPTIMA1_IRQn);
    break;
    case 1:
        __HAL_RCC_GPTIMB_CLK_ENABLE();
        arm_cm_set_int_isr(GPTIMB1_IRQn,GPTIMB1_Handler);
        NVIC_EnableIRQ(GPTIMB1_IRQn);
    break;
    case 2:
        __HAL_RCC_GPTIMC_CLK_ENABLE();
        arm_cm_set_int_isr(GPTIMC1_IRQn,GPTIMC1_Handler);
        NVIC_EnableIRQ(GPTIMC1_IRQn);
    break;
    case 3:
        __HAL_RCC_BTIM_CLK_ENABLE();
        arm_cm_set_int_isr(BSTIM1_IRQn,BSTIM1_Handler);
        NVIC_EnableIRQ(BSTIM1_IRQn);
    break;
    default:
    break;
    }
}