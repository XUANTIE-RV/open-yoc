#ifndef _SWITCHES_REPORT_H
#define _SWITCHES_REPORT_H
#include "types.h"

#define REPORT_DEFAULT_DELAY (500) //uint ms

typedef enum {
    ONOFF_MESSAGES,
} report_message_type_e;

typedef struct _report_onoff_messages_s {
    uint8_t elem_id;
    uint8_t onoff_status;
} _report_onoff_messages_t;


int switches_report_init();
int switches_report_start(uint8_t type, void* arg, uint16_t timeout);
int switches_report_stop();




#endif

