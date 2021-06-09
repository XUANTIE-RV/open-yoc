/**
  ******************************************************************************
  * @file    le501x_hal_dmic.c
  * @author  Linkedsemi Application Team
  * @brief   DMIC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the DMIC.
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State and Errors functions
  @verbatim
  ******************************************************************************
  */
#include <stddef.h>
#include "dmic.h"
#include "reg_rcc.h"
#include "field_manipulate.h"
//#ifdef HAL_DMIC_MODULE_ENABLED

volatile DMIC_STATE_TypeDef DmicState __attribute__((section("DMARAM")));

#ifdef DMIC_STEREO
volatile DMIC_STATE_TypeDef DmicState1 __attribute__((section("DMARAM")));
#endif

/* ------------------- Private const/var/function Group -------------------*/
/* Private variables ------------------------------------------------------*/
uint32_t fir_coef_16khz[64] = {
    0x0012fffd, 0x0013fffd, 0x00010003, 0xfff70015,
    0x00040013, 0x001dfff9, 0x0010ffea, 0xffe20005,
    0xffd30024, 0x00040000, 0x0025ffb5, 0xffdaffae,
    0xff6a0000, 0xff7d0018, 0xfffcff8f, 0xfff8feff,
    0xff18ff45, 0xfe76fff7, 0xff13ffb9, 0xfff9fe6f,
    0xff59fdd7, 0xfd8ffefa, 0xfd30000d, 0xff1bfed1,
    0x0050fc6c, 0xfe0ffc93, 0xfad9ffb4, 0xfc1200f5,
    0x017cfccc, 0x02d1f7f9, 0xf932fbbd, 0xe9af09aa,
    0x61fe16e7, 0xe9af16e7, 0xf93209aa, 0x02d1fbbd,
    0x017cf7f9, 0xfc12fccc, 0xfad900f5, 0xfe0fffb4,
    0x0050fc93, 0xff1bfc6c, 0xfd30fed1, 0xfd8f000d,
    0xff59fefa, 0xfff9fdd7, 0xff13fe6f, 0xfe76ffb9,
    0xff18fff7, 0xfff8ff45, 0xfffcfeff, 0xff7dff8f,
    0xff6a0018, 0xffda0000, 0x0025ffae, 0x0004ffb5,
    0xffd30000, 0xffe20024, 0x00100005, 0x001dffea,
    0x0004fff9, 0xfff70013, 0x00010015, 0x00130003};

uint32_t fir_coef_8khz[64] = {
    0x8002801a, 0x8001801a, 0x80138011, 0x801e0000,
    0x800d0001, 0x000e8018, 0x000a8021, 0x80170005,
    0x80140033, 0x00350025, 0x0074800c, 0x004a000c,
    0x00000085, 0x003a00c4, 0x00e00060, 0x00fd800a,
    0x00370059, 0x805b011f, 0x004100e9, 0x01178067,
    0x00558118, 0x81a88020, 0x824c00b7, 0x80b680d3,
    0x00208393, 0x828983d1, 0x862a8119, 0x8558803e,
    0x804784cd, 0x00bc89f9, 0x8921867a, 0x98cf0746,
    0x5f94146c, 0x98cf146c, 0x89210746, 0x00bc867a,
    0x804789f9, 0x855884cd, 0x862a803e, 0x82898119,
    0x002083d1, 0x80b68393, 0x824c80d3, 0x81a800b7,
    0x00558020, 0x01178118, 0x00418067, 0x805b00e9,
    0x0037011f, 0x00fd0059, 0x00e0800a, 0x003a0060,
    0x000000c4, 0x004a0085, 0x0074000c, 0x0035800c,
    0x80140025, 0x80170033, 0x000a0005, 0x000e8021,
    0x800d8018, 0x801e0001, 0x80130000, 0x80018011};
/* Private function prototypes -----------------------------------------------*/
void DMIC_COEF_16KHz_Init(void);
void DMIC_COEF_8KHz_Init(void);
static void DMIC_DMACh0Cplt(DMA_HandleTypeDef *hdma);
__attribute__((weak)) void DMIC_DMACh1Cplt(DMA_HandleTypeDef *hdma);

void DMIC_MSPInit()
{
  // Enable peripher clock
  REG_FIELD_WR(RCC->APB1EN, RCC_PDM, 1);
  // software reset DMIC module
  REG_FIELD_WR(RCC->APB1RST, RCC_PDM, 1);
  REG_FIELD_WR(RCC->APB1RST, RCC_PDM, 0);
}

/* @brief initial PDM as 16Khz/16Bit
 *        Notes:
 *        capture_delay - related to L/R config and sensor's Tdd(delay time to data line driven)
 *           DMIC as memsensing, 
 *           -- if L/R=VDD, has a 50ns tdd after rising edge, data will be invalid after falling edge
 */
void DMIC_Init_16KHz_16Bit_Mono(DMIC_Init_TypeDef *dmicInit)
{
	dmicInit->clkratio = DMIC_CLK1P024MHZ_RATIO;//124;        // DMIC clock freq = 128M/(124+1)=1.024MHz
	dmicInit->gain = 6;              // gain adjustment
	dmicInit->sampleRate = 63;       // DMIC sample freq = 1.024MHz/(63+1)= 16000
	dmicInit->mode = DMIC_MODE_Mono;  // Mono for voice 
	dmicInit->captureDelay = 30;     // DMIC data capture after 30 cycles of 128Mhz = 30 * 7.8125ns = 234.375ns
	dmicInit->sampleFreq = DMIC_SampleFreq_16KHz;
}
/* @brief DMIC module initialization
 *
 */
void HAL_DMIC_Init(DMIC_HandleTypeDef *hdmic)
{
  DMIC_Init_TypeDef dmicInit;
  /* Enable clock and do software reset */
  DMIC_MSPInit();

  hdmic->Instance = PDM;

  DMIC_Init_16KHz_16Bit_Mono(&dmicInit);
 
  if (dmicInit.mode == DMIC_MODE_Stereo)
    WRITE_REG(PDM->CR, 0x08);
  else
    WRITE_REG(PDM->CR, 0x00);

  WRITE_REG(PDM->CFG, (dmicInit.gain << 24) | (dmicInit.sampleRate << 16) 
                    | (dmicInit.captureDelay << 8) | (dmicInit.clkratio));

  WRITE_REG(PDM->IDR, 0x1); // Disable Int

  if (dmicInit.sampleFreq == DMIC_SampleFreq_8KHz)
    DMIC_COEF_8KHz_Init();
  else
    DMIC_COEF_16KHz_Init();
  
  // initial buffer pointers
  DmicState.Event = 0;
  DmicState.WriteIndex = 0;
  DmicState.ReadIndex = 0;
  hdmic->dmicState = (void *)&DmicState;
}

void HAL_DMIC_START(DMIC_HandleTypeDef *hdmic)
{
  SET_BIT(PDM->CR, 1);
}

/* stop DMIC */
void HAL_DMIC_STOP(DMIC_HandleTypeDef *hdmic)
{
  SET_BIT(PDM->CR, 0);
}

void HAL_DMIC_DMA_Mono_Init(DMIC_HandleTypeDef *hdmic, uint8_t dma_ch)
{
  hdmic->hdmadmicrx.Parent = hdmic;
  hdmic->hdmadmicrx.Instance = DMA1;
  hdmic->hdmadmicrx.Config.dst_inc = DMA_INC_HALFWORD;
  hdmic->hdmadmicrx.Config.src_inc = DMA_INC_NONE;
  hdmic->hdmadmicrx.Config.data_size = DMA_SIZE_HALFWORD;
  hdmic->hdmadmicrx.Config.next_useburst = DISABLED;
  hdmic->hdmadmicrx.Config.primary = ENABLED;
  hdmic->hdmadmicrx.Config.priority = DMA_PRIO_HIGH;
  hdmic->hdmadmicrx.Config.cycle_ctrl = DMA_CC_BASIC;
  hdmic->hdmadmicrx.Config.burst = DISABLED;
  hdmic->hdmadmicrx.Config.intEn = ENABLED;
  hdmic->hdmadmicrx.Config.sigSel = CH_PDMCH0_RX;
  hdmic->hdmadmicrx.Config.rpower = DMA_RPOWER1;

  hdmic->hdmadmicrx.Config.src_end_ptr = (void *)hdmic->Instance->DATA0;
  hdmic->hdmadmicrx.Config.dst_end_ptr = (void *)&hdmic->dmicState->Buffer[0];  
  hdmic->hdmadmicrx.Config.chnIndex = dma_ch;

  HAL_DMA_Init(&hdmic->hdmadmicrx);

}

#ifdef DMIC_STEREO
void HAL_DMIC_DMA_Stereo_Init(DMIC_HandleTypeDef *hdmic, uint8_t dma_ch0, uint8_t dma_ch1)
{
  HAL_DMIC_DMA_Mono_Init(hdmic, dma_ch0);
  hdmic->hdmadmicrx1.Config.dst_inc = DMA_INC_HALFWORD;
  hdmic->hdmadmicrx1.Config.src_inc = DMA_INC_NONE;
  hdmic->hdmadmicrx1.Config.data_size = DMA_SIZE_HALFWORD;
  hdmic->hdmadmicrx1.Config.next_useburst = DISABLED;
  hdmic->hdmadmicrx1.Config.primary = ENABLED;
  hdmic->hdmadmicrx1.Config.priority = DMA_PRIO_HIGH;
  hdmic->hdmadmicrx1.Config.cycle_ctrl = DMA_CC_BASIC;
  hdmic->hdmadmicrx1.Config.burst = DISABLED;
  hdmic->hdmadmicrx1.Config.intEn = ENABLED;
  hdmic->hdmadmicrx1.Config.rpower = DMA_RPOWER1;
  hdmic->hdmadmicrx1.Parent = hdmic;
  hdmic->hdmadmicrx1.Instance = DMA1;

  hdmic->hdmadmicrx1.Config.sigSel = CH_PDMCH1_RX;
  hdmic->hdmadmicrx1.Config.src_end_ptr = (void *)hdmic->Instance->DATA1;
  hdmic->hdmadmicrx1.Config.dst_end_ptr = (void *)&hdmic->dmicState1->Buffer[0];
  hdmic->hdmadmicrx1.Config.chnIndex = dma_ch1;
  
  HAL_DMA_Init(&hdmic->hdmadmicrx1);

}
#endif

/* @brief Start accquire PCM data by DMA
 *
 */
HAL_StatusTypeDef HAL_DMIC_Start_DMA(DMIC_HandleTypeDef *hdmic)
{
  if (&(hdmic->hdmadmicrx) != NULL)
  {
    /* Set the DMA transfer complete callback */
    hdmic->hdmadmicrx.XferCpltCallback = DMIC_DMACh0Cplt;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(&hdmic->hdmadmicrx, (uint32_t)&hdmic->Instance->DATA0, (uint32_t)hdmic->dmicState->Buffer[0], DMIC_FRAME_SIZE);
  }

#ifndef DMIC_STEREO

  REG_FIELD_WR(hdmic->Instance->CR, PDM_CR_DMAEN, 1);

#else

  if (&(hdmic->hdmadmicrx1) != NULL)
  {
    /* Set the DMA transfer complete callback */
    hdmic->hdmadmicrx1.XferCpltCallback = DMIC_DMACh1Cplt;

    /* Enable the DMA channel */
    HAL_DMA_Start_IT(&hdmic->hdmadmicrx1, (uint32_t)&hdmic->Instance->DATA1, (uint32_t)hdmic->DmicState1->Buffer[0], DMIC_FRAME_SIZE);
  }

  REG_FIELD_WR(hdmic->Instance->CR, DMIC_CR_DMAEN, 3);

#endif

  return HAL_OK;
}

/* @brief DMIC DMA xfer complete callback
 *        callback can be:
 *        Sent through UART
 *        Call ADPCM
 * 
 */
static void DMIC_DMACh0Cplt(DMA_HandleTypeDef *hdma)
{
  uint8_t Index;
  DMIC_HandleTypeDef *hdmic;
  DMIC_STATE_TypeDef *dmicState;

  hdmic = hdma->Parent;
  dmicState = hdmic->dmicState;

  Index = hdmic->dmicState->WriteIndex + 1;
  if (Index >= DMIC_NUM_BUFFERS)
  {
    Index = 0;
  }

  if (Index != hdmic->dmicState->ReadIndex)
  {
    hdmic->dmicState->WriteIndex = Index;
  }

  HAL_DMA_Start_IT(hdma, (uint32_t)&hdmic->Instance->DATA0, (uint32_t)hdmic->dmicState->Buffer[dmicState->WriteIndex], DMIC_FRAME_SIZE);

  hdma->State = HAL_DMA_STATE_READY;
  dmicState->Event = 1;
}

#ifdef DMIC_STEREO
__attribute__((weak)) void DMIC_DMACh1Cplt(DMA_HandleTypeDef *hdma)
{
  uint8_t Index;
  DMIC_HandleTypeDef *hdmic;
  DMIC_STATE_TypeDef *dmicState;

  hdmic = hdma->Parent;
  dmicState = hdmic->dmicState1;

  Index = hdmic->dmicState->WriteIndex + 1;
  if (Index >= DMIC_NUM_BUFFERS)
  {
    Index = 0;
  }

  if (Index != hdmic->dmicState->ReadIndex)
  {
    hdmic->dmicState->WriteIndex = Index;
  }

  HAL_DMA_Start_IT(hdma, (uint32_t)&hdmic->Instance->DATA1, (uint32_t)hdmic->dmicState1->Buffer[dmicState->WriteIndex], DMIC_FRAME_SIZE);

  hdma->State = HAL_DMA_STATE_READY;
  dmicState1->Event = 1;
}
#endif

/* @brief Initialize filter coef according to sample rate 16KHz
 *
 */
void DMIC_COEF_16KHz_Init()
{
  uint8_t i;
  for (i = 0; i <= 63; i++)
  {
    PDM_COEF->pdm_fir_coef[i] = fir_coef_16khz[i];
  }
  return;
}

/* @brief Initialize filter coef according to sample rate 8KHz
 *
 */
void DMIC_COEF_8KHz_Init()
{
  uint8_t i;
  for (i = 0; i <= 63; i++)
  {
    PDM_COEF->pdm_fir_coef[i] = fir_coef_8khz[i];
  }
  return;
}

//#endif
/************************ (C) COPYRIGHT Linkedsemi *****END OF FILE****/
