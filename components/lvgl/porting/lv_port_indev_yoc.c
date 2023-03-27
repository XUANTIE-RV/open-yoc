/**
 * @file lv_port_indev_yoc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <assert.h>
#include <devices/input.h>

#include "../../lvgl.h"

#define TAG "TOUCH"

static rvm_dev_t *touch_dev;

static void touchpad_init(void)
{
    touch_dev = rvm_hal_input_open("input");
    assert(touch_dev);
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    rvm_hal_input_event_t     tevent;
    int               res;
    static uint8_t    down   = false;
    static uint8_t    moving = false;
    static lv_coord_t x      = 0;
    static lv_coord_t y      = 0;

    while (1) {
        res = rvm_hal_input_read(touch_dev, &tevent, sizeof(tevent), 0);
        if (res < (int)sizeof(tevent)) {
            data->point.x = x;
            data->point.y = y;

            if (down || moving) {
                data->state = LV_INDEV_STATE_PR;
            } else {
                data->state = LV_INDEV_STATE_REL;
            }

            data->continue_reading = 0;

            // LOGD(TAG, "end %d %d %d", x, y, data->state);
            // LOGE(TAG, "could not get event res=%d", res);
            break;
        }

        data->continue_reading = 1;

        if (tevent.type == EV_ABS) {
            if (tevent.code == ABS_X) {
                x = tevent.value;
            } else if (tevent.code == ABS_Y) {
                y = tevent.value;
            }
        } else if (tevent.type == EV_SYN && tevent.code == SYN_REPORT) {
            if (down) {
                down   = false;
                moving = true;

                data->point.x = x;
                data->point.y = y;

                data->state = LV_INDEV_STATE_PR;

                // LOGD(TAG, "%d %d %d", x, y, data->state);

                break;
            }

            if (moving) {

                data->point.x = x;
                data->point.y = y;

                data->state = LV_INDEV_STATE_PR;

                // LOGD(TAG, "%d %d %d", x, y, data->state);
                break;
            }
        } else if (tevent.type == EV_KEY && tevent.code == BTN_TOUCH) {
            if (tevent.value == 1) {
                down = true;
            } else if (tevent.value == 0) {
                down   = false;
                moving = false;

                data->point.x = x;
                data->point.y = y;

                data->state = LV_INDEV_STATE_REL;

                // LOGD(TAG, "%d %d %d", x, y, data->state);
                break;
            }
        }
    }
}

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);
}
