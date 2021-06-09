#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "le501x.h"
#include "ls_ble.h"
#include "platform.h"
#include "cpu.h"
#include "field_manipulate.h"
#include "reg_lsiwdt.h"
#include "reg_lsiwdt_type.h"
#include "lsiwdt.h"
#include "reg_rcc.h"
#include "reg_syscfg_type.h"
#include "reg_syscfg.h"
#include "log.h"

uint32_t iwdt_sw_flag = 0xa55a1234;
/** @addtogroup aqua_StdPeriph_Driver
  * @{
  */

/** @defgroup IWDG 
  * @brief IWDG driver modules
  * @{
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup IWDG_Private_Functions
  * @{
  */

/** @defgroup IWDG_Group1 Prescaler and Counter configuration functions
 *  @brief   Prescaler and Counter configuration functions
 *
@verbatim   
  ==============================================================================
            ##### Prescaler and Counter configuration functions #####
  ==============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the IWDG peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void IWDG_DeInit(void)
{
    REG_FIELD_WR(RCC->AHBRST, RCC_IWDT, 1);
    REG_FIELD_WR(RCC->AHBRST, RCC_IWDT, 0);
}

/**
  * @brief  Enables or disables write access to IWDG_PR and IWDG_RLR registers.
  * @param  IWDG_WriteAccess: new state of write access to IWDG_PR and IWDG_RLR registers.
  *          This parameter can be one of the following values:
  *            @arg IWDG_WriteAccess_Enable: Enable write access to IWDG_PR and IWDG_RLR registers
  *            @arg IWDG_WriteAccess_Disable: Disable write access to IWDG_PR and IWDG_RLR registers
  * @retval None
  */
void IWDG_WriteAccessCmd(uint32_t IWDG_WriteAccess)
{
    /* Check the parameters */
    REG_FIELD_WR(LSIWDT->IWDT_LOCK, IWDT_LOCK, IWDG_WriteAccess);
}

/**
  * @brief  Sets IWDG Load value.
  * @param  Reload: specifies the IWDG Load value.
  *          This parameter must be a number between 0 and 0xFFFFFFFF.
  * @retval None
  */
void IWDG_SetLoad(uint32_t Load)
{
    /* Check the parameters */
    REG_FIELD_WR(LSIWDT->IWDT_LOAD, IWDT_LOAD, Load);
}

/**
  * @brief  Return the IWDG Current Counter Value
  * @param  None
  * @retval None
  */
uint32_t IWDG_GetCounterValue(void)
{
    /* Return the IWDG Current Counter Value */
    return (uint32_t)REG_FIELD_RD(LSIWDT->IWDT_VALUE, IWDT_VALUE);
}

/**
  * @}
  */

/** @defgroup IWDG_Group2 IWDG activation function
 *  @brief   IWDG activation function 
 *
@verbatim   
 ==============================================================================
                          ##### IWDG activation function #####
 ==============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables IWDG .
  * @param  NewState: new state of the IWDG peripheral. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void IWDG_Enable(FunctionalState NewState)
{
    if (NewState != DISABLE)
    {
        REG_FIELD_WR(LSIWDT->IWDT_CON, IWDT_EN, 1);
    }
    else
    {
        REG_FIELD_WR(LSIWDT->IWDT_CON, IWDT_EN, 0);
    }
}

/**
  * @brief  Enables IWDG Interrupt
  * @param  NewState: new state of the IWDG peripheral. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void IWDG_ITConfig(FunctionalState NewState)
{
    if (NewState != DISABLE)
    {
        REG_FIELD_WR(LSIWDT->IWDT_CON, IWDT_IE, 1);
    }
    else
    {
        REG_FIELD_WR(LSIWDT->IWDT_CON, IWDT_IE, 0);
    }
}

/**
  * @brief  Enables IWDG Reset 
  * @param  NewState: new state of the IWDG peripheral. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void IWDG_ResetEnable(FunctionalState NewState)
{
    if (NewState != DISABLE)
    {
        REG_FIELD_WR(LSIWDT->IWDT_CON, IWDT_RSTEN, 1);
    }
    else
    {
        REG_FIELD_WR(LSIWDT->IWDT_CON, IWDT_RSTEN, 0);
    }
}

/**
  * @brief  Sets IWDG Counter Clock.
  * @param  IWDG_ClockSel: specifies the IWDG Counter Clock.
  *          This parameter can be one of the following values:
  *            @arg IWDG_hrc_select: IWDG Counter Clock is HRC/64 Clock
  *            @arg IWDG_lse_select: IWDG Counter Clock is LSE Clock
  *            @arg IWDG_lrc_select: IWDG Counter Clock is LRC Clock
  *            @arg IWDG_hse_select: IWDG Counter Clock is HSE/512 Clock
  * @retval None
  */
void IWDG_SetClockCmd(uint8_t IWDG_ClockSel)
{
    /* Check the parameters */
    REG_FIELD_WR(LSIWDT->IWDT_CON, IWDT_CLKS, IWDG_ClockSel);
}

/**
  * @}
  */

/** @defgroup IWDG_Group3 Flag management function 
 *  @brief  Flag management function  
 *
@verbatim   
 ===============================================================================
                      ##### Flag management function ##### 
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the specified IWDG flag is set or not.
  * @param  IWDG_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg IWDG_RIS_IT: IWDG Interrrupt Flag
  * @retval The new state of IWDG_IT (SET or RESET).
  */
ITStatus IWDG_GetITStatus(void)
{
    ITStatus bitstatus = RESET;

    if (REG_FIELD_RD(LSIWDT->IWDT_RIS, IWDT_WDTIF) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    /* Return the flag status */
    return bitstatus;
}

/**
  * @brief  Clears the IWDG pending flags.
  * @param  IWDG_FLAG: specifies the flag to clear. 
  *          This parameter can be any combination of the following values:
  *            @arg IWDG_RIS_IT    : IWDG Interrrupt Flag
  * @retval The new state of IWDG_IT (SET or RESET).
  */
void IWDG_ClearITFlag(void)
{
    if(iwdt_sw_flag == IWDT_SW_FEED_VAL)
    {
        iwdt_sw_flag=0;
    /* Clear the selected flag */
         REG_FIELD_WR(LSIWDT->IWDT_INTCLR, IWDT_INTCLR, 1);
    }

    for (uint8_t i = 0; i < 200; i++)  //200 delay count
    {
        REG_FIELD_RD(LSIWDT->IWDT_INTCLR, IWDT_INTCLR);
    }
}

void IWDT_Handler(void)
{
    IWDG_ClearITFlag();
}

void lsiwdt_Init(void)
{
    REG_FIELD_WR(SYSCFG->CFG, SYSCFG_IWDG_DEBUG, 1);
    REG_FIELD_WR(RCC->CK, RCC_HSE_EN, 1);
    REG_FIELD_WR(RCC->CK, RCC_LSI_EN, 1);
    REG_FIELD_WR(RCC->AHBEN, RCC_IWDT, 1);

    IWDG_DeInit();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetClockCmd(IWDG_lrc_select);
    IWDG_SetLoad(0x27100); //period time 5s  5000000/((1/32)*1000)us  //32khz
    IWDG_ClearITFlag();
    arm_cm_set_int_isr(IWDT_IRQn,IWDT_Handler);
    NVIC_EnableIRQ(IWDT_IRQn);
    IWDG_ITConfig(ENABLE);
    IWDG_ResetEnable(ENABLE);
    IWDG_Enable(ENABLE);
}
/*****************************END OF FILE***************************/
