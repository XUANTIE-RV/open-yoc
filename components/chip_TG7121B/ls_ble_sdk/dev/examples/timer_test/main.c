#include "main.h"
#include "io_config.h"
#if 1
#define PERIOD_VALUE (666 - 1) /* Period Value  */
#define PULSE1_VALUE 333       /* Capture Compare 1 Value  */
#define PULSE2_VALUE 249       /* Capture Compare 2 Value  */
#define PULSE3_VALUE 166       /* Capture Compare 3 Value  */
#define PULSE4_VALUE 83        /* Capture Compare 4 Value  */

TIM_HandleTypeDef TimHandle;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;

static void Error_Handler(void);

/* Counter Prescaler value */
uint32_t uhPrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    /* TIMx Peripheral clock enable */
    TIMx_CLK_ENABLE();

    /* Configure PA00, PA01, PA07, PA08 for PWM output */
    gptimb1_ch1_io_init(PA00, true, 0);
    gptimb1_ch2_io_init(PA01, true, 0);
    gptimb1_ch3_io_init(PA07, true, 0);
    gptimb1_ch4_io_init(PA08, true, 0);
}
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Configure the system clock to 16 MHz */
    SystemClock_Config();

    /*##-1- Configure the TIM peripheral #######################################*/
    TimHandle.Instance = TIMx;
    TimHandle.Init.Prescaler = 0; // 16MHz
    TimHandle.Init.Period = PERIOD_VALUE;
    TimHandle.Init.ClockDivision = 0;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;

    /* Set the pulse value for channel 1 */
    sConfig.Pulse = PULSE1_VALUE;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /* Set the pulse value for channel 2 */
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfig.Pulse = PULSE2_VALUE;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /* Set the pulse value for channel 3 */
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.Pulse = PULSE3_VALUE;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /* Set the pulse value for channel 4 */
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfig.Pulse = PULSE4_VALUE;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    /* Start channel 2 */
    if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    /* Start channel 3 */
    if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    /* Start channel 4 */
    if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /* Infinite loop */
    while (1)
    {
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
    /* Turn LED3 on */
    //BSP_LED_On(LED3);
    while (1)
    {
    }
}
#endif
/**
  * @brief  System Clock Configuration
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    /* switch to 16MHz system clock */
    REG_FIELD_WR(RCC->CFG, RCC_SYSCLK_SW, 1);
    REG_FIELD_WR(RCC->CFG, RCC_CKCFG, 1);
}

#if 0 //带死区互补的PWM输出
TIM_HandleTypeDef light_tim_hdl;
TIM_OC_InitTypeDef light_tim_cfg;
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef   GPIO_InitStruct;
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* TIMx Peripheral clock enable */
    __HAL_RCC_ADTIM_CLK_ENABLE();
    /* Enable all GPIO Channels Clock requested */
    /* Configure PA00, PA01 for PWM output*/
    gptimb1_ch1_io_init(PA00,true,0);
	gptimb1_ch1n_io_init(PA01,true,0);
}

void main(void)
{
    SystemClock_Config();
    TIM_BreakDeadTimeConfigTypeDef light_tim_bdt;

    light_tim_hdl.Instance = LSADTIM1;

    light_tim_hdl.Init.Prescaler = 63;
    light_tim_hdl.Init.Period = 249;
    light_tim_hdl.Init.ClockDivision = 0;
    light_tim_hdl.Init.CounterMode = TIM_COUNTERMODE_UP;
    light_tim_hdl.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&light_tim_hdl);

    light_tim_bdt.OffStateRunMode = TIM_OSSR_DISABLE;
    light_tim_bdt.OffStateIDLEMode = TIM_OSSI_DISABLE;
    light_tim_bdt.DeadTime = 0x3f;//0~0xFF
    light_tim_bdt.BreakState = TIM_BREAK_DISABLE;
    light_tim_bdt.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    light_tim_bdt.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&light_tim_hdl,&light_tim_bdt);

    light_tim_cfg.OCMode = TIM_OCMODE_PWM1;
    light_tim_cfg.OCPolarity = TIM_OCPOLARITY_HIGH;
    light_tim_cfg.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    light_tim_cfg.OCFastMode = TIM_OCFAST_DISABLE;

    light_tim_cfg.Pulse = 25;
        
    HAL_TIM_PWM_ConfigChannel(&light_tim_hdl, &light_tim_cfg, TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(&light_tim_hdl, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&light_tim_hdl, TIM_CHANNEL_1);

    while(1);
}
#endif

#if 0 //不同频率的PWM输出
TIM_HandleTypeDef light_tim_hdl;
TIM_HandleTypeDef light_tim_hdl_t;
TIM_OC_InitTypeDef light_tim_cfg;
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* TIMx Peripheral clock enable */
    __HAL_RCC_ADTIM_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();
    
    adtim1_ch1_io_init(PA00,true,0);
    gptimb1_ch1_io_init(PA01,true,0);
}

void main(void)
{
    SystemClock_Config();

    light_tim_hdl.Instance = LSADTIM1;
    light_tim_hdl.Init.Prescaler = 63;
    light_tim_hdl.Init.Period = 249;
    light_tim_hdl.Init.ClockDivision = 0;
    light_tim_hdl.Init.CounterMode = TIM_COUNTERMODE_UP;
    light_tim_hdl.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&light_tim_hdl);

    light_tim_hdl_t.Instance = LSGPTIM3;
    light_tim_hdl_t.Init.Prescaler = 63;
    light_tim_hdl_t.Init.Period = 499;
    light_tim_hdl_t.Init.ClockDivision = 0;
    light_tim_hdl_t.Init.CounterMode = TIM_COUNTERMODE_UP;
    light_tim_hdl_t.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&light_tim_hdl_t);

    light_tim_cfg.OCMode = TIM_OCMODE_PWM1;
    light_tim_cfg.OCPolarity = TIM_OCPOLARITY_HIGH;
    // light_tim_cfg.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    light_tim_cfg.OCFastMode = TIM_OCFAST_DISABLE;
    light_tim_cfg.Pulse = 125;
       
    HAL_TIM_PWM_ConfigChannel(&light_tim_hdl, &light_tim_cfg, TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(&light_tim_hdl, TIM_CHANNEL_1);

    light_tim_cfg.OCPolarity = TIM_OCPOLARITY_HIGH;
    light_tim_cfg.Pulse = 250;
    HAL_TIM_PWM_ConfigChannel(&light_tim_hdl_t, &light_tim_cfg, TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(&light_tim_hdl_t, TIM_CHANNEL_1);

    while(1);
}
#endif

#if 0 //10ms timer 
TIM_HandleTypeDef light_tim_hdl;
TIM_OC_InitTypeDef light_tim_cfg;
void gpio_test_init(void)
{
    io_cfg_output(PA00);
    io_write_pin(PA00,0);
}
uint16_t time;
void main(void)
{
    SystemClock_Config();
    gpio_test_init();

    __HAL_RCC_BTIM_CLK_ENABLE();
    light_tim_hdl.Instance = LSBSTIM1;
    light_tim_hdl.Init.Prescaler = 15;
    light_tim_hdl.Init.Period = 9999;
    light_tim_hdl.Init.ClockDivision = 0;
    light_tim_hdl.Init.CounterMode = TIM_COUNTERMODE_UP;
    //light_tim_hdl.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&light_tim_hdl);

    NVIC_SetPriority(BSTIM1_IRQn, 1);
    NVIC_EnableIRQ(BSTIM1_IRQn);

    HAL_TIM_Base_Start_IT(&light_tim_hdl);
    while(1);
}

void BSTIM1_Handler(void)
{
    HAL_TIM_IRQHandler(&light_tim_hdl);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    io_toggle_pin(PA00);
}
#endif
