#ifndef LSGPIO_H_
#define LSGPIO_H_
#include <stdbool.h>
#include "reg_rcc.h"
#include "reg_lsgpio.h"
#include "HAL_def.h"
#include "field_manipulate.h"

#define GPIO_GET_INDEX(__GPIOx__) (((__GPIOx__) == (LSGPIOA))? 0uL :\
                                   ((__GPIOx__) == (LSGPIOB))? 1uL :\
                                   ((__GPIOx__) == (LSGPIOC))? 2uL :3uL)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup GPIO_Private_Constants GPIO Private Constants
  * @{
  */
#define GPIO_MODE             0x00000003u
#define EXTI_MODE             0x10000000u
#define EXTI_FLT_MODE         0x01000000u
#define GPIO_MODE_IT          0x00010000u
#define ESW_MODE_IT           0x00020000u
#define EAD_MODE_IT           0x00030000u
#define RISING_EDGE           0x00100000u
#define FALLING_EDGE          0x00200000u
#define RISING_FALLING_EDGE   0x00300000u

/**
  * @brief  GPIO Bit SET and Bit RESET enumeration
  */
typedef enum
{
    GPIO_PIN_RESET = 0u,
    GPIO_PIN_SET   = 1u
} GPIO_PinState;

/** @defgroup GPIO_pins_define 
  * @{
  */
#define GPIO_PIN_0 ((uint16_t)0x0001)   /* Pin 0 selected    */
#define GPIO_PIN_1 ((uint16_t)0x0002)   /* Pin 1 selected    */
#define GPIO_PIN_2 ((uint16_t)0x0004)   /* Pin 2 selected    */
#define GPIO_PIN_3 ((uint16_t)0x0008)   /* Pin 3 selected    */
#define GPIO_PIN_4 ((uint16_t)0x0010)   /* Pin 4 selected    */
#define GPIO_PIN_5 ((uint16_t)0x0020)   /* Pin 5 selected    */
#define GPIO_PIN_6 ((uint16_t)0x0040)   /* Pin 6 selected    */
#define GPIO_PIN_7 ((uint16_t)0x0080)   /* Pin 7 selected    */
#define GPIO_PIN_8 ((uint16_t)0x0100)   /* Pin 8 selected    */
#define GPIO_PIN_9 ((uint16_t)0x0200)   /* Pin 9 selected    */
#define GPIO_PIN_10 ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11 ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12 ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13 ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14 ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15 ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All ((uint16_t)0xFFFF) /* All pins selected */

#define GPIO_PIN_MASK 0x0000FFFFu /* PIN mask for assert test */

/** @defgroup GPIO AF define
  * @{
  */
enum GPIO_AF
{
    AF_JLINK_SWD = 0x00,
    AF_JLINK_SCK,

    AF_UART1_CK,
    AF_UART1_TXD,
    AF_UART1_RXD,
    AF_UART1_CTSN,
    AF_UART1_RTSN,

    AF_ANT_SW0,

    AF_UART2_TXD,
    AF_UART2_RXD,

    AF_ANT_SW1,
    AF_ANT_SW2,

    AF_PIS_CH0_OUT,
    AF_PIS_CH1_OUT,

    AF_ANT_SW3,
    AF_ANT_SW4,

    AF_I2C1_SCL,
    AF_I2C1_SDA,
    AF_I2C1_SMBA,

    AF_I2C2_SCL,
    AF_I2C2_SDA,

    AF_SPI1_NSS0,
    AF_SPI1_SCK,
    AF_SPI1_DQ0,
    AF_SPI1_DQ1,
    AF_SPI1_DQ2,
    AF_SPI1_DQ3,

    AF_SPI2_SCK,
    AF_SPI2_NSS,
    AF_SPI2_MOSI,
    AF_SPI2_MISO,

    AF_ADTIM1_CH1,

    AF_ADTIM1_CH1N,
    AF_ADTIM1_CH2,
    AF_ADTIM1_CH2N,
    AF_ADTIM1_CH3,
    AF_ADTIM1_CH3N,
    AF_ADTIM1_CH4,
    AF_ADTIM1_ETR,
    AF_ADTIM1_BK,

    AF_GPTIMA1_CH1,
    AF_GPTIMA1_CH2,
    AF_GPTIMA1_CH3,
    AF_GPTIMA1_CH4,
    AF_GPTIMA1_ETR,

    AF_GPTIMB1_CH1,
    AF_GPTIMB1_CH2,
    AF_GPTIMB1_CH3,
    AF_GPTIMB1_CH4,
    AF_GPTIMB1_ETR,

    AF_GPTIMC1_CH1,
    AF_GPTIMC1_CH1N,
    AF_GPTIMC1_CH2,
    AF_GPTIMC1_BK,

    AF_LPTIM_OUT,

    AF_ANT_SW5,

    AF_PDM_CLK,

    AF_UART3_TXD,
    AF_UART3_RXD,

    AF_PDM_DATA0,
    AF_PDM_DATA1,

    AF_ANT_SW6,
    AF_SPI1_NSS1,
    AF_I2S_CLK
};

enum GPIO_ANA_FUNC
{
    ANA_FUNC_DIS = 0,
    ANA_FUNC1,
    ANA_FUNC2
};
/** @defgroup GPIO_mode define 
  */
#define SET_GPIO_MODE_GPIO 0x00000000u /*!< gpio Mode    */
#define SET_GPIO_MODE_ANALOG 0x00000001u   /*!< Analog Mode  */
#define SET_GPIO_MODE_AF 0x00000002u       /*!< Alternate Function Mode  */
#define SET_GPIO_MODE_TEST 0x00000003u     /*!<  Test Mode        */

/** @defgroup GPIO_InPut_define
  * @brief GPIO Pull-Up or Pull-Down Activation
  * @{
  */
#define GPIO_INPUT_NOPULL 0x00000000u   /*!< No Pull-up or Pull-down activation  */
#define GPIO_INPUT_PULLUP 0x00000001u   /*!< Pull-up activation                  */
#define GPIO_INPUT_PULLDOWN 0x00000002u /*!< Pull-down activation                */

#define GPIO_INPUT_DIS 0x00000000u /*!< enable input        */
#define GPIO_INPUT_EN 0x00000001u /*!< enable input        */

/** @defgroup GPIO_OutPut define
  * @brief GPIO open Drain or GPIO push pull Activation
  * @{
  */
#define GPIO_OUTPUT_PUSHPLL 0x00000000u   /*push pull activation */
#define GPIO_OUTPUT_OPENDRAIN 0x00000001u /*open drain activation */

#define GPIO_OUTPUT_DIS 0x00000000u /*!< enable input        */
#define GPIO_OUTPUT_EN 0x00000001u /*!< enable input        */
/** @defgroup GPIO OutPut Driver Power define
  * @brief 
  * @{
  */
#define GPIO_OUTPUT_1_4_MAX_DRIVER 0x00000000u /*1/4 max driver */
#define GPIO_OUTPUT_1_2_MAX_DRIVER 0x00000002u /*push pull activation */
#define GPIO_OUTPUT_MAX_DRIVER 0x00000003u     /*max driver */

/** @defgroup GPIO Input Filter define
  * @brief 
  * @{
  */
#define GPIO_INPUT_FLI_20NS_DIS 0x00000000u /*input 20ns filter function disable */
#define GPIO_INPUT_FLI_20NS_EN 0x00000001u  /*input 20ns filter function enable */

/** @defgroup GPIO Analog Function
  * @brief 
  * @{
  */
#define GPIO_ANA_DIS 0x00000000u /*analog function disable */
#define GPIO_ANA_EN 0x00000001u  /*analog function enable */

/**
  * @}
  */

/** @defgroup GPIO_mode_define GPIO mode define
  * @brief GPIO Configuration Mode
  *        Elements values convention: hex(Xxyz000Z)
  *           - X  : GPIO mode or EXTI Mode
  *           - x  :EXTI Mode filter Function
  *           - y  : External IT  trigger detection
  *           - z  : IO configuration on External IT
  *           - Z  : IO Direction mode (Input, Output, Alternate or Analog)
  * @{
  */
#define    GPIO_MODE_INPUT                      0x00000000u /*!< Input  Mode                   */
#define    GPIO_MODE_OUTPUT                     0x00000001u /*!< Output  Mode                 */

#define    GPIO_MODE_AF                         0x00000002u/*!< Alternate Function  Mode     */
#define    GPIO_MODE_ANALOG                     0x00000003u /*!< Analog Mode  */
#define    GPIO_MODE_TEST                       0x00000004u   /*!< Test Mode  */

#define    GPIO_MODE_IT_RISING                  0x10110000u/*!< External Interrupt Mode with Rising edge trigger detection          */
#define    GPIO_MODE_IT_RISING_FLT              0x11110000u/*!< External Interrupt Mode with Rising edge trigger detection and filter function         */
#define    GPIO_MODE_IT_FALLING                 0x10210000u/*!< External Interrupt Mode with Falling edge trigger detection         */
#define    GPIO_MODE_IT_FALLING_FLT             0x11210000u/*!< External Interrupt Mode with Falling edge trigger detectionand and filter function         */
#define    GPIO_MODE_IT_RISING_FALLING          0x10310000u/*!< External Interrupt Mode with Rsing/Falling edge trigger detection         */
#define    GPIO_MODE_IT_RISING_FALLING_FLT      0x11310000u/*!< External Interrupt Mode with Rsing/Falling edge trigger detectionand and filter function         */
#define    GPIO_MODE_IT_EAD                     0x10130000u/*!<External Interrupt Mode with external AD trigger detection  */
#define    GPIO_MODE_IT_EAD_FLT                 0x11130000u/*!<External Interrupt Mode with external AD trigger detection   and filter function */

/**
  * @brief Exti GPIO filter parameter structure definition
  */
typedef struct  __attribute__((packed))
{
      uint8_t Cnt;
      uint16_t Pre; 
}GPIO_EdbcTypeDef;

/**
  * @brief GPIO Init structure definition
  */
typedef struct  __attribute__((packed))
{
    uint32_t Pin; /*!< Specifies the GPIO pins to be configured.
                           This parameter can be any value of @ref GPIO_pins_define  */

    uint32_t Mode; /*!< Specifies the operating mode for the selected pins.
                           This parameter can be a value of @ref GPIO_mode define  */

    uint32_t Pull; /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                           This parameter can be a value of @ref GPIO_pull_define */

    uint32_t OT;  /*!< Specifies the Push Pull or Open Drain activation for the selected pins.
                           This parameter can be a value of @ref GPIO_OutPut define */                       

    uint32_t Driver_Pwr; /*!< Specifies the driver for the selected pins.
                           This parameter can be a value of @ref GPIO OutPut Driver Power define */

    uint32_t Filter; /*!< Specifies the Filter for the selected pins.
                           This parameter can be a value of @ref GPIO Input Filter define */

    uint32_t AF_Type; /*!< Specifies GPIO Alternate Function  for the selected pins.
                           This parameter can be a value of @ref GPIO AF define */

    uint32_t ANA_Func; /*!< Specifies GPIO ANA FUNC??for the selected pins.
                           This parameter can be a value of @ref GPIO ANA FUNC define */

    GPIO_EdbcTypeDef Edbc_filter; /*!< Specifies EXTI GPIO EDBC Param for filter.*/
} GPIO_InitTypeDef;

/* Exported macro ------------------------------------------------------------*/
/** @defgroup GPIO_Exported_Macros GPIO Exported Macros
  * @{
  */

/**
  * @brief  Checks whether the specified EXTI line flag is set or not.
  * @param  __EXTI_LINE__: specifies the EXTI line flag to check.
  *         This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#define __HAL_GPIO_EXTI_GET_FLAG(__EXTI_LINE__) (EXTI->ERIF & (__EXTI_LINE__))

/**
  * @brief  Clears the EXTI's line pending flags.
  * @param  __EXTI_LINE__: specifies the EXTI lines flags to clear.
  *         This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#define __HAL_GPIO_EXTI_CLEAR_FLAG(__EXTI_LINE__) (EXTI->EICR = (__EXTI_LINE__))


/**
  * @brief  Checks whether the specified EXTI line flag is set or not.
  * @param  __EXTI_LINE__: specifies the EXTI line flag to check.
  *         This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#define __HAL_GPIO_EXTI_GET_IT(__EXTI_LINE__) (EXTI->ERIF & (__EXTI_LINE__))

/**
  * @brief  Clears the EXTI's line pending flags.
  * @param  __EXTI_LINE__: specifies the EXTI lines flags to clear.
  *         This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#define __HAL_GPIO_EXTI_CLEAR_IT(__EXTI_LINE__) (EXTI->EICR = (__EXTI_LINE__))
/**
  * @brief  Generates a Software interrupt on selected EXTI line.
  * @param  __EXTI_LINE__: specifies the EXTI line to check.
  *          This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval None
  */
#define __HAL_GPIO_EXTI_GENERATE_SWIT(__EXTI_LINE__) (EXTI->ESWI |= (__EXTI_LINE__))

/**
  * @brief  Configure GPIOA AHBclock, sleep and deep sleep
  * @param  .
  * @retval None
  */

#define __HAL_RCC_GPIOA_CLK_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBEN, RCC_GPIOA, 1);\
                                      } while(0U)

#define __HAL_RCC_GPIOA_CLK_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBEN, RCC_GPIOA, 0);\
                                      } while(0U)

#define __HAL_RCC_GPIOA_SLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBSL, RCC_GPIOA, 1);\
                                      } while(0U)    

#define __HAL_RCC_GPIOA_SLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBSL, RCC_GPIOA, 0);\
                                      } while(0U)                                        
 

#define __HAL_RCC_GPIOA_DSLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBDSL, RCC_GPIOA, 1);\
                                      } while(0U)     

#define __HAL_RCC_GPIOA_DSLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBDSL, RCC_GPIOA, 0);\
                                      } while(0U)  
/**
  * @brief  Configure GPIOB AHBclock, sleep and deep sleep
  * @param  GPIO type
  * @retval None
  */
#define __HAL_RCC_GPIOB_CLK_ENABLE()   do { \
                                        REG_FIELD_WR(RCC->AHBEN, RCC_GPIOB, 1);\
                                      } while(0U)

#define __HAL_RCC_GPIOB_CLK_DISABLE()   do { \
                                        REG_FIELD_WR(RCC->AHBEN, RCC_GPIOB, 0);\
                                      } while(0U)                                      

#define __HAL_RCC_GPIOB_SLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBSL, RCC_GPIOB, 1);\
                                      } while(0U)    

#define __HAL_RCC_GPIOB_SLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBSL, RCC_GPIOB, 0);\
                                      } while(0U)                                       
 

#define __HAL_RCC_GPIOB_DSLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBDSL, RCC_GPIOB, 1);\
                                      } while(0U) 

#define __HAL_RCC_GPIOB_DSLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBDSL, RCC_GPIOB, 0);\
                                      } while(0U)  
 
/**
  * @brief  Configure GPIOC AHBclock, sleep and deep sleep
  * @param  .
  * @retval None
  */                                     
#define __HAL_RCC_GPIOC_CLK_ENABLE()   do { \
                                        REG_FIELD_WR(RCC->AHBEN, RCC_GPIOC, 1);\
                                      } while(0U)

#define __HAL_RCC_GPIOC_SLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBSL, RCC_GPIOC, 1);\
                                      } while(0U)    
 
#define __HAL_RCC_GPIOC_SLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBSL, RCC_GPIOC, 0);\
                                      } while(0U) 


#define __HAL_RCC_GPIOC_DSLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBDSL, RCC_GPIOC, 1);\
                                      } while(0U)      

#define __HAL_RCC_GPIOC_DSLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->AHBDSL, RCC_GPIOC, 0);\
                                      } while(0U)  
/**
  * @}
  */
/** @addtogroup GPIO_Exported_Functions_Group1
  * @{
  */
/* IO operation functions *****************************************************/
void HAL_GPIO_Init(reg_lsgpio_t *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(reg_lsgpio_t *GPIOx, uint32_t GPIO_Pin);

/** @addtogroup GPIO_Exported_Functions_Group2
  * @{
  */
/* IO operation functions *****************************************************/
GPIO_PinState HAL_GPIO_ReadPin(reg_lsgpio_t *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(reg_lsgpio_t *GPIOx, uint16_t GPIO_Pin, GPIO_PinState const PinState);
void HAL_GPIO_TogglePin(reg_lsgpio_t *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(reg_lsgpio_t *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_GPIO_SET_INPUT(reg_lsgpio_t *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_SET_INPUT_FILT_EN(reg_lsgpio_t *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_SET_OUTPUT(reg_lsgpio_t *GPIOx, uint16_t GPIO_Pin);

																			
#endif //(LSGPIO_H_)
