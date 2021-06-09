#include "uart_msp.h"
#include "field_manipulate.h"
#include "lsuart.h"
#include "core_rv32.h"
#include "HAL_def.h"
#include "reg_sysc_per.h"
#include "exception_isr.h"
#include "taurus.h"
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
        rv_set_int_isr(UART1_IRQn,UART1_Handler);
        UART_inst_env[0] = inst;
        csi_vic_clear_pending_irq(UART1_IRQn);
        csi_vic_enable_irq(UART1_IRQn);
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_SET_UART1_MASK;
    break;
    case (uint32_t)UART2:
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_CLR_UART2_N_MASK;
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_SET_UART2_N_MASK;
        rv_set_int_isr(UART2_IRQn,UART2_Handler);
        UART_inst_env[1] = inst;
        csi_vic_clear_pending_irq(UART2_IRQn);
        csi_vic_enable_irq(UART2_IRQn);
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_SET_UART2_MASK;
    break;
    case (uint32_t)UART3:
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_CLR_UART3_N_MASK;
        SYSC_PER->PD_PER_SRST0 = SYSC_PER_SRST_SET_UART3_N_MASK;
        rv_set_int_isr(UART3_IRQn,UART3_Handler);
        UART_inst_env[2] = inst;
        csi_vic_clear_pending_irq(UART3_IRQn);
        csi_vic_enable_irq(UART3_IRQn);
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
        csi_vic_disable_irq(UART1_IRQn);
    break;
    case (uint32_t)UART2:
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_CLR_UART2_MASK;
        csi_vic_disable_irq(UART2_IRQn);
    break;
    case (uint32_t)UART3:
        SYSC_PER->PD_PER_CLKG0 = SYSC_PER_CLKG_CLR_UART3_MASK;
        csi_vic_disable_irq(UART3_IRQn);
    break;
    }
}

void HAL_UART_MSP_Busy_Set(UART_HandleTypeDef *inst)
{

}

void HAL_UART_MSP_Idle_Set(UART_HandleTypeDef *inst)
{

}
