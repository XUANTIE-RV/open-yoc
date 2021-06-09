#ifndef __LSIWDT_H__
#define __LSIWDT_H__
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "HAL_def.h"

#define IWDT_SW_FEED_VAL   0xa55a1234
extern uint32_t iwdt_sw_flag;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))


/** @addtogroup RL3301xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup IWDG
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup IWDG_Exported_Constants
  * @{
  */

/** @defgroup IWDG_WriteAccess
  * @{
  */

#define IWDG_WriteAccess_Enable ((uint32_t)0x1ACCE551)
#define IWDG_WriteAccess_Disable ((uint32_t)0x00000000)

/**
  * @}
  */

/** @defgroup IWDG_ClockSel 
  * @{
  */
#define IWDG_lrc_select ((uint8_t)0x02)

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Prescaler and Counter configuration functions ******************************/
void IWDG_DeInit(void);
void IWDG_WriteAccessCmd(uint32_t IWDG_WriteAccess);
void IWDG_SetLoad(uint32_t Load);
uint32_t IWDG_GetCounterValue(void);

/* IWDG activation function ***************************************************/
void IWDG_Enable(FunctionalState NewState);
void IWDG_ITConfig(FunctionalState NewState);
void IWDG_ResetEnable(FunctionalState NewState);
void IWDG_SetClockCmd(uint8_t IWDG_ClockSel);

/* Flag management function ***************************************************/
ITStatus IWDG_GetITStatus(void);
void IWDG_ClearITFlag(void);

#ifdef __cplusplus
}
#endif

void lsiwdt_Init(void);
/*****************************END OF FILE*************************************/
#endif //(__LSIWDT_H__)

