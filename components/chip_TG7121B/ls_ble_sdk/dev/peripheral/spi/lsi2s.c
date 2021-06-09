/**
  ******************************************************************************
  * @file    le501x_hal_i2s.c
  * @author  MCD Application Team
  * @brief   SPI HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Serial Peripheral Interface (SPI) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lsi2s.h"
#include "spi_msp.h"

/** @addtogroup le501x_HAL_Driver
  * @{
  */

/** @defgroup I2S 
  * @brief I2S HAL module driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup I2S_Private_Functions
  * @{
  */
//static void               I2S_DMATxCplt(DMA_HandleTypeDef *hdma);
//static void               I2S_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
//static void               I2S_DMARxCplt(DMA_HandleTypeDef *hdma);
//static void               I2S_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
//static void               I2S_DMAError(DMA_HandleTypeDef *hdma);
static void               I2S_Transmit_IT(I2S_HandleTypeDef *hi2s);
static void               I2S_Receive_IT(I2S_HandleTypeDef *hi2s);
static HAL_StatusTypeDef  I2S_WaitFlagStateUntilTimeout(I2S_HandleTypeDef *hi2s, uint32_t Flag, FlagStatus State,
                                                        uint32_t Timeout);
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @defgroup I2S_Exported_Functions 
  * @{
  */

/** @defgroup  I2S_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          de-initialize the I2Sx peripheral in simplex mode:

      (+) User must Implement HAL_I2S_MspInit() function in which he configures
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC ).

      (+) Call the function HAL_I2S_Init() to configure the selected device with
          the selected configuration:
        (++) Mode
        (++) Standard
        (++) Data Format
        (++) MCLK Output
        (++) Audio frequency
        (++) Polarity

     (+) Call the function HAL_I2S_DeInit() to restore the default configuration
          of the selected I2Sx peripheral.
  @endverbatim
  * @{
  */

/**
  * @brief  Initializes the I2S according to the specified parameters
  *         in the I2S_InitTypeDef and create the associated handle.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Init(I2S_HandleTypeDef *hi2s)
{
  uint32_t i2sdiv;
  uint32_t i2sodd;
  uint32_t packetlength;
  uint32_t tmp;

  /* Check the I2S handle allocation */
  if (hi2s == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the I2S parameters */
//  LS_ASSERT(IS_I2S_ALL_INSTANCE(hi2s->Instance));
  LS_ASSERT(IS_I2S_MODE(hi2s->Init.Mode));
  LS_ASSERT(IS_I2S_STANDARD(hi2s->Init.Standard));
  LS_ASSERT(IS_I2S_DATA_FORMAT(hi2s->Init.DataFormat));
  LS_ASSERT(IS_I2S_MCLK_OUTPUT(hi2s->Init.MCLKOutput));
  LS_ASSERT(IS_I2S_AUDIO_FREQ(hi2s->Init.AudioFreq));
  LS_ASSERT(IS_I2S_CPOL(hi2s->Init.CPOL));

  if (hi2s->State == HAL_I2S_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hi2s->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    HAL_I2S_MspInit(hi2s);
  }

  hi2s->State = HAL_I2S_STATE_BUSY;

   HAL_I2S_MSP_Init(hi2s);
   HAL_I2S_MSP_Busy_Set(hi2s);
	
  /*----------------------- SPIx I2SCFGR & I2SPR Configuration ----------------*/
  /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
  CLEAR_BIT(hi2s->Instance->I2SCFGR, (SPI_I2SCFGR_CHLEN_MASK | SPI_I2SCFGR_DATLEN_MASK | SPI_I2SCFGR_CKPOL_MASK | \
                                      SPI_I2SCFGR_I2SSTD_MASK | SPI_I2SCFGR_PCMSYNC_MASK | SPI_I2SCFGR_I2SCFG_MASK | \
                                      SPI_I2SCFGR_I2SE_MASK | SPI_I2SCFGR_I2SMOD_MASK));
  hi2s->Instance->I2SPR = 0x0002U;

  /*----------------------- I2SPR: I2SDIV and ODD Calculation -----------------*/
  /* If the requested audio frequency is not the default, compute the prescaler */
  if (hi2s->Init.AudioFreq != I2S_AUDIOFREQ_DEFAULT)
  {
    /* Check the frame length (For the Prescaler computing) ********************/
    if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
    {
      /* Packet length is 16 bits */
      packetlength = 16U;
    }
    else
    {
      /* Packet length is 32 bits */
      packetlength = 32U;
    }

    /* I2S standard */
    if (hi2s->Init.Standard <= I2S_STANDARD_LSB)
    {
      /* In I2S standard packet lenght is multiplied by 2 */
      packetlength = packetlength * 2U;
    }

    /* Compute the Real divider depending on the MCLK output state, with a floating point */
    if (hi2s->Init.MCLKOutput == I2S_MCLKOUTPUT_ENABLE)
    {
      /* MCLK output is enabled */
      if (hi2s->Init.DataFormat != I2S_DATAFORMAT_16B)
      {
        tmp = (uint32_t)(((((I2S_CLOCK / (packetlength * 4U)) * 10U) / hi2s->Init.AudioFreq)) + 5U);
      }
      else
      {
        tmp = (uint32_t)(((((I2S_CLOCK / (packetlength * 8U)) * 10U) / hi2s->Init.AudioFreq)) + 5U);
      }
    }
    else
    {
      /* MCLK output is disabled */
      tmp = (uint32_t)(((((I2S_CLOCK / packetlength) * 10U) / hi2s->Init.AudioFreq)) + 5U);
    }

    /* Remove the flatting point */
    tmp = tmp / 10U;

    /* Check the parity of the divider */
    i2sodd = (uint32_t)(tmp & (uint32_t)1U);

    /* Compute the i2sdiv prescaler */
    i2sdiv = (uint32_t)((tmp - i2sodd) / 2U);

    /* Get the Mask for the Odd bit (SPI_I2SPR[8]) register */
    i2sodd = (uint32_t)(i2sodd << 8U);
  }
  else
  {
    /* Set the default values */
    i2sdiv = 2U;
    i2sodd = 0U;
  }

  /* Test if the divider is 1 or 0 or greater than 0xFF */
  if ((i2sdiv < 2U) || (i2sdiv > 0xFFU))
  {
    /* Set the error code and execute error callback*/
    SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_PRESCALER);
    return  HAL_ERROR;
  }

  /*----------------------- SPIx I2SCFGR & I2SPR Configuration ----------------*/

  /* Write to SPIx I2SPR register the computed value */
  hi2s->Instance->I2SPR = (uint32_t)((uint32_t)i2sdiv | (uint32_t)(i2sodd | (uint32_t)hi2s->Init.MCLKOutput));

  /* Clear I2SMOD, I2SE, I2SCFG, PCMSYNC, I2SSTD, CKPOL, DATLEN and CHLEN bits */
  /* And configure the I2S with the I2S_InitStruct values                      */
  MODIFY_REG(hi2s->Instance->I2SCFGR, (SPI_I2SCFGR_CHLEN_MASK | SPI_I2SCFGR_DATLEN_MASK | \
                                       SPI_I2SCFGR_CKPOL_MASK | SPI_I2SCFGR_I2SSTD_MASK | \
                                       SPI_I2SCFGR_PCMSYNC_MASK | SPI_I2SCFGR_I2SCFG_MASK | \
                                       SPI_I2SCFGR_I2SE_MASK  | SPI_I2SCFGR_I2SMOD_MASK), \
             (SPI_I2SCFGR_I2SMOD_MASK | hi2s->Init.Mode | \
              hi2s->Init.Standard | hi2s->Init.DataFormat | \
              hi2s->Init.CPOL));

  hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
  hi2s->State     = HAL_I2S_STATE_READY;

  return HAL_OK;
}

/**
  * @brief DeInitializes the I2S peripheral
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DeInit(I2S_HandleTypeDef *hi2s)
{
  /* Check the I2S handle allocation */
  if (hi2s == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  //LS_ASSERT(IS_I2S_ALL_INSTANCE(hi2s->Instance));

  hi2s->State = HAL_I2S_STATE_BUSY;

  /* Disable the I2S Peripheral Clock */
  __HAL_I2S_DISABLE(hi2s);

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC... */
  HAL_I2S_MSP_DeInit(hi2s);
  HAL_I2S_MSP_Idle_Set(hi2s);
  hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
  hi2s->State     = HAL_I2S_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hi2s);

  return HAL_OK;
}

/**
  * @brief I2S MSP Init
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_MspInit could be implemented in the user file
   */
}

/**
  * @brief I2S MSP DeInit
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_MspDeInit could be implemented in the user file
   */
}

/** @defgroup I2S_Exported_Functions_Group2 IO operation functions
  *  @brief Data transfers functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the I2S data
    transfers.

    (#) There are two modes of transfer:
       (++) Blocking mode : The communication is performed in the polling mode.
            The status of all data processing is returned by the same function
            after finishing transfer.
       (++) No-Blocking mode : The communication is performed using Interrupts
            or DMA. These functions return the status of the transfer startup.
            The end of the data processing will be indicated through the
            dedicated I2S IRQ when using Interrupt mode or the DMA IRQ when
            using DMA mode.

    (#) Blocking mode functions are :
        (++) HAL_I2S_Transmit()
        (++) HAL_I2S_Receive()

    (#) No-Blocking mode functions with Interrupt are :
        (++) HAL_I2S_Transmit_IT()
        (++) HAL_I2S_Receive_IT()

    (#) No-Blocking mode functions with DMA are :
        (++) HAL_I2S_Transmit_DMA()
        (++) HAL_I2S_Receive_DMA()

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_I2S_TxCpltCallback()
        (++) HAL_I2S_RxCpltCallback()
        (++) HAL_I2S_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Transmit an amount of data in blocking mode
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 16-bit pointer to data buffer.
  * @param  Size number of data sample to be sent:
  * @note   When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *         configuration phase, the Size parameter means the number of 16-bit data length
  *         in the transaction and when a 24-bit data frame or a 32-bit data frame is selected
  *         the Size parameter means the number of 16-bit data length.
  * @param  Timeout Timeout duration
  * @note   The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization
  *         between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Transmit(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tmpreg_cfgr;

  if ((pData == NULL) || (Size == 0U))
  {
    return  HAL_ERROR;
  }

  /* Process Locked */
  __HAL_LOCK(hi2s);

  if (hi2s->State != HAL_I2S_STATE_READY)
  {
    __HAL_UNLOCK(hi2s);
    return HAL_BUSY;
  }

  /* Set state and reset error code */
  hi2s->State = HAL_I2S_STATE_BUSY_TX;
  hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
  hi2s->pTxBuffPtr = pData;

  tmpreg_cfgr = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN_MASK | SPI_I2SCFGR_CHLEN_MASK);

  if ((tmpreg_cfgr == I2S_DATAFORMAT_24B) || (tmpreg_cfgr == I2S_DATAFORMAT_32B))
  {
    hi2s->TxXferSize = (Size << 1U);
    hi2s->TxXferCount = (Size << 1U);
  }
  else
  {
    hi2s->TxXferSize = Size;
    hi2s->TxXferCount = Size;
  }

  tmpreg_cfgr = hi2s->Instance->I2SCFGR;

  /* Check if the I2S is already enabled */
  if ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SE_MASK) != SPI_I2SCFGR_I2SE_MASK)
  {
    /* Enable I2S peripheral */
    __HAL_I2S_ENABLE(hi2s);
  }

  /* Wait until TXE flag is set */
  if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_TXE, SET, Timeout) != HAL_OK)
  {
    /* Set the error code */
    SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_TIMEOUT);
    hi2s->State = HAL_I2S_STATE_READY;
    __HAL_UNLOCK(hi2s);
    return HAL_ERROR;
  }

  while (hi2s->TxXferCount > 0U)
  {
    hi2s->Instance->DR = (*hi2s->pTxBuffPtr);
    hi2s->pTxBuffPtr++;
    hi2s->TxXferCount--;

    /* Wait until TXE flag is set */
    if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_TXE, SET, Timeout) != HAL_OK)
    {
      /* Set the error code */
      SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_TIMEOUT);
      hi2s->State = HAL_I2S_STATE_READY;
      __HAL_UNLOCK(hi2s);
      return HAL_ERROR;
    }

    /* Check if an underrun occurs */
    if (__HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_UDR) == SET)
    {
      /* Clear underrun flag */
      __HAL_I2S_CLEAR_UDRFLAG(hi2s);

      /* Set the error code */
      SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_UDR);
    }
  }

  /* Check if Slave mode is selected */
  if (((tmpreg_cfgr & SPI_I2SCFGR_I2SCFG_MASK) == I2S_MODE_SLAVE_TX)
      || ((tmpreg_cfgr & SPI_I2SCFGR_I2SCFG_MASK) == I2S_MODE_SLAVE_RX))
  {
    /* Wait until Busy flag is reset */
    if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_BSY, RESET, Timeout) != HAL_OK)
    {
      /* Set the error code */
      SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_TIMEOUT);
      hi2s->State = HAL_I2S_STATE_READY;
      __HAL_UNLOCK(hi2s);
      return HAL_ERROR;
    }
  }

  hi2s->State = HAL_I2S_STATE_READY;
  __HAL_UNLOCK(hi2s);
  return HAL_OK;
}

/**
  * @brief  Receive an amount of data in blocking mode
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 16-bit pointer to data buffer.
  * @param  Size number of data sample to be sent:
  * @note   When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *         configuration phase, the Size parameter means the number of 16-bit data length
  *         in the transaction and when a 24-bit data frame or a 32-bit data frame is selected
  *         the Size parameter means the number of 16-bit data length.
  * @param  Timeout Timeout duration
  * @note   The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization
  *         between Master and Slave(example: audio streaming).
  * @note   In I2S Master Receiver mode, just after enabling the peripheral the clock will be generate
  *         in continuous way and as the I2S is not disabled at the end of the I2S transaction.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Receive(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tmpreg_cfgr;

  if ((pData == NULL) || (Size == 0U))
  {
    return  HAL_ERROR;
  }

  /* Process Locked */
  __HAL_LOCK(hi2s);

  if (hi2s->State != HAL_I2S_STATE_READY)
  {
    __HAL_UNLOCK(hi2s);
    return HAL_BUSY;
  }

  /* Set state and reset error code */
  hi2s->State = HAL_I2S_STATE_BUSY_RX;
  hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
  hi2s->pRxBuffPtr = pData;

  tmpreg_cfgr = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN_MASK | SPI_I2SCFGR_CHLEN_MASK);

  if ((tmpreg_cfgr == I2S_DATAFORMAT_24B) || (tmpreg_cfgr == I2S_DATAFORMAT_32B))
  {
    hi2s->RxXferSize = (Size << 1U);
    hi2s->RxXferCount = (Size << 1U);
  }
  else
  {
    hi2s->RxXferSize = Size;
    hi2s->RxXferCount = Size;
  }

  /* Check if the I2S is already enabled */
  if ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SE_MASK) != SPI_I2SCFGR_I2SE_MASK)
  {
    /* Enable I2S peripheral */
    __HAL_I2S_ENABLE(hi2s);
  }

  /* Check if Master Receiver mode is selected */
  if ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SCFG_MASK) == I2S_MODE_MASTER_RX)
  {
    /* Clear the Overrun Flag by a read operation on the SPI_DR register followed by a read
    access to the SPI_SR register. */
    __HAL_I2S_CLEAR_OVRFLAG(hi2s);
  }

  /* Receive data */
  while (hi2s->RxXferCount > 0U)
  {
    /* Wait until RXNE flag is set */
    if (I2S_WaitFlagStateUntilTimeout(hi2s, I2S_FLAG_RXNE, SET, Timeout) != HAL_OK)
    {
      /* Set the error code */
      SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_TIMEOUT);
      hi2s->State = HAL_I2S_STATE_READY;
      __HAL_UNLOCK(hi2s);
      return HAL_ERROR;
    }

    (*hi2s->pRxBuffPtr) = (uint16_t)hi2s->Instance->DR;
    hi2s->pRxBuffPtr++;
    hi2s->RxXferCount--;

    /* Check if an overrun occurs */
    if (__HAL_I2S_GET_FLAG(hi2s, I2S_FLAG_OVR) == SET)
    {
      /* Clear overrun flag */
      __HAL_I2S_CLEAR_OVRFLAG(hi2s);

      /* Set the error code */
      SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_OVR);
    }
  }

  hi2s->State = HAL_I2S_STATE_READY;
  __HAL_UNLOCK(hi2s);
  return HAL_OK;
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 16-bit pointer to data buffer.
  * @param  Size number of data sample to be sent:
  * @note   When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *         configuration phase, the Size parameter means the number of 16-bit data length
  *         in the transaction and when a 24-bit data frame or a 32-bit data frame is selected
  *         the Size parameter means the number of 16-bit data length.
  * @note   The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization
  *         between Master and Slave(example: audio streaming).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Transmit_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size)
{
  uint32_t tmpreg_cfgr;

  if ((pData == NULL) || (Size == 0U))
  {
    return  HAL_ERROR;
  }

  /* Process Locked */
  __HAL_LOCK(hi2s);

  if (hi2s->State != HAL_I2S_STATE_READY)
  {
    __HAL_UNLOCK(hi2s);
    return HAL_BUSY;
  }

  /* Set state and reset error code */
  hi2s->State = HAL_I2S_STATE_BUSY_TX;
  hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
  hi2s->pTxBuffPtr = pData;

  tmpreg_cfgr = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN_MASK | SPI_I2SCFGR_CHLEN_MASK);

  if ((tmpreg_cfgr == I2S_DATAFORMAT_24B) || (tmpreg_cfgr == I2S_DATAFORMAT_32B))
  {
    hi2s->TxXferSize = (Size << 1U);
    hi2s->TxXferCount = (Size << 1U);
  }
  else
  {
    hi2s->TxXferSize = Size;
    hi2s->TxXferCount = Size;
  }

  /* Enable TXE and ERR interrupt */
  __HAL_I2S_ENABLE_IT(hi2s, (I2S_IT_TXE | I2S_IT_ERR));

  /* Check if the I2S is already enabled */
  if ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SE_MASK) != SPI_I2SCFGR_I2SE_MASK)
  {
    /* Enable I2S peripheral */
    __HAL_I2S_ENABLE(hi2s);
  }
  __HAL_UNLOCK(hi2s);
	
  /* Transmit data */
  hi2s->Instance->DR = (*hi2s->pTxBuffPtr);
  hi2s->pTxBuffPtr++;
  hi2s->TxXferCount--;

  if (hi2s->TxXferCount == 0U)
  {
    /* Disable TXE and ERR interrupt */
    __HAL_I2S_DISABLE_IT(hi2s, (I2S_IT_TXE | I2S_IT_ERR));

    hi2s->State = HAL_I2S_STATE_READY;
    /* Call user Tx complete callback */
    HAL_I2S_TxCpltCallback(hi2s);
  }	
  return HAL_OK;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 16-bit pointer to the Receive data buffer.
  * @param  Size number of data sample to be sent:
  * @note   When a 16-bit data frame or a 16-bit data frame extended is selected during the I2S
  *         configuration phase, the Size parameter means the number of 16-bit data length
  *         in the transaction and when a 24-bit data frame or a 32-bit data frame is selected
  *         the Size parameter means the number of 16-bit data length.
  * @note   The I2S is kept enabled at the end of transaction to avoid the clock de-synchronization
  *         between Master and Slave(example: audio streaming).
  * @note   It is recommended to use DMA for the I2S receiver to avoid de-synchronization
  * between Master and Slave otherwise the I2S interrupt should be optimized.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Receive_IT(I2S_HandleTypeDef *hi2s, uint16_t *pData, uint16_t Size)
{
  uint32_t tmpreg_cfgr;

  if ((pData == NULL) || (Size == 0U))
  {
    return  HAL_ERROR;
  }

  /* Process Locked */
  __HAL_LOCK(hi2s);

  if (hi2s->State != HAL_I2S_STATE_READY)
  {
    __HAL_UNLOCK(hi2s);
    return HAL_BUSY;
  }

  /* Set state and reset error code */
  hi2s->State = HAL_I2S_STATE_BUSY_RX;
  hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
  hi2s->pRxBuffPtr = pData;

  tmpreg_cfgr = hi2s->Instance->I2SCFGR & (SPI_I2SCFGR_DATLEN_MASK | SPI_I2SCFGR_CHLEN_MASK);

  if ((tmpreg_cfgr == I2S_DATAFORMAT_24B) || (tmpreg_cfgr == I2S_DATAFORMAT_32B))
  {
    hi2s->RxXferSize = (Size << 1U);
    hi2s->RxXferCount = (Size << 1U);
  }
  else
  {
    hi2s->RxXferSize = Size;
    hi2s->RxXferCount = Size;
  }

  /* Enable RXNE and ERR interrupt */
  __HAL_I2S_ENABLE_IT(hi2s, (I2S_IT_RXNE | I2S_IT_ERR));

  /* Check if the I2S is already enabled */
  if ((hi2s->Instance->I2SCFGR & SPI_I2SCFGR_I2SE_MASK) != SPI_I2SCFGR_I2SE_MASK)
  {
    /* Enable I2S peripheral */
    __HAL_I2S_ENABLE(hi2s);
  }

  __HAL_UNLOCK(hi2s);
  return HAL_OK;
}

/**
  * @brief  This function handles I2S interrupt request.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_IRQHandler(I2S_HandleTypeDef *hi2s)
{
  uint32_t itsource = hi2s->Instance->IVS;
  uint32_t itflag   = hi2s->Instance->SR;

  /* I2S in mode Receiver ------------------------------------------------*/
  if ((I2S_CHECK_FLAG(itflag, I2S_FLAG_OVR) == RESET) &&
      (I2S_CHECK_FLAG(itflag, I2S_FLAG_RXNE) != RESET) && (I2S_CHECK_IT_SOURCE(itsource, I2S_IT_RXNE) != RESET))
  {
    I2S_Receive_IT(hi2s);
    return;
  }

  /* I2S in mode Tramitter -----------------------------------------------*/
  if ((I2S_CHECK_FLAG(itflag, I2S_FLAG_TXE) != RESET) && (I2S_CHECK_IT_SOURCE(itsource, I2S_IT_TXE) != RESET))
  {
    I2S_Transmit_IT(hi2s);
    return;
  }

  /* I2S interrupt error -------------------------------------------------*/
  if (I2S_CHECK_IT_SOURCE(itsource, I2S_IT_ERR) != RESET)
  {
    /* I2S Overrun error interrupt occurred ---------------------------------*/
    if (I2S_CHECK_FLAG(itflag, I2S_FLAG_OVR) != RESET)
    {
      /* Disable RXNE and ERR interrupt */
      __HAL_I2S_DISABLE_IT(hi2s, (I2S_IT_RXNE | I2S_IT_ERR));

      /* Set the error code and execute error callback*/
      SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_OVR);
    }

    /* I2S Underrun error interrupt occurred --------------------------------*/
    if (I2S_CHECK_FLAG(itflag, I2S_FLAG_UDR) != RESET)
    {
      /* Disable TXE and ERR interrupt */
      __HAL_I2S_DISABLE_IT(hi2s, (I2S_IT_TXE | I2S_IT_ERR));

      /* Set the error code and execute error callback*/
      SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_UDR);
    }

    /* Set the I2S State ready */
    hi2s->State = HAL_I2S_STATE_READY;

    /* Call user error callback */
    HAL_I2S_ErrorCallback(hi2s);
  }
}

/**
  * @brief  Tx Transfer Half completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_TxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Tx Transfer completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer half completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_RxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_RxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  I2S error callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__weak void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_ErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup I2S_Exported_Functions_Group3 Peripheral State and Errors functions
  *  @brief   Peripheral State functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the I2S state
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL state
  */
HAL_I2S_StateTypeDef HAL_I2S_GetState(I2S_HandleTypeDef *hi2s)
{
  return hi2s->State;
}

/**
  * @brief  Return the I2S error code
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval I2S Error Code
  */
uint32_t HAL_I2S_GetError(I2S_HandleTypeDef *hi2s)
{
  return hi2s->ErrorCode;
}
/**
  * @}
  */

/**
  * @brief  Transmit an amount of data in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
static void I2S_Transmit_IT(I2S_HandleTypeDef *hi2s)
{
  /* Transmit data */
  hi2s->Instance->DR = (*hi2s->pTxBuffPtr);
  hi2s->pTxBuffPtr++;
  hi2s->TxXferCount--;

  if (hi2s->TxXferCount == 0U)
  {
    /* Disable TXE and ERR interrupt */
    __HAL_I2S_DISABLE_IT(hi2s, (I2S_IT_TXE | I2S_IT_ERR));

    hi2s->State = HAL_I2S_STATE_READY;
    /* Call user Tx complete callback */
    HAL_I2S_TxCpltCallback(hi2s);
  }
}

/**
  * @brief  Receive an amount of data in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
static void I2S_Receive_IT(I2S_HandleTypeDef *hi2s)
{
  /* Receive data */
  (*hi2s->pRxBuffPtr) = (uint16_t)hi2s->Instance->DR;
  hi2s->pRxBuffPtr++;
  hi2s->RxXferCount--;

  if (hi2s->RxXferCount == 0U)
  {
    /* Disable RXNE and ERR interrupt */
    __HAL_I2S_DISABLE_IT(hi2s, (I2S_IT_RXNE | I2S_IT_ERR));

    hi2s->State = HAL_I2S_STATE_READY;
    /* Call user Rx complete callback */
    HAL_I2S_RxCpltCallback(hi2s);
  }
}

/**
  * @brief  This function handles I2S Communication Timeout.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  Flag Flag checked
  * @param  State Value of the flag expected
  * @param  Timeout Duration of the timeout
  * @retval HAL status
  */
static HAL_StatusTypeDef I2S_WaitFlagStateUntilTimeout(I2S_HandleTypeDef *hi2s, uint32_t Flag, FlagStatus State,
                                                       uint32_t Timeout)
{
  uint32_t tickstart;

  /* Get tick */
  tickstart = systick_get_value();

  /* Wait until flag is set to status*/
  while (((__HAL_I2S_GET_FLAG(hi2s, Flag)) ? SET : RESET) != State)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((systick_get_value() - tickstart) >= Timeout) || (Timeout == 0U))
      {
        /* Set the I2S State ready */
        hi2s->State = HAL_I2S_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2s);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @}
  */

/**
  * @}
  */
 /* HAL_I2S_MODULE_ENABLED */

/************************ (C) COPYRIGHT linkedsemi *****END OF FILE****/
