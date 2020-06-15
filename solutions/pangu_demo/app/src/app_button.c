#include "app_config.h"
#include <pin_name.h>
#include <aos/aos.h>
#include <yoc/button.h>
#include "app_main.h"
#include "pin.h"

#define TAG "app-button"

typedef struct _evt_data{
    int event_id;
    char name[MAX_BUTTON_NAME];
} evt_data_t;
#define MESSAGE_NUM 10
static uint8_t s_q_buffer[sizeof(evt_data_t) * MESSAGE_NUM];
static aos_queue_t s_queue;

static void volume_inc(void)
{
    app_volume_inc(0);
}

static void volume_dec(void)
{
    app_volume_dec(0);
}

static void volume_mute(void)
{
    app_volume_mute();
}

/*************************************************
 * 驱动事件处理
 *************************************************/

void button_volume_inc()
{
    LOGD(TAG, "%s", __func__);
    volume_inc();
}

void button_volume_dec()
{
    LOGD(TAG, "%s", __func__);
    volume_dec();
}

void button_volume_mute()
{
    LOGD(TAG, "%s", __func__);
    volume_mute();
}

static void button_evt(button_evt_id_t event_id, char *name, void *priv)
{
    LOGE(TAG, "evt[%d] key[%s]", event_id, name);
    evt_data_t data;
    data.event_id = event_id;
    strlcpy(data.name, name, MAX_BUTTON_NAME);
    int ret = aos_queue_send(&s_queue, &data, sizeof(evt_data_t));
    if (ret < 0) {
        LOGE(TAG, "queue send failed");
    }
}

static void bc_evt(button_evt_id_t event_id, char *name, void *priv)
{
    LOGE(TAG, "bc evt[%s]", name);
}
// FIXME: BUTTON配置和上报的事件ID需要统一
const static button_config_t button_table[] = {
    {APP_KEY_MUTE,    EVT_ALL_FLAG, button_evt, NULL, BUTTON_TYPE_GPIO,  "mute"},
    {APP_KEY_VOL_INC, EVT_ALL_FLAG, button_evt, NULL, BUTTON_TYPE_GPIO,  "inc"},
    {APP_KEY_VOL_DEC, EVT_ALL_FLAG, button_evt, NULL, BUTTON_TYPE_GPIO,  "dec"},
    {0, 0, NULL, NULL},
};

const static button_combinations_t bc_table[] = {
    {
        .pin_name[0] = "mute",
        .pin_name[1] = "inc",
        .evt_flag = PRESS_LONG_DOWN_FLAG,
        .pin_sum = 2,
        .tmout = 500,
        .cb = bc_evt,
        .priv = NULL,
        .name = "mute&inc"
    },
    {
        .pin_name[0] = "mute",
        .pin_name[1] = "dec",
        .evt_flag = PRESS_LONG_DOWN_FLAG,
        .pin_sum = 2,
        .tmout = 500,
        .cb = bc_evt,
        .priv = NULL,
        .name = "mute&dec"
    },
    {
        .pin_sum = 0,
        .cb = NULL,
    },
};

static void button_task_thread(void *arg)
{
    evt_data_t data;
    unsigned int len;

    while (1) {
        aos_queue_recv(&s_queue, AOS_WAIT_FOREVER, &data, &len);
        
        if (strcmp(data.name, "mute") == 0) {
            if (data.event_id == BUTTON_PRESS_LONG_DOWN) {
                LOGD(TAG, "go to wifi pair...");
                wifi_pair_start();
            } else if (data.event_id == BUTTON_PRESS_UP) {
                button_volume_mute();
            }
        } else if (strcmp(data.name, "inc") == 0) {
            if (data.event_id == BUTTON_PRESS_UP)
                button_volume_inc();
        } else if (strcmp(data.name, "dec") == 0) {
            if (data.event_id == BUTTON_PRESS_UP)
                button_volume_dec();
        }
    }
}

static void button_task(void)
{
    aos_task_t task;
    
    int ret = aos_queue_new(&s_queue, s_q_buffer, MESSAGE_NUM * sizeof(evt_data_t), sizeof(evt_data_t));
    aos_check(!ret, EIO);
    aos_task_new_ext(&task, "b-press", button_task_thread, NULL, 4096, AOS_DEFAULT_APP_PRI + 4);
}

void app_button_init(void)
{
    /* 按键初始化 */
    button_task();
    button_srv_init();
    button_init(button_table);
    button_param_t pb;
    button_param_cur("mute", &pb);
    pb.ld_tmout = 5000;
    button_param_set("mute", &pb);
    button_param_set("inc", &pb);
    button_param_set("dec", &pb);
    button_combination_init(bc_table);
}