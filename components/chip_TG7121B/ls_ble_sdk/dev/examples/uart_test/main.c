#include "lsuart.h"
#include "lsgpio.h"
#include "le501x.h"
#include "platform.h"
#include "io_config.h"
#include <string.h>

#define TEST_ZONE_SIZE 512
static void uart_test_init(void);
static void gpio_init();

UART_HandleTypeDef UART_Config; 
uint8_t test_zone_a[TEST_ZONE_SIZE * 2] ;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart,void *tx_arg)
{
    //UART disable
    // HAL_UART_DeInit(huart);
    // uart1_io_deinit();
    /*code */

    /* user code end */

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart,void *rx_arg)
{
    HAL_UART_Transmit_IT(&UART_Config,test_zone_a,1,NULL);
    HAL_UART_Receive_IT(&UART_Config,test_zone_a,1,NULL);
}

static void gpio_init()
{
    uart1_io_init(PB00,PB01);
}

static void uart_test_init(void)
{
    UART_Config.UARTX = UART1;
    UART_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART_Config.Init.MSBEN = 0;
    UART_Config.Init.Parity = UART_NOPARITY;
    UART_Config.Init.StopBits = UART_STOPBITS1;
    UART_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Config);
}

static void uart_test()
{
    HAL_UART_Receive_IT(&UART_Config,test_zone_a,1,NULL);
}

int main()
{
    sys_init_app();
    gpio_init();
    uart_test_init();
    uart_test();
    while(1)
    {
    }
}




