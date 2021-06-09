#include "uart_msp.h"
#include "reg_rcc.h"
#include "field_manipulate.h"
#include "lsuart.h"
#include "le501x.h"
#include "HAL_def.h"
#include "sys_stat.h"
#include "platform.h"
#include <stddef.h>
static UART_HandleTypeDef *uart_inst_env[3];
static SMARTCARD_HandleTypeDef *smartcard_inst_env;

void UART1_Handler(void)
{
    if(uart_inst_env[0] != NULL){
        HAL_UARTx_IRQHandler( uart_inst_env[0]);
    }
    else{
        HAL_SMARTCARD_IRQHandler( smartcard_inst_env);
    }
}
void UART2_Handler(void)
{
    HAL_UARTx_IRQHandler( uart_inst_env[1]);
}

void UART3_Handler(void)
{
    HAL_UARTx_IRQHandler( uart_inst_env[2]);
}

void HAL_UART_MSP_Init(UART_HandleTypeDef *inst)
{
    switch((uint32_t)inst->UARTX)
    {
    case (uint32_t)UART1:
        REG_FIELD_WR(RCC->APB2RST, RCC_UART1, 1);
        REG_FIELD_WR(RCC->APB2RST, RCC_UART1, 0);
        arm_cm_set_int_isr(UART1_IRQn,UART1_Handler);
        uart_inst_env[0] = inst;
        __NVIC_ClearPendingIRQ(UART1_IRQn);
        __NVIC_EnableIRQ(UART1_IRQn);
        REG_FIELD_WR(RCC->APB2EN, RCC_UART1, 1);
    break;
    case (uint32_t)UART2:
        REG_FIELD_WR(RCC->APB1RST, RCC_UART2, 1);
        REG_FIELD_WR(RCC->APB1RST, RCC_UART2, 0);
        arm_cm_set_int_isr(UART2_IRQn,UART2_Handler);
        uart_inst_env[1] = inst;
        __NVIC_ClearPendingIRQ(UART2_IRQn);
        __NVIC_EnableIRQ(UART2_IRQn);
        REG_FIELD_WR(RCC->APB1EN, RCC_UART2, 1);
    break;
    case (uint32_t)UART3:
        REG_FIELD_WR(RCC->APB1RST, RCC_UART3, 1);
        REG_FIELD_WR(RCC->APB1RST, RCC_UART3, 0);
        arm_cm_set_int_isr(UART3_IRQn,UART3_Handler);
        uart_inst_env[2] = inst;
        __NVIC_ClearPendingIRQ(UART3_IRQn);
        __NVIC_EnableIRQ(UART3_IRQn);
        REG_FIELD_WR(RCC->APB1EN, RCC_UART3, 1);
    break;
    }
}

void HAL_UART_MSP_DeInit(UART_HandleTypeDef *inst)
{
    switch((uint32_t)inst->UARTX)
    {
    case (uint32_t)UART1:
        REG_FIELD_WR(RCC->APB2EN, RCC_UART1, 0);
        __NVIC_DisableIRQ(UART1_IRQn);
    break;
    case (uint32_t)UART2:
        REG_FIELD_WR(RCC->APB1EN, RCC_UART2, 0);
        __NVIC_DisableIRQ(UART2_IRQn);
    break;
    case (uint32_t)UART3:
        REG_FIELD_WR(RCC->APB1EN, RCC_UART3, 0);
        __NVIC_DisableIRQ(UART3_IRQn);
    break;
    }
}

static void uart_status_set(UART_HandleTypeDef *inst,uint8_t status)
{
    switch((uint32_t)inst->UARTX)
    {
    case (uint32_t)UART1:
        uart1_status_set(status);
    break;
    case (uint32_t)UART2:
        uart2_status_set(status);
    break;
    case (uint32_t)UART3:
        uart3_status_set(status);
    break;
    }
}

void HAL_UART_MSP_Busy_Set(UART_HandleTypeDef *inst)
{
    uart_status_set(inst,1);
}

void HAL_UART_MSP_Idle_Set(UART_HandleTypeDef *inst)
{
    uart_status_set(inst,0);
}

//smartcard 
void HAL_SMARTCARD_MSP_Init(SMARTCARD_HandleTypeDef *inst)
{
    REG_FIELD_WR(RCC->APB2RST, RCC_UART1, 1);
    REG_FIELD_WR(RCC->APB2RST, RCC_UART1, 0);
    arm_cm_set_int_isr(UART1_IRQn,UART1_Handler);
    smartcard_inst_env = inst;
    __NVIC_ClearPendingIRQ(UART1_IRQn);
    __NVIC_EnableIRQ(UART1_IRQn);
    REG_FIELD_WR(RCC->APB2EN, RCC_UART1, 1);
}

void HAL_SMARTCARD_MSP_DeInit(SMARTCARD_HandleTypeDef *inst)
{
    REG_FIELD_WR(RCC->APB2EN, RCC_UART1, 0);
    __NVIC_DisableIRQ(UART1_IRQn);
}

static void smartcard_status_set(uint8_t status)
{
    uart1_status_set(status);
}

void HAL_SMARTCARD_MSP_Busy_Set(void)
{
    smartcard_status_set(1);
}

void HAL_SMARTCARD_MSP_Idle_Set(void)
{
    smartcard_status_set(0);
}
