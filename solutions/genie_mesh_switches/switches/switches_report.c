#include "switches_report.h"
#include "aos/kernel.h"
#include "genie_transport.h"

typedef struct _switch_report_s
{
  uint8_t report_index;
  aos_timer_t report_timer;
} _switch_report_t;

_switch_report_t g_report_config;

_report_onoff_messages_t g_report[3];

#if 0
int switches_report_start(uint8_t type, void* arg, uint16_t timeout)
{
    if(!arg) {
        return -1;
    }

    aos_timer_stop(&g_report_config.report_timer);
    if(timeout != REPORT_DEFAULT_DELAY) {
        aos_timer_change(&g_report_config.report_timer, timeout);
    }
    if (type == ONOFF_MESSAGES) {
        _report_onoff_messages_t* data = (_report_onoff_messages_t*)arg;
        g_report[data->elem_id].elem_id = data->elem_id;
        g_report[data->elem_id].onoff_status = data->onoff_status;
        g_report_config.report_index = g_report[data->elem_id].elem_id;
    } else {
        return -1;
    }
    aos_timer_start(&g_report_config.report_timer);
    return 0;
}

int switches_report_stop()
{
    aos_timer_stop(&g_report_config.report_timer);
    return 0;
}

static void report_timeout(void* timer, void* arg)
{
    uint8_t onoff = g_report[g_report_config.report_index].onoff_status;
    report_event_to_cloud(onoff, g_report_config.report_index);
    aos_timer_stop(&g_report_config.report_timer);
}


int switches_report_init()
{
    int ret = 0;
    g_report_config.report_index = 0XFF;
    ret = aos_timer_new_ext(&g_report_config.report_timer, report_timeout, NULL,REPORT_DEFAULT_DELAY, 1,0);
    if (ret) {
        printf("report timer init faild\r\n");
    }
}
#endif
