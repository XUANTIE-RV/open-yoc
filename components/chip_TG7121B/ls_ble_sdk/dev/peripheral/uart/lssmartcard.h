/**
  ******************************************************************************
  * @file    le501x_hal_smartcard.h
  * @author  SOC AE Team
  * @brief   Header file of SMARTCARD HAL module.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LE501x_HAL_SMARTCARD_H
#define __LE501x_HAL_SMARTCARD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "reg_uart.h"
#include "HAL_def.h"

	 
#define SMARTCARD_CLOCK   (SDK_PCLK_MHZ*1000000)
#define SMARTCARD_BUADRATE_ENUM_GEN(BAUD)  (SMARTCARD_CLOCK/BAUD)
	 
	 
/** @addtogroup le501x_HAL_Driver
  * @{
  */

/** @addtogroup SMARTCARD
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SMARTCARD_Exported_Types SMARTCARD Exported Types
  * @{
  */

/**
  * @brief SMARTCARD Init Structure definition
  */
typedef struct
{
  uint32_t BaudRate;                  /*!< This member configures the SmartCard communication baud rate.
                                           The baud rate is computed using the following formula:
                                           - IntegerDivider = ((PCLKx) / (16 * (hsc->Init.BaudRate))) */
  uint32_t WordLength;                /*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref SMARTCARD_Word_Length */

  uint32_t StopBits;                  /*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref SMARTCARD_Stop_Bits */

  uint32_t Parity;                    /*!< Specifies the parity mode.
                                           This parameter can be a value of @ref SMARTCARD_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits).*/

  uint32_t Mode;                      /*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref SMARTCARD_Mode */

  uint32_t Prescaler;                 /*!< Specifies the SmartCard Prescaler value used for dividing the system clock
                                           to provide the smartcard clock. The value given in the register (5 significant bits)
                                           is multiplied by 2 to give the division factor of the source clock frequency.
                                           This parameter can be a value of @ref SMARTCARD_Prescaler */

  uint32_t GuardTime;                 /*!< Specifies the SmartCard Guard Time value in terms of number of baud clocks */

  uint32_t NACKState;                 /*!< Specifies the SmartCard NACK Transmission state. */

  uint32_t Retry;                 		/*!< Specifies the SmartCard Retry transmit Time value */
	
}SMARTCARD_InitTypeDef;

/**
  * @brief HAL SMARTCARD State structures definition
  * @note  HAL SMARTCARD State value is a combination of 2 different substates: gState and RxState.
  *        - gState contains SMARTCARD state information related to global Handle management
  *          and also information related to Tx operations.
  *          gState value coding follow below described bitmap :
  *          b7-b6  Error information
  *             00 : No Error
  *             01 : (Not Used)
  *             10 : Timeout
  *             11 : Error
  *          b5     IP initilisation status
  *             0  : Reset (IP not initialized)
  *             1  : Init done (IP not initialized. HAL SMARTCARD Init function already called)
  *          b4-b3  (not used)
  *             xx : Should be set to 00
  *          b2     Intrinsic process state
  *             0  : Ready
  *             1  : Busy (IP busy with some configuration or internal operations)
  *          b1     (not used)
  *             x  : Should be set to 0
  *          b0     Tx state
  *             0  : Ready (no Tx operation ongoing)
  *             1  : Busy (Tx operation ongoing)
  *        - RxState contains information related to Rx operations.
  *          RxState value coding follow below described bitmap :
  *          b7-b6  (not used)
  *             xx : Should be set to 00
  *          b5     IP initilisation status
  *             0  : Reset (IP not initialized)
  *             1  : Init done (IP not initialized)
  *          b4-b2  (not used)
  *            xxx : Should be set to 000
  *          b1     Rx state
  *             0  : Ready (no Rx operation ongoing)
  *             1  : Busy (Rx operation ongoing)
  *          b0     (not used)
  *             x  : Should be set to 0.
  */
typedef enum
{
  HAL_SMARTCARD_STATE_RESET             = 0x00U,    /*!< Peripheral is not yet Initialized
                                                        Value is allowed for gState and RxState */
  HAL_SMARTCARD_STATE_READY             = 0x20U,    /*!< Peripheral Initialized and ready for use
                                                        Value is allowed for gState and RxState */
  HAL_SMARTCARD_STATE_BUSY              = 0x24U,    /*!< an internal process is ongoing
                                                        Value is allowed for gState only */
  HAL_SMARTCARD_STATE_BUSY_TX           = 0x21U,    /*!< Data Transmission process is ongoing
                                                        Value is allowed for gState only */
  HAL_SMARTCARD_STATE_BUSY_RX           = 0x22U,    /*!< Data Reception process is ongoing
                                                        Value is allowed for RxState only */
  HAL_SMARTCARD_STATE_BUSY_TX_RX        = 0x23U,    /*!< Data Transmission and Reception process is ongoing
                                                        Not to be used for neither gState nor RxState.
                                                        Value is result of combination (Or) between gState and RxState values */
  HAL_SMARTCARD_STATE_TIMEOUT           = 0xA0U,    /*!< Timeout state
                                                        Value is allowed for gState only */
  HAL_SMARTCARD_STATE_ERROR             = 0xE0U     /*!< Error
                                                        Value is allowed for gState only */
}HAL_SMARTCARD_StateTypeDef;

/**
  * @brief  SMARTCARD handle Structure definition
  */
typedef struct __SMARTCARD_HandleTypeDef
{
  reg_uart_t                    	 *UARTX;        /*!< USART registers base address */

  SMARTCARD_InitTypeDef            Init;             /*!< SmartCard communication parameters */

  uint8_t                          *pTxBuffPtr;      /*!< Pointer to SmartCard Tx transfer Buffer */

  uint16_t                         TxXferSize;       /*!< SmartCard Tx Transfer size */

  uint16_t                    TxXferCount;      /*!< SmartCard Tx Transfer Counter */

  uint8_t                          *pRxBuffPtr;      /*!< Pointer to SmartCard Rx transfer Buffer */

  uint16_t                         RxXferSize;       /*!< SmartCard Rx Transfer size */

  uint16_t                    RxXferCount;      /*!< SmartCard Rx Transfer Counter */

//  DMA_HandleTypeDef                *hdmatx;          /*!< SmartCard Tx DMA Handle parameters */

//  DMA_HandleTypeDef                *hdmarx;          /*!< SmartCard Rx DMA Handle parameters */

  HAL_LockTypeDef                  Lock;             /*!< Locking object */

  HAL_SMARTCARD_StateTypeDef  gState;           /*!< SmartCard state information related to global Handle management
                                                        and also related to Tx operations.
                                                        This parameter can be a value of @ref HAL_SMARTCARD_StateTypeDef */

  HAL_SMARTCARD_StateTypeDef  RxState;          /*!< SmartCard state information related to Rx operations.
                                                        This parameter can be a value of @ref HAL_SMARTCARD_StateTypeDef */

  uint32_t                    ErrorCode;        /*!< SmartCard Error code */

} SMARTCARD_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SMARTCARD_Exported_Constants  SMARTCARD Exported constants
  * @{
  */

/** @defgroup SMARTCARD_Error_Code SMARTCARD Error Code
  * @{
  */
#define HAL_SMARTCARD_ERROR_NONE             0x00000000U   /*!< No error            */
#define HAL_SMARTCARD_ERROR_PE               0x00000001U   /*!< Parity error        */
#define HAL_SMARTCARD_ERROR_NE               0x00000002U   /*!< Noise error         */
#define HAL_SMARTCARD_ERROR_FE               0x00000004U   /*!< Frame error         */
#define HAL_SMARTCARD_ERROR_ORE              0x00000008U   /*!< Overrun error       */
#define HAL_SMARTCARD_ERROR_DMA              0x00000010U   /*!< DMA transfer error  */
/**
  * @}
  */

/** @defgroup SMARTCARD_Word_Length SMARTCARD Word Length
  * @{
  */
#define SMARTCARD_WORDLENGTH_8B               0x3
/**
  * @}
  */

/** @defgroup SMARTCARD_Stop_Bits SMARTCARD Number of Stop Bits
  * @{
  */
#define SMARTCARD_STOPBITS_0_5             (0x0 << UART_LCR_STOP_POS)      /*!< smartcard mode, 0.5 bit */
#define SMARTCARD_STOPBITS_1_5             (0x1 << UART_LCR_STOP_POS)      /*!< smartcard mode, 1.5 bit */

/**
  * @}
  */

/** @defgroup SMARTCARD_Parity SMARTCARD Parity
  * @{
  */
#define SMARTCARD_PARITY_EVEN                    ((uint32_t)UART_LCR_PE_MASK)
#define SMARTCARD_PARITY_ODD                     ((uint32_t)(UART_LCR_PE_MASK | UART_LCR_PS_MASK))
/**
  * @}
  */

/** @defgroup SMARTCARD_Mode SMARTCARD Mode
  * @{
  */
#define SMARTCARD_MODE_RX                   ((uint32_t)UART_LCR_RXEN_MASK)
#define SMARTCARD_MODE_TX                   0x00000000U										
#define SMARTCARD_MODE_TX_RX                ((uint32_t)UART_LCR_RXEN_MASK)

/** @defgroup SMARTCARD_NACK_State  SMARTCARD NACK State
  * @{
  */
#define SMARTCARD_NACK_ENABLE               ((uint32_t)UART_MCR_SCNACK_MASK)
#define SMARTCARD_NACK_DISABLE              0x00000000U
/**
  * @}
  */

/** @defgroup SMARTCARD_RETRYTIME  SMARTCARD RETRY TIMES
  * @{
  */
#define SMARTCARD_RETRYTIME_NONE            (0x00000000U)
#define SMARTCARD_RETRYTIME_1T              (0x00000001U<<UART_MCR_SCCNT_POS)
#define SMARTCARD_RETRYTIME_2T              (0x00000002U<<UART_MCR_SCCNT_POS)
#define SMARTCARD_RETRYTIME_3T              (0x00000003U<<UART_MCR_SCCNT_POS)
#define SMARTCARD_RETRYTIME_4T              (0x00000004U<<UART_MCR_SCCNT_POS)
#define SMARTCARD_RETRYTIME_5T              (0x00000005U<<UART_MCR_SCCNT_POS)
#define SMARTCARD_RETRYTIME_6T              (0x00000006U<<UART_MCR_SCCNT_POS)
#define SMARTCARD_RETRYTIME_7T              (0x00000007U<<UART_MCR_SCCNT_POS)
/**
  * @}
  */
	
/** @defgroup SMARTCARD_DMA_Requests   SMARTCARD DMA requests
  * @{
  */
#define SMARTCARD_DMAREQ_TX                 ((uint32_t)UART_MCR_DMAEN_MASK)
#define SMARTCARD_DMAREQ_RX                 ((uint32_t)UART_MCR_DMAEN_MASK)
/**
  * @}
  */

/**
  * @}
  */
#define SMARTCARD_FIFO_RL_1      0x0     // RX FIFO trigger threshold 1 bytes 
#define SMARTCARD_FIFO_RL_4      (0x1<<UART_FCR_RXTL_POS)     // RX FIFO trigger threshold 4 bytes 
#define SMARTCARD_FIFO_RL_8      (0x2<<UART_FCR_RXTL_POS)     // RX FIFO trigger threshold 8 bytes 
#define SMARTCARD_FIFO_RL_14     (0x3<<UART_FCR_RXTL_POS)     // RX FIFO trigger threshold 14 bytes 

#define SMARTCARD_FIFO_TL_0      0x0     // TX FIFO trigger level 0 bytes
#define SMARTCARD_FIFO_TL_2      (0x1<<UART_FCR_TXTL_POS)     // TX FIFO trigger level 2 bytes
#define SMARTCARD_FIFO_TL_4      (0x2<<UART_FCR_TXTL_POS)     // TX FIFO trigger level 4 bytes
#define SMARTCARD_FIFO_TL_8      (0x3<<UART_FCR_TXTL_POS)     // TX FIFO trigger level 8 bytes

/**
  * @}
  */
	
/** @defgroup SMARTCARD_Prescaler SMARTCARD Prescaler
  * @{
  */
#define SMARTCARD_PRESCALER_SYSCLK_DIV2     (0x00000000U << UART_CR_PSC_POS)       		/*!< SYSCLK divided by 2 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV4     (0x00000001U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 4 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV6     (0x00000002U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 6 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV8     (0x00000003U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 8 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV10    (0x00000004U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 10 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV12    (0x00000005U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 12 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV14    (0x00000006U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 14 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV16    (0x00000007U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 16 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV18    (0x00000008U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 18 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV20    (0x00000009U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 20 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV22    (0x0000000AU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 22 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV24    (0x0000000BU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 24 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV26    (0x0000000CU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 26 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV28    (0x0000000DU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 28 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV30    (0x0000000EU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 30 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV32    (0x0000000FU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 32 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV34    (0x00000010U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 34 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV36    (0x00000011U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 36 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV38    (0x00000012U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 38 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV40    (0x00000013U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 40 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV42    (0x00000014U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 42 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV44    (0x00000015U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 44 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV46    (0x00000016U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 46 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV48    (0x00000017U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 48 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV50    (0x00000018U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 50 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV52    (0x00000019U << UART_CR_PSC_POS)          /*!< SYSCLK divided by 52 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV54    (0x0000001AU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 54 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV56    (0x0000001BU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 56 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV58    (0x0000001CU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 58 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV60    (0x0000001DU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 60 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV62    (0x0000001EU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 62 */
#define SMARTCARD_PRESCALER_SYSCLK_DIV64    (0x0000001FU << UART_CR_PSC_POS)          /*!< SYSCLK divided by 62 */
/**
  * @}
  */

/** @defgroup SmartCard_Flags SMARTCARD Flags
  *        Elements values convention: 0xXXXX
  *           - 0xXXXX  : Flag mask in the SR register
  * @{
  */
#define SMARTCARD_FLAG_TBEM                 ((uint32_t)UART_SR_TBEM_MASK)
#define SMARTCARD_FLAG_TEMT                 ((uint32_t)UART_SR_TEMT_MASK)
#define SMARTCARD_FLAG_RXNE                 ((uint32_t)UART_SR_RFNE_MASK)
#define SMARTCARD_FLAG_DR                 	((uint32_t)UART_SR_DR_MASK)
#define SMARTCARD_FLAG_BUSY                 ((uint32_t)UART_SR_BUSY_MASK)
#define SMARTCARD_FLAG_ORE                  ((uint32_t)UART_SR_OE_MASK)
#define SMARTCARD_FLAG_BI                   ((uint32_t)UART_SR_BI_MASK)
#define SMARTCARD_FLAG_FE                   ((uint32_t)UART_SR_FE_MASK)
#define SMARTCARD_FLAG_PE                   ((uint32_t)UART_SR_PE_MASK)


/**
  * @}
  */

/** @defgroup SmartCard_Interrupt_definition SMARTCARD Interrupts Definition
  *        Elements values convention: 0xY000XXXX
  *           - XXXX  : Interrupt mask in the Y register
  *           - Y  : Interrupt source register (2bits)
  *                 - 01: CR1 register
  *                 - 11: CR3 register
  * @{
  */
#define SMARTCARD_IT_TXS                    ((uint32_t)UART_TXS_MASK)
#define SMARTCARD_IT_TC                     ((uint32_t)UART_TC_MASK)
#define SMARTCARD_IT_RXRD                   ((uint32_t)UART_RXRD_MASK)
#define SMARTCARD_IT_BUSY                   ((uint32_t)UART_SR_BUSY_MASK)
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup SMARTCARD_Exported_Macros SMARTCARD Exported Macros
  * @{
  */

/** @brief Reset SMARTCARD handle gstate & RxState
  * @param  __HANDLE__ specifies the SMARTCARD Handle.
  *         SMARTCARD Handle selects the USARTx peripheral (USART availability and x value depending on device).
  * @retval None
  */
#define __HAL_SMARTCARD_RESET_HANDLE_STATE(__HANDLE__)  do{                                                       \
                                                           (__HANDLE__)->gState = HAL_SMARTCARD_STATE_RESET;      \
                                                           (__HANDLE__)->RxState = HAL_SMARTCARD_STATE_RESET;     \
                                                          } while(0U)

/** @brief  Check whether the specified Smartcard flag is set or not.
  * @param  __HANDLE__ specifies the SMARTCARD Handle.
  *         SMARTCARD Handle selects the USARTx peripheral (USART availability and x value depending on device).
  * @param  __FLAG__ specifies the flag to check.
  *        This parameter can be one of the following values:
  *            @arg SMARTCARD_FLAG_TBEM: Transmit data register empty flag
  *            @arg SMARTCARD_FLAG_TEMT: Transmission Complete flag
  *            @arg SMARTCARD_FLAG_DR: 	 Receive data register not empty flag
  *            @arg SMARTCARD_FLAG_BUSY: Busy Line detection flag
  *            @arg SMARTCARD_FLAG_ORE:  Overrun Error flag
  *            @arg SMARTCARD_FLAG_BI:   Noise Error flag
  *            @arg SMARTCARD_FLAG_FE:   Framing Error flag
  *            @arg SMARTCARD_FLAG_PE:   Parity Error flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_SMARTCARD_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))

/** @brief  Checks whether the specified SmartCard interrupt has occurred or not.
  * @param  __HANDLE__ specifies the SmartCard Handle.
  * @param  __IT__ specifies the SMARTCARD interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg SMARTCARD_IT_TXS: Transmit Data Register empty interrupt
  *            @arg SMARTCARD_IT_TC:  Transmission complete interrupt
  *            @arg SMARTCARD_IT_RXRD: Receive Data register not empty interrupt
  *            @arg SMARTCARD_IT_BUSY: BUSY line detection interrupt
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_SMARTCARD_GET_IT_SOURCE(__HANDLE__, __IT__) ((__HANDLE__)->Instance->IER & (((uint32_t)(__IT__)) & SMARTCARD_IT_MASK))

/** @brief  Macros to enable the SmartCard DMA request.
  * @param  __HANDLE__ specifies the SmartCard Handle.
  * @param  __REQUEST__ specifies the SmartCard DMA request.
  *          This parameter can be one of the following values:
  *            @arg SMARTCARD_DMAREQ_TX: SmartCard DMA transmit request
  *            @arg SMARTCARD_DMAREQ_RX: SmartCard DMA receive request
  * @retval None
  */
#define __HAL_SMARTCARD_DMA_REQUEST_ENABLE(__HANDLE__, __REQUEST__)    ((__HANDLE__)->Instance->MCR |=  (__REQUEST__))

/** @brief  Macros to disable the SmartCard DMA request.
  * @param  __HANDLE__ specifies the SmartCard Handle.
  * @param  __REQUEST__ specifies the SmartCard DMA request.
  *          This parameter can be one of the following values:
  *            @arg SMARTCARD_DMAREQ_TX: SmartCard DMA transmit request
  *            @arg SMARTCARD_DMAREQ_RX: SmartCard DMA receive request
  * @retval None
  */
#define __HAL_SMARTCARD_DMA_REQUEST_DISABLE(__HANDLE__, __REQUEST__)   ((__HANDLE__)->Instance->MCR &=  ~(__REQUEST__))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SMARTCARD_Exported_Functions
  * @{
  */

/** @addtogroup SMARTCARD_Exported_Functions_Group1
  * @{
  */
/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_SMARTCARD_Init(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_ReInit(SMARTCARD_HandleTypeDef *hsc);
HAL_StatusTypeDef HAL_SMARTCARD_DeInit(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_MspInit(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_MspDeInit(SMARTCARD_HandleTypeDef *hsc);

/**
  * @}
  */

/** @addtogroup SMARTCARD_Exported_Functions_Group2
  * @{
  */
/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_SMARTCARD_Transmit(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SMARTCARD_Receive(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size);

void HAL_SMARTCARD_IRQHandler(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_TxCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef *hsc);
void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef *hsc);

/**
  * @}
  */

/** @addtogroup SMARTCARD_Exported_Functions_Group3
  * @{
  */
/* Peripheral State functions  **************************************************/
HAL_SMARTCARD_StateTypeDef HAL_SMARTCARD_GetState(SMARTCARD_HandleTypeDef *hsc);
uint32_t HAL_SMARTCARD_GetError(SMARTCARD_HandleTypeDef *hsc);
/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup SMARTCARD_Private_Constants SMARTCARD Private Constants
  * @{
  */

/** @brief SMARTCARD interruptions flag mask
  *
  */
#define SMARTCARD_IT_MASK                   ((uint32_t) SMARTCARD_IT_TXS | SMARTCARD_IT_TC | SMARTCARD_IT_RXRD | SMARTCARD_IT_BUSY )

#define SMARTCARD_CR1_REG_INDEX             1U
#define SMARTCARD_CR3_REG_INDEX             3U

/**
  * @}
  */

/* Private macros --------------------------------------------------------*/
/** @defgroup SMARTCARD_Private_Macros   SMARTCARD Private Macros
  * @{
  */
#define IS_SMARTCARD_WORD_LENGTH(LENGTH)    ((LENGTH) == SMARTCARD_WORDLENGTH_8B)
#define IS_SMARTCARD_STOPBITS(STOPBITS)     (((STOPBITS) == SMARTCARD_STOPBITS_0_5) || \
                                             ((STOPBITS) == SMARTCARD_STOPBITS_1_5))
#define IS_SMARTCARD_PARITY(PARITY)         (((PARITY) == SMARTCARD_PARITY_EVEN) || \
                                             ((PARITY) == SMARTCARD_PARITY_ODD))
#define IS_SMARTCARD_MODE(MODE)             (((MODE)  == SMARTCARD_MODE_RX) || ((MODE) == SMARTCARD_MODE_TX))
#define IS_SMARTCARD_NACK_STATE(NACK)       (((NACK) == SMARTCARD_NACK_ENABLE) || \
                                             ((NACK) == SMARTCARD_NACK_DISABLE))
#define IS_SMARTCARD_BAUDRATE(BAUDRATE)     ((BAUDRATE) < 4500001U)

#define SMARTCARD_DIV(__PCLK__, __BAUD__)                (((__PCLK__)*25U)/(4U*(__BAUD__)))
#define SMARTCARD_DIVMANT(__PCLK__, __BAUD__)            (SMARTCARD_DIV((__PCLK__), (__BAUD__))/100U)
#define SMARTCARD_DIVFRAQ(__PCLK__, __BAUD__)            (((SMARTCARD_DIV((__PCLK__), (__BAUD__)) - (SMARTCARD_DIVMANT((__PCLK__), (__BAUD__)) * 100U)) * 16U + 50U) / 100U)
/* SMARTCARD BRR = mantissa + overflow + fraction
            = (SMARTCARD DIVMANT << 4) + (SMARTCARD DIVFRAQ & 0xF0) + (SMARTCARD DIVFRAQ & 0x0FU) */
#define SMARTCARD_BRR(__PCLK__, __BAUD__)       (((SMARTCARD_DIVMANT((__PCLK__), (__BAUD__)) << 4U) + \
                                                 (SMARTCARD_DIVFRAQ((__PCLK__), (__BAUD__)) & 0xF0U)) + \
                                                 (SMARTCARD_DIVFRAQ((__PCLK__), (__BAUD__)) & 0x0FU))

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup SMARTCARD_Private_Functions SMARTCARD Private Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __le501x_HAL_SMARTCARD_H */

/************************ (C) COPYRIGHT Linkedsemi *****END OF FILE****/
