#ifndef __SWITCHES_INPUT_H__
#define __SWITCHES_INPUT_H__
#include "switches_gpio.h"

#define INPUT_IO_NUM (3)
#define INPUT_EVENT_PIN_1 (P14)
#define INPUT_EVENT_PIN_2 (P15)
#define INPUT_EVENT_PIN_3 (P24)
#define INPUT_PIN_POL_PIN_1 (FALLING)
#define INPUT_PIN_POL_PIN_2 (FALLING)
#define INPUT_PIN_POL_PIN_3 (FALLING)

#define INPUT_EVENT_KEY_CHECK_TIMEROUT (50)         //unit ms
#define INPUT_EVENT_KEY_LONG_PRESS (50)             //uint 50* 100ms
#define INPUT_EVENT_CHECK_PROV_TIMEROUT (10 * 1000) //unit ms

#define INPUT_EVENT_PROVISION_PRESS_TIMEROUT (10 * 1000)
#define INPUT_EVENT_SLEEP_PRESS_TIMEROUT (30 * 1000)

#define INPUT_EVENT_SWTICH_KEEP_TIME (20)

#define SECONDS(t) (t * 1000)

typedef enum _switch_press_type
{
    SWITCH_PRESS_NONE,
    SWITCH_PRESS_ONCE,
    SWITCH_PRESS_LONG,
} switch_press_type_e;

typedef enum _key_status_s
{
    KEY_RELEASED,
    KEY_PRESSED
} key_status_e;

typedef struct _sb_input_event_s
{
    uint8_t event;
    switch_press_type_e press_type;
    bool do_once;
} sb_input_event_t;

int32_t input_event_init(void);
void input_event_check_key_state(uint8_t port);

#endif
