#include "aos/hal/pwm.h"
#include "lstimer.h"
#include "field_manipulate.h"
#include "io_config.h"
#include <stdio.h>

#define PORT_TIMER_SIZE    14
TIM_OC_InitTypeDef sConfig;
//port:0~3   LSADTIM(ADTIM)  ch1,ch2,ch3,ch4
//port:4~7   LSGPTIMA(GPTIMA) ch1,ch2,ch3,ch4
//port:8~11  LSGPTIMB(GPTIMB) ch1,ch2,ch3,ch4
//port:12~15 LSGPTIMC(GPTIMC) ch1,ch2

void TIM_OC1_SetConfig(reg_timer_t *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC3_SetConfig(reg_timer_t *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC4_SetConfig(reg_timer_t *TIMx, TIM_OC_InitTypeDef *OC_Config);

static reg_timer_t *get_timer_reg(pwm_dev_t *tim)
{
    reg_timer_t *reg;
    switch(tim->port/4)
    {
    case 0:
        reg = LSADTIM;
    break;
    case 1:
        reg = LSGPTIMA;
    break;
    case 2:
        reg = LSGPTIMB;
    break;
    case 3:
        reg = LSGPTIMC;
    break;
    default:
        reg = NULL;
    break;
    }
    return reg;
}

static void enable_timer_clk(pwm_dev_t *tim)
{
    switch(tim->port/4)
    {
    case 0:
        __HAL_RCC_ADTIM_CLK_ENABLE();
    break;
    case 1:
        __HAL_RCC_GPTIMA_CLK_ENABLE();
    break;
    case 2:
        __HAL_RCC_GPTIMB_CLK_ENABLE();
    break;
    case 3:
        __HAL_RCC_GPTIMC_CLK_ENABLE();
    break;
    default:
    break;
    }
}

static uint32_t get_pwm_channel(uint8_t pwm_port)
{
    uint32_t ret;
    switch(pwm_port%4)
    {
    case 0:
        ret = TIM_CHANNEL_1;
    break;
    case 1:
        ret = TIM_CHANNEL_2;
    break;
    case 2:
        ret = TIM_CHANNEL_3;
    break;
    case 3:
        ret = TIM_CHANNEL_4;
    break;
    default:
        ret = 0;
    break;
    }
    return ret;
}

static void pwm_base_setconfig(reg_timer_t *TIMx,pwm_config_t para)
{
    uint32_t tmpcr1;
    tmpcr1 = TIMx->CR1;

    if (IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMx)) {
        /* Select the Counter Mode */
        tmpcr1 &= ~(TIMER_CR1_DIR | TIMER_CR1_CMS);
        tmpcr1 |= TIM_COUNTERMODE_UP;
    }

    if (IS_TIM_CLOCK_DIVISION_INSTANCE(TIMx)){
        /* Set the clock division */
        tmpcr1 &= ~TIMER_CR1_CKD;
        tmpcr1 |= 0;
    }

    /* Set the auto-reload preload */
    MODIFY_REG(tmpcr1, TIMER_CR1_ARPE, TIM_AUTORELOAD_PRELOAD_DISABLE);
    TIMx->CR1 = tmpcr1;

    if(para.freq > 0){
        /* Set the Autoreload value */
        TIMx->ARR =1000000/para.freq - 1;
    }
    

    /* Set the Prescaler value */
    TIMx->PSC = SDK_HCLK_MHZ*10 -1;
}

int32_t hal_pwm_init(pwm_dev_t *pwm)
{
    if(pwm == NULL) {
        return -1;
    }

    if(pwm->port > PORT_TIMER_SIZE-1) {
        return -1;
    }
    enable_timer_clk(pwm);
    reg_timer_t* TIMx = get_timer_reg(pwm);

    pwm_base_setconfig(TIMx,pwm->config);
    
    return 0;
}
static void pwm_config_channel(reg_timer_t* TIMx,TIM_OC_InitTypeDef *sConfig,uint32_t channel)
{
    switch(channel)
    {
        case TIM_CHANNEL_1:
        {
            /* Configure the Channel 1 in PWM mode */
            TIM_OC1_SetConfig(TIMx, sConfig);

            /* Set the Preload enable bit for channel1 */
            TIMx->CCMR1 |= TIMER_CCMR1_OC1PE;

            /* Configure the Output Fast mode */
            TIMx->CCMR1 &= ~TIMER_CCMR1_OC1FE;
            TIMx->CCMR1 |= sConfig->OCFastMode;
            break;
        }
        case TIM_CHANNEL_2:
        {
            /* Configure the Channel 2 in PWM mode */
            TIM_OC2_SetConfig(TIMx, sConfig);

            /* Set the Preload enable bit for channel2 */
            TIMx->CCMR1 |= TIMER_CCMR1_OC2PE;

            /* Configure the Output Fast mode */
            TIMx->CCMR1 &= ~TIMER_CCMR1_OC2FE;
            TIMx->CCMR1 |= sConfig->OCFastMode << 8U;
            break;
        }
        case TIM_CHANNEL_3:
        {
            /* Configure the Channel 3 in PWM mode */
            TIM_OC3_SetConfig(TIMx, sConfig);

            /* Set the Preload enable bit for channel3 */
            TIMx->CCMR2 |= TIMER_CCMR2_OC3PE;

            /* Configure the Output Fast mode */
            TIMx->CCMR2 &= ~TIMER_CCMR2_OC3FE;
            TIMx->CCMR2 |= sConfig->OCFastMode;
            break;
        }
        case TIM_CHANNEL_4:
        {
            /* Configure the Channel 4 in PWM mode */
            TIM_OC4_SetConfig(TIMx, sConfig);

            /* Set the Preload enable bit for channel4 */
            TIMx->CCMR2 |= TIMER_CCMR2_OC4PE;

            /* Configure the Output Fast mode */
            TIMx->CCMR2 &= ~TIMER_CCMR2_OC4FE;
            TIMx->CCMR2 |= sConfig->OCFastMode << 8U;
            break;
        }
    }
}
int32_t hal_pwm_start(pwm_dev_t *pwm)
{
    /* Timer Output Compare Configuration Structure declaration */

    if (pwm == NULL)
    {
        return -1;
    }

    if (pwm->port > PORT_TIMER_SIZE-1)
    {
        return -1;
    }
    
    reg_timer_t *TIMx = get_timer_reg(pwm);

    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.Pulse = pwm->config.duty_cycle * (1000000/pwm->config.freq);

    uint32_t channel_t = get_pwm_channel(pwm->port);
    pwm_config_channel(TIMx, &sConfig, channel_t);

    /* Enable the Capture compare channel */
    TIM_CCxChannelCmd(TIMx, channel_t, TIM_CCx_ENABLE);

    if (IS_TIM_BREAK_INSTANCE(TIMx) != RESET)
    {
        /* Enable the main output */
        TIMx->BDTR |= TIMER_BDTR_MOE;
    }

    /* Enable the Peripheral, except in trigger mode where enable is automatically done with trigger */
    uint32_t tmpsmcr = TIMx->SMCR & TIMER_SMCR_SMS;
    if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
    {
        TIMx->CR1 |= TIMER_CR1_CEN;
    }

    return 0;
}

int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
    if(pwm == NULL) {
        return -1;
    }

    if(pwm->port > PORT_TIMER_SIZE-1) {
        return -1;
    }
    reg_timer_t* TIMx = get_timer_reg(pwm);
    uint32_t channel_t = get_pwm_channel(pwm->port);

    TIM_CCxChannelCmd(TIMx,channel_t, TIM_CCx_DISABLE);

    if ((TIMx->CCER & TIM_CCER_CCxE_MASK) == 0UL)
    {
        if ((TIMx->CCER & TIM_CCER_CCxNE_MASK) == 0UL)
        {
            TIMx->BDTR &= ~(TIMER_BDTR_MOE);
        }
    }
    if ((TIMx->CCER & TIM_CCER_CCxE_MASK) == 0UL)
    {
        if ((TIMx->CCER & TIM_CCER_CCxNE_MASK) == 0UL)
        {
            TIMx->CR1 &= ~(TIMER_CR1_CEN);
        }
    }

    return 0;
}

int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    if(pwm == NULL) {
        return -1;
    }

    if(pwm->port > PORT_TIMER_SIZE-1) {
        return -1;
    }
    reg_timer_t* TIMx = get_timer_reg(pwm);
    uint32_t channel_t = get_pwm_channel(pwm->port);


    TIMx->ARR =1000000/para.freq - 1;
    uint32_t puse_t = para.duty_cycle*(1000000/para.freq);
    switch(channel_t)
    {
        case TIM_CHANNEL_1:
        {
            TIM_SetCompare1(TIMx,puse_t);
        }
        break;
        case TIM_CHANNEL_2:
        {
            TIM_SetCompare2(TIMx,puse_t);
        }
        break;
        case TIM_CHANNEL_3:
        {
            TIM_SetCompare3(TIMx,puse_t);
        }
        break;
        case TIM_CHANNEL_4:
        {
            TIM_SetCompare4(TIMx,puse_t);
        }
        break;
        default:
         break;
    }
    
    return 0;
}

int32_t hal_pwm_finalize(pwm_dev_t *pwm)
{
    if(pwm == NULL) {
        return -1;
    }

    if(pwm->port > PORT_TIMER_SIZE-1) {
        return -1;
    }

    switch(pwm->port/4)
    {
    case 0:
        __HAL_RCC_ADTIM_CLK_DISABLE();
    break;
    case 1:
        __HAL_RCC_GPTIMA_CLK_ENABLE();
    break;
    case 2:
        __HAL_RCC_GPTIMB_CLK_ENABLE();
    break;
    case 3:
        __HAL_RCC_GPTIMC_CLK_ENABLE();
    break;
    default:
    break;
    }

    return 0;
}
