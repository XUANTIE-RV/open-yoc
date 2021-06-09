/**
  ******************************************************************************
  * @file    le501x_hal_i2c.h
  * @author  AE Team
  * @brief   Header file of I2C HAL module.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LE501X_HAL_I2C_H
#define __LE501X_HAL_I2C_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "reg_i2c.h"
#include "dma.h"
#include "HAL_def.h"
#include <string.h>
#include "sdk_config.h"
#include "field_manipulate.h"
#include "ls_dbg.h"
#include "systick.h"

#define I2C_CLOCK (SDK_PCLK_MHZ * 1000000)
#define HAL_I2C_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED

    // uint32_t SystemCoreClock = SDK_PCLK_MHZ*1000000;

    /** @addtogroup le501x_HAL_Driver
  * @{
  */

    /** @addtogroup I2C
  * @{
  */

    /* Exported types ------------------------------------------------------------*/
    /** @defgroup I2C_Exported_Types I2C Exported Types
  * @{
  */

    /** @defgroup I2C_Configuration_Structure_definition I2C Configuration Structure definition
  * @brief  I2C Configuration Structure definition
  * @{
  */
    typedef struct
    {
        uint32_t ClockSpeed; /*!< Specifies the clock frequency.
                                  This parameter must be set to a value lower than 400kHz */

        uint32_t OwnAddress1; /*!< Specifies the first device own address.
                                  This parameter can be a 7-bit or 10-bit address. */

        uint32_t AddressingMode; /*!< Specifies if 7-bit or 10-bit addressing mode is selected.
                                  This parameter can be a value of @ref I2C_addressing_mode */

        uint32_t DualAddressMode; /*!< Specifies if dual addressing mode is selected.
                                  This parameter can be a value of @ref I2C_dual_addressing_mode */

        uint32_t OwnAddress2; /*!< Specifies the second device own address if dual addressing mode is selected
                                  This parameter can be a 7-bit address. */

        uint32_t GeneralCallMode; /*!< Specifies if general call mode is selected.
                                  This parameter can be a value of @ref I2C_general_call_addressing_mode */

        uint32_t NoStretchMode; /*!< Specifies if nostretch mode is selected.
                                  This parameter can be a value of @ref I2C_nostretch_mode */

    } I2C_InitTypeDef;

    /**
  * @}
  */

    /** @defgroup HAL_state_structure_definition HAL state structure definition
  * @brief  HAL State structure definition
  * @note  HAL I2C State value coding follow below described bitmap :
  *          b7-b6  Error information
  *             00 : No Error
  *             01 : Abort (Abort user request on going)
  *             10 : Timeout
  *             11 : Error
  *          b5     Peripheral initilisation status
  *             0  : Reset (Peripheral not initialized)
  *             1  : Init done (Peripheral initialized and ready to use. HAL I2C Init function called)
  *          b4     (not used)
  *             x  : Should be set to 0
  *          b3
  *             0  : Ready or Busy (No Listen mode ongoing)
  *             1  : Listen (Peripheral in Address Listen Mode)
  *          b2     Intrinsic process state
  *             0  : Ready
  *             1  : Busy (Peripheral busy with some configuration or internal operations)
  *          b1     Rx state
  *             0  : Ready (no Rx operation ongoing)
  *             1  : Busy (Rx operation ongoing)
  *          b0     Tx state
  *             0  : Ready (no Tx operation ongoing)
  *             1  : Busy (Tx operation ongoing)
  * @{
  */
    typedef enum
    {
        HAL_I2C_STATE_RESET = 0x00U,          /*!< Peripheral is not yet Initialized         */
        HAL_I2C_STATE_READY = 0x20U,          /*!< Peripheral Initialized and ready for use  */
        HAL_I2C_STATE_BUSY = 0x24U,           /*!< An internal process is ongoing            */
        HAL_I2C_STATE_BUSY_TX = 0x21U,        /*!< Data Transmission process is ongoing      */
        HAL_I2C_STATE_BUSY_RX = 0x22U,        /*!< Data Reception process is ongoing         */
        HAL_I2C_STATE_LISTEN = 0x28U,         /*!< Address Listen Mode is ongoing            */
        HAL_I2C_STATE_BUSY_TX_LISTEN = 0x29U, /*!< Address Listen Mode and Data Transmission
                                                 process is ongoing                         */
        HAL_I2C_STATE_BUSY_RX_LISTEN = 0x2AU, /*!< Address Listen Mode and Data Reception
                                                 process is ongoing                         */
        HAL_I2C_STATE_ABORT = 0x60U,          /*!< Abort user request ongoing                */
        HAL_I2C_STATE_TIMEOUT = 0xA0U,        /*!< Timeout state                             */
        HAL_I2C_STATE_ERROR = 0xE0U           /*!< Error                                     */

    } HAL_I2C_StateTypeDef;

    /**
  * @}
  */

    /** @defgroup HAL_mode_structure_definition HAL mode structure definition
  * @brief  HAL Mode structure definition
  * @note  HAL I2C Mode value coding follow below described bitmap :
  *          b5\n
  *             0  : None\n
  *             1  : Slave (HAL I2C communication is in Slave Mode)\n
  *          b4\n
  *             0  : None\n
  *             1  : Master (HAL I2C communication is in Master Mode)\n
  *          b3-b2-b1-b0  (not used)\n
  *             xxxx : Should be set to 0000
  * @{
  */
    typedef enum
    {
        HAL_I2C_MODE_NONE = 0x00U,   /*!< No I2C communication on going             */
        HAL_I2C_MODE_MASTER = 0x10U, /*!< I2C communication is in Master Mode       */
        HAL_I2C_MODE_SLAVE = 0x20U,  /*!< I2C communication is in Slave Mode        */

    } HAL_I2C_ModeTypeDef;

/**
  * @}
  */

/** @defgroup I2C_Error_Code_definition 
  * @brief  I2C Error Code definition
  * @{
  */
#define HAL_I2C_ERROR_NONE 0x00000000U      /*!< No error              */
#define HAL_I2C_ERROR_BERR 0x00000001U      /*!< BERR error            */
#define HAL_I2C_ERROR_ARLO 0x00000002U      /*!< ARLO error            */
#define HAL_I2C_ERROR_NACKF 0x00000004U     /*!< NACK error            */
#define HAL_I2C_ERROR_OVR 0x00000008U       /*!< OVR error             */
#define HAL_I2C_ERROR_DMA 0x00000010U       /*!< DMA transfer error    */
#define HAL_I2C_ERROR_TIMEOUT 0x00000020U   /*!< Timeout Error         */
#define HAL_I2C_ERROR_SIZE 0x00000040U      /*!< Size Management error */
#define HAL_I2C_ERROR_DMA_PARAM 0x00000080U /*!< DMA Parameter Error   */

    /**
  * @}
  */

    /** @defgroup I2C_handle_Structure_definition I2C handle Structure definition
  * @brief  I2C handle Structure definition
  * @{
  */
    typedef struct __I2C_HandleTypeDef
    {
        reg_i2c_t *Instance; /*!< I2C registers base address               */

        I2C_InitTypeDef Init; /*!< I2C communication parameters             */

        uint8_t *pBuffPtr; /*!< Pointer to I2C transfer buffer           */

        uint16_t XferSize; /*!< I2C transfer size                        */

        volatile uint16_t XferCount; /*!< I2C transfer counter                     */

        volatile uint32_t XferOptions; /*!< I2C transfer options                     */

        volatile uint32_t PreviousState; /*!< I2C communication Previous state and mode
                                                  context for internal usage               */
#ifdef HAL_DMA_MODULE_ENABLED
        DMA_HandleTypeDef hdmatx; /*!< I2C Tx DMA handle parameters             */

        DMA_HandleTypeDef hdmarx; /*!< I2C Rx DMA handle parameters             */
#endif
        HAL_LockTypeDef Lock; /*!< I2C locking object                       */

        volatile HAL_I2C_StateTypeDef State; /*!< I2C communication state                  */

        volatile HAL_I2C_ModeTypeDef Mode; /*!< I2C communication mode                   */

        volatile uint32_t ErrorCode; /*!< I2C Error code                           */

        volatile uint32_t Devaddress; /*!< I2C Target device address                */

        volatile uint32_t EventCount; /*!< I2C Event counter                        */
    } I2C_HandleTypeDef;

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/

/** @defgroup I2C_Exported_Constants 
  * @{
  */

/** @defgroup I2C_addressing_mode I2C addressing mode
  * @{
  */
#define I2C_ADDRESSINGMODE_7BIT 0x00000000U
#define I2C_ADDRESSINGMODE_10BIT I2C_OAR1_OA1MODE_MASK
/**
  * @}
  */

/** @defgroup I2C_dual_addressing_mode  I2C dual addressing mode
  * @{
  */
#define I2C_DUALADDRESS_DISABLE 0x00000000U
#define I2C_DUALADDRESS_ENABLE I2C_OAR2_OA2EN_MASK
/**
  * @}
  */

/** @defgroup I2C_general_call_addressing_mode I2C general call addressing mode
  * @{
  */
#define I2C_GENERALCALL_DISABLE 0x00000000U
#define I2C_GENERALCALL_ENABLE I2C_CR1_GCEN_MASK
/**
  * @}
  */

/** @defgroup I2C_nostretch_mode I2C nostretch mode
  * @{
  */
#define I2C_NOSTRETCH_DISABLE 0x00000000U
#define I2C_NOSTRETCH_ENABLE I2C_CR1_NOSTRETCH_MASK
/**
  * @}
  */

/** @defgroup I2C_XferDirection_definition I2C XferDirection definition
  * @{
  */
#define I2C_DIRECTION_RECEIVE 0x00000000U
#define I2C_DIRECTION_TRANSMIT 0x00000001U
/**
  * @}
  */

/** @defgroup I2C_XferOptions_definition I2C XferOptions definition
  * @{
  */
#define I2C_FIRST_FRAME 0x00000001U
#define I2C_FIRST_AND_NEXT_FRAME 0x00000002U
#define I2C_NEXT_FRAME 0x00000004U
#define I2C_FIRST_AND_LAST_FRAME 0x00000008U
#define I2C_LAST_FRAME_NO_STOP 0x00000010U
#define I2C_LAST_FRAME 0x00000020U

/* List of XferOptions in usage of :
 * 1- Restart condition in all use cases (direction change or not)
 */
#define I2C_OTHER_FRAME (0x00AA0000U)
#define I2C_OTHER_AND_LAST_FRAME (0xAA000000U)
/**
  * @}
  */

/** @defgroup I2C_Interrupt_configuration_definition I2C Interrupt configuration definition
  * @brief I2C Interrupt definition
  *        Elements values convention: 0xXXXXXXXX
  *           - XXXXXXXX  : Interrupt control mask
  * @{
  */
#define I2C_IT_RXNE I2C_IER_RXNEIE_MASK
#define I2C_IT_TXE I2C_IER_TXEIE_MASK
#define I2C_IT_TC I2C_IER_TCIE_MASK
#define I2C_IT_TCR I2C_IER_TCRIE_MASK
#define I2C_IT_EVT I2C_IER_ADDRIE_MASK | I2C_IER_NACKIE_MASK | I2C_IER_STOPIE_MASK
#define I2C_IT_ERR I2C_IER_BERRIE_MASK
/**
  * @}
  */

/** @defgroup I2C_Flag_definition I2C Flag definition
  * @{
  */
#define I2C_FLAG_TXE I2C_SR_TXE_MASK
#define I2C_FLAG_RXNE I2C_SR_RXNE_MASK
#define I2C_FLAG_ADDR I2C_SR_ADDR_MASK
#define I2C_FLAG_NACK I2C_SR_NACKF_MASK
#define I2C_FLAG_STOPF I2C_SR_STOPF_MASK
#define I2C_FLAG_TC I2C_SR_TC_MASK
#define I2C_FLAG_TCR I2C_SR_TCR_MASK
#define I2C_FLAG_BERR I2C_SR_BERR_MASK
#define I2C_FLAG_ARLO I2C_SR_ARLO_MASK
#define I2C_FLAG_OVR I2C_SR_OVR_MASK
#define I2C_FLAG_PECERR I2C_SR_PECERR_MASK
#define I2C_FLAG_BUSY I2C_SR_BUSY_MASK
#define I2C_FLAG_TIMEOUT I2C_SR_TIMEOUT_MASK
#define I2C_FLAG_ALERT I2C_SR_ALERT_MASK
#define I2C_FLAG_DIR I2C_SR_DIR_MASK

    /**
  * @}
  */

    /**
  * @}
  */

    /* Exported macros -----------------------------------------------------------*/

#define I2C_MIN_FREQ 32000U /*!< 32kHz                     */

/** @defgroup I2C_Exported_Macros I2C Exported Macros
  * @{
  */

/** @brief Reset I2C handle state.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @retval None
  */
#define I2C_FREQRANGE(__PCLK__, __SPEED__) ((__PCLK__) / (__SPEED__))

#define __HAL_I2C_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_I2C_STATE_RESET)

/** @brief  Enable or disable the specified I2C interrupts.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @param  __INTERRUPT__ specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg I2C_IT_BUF: Buffer interrupt enable
  *            @arg I2C_IT_EVT: Event interrupt enable
  *            @arg I2C_IT_ERR: Error interrupt enable
  * @retval None
  */
#define __HAL_I2C_ENABLE_IT(__HANDLE__, __INTERRUPT__) SET_BIT((__HANDLE__)->Instance->IER, (__INTERRUPT__))

#define __HAL_I2C_DISABLE_IT(__HANDLE__, __INTERRUPT__) SET_BIT((__HANDLE__)->Instance->IDR, (__INTERRUPT__))

/** @brief  Checks if the specified I2C interrupt source is enabled or disabled.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @param  __INTERRUPT__ specifies the I2C interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg I2C_IT_BUF: Buffer interrupt enable
  *            @arg I2C_IT_EVT: Event interrupt enable
  *            @arg I2C_IT_ERR: Error interrupt enable
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_I2C_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->IVS & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

#define __HAL_I2C_GET_CR2_FLAG(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CR2 & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Checks whether the specified I2C flag is set or not.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @param  __FLAG__ specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg I2C_FLAG_OVR: Overrun/Underrun flag
  *            @arg I2C_FLAG_AF: Acknowledge failure flag
  *            @arg I2C_FLAG_ARLO: Arbitration lost flag
  *            @arg I2C_FLAG_BERR: Bus error flag
  *            @arg I2C_FLAG_TXE: Data register empty flag
  *            @arg I2C_FLAG_RXNE: Data register not empty flag
  *            @arg I2C_FLAG_STOPF: Stop detection flag
  *            @arg I2C_FLAG_ADDR: Address sent flag
  *                                Address matched flag
  *            @arg I2C_FLAG_TRA: Transmitter/Receiver flag
  *            @arg I2C_FLAG_BUSY: Bus busy flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_I2C_GET_FLAG(__HANDLE__, __FLAG__) (((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__)) ? SET : RESET)

/** @brief  Clears the I2C pending flags which are cleared by writing 0 in a specific bit.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @param  __FLAG__ specifies the flag to clear.
  *         This parameter can be any combination of the following values:
  *            @arg I2C_FLAG_OVR: Overrun/Underrun flag (Slave mode)
  *            @arg I2C_FLAG_AF: Acknowledge failure flag
  *            @arg I2C_FLAG_ARLO: Arbitration lost flag (Master mode)
  *            @arg I2C_FLAG_BERR: Bus error flag
  * @retval None
  */
#define __HAL_I2C_CLEAR_FLAG(__HANDLE__, __FLAG__) SET_BIT((__HANDLE__)->Instance->CFR, (__FLAG__))

#define __HAL_I2C_CLEAR_IF(__HANDLE__, __FLAG__) SET_BIT((__HANDLE__)->Instance->ICR, (__FLAG__))

/** @brief  Clears the I2C ADDR pending flag.
  * @param  __HANDLE__ specifies the I2C Handle.
  *         This parameter can be I2C where x: 1, 2, or 3 to select the I2C peripheral.
  * @retval None
  */
#define __HAL_I2C_CLEAR_ADDRFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->CFR, I2C_CFR_ADDRCF_MASK)

/** @brief  Clears the I2C STOPF pending flag.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @retval None
  */
#define __HAL_I2C_CLEAR_STOPFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->CFR, I2C_CFR_STOPCF_MASK)

/** @brief  Clears the I2C SR flag.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @retval None
  */
#define __HAL_I2C_CLEAR_SR(__HANDLE__) SET_BIT((__HANDLE__)->Instance->CFR, 0x3F38)

/** @brief  Enable the specified I2C peripheral.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @retval None
  */
#define __HAL_I2C_ENABLE(__HANDLE__) SET_BIT((__HANDLE__)->Instance->CR1, I2C_CR1_PE_MASK)

/** @brief  Disable the specified I2C peripheral.
  * @param  __HANDLE__ specifies the I2C Handle.
  * @retval None
  */
#define __HAL_I2C_DISABLE(__HANDLE__) CLEAR_BIT((__HANDLE__)->Instance->CR1, I2C_CR1_PE_MASK)

    /**
  * @}
  */

    /* Exported functions --------------------------------------------------------*/
    /** @addtogroup I2C_Exported_Functions
  * @{
  */

    /** @addtogroup I2C_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
    /* Initialization and de-initialization functions******************************/
    HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
    HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);

    /** @addtogroup I2C_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
    /* IO operation functions  ****************************************************/
    /******* Blocking mode: Polling */
    HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout);

    /******* Non-Blocking mode: Interrupt */
    HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
    HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
    HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
    HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);

    HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
    HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
    HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
    HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
    HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c);
    HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c);
    HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress);

    /******* Non-Blocking mode: DMA */
    HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
    //HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
    //HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
    //HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
    //HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
    //HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

    //HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
    //HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
    //HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
    //HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t XferOptions);
    /**
  * @}
  */

    /** @addtogroup I2C_IRQ_Handler_and_Callbacks IRQ Handler and Callbacks
 * @{
 */
    /******* I2C IRQHandler and Callbacks used in non blocking modes (Interrupt and DMA) */
    void HAL_I2C_IRQHandler(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);
    void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
    void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c);
    /**
  * @}
  */

    /** @addtogroup I2C_Exported_Functions_Group3 Peripheral State, Mode and Error functions
  * @{
  */
    /* Peripheral State, Mode and Error functions  *********************************/
    HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c);
    HAL_I2C_ModeTypeDef HAL_I2C_GetMode(I2C_HandleTypeDef *hi2c);
    uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *hi2c);

/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup I2C_Private_Constants I2C Private Constants
  * @{
  */
#define I2C_MIN_PCLK_FREQ_STANDARD 400000U /*!< 400 kHz                     */
#define I2C_MIN_PCLK_FREQ_FAST 1000000U    /*!< 1 MHz                     */
/**
  * @}
  */

/** @defgroup I2C_IS_RTC_Definitions I2C Private macros to check input parameters
  * @{
  */
#define IS_I2C_ALL_INSTANCE(INSTANCE) (((INSTANCE) == I2C1) || \
                                       ((INSTANCE) == I2C2))

#define IS_I2C_ADDRESSING_MODE(ADDRESS) (((ADDRESS) == I2C_ADDRESSINGMODE_7BIT) || \
                                         ((ADDRESS) == I2C_ADDRESSINGMODE_10BIT))
#define IS_I2C_DUAL_ADDRESS(ADDRESS) (((ADDRESS) == I2C_DUALADDRESS_DISABLE) || \
                                      ((ADDRESS) == I2C_DUALADDRESS_ENABLE))
#define IS_I2C_GENERAL_CALL(CALL) (((CALL) == I2C_GENERALCALL_DISABLE) || \
                                   ((CALL) == I2C_GENERALCALL_ENABLE))
#define IS_I2C_NO_STRETCH(STRETCH) (((STRETCH) == I2C_NOSTRETCH_DISABLE) || \
                                    ((STRETCH) == I2C_NOSTRETCH_ENABLE))

#define IS_I2C_CLOCK_SPEED(SPEED) (((SPEED) > 1000U) && ((SPEED) <= 400000U))
#define IS_I2C_OWN_ADDRESS1(ADDRESS1) (((ADDRESS1)&0xFFFFFC00U) == 0U)
#define IS_I2C_OWN_ADDRESS2(ADDRESS2) (((ADDRESS2)&0xFFFFFF01U) == 0U)
#define IS_I2C_TRANSFER_OPTIONS_REQUEST(REQUEST) (((REQUEST) == I2C_FIRST_FRAME) ||          \
                                                  ((REQUEST) == I2C_FIRST_AND_NEXT_FRAME) || \
                                                  ((REQUEST) == I2C_NEXT_FRAME) ||           \
                                                  ((REQUEST) == I2C_FIRST_AND_LAST_FRAME) || \
                                                  ((REQUEST) == I2C_LAST_FRAME) ||           \
                                                  ((REQUEST) == I2C_LAST_FRAME_NO_STOP) ||   \
                                                  IS_I2C_TRANSFER_OTHER_OPTIONS_REQUEST(REQUEST))

#define IS_I2C_TRANSFER_OTHER_OPTIONS_REQUEST(REQUEST) (((REQUEST) == I2C_OTHER_FRAME) || \
                                                        ((REQUEST) == I2C_OTHER_AND_LAST_FRAME))

#define I2C_CHECK_FLAG(__ISR__, __FLAG__) ((((__ISR__) & (__FLAG__)) == (__FLAG__)) ? SET : RESET)
#define I2C_CHECK_IT_SOURCE(__CR1__, __IT__) ((((__CR1__) & (__IT__)) == (__IT__)) ? SET : RESET)
    /**
  * @}
  */

    /**
  * @}
  */

    /* Private functions ---------------------------------------------------------*/
    /** @defgroup I2C_Private_Functions I2C Private Functions
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

#endif /* __le501x_HAL_I2C_H */

/************************ (C) COPYRIGHT Linkedsemi ***********END OF FILE****/
