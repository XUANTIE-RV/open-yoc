/*********************************************************************************
    _     _____  _   _  _   __  _____   _____    _____   _____   _      _   _____
   | |   |_   _|| \ | || | / / |  ___| |  _  \  |  ___| |  ___| | \    / | |_   _|
   | |     | |  |  \| || |/ /  | |__   | | \  | | |___  | |__   |  \  /  |   | |
   | |     | |  | \ ` ||    \  |  __|  | |  | | |___  | |  __|  | . \/ . |   | |
   | |___ _| |_ | |\  || |\  \ | |___  | |__/ |  ___| | | |___  | |\  /| |  _| |_
   \____/ \___/ \_| \_/\_| \_/ \____/  |_____/  |_____| \____/  \_| \/ |_/ \_____/

	********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "ls_ble.h"
#include "platform.h"
#include "prf_diss.h"
#include "log.h"
#include "ls_dbg.h"
#include "cpu.h"
#include "builtin_timer.h"
#include <string.h>
#include "co_math.h"
#include "io_config.h"
#include "SEGGER_RTT.h"
#include "lssmartcard.h"
/** @addtogroup le501x_HAL_Examples
  * @{
  */

/** @addtogroup SMARTCARD
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* SMARTCARD handler declaration */
SMARTCARD_HandleTypeDef SCHandle;
	
int8_t Txbuff[20] = {0};

int8_t Com_Sta = 0x55;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void Error_Handler(void);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* le501x HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Low Level Initialization
     */
  sys_init_app();
	
	uart1_7816_io_init(PB12,PB13);
	
  /* Audio reset pin configuration -------------------------------------------------*/
	io_cfg_output(PB15);
  /* SC_USART configuration --------------------------------------------------*/
  /* SC_USART configured as follows:
  - Word Length = 8 Bits
  - 1.5 Stop Bit
  - Even parity
  - BaudRate = 5376 baud
  - Hardware flow control disabled (RTS and CTS signals)
  - Tx and Rx enabled
  - USART Clock enabled
  */
  /* USART Clock set to 2 MHz (PCLK1 (16 MHz) / 8) => prescaler set to 0x03 */
  SCHandle.UARTX = UART1;
  SCHandle.Init.BaudRate = 5376;  /* Starting baudrate = 2MHz / 372etu */
  SCHandle.Init.WordLength = SMARTCARD_WORDLENGTH_8B;
  SCHandle.Init.StopBits = SMARTCARD_STOPBITS_1_5;
  SCHandle.Init.Parity = SMARTCARD_PARITY_EVEN;
  SCHandle.Init.Mode = SMARTCARD_MODE_TX;
  SCHandle.Init.Prescaler = SMARTCARD_PRESCALER_SYSCLK_DIV8;
  SCHandle.Init.GuardTime = 16;
  SCHandle.Init.NACKState = SMARTCARD_NACK_ENABLE;
	SCHandle.Init.Retry = SMARTCARD_RETRYTIME_3T;

  if(HAL_SMARTCARD_Init(&SCHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	io_write_pin(PB15,1);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	io_write_pin(PB15,0);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	DELAY_US(500);
	io_write_pin(PB15,1);
//	HAL_SMARTCARD_Receive(&SCHandle, (uint8_t *)Txbuff, 13, 1000);
	HAL_SMARTCARD_Receive_IT(&SCHandle, (uint8_t *)Txbuff, 13);
	
	while(Com_Sta == 0x55)
	{
		;
	}
	
  /* Infinite loop */
  while (1)
  {
		
  }
}

 void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef *hsc)
{
	Com_Sta = 0xAA;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuff1, pBuff2: buffers to be compared.
  * @param  Length: buffer's length
  * @retval 0  : pBuff1 identical to pBuff2
  *         >0 : pBuff1 differs from pBuff2
  */
// static uint16_t Buffercmp(uint8_t* pBuff1, uint8_t* pBuff2, uint16_t Length)
// {
//   while (Length--)
//   {
//     if((*pBuff1) != *pBuff2)
//     {
//       return Length;
//     }
//     pBuff1++;
//     pBuff2++;
//   }

//   return 0;
// }

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  while (1)
  {
		;
  }
}

 
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/*****************************END OF FILE************************************/
