#include "switches_report.h"
#include "aos/kernel.h"
#include "mesh_node.h"
#include <api/mesh.h>
#include "sig_model/generic_onoff_srv.h"
//#include "mesh/access.h"

typedef struct _switch_report_s {
    uint8_t report_index;
    aos_timer_t report_timer;
} _switch_report_t;

_switch_report_t g_report_config;

_report_onoff_messages_t g_report[3];

#define TAG "SWITCH_REPORT"

int switches_report_start(uint8_t element_idx, uint8_t onoff)
{
    int ret;
    struct bt_mesh_model *onoff_srv_model;

    /* Find Generic OnOff Srv Model in device primary element */
    onoff_srv_model = ble_mesh_model_find(element_idx, BT_MESH_MODEL_ID_GEN_ONOFF_SRV, 0xFFFF);

    if (!onoff_srv_model) {
        LOGE(TAG, "Not Support!");
        return -1;
    }

    /* publish on or off command to destinated address */
    ret = ble_mesh_generic_onoff_publication(onoff_srv_model, onoff);
    if (ret) {
        LOGE(TAG, "Gen OnOff Pub err %d", ret);
        return ret;
    }

    return 0;
}



int switches_report_stop()
{
#if 0
    aos_timer_stop(&g_report_config.report_timer);
    return 0;
#endif
    return 0;
}

static void report_timeout(void* timer, void* arg)
{
#if 0
    uint8_t onoff = g_report[g_report_config.report_index].onoff_status;
    report_event_to_cloud(onoff, g_report_config.report_index);
    aos_timer_stop(&g_report_config.report_timer);
#endif
}


int switches_report_init()
{
#if 0
    int ret = 0;
    g_report_config.report_index = 0XFF;
    ret = aos_timer_new_ext(&g_report_config.report_timer, report_timeout, NULL,REPORT_DEFAULT_DELAY, 1,0);
    if (ret) {
        printf("report timer init failed");
    }
#endif
    return 0;
}

