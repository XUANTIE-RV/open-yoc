/**
  ******************************************************************************
  * @file    le501x_hal_i2c.c
  * @author  AE Team
  * @brief   I2C HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Inter Integrated Circuit (I2C) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State, Mode and Error functions
  *
  @verbatim
  ==============================================================================
  */

/* Includes ------------------------------------------------------------------*/
#include "le501x_hal_i2c.h"
#include "log.h"
/** @addtogroup le501x_HAL_Driver
  * @{
  */

/** @defgroup I2C I2C
  * @brief I2C HAL module driver
  * @{
  */

#ifdef HAL_I2C_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup I2C_Private_Define
  * @{
  */
#define I2C_TIMEOUT_FLAG 35U            /*!< Timeout 35 ms             */
#define I2C_TIMEOUT_BUSY_FLAG 25U       /*!< Timeout 25 ms             */
#define I2C_NO_OPTION_FRAME 0xFFFF0000U /*!< XferOptions default value */

/* Private define for @ref PreviousState usage */
#define I2C_STATE_MSK ((uint32_t)((uint32_t)((uint32_t)HAL_I2C_STATE_BUSY_TX | (uint32_t)HAL_I2C_STATE_BUSY_RX) & (uint32_t)(~((uint32_t)HAL_I2C_STATE_READY)))) /*!< Mask State define, keep only RX and TX bits            */
#define I2C_STATE_NONE ((uint32_t)(HAL_I2C_MODE_NONE))                                                                                                           /*!< Default Value                                          */
#define I2C_STATE_MASTER_BUSY_TX ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) | (uint32_t)HAL_I2C_MODE_MASTER))                                 /*!< Master Busy TX, combinaison of State LSB and Mode enum */
#define I2C_STATE_MASTER_BUSY_RX ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) | (uint32_t)HAL_I2C_MODE_MASTER))                                 /*!< Master Busy RX, combinaison of State LSB and Mode enum */
#define I2C_STATE_SLAVE_BUSY_TX ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_TX & I2C_STATE_MSK) | (uint32_t)HAL_I2C_MODE_SLAVE))                                   /*!< Slave Busy TX, combinaison of State LSB and Mode enum  */
#define I2C_STATE_SLAVE_BUSY_RX ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_RX & I2C_STATE_MSK) | (uint32_t)HAL_I2C_MODE_SLAVE))                                   /*!< Slave Busy RX, combinaison of State LSB and Mode enum  */

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/** @defgroup I2C_Private_Functions I2C Private Functions
  * @{
  */
/* Private functions to handle DMA transfer */
static void I2C_DMAXferCplt(DMA_HandleTypeDef *hdma);
static void I2C_DMAError(DMA_HandleTypeDef *hdma);
//static void I2C_DMAAbort(DMA_HandleTypeDef *hdma);

static void I2C_ITError(I2C_HandleTypeDef *hi2c);

static HAL_StatusTypeDef I2C_MasterRequestWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_MasterRequestRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart);

/* Private functions to handle flags during polling transfer */
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnTXEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c);

/* Private functions for I2C transfer IRQ handler */
static void I2C_MasterTransmit_TXE(I2C_HandleTypeDef *hi2c);
static void I2C_MasterReceive_RXNE(I2C_HandleTypeDef *hi2c);
static void I2C_Master_TCR(I2C_HandleTypeDef *hi2c);
static void I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c);
static void I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c);
static void I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c, uint32_t IT2Flags);
static void I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c);

/* Private function to Convert Specific options */
//static void I2C_ConvertOtherXferOptions(I2C_HandleTypeDef *hi2c);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup I2C_Exported_Functions I2C Exported Functions
  * @{
  */

/** @defgroup I2C_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          deinitialize the I2Cx peripheral:

      (+) User must Implement HAL_I2C_MspInit() function in which he configures
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC).

      (+) Call the function HAL_I2C_Init() to configure the selected device with
          the selected configuration:
        (++) Communication Speed
        (++) Duty cycle
        (++) Addressing mode
        (++) Own Address 1
        (++) Dual Addressing mode
        (++) Own Address 2
        (++) General call mode
        (++) Nostretch mode

      (+) Call the function HAL_I2C_DeInit() to restore the default configuration
          of the selected I2Cx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the I2C according to the specified parameters
  *         in the I2C_InitTypeDef and initialize the associated handle.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c)
{
    volatile uint32_t freq, SCLH, SCLL, sdadel, scldel;
    volatile uint32_t pclk1;

    /* Check the I2C handle allocation */
    if (hi2c == NULL)
    {
        return HAL_ERROR;
    }

    /* Check the parameters */
    LS_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));
    LS_ASSERT(IS_I2C_CLOCK_SPEED(hi2c->Init.ClockSpeed));
    LS_ASSERT(IS_I2C_OWN_ADDRESS1(hi2c->Init.OwnAddress1));
    LS_ASSERT(IS_I2C_ADDRESSING_MODE(hi2c->Init.AddressingMode));
    LS_ASSERT(IS_I2C_DUAL_ADDRESS(hi2c->Init.DualAddressMode));
    //LS_ASSERT(IS_I2C_OWN_ADDRESS2(hi2c->Init.OwnAddress2));
    LS_ASSERT(IS_I2C_GENERAL_CALL(hi2c->Init.GeneralCallMode));
    LS_ASSERT(IS_I2C_NO_STRETCH(hi2c->Init.NoStretchMode));

    if (hi2c->State == HAL_I2C_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        hi2c->Lock = HAL_UNLOCKED;

        /* Init the low level hardware : reserved */
        HAL_I2C_MspInit(hi2c);
    }

    hi2c->State = HAL_I2C_STATE_BUSY;

    /* Disable the selected I2C peripheral */
    __HAL_I2C_DISABLE(hi2c);

    /* Calculate frequency value */
    if (hi2c->Init.ClockSpeed < I2C_MIN_FREQ)
        pclk1 = I2C_CLOCK >> 5;
    else
    {
        pclk1 = I2C_CLOCK;
    }

    freq = I2C_FREQRANGE(pclk1, hi2c->Init.ClockSpeed);
    SCLL = freq / 2;
    SCLH = freq - SCLL; //If it's not divisible,The remainder is added to SCLH
    sdadel = SCLH / 4;
    scldel = SCLH / 4;
    /*---------------------------- I2Cx TIMINGR Configuration ----------------------*/
    /* Configure I2Cx: Frequency range */
    if (hi2c->Init.ClockSpeed < I2C_MIN_FREQ)
    {
        MODIFY_REG(hi2c->Instance->TIMINGR, I2C_TIMINGR_PRESC_MASK, 32u << I2C_TIMINGR_PRESC_POS);
    }
    else
    {
        CLEAR_BIT(hi2c->Instance->TIMINGR, I2C_TIMINGR_PRESC_MASK);
    }
    MODIFY_REG(hi2c->Instance->TIMINGR, (I2C_TIMINGR_SCLH_MASK | I2C_TIMINGR_SCLL_MASK), (SCLH << I2C_TIMINGR_SCLH_POS | SCLL << I2C_TIMINGR_SCLL_POS | sdadel << I2C_TIMINGR_SDADEL_POS | scldel << I2C_TIMINGR_SCLDEL_POS));

    /*---------------------------- I2Cx CR1 Configuration ----------------------*/
    /* Configure I2Cx: Generalcall and NoStretch mode */
    MODIFY_REG(hi2c->Instance->CR1, (I2C_CR1_GCEN_MASK | I2C_CR1_NOSTRETCH_MASK), (hi2c->Init.GeneralCallMode | hi2c->Init.NoStretchMode));

    /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
    /* Configure I2Cx: Own Address1 and addressing mode */
    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
    {
        MODIFY_REG(hi2c->Instance->OAR1, (I2C_OAR1_OA1MODE_MASK | I2C_OAR1_OA18_9_MASK | I2C_OAR1_OA11_7_MASK | I2C_OAR1_OA10_MASK), (I2C_OAR1_OA1EN_MASK | hi2c->Init.AddressingMode | (hi2c->Init.OwnAddress1 << 1)));
    }
    else
    {
        MODIFY_REG(hi2c->Instance->OAR1, (I2C_OAR1_OA1MODE_MASK | I2C_OAR1_OA18_9_MASK | I2C_OAR1_OA11_7_MASK | I2C_OAR1_OA10_MASK), (I2C_OAR1_OA1EN_MASK | hi2c->Init.AddressingMode | hi2c->Init.OwnAddress1));
    }

    /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
    /* Configure I2Cx: Dual mode and Own Address2 */
    MODIFY_REG(hi2c->Instance->OAR2, (I2C_OAR2_OA2EN_MASK | I2C_OAR2_OA21_7_MASK), (hi2c->Init.DualAddressMode | hi2c->Init.OwnAddress2));

    /* Enable the selected I2C peripheral */
    __HAL_I2C_ENABLE(hi2c);

    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->Mode = HAL_I2C_MODE_NONE;

    return HAL_OK;
}

/**
  * @brief  DeInitialize the I2C peripheral.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{
    /* Check the I2C handle allocation */
    if (hi2c == NULL)
    {
        return HAL_ERROR;
    }

    /* Check the parameters */
    LS_ASSERT(IS_I2C_ALL_INSTANCE(hi2c->Instance));

    hi2c->State = HAL_I2C_STATE_BUSY;

    /* Disable the I2C Peripheral Clock */
    __HAL_I2C_DISABLE(hi2c);

    /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
    HAL_I2C_MspDeInit(hi2c);

    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    hi2c->State = HAL_I2C_STATE_RESET;
    hi2c->PreviousState = I2C_STATE_NONE;
    hi2c->Mode = HAL_I2C_MODE_NONE;

    /* Release Lock */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
}

/**
  * @brief  Initialize the I2C MSP.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitialize the I2C MSP.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MspDeInit could be implemented in the user file
   */
}

/** @defgroup I2C_Exported_Functions_Group2 Input and Output operation functions
 *  @brief   Data transfers functions
 *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the I2C data
    transfers.

    (#) There are two modes of transfer:
       (++) Blocking mode : The communication is performed in the polling mode.
            The status of all data processing is returned by the same function
            after finishing transfer.
       (++) No-Blocking mode : The communication is performed using Interrupts
            or DMA. These functions return the status of the transfer startup.
            The end of the data processing will be indicated through the
            dedicated I2C IRQ when using Interrupt mode or the DMA IRQ when
            using DMA mode.

    (#) Blocking mode functions are :
        (++) HAL_I2C_Master_Transmit()
        (++) HAL_I2C_Master_Receive()
        (++) HAL_I2C_Slave_Transmit()
        (++) HAL_I2C_Slave_Receive()
        (++) HAL_I2C_IsDeviceReady()

    (#) No-Blocking mode functions with Interrupt are :
        (++) HAL_I2C_Master_Transmit_IT()
        (++) HAL_I2C_Master_Receive_IT()
        (++) HAL_I2C_Slave_Transmit_IT()
        (++) HAL_I2C_Slave_Receive_IT()
        (++) HAL_I2C_Master_Seq_Transmit_IT()
        (++) HAL_I2C_Master_Seq_Receive_IT()
        (++) HAL_I2C_Slave_Seq_Transmit_IT()
        (++) HAL_I2C_Slave_Seq_Receive_IT()
        (++) HAL_I2C_EnableListen_IT()
        (++) HAL_I2C_DisableListen_IT()
        (++) HAL_I2C_Master_Abort_IT()

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_I2C_MasterTxCpltCallback()
        (++) HAL_I2C_MasterRxCpltCallback()
        (++) HAL_I2C_SlaveTxCpltCallback()
        (++) HAL_I2C_SlaveRxCpltCallback()
        (++) HAL_I2C_AddrCallback()
        (++) HAL_I2C_ListenCpltCallback()
        (++) HAL_I2C_ErrorCallback()
        (++) HAL_I2C_AbortCpltCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* Init tickstart for timeout management*/
    uint32_t tickstart = systick_get_value();

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        /* Wait until BUSY flag is reset */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
        {
            return HAL_BUSY;
        }

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_TX;
        hi2c->Mode = HAL_I2C_MODE_MASTER;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);

        /* Set NBYTES */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | I2C_CR2_RELOAD_MASK | I2C_CR2_NBYTES_MASK);
        if (hi2c->XferSize > 0xFF)
        {
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_RELOAD_MASK | 0xFF0000);
        }
        else
            SET_BIT(hi2c->Instance->CR2, (((uint32_t)hi2c->XferSize) << I2C_CR2_NBYTES_POS));

        /* Send Slave Address */
        if (I2C_MasterRequestWrite(hi2c, DevAddress, Timeout, tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        while (hi2c->XferCount > 0U)
        {
            /* Wait until TXE flag is set */
            if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
            {
                if (hi2c->ErrorCode == HAL_I2C_ERROR_NACKF)
                {
                    /* Generate Stop */
                    SET_BIT(hi2c->Instance->CR2, I2C_CR2_STOP_MASK);
                }
                return HAL_ERROR;
            }

            /* Write data to TXDR */
            hi2c->Instance->TXDR = *hi2c->pBuffPtr;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;

            /* Updata NBYTES */
            if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TCR) == SET)
            {
                if (hi2c->XferCount > 0xFF)
                {
                    MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_NBYTES_MASK, 0xFF0000);
                }
                else
                    MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_NBYTES_MASK, (((uint32_t)hi2c->XferCount) << I2C_CR2_NBYTES_POS));
            }
        }

        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_STOP_MASK);
        /* Wait until STOP flag is set */
        if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_CR2_STOP_MASK, Timeout, tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Receives in master mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* Init tickstart for timeout management*/
    uint32_t tickstart = systick_get_value();

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        if (Size == 0U)
        {
            return HAL_OK;
        }

        /* Wait until BUSY flag is reset */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
        {
            return HAL_BUSY;
        }

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_RX;
        hi2c->Mode = HAL_I2C_MODE_MASTER;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);

        /* Set NBYTES */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | I2C_CR2_RELOAD_MASK | I2C_CR2_NBYTES_MASK);
        if (hi2c->XferSize > 0xFF)
        {
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_RELOAD_MASK | 0xFF0000);
        }
        else
            SET_BIT(hi2c->Instance->CR2, (((uint32_t)hi2c->XferSize) << I2C_CR2_NBYTES_POS));

        /* Send Slave Address */
        if (I2C_MasterRequestRead(hi2c, DevAddress, Timeout, tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        if (hi2c->XferSize == 1U)
        {
            /* Disable NAck*/
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

            /* Generate Stop */
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_STOP_MASK);
        }

        while (hi2c->XferCount > 0U)
        {
            /* Wait until RXNE flag is set */
            if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            /* Read data from DR */
            *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->RXDR;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;

            if (hi2c->XferCount == 1U)
            {
                /* Disable NAck*/
                SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);
                /* Generate Stop */
                SET_BIT(hi2c->Instance->CR2, I2C_CR2_STOP_MASK);
            }
            /* Updata NBYTES */
            if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TCR) == SET)
            {
                if (hi2c->XferCount > 0xFF)
                {
                    MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_NBYTES_MASK, 0xFF0000);
                }
                else
                    MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_NBYTES_MASK, (((uint32_t)hi2c->XferCount) << I2C_CR2_NBYTES_POS));
            }
        }
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | I2C_CR2_RELOAD_MASK | I2C_CR2_NBYTES_MASK);
        //    /* Generate Stop */
        //    SET_BIT(hi2c->Instance->CR2, I2C_CR2_STOP_MASK);

        /* Wait until STOP flag is set */
        if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_CR2_STOP_MASK, Timeout, tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Transmits in slave mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* Init tickstart for timeout management*/
    uint32_t tickstart = systick_get_value();

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        if ((pData == NULL) || (Size == 0U))
        {
            return HAL_ERROR;
        }

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_TX;
        hi2c->Mode = HAL_I2C_MODE_SLAVE;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);

        /* Enable Address Acknowledge */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        /* Wait until ADDR flag is set */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout, tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* Clear ADDR flag */
        __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
        /* Clear NACKF Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_NACK);

        while (hi2c->XferCount > 0U)
        {
            /* Wait until TXE flag is set */
            if (I2C_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
            {
                return HAL_ERROR;
            }

            /* Write data to DR */
            hi2c->Instance->TXDR = *hi2c->pBuffPtr;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;
        }

        /* Wait until NACKF flag is set */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_SR_NACKF_MASK, RESET, Timeout, tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* Clear NACKF flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_SR_NACKF_MASK);

        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Receive in slave mode an amount of data in blocking mode
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    /* Init tickstart for timeout management*/
    uint32_t tickstart = systick_get_value();

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        if ((pData == NULL) || (Size == (uint16_t)0))
        {
            return HAL_ERROR;
        }

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_RX;
        hi2c->Mode = HAL_I2C_MODE_SLAVE;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);

        /* Enable Address Acknowledge */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        /* Wait until ADDR flag is set */
        if (I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, RESET, Timeout, tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* Clear ADDR flag */
        __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

        while (hi2c->XferCount > 0U)
        {
            if (hi2c->XferSize == 1U)
            {
                /* Disable NAck*/
                SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);
            }
            /* Wait until RXNE flag is set */
            if (I2C_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
            {
                /* Disable Address Acknowledge */
                SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);
                return HAL_ERROR;
            }
            /* Check if a STOPF is detected */
            if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
            {
                /* Clear STOP Flag */
                __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);
                /* Disable Address Acknowledge */
                SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

                hi2c->PreviousState = I2C_STATE_NONE;
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->Mode = HAL_I2C_MODE_NONE;
                hi2c->ErrorCode |= HAL_I2C_ERROR_NONE;

                /* Process Unlocked */
                __HAL_UNLOCK(hi2c);

                return HAL_ERROR;
            }
            /* Read data from DR */
            *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->RXDR;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;
        }

        /* Wait until STOP flag is set */
        if (I2C_WaitOnSTOPFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* Clear STOP flag */
        __HAL_I2C_CLEAR_STOPFLAG(hi2c);

        /* Disable Address Acknowledge */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Transmit in master mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    volatile uint32_t count = 0U;

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        /* Wait until BUSY flag is reset */
        count = I2C_TIMEOUT_BUSY_FLAG * (I2C_CLOCK / 25U / 1000U);
        do
        {
            count--;
            if (count == 0U)
            {
                hi2c->PreviousState = I2C_STATE_NONE;
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->Mode = HAL_I2C_MODE_NONE;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

                /* Process Unlocked */
                __HAL_UNLOCK(hi2c);

                return HAL_ERROR;
            }
        } while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) != RESET);

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_TX;
        hi2c->Mode = HAL_I2C_MODE_MASTER;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;
        hi2c->Devaddress = DevAddress;

        /* Set Reload */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | I2C_CR2_RELOAD_MASK | I2C_CR2_NBYTES_MASK);
        if (hi2c->XferSize > 0xFF)
        {
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_RELOAD_MASK | 0xFF0000);
        }
        else
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | (((uint32_t)hi2c->XferSize) << I2C_CR2_NBYTES_POS));

        if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
        {
            /* Send slave address */
            CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
            MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD1_7_MASK, (DevAddress << 1) & 0xFE);
        }
        else
        {
            /* Send header of slave address */
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
            MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD0_MASK | I2C_CR2_SADD1_7_MASK | I2C_CR2_SADD8_9_MASK, DevAddress & 0x3FF);
            CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_HEAD10R_MASK);
        }

        /* Generate Start */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_START_MASK);

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IT_EVT | I2C_IT_TXE | I2C_IT_ERR | I2C_IT_TCR | I2C_IT_TC);

        /* Enable EVT, TXE and ERR interrupt */
        __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_TXE | I2C_IT_ERR);

        if (hi2c->XferSize > 0xFF)
            __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_TCR);
        else
            __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_TC);

        //		MODIFY_REG(hi2c->Instance->CR1,I2C_CR1_TXFTH_MASK,0x1<<I2C_CR1_TXFTH_POS);

        if ((hi2c->XferCount > 0U) && (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == SET))
        {

            /* Write data to TXDR */
            hi2c->Instance->TXDR = *hi2c->pBuffPtr;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;
        }
        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}
/**
  * @brief  Transmit in master mode an amount of data in non-blocking mode with DMA
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    volatile uint32_t count = 0U;

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        if (Size == 0U)
        {
            return HAL_ERROR;
        }
        /* Wait until BUSY flag is reset */
        count = I2C_TIMEOUT_BUSY_FLAG * (I2C_CLOCK / 25U / 1000U);
        do
        {
            count--;
            if (count == 0U)
            {
                hi2c->PreviousState = I2C_STATE_NONE;
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->Mode = HAL_I2C_MODE_NONE;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

                /* Process Unlocked */
                __HAL_UNLOCK(hi2c);

                return HAL_ERROR;
            }
        } while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) != RESET);

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_TX;
        hi2c->Mode = HAL_I2C_MODE_MASTER;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;
        hi2c->Devaddress = DevAddress;

        /* Set the I2C DMA transfer complete callback */
        hi2c->hdmatx.XferCpltCallback = I2C_DMAXferCplt;

        /* Set the DMA error callback */
        hi2c->hdmatx.XferErrorCallback = I2C_DMAError;

        /* Enable the DMA channel */
        if (HAL_DMA_Start_IT(&hi2c->hdmatx, (uint32_t)hi2c->pBuffPtr, (uint32_t)&hi2c->Instance->TXDR, hi2c->XferSize))
        {
            /* Set error code to DMA */
            hi2c->ErrorCode = 0x10;

            /* Process Unlocked */
            __HAL_UNLOCK(hi2c);

            return HAL_ERROR;
        }

        /* Set Reload */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | I2C_CR2_RELOAD_MASK | I2C_CR2_NBYTES_MASK);
        if (hi2c->XferSize > 0xFF)
        {
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_RELOAD_MASK | 0xFF0000);
        }
        else
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | (((uint32_t)hi2c->XferSize) << I2C_CR2_NBYTES_POS));

        if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
        {
            /* Send slave address */
            CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
            MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD1_7_MASK, (DevAddress << 1) & 0xFE);
        }
        else
        {
            /* Send header of slave address */
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
            MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD0_MASK | I2C_CR2_SADD1_7_MASK | I2C_CR2_SADD8_9_MASK, DevAddress & 0x3FF);
            CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_HEAD10R_MASK);
        }

        /* Generate Start */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_START_MASK);

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IT_EVT | I2C_IT_TXE | I2C_IT_ERR | I2C_IT_TCR | I2C_IT_TC);

        /* Enable EVT and ERR interrupt */
        __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_ERR);

        if (hi2c->XferSize > 0xFF)
            __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_TCR);
        else
            __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_TC);

        /* Enable DMA Request */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_TXDMAEN_MASK);

        if ((hi2c->XferCount > 0U) && (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == SET))
        {

            /* Write data to TXDR */
            hi2c->Instance->TXDR = *hi2c->pBuffPtr;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;
        }
        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}
/**
  * @brief  Receive in master mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    volatile uint32_t count = 0U;

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        /* Wait until BUSY flag is reset */
        count = I2C_TIMEOUT_BUSY_FLAG * (I2C_CLOCK / 25U / 1000U);
        do
        {
            count--;
            if (count == 0U)
            {
                hi2c->PreviousState = I2C_STATE_NONE;
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->Mode = HAL_I2C_MODE_NONE;
                hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

                /* Process Unlocked */
                __HAL_UNLOCK(hi2c);

                return HAL_ERROR;
            }
        } while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY) != RESET);

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_RX;
        hi2c->Mode = HAL_I2C_MODE_MASTER;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;
        hi2c->Devaddress = DevAddress;

        /* Enable Acknowledge */
        if (hi2c->XferSize == 1)
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);
        else
            CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        /* Set Reload */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | I2C_CR2_RELOAD_MASK | I2C_CR2_NBYTES_MASK);
        if (hi2c->XferSize > 0xFF)
        {
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_RELOAD_MASK | 0xFF0000);
        }
        else
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK | (((uint32_t)hi2c->XferSize) << I2C_CR2_NBYTES_POS));

        if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
        {
            /* Send slave address */
            CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
            MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD1_7_MASK, I2C_CR2_RD_WEN_MASK | ((DevAddress << 1) & 0xFE));
        }
        else
        {
            /* Send header of slave address */
            SET_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
            MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD0_MASK | I2C_CR2_SADD1_7_MASK | I2C_CR2_SADD8_9_MASK,
                       I2C_CR2_RD_WEN_MASK | (DevAddress & 0x3FF));
            CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_HEAD10R_MASK);
        }

        /* Generate Start */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_START_MASK);

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_ERR);

        /* Enable EVT, RXNE and ERR interrupt */
        __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_ERR);

        if (hi2c->XferSize > 255)
            __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_TCR);
        else
            __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_TC);
        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Transmit in slave mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        if ((pData == NULL) || (Size == 0U))
        {
            return HAL_ERROR;
        }

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_TX;
        hi2c->Mode = HAL_I2C_MODE_SLAVE;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;

        /* Enable Address Acknowledge */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IT_EVT | I2C_IT_TXE | I2C_IT_ERR);

        /* Enable EVT, TXE and ERR interrupt */
        __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_TXE | I2C_IT_ERR);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Receive in slave mode an amount of data in non-blocking mode with Interrupt
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{

    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        if ((pData == NULL) || (Size == 0U))
        {
            return HAL_ERROR;
        }

        /* Process Locked */
        __HAL_LOCK(hi2c);

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        hi2c->State = HAL_I2C_STATE_BUSY_RX;
        hi2c->Mode = HAL_I2C_MODE_SLAVE;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Prepare transfer parameters */
        hi2c->pBuffPtr = pData;
        hi2c->XferCount = Size;
        hi2c->XferSize = hi2c->XferCount;
        hi2c->XferOptions = I2C_NO_OPTION_FRAME;

        /* Enable Address Acknowledge */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_ERR);

        /* Enable EVT, RXNE and ERR interrupt */
        __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_ERR);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Enable the Address listen mode with Interrupt.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->State == HAL_I2C_STATE_READY)
    {
        hi2c->State = HAL_I2C_STATE_LISTEN;

        /* Check if the I2C is already enabled */
        if ((hi2c->Instance->CR1 & I2C_CR1_PE_MASK) != I2C_CR1_PE_MASK)
        {
            /* Enable I2C peripheral */
            __HAL_I2C_ENABLE(hi2c);
        }

        /* Enable Address Acknowledge */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        /* Enable EVT and ERR interrupt */
        __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_ERR);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Disable the Address listen mode with Interrupt.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c)
{
    /* Declaration of tmp to prevent undefined behavior of volatile usage */
    uint32_t tmp;

    /* Disable Address listen mode only if a transfer is not ongoing */
    if (hi2c->State == HAL_I2C_STATE_LISTEN)
    {
        tmp = (uint32_t)(hi2c->State) & I2C_STATE_MSK;
        hi2c->PreviousState = tmp | (uint32_t)(hi2c->Mode);
        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Disable Address Acknowledge */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        /* Disable EVT and ERR interrupt */
        __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_ERR);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Abort a master I2C IT or DMA process communication with Interrupt.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(DevAddress);

    /* Abort Master transfer during Receive or Transmit process    */
    if (hi2c->Mode == HAL_I2C_MODE_MASTER)
    {
        /* Process Locked */
        __HAL_LOCK(hi2c);

        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State = HAL_I2C_STATE_ABORT;

        /* Disable Acknowledge */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_STOP_MASK);

        hi2c->XferCount = 0U;

        /* Clear SR Flag */
        __HAL_I2C_CLEAR_SR(hi2c);
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_TXE | I2C_IT_ERR);

        /* Disable EVT, I2C_IT_RXNE, I2C_IT_TXE and ERR interrupt */
        __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_TXE | I2C_IT_ERR);

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        /* Call the corresponding callback to inform upper layer of End of Transfer */
        I2C_ITError(hi2c);

        return HAL_OK;
    }
    else
    {
        /* Wrong usage of abort function */
        /* This function should be used only in case of abort monitored by master device */
        return HAL_ERROR;
    }
}

/**
  * @}
  */

/** @defgroup I2C_IRQ_Handler
 * @{
 */

/**
  * @brief  This function handles I2C event interrupt request.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_IRQHandler(I2C_HandleTypeDef *hi2c)
{
    uint32_t sr1itflags = hi2c->Instance->SR;
    uint32_t itsources = hi2c->Instance->IVS;

    //uint32_t CurrentXferOptions = hi2c->XferOptions;
    //HAL_I2C_ModeTypeDef CurrentMode = hi2c->Mode;
    HAL_I2C_StateTypeDef CurrentState = hi2c->State;
    uint32_t error = HAL_I2C_ERROR_NONE;

    /* Check NACK flag */
    if ((I2C_CHECK_FLAG(sr1itflags, I2C_FLAG_NACK) != RESET) && (I2C_CHECK_IT_SOURCE(itsources, I2C_IER_NACKIE_MASK) != RESET))
    {
        /* Clear AF flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_NACK);
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IER_NACKIE_MASK);

        if (CurrentState == HAL_I2C_STATE_BUSY_TX)
        {
            if (hi2c->XferCount != 0)
            {
                error |= HAL_I2C_ERROR_NACKF;
                /* Do not generate a STOP in case of Slave receive non acknowledge during transfer (mean not at the end of transfer) */
                if (hi2c->Mode == HAL_I2C_MODE_MASTER)
                {
                    /* Generate Stop */
                    SET_BIT(hi2c->Instance->CR2, I2C_CR2_STOP_MASK);
                }
            }
        }
    }

    /* Check STOP flag */
    if (I2C_CHECK_FLAG(sr1itflags, I2C_FLAG_STOPF) != RESET)
    {
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IER_STOPIE_MASK);

        if (hi2c->Mode == HAL_I2C_MODE_SLAVE)
        {
            I2C_Slave_STOPF(hi2c);
        }
    }

    /* Check ADDR flag */
    if (I2C_CHECK_FLAG(sr1itflags, I2C_FLAG_ADDR) != RESET)
    {
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IER_ADDRIE_MASK);

        I2C_Slave_ADDR(hi2c, sr1itflags);
    }

    /*  Check TX flag -----------------------------------------------*/
    if ((I2C_CHECK_FLAG(sr1itflags, I2C_FLAG_TXE) != RESET) && (I2C_CHECK_IT_SOURCE(itsources, I2C_IT_TXE) != RESET))
    {
        if (hi2c->Mode == HAL_I2C_MODE_MASTER)
        {
            I2C_MasterTransmit_TXE(hi2c);
        }
        else
        {
            I2C_SlaveTransmit_TXE(hi2c);
        }
    }
    /* Check RXNE flag -----------------------------------------------*/
    if ((I2C_CHECK_FLAG(sr1itflags, I2C_FLAG_RXNE) != RESET) && (I2C_CHECK_IT_SOURCE(itsources, I2C_IT_RXNE) != RESET))
    {
        if (hi2c->Mode == HAL_I2C_MODE_MASTER)
        {
            I2C_MasterReceive_RXNE(hi2c);
        }
        else
        {
            I2C_SlaveReceive_RXNE(hi2c);
        }
    }
    /*  Check TCR flag -----------------------------------------------*/
    if ((I2C_CHECK_FLAG(sr1itflags, I2C_FLAG_TCR) != RESET) && (I2C_CHECK_IT_SOURCE(itsources, I2C_IT_TCR) != RESET))
    {
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IER_TCRIE_MASK);

        if (hi2c->Mode == HAL_I2C_MODE_MASTER)
        {
            I2C_Master_TCR(hi2c);
        }
    }
    /*  Check TC flag -----------------------------------------------*/
    if ((I2C_CHECK_FLAG(sr1itflags, I2C_FLAG_TC) != RESET) && (I2C_CHECK_IT_SOURCE(itsources, I2C_IT_TC) != RESET))
    {
        /* Clear interrupt Flag */
        __HAL_I2C_CLEAR_IF(hi2c, I2C_IER_TCIE_MASK);
        if (hi2c->XferCount != 0)
        {
            error |= HAL_I2C_ERROR_SIZE;
        }
    }
    /* Call the Error Callback in case of Error detected -----------------------*/
    if (error != HAL_I2C_ERROR_NONE)
    {
        hi2c->ErrorCode |= error;
        I2C_ITError(hi2c);
    }
}

/**
  * @brief  Master Tx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MasterTxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Master Rx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_MasterRxCpltCallback could be implemented in the user file
   */
}

/** @brief  Slave Tx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_SlaveTxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Slave Rx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_SlaveRxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Slave Address Match callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  TransferDirection Master request Transfer Direction (Write/Read), value of @ref I2C_XferDirection_definition
  * @param  AddrMatchCode Address Match Code
  * @retval None
  */
__weak void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);
    UNUSED(TransferDirection);
    UNUSED(AddrMatchCode);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_AddrCallback() could be implemented in the user file
   */
}

/**
  * @brief  Listen Complete callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_ListenCpltCallback() could be implemented in the user file
  */
}

/**
  * @brief  I2C error callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  I2C abort callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
__weak void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);

    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_I2C_AbortCpltCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup I2C_Exported_Functions_Group3 Peripheral State, Mode and Error functions
 *  @brief   Peripheral State, Mode and Error functions
  *
@verbatim
 ===============================================================================
            ##### Peripheral State, Mode and Error functions #####
 ===============================================================================
    [..]
    This subsection permit to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the I2C handle state.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL state
  */
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c)
{
    /* Return I2C handle state */
    return hi2c->State;
}

/**
  * @brief  Returns the I2C Master, Slave, Memory or no mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval HAL mode
  */
HAL_I2C_ModeTypeDef HAL_I2C_GetMode(I2C_HandleTypeDef *hi2c)
{
    return hi2c->Mode;
}

/**
  * @brief  Return the I2C error code.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *              the configuration information for the specified I2C.
  * @retval I2C Error Code
  */
uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *hi2c)
{
    return hi2c->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup I2C_Private_Functions
  * @{
  */

/**
  * @brief  Handle TXE flag for Master
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_MasterTransmit_TXE(I2C_HandleTypeDef *hi2c)
{
    /* Declaration of temporary variables to prevent undefined behavior of volatile usage */
    //HAL_I2C_ModeTypeDef CurrentMode = hi2c->Mode;
    //uint32_t CurrentXferOptions = hi2c->XferOptions;

    if (hi2c->State == HAL_I2C_STATE_BUSY_TX)
    {
        if (hi2c->XferCount == 0U)
        {
            /* Disable EVT, TXE and ERR interrupt */
            __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_TXE | I2C_IT_ERR | I2C_IT_TC | I2C_IT_TCR);

            hi2c->PreviousState = I2C_STATE_NONE;
            hi2c->State = HAL_I2C_STATE_READY;

            hi2c->Mode = HAL_I2C_MODE_NONE;
            HAL_I2C_MasterTxCpltCallback(hi2c);
        }
        else
        {
            /* Write data to TXDR */
            hi2c->Instance->TXDR = *hi2c->pBuffPtr;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;
        }
    }
    else
    {
        /* Do nothing */
    }
}

/**
  * @brief  Handle RXNE flag for Master
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_MasterReceive_RXNE(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->State == HAL_I2C_STATE_BUSY_RX)
    {
        if (hi2c->XferCount > 0)
        {
            /* Read data from RXDR */
            *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->RXDR;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;
        }
        if (hi2c->XferCount == 0)
        {
            /* Disable EVT, I2C_IT_RXNE and ERR interrupt */
            __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_ERR | I2C_IT_TC | I2C_IT_TCR);

            hi2c->State = HAL_I2C_STATE_READY;

            hi2c->Mode = HAL_I2C_MODE_NONE;
            hi2c->PreviousState = I2C_STATE_MASTER_BUSY_RX;

            HAL_I2C_MasterRxCpltCallback(hi2c);
        }
    }
    else
    {
        /* Do nothing */
    }
}

/**
  * @brief  Handle RXNE flag for Master
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_Master_TCR(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->XferCount > 0xFF)
    {
        MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_NBYTES_MASK, ((uint32_t)0x0000FF) << I2C_CR2_NBYTES_POS);
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_RELOAD_MASK);
    }
    else
    {
        MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_NBYTES_MASK, ((uint32_t)hi2c->XferCount) << I2C_CR2_NBYTES_POS);
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_RELOAD_MASK);
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_AUTOEND_MASK);
    }
}

/**
  * @brief  Handle TXE flag for Slave
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c)
{
    /* Declaration of temporary variables to prevent undefined behavior of volatile usage */
    HAL_I2C_StateTypeDef CurrentState = hi2c->State;

    if (hi2c->XferCount != 0U)
    {
        /* Write data to TXDR */
        hi2c->Instance->TXDR = *hi2c->pBuffPtr;

        /* Increment Buffer pointer */
        hi2c->pBuffPtr++;

        /* Update counter */
        hi2c->XferCount--;
        if ((hi2c->XferCount == 0U) && (CurrentState == HAL_I2C_STATE_BUSY_TX_LISTEN))
        {
            /* Last Byte is received, disable Interrupt */
            __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_TXE);

            /* Set state at HAL_I2C_STATE_LISTEN */
            hi2c->PreviousState = I2C_STATE_SLAVE_BUSY_TX;
            hi2c->State = HAL_I2C_STATE_LISTEN;

            /* Call the corresponding callback to inform upper layer of End of Transfer */
            HAL_I2C_SlaveTxCpltCallback(hi2c);
        }
    }
}

/**
  * @brief  Handle RXNE flag for Slave
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c)
{
    /* Declaration of temporary variables to prevent undefined behavior of volatile usage */
    HAL_I2C_StateTypeDef CurrentState = hi2c->State;

    if (hi2c->XferCount != 0U)
    {
        /* Read data from RXDR */
        *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->RXDR;

        /* Increment Buffer pointer */
        hi2c->pBuffPtr++;

        /* Update counter */
        hi2c->XferCount--;

        if ((hi2c->XferCount == 0U) && (CurrentState == HAL_I2C_STATE_BUSY_RX_LISTEN))
        {
            /* Last Byte is received, disable Interrupt */
            __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_RXNE);

            /* Set state at HAL_I2C_STATE_LISTEN */
            hi2c->PreviousState = I2C_STATE_SLAVE_BUSY_RX;
            hi2c->State = HAL_I2C_STATE_LISTEN;

            /* Call the corresponding callback to inform upper layer of End of Transfer */
            HAL_I2C_SlaveRxCpltCallback(hi2c);
        }
    }
}

/**
  * @brief  Handle ADD flag for Slave
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  IT2Flags Interrupt2 flags to handle.
  * @retval None
  */
static void I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c, uint32_t IT2Flags)
{
    uint8_t TransferDirection = I2C_DIRECTION_RECEIVE;
    uint8_t SlaveAddrCode;

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ADDR);

    if (((uint32_t)hi2c->State & (uint32_t)HAL_I2C_STATE_LISTEN) == (uint32_t)HAL_I2C_STATE_LISTEN)
    {
        /* Disable BUF interrupt, BUF enabling is manage through slave specific interface */
        __HAL_I2C_DISABLE_IT(hi2c, (I2C_IT_RXNE | I2C_IT_TXE));

        /* Transfer Direction requested by Master */
        if (I2C_CHECK_FLAG(IT2Flags, I2C_FLAG_DIR) == SET)
        {
            TransferDirection = I2C_DIRECTION_TRANSMIT;
        }

        SlaveAddrCode = (IT2Flags >> I2C_SR_ADDCODE_POS) & 0xEF;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        /* Call Slave Addr callback */
        HAL_I2C_AddrCallback(hi2c, TransferDirection, SlaveAddrCode);
    }
    else
    {

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);
    }
}

/**
  * @brief  Handle STOPF flag for Slave
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @retval None
  */
static void I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c)
{
    /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
    HAL_I2C_StateTypeDef CurrentState = hi2c->State;

    /* Disable EVT, BUF and ERR interrupt */
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_ERR);

    /* Clear STOPF flag */
    __HAL_I2C_CLEAR_STOPFLAG(hi2c);

    /* Disable Acknowledge */
    SET_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

    //  /* If a DMA is ongoing, Update handle size context */
    //  if (((hi2c->Instance->CR1 & I2C_CR1_TXDMAEN_MASK) == I2C_CR1_TXDMAEN_MASK)
    //		||((hi2c->Instance->CR1 & I2C_CR1_RXDMAEN_MASK) == I2C_CR1_RXDMAEN_MASK))
    //  {
    //    if ((CurrentState == HAL_I2C_STATE_BUSY_RX) || (CurrentState == HAL_I2C_STATE_BUSY_RX_LISTEN))
    //    {
    //      hi2c->XferCount = (uint16_t)(__HAL_DMA_GET_COUNTER(hi2c->hdmarx));

    //      if (hi2c->XferCount != 0U)
    //      {
    //        /* Set ErrorCode corresponding to a Non-Acknowledge */
    //        hi2c->ErrorCode |= HAL_I2C_ERROR_NACKF;
    //      }

    //      /* Disable, stop the current DMA */
    //      CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_RXDMAEN_MASK);

    //      /* Abort DMA Xfer if any */
    //      if (HAL_DMA_GetState(hi2c->hdmarx) != HAL_DMA_STATE_READY)
    //      {
    //        /* Set the I2C DMA Abort callback :
    //        will lead to call HAL_I2C_ErrorCallback() at end of DMA abort procedure */
    //        hi2c->hdmarx->XferAbortCallback = I2C_DMAAbort;

    //        /* Abort DMA RX */
    //        if (HAL_DMA_Abort_IT(hi2c->hdmarx) != HAL_OK)
    //        {
    //          /* Call Directly XferAbortCallback function in case of error */
    //          hi2c->hdmarx->XferAbortCallback(hi2c->hdmarx);
    //        }
    //      }
    //    }
    //    else
    //    {
    //      hi2c->XferCount = (uint16_t)(__HAL_DMA_GET_COUNTER(hi2c->hdmatx));

    //      if (hi2c->XferCount != 0U)
    //      {
    //        /* Set ErrorCode corresponding to a Non-Acknowledge */
    //        hi2c->ErrorCode |= HAL_I2C_ERROR_NACKF;
    //      }

    //      /* Disable, stop the current DMA */
    //      CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_TXDMAEN_MASK);

    //      /* Abort DMA Xfer if any */
    //      if (HAL_DMA_GetState(hi2c->hdmatx) != HAL_DMA_STATE_READY)
    //      {
    //        /* Set the I2C DMA Abort callback :
    //        will lead to call HAL_I2C_ErrorCallback() at end of DMA abort procedure */
    //        hi2c->hdmatx->XferAbortCallback = I2C_DMAAbort;

    //        /* Abort DMA TX */
    //        if (HAL_DMA_Abort_IT(hi2c->hdmatx) != HAL_OK)
    //        {
    //          /* Call Directly XferAbortCallback function in case of error */
    //          hi2c->hdmatx->XferAbortCallback(hi2c->hdmatx);
    //        }
    //      }
    //    }
    //  }

    /* All data are not transferred, so set error code accordingly */
    if (hi2c->XferCount != 0U)
    {

        /* Store Last receive data if any */
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET)
        {
            /* Read data from DR */
            *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->RXDR;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;

            /* Update counter */
            hi2c->XferCount--;
        }

        if (hi2c->XferCount != 0U)
        {
            /* Set ErrorCode corresponding to a Non-Acknowledge */
            hi2c->ErrorCode |= HAL_I2C_ERROR_NACKF;
        }
    }

    if (hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
    {
        /* Call the corresponding callback to inform upper layer of End of Transfer */
        I2C_ITError(hi2c);
    }
    else
    {
        if (CurrentState == HAL_I2C_STATE_BUSY_RX_LISTEN)
        {
            /* Set state at HAL_I2C_STATE_LISTEN */
            hi2c->PreviousState = I2C_STATE_NONE;
            hi2c->State = HAL_I2C_STATE_LISTEN;

            /* Call the corresponding callback to inform upper layer of End of Transfer */
            HAL_I2C_SlaveRxCpltCallback(hi2c);
        }

        if (hi2c->State == HAL_I2C_STATE_LISTEN)
        {
            hi2c->XferOptions = I2C_NO_OPTION_FRAME;
            hi2c->PreviousState = I2C_STATE_NONE;
            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->Mode = HAL_I2C_MODE_NONE;

            /* Call the Listen Complete callback, to inform upper layer of the end of Listen usecase */
            HAL_I2C_ListenCpltCallback(hi2c);
        }
        else
        {
            if ((hi2c->PreviousState == I2C_STATE_SLAVE_BUSY_RX) || (CurrentState == HAL_I2C_STATE_BUSY_RX))
            {
                hi2c->PreviousState = I2C_STATE_NONE;
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->Mode = HAL_I2C_MODE_NONE;

                HAL_I2C_SlaveRxCpltCallback(hi2c);
            }
            else if ((hi2c->PreviousState == I2C_STATE_SLAVE_BUSY_TX) || (CurrentState == HAL_I2C_STATE_BUSY_TX))
            {
                hi2c->PreviousState = I2C_STATE_NONE;
                hi2c->State = HAL_I2C_STATE_READY;
                hi2c->Mode = HAL_I2C_MODE_NONE;

                HAL_I2C_SlaveTxCpltCallback(hi2c);
            }
        }
    }
}

/**
  * @brief  I2C interrupts error process
  * @param  hi2c I2C handle.
  * @retval None
  */
static void I2C_ITError(I2C_HandleTypeDef *hi2c)
{
    /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
    HAL_I2C_StateTypeDef CurrentState = hi2c->State;

    if (((uint32_t)CurrentState & (uint32_t)HAL_I2C_STATE_LISTEN) == (uint32_t)HAL_I2C_STATE_LISTEN)
    {
        /* keep HAL_I2C_STATE_LISTEN */
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State = HAL_I2C_STATE_LISTEN;
    }
    else
    {
        /* If state is an abort treatment on going, don't change state */
        /* This change will be do later */
        if (((READ_BIT(hi2c->Instance->CR2, I2C_CR1_TXDMAEN_MASK) != I2C_CR1_TXDMAEN_MASK) || (READ_BIT(hi2c->Instance->CR2, I2C_CR1_RXDMAEN_MASK) != I2C_CR1_RXDMAEN_MASK)) && (CurrentState != HAL_I2C_STATE_ABORT))
        {
            hi2c->State = HAL_I2C_STATE_READY;
        }
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->Mode = HAL_I2C_MODE_NONE;
    }

    //  /* Abort DMA transfer */
    //  if (READ_BIT(hi2c->Instance->CR2, I2C_CR2_DMAEN) == I2C_CR2_DMAEN)
    //  {
    //    hi2c->Instance->CR2 &= ~I2C_CR2_DMAEN;

    //    if (hi2c->hdmatx->State != HAL_DMA_STATE_READY)
    //    {
    //      /* Set the DMA Abort callback :
    //      will lead to call HAL_I2C_ErrorCallback() at end of DMA abort procedure */
    //      hi2c->hdmatx->XferAbortCallback = I2C_DMAAbort;

    //      if (HAL_DMA_Abort_IT(hi2c->hdmatx) != HAL_OK)
    //      {
    //        /* Disable I2C peripheral to prevent dummy data in buffer */
    //        __HAL_I2C_DISABLE(hi2c);

    //        hi2c->State = HAL_I2C_STATE_READY;

    //        /* Call Directly XferAbortCallback function in case of error */
    //        hi2c->hdmatx->XferAbortCallback(hi2c->hdmatx);
    //      }
    //    }
    //    else
    //    {
    //      /* Set the DMA Abort callback :
    //      will lead to call HAL_I2C_ErrorCallback() at end of DMA abort procedure */
    //      hi2c->hdmarx->XferAbortCallback = I2C_DMAAbort;

    //      if (HAL_DMA_Abort_IT(hi2c->hdmarx) != HAL_OK)
    //      {
    //        /* Store Last receive data if any */
    //        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET)
    //        {
    //          /* Read data from DR */
    //          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

    //          /* Increment Buffer pointer */
    //          hi2c->pBuffPtr++;
    //        }

    //        /* Disable I2C peripheral to prevent dummy data in buffer */
    //        __HAL_I2C_DISABLE(hi2c);

    //        hi2c->State = HAL_I2C_STATE_READY;

    //        /* Call Directly hi2c->hdmarx->XferAbortCallback function in case of error */
    //        hi2c->hdmarx->XferAbortCallback(hi2c->hdmarx);
    //      }
    //    }
    //  }
    //  else
    if (hi2c->State == HAL_I2C_STATE_ABORT)
    {
        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

        /* Store Last receive data if any */
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET)
        {
            /* Read data from TXDR */
            *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->TXDR;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;
        }

        /* Disable I2C peripheral to prevent dummy data in buffer */
        __HAL_I2C_DISABLE(hi2c);

        /* Call the corresponding callback to inform upper layer of End of Transfer */
        HAL_I2C_AbortCpltCallback(hi2c);
    }
    else
    {
        /* Store Last receive data if any */
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == SET)
        {
            /* Read data from TXDR */
            *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->TXDR;

            /* Increment Buffer pointer */
            hi2c->pBuffPtr++;
        }

        /* Call user error callback */
        HAL_I2C_ErrorCallback(hi2c);
    }
    /* STOP Flag is not set after a NACK reception */
    /* So may inform upper layer that listen phase is stopped */
    /* during NACK error treatment */
    CurrentState = hi2c->State;
    if (((hi2c->ErrorCode & HAL_I2C_ERROR_NACKF) == HAL_I2C_ERROR_NACKF) && (CurrentState == HAL_I2C_STATE_LISTEN))
    {
        /* Disable EVT, BUF and ERR interrupt */
        __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_RXNE | I2C_IT_TXE | I2C_IT_ERR);

        hi2c->XferOptions = I2C_NO_OPTION_FRAME;
        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;

        /* Call the Listen Complete callback, to inform upper layer of the end of Listen usecase */
        HAL_I2C_ListenCpltCallback(hi2c);
    }
}

/**
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterRequestWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart)
{
    /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
    uint32_t CurrentXferOptions = hi2c->XferOptions;

    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
    {
        /* Send slave address */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
        MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD1_7_MASK, (DevAddress << 1) & 0xFE);
    }
    else
    {
        /* Send header of slave address */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
        MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD0_MASK | I2C_CR2_SADD1_7_MASK | I2C_CR2_SADD8_9_MASK, DevAddress & 0x3FF);
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_HEAD10R_MASK);
    }

    /* Generate Start condition if first transfer */
    if ((CurrentXferOptions == I2C_FIRST_AND_LAST_FRAME) || (CurrentXferOptions == I2C_FIRST_FRAME) || (CurrentXferOptions == I2C_NO_OPTION_FRAME))
    {
        /* Generate Start */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_START_MASK);
    }
    else if (hi2c->PreviousState == I2C_STATE_MASTER_BUSY_RX)
    {
        /* Generate ReStart */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_START_MASK);
    }
    else
    {
        /* Do nothing */
    }

    /* Wait until ADDR flag is set */
    if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_CR2_START_MASK, Timeout, Tickstart) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
  * @brief  Master sends target device address for read request.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_MasterRequestRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart)
{
    /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
    uint32_t CurrentXferOptions = hi2c->XferOptions;

    /* Enable Ack*/
    CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_NACK_MASK);

    if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
    {
        /* Send slave address */
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
        MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD1_7_MASK, I2C_CR2_RD_WEN_MASK | ((DevAddress << 1) & 0xFE));
    }
    else
    {
        /* Send header of slave address */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_SADD10_MASK);
        MODIFY_REG(hi2c->Instance->CR2, I2C_CR2_RD_WEN_MASK | I2C_CR2_SADD0_MASK | I2C_CR2_SADD1_7_MASK | I2C_CR2_SADD8_9_MASK, I2C_CR2_RD_WEN_MASK | (DevAddress & 0x3FF));
        CLEAR_BIT(hi2c->Instance->CR2, I2C_CR2_HEAD10R_MASK);
    }

    /* Generate Start condition if first transfer */
    if ((CurrentXferOptions == I2C_FIRST_AND_LAST_FRAME) || (CurrentXferOptions == I2C_FIRST_FRAME) || (CurrentXferOptions == I2C_NO_OPTION_FRAME))
    {
        /* Generate Start */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_START_MASK);
    }
    else if (hi2c->PreviousState == I2C_STATE_MASTER_BUSY_TX)
    {
        /* Generate ReStart */
        SET_BIT(hi2c->Instance->CR2, I2C_CR2_START_MASK);
    }
    else
    {
        /* Do nothing */
    }

    /* Wait until ADDR flag is set */
    if (I2C_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_CR2_START_MASK, Timeout, Tickstart) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
  * @brief  DMA I2C process complete callback.
  * @param  hdma DMA handle
  * @retval None
  */
static void I2C_DMAXferCplt(DMA_HandleTypeDef *hdma)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent; /* Derogation MISRAC2012-Rule-11.5 */

    /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
    HAL_I2C_StateTypeDef CurrentState = hi2c->State;
    HAL_I2C_ModeTypeDef CurrentMode = hi2c->Mode;
    //uint32_t CurrentXferOptions = hi2c->XferOptions;

    /* Disable EVT and ERR interrupt */
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_ERR);

    /* Clear Complete callback */
    hi2c->hdmatx.XferCpltCallback = NULL;
    hi2c->hdmarx.XferCpltCallback = NULL;

    if (CurrentMode == HAL_I2C_MODE_MASTER)
    {
        if (CurrentState == HAL_I2C_STATE_BUSY_TX)
        {
            /* Disable EVT, TXE and ERR interrupt */
            __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_ERR | I2C_IT_TC | I2C_IT_TCR);

            /* Disable DMA Request */
            CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_TXDMAEN_MASK);

            hi2c->PreviousState = I2C_STATE_NONE;
            hi2c->State = HAL_I2C_STATE_READY;

            hi2c->Mode = HAL_I2C_MODE_NONE;

            HAL_I2C_MasterTxCpltCallback(hi2c);
        }
    }
    else
    {
        /* Do nothing */
    }
}

/**
  * @brief  DMA I2C communication error callback.
  * @param  hdma DMA handle
  * @retval None
  */
static void I2C_DMAError(DMA_HandleTypeDef *hdma)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent; /* Derogation MISRAC2012-Rule-11.5 */

    /* Clear Complete callback */
    hi2c->hdmatx.XferCpltCallback = NULL;
    hi2c->hdmarx.XferCpltCallback = NULL;

    hi2c->XferCount = 0U;
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;
    hi2c->ErrorCode |= HAL_I2C_ERROR_DMA;

    HAL_I2C_ErrorCallback(hi2c);
}

///**
//  * @brief DMA I2C communication abort callback
//  *        (To be called at end of DMA Abort procedure).
//  * @param hdma DMA handle.
//  * @retval None
//  */
//static void I2C_DMAAbort(DMA_HandleTypeDef *hdma)
//{
//  I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent; /* Derogation MISRAC2012-Rule-11.5 */

//  /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
//  HAL_I2C_StateTypeDef CurrentState = hi2c->State;

//  /* Clear Complete callback */
//  hi2c->hdmatx->XferCpltCallback = NULL;
//  hi2c->hdmarx->XferCpltCallback = NULL;

//  /* Disable Acknowledge */
//  CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

//  hi2c->XferCount = 0U;

//  /* Reset XferAbortCallback */
//  hi2c->hdmatx->XferAbortCallback = NULL;
//  hi2c->hdmarx->XferAbortCallback = NULL;

//  /* Disable I2C peripheral to prevent dummy data in buffer */
//  __HAL_I2C_DISABLE(hi2c);

//  /* Check if come from abort from user */
//  if (hi2c->State == HAL_I2C_STATE_ABORT)
//  {
//    hi2c->State         = HAL_I2C_STATE_READY;
//    hi2c->Mode          = HAL_I2C_MODE_NONE;
//    hi2c->ErrorCode     = HAL_I2C_ERROR_NONE;

//    /* Call the corresponding callback to inform upper layer of End of Transfer */
//#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
//    hi2c->AbortCpltCallback(hi2c);
//#else
//    HAL_I2C_AbortCpltCallback(hi2c);
//#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
//  }
//  else
//  {
//    if (((uint32_t)CurrentState & (uint32_t)HAL_I2C_STATE_LISTEN) == (uint32_t)HAL_I2C_STATE_LISTEN)
//    {
//      /* Renable I2C peripheral */
//      __HAL_I2C_ENABLE(hi2c);

//      /* Enable Acknowledge */
//      SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

//      /* keep HAL_I2C_STATE_LISTEN */
//      hi2c->PreviousState = I2C_STATE_NONE;
//      hi2c->State = HAL_I2C_STATE_LISTEN;
//    }
//    else
//    {
//      hi2c->State = HAL_I2C_STATE_READY;
//      hi2c->Mode = HAL_I2C_MODE_NONE;
//    }

//    /* Call the corresponding callback to inform upper layer of End of Transfer */
//#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
//    hi2c->ErrorCallback(hi2c);
//#else
//    HAL_I2C_ErrorCallback(hi2c);
//#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
//  }
//}

/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag specifies the I2C flag to check.
  * @param  Status The new Flag status (SET or RESET).
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart)
{
    /* Wait until flag is set */
    while (__HAL_I2C_GET_FLAG(hi2c, Flag) == Status)
    {
        /* Check for the Timeout */
        // if (Timeout != HAL_MAX_DELAY)
        // {
        //     if (((systick_get_value() - Tickstart) >= Timeout) || (Timeout == 0U))
        //     {
        //         hi2c->PreviousState = I2C_STATE_NONE;
        //         hi2c->State = HAL_I2C_STATE_READY;
        //         hi2c->Mode = HAL_I2C_MODE_NONE;
        //         hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

        //         /* Process Unlocked */
        //         __HAL_UNLOCK(hi2c);
        //         LOG_I("i2c timeout 1");
        //         return HAL_ERROR;
        //     }
        // }
    }
    return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for Master addressing phase.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag specifies the I2C flag to check.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart)
{
    while (__HAL_I2C_GET_CR2_FLAG(hi2c, Flag) == SET)
    {
        /* Check for the Timeout */
        // if (Timeout != HAL_MAX_DELAY)
        // {
        //     if (((systick_get_value() - Tickstart) >= Timeout) || (Timeout == 0U))
        //     {
        //         hi2c->PreviousState = I2C_STATE_NONE;
        //         hi2c->State = HAL_I2C_STATE_READY;
        //         hi2c->Mode = HAL_I2C_MODE_NONE;
        //         hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

        //         /* Process Unlocked */
        //         __HAL_UNLOCK(hi2c);
        //         LOG_I("i2c timeout 2");
        //         return HAL_ERROR;
        //     }
        // }
    }
    return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of TXE flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnTXEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
    while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET)
    {
        /* Check if a NACK is detected */
        if (I2C_IsAcknowledgeFailed(hi2c) != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* Check for the Timeout */
        if (Timeout != HAL_MAX_DELAY)
        {
            // if (((systick_get_value() - Tickstart) >= Timeout) || (Timeout == 0U))
            // {
            //     hi2c->PreviousState = I2C_STATE_NONE;
            //     hi2c->State = HAL_I2C_STATE_READY;
            //     hi2c->Mode = HAL_I2C_MODE_NONE;
            //     hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

            //     /* Process Unlocked */
            //     __HAL_UNLOCK(hi2c);

            //     return HAL_ERROR;
            // }
        }
    }
    return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of STOP flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
    while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == RESET)
    {
        /* Check if a NACK is detected */
        if (I2C_IsAcknowledgeFailed(hi2c) != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* Check for the Timeout */
        // if (((systick_get_value() - Tickstart) >= Timeout) || (Timeout == 0U))
        // {
        //     hi2c->PreviousState = I2C_STATE_NONE;
        //     hi2c->State = HAL_I2C_STATE_READY;
        //     hi2c->Mode = HAL_I2C_MODE_NONE;
        //     hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

        //     /* Process Unlocked */
        //     __HAL_UNLOCK(hi2c);

        //     return HAL_ERROR;
        // }
    }
    return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of RXNE flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{

    while (__HAL_I2C_GET_FLAG(hi2c, I2C_SR_RXNE_MASK) == RESET)
    {
        /* Check if a STOPF is detected */
        if (__HAL_I2C_GET_FLAG(hi2c, I2C_SR_STOPF_MASK) == SET)
        {
            /* Clear STOP Flag */
            __HAL_I2C_CLEAR_FLAG(hi2c, I2C_CFR_STOPCF_MASK);

            hi2c->PreviousState = I2C_STATE_NONE;
            hi2c->State = HAL_I2C_STATE_READY;
            hi2c->Mode = HAL_I2C_MODE_NONE;
            hi2c->ErrorCode |= HAL_I2C_ERROR_NONE;

            /* Process Unlocked */
            __HAL_UNLOCK(hi2c);

            return HAL_ERROR;
        }

        /* Check for the Timeout */
        // if (((systick_get_value() - Tickstart) >= Timeout) || (Timeout == 0U))
        // {
        //     hi2c->PreviousState = I2C_STATE_NONE;
        //     hi2c->State = HAL_I2C_STATE_READY;
        //     hi2c->Mode = HAL_I2C_MODE_NONE;
        //     hi2c->ErrorCode |= HAL_I2C_ERROR_TIMEOUT;

        //     /* Process Unlocked */
        //     __HAL_UNLOCK(hi2c);

        //     return HAL_ERROR;
        // }
    }
    return HAL_OK;
}

/**
  * @brief  This function handles Acknowledge failed detection during an I2C Communication.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c)
{
    if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_NACK) == SET)
    {
        /* Clear NACKF Flag */
        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_NACK);

        hi2c->PreviousState = I2C_STATE_NONE;
        hi2c->State = HAL_I2C_STATE_READY;
        hi2c->Mode = HAL_I2C_MODE_NONE;
        hi2c->ErrorCode |= HAL_I2C_ERROR_NACKF;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_ERROR;
    }
    return HAL_OK;
}

/**
  * @brief  Convert I2Cx OTHER_xxx XferOptions to functionnal XferOptions.
  * @param  hi2c I2C handle.
  * @retval None
  */
// static void I2C_ConvertOtherXferOptions(I2C_HandleTypeDef *hi2c)
// {
//     /* if user set XferOptions to I2C_OTHER_FRAME            */
//     /* it request implicitly to generate a restart condition */
//     /* set XferOptions to I2C_FIRST_FRAME                    */
//     if (hi2c->XferOptions == I2C_OTHER_FRAME)
//     {
//         hi2c->XferOptions = I2C_FIRST_FRAME;
//     }
//     /* else if user set XferOptions to I2C_OTHER_AND_LAST_FRAME */
//     /* it request implicitly to generate a restart condition    */
//     /* then generate a stop condition at the end of transfer    */
//     /* set XferOptions to I2C_FIRST_AND_LAST_FRAME              */
//     else if (hi2c->XferOptions == I2C_OTHER_AND_LAST_FRAME)
//     {
//         hi2c->XferOptions = I2C_FIRST_AND_LAST_FRAME;
//     }
//     else
//     {
//         /* Nothing to do */
//     }
// }

/**
  * @}
  */

#endif /* HAL_I2C_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
