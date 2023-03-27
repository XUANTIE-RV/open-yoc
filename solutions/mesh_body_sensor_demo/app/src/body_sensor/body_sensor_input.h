#ifndef __BODY_SENSOR_INPUT_H__
#define __BODY_SENSOR_INPUT_H__

#include "body_sensor_gpio.h"
#include "mesh_sal_lpm.h"

#define INPUT_IO_NUM (1)
#define INPUT_EVENT_PIN_1 (P14)
#define INPUT_PIN_POL_PIN_1 (FALLING)

#define INPUT_EVENT_KEY_CHECK_TIMEROUT (50)         //unit ms
#define INPUT_EVENT_KEY_LONG_PRESS (50)             //uint 50* 100ms
#define INPUT_EVENT_CHECK_PROV_TIMEROUT (10 * 1000) //unit ms

#define INPUT_EVENT_PROVISION_PRESS_TIMEROUT (10 * 1000)
#define INPUT_EVENT_SLEEP_PRESS_TIMEROUT (30 * 1000)

#define INPUT_EVENT_BODY_SENSOR_KEEP_TIME (20)

#define SECONDS(t) (t * 1000)

typedef enum _body_sensor_input_type
{
    BODY_SENSOR_INPUT_NONE,
    BODY_SENSOR_INPUT_ONCE,
    BODY_SENSOR_INPUT_LONG,
} body_sensor_input_type_e;

typedef enum _key_status_s
{
    KEY_RELEASED,
    KEY_PRESSED
} key_status_e;

typedef struct _sb_input_event_s
{
    uint8_t event;
    body_sensor_input_type_e input_type;
    bool do_once;
} sb_input_event_t;

int32_t input_event_init(void);

#endif
