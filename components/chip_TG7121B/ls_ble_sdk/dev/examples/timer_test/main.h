#ifndef __MAIN_H
#define __MAIN_H

#include "lstimer.h"
#include "lsgpio.h"
#include <string.h>

#define TIMx                           LSGPTIMB
#define TIMx_CLK_ENABLE()              __HAL_RCC_GPTIMB_CLK_ENABLE()

/* Definition for TIMx Channel Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define TIMx_GPIO_PORT_CHANNEL1        LSGPIOA
#define TIMx_GPIO_PORT_CHANNEL2        LSGPIOA
#define TIMx_GPIO_PORT_CHANNEL3        LSGPIOA
#define TIMx_GPIO_PORT_CHANNEL4        LSGPIOA
#define TIMx_GPIO_PIN_CHANNEL1         GPIO_PIN_0
#define TIMx_GPIO_PIN_CHANNEL2         GPIO_PIN_1
#define TIMx_GPIO_PIN_CHANNEL3         GPIO_PIN_7
#define TIMx_GPIO_PIN_CHANNEL4         GPIO_PIN_8
#define TIMx_GPIO_AF_CHANNEL1          /
#define TIMx_GPIO_AF_CHANNEL2          /
#define TIMx_GPIO_AF_CHANNEL3          /
#define TIMx_GPIO_AF_CHANNEL4          /


#endif // __MAIN_H