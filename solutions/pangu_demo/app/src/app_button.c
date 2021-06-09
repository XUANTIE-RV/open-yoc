#include "app_config.h"
#ifdef CONFIG_CSI_V2
#else
#include <pin_name.h>
#endif
#include <aos/aos.h>
#include <yoc/button.h>
#include "app_main.h"
#include "board.h"

#define TAG "app-button"

typedef struct _evt_data {
    int event_id;
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

typedef struct app_button_tbl {
    int event_id;
    button_evt_t button;
} app_button_tbl_t;

typedef enum {
    mute_press_up = 0,
    mute_press_long,
    inc_press_up,
    dec_press_up
} button_event_id_t;

typedef enum {
    button_mute = 0,
    button_inc,
    button_dec
} button_id_t;

static void button_event(int event_id, void *priv)
{
    LOGE(TAG, "evt[%d]", event_id);
    evt_data_t data;
    data.event_id = event_id;
    int ret = aos_queue_send(&s_queue, &data, sizeof(evt_data_t));

    if (ret < 0) {
        LOGE(TAG, "queue send failed");
    }
}

static void button_task_thread(void *arg)
{
    evt_data_t data;
    unsigned int len;

    while (1) {
        aos_queue_recv(&s_queue, AOS_WAIT_FOREVER, &data, &len);

        switch (data.event_id) {
            case mute_press_up:
                button_volume_mute();
                break;

            case mute_press_long: {
                LOGD(TAG, "go to wifi pair...");
                wifi_pair_start();
            }
                break;

            case inc_press_up:
                button_volume_inc();
                break;

            case dec_press_up:
                button_volume_dec();
                break;

            default:
                break;
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

static app_button_tbl_t b_tbl[] = {
    {mute_press_up, {button_mute, BUTTON_PRESS_UP, 0}},
    {mute_press_long, {button_mute, BUTTON_PRESS_LONG_DOWN, 5000}},
    {inc_press_up, {button_inc, BUTTON_PRESS_UP, 0}},
    {dec_press_up, {button_dec, BUTTON_PRESS_UP, 0}},
};

char *log[] = {
    "mute_press_up",
    "mute_press_long",
    "inc_press_up",
    "dec_press_up",
};
void app_button_init(void)
{
    /* 按键初始化 */
    button_task();
    button_init();

    button_add_gpio(button_mute, APP_KEY_MUTE, LOW_LEVEL);
    button_add_gpio(button_inc, APP_KEY_VOL_INC, LOW_LEVEL);
    button_add_gpio(button_dec, APP_KEY_VOL_DEC, LOW_LEVEL);

    for (int i = 0; i < sizeof(b_tbl) / sizeof(app_button_tbl_t); i++) {
        button_add_event(b_tbl[i].event_id, &b_tbl[i].button, 1, button_event, log[i]);
    }

}