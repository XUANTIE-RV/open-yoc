/* Includes ------------------------------------------------------------------*/
#include "field_manipulate.h"
#include "dma.h"
#include "le501x.h"
#include <string.h>
#include "reg_rcc.h"

/**
 * @brief Global definitions for DMA usage
 *        dma_ctrl_base[n] - the control structure for each channel, 
 *              - src_end_pointer
 *              - dst_end_pointer
 *              - channel transmission paramters
 *        dma_callback[n] - completion/error callbacks
 *        Important Notes:
 *        1. Only the DMARAM is available for DMA transfers
 *        2. dma_ctrl_base[n] and memory location for DMA transfers, must located in DMARAM
 *        3. ctrlstruct memory map:
 *           |primary_0|primary_1|...|primary_7|alt_0|alt_1|...|alt_7|
 *           alt_0 is always offset 8*sizeof(DMA_CTRLSTRUCT_TypeDef) from Primary_0
 *        Default, 8 primary + 8 alt is defined, requires total 256 bytes
 *        Tips:
 *        If want to save SRAM usage,
 *        1. If use only Primary for simple DMA transfer, you can define n=channels used.
 *        2. If use primary+alt, even you only 2 channels, you need define n size at lease 8+2=10.
 */
DMA_CTRLSTURCT_TypeDef dma_ctrl_base[16]  __attribute__((aligned(256))) __attribute__((section("DMARAM")));
//HAL_DMA_Callback_TypeDef dma_callback[8];

static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);

/** @addtogroup HAL_Driver
  * @{
  */

/** @defgroup DMA
  * @brief DMA HAL module driver
  * @{
  */
/**
 * @brief DMA controller register reset
 *        Reset the DMA controller registers.
 *        Attention pls, the contrl struct is not affect.
 */
void HAL_DMA_Reset_Reg(void)
{
    WRITE_REG(DMA1->CFG, 0);
    WRITE_REG(DMA1->BSTCLR, 0xFF);
    WRITE_REG(DMA1->REQMSKCLR, 0xFF);
//    WRITE_REG(DMA1->ENCLR, 0xFF);
    WRITE_REG(DMA1->PRIALTCLR, 0xFF);
    WRITE_REG(DMA1->PRIOCLR, 0xFF);
    WRITE_REG(DMA1->ERRCLR, 0x1);
    WRITE_REG(DMA1INT->DONEICF, 0xFF);
    WRITE_REG(DMA1INT->ERRICF, 0x1);
    WRITE_REG(DMA1INT->DONEIEF, 0x00);
    WRITE_REG(DMA1INT->ERRIEF, 0x0);
    WRITE_REG(DMA1CHSEL->CHSEL0, DMA_ALLSEL_NONE);
    WRITE_REG(DMA1CHSEL->CHSEL1, DMA_ALLSEL_NONE);
    return;
}

/**
 * @brief DMA controller low level init
 *        Initialize the control structs
 *        Enable DMA clocks (a sw reset of DMA controller can be assigned)
 *        Reset DMA controller hw registers
 *        Assign the basepointer to 1st location of control struct. (altnate is cacl from base through HW)
 *        Enable DMA error interrupt (channel done interrupt will set by peripheral dma handlers)
 *        Enable DMA controller
 */
void HAL_DMA_LL_Init(void)
{
    // clear control structure and callback in DataRAM */
    memset(dma_ctrl_base, 0x0, sizeof(dma_ctrl_base));
    //memset(dma_callback, 0x0, sizeof(dma_callback));
    RCC->APB2EN |= RCC_DMA1_MASK; // Enable DMA clock
    HAL_DMA_Reset_Reg();                       
    WRITE_REG(DMA1->BPTR, (uint32_t) &dma_ctrl_base[0]);     // set DMA base ptr to dma_ctrl_base[0]
		WRITE_REG(DMA1->CFG, 1);
//    NVIC_SetPriority(DMA_IRQn, 2);
    NVIC_EnableIRQ(DMA_IRQn);
    WRITE_REG(DMA1INT->ERRIEF, DMA_ERRIEF_SET);
    return;
}
/**
  * @brief  Initialize the DMA according to the specified
  *         parameters in the DMA_InitTypeDef and initialize the associated handle.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  *         note: channel is not enable, channel interrupt is not enabled here
  *         note2: memory address specified for srt_ptr/dst_ptr, should be base+size (diff for byte/hw/word)
  *                base + (byte/hw/word)*n_minus_1
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma)
{
    uint8_t chnPos;
    DMA_CTRLSTURCT_TypeDef *ctrlstruct;
 
  	/* Check the DMA handle allocation */
    if (hdma == NULL)
    {
        return HAL_ERROR;
    }

    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;
		
		hdma->Instance = DMA1;
		
    /* get channel control structure pointer */
    if (hdma->Config.primary)
        ctrlstruct = (DMA_CTRLSTURCT_TypeDef *)(DMA1->BPTR) + hdma->Config.chnIndex;
    else
        ctrlstruct = (DMA_CTRLSTURCT_TypeDef *)(DMA1->ALTBPTR) + hdma->Config.chnIndex;
    
    /* set control struture */
		/* src/dst address, transfer cycle numbers (n_minus_1), are set on-fly */
		ctrlstruct->ctrl.s.CYCLE_CTRL = hdma->Config.cycle_ctrl;
		ctrlstruct->ctrl.s.NEXT_USEBURST = hdma->Config.next_useburst;
		ctrlstruct->ctrl.s.DST_INC = hdma->Config.dst_inc;
		ctrlstruct->ctrl.s.SRC_INC = hdma->Config.src_inc;
		ctrlstruct->ctrl.s.SRC_SIZE = hdma->Config.data_size;
		ctrlstruct->ctrl.s.RPOWER = hdma->Config.rpower;
    ctrlstruct->ctrl.s.DST_PROT_CTRL = 0;
		ctrlstruct->ctrl.s.SRC_PROT_CTRL = 0;
				
    /* config registers */
    chnPos = (1<<hdma->Config.chnIndex); // get position in DMA registers.

    if (hdma->Config.primary)
        WRITE_REG(DMA1->PRIALTCLR, chnPos);
    else
        WRITE_REG(DMA1->PRIALTSET, chnPos);
    
    if (hdma->Config.burst)
        WRITE_REG(DMA1->BSTSET, chnPos);
    else
        WRITE_REG(DMA1->BSTCLR, chnPos);
    
    if (hdma->Config.priority)
        WRITE_REG(DMA1->PRIOSET, chnPos);
    else 
        WRITE_REG(DMA1->PRIOCLR, chnPos);
		
		if (hdma->Config.intEn)
			  SET_BIT(DMA1INT->DONEIEF, chnPos);
		else
			  CLEAR_BIT(DMA1INT->DONEICF, chnPos);

    /* set DMA channel mux */
    if (hdma->Config.chnIndex < 4)
        MODIFY_REG(DMA1CHSEL->CHSEL0, (0x3f << (hdma->Config.chnIndex<<3)), (hdma->Config.sigSel) << (hdma->Config.chnIndex<<3));
    else
        MODIFY_REG(DMA1CHSEL->CHSEL1, (0x3f << ((hdma->Config.chnIndex & 0x3)<<3)), (hdma->Config.sigSel) << ((hdma->Config.chnIndex & 0x3)<<3));

    /* Initialize the DMA state*/
    hdma->State = HAL_DMA_STATE_READY;

    return HAL_OK;
}

/**
  * @brief  DeInitialize the DMA peripheral.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma)
{
	DMA_CTRLSTURCT_TypeDef *ctrlstruct;
	/* Check the DMA handle allocation */
	if (hdma == NULL)
	{
			return HAL_ERROR;
	}

	/* get channel control structure pointer */
	if (hdma->Config.primary)
			ctrlstruct = (DMA_CTRLSTURCT_TypeDef *)(DMA1->BPTR) + hdma->Config.chnIndex;
	else
			ctrlstruct = (DMA_CTRLSTURCT_TypeDef *)(DMA1->ALTBPTR) + hdma->Config.chnIndex;

	/* Disable the selected DMA Channelx */
	__HAL_DMA_DISABLE(DMA1, hdma->Config.chnIndex);
  
  /* Reset DMA Channel control register */
  ctrlstruct->ctrl.word  = 0U;
  ctrlstruct->src_end_ptr = 0U;
  ctrlstruct->dst_end_ptr = 0U;

	/* clean callbacks */
	hdma->XferCpltCallback = NULL;
	hdma->XferErrorCallback = NULL;
	
  /* Clear all flags */
  DMA1INT->DONEICF = (1 << (hdma->Config.chnIndex));
	DMA1INT->ERRICF = 1;

  /* Clean all callbacks */
  hdma->XferCpltCallback = NULL;
  hdma->XferErrorCallback = NULL;

  /* Reset the DMA state */
  hdma->State = HAL_DMA_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Start the DMA Transfer with interrupt enabled.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  //assert_param(IS_DMA_BUFFER_SIZE(DataLength));
  
  if(HAL_DMA_STATE_READY == hdma->State)
  {
    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;
    
    /* Configure the source, destination address and the data length & clear flags*/
    DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);
    
    /* Enable the transfer complete interrupt */
    /* Enable the transfer Error interrupt */
//    __HAL_DMA_DISABLE_IT(DMA1INT, hdma->Config.chnIndex);
    __HAL_DMA_ENABLE_IT(DMA1INT, hdma->Config.chnIndex);
    __HAL_DMA_EN_ERR_IT(DMA1INT);
    
    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(DMA1, hdma->Config.chnIndex);
  }
  else
  {      
    /* Process Unlocked */
    __HAL_UNLOCK(hdma); 

    /* Remain BUSY */
    status = HAL_BUSY;
  }    
  return status;
}

/**
  * @brief  Sets the DMA Transfer parameter.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Channel.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
		DMA_CTRLSTURCT_TypeDef *ctrlstruct;

    /* Clear all flags */
    DMA1INT->DONEICF = (1 << hdma->Config.chnIndex);

	/* get channel control structure pointer */
    if (hdma->Config.primary)
        ctrlstruct = (DMA_CTRLSTURCT_TypeDef *)(DMA1->BPTR) + hdma->Config.chnIndex;
    else
        ctrlstruct = (DMA_CTRLSTURCT_TypeDef *)(DMA1->ALTBPTR) + hdma->Config.chnIndex;

		/* configure number of DMA xfers */
		ctrlstruct->ctrl.s.N_MINUS_1 = DataLength - 1;
		
    /* configure destiniation address */    
		if (hdma->Config.dst_inc == DMA_INC_NONE)
			ctrlstruct->dst_end_ptr = (uint32_t *)DstAddress;
		else
			ctrlstruct->dst_end_ptr = (uint32_t *)(DstAddress + ((DataLength -1) << hdma->Config.data_size));

		if (hdma->Config.src_inc == DMA_INC_NONE)
			ctrlstruct->src_end_ptr = (uint32_t *)SrcAddress;
		else
			ctrlstruct->src_end_ptr = (uint32_t *)(SrcAddress + ((DataLength -1) << hdma->Config.data_size));
		
		ctrlstruct->ctrl.s.CYCLE_CTRL = hdma->Config.cycle_ctrl;
		
    return;
}

/**
  * @brief  Handles DMA interrupt request.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.  
  *         TODO: check in ping-pong, scatter/gather mode, should we disable interrupt?
  *         TODO: add HAL_
  * @retval None
  */

void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
	uint32_t flag_it = DMA1INT->DONERIF;
		
	/* Tranfer complete interrupt management */
	if (((1 << hdma->Config.chnIndex) & flag_it) != 0) 
	{
		DMA1INT->DONEICF = 1<<hdma->Config.chnIndex;
		
		if (hdma->XferCpltCallback != NULL)
			hdma->XferCpltCallback(hdma);
	}
	hdma->State = HAL_DMA_STATE_READY;
}

#if 0
void HAL_DMA_IRQHandler(void)
{
    uint8_t i;
    uint8_t flag_it = DMA1INT->DONERIF;
    uint8_t error_flag_it = DMA1INT->ERRRIF;

    for (i = 0; i < DMA_CH_NUMBER; i++)
    {
      if (flag_it & (1<<i))
      {
            if (dma_callback[i].XferCpltCallback != NULL)
                dma_callback[i].XferCpltCallback(dma_callback[i].XferCplt_arg);

            __HAL_DMA_CLEAR_FLAG(DMA1INT, i);
      }
    }

    if (error_flag_it)
    {
        __HAL_DMA_CLEAR_ERR_FLAG(DMA1INT);
        for (i= 0; i < DMA_CH_NUMBER; i++)
        {
          if (DMA1->ENSET & (1<<i) && (dma_callback[i].XferErrorCallback != NULL))
            dma_callback[i].XferErrorCallback(dma_callback[i].XferError_arg);
        }
    }

    return;
}
#endif

#if 0
/**
  * @brief  Start the DMA Transfer.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  if(HAL_DMA_STATE_READY == hdma->State)
  {
    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;
            
    /* Configure the source, destination address and the data length & clear flags*/
    DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);
  }
  else
  {
		status = HAL_BUSY;
  }  
  return status;
}

/**
  * @brief  Aborts the DMA Transfer in Interrupt mode.
  * @param  hdma  : pointer to a DMA_HandleTypeDef structure that contains
  *                 the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma)
{  
  HAL_StatusTypeDef status = HAL_OK;
  
  if(HAL_DMA_STATE_BUSY != hdma->State)
  {
    /* no transfer ongoing */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
        
    status = HAL_ERROR;
  }
  else
  { 
    /* Disable DMA IT */
    __HAL_DMA_DISABLE_IT(DMA1, hdma->ChannelIndex);

    /* Disable the channel */
    __HAL_DMA_DISABLE(DMA1, hdma->ChannelIndex);

    /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG(DMA1, hdma->ChannelIndex);

    /* Change the DMA state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Call User Abort callback */
    if(hdma->XferAbortCallback != NULL)
    {
      hdma->XferAbortCallback(hdma);
    } 
  }
  return status;
}





/** @addtogroup DMA_Private_Functions
  * @{
  */

/**
  * @brief  Sets the DMA Transfer parameter.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Channel.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
    /* Clear all flags */
    DMA1INT->ICF = (1 << hdma->ChannelIndex);

    /* Configure DMA Channel data length */
    hdma->Instance->ctrl.N_MINUS_1 = DataLength - 1;

    /* configure destiniation address */
    hdma->Instance->dst_end_ptr = DstAddress;
    hdma->Instance->src_end_ptr = SrcAddress;

    return;
}

#endif
