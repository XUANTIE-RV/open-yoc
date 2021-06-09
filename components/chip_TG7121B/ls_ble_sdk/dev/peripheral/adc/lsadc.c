#include <stdio.h>
#include <stdint.h>
#include "le501x.h"
#include "ls_ble.h"
#include "platform.h"
#include "cpu.h"
#include "field_manipulate.h"
#include "reg_lsadc.h"
#include "reg_lsadc_type.h"
#include "lsadc.h"
#include "reg_rcc.h"
#include "reg_syscfg_type.h"
#include "reg_syscfg.h"
#include "HAL_def.h"
#include "log.h"

/* Includes ------------------------------------------------------------------*/

/** @addtogroup _HAL_Driver
  * @{
  */

/** @defgroup ADC ADC
  * @brief ADC HAL module driver
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup ADC_Private_Constants ADC Private Constants
  * @{
  */

/* Timeout values for ADC enable and disable settling time.                 */
/* Values defined to be higher than worst cases: low clocks freq,           */
/* maximum prescaler.                                                       */
/* Ex of profile low frequency : Clock source at 0.1 MHz, ADC clock         */
/* prescaler 4, sampling time 12.5 ADC clock cycles, resolution 12 bits.    */
/* Unit: ms                                                                 */
#define ADC_ENABLE_TIMEOUT 2U
#define ADC_DISABLE_TIMEOUT 2U

/* Delay for ADC stabilization time.                                        */
/* Maximum delay is 1us (refer to device datasheet, parameter tSTAB).       */
/* Unit: us                                                                 */
#define ADC_STAB_DELAY_US 1U

/* Delay for temperature sensor stabilization time.                         */
/* Maximum delay is 10us (refer to device datasheet, parameter tSTART).     */
/* Unit: us                                                                 */
#define ADC_TEMPSENSOR_DELAY_US 10U

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup ADC_Private_Functions ADC Private Functions
  * @{
  */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup ADC_Exported_Functions ADC Exported Functions
  * @{
  */

/** @defgroup ADC_Exported_Functions_Group1 Initialization/de-initialization functions 
  * @brief    Initialization and Configuration functions
  *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the ADC. 
      (+) De-initialize the ADC.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the ADC peripheral and regular group according to  
  *         parameters specified in structure "ADC_InitTypeDef".
  * @note   As prerequisite, ADC clock must be configured at RCC top level
  *         (clock source APB2).
  *         See commented example code below that can be copied and uncommented 
  *         into HAL_ADC_MspInit().
  * @note   Possibility to update parameters on the fly:
  *         This function initializes the ADC MSP (HAL_ADC_MspInit()) only when
  *         coming from ADC state reset. Following calls to this function can
  *         be used to reconfigure some parameters of ADC_InitTypeDef  
  *         structure on the fly, without modifying MSP configuration. If ADC  
  *         MSP has to be modified again, HAL_ADC_DeInit() must be called
  *         before HAL_ADC_Init().
  *         The setting of these parameters is conditioned to ADC state.
  *         For parameters constraints, see comments of structure 
  *         "ADC_InitTypeDef".
  * @note   This function configures the ADC within 2 scopes: scope of entire 
  *         ADC and scope of regular group. For parameters details, see comments 
  *         of structure "ADC_InitTypeDef".
  * @param  hadc: ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *hadc)
{
    HAL_StatusTypeDef tmp_hal_status = HAL_OK;
    uint32_t tmp_cr2 = 0U;
    uint32_t tmp_ccr = 0U;

    /* Check ADC handle */
    if (hadc == NULL)
    {
        return HAL_STATE_ERROR;
    }

    tmp_cr2 = FIELD_BUILD(ADC_CONT, (uint32_t)hadc->Init.ContinuousConvMode) |
              FIELD_BUILD(ADC_BATADJ, 0) |
              FIELD_BUILD(ADC_BINRES, 1) |
              FIELD_BUILD(ADC_BINBUF, 1) |
              FIELD_BUILD(ADC_EINBUF, 0) |
              FIELD_BUILD(ADC_TEST, 0) |
              FIELD_BUILD(ADC_DIFF, 0);

    /* Update ADC configuration register CR2 with previous settings */
    MODIFY_REG(hadc->Instance->CR2,
                   ADC_CONT_MASK   |
                   ADC_BATADJ_MASK |
                   ADC_BINRES_MASK |
                   ADC_BINBUF_MASK |
                   ADC_EINBUF_MASK |
                   ADC_TEST_MASK   |
                   ADC_DIFF_MASK,
                   tmp_cr2);

    tmp_ccr = FIELD_BUILD(ADC_MSBCAL, 2)  |
              FIELD_BUILD(ADC_VRPS, 4)    |
              FIELD_BUILD(ADC_VRBUFEN, 1) |
              FIELD_BUILD(ADC_BP, 0)      |
              FIELD_BUILD(ADC_VCMEN, 1)   |
              FIELD_BUILD(ADC_VREFEN, 1)  |
              FIELD_BUILD(ADC_LPCTL, 1)   |
              FIELD_BUILD(ADC_GCALV, 0)   |
              FIELD_BUILD(ADC_OCALV, 0)   |
              FIELD_BUILD(ADC_CKDIV, 4); //default 2Mhz

    MODIFY_REG(hadc->Instance->CCR,
               ADC_MSBCAL_MASK      |
                   ADC_VRPS_MASK    |
                   ADC_VRBUFEN_MASK |
                   ADC_BP_MASK      |
                   ADC_VCMEN_MASK   |
                   ADC_LPCTL_MASK   |
                   ADC_GCALV_MASK   |
                   ADC_OCALV_MASK   |
                   ADC_CKDIV_MASK,
               tmp_ccr);
    /* Return function status */
    return tmp_hal_status;
}

/**
  * @brief  Configures the the selected channel to be linked to the regular
  *         group.
  * @note   In case of usage of internal measurement channels:
  *         Vbat/VrefInt/TempSensor.
  *         These internal paths can be be disabled using function 
  *         HAL_ADC_DeInit().
  * @note   Possibility to update parameters on the fly:
  *         This function initializes channel into regular group, following  
  *         calls to this function can be used to reconfigure some parameters 
  *         of structure "ADC_ChannelConfTypeDef" on the fly, without reseting 
  *         the ADC.
  *         The setting of these parameters is conditioned to ADC state.
  *         For parameters constraints, see comments of structure 
  *         "ADC_ChannelConfTypeDef".
  * @param  hadc: ADC handle
  * @param  sConfig: Structure of ADC channel for regular group.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig)
{
    HAL_StatusTypeDef tmp_hal_status = HAL_OK;

    //    MODIFY_REG(hadc->Instance->SMPR1                             ,
    //               ADC_SMPR1(ADC_SMPR1_SMP10, sConfig->Channel)       ,
    //               ADC_SMPR1(sConfig->SamplingTime, sConfig->Channel) );
    //
    //    MODIFY_REG(hadc->Instance->RSQR1, ADC_RSQ1_MASK,sConfig->Channel);
    if (sConfig->Channel == ADC_CHANNEL_TEMPSENSOR_VBAT)
		{
				REG_FIELD_WR(hadc->Instance->SMPR1, ADC_SMP10, sConfig->SamplingTime); //select adc sample
			  REG_FIELD_WR(hadc->Instance->RSQR1, ADC_RSQ1, sConfig->Channel); //select ADC channel 4
			  
		}
		else
		{
			REG_FIELD_WR(hadc->Instance->SMPR1, ADC_SMP4, sConfig->SamplingTime); //select adc sample
			REG_FIELD_WR(hadc->Instance->RSQR1, ADC_RSQ1, sConfig->Channel); //select ADC channel 4
		}
		
    /* Return function status */
    return tmp_hal_status;
}

/** @defgroup ADC_Private_Functions ADC Private Functions
  * @{
  */

/**
  * @brief  Enable the selected ADC.
  * @note   Prerequisite condition to use this function: ADC must be disabled
  *         and voltage regulator must be enabled (done into HAL_ADC_Init()).
  * @param  hadc: ADC handle
  * @retval HAL status.
  */
HAL_StatusTypeDef ADC_Enable(ADC_HandleTypeDef *hadc)
{
    /* ADC enable and wait for ADC ready (in case of ADC is disabled or         */
    /* enabling phase not yet completed: flag ADC ready not yet set).           */
    /* Timeout implemented to not be stuck if ADC cannot be enabled (possible   */
    /* causes: ADC clock not running, ...).                                     */
    if (ADC_IS_ENABLE(hadc) == RESET)
    {
        /* Enable the Peripheral */
        __HAL_ADC_ENABLE(hadc);
    }

    /* Return HAL status */
    return HAL_OK;
}

/**
  * @brief  Stop ADC conversion and disable the selected ADC
  * @note   Prerequisite condition to use this function: ADC conversions must be
  *         stopped to disable the ADC.
  * @param  hadc: ADC handle
  * @retval HAL status.
  */
HAL_StatusTypeDef ADC_ConversionStop_Disable(ADC_HandleTypeDef* hadc)
{
  
  /* Verification if ADC is not already disabled */
  if (ADC_IS_ENABLE(hadc) != RESET)
  {
    /* Disable the ADC peripheral */
    __HAL_ADC_DISABLE(hadc);
     
  }
  
  /* Return HAL status */
  return HAL_OK;
}
/** @defgroup ADC_Exported_Functions_Group2 IO operation functions
 *  @brief    Input and Output operation functions
 *
@verbatim   
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Start conversion of regular group.
      (+) Stop conversion of regular group.
      (+) Poll for conversion complete on regular group.
      (+) Poll for conversion event.
      (+) Get result of regular channel conversion.
      (+) Start conversion of regular group and enable interruptions.
      (+) Stop conversion of regular group and disable interruptions.
      (+) Handle ADC interrupt request
      (+) Start conversion of regular group and enable DMA transfer.
      (+) Stop conversion of regular group and disable ADC DMA transfer.
@endverbatim
  * @{
  */

/**
  * @brief  Enables ADC, starts conversion of regular group.
  *         Interruptions enabled in this function: None.
  * @param  hadc: ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc)
{
    HAL_StatusTypeDef tmp_hal_status = HAL_OK;

    /* Enable the ADC peripheral */
    tmp_hal_status = ADC_Enable(hadc);

    /* Start conversion if ADC is effectively enabled */
    if (tmp_hal_status == HAL_OK)
    {

        /* Clear regular group conversion flag */
        /* (To ensure of no unknown state from potential previous ADC operations) */
        __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC);

        /* Enable conversion of regular group.                                    */
        /* If software start has been selected, conversion starts immediately.    */
        /* If external trigger has been selected, conversion will start at next   */
        /* trigger event.                                                         */
        /* Case of multimode enabled:                                             */
        /*  - if ADC is slave, ADC is enabled only (conversion is not started).   */
        /*  - if ADC is master, ADC is enabled and conversion is started.         */
        /* If ADC is master, ADC is enabled and conversion is started.            */
        /* Note: Alternate trigger for single conversion could be to force an     */
        /*       additional set of bit ADON "hadc->Instance->CR2 |= ADC_CR2_ADON;"*/

        /* Start ADC conversion on regular group with SW start */
        SET_BIT(hadc->Instance->CR2, ADC_CR2_SWSTART);
    }

    /* Return function status */
    return tmp_hal_status;
}

/**
  * @brief  Stop ADC conversion of regular group (and injected channels in 
  *         case of auto_injection mode), disable ADC peripheral.
  * @note:  ADC peripheral disable is forcing stop of potential 
  *         conversion on injected group. If injected group is under use, it
  *         should be preliminarily stopped using HAL_ADCEx_InjectedStop function.
  * @param  hadc: ADC handle
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *hadc)
{
    HAL_StatusTypeDef tmp_hal_status = HAL_OK;

    /* Stop potential conversion on going, on regular and injected groups */
    /* Disable ADC peripheral */
    tmp_hal_status = ADC_ConversionStop_Disable(hadc);

    /* Return function status */
    return tmp_hal_status;
}
/**
  * @brief  Enables ADC, starts conversion of regular group with interruption.
  *         Interruptions enabled in this function:
  *          - EOC (end of conversion of regular group)
  *         Each of these interruptions has its dedicated callback function.
  * @param  hadc: ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc)
{
    HAL_StatusTypeDef tmp_hal_status = HAL_OK;

    /* Enable the ADC peripheral */
    tmp_hal_status = ADC_Enable(hadc);

    /* Start conversion if ADC is effectively enabled */
    if (tmp_hal_status == HAL_OK)
    {

        /* Clear regular group conversion flag */
        /* (To ensure of no unknown state from potential previous ADC operations) */
        __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC);

        /* Enable end of conversion interrupt for regular group */
        __HAL_ADC_ENABLE_IT(hadc, ADC_IT_EOC);

        /* Start ADC conversion on regular group with SW start */
        SET_BIT(hadc->Instance->CR2, ADC_CR2_SWSTART);
    }

    /* Return function status */
    return tmp_hal_status;
}

/**
  * @brief  Stop ADC conversion of regular group (and injected group in 
  *         case of auto_injection mode), disable interrution of 
  *         end-of-conversion, disable ADC peripheral.
  * @param  hadc: ADC handle
  * @retval None
  */
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef *hadc)
{
    HAL_StatusTypeDef tmp_hal_status = HAL_OK;

    /* Disable ADC end of conversion interrupt for regular group */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);

    /* Return function status */
    return tmp_hal_status;
}

/**
  * @brief  Get ADC regular group conversion result.
  * @note   Reading register DR automatically clears ADC flag EOC
  *         (ADC group regular end of unitary conversion).
  * @note   This function does not clear ADC flag EOS 
  *         (ADC group regular end of sequence conversion).
  *         Occurrence of flag EOS rising:
  *          - If sequencer is composed of 1 rank, flag EOS is equivalent
  *            to flag EOC.
  *          - If sequencer is composed of several ranks, during the scan
  *            sequence flag EOC only is raised, at the end of the scan sequence
  *            both flags EOC and EOS are raised.
  *         To clear this flag, either use function: 
  *         in programming model IT: @ref HAL_ADC_IRQHandler(), in programming
  *         model polling: @ref HAL_ADC_PollForConversion() 
  *         or @ref __HAL_ADC_CLEAR_FLAG(&hadc, ADC_FLAG_EOS).
  * @param  hadc: ADC handle
  * @retval ADC group regular conversion data
  */
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc)
{
    /* Note: EOC flag is not cleared here by software because automatically     */
    /*       cleared by hardware when reading register DR.                      */

    /* Return ADC converted value */
    return REG_FIELD_RD(hadc->Instance->RDR, ADC_RDATA);
}

/**
  * @brief  Handles ADC interrupt request  
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_IRQHandler(ADC_HandleTypeDef *hadc)
{

    /* ========== Check End of Conversion flag for regular group ========== */
    if (__HAL_ADC_GET_IT_SOURCE(hadc, ADC_IT_EOC))
    {
        if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC))
        {
            /* Conversion complete callback */
            hadc->ConvCpltCallback(hadc);

            /* Clear regular group conversion flag */
            __HAL_ADC_CLEAR_FLAG(hadc, ADC_REOCC_MASK | ADC_RSTRTCC_MASK);
        }
    }
}
