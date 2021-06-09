#include "spi_msp.h"
#include "reg_rcc.h"
#include "le501x.h"
#include "sys_stat.h"
#include "platform.h"
static SPI_HandleTypeDef *spi_inst_env;

static I2S_HandleTypeDef *i2s_inst_env;

void SPI2_Handler(void)
{
    if(i2s_inst_env == NULL)
    {
        HAL_SPI_IRQHandler(spi_inst_env);
    }
    else
    {
        /* code */
        HAL_I2S_IRQHandler(i2s_inst_env);
    }
    
}

void HAL_SPI_MSP_Init(SPI_HandleTypeDef *inst)
{
    if (inst->Instance == SPI2)
    {
        REG_FIELD_WR(RCC->APB1RST, RCC_SPI2, 1);
        REG_FIELD_WR(RCC->APB1RST, RCC_SPI2, 0);
        arm_cm_set_int_isr(SPI2_IRQn,SPI2_Handler);
        spi_inst_env = inst;
        NVIC_ClearPendingIRQ(SPI2_IRQn);
        i2s_inst_env = NULL;
        NVIC_EnableIRQ(SPI2_IRQn);
        REG_FIELD_WR(RCC->APB1EN, RCC_SPI2, 1);
    }
}

void HAL_I2S_MSP_Init(I2S_HandleTypeDef *inst)
{
    if (inst->Instance == SPI2)
    {
        REG_FIELD_WR(RCC->APB1RST, RCC_SPI2, 1);
        REG_FIELD_WR(RCC->APB1RST, RCC_SPI2, 0);
        arm_cm_set_int_isr(SPI2_IRQn,SPI2_Handler);
        spi_inst_env = NULL;
        NVIC_ClearPendingIRQ(SPI2_IRQn);
        i2s_inst_env = inst;
        NVIC_EnableIRQ(SPI2_IRQn);
        REG_FIELD_WR(RCC->APB1EN, RCC_SPI2, 1);
    }
}

void HAL_SPI_MSP_DeInit(SPI_HandleTypeDef *inst)
{
    if (inst->Instance == SPI2)
    {
        REG_FIELD_WR(RCC->APB1EN, RCC_SPI2, 0);
        NVIC_DisableIRQ(SPI2_IRQn);
    }
}

void HAL_I2S_MSP_DeInit(I2S_HandleTypeDef *inst)
{
    if (inst->Instance == SPI2)
    {
        REG_FIELD_WR(RCC->APB1EN, RCC_SPI2, 0);
        NVIC_DisableIRQ(SPI2_IRQn);
    }
}

static void spi_status_set(SPI_HandleTypeDef *inst,bool status)
{
    switch((uint32_t)inst->Instance)
    {
    case (uint32_t)SPI2:
        spi2_status_set(status);
    break;
    }
}
void i2s_status_set(I2S_HandleTypeDef *inst,bool status)
{
    switch((uint32_t)inst->Instance)
    {
    case (uint32_t)SPI2:
        spi2_status_set(status);
    break;
    }
}

void HAL_SPI_MSP_Busy_Set(SPI_HandleTypeDef *inst)
{
    spi_status_set(inst,1);
}

void HAL_SPI_MSP_Idle_Set(SPI_HandleTypeDef *inst)
{
    spi_status_set(inst,0);
}

void HAL_I2S_MSP_Busy_Set(I2S_HandleTypeDef *inst)
{
    i2s_status_set(inst,1);
}

void HAL_I2S_MSP_Idle_Set(I2S_HandleTypeDef *inst)
{
    i2s_status_set(inst,0);
}


