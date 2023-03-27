/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

/*
   This is an example board.h for Board Compment, New Board should flow the macro defines.
*/

#ifndef __BOARD_H__
#define __BOARD_H__

#include <soc.h>
#include <pin_name.h>

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

#ifndef CONFIG_BOARD_PWM
#define CONFIG_BOARD_PWM 1
#endif

//#ifndef CONFIG_BOARD_ADC
//#define CONFIG_BOARD_ADC 1
//#endif

#ifndef CONFIG_BOARD_BUTTON
#define CONFIG_BOARD_BUTTON 1
#endif

#ifndef CONFIG_BOARD_LED
#define CONFIG_BOARD_LED 1
#endif

#ifndef CONFIG_BOARD_WIFI
#define CONFIG_BOARD_WIFI 1
#endif

#ifndef CONFIG_BOARD_BT
#define CONFIG_BOARD_BT 1
#endif

#ifndef CONFIG_BOARD_RF_CMD
#define CONFIG_BOARD_RF_CMD 1
#endif

//#ifndef CONFIG_BOARD_AUDIO
//#define CONFIG_BOARD_AUDIO 1
//#endif

#ifndef CONFIG_BOARD_NSP_AUDIO
#define CONFIG_BOARD_NSP_AUDIO 1
#endif

#define BOARD_NAME "IoTGW_CB800"

/* the board pins, can be used as uart, gpio, pwd... */
#define BOARD_PIN0 (PA1)
#define BOARD_PIN1 (PA4)
#define BOARD_PIN2 (PA7)
#define BOARD_PIN3 (PB4)
#define BOARD_PIN4 (PB5)
#define BOARD_PIN5 (PB8)
#define BOARD_PIN6 (PB9)
#define BOARD_PIN7 (PB10)
#define BOARD_PIN8 (PB19)
#define BOARD_PIN9 (PB20)

//...

#if defined(CONFIG_BOARD_UART) && CONFIG_BOARD_UART
// UART

/*
    The total supported uart numbers on this board, 0 meas No uart support.
    the BOARD_UART<x>_XXX, x in rang of (0, BOARD_UART_NUM - 1)
*/
#ifndef BOARD_UART_NUM
#define BOARD_UART_NUM (2)
#endif

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0
/* the board uart0 tx pin */
#define BOARD_UART0_TX_PIN (BOARD_PIN8)
/* the borad uart0 rx pin */
#define BOARD_UART0_RX_PIN (BOARD_PIN9)
/* The real UART port reference to board logic port 0 */
#define BOARD_UART0_IDX (0)
/* The default baudrate for uart0 */
#define BOARD_UART0_BAUD (1000000)

/* the board uart1 tx pin */
#define BOARD_UART1_TX_PIN (BOARD_PIN3)
/* the borad uart1 rx pin */
#define BOARD_UART1_RX_PIN (BOARD_PIN4)
/* The real UART port reference to board logic port 0 */
#define BOARD_UART1_IDX (4)
/* The default baudrate for uart1 */
#define BOARD_UART1_BAUD (1000000)

#endif // defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0

#endif // defined(CONFIG_BOARD_UART) && CONFIG_BOARD_UART

#if defined(CONFIG_BOARD_GPIO) && CONFIG_BOARD_GPIO
// GPIO
/*
    The total supported GPIO Pin numbers on this board, 0 meas No uart support.
    the BOARD_GPIO_PIN<x>, x in rang of (0, BOARD_GPIO_PIN_NUM - 1)
*/
#ifndef BOARD_GPIO_PIN_NUM
#define BOARD_GPIO_PIN_NUM (2)
#endif

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
/* The real gpio reference to board logic gpio pin */
#define BOARD_GPIO_PIN0 (BOARD_PIN2)
#define BOARD_GPIO_PIN1 (BOARD_PIN3)
//#define BOARD_GPIO_PIN2                 (x)
//#define BOARD_GPIO_PIN3                 (x)
#endif // defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
#endif // defined(CONFIG_BOARD_GPIO) && CONFIG_BOARD_GPIO

#if defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM
// PWM
/* the board supported pwm channels */
#ifndef BOARD_PWM_NUM
#define BOARD_PWM_NUM (1)
#endif

#if defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
/* the board pwm pin */
#define BOARD_PWM0_PIN (BOARD_PIN2)
/* The real pwm channel reference to board logic pwm channel */
#define BOARD_PWM0_CH (4)

#endif // defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
#endif // defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM

#if defined(CONFIG_BOARD_ADC) && CONFIG_BOARD_ADC > 0
// ADC
/* the board supported adc channels */
#ifndef BOARD_ADC_NUM
#define BOARD_ADC_NUM (0)
#endif

#if defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
/* the board adc pin */
//#define BOARD_PWM0_PIN (BOARD_PIN6)
/* The real adc channel reference to board logic adc channel */
//#define BOARD_ADC0_CH  (0)
#endif // defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
#endif // defined(CONFIG_BOARD_ADC) && CONFIG_BOARD_ADC > 0

#if defined(CONFIG_BOARD_BUTTON) && CONFIG_BOARD_BUTTON > 0
// BUTTON
#ifndef BOARD_BUTTON_NUM
/*
    the board supported buttons, include gpio button and adc button,
    BOARD_BUTTON_NUM = BOARD_BUTTON_GPIO_NUM + BOARD_BUTTON_ADC_NUM.

*/
#define BOARD_BUTTON_NUM (1)
#endif

#if defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0

#define BOARD_BUTTON0_PIN (BOARD_PIN5)

// GPIO BUTTON
/* the board supported GPIO Buttons */
#ifndef BOARD_BUTTON_GPIO_NUM
#define BOARD_BUTTON_GPIO_NUM (1)
#endif

#if defined(BOARD_BUTTON_GPIO_NUM) && BOARD_BUTTON_GPIO_NUM > 0
/* the board logic button id, in range of (0, BOARD_BUTTON_GPIO_NUM - 1) */
#define BOARD_BUTTON0 (0)
/* for gpio button, define the pin numner. if the gpio pin used as gpio button, it shoudn't reference as BOARD_GPIO_PINx
 */
#define BOARD_BUTTON0_GPIO_PIN (BOARD_BUTTON0_PIN)
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

#endif // defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0

#if defined(CONFIG_BOARD_LED) && CONFIG_BOARD_LED > 0
// LED
/* the board supported leds */
#ifndef BOARD_LED_NUM
#define BOARD_LED_NUM (1)
#endif

#define BOARD_LED0_PIN BOARD_PIN2

// PWM LED
/* the board supported pwm leds */
#ifndef BOARD_LED_PWM_NUM
#define BOARD_LED_PWM_NUM (1)
#endif

#if defined(BOARD_LED_PWM_NUM) && BOARD_LED_PWM_NUM > 0
#define BOARD_LED0_PWM_PIN (BOARD_LED0_PIN)
/* the pwm channel used for led0, if the pwm channel used as led0, it shoudn't reference as BOARD_PWMx_CH */
#define BOARD_LED0_PWM_CH (4)
#endif // defined(BOARD_LED_PWM_NUM) && BOARD_LED_PWM_NUM > 0

// GPIO LED
#ifndef BOARD_LED_GPIO_NUM
#define BOARD_LED_GPIO_NUM (0)
#endif

#if defined(BOARD_LED_GPIO_NUM) && BOARD_LED_GPIO_NUM > 0
/* the gpio pin used for led0, if the gpio pin used as led, it shoudn't reference as BOARD_GPIO_PINx */
//#define BOARD_LED1_GPIO_PIN (BOARD_LED1_PIN)
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
#define CONFIG_CONSOLE_UART_BUFSIZE (128)
#endif

// AT uart config, Almost all solutions and demos use these.
#define CONFIG_AT_UART_IDX            (BOARD_UART1_IDX)
#define CONFIG_AT_UART_BAUD           (BOARD_UART1_BAUD)
#define CONFIG_AT_UART_NAME           "uart4"

/****************************************************************************/
// Commom test demos defines

/****************************************************************************/
// Vendor board defines

#if (defined(CONFIG_GW_SMARTLIVING_SUPPORT) && (CONFIG_GW_SMARTLIVING_SUPPORT)                                         \
     || defined(CONFIG_GW_FOTA_EN) && (CONFIG_GW_FOTA_EN)                                                              \
     || defined(CONFIG_SUPPORT_YMODEM) && (CONFIG_SUPPORT_YMODEM))
#define CONFIG_BOARD_OTA_SUPPORT 1
#endif

#ifndef BOARD_W800
#define BOARD_W800
#endif

/**
 * @brief  check if in factory mode.
 * re-implement if need.
 * @return
 */
int board_ftmode_check(void);

/**
 * @brief  gpio test on board.
 * re-implement if need.
 * @return
 */
int board_gpio_test(void);

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

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
