#include "uart_msp.h"
#include "field_manipulate.h"
#include "lsuart.h"
#include "ARMCM3.h"
#include "HAL_def.h"
#include "reg_sysc_per.h"
#include "platform.h"

static UART_HandleTypeDef *UART_inst_env[3];

void UART1_Handler(void)
{
    HAL_UARTx_IRQHandler( UART_inst_env[0]);
}

void UART2_Handler(void)
{
    HAL_UARTx_IRQHandler( UART_inst_env[1]);
}

void UART3_Handler(void)
{
    HAL_UARTx_IRQHandler( UART_inst_env[2]);
}

void HAL_UART_MSP_Init(UART_HandleTypeDef *inst)
{
    switch((uint32_t)inst->UARTX)
    {
    case (uint32_t)UART1:
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_CLR_UART1_N_MASK;
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_SET_UART1_N_MASK;
        arm_cm_set_int_isr(UART1_IRQn,UART1_Handler);
        UART_inst_env[0] = inst;
        __NVIC_ClearPendingIRQ(UART1_IRQn);
        __NVIC_EnableIRQ(UART1_IRQn);
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_SET_UART1_MASK;
    break;
    case (uint32_t)UART2:
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_CLR_UART2_N_MASK;
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_SET_UART2_N_MASK;
        arm_cm_set_int_isr(UART2_IRQn,UART2_Handler);
        UART_inst_env[1] = inst;
        __NVIC_ClearPendingIRQ(UART2_IRQn);
        __NVIC_EnableIRQ(UART2_IRQn);
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_SET_UART2_MASK;
    break;
    case (uint32_t)UART3:
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_CLR_UART3_N_MASK;
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_SET_UART3_N_MASK;
        arm_cm_set_int_isr(UART3_IRQn,UART3_Handler);
        UART_inst_env[2] = inst;
        __NVIC_ClearPendingIRQ(UART3_IRQn);
        __NVIC_EnableIRQ(UART3_IRQn);
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_SET_UART3_MASK;
    break;
    }
}

void HAL_UART_MSP_DeInit(UART_HandleTypeDef *inst)
{
    switch((uint32_t)inst->UARTX)
    {
    case (uint32_t)UART1:
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_CLR_UART1_MASK;
        __NVIC_DisableIRQ(UART1_IRQn);
    break;
    case (uint32_t)UART2:
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_CLR_UART2_MASK;
        __NVIC_DisableIRQ(UART2_IRQn);
    break;
    case (uint32_t)UART3:
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_CLR_UART3_MASK;
        __NVIC_DisableIRQ(UART3_IRQn);
    break;
    }
}

void HAL_UART_MSP_Busy_Set(UART_HandleTypeDef *inst)
{

}

void HAL_UART_MSP_Idle_Set(UART_HandleTypeDef *inst)
{

}
