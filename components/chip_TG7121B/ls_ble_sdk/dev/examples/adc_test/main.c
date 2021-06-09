#include "lsgpio.h"
#include "lsadc.h"
#include "le501x.h"
#include "reg_lsadc.h"
#include "reg_syscfg.h"
#include "field_manipulate.h"
#include "lsiwdt.h"
#include "sys_stat.h"
#include "io_config.h"
#include <string.h>

void Error_Handler(void);
void software_trig(void);
ADC_HandleTypeDef hadc;
static void lsgpio_init(void)
{
   adc12b_in4_io_init();
}

void lsadc_init(void)
{
    __HAL_RCC_ADC_CLK_ENABLE();

    ADC_ChannelConfTypeDef sConfig = {0};

    /** Common config 
  */
    hadc.Instance = LSADC;
    hadc.Init.DiscontinuousConvMode = ENABLE;
    hadc.ConvCpltCallback = HAL_ADC_ConvCpltCallback;
    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Regular Channel 
  */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void usr_adc_vbat_sample_func(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Channel  = ADC_CHANNEL_TEMPSENSOR_VBAT;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;//ADC_SAMPLETIME_15CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
       Error_Handler();
    }	
	  else
	  {
	    REG_FIELD_WR(hadc.Instance->CR2,ADC_BATADJ, 0x02);  //3/8 VBAT
		REG_FIELD_WR(SYSCFG->PMU_TRIM,SYSCFG_EN_BAT_DET,1);  //enable VBAT test
	  }	
	  HAL_ADC_Start_IT(&hadc);
}

int main(void)
{
    lsgpio_init();
    lsiwdt_Init();
    lsadc_init();
    NVIC_EnableIRQ(ADC_IRQn); //ADC_IRQHandler
    HAL_ADC_Start_IT(&hadc);
    while (1)
    {
        HAL_ADC_Start_IT(&hadc);
    }
    return 0;
}

/**
  * @brief  Conversion complete callback in non blocking mode 
  * @param  hadc: ADC handle
  * @retval None
  */
uint32_t rd_adc_data = 0;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    /* Prevent unused argument(s) compilation warning */
    //UNUSED(hadc);
    /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADC_ConvCpltCallback must be implemented in the user file.
   */
    rd_adc_data = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop_IT(hadc);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

void ADC_Handler(void)
{
    HAL_ADC_IRQHandler(&hadc);
}
