#ifndef UART_MSP_H_
#define UART_MSP_H_
#include "lsuart.h"
#include "lssmartcard.h"

void HAL_UART_MSP_Init(UART_HandleTypeDef *inst);
void HAL_UART_MSP_DeInit(UART_HandleTypeDef *inst);
void HAL_UART_MSP_Busy_Set(UART_HandleTypeDef *inst);
void HAL_UART_MSP_Idle_Set(UART_HandleTypeDef *inst);

void HAL_SMARTCARD_MSP_Init(SMARTCARD_HandleTypeDef *inst);
void HAL_SMARTCARD_MSP_DeInit(SMARTCARD_HandleTypeDef *inst);
void HAL_SMARTCARD_MSP_Busy_Set(void);
void HAL_SMARTCARD_MSP_Idle_Set(void);

#endif
