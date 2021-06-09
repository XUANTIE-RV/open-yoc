#ifndef __NODE_CTL_H__
#define __NODE_CTL_H__

#include "board_config.h"

/* unprovision device beacon adv time */
#define MESH_PBADV_TIME 600 * 1000 //10 minutes

#define MESH_ELEM_COUNT 1
#define ELEMENT_NUM MESH_ELEM_COUNT

#define NODE_CTL_LED_PIN (PIN_LED_R)

typedef enum _led_status
{
    LED_ON,
    LED_OFF
} led_status_e;

#endif
