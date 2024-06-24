/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

/*
   This is an example board.h for Board Compment, New Board should flow the macro defines.
*/

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

// Common Board Features Define

/*
    The Common BOARD_XXX Macro Defines Boards supported features which may reference by Solutions.
    Common board macro include:
     . BOARD_NAME
     · UART
     · GPIO
     · PWM
     · ADC
     · BUTTON
     · LED
     · WIFI
     · BT
     · AUDIO
    BOARD_XXX Macro descripted below should be defined if the board support.
*/

/****************************************************************************/

/*
    This riscv dummy board include:
     · UART        x1
     · GPIO        x2
     · PWM         x2
     · ADC         x1
     · BUTTON      x2
     · LED         x2
     · WIFI        x0
     · BT          x0
     · AUDIO       x1
*/

#ifndef CONFIG_BOARD_UART
#define CONFIG_BOARD_UART 1
#endif

#ifndef CONFIG_BOARD_GPIO
#define CONFIG_BOARD_GPIO 1
#endif

//#ifndef CONFIG_BOARD_PWM
//#define CONFIG_BOARD_PWM 0
//#endif

//#ifndef CONFIG_BOARD_ADC
//#define CONFIG_BOARD_ADC 0
//#endif

#ifndef CONFIG_BOARD_BUTTON
#define CONFIG_BOARD_BUTTON 0
#endif

#ifndef CONFIG_BOARD_LED
#define CONFIG_BOARD_LED 0
#endif

#ifndef CONFIG_BOARD_WIFI
#define CONFIG_BOARD_WIFI 0
#endif

#ifndef CONFIG_BOARD_BT
#define CONFIG_BOARD_BT 0
#endif

#ifndef CONFIG_BOARD_AUDIO
#define CONFIG_BOARD_AUDIO 0
#endif

#ifndef CONFIG_BOARD_DISPLAY
#define CONFIG_BOARD_DISPLAY 0
#endif

#ifndef CONFIG_BOARD_VENDOR
#define CONFIG_BOARD_VENDOR 1
#endif

#define BOARD_NAME "D1_DOCKER_PRO"

/* the board pins, can be used as uart, gpio, pwd... */
#define BOARD_PIN0  (PF4)
#define BOARD_PIN1  (PF2)
#define BOARD_PIN2  (PG8)
#define BOARD_PIN3  (PG9)
#define BOARD_PIN4  (PB3)
#define BOARD_PIN5  (PB2)
#define BOARD_PIN6  (PB10)
#define BOARD_PIN7  (PE4)
#define BOARD_PIN8  (PE14)
#define BOARD_PIN9  (PE1)
#define BOARD_PIN10 (PE7)
#define BOARD_PIN11 (PE13)
#define BOARD_PIN12 (PE10)
#define BOARD_PIN13 (PE8)
#define BOARD_PIN14 (PB4)
#define BOARD_PIN15 (PB5)
#define BOARD_PIN16 (PE6)
#define BOARD_PIN17 (PE5)
#define BOARD_PIN18 (PB7)
#define BOARD_PIN19 (PB6)
#define BOARD_PIN20 (PE3)
#define BOARD_PIN21 (PE2)
#define BOARD_PIN22 (PE0)
#define BOARD_PIN23 (PE9)
#define BOARD_PIN24 (PE11)
#define BOARD_PIN25 (PE16)
#define BOARD_PIN26 (PC1)

//...

#if defined(CONFIG_BOARD_UART) && CONFIG_BOARD_UART
// UART

/*
    The total supported uart numbers on this board, 0 meas No uart support.
    the BOARD_UART<x>_XXX, x in rang of (0, BOARD_UART_NUM - 1)
*/
#ifndef BOARD_UART_NUM
#define BOARD_UART_NUM (1)
#endif

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0
/* the board uart0 tx pin */
#define BOARD_UART0_TX_PIN (BOARD_PIN1)
/* the borad uart0 rx pin */
#define BOARD_UART0_RX_PIN (BOARD_PIN0)
/* The real UART port reference to board logic port 0 */
#define BOARD_UART0_IDX (0)
/* The default baudrate for uart0 */
#define BOARD_UART0_BAUD (115200)

#define BOARD_UART1_TX_PIN (BOARD_PIN14)
#define BOARD_UART1_RX_PIN (BOARD_PIN15)
#define BOARD_UART1_IDX    (5)
#define BOARD_UART1_BAUD   (115200)

// ...
#endif // defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0

#endif // defined(CONFIG_BOARD_UART) && CONFIG_BOARD_UART

#if defined(CONFIG_BOARD_GPIO) && CONFIG_BOARD_GPIO
// GPIO
/*
    The total supported GPIO Pin numbers on this board, 0 meas No uart support.
    the BOARD_GPIO_PIN<x>, x in rang of (0, BOARD_GPIO_PIN_NUM - 1)
*/
#ifndef BOARD_GPIO_PIN_NUM
#define BOARD_GPIO_PIN_NUM (20)
#endif

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
/* The real gpio reference to board logic gpio pin */
#define BOARD_GPIO_PIN0  (BOARD_PIN4)
#define BOARD_GPIO_PIN1  (BOARD_PIN5)
#define BOARD_GPIO_PIN2  (BOARD_PIN6)
#define BOARD_GPIO_PIN3  (BOARD_PIN7)
#define BOARD_GPIO_PIN4  (BOARD_PIN8)
#define BOARD_GPIO_PIN5  (BOARD_PIN9)
#define BOARD_GPIO_PIN6  (BOARD_PIN10)
#define BOARD_GPIO_PIN7  (BOARD_PIN11)
#define BOARD_GPIO_PIN8  (BOARD_PIN12)
#define BOARD_GPIO_PIN9  (BOARD_PIN13)
#define BOARD_GPIO_PIN10 (BOARD_PIN16)
#define BOARD_GPIO_PIN11 (BOARD_PIN17)
#define BOARD_GPIO_PIN12 (BOARD_PIN18)
#define BOARD_GPIO_PIN13 (BOARD_PIN19)
#define BOARD_GPIO_PIN14 (BOARD_PIN20)
#define BOARD_GPIO_PIN15 (BOARD_PIN21)
#define BOARD_GPIO_PIN16 (BOARD_PIN22)
#define BOARD_GPIO_PIN17 (BOARD_PIN23)
#define BOARD_GPIO_PIN18 (BOARD_PIN24)
#define BOARD_GPIO_PIN19 (BOARD_PIN26)

#endif // defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
#endif // defined(CONFIG_BOARD_GPIO) && CONFIG_BOARD_GPIO

#if defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM
// PWM
/* the board supported pwm channels */
#ifndef BOARD_PWM_NUM
#define BOARD_PWM_NUM (2)
#endif

#if defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
/* the board pwm pin */
#define BOARD_PWM0_PIN (BOARD_PIN4)
/* The real pwm channel reference to board logic pwm channel */
#define BOARD_PWM0_CH (0)

#define BOARD_PWM1_PIN (BOARD_PIN5)
#define BOARD_PWM1_CH  (1)
#endif // defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
#endif // defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM

#if defined(CONFIG_BOARD_ADC) && CONFIG_BOARD_ADC > 0
// ADC
/* the board supported adc channels */
#ifndef BOARD_ADC_NUM
#define BOARD_ADC_NUM (1)
#endif

#if defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
/* the board adc pin */
#define BOARD_ADC0_PIN (BOARD_PIN6)
/* The real adc channel reference to board logic adc channel */
#define BOARD_ADC0_CH (0)
#endif // defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
#endif // defined(CONFIG_BOARD_ADC) && CONFIG_BOARD_ADC > 0

#if defined(CONFIG_BOARD_BUTTON) && CONFIG_BOARD_BUTTON > 0
// BUTTON
#ifndef BOARD_BUTTON_NUM
/*
    the board supported buttons, include gpio button and adc button,
    BOARD_BUTTON_NUM = BOARD_BUTTON_GPIO_NUM + BOARD_BUTTON_ADC_NUM.

*/
#define BOARD_BUTTON_NUM (0)
#endif

#if defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0

#define BOARD_BUTTON0_PIN (BOARD_PIN7)
#define BOARD_BUTTON1_PIN (BOARD_PIN8)
#define BOARD_BUTTON2_PIN (BOARD_PIN9)
#define BOARD_BUTTON3_PIN (BOARD_PIN10)

// GPIO BUTTON
/* the board supported GPIO Buttons */
#ifndef BOARD_BUTTON_GPIO_NUM
#define BOARD_BUTTON_GPIO_NUM (2)
#endif

#if defined(BOARD_BUTTON_GPIO_NUM) && BOARD_BUTTON_GPIO_NUM > 0
/* the board logic button id, in range of (0, BOARD_BUTTON_GPIO_NUM - 1) */
#define BOARD_BUTTON0 (0)
/* for gpio button, define the pin numner. if the gpio pin used as gpio button, it shoudn't reference as BOARD_GPIO_PINx
 */
#define BOARD_BUTTON0_GPIO_PIN (BOARD_BUTTON0_PIN)

#define BOARD_BUTTON1          (1)
#define BOARD_BUTTON1_GPIO_PIN (BOARD_BUTTON1_PIN)
#endif // defined(BOARD_BUTTON_GPIO_NUM) && BOARD_BUTTON_GPIO_NUM > 0

// ADC BUTTON
/* the board supported adc Buttons */
#ifndef BOARD_BUTTON_ADC_NUM
#define BOARD_BUTTON_ADC_NUM (0)
#endif

#if defined(BOARD_BUTTON_ADC_NUM) && BOARD_BUTTON_ADC_NUM > 0
/* the board logic adc button id, in range of (BOARD_BUTTON_GPIO_NUM, BOARD_BUTTON_NUM - 1), if not suuport GPIO Button,
 * BOARD_BUTTON_GPIO_NUM should be 0 */
#define BOARD_BUTTON2         (BOARD_BUTTON_GPIO_NUM + 0)
#define BOARD_BUTTON2_ADC_PIN (BOARD_BUTTON2_PIN)
/* the adc channel used for button2, if the adc channel used as adc button, it shoudn't reference as BOARD_ADCx_CH*/
#define BOARD_BUTTON2_ADC_CH (1)
/* the adc device name */
#define BOARD_BUTTON2_ADC_NAME "adc1"
/* adc voltage reference */
#define BOARD_BUTTON2_ADC_REF (100)
/* adc voltage range */
#define BOARD_BUTTON2_ADC_RANG (500)

#define BOARD_BUTTON3          (BOARD_BUTTON_GPIO_NUM + 1)
#define BOARD_BUTTON2_ADC_PIN  (BOARD_BUTTON3_PIN)
#define BOARD_BUTTON3_ADC_CH   (1)
#define BOARD_BUTTON3_ADC_NAME "adc1"
#define BOARD_BUTTON3_ADC_REF  (600)
#define BOARD_BUTTON3_ADC_RANG (500)

//#define BOARD_ADC_BUTTON2       (2)
//#define BOARD_ADC_BUTTON2_CH    (1)
//#define BOARD_ADC_BUTTON2_NAME  "adc1"
//#define BOARD_ADC_BUTTON2_REF   xxx
//#define BOARD_ADC_BUTTON2_RANG  xxx
#endif // defined(BOARD_BUTTON_ADC_NUM) && BOARD_BUTTON_ADC_NUM > 0

#endif // defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0

#endif // defined(CONFIG_BOARD_BUTTON) && CONFIG_BOARD_BUTTON > 0

#if defined(CONFIG_BOARD_LED) && CONFIG_BOARD_LED > 0
// LED
/* the board supported leds */
#ifndef BOARD_LED_NUM
#define BOARD_LED_NUM (1)
#endif

#define BOARD_LED0_PIN BOARD_PIN25

// PWM LED
/* the board supported pwm leds */
#ifndef BOARD_LED_PWM_NUM
#define BOARD_LED_PWM_NUM (1)
#endif

#if defined(BOARD_LED_PWM_NUM) && BOARD_LED_PWM_NUM > 0
#define BOARD_LED0_PWM_PIN (BOARD_LED0_PIN)
/* the pwm channel used for led0, if the pwm channel used as led0, it shoudn't reference as BOARD_PWMx_CH */
#define BOARD_LED0_PWM_CH (7)
#endif // defined(BOARD_LED_PWM_NUM) && BOARD_LED_PWM_NUM > 0

// GPIO LED
#ifndef BOARD_LED_GPIO_NUM
#define BOARD_LED_GPIO_NUM (0)
#endif

#if defined(BOARD_LED_GPIO_NUM) && BOARD_LED_GPIO_NUM > 0
/* the gpio pin used for led0, if the gpio pin used as led, it shoudn't reference as BOARD_GPIO_PINx */
#define BOARD_LED1_GPIO_PIN (BOARD_LED1_PIN)
#endif // defined(BOARD_LED_GPIO_NUM) && BOARD_LED_GPIO_NUM > 0
#endif // defined(CONFIG_BOARD_LED) && CONFIG_BOARD_LED > 0

#if defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT > 0
// BT
/* the board support bluetooth */
#ifndef BOARD_BT_SUPPORT
#define BOARD_BT_SUPPORT 1
#endif
#endif // defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT > 0

#if defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI > 0
// WIFI
/* the board support wifi */
#ifndef BOARD_WIFI_SUPPORT
#define BOARD_WIFI_SUPPORT 1
#endif
#endif // defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI > 0

#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO > 0
// Audio
/* the board support audio */
#ifndef BOARD_AUDIO_SUPPORT
#define BOARD_AUDIO_SUPPORT 1
#endif
#endif // defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO > 0

/****************************************************************************/
// Common solutions defines

// Console config, Almost all solutions and demos use these.
#ifndef CONSOLE_UART_IDX
#define CONSOLE_UART_IDX (BOARD_UART0_IDX)
#endif

#ifndef CONFIG_CLI_USART_BAUD
#define CONFIG_CLI_USART_BAUD (BOARD_UART0_BAUD)
#endif

#ifndef CONFIG_CONSOLE_UART_BUFSIZE
#define CONFIG_CONSOLE_UART_BUFSIZE (512)
#endif

/****************************************************************************/
// Commom test demos defines

// i2c
#define EXAMPLE_IIC_IDX          0 // 1
#define EXAMPLE_PIN_IIC_SDA      0 // PC1
#define EXAMPLE_PIN_IIC_SCL      0 // PC0
#define EXAMPLE_PIN_IIC_SDA_FUNC 0 // PC1_I2C1_SDA
#define EXAMPLE_PIN_IIC_SCL_FUNC 0 // PC0_I2C1_SCL

// adc
#define EXAMPLE_ADC_CH0       0 // PA8
#define EXAMPLE_ADC_CH0_FUNC  0 // PA8_ADC_A0
#define EXAMPLE_ADC_CH12      0 // PA26
#define EXAMPLE_ADC_CH12_FUNC 0 // PA26_ADC_A12

/****************************************************************************/
// Vendor board defines

#define CLOCK_GETTIME_USE_TIMER_ID 0
#define WLAN_ENABLE_PIN            PG12
#define WLAN_POWER_PIN             0xFFFFFFFF

#define LED_PIN PC1 // LED RGB

/****************************************************************************/
/**
 * @brief  init the board for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_init(void);

/**
 * @brief  init the board gpio pin for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_gpio_pin_init(void);

/**
 * @brief  init the board uart for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_uart_init(void);

/**
 * @brief  init the board flash for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_flash_init(void);

/**
 * @brief  init the board pwm for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_pwm_init(void);

/**
 * @brief  init the board adc for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_adc_init(void);

/**
 * @brief  init the board button for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_button_init(void);

/**
 * @brief  init the board led for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_led_init(void);

/**
 * @brief  init the board wifi for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_wifi_init(void);

/**
 * @brief  init the board bt for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_bt_init(void);

/**
 * @brief  init the board audio for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_audio_init(void);

/**
 * @brief  init the board display for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_display_init(void);

/**
 * @brief init board cli cmd
 * re-implement if need.
 * @return
 */
void board_vendor_init(void);

/****************************************************************************/
//board extend api

#ifdef CONFIG_BOARD_AUDIO
#include "audio/board_audio.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */