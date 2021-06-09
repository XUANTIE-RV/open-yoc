#ifndef SPI_MSP_H_
#define SPI_MSP_H_

#include "lsspi.h"
#include "lsi2s.h"
#include "reg_spi.h"
#include "field_manipulate.h"


void HAL_SPI_MSP_Init(SPI_HandleTypeDef *inst);
void HAL_SPI_MSP_DeInit(SPI_HandleTypeDef *inst);
void HAL_SPI_MSP_Busy_Set(SPI_HandleTypeDef *inst);
void HAL_SPI_MSP_Idle_Set(SPI_HandleTypeDef *inst);

void HAL_I2S_MSP_Init(I2S_HandleTypeDef *inst);
void HAL_I2S_MSP_DeInit(I2S_HandleTypeDef *inst);
void HAL_I2S_MSP_Busy_Set(I2S_HandleTypeDef *inst);
void HAL_I2S_MSP_Idle_Set(I2S_HandleTypeDef *inst);


#endif
