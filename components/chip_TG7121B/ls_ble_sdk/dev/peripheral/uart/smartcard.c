/**
  ******************************************************************************
  * @file    le501x_hal_smartcard.c
  * @author  MCD Application Team
  * @brief   SMARTCARD HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the SMARTCARD peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State and Error functions
  *
  @verbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "lsuart.h"
#include "uart_msp.h" 
#include "log.h"
#include "lssmartcard.h"
#include "systick.h"
#include "ls_dbg.h"
#include "field_manipulate.h"
/** @addtogroup le501x_HAL_Driver
  * @{
  */

/** @defgroup SMARTCARD SMARTCARD
  * @brief HAL SMARTCARD module driver
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup SMARTCARD_Private_Constants
  * @{
  */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup SMARTCARD_Private_Functions
  * @{
  */
static void SMARTCARD_SetConfig (SMARTCARD_HandleTypeDef *hsc);
static HAL_StatusTypeDef SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsc);
static HAL_StatusTypeDef SMARTCARD_EndTransmit_IT(SMARTCARD_HandleTypeDef *hsc);
static HAL_StatusTypeDef SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsc);

// static HAL_StatusTypeDef SMARTCARD_WaitOnFlagUntilTimeout(SMARTCARD_HandleTypeDef *hsc, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout);
/**
  * @}
  */

/**
  * @brief  Initializes the SmartCard mode according to the specified
  *         parameters in the SMARTCARD_InitTypeDef and create the associated handle.
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Init(SMARTCARD_HandleTypeDef *hsc)
{
  /* Check the SMARTCARD handle allocation */
  if(hsc == NULL)
  {
    return HAL_INVALID_OPERATION;
  }

  /* Check the parameters */
 // LS_ASSERT(IS_SMARTCARD_UARTX(hsc->UARTX));

  if(hsc->gState == HAL_SMARTCARD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hsc->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK */
    HAL_SMARTCARD_MspInit(hsc);
  }

  hsc->gState = HAL_SMARTCARD_STATE_BUSY;

    HAL_SMARTCARD_MSP_Init(hsc);
    HAL_SMARTCARD_MSP_Busy_Set();
  /* Set the Prescaler */
  MODIFY_REG(hsc->UARTX->CR, UART_CR_PSC_MASK, hsc->Init.Prescaler);

  /* Set the Guard Time */
  MODIFY_REG(hsc->UARTX->CR, UART_CR_GT_MASK, ((hsc->Init.GuardTime)<<UART_CR_GT_POS));

  /* Set the Smartcard Communication parameters */
  SMARTCARD_SetConfig(hsc);

  /* In SmartCard mode, the following bits must be kept cleared:
  - LINEN ��HDSEL and IREN bits in the MCR register.*/
  CLEAR_BIT(hsc->UARTX->MCR, (UART_MCR_LINEN_MASK | UART_MCR_IREN_MASK | UART_MCR_HDSEL_MASK));

  /* Enable the SC mode by setting the SCEN bit in the CR3 register */
  hsc->UARTX->MCR |= (UART_MCR_SCEN_MASK);

  /* Initialize the SMARTCARD state*/
  hsc->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
  hsc->gState= HAL_SMARTCARD_STATE_READY;
  hsc->RxState= HAL_SMARTCARD_STATE_READY;

  return HAL_OK;
}

/**
  * @brief DeInitializes the USART SmartCard peripheral
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_DeInit(SMARTCARD_HandleTypeDef *hsc)
{
  /* Check the SMARTCARD handle allocation */
  if(hsc == NULL)
  {
    return HAL_INVALID_OPERATION;
  }

  /* Check the parameters */
//  LS_ASSERT(IS_SMARTCARD_UARTX(hsc->UARTX));

  hsc->gState = HAL_SMARTCARD_STATE_BUSY;

  /* DeInit the low level hardware */
  HAL_SMARTCARD_MSP_DeInit(hsc);
  HAL_SMARTCARD_MSP_Idle_Set();
  hsc->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
  hsc->gState = HAL_SMARTCARD_STATE_RESET;
  hsc->RxState = HAL_SMARTCARD_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hsc);

  return HAL_OK;
}

/**
  * @brief  SMARTCARD MSP Init
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_MspInit(SMARTCARD_HandleTypeDef *hsc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_MspInit can be implemented in the user file
   */
}

/**
  * @brief SMARTCARD MSP DeInit
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_MspDeInit(SMARTCARD_HandleTypeDef *hsc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_MspDeInit can be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup SMARTCARD_Exported_Functions_Group2 IO operation functions
  * @brief    SMARTCARD Transmit and Receive functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
 [..]
   This subsection provides a set of functions allowing to manage the SMARTCARD data transfers.

 [..]
    (#) Smartcard is a single wire half duplex communication protocol.
    The Smartcard interface is designed to support asynchronous protocol Smartcards as
    defined in the ISO 7816-3 standard.
    (#) The USART should be configured as:
       (++) 8 bits plus parity: where M=1 and PCE=1 in the USART_CR1 register
       (++) 1.5 stop bits when transmitting and receiving: where STOP=11 in the USART_CR2 register.

    (#) There are two modes of transfer:
       (++) Blocking mode: The communication is performed in polling mode.
            The HAL status of all data processing is returned by the same function
            after finishing transfer.
       (++) Non Blocking mode: The communication is performed using Interrupts
           or DMA, These APIs return the HAL status.
           The end of the data processing will be indicated through the
           dedicated SMARTCARD IRQ when using Interrupt mode or the DMA IRQ when
           using DMA mode.
           The HAL_SMARTCARD_TxCpltCallback(), HAL_SMARTCARD_RxCpltCallback() user callbacks
           will be executed respectively at the end of the Transmit or Receive process
           The HAL_SMARTCARD_ErrorCallback() user callback will be executed when a communication error is detected

    (#) Blocking mode APIs are :
        (++) HAL_SMARTCARD_Transmit()
        (++) HAL_SMARTCARD_Receive()

    (#) Non Blocking mode APIs with Interrupt are :
        (++) HAL_SMARTCARD_Transmit_IT()
        (++) HAL_SMARTCARD_Receive_IT()
        (++) HAL_SMARTCARD_IRQHandler()

    (#) Non Blocking mode functions with DMA are :
        (++) HAL_SMARTCARD_Transmit_DMA()
        (++) HAL_SMARTCARD_Receive_DMA()

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_SMARTCARD_TxCpltCallback()
        (++) HAL_SMARTCARD_RxCpltCallback()
        (++) HAL_SMARTCARD_ErrorCallback()

    (#) Non-Blocking mode transfers could be aborted using Abort API's :
        (+) HAL_SMARTCARD_Abort()
        (+) HAL_SMARTCARD_AbortTransmit()
        (+) HAL_SMARTCARD_AbortReceive()
        (+) HAL_SMARTCARD_Abort_IT()
        (+) HAL_SMARTCARD_AbortTransmit_IT()
        (+) HAL_SMARTCARD_AbortReceive_IT()

    (#) For Abort services based on interrupts (HAL_SMARTCARD_Abortxxx_IT), a set of Abort Complete Callbacks are provided:
        (+) HAL_SMARTCARD_AbortCpltCallback()
        (+) HAL_SMARTCARD_AbortTransmitCpltCallback()
        (+) HAL_SMARTCARD_AbortReceiveCpltCallback()

    (#) In Non-Blocking mode transfers, possible errors are split into 2 categories.
        Errors are handled as follows :
       (+) Error is considered as Recoverable and non blocking : Transfer could go till end, but error severity is
           to be evaluated by user : this concerns Frame Error, Parity Error or Noise Error in Interrupt mode reception .
           Received character is then retrieved and stored in Rx buffer, Error code is set to allow user to identify error type,
           and HAL_SMARTCARD_ErrorCallback() user callback is executed. Transfer is kept ongoing on SMARTCARD side.
           If user wants to abort it, Abort services should be called by user.
       (+) Error is considered as Blocking : Transfer could not be completed properly and is aborted.
           This concerns Frame Error in Interrupt mode tranmission, Overrun Error in Interrupt mode reception and all errors in DMA mode.
           Error code is set to allow user to identify error type, and HAL_SMARTCARD_ErrorCallback() user callback is executed.

@endverbatim
  * @{
  */

/**
  * @brief Send an amount of data in blocking mode
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @param  pData  Pointer to data buffer
  * @param  Size   Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Transmit(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
//   uint32_t tickstart = 0U;

  if(hsc->gState == HAL_SMARTCARD_STATE_READY)
  {
    if((pData == NULL) || (Size == 0U))
    {
      return  HAL_INVALID_OPERATION;
    }

    /* Process Locked */
    __HAL_LOCK(hsc);

    hsc->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsc->gState = HAL_SMARTCARD_STATE_BUSY_TX;

	 /* Enable the SMARTCARD TX */
    MODIFY_REG(hsc->UARTX->LCR, UART_LCR_RXEN_MASK, SMARTCARD_MODE_TX); 
		
    /* Init tickstart for timeout managment */
    // tickstart = HAL_GetTick();

    hsc->TxXferSize = Size;
    hsc->TxXferCount = Size;
    while(hsc->TxXferCount > 0U)
    {
      hsc->TxXferCount--;
    //   if(SMARTCARD_WaitOnFlagUntilTimeout(hsc, SMARTCARD_FLAG_TBEM, RESET, tickstart, Timeout) != HAL_OK)
    //   {
    //     return HAL_TIMEOUT;
    //   }
      hsc->UARTX->TBR = (*pData++ & 0xFF);
    }

    // if(SMARTCARD_WaitOnFlagUntilTimeout(hsc, SMARTCARD_FLAG_TEMT, RESET, tickstart, Timeout) != HAL_OK)
    // {
    //   return HAL_TIMEOUT;
    // }
	 /* Enable the SMARTCARD RX */
    MODIFY_REG(hsc->UARTX->LCR, UART_LCR_RXEN_MASK, SMARTCARD_MODE_RX); 
		
	/* At end of Tx process, restore hsc->gState to Ready */
    hsc->gState = HAL_SMARTCARD_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hsc);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in blocking mode
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @param  pData  Pointer to data buffer
  * @param  Size   Amount of data to be received
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Receive(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
//   uint32_t tickstart = 0U;

  if(hsc->RxState == HAL_SMARTCARD_STATE_READY)
  {
    if((pData == NULL) || (Size == 0U))
    {
      return  HAL_INVALID_OPERATION;
    }

    /* Process Locked */
    __HAL_LOCK(hsc);

    hsc->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsc->RxState = HAL_SMARTCARD_STATE_BUSY_RX;

	 /* Enable the SMARTCARD RX */
    MODIFY_REG(hsc->UARTX->LCR, UART_LCR_RXEN_MASK, UART_LCR_RXEN_MASK);
		
    /* Init tickstart for timeout managment */
    // tickstart = HAL_GetTick();

    hsc->RxXferSize = Size;
    hsc->RxXferCount = Size;

    /* Check the remain data to be received */
    while(hsc->RxXferCount > 0U)
    {
    //   if(SMARTCARD_WaitOnFlagUntilTimeout(hsc, SMARTCARD_FLAG_DR, RESET, tickstart, Timeout) != HAL_OK)
    //   {
    //     return HAL_TIMEOUT;
    //   }
      *pData++ = (uint8_t)(hsc->UARTX->RBR & (uint8_t)0xFF);
      hsc->RxXferCount--;
    }

    /* At end of Rx process, restore hsc->RxState to Ready */
    hsc->RxState = HAL_SMARTCARD_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hsc);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Send an amount of data in non blocking mode
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @param  pData  Pointer to data buffer
  * @param  Size   Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size)
{
  /* Check that a Tx process is not already ongoing */
  if(hsc->gState == HAL_SMARTCARD_STATE_READY)
  {
    if((pData == NULL) || (Size == 0U))
    {
      return HAL_INVALID_OPERATION;
    }

    /* Process Locked */
    __HAL_LOCK(hsc);

    hsc->pTxBuffPtr = pData;
    hsc->TxXferSize = Size;
    hsc->TxXferCount = Size;

    hsc->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsc->gState = HAL_SMARTCARD_STATE_BUSY_TX;

		/* Enable the SMARTCARD TX */
    MODIFY_REG(hsc->UARTX->LCR, UART_LCR_RXEN_MASK, SMARTCARD_MODE_TX); 
		
    /* Process Unlocked */
    __HAL_UNLOCK(hsc);

	MODIFY_REG(hsc->UARTX->FCR,UART_FCR_TXTL_MASK,SMARTCARD_FIFO_TL_0);

    /* Enable the SMARTCARD Transmit data register empty Interrupt */
        SET_BIT(hsc->UARTX->ICR, SMARTCARD_IT_TXS); // threshold empty 
        SET_BIT(hsc->UARTX->ICR, SMARTCARD_IT_TC); // transmission complete, TODO: should not set here!
        SET_BIT(hsc->UARTX->IER, SMARTCARD_IT_TXS);

        hsc->TxXferCount--; 
        hsc->UARTX->TBR = (*hsc->pTxBuffPtr++ & (uint8_t)0xFF);
        if(hsc->TxXferCount == 0)
        {
            SET_BIT(hsc->UARTX->IER,UART_IT_TC);
        }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in non blocking mode
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @param  pData  Pointer to data buffer
  * @param  Size   Amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsc, uint8_t *pData, uint16_t Size)
{
  /* Check that a Rx process is not already ongoing */
  if(hsc->RxState == HAL_SMARTCARD_STATE_READY)
  {
    if((pData == NULL) || (Size == 0U))
    {
      return HAL_INVALID_OPERATION;
    }

    /* Process Locked */
    __HAL_LOCK(hsc);

    hsc->pRxBuffPtr = pData;
    hsc->RxXferSize = Size;
    hsc->RxXferCount = Size;

    hsc->ErrorCode = HAL_SMARTCARD_ERROR_NONE;
    hsc->RxState = HAL_SMARTCARD_STATE_BUSY_RX;

	 /* Enable the SMARTCARD RX */
    MODIFY_REG(hsc->UARTX->LCR, UART_LCR_RXEN_MASK, SMARTCARD_MODE_RX); 
		
    /* Process Unlocked */
    __HAL_UNLOCK(hsc);

		if(hsc->RxXferCount<8)
		{
				MODIFY_REG(hsc->UARTX->FCR,UART_FCR_RXTL_MASK,SMARTCARD_FIFO_RL_1);
		}
		else
		{
				MODIFY_REG(hsc->UARTX->FCR,UART_FCR_RXTL_MASK,SMARTCARD_FIFO_RL_8);
		}	
		
    /* Enable the SMARTCARD Data Register not empty Interrupts */
    SET_BIT(hsc->UARTX->ICR, SMARTCARD_IT_RXRD); 
    SET_BIT(hsc->UARTX->IER, SMARTCARD_IT_RXRD);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief This function handles SMARTCARD interrupt request.
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval None
  */
void HAL_SMARTCARD_IRQHandler(SMARTCARD_HandleTypeDef *hsc)
{
	uint32_t srflags   = hsc->UARTX->SR;
  uint32_t ierflags  = hsc->UARTX->IVS;

	
	/* SMARTCARD frame error interrupt occurred ----------------------------*/
	if((srflags & SMARTCARD_FLAG_FE) != RESET)
	{
		hsc->ErrorCode |= HAL_SMARTCARD_ERROR_FE;
	}

	/* SMARTCARD PE interrupt occurred ----------------------------*/
	if((srflags & SMARTCARD_FLAG_PE) != RESET)
	{
		hsc->ErrorCode |= HAL_SMARTCARD_ERROR_FE;
	}

	/* SMARTCARD Over-Run interrupt occurred -------------------------------*/
	if((srflags & SMARTCARD_FLAG_ORE) != RESET)
	{
		hsc->ErrorCode |= HAL_SMARTCARD_ERROR_ORE;
	}

	/* SMARTCARD in mode Receiver -------------------------------------------------*/
	if(((srflags & SMARTCARD_FLAG_RXNE) != RESET) && ((ierflags & UART_IT_RXRD) != RESET))
	{
		SMARTCARD_Receive_IT(hsc);
	}
  /* SMARTCARD in mode Transmitter ------------------------------------------*/
  if(((srflags & SMARTCARD_FLAG_TBEM) != RESET) && ((ierflags & SMARTCARD_IT_TXS) != RESET))
  {
    SMARTCARD_Transmit_IT(hsc);
    SET_BIT(hsc->UARTX->ICR, UART_TXS_MASK);
  }

  /* SMARTCARD in mode Transmitter (transmission end) -----------------------*/
  if(((srflags & SMARTCARD_FLAG_TEMT) != RESET) && ((ierflags & SMARTCARD_IT_TC) != RESET))
  {

    SMARTCARD_EndTransmit_IT(hsc);
		SET_BIT(hsc->UARTX->ICR,UART_TC_MASK);
  }
}

/**
  * @brief Tx Transfer completed callbacks
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_TxCpltCallback(SMARTCARD_HandleTypeDef *hsc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_TxCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief Rx Transfer completed callback
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef *hsc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_RxCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief SMARTCARD error callback
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval None
  */
__weak void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef *hsc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SMARTCARD_ErrorCallback can be implemented in the user file.
   */
}


/**
  * @}
  */

/** @defgroup SMARTCARD_Exported_Functions_Group3 Peripheral State and Errors functions
  *  @brief   SMARTCARD State and Errors functions
  *
@verbatim
 ===============================================================================
                ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the SmartCard.
     (+) HAL_SMARTCARD_GetState() API can be helpful to check in run-time the state of the SmartCard peripheral.
     (+) HAL_SMARTCARD_GetError() check in run-time errors that could be occurred during communication.
@endverbatim
  * @{
  */

/**
  * @brief Return the SMARTCARD handle state
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval HAL state
  */
HAL_SMARTCARD_StateTypeDef HAL_SMARTCARD_GetState(SMARTCARD_HandleTypeDef *hsc)
{
  uint32_t temp1= 0x00U, temp2 = 0x00U;
  temp1 = hsc->gState;
  temp2 = hsc->RxState;

  return (HAL_SMARTCARD_StateTypeDef)(temp1 | temp2);
}

/**
  * @brief  Return the SMARTCARD error code
  * @param  hsc  Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *              the configuration information for the specified SMARTCARD.
  * @retval SMARTCARD Error Code
  */
uint32_t HAL_SMARTCARD_GetError(SMARTCARD_HandleTypeDef *hsc)
{
  return hsc->ErrorCode;
}

/**
  * @}
  */

/**
  * @brief  This function handles SMARTCARD Communication Timeout.
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @param  Flag   Specifies the SMARTCARD flag to check.
  * @param  Status The new Flag status (SET or RESET).
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
// static HAL_StatusTypeDef SMARTCARD_WaitOnFlagUntilTimeout(SMARTCARD_HandleTypeDef *hsc, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout)
// {
//   /* Wait until flag is set */
//   while((__HAL_SMARTCARD_GET_FLAG(hsc, Flag) ? SET : RESET) == Status)
//   {
//     /* Check for the Timeout */
//     if(Timeout != HAL_MAX_DELAY)
//     {
//       if((Timeout == 0U)||((HAL_GetTick() - Tickstart ) > Timeout))
//       {
//         /* Disable TXE and RXNE interrupts for the interrupt process */
//         SET_BIT(hsc->UARTX->IDR, SMARTCARD_IT_RXRD | SMARTCARD_IT_TXS);
//         SET_BIT(hsc->UARTX->ICR, SMARTCARD_IT_RXRD | SMARTCARD_IT_TXS);

//         hsc->gState= HAL_SMARTCARD_STATE_READY;
//         hsc->RxState= HAL_SMARTCARD_STATE_READY;

//         /* Process Unlocked */
//         __HAL_UNLOCK(hsc);

//         return HAL_TIMEOUT;
//       }
//     }
//   }
//   return HAL_OK;
// }

/**
  * @brief Send an amount of data in non blocking mode
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMARTCARD_Transmit_IT(SMARTCARD_HandleTypeDef *hsc)
{

  /* Check that a Tx process is ongoing */
  if(hsc->gState == HAL_SMARTCARD_STATE_BUSY_TX)
  {
    hsc->UARTX->TBR = (*hsc->pTxBuffPtr++ & 0xFF);

    if(--hsc->TxXferCount == 0U)
    {
      /* Disable the SMARTCARD Transmit data register empty Interrupt */
      SET_BIT(hsc->UARTX->IDR, SMARTCARD_IT_TXS);
	  SET_BIT(hsc->UARTX->ICR, SMARTCARD_IT_TXS);
      /* Enable the SMARTCARD Transmit Complete Interrupt */
      SET_BIT(hsc->UARTX->IER, SMARTCARD_IT_TC);
    }
		if(!READ_BIT(hsc->UARTX->FCR, UART_FCR_TXFL_MASK))
		{
			if (hsc->TxXferCount == 0U)
			{
					SET_BIT(hsc->UARTX->IDR,SMARTCARD_IT_TXS);
					SET_BIT(hsc->UARTX->IER,SMARTCARD_IT_TC);
			}
			else
			{
				while(READ_BIT(hsc->UARTX->SR,  UART_SR_TFNF_MASK))
				{
					if (hsc->TxXferCount == 0U)
					{
							break;
					}
					hsc->TxXferCount--;
					hsc->UARTX->TBR = (*hsc->pTxBuffPtr++ & (uint8_t)0xFF);
				}
			}
		}
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Wraps up transmission in non blocking mode.
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for the specified SMARTCARD module.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMARTCARD_EndTransmit_IT(SMARTCARD_HandleTypeDef *hsc)
{
  /* Disable the SMARTCARD Transmit Complete Interrupt */
	SET_BIT(hsc->UARTX->IDR, SMARTCARD_IT_TC);
	SET_BIT(hsc->UARTX->ICR, SMARTCARD_IT_TC);

  /* Tx process is ended, restore hsc->gState to Ready */
  hsc->gState = HAL_SMARTCARD_STATE_READY;

  /* Call legacy weak Tx complete callback */
  HAL_SMARTCARD_TxCpltCallback(hsc);

  return HAL_OK;
}

/**
  * @brief Receive an amount of data in non blocking mode
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMARTCARD_Receive_IT(SMARTCARD_HandleTypeDef *hsc)
{
	uint8_t fifo_level;
	
  /* Check that a Rx process is ongoing */
  if(hsc->RxState == HAL_SMARTCARD_STATE_BUSY_RX)
  {
		fifo_level = REG_FIELD_RD(hsc->UARTX->FCR, UART_FCR_RXFL);
		
		for (;fifo_level>0;fifo_level--)
		{
			*hsc->pRxBuffPtr++ = (uint8_t)(hsc->UARTX->RBR & (uint8_t)0x00FF);
			if(--hsc->RxXferCount == 0U)
			{
				SET_BIT(hsc->UARTX->IDR, SMARTCARD_IT_RXRD);
				SET_BIT(hsc->UARTX->ICR, SMARTCARD_IT_RXRD);


				/* Rx process is completed, restore hsc->RxState to Ready */
				hsc->RxState = HAL_SMARTCARD_STATE_READY;

				/* Call legacy weak Rx complete callback */
				HAL_SMARTCARD_RxCpltCallback(hsc);

				return HAL_OK;
			}
		
			if(hsc->RxXferCount>0x0E)
			{
				MODIFY_REG(hsc->UARTX->FCR,UART_FCR_RXTL_MASK,SMARTCARD_FIFO_RL_14);
			}
			else if(hsc->RxXferCount>0x08) 
			{
				MODIFY_REG(hsc->UARTX->FCR,UART_FCR_RXTL_MASK,SMARTCARD_FIFO_RL_8);				
			}
			else if(hsc->RxXferCount>0x04) 
			{
				MODIFY_REG(hsc->UARTX->FCR,UART_FCR_RXTL_MASK,SMARTCARD_FIFO_RL_4);				
			}
			else
			{
				MODIFY_REG(hsc->UARTX->FCR,UART_FCR_RXTL_MASK,SMARTCARD_FIFO_RL_1);
			}
		}

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


/**
  * @brief Configure the SMARTCARD peripheral
  * @param  hsc    Pointer to a SMARTCARD_HandleTypeDef structure that contains
  *                the configuration information for SMARTCARD module.
  * @retval None
  */
static void SMARTCARD_SetConfig(SMARTCARD_HandleTypeDef *hsc)
{
 // uint32_t pclk;

  /* Check the parameters */
//  LS_ASSERT(IS_SMARTCARD_UARTX(hsc->UARTX));
  LS_ASSERT(IS_SMARTCARD_BAUDRATE(hsc->Init.BaudRate));
  LS_ASSERT(IS_SMARTCARD_WORD_LENGTH(hsc->Init.WordLength));
  LS_ASSERT(IS_SMARTCARD_STOPBITS(hsc->Init.StopBits));
  LS_ASSERT(IS_SMARTCARD_PARITY(hsc->Init.Parity));
  LS_ASSERT(IS_SMARTCARD_MODE(hsc->Init.Mode));
  LS_ASSERT(IS_SMARTCARD_NACK_STATE(hsc->Init.NACKState));

  /*-------------------------- USART BRR Configuration -----------------------*/
  //pclk = HAL_RCC_GetHCLKFreq();
  SET_BIT(hsc->UARTX->LCR,UART_LCR_BRWEN_MASK);	
  //hsc->UARTX->BRR = SMARTCARD_BRR(pclk, hsc->Init.BaudRate);
	hsc->UARTX->BRR = SMARTCARD_BUADRATE_ENUM_GEN(hsc->Init.BaudRate);
	CLEAR_BIT(hsc->UARTX->LCR,UART_LCR_BRWEN_MASK);
	
	/*-------------------------- USART LCR Configuration -----------------------*/
  /*  Configure the SMARTCARD STOP[2],Word Length, Parity and mode:
		 Set STOP[2] bits according to hsc->Init.StopBits value
     Set the M bits according to hsc->Init.WordLength value
     Set PCE and PS bits according to hsc->Init.Parity value
     Set TE and RE bits according to hsc->Init.Mode value */
	CLEAR_BIT(hsc->UARTX->LCR, (UART_LCR_STOP_MASK | UART_LCR_DLS_MASK | UART_LCR_PE_MASK | UART_LCR_PS_MASK | UART_LCR_RXEN_MASK));
  SET_BIT(hsc->UARTX->LCR, (uint32_t)hsc->Init.StopBits | hsc->Init.WordLength | hsc->Init.Parity | hsc->Init.Mode);

  /*---------------------------- USART MCR Configuration ---------------------*/
  /* Configure the SMARTCARD Clock -----------------------*/
  SET_BIT(hsc->UARTX->MCR, (uint32_t)UART_MCR_CLKEN_MASK);
  /* Configure the Smartcard NACK state and RETRYTIME*/
  MODIFY_REG(hsc->UARTX->MCR, UART_MCR_SCNACK_MASK | UART_MCR_SCCNT_MASK, hsc->Init.Retry | hsc->Init.NACKState);

    /* ---------------------- UART Fifo configuration ------------------------ */
    /* by default, even use DMA transfer, it's better to enable FIFOEN with 1 byte */
    SET_BIT(hsc->UARTX->FCR, UART_FCR_FIFOEN_MASK); // enable UART Fifo
    SET_BIT(hsc->UARTX->FCR, UART_FCR_RFRST_MASK | UART_FCR_TFRST_MASK);  // reset UART RX/TX Fifo
    MODIFY_REG(hsc->UARTX->FCR, UART_FCR_RXFL_MASK, 0);
    MODIFY_REG(hsc->UARTX->FCR, UART_FCR_TXFL_MASK, 0);
		
  /*-------------------------- USART IER Configuration -----------------------*/
  /* Clear INT bits */
	SET_BIT(hsc->UARTX->IDR, SMARTCARD_IT_MASK);

}

/**
  * @}
  */

 /* HAL_SMARTCARD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Linkedsemi *****END OF FILE****/
