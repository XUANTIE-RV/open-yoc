/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <assert.h>
#include <stdio.h>

#include <aos/kernel.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <devices/display.h>
#include <devices/input.h>

#include "board.h"

#define TAG "app"

#define HELP_USAGE                                                                                                     \
    "usage:\r\n"                                                                                                       \
    "\tdisplay brightness [num]\r\n"                                                                                      \
    "\tdisplay blank [0/1]\r\n"                                                                                           \
    "\tdisplay write [red color num] [green color num] [blue color num]\r\n"                                              \
    "\tdisplay write_async [red color num] [green color num] [blue color num]\r\n"                                        \
    "\tdisplay pan_dispaly [red color num] [green color num] [blue color num]\r\n"

static rvm_dev_t *    disp_dev;
static rvm_dev_t *    touch_dev;
static rvm_hal_display_info_t disp_info;
static void *         show_buffer;

static char *pixel_format_str[] = {
    "ARGB_8888",
    "ABGR_8888",
    "RGB_888",
    "BGR_888",
    "RGB_565",
    "RGB_655",
    "RGB_556",
    "BGR_565",
    "ARGB_1555",
    "ARGB_4444",
    "RGB_444",
    "RGB_332",
    "RGB_233",
    "RGB_323",
    "MONO_8",
    "MONO_4",
    "MONO_1",
};

/*
static char *event_str[] = {
    "EVENT_WRITE_DONE",
    "EVENT_FLUSH_DONE",
    "EVENT_READ_DONE",
};
*/

extern void board_yoc_init(void);

static void disp_event(rvm_dev_t *dev, int event_id, void *arg)
{
    //printf("%s event_id %s\n", __func__, event_str[event_id]);
}

static void touchscreen_event(rvm_dev_t *dev, int event_id, void *arg)
{
    rvm_hal_input_event_t   tevent;
    int             res;
    static uint8_t  down   = false;
    static uint8_t  moving = false;
    static uint16_t x      = 0;
    static uint16_t y      = 0;

    while (1) {
        res = rvm_hal_input_read(dev, &tevent, sizeof(tevent), 0);
        if (res < (int)sizeof(tevent)) {
            break;
        }

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

                LOGD(TAG, "touch pressed x: %d y: %d", x, y);
            }

            if (moving) {

                LOGD(TAG, "touch move x: %d y: %d", x, y);
            }
        } else if (tevent.type == EV_KEY && tevent.code == BTN_TOUCH) {
            if (tevent.value == 1) {
                down = true;
            } else if (tevent.value == 0) {
                down   = false;
                moving = false;

                LOGD(TAG, "touch unpressed x: %d y: %d", x, y);
            }
        }
    }
}

static void full_area(void *data_buffer, uint8_t r, uint8_t g, uint8_t b)
{
    LOGD(TAG, "show area");

    if (disp_info.pixel_format == DISPLAY_PIXEL_FORMAT_ARGB_8888) {
        uint32_t  color  = ((0xff) << 24) | ((r << 16) + (g << 8) + b);
        uint32_t *buffer = data_buffer;
        for (int i = 0; i < disp_info.x_res * disp_info.y_res; i++) {
            *buffer++ = color;
        }
    } else if (disp_info.pixel_format == DISPLAY_PIXEL_FORMAT_RGB_565) {
        uint16_t  color  = (r << 11) + (g << 6) + b;
        uint16_t *buffer = data_buffer;
        for (int i = 0; i < disp_info.x_res * disp_info.y_res; i++) {
            *buffer++ = color;
        }
    } else {
        LOGE(TAG, "no full area");
    }
}

static void disp_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc < 2) {
        printf("%s\n", HELP_USAGE);
        return;
    }

    // cmd parse
    if ((strcmp(argv[1], "brightness") == 0)) {
        if (argc < 3) {
            printf("%s\n", HELP_USAGE);
            return;
        }

        int brightness = atoi(argv[2]);

        rvm_hal_display_set_brightness(disp_dev, brightness);
    } else if ((strcmp(argv[1], "blank") == 0)) {
        if (argc < 3) {
            printf("%s\n", HELP_USAGE);
            return;
        }

        int on_off = atoi(argv[2]);

        rvm_hal_display_blank_on_off(disp_dev, on_off);
    } else if (strcmp(argv[1], "write") == 0) {
        if (argc < 5) {
            printf("%s\n", HELP_USAGE);
            return;
        }

        uint8_t r = atoi(argv[2]);
        uint8_t g = atoi(argv[3]);
        uint8_t b = atoi(argv[4]);

        full_area(show_buffer, r, g, b);

        rvm_hal_display_area_t area = {
            .x_start = 0,
            .y_start = 0,
            .x_leght = disp_info.x_res,
            .y_leght = disp_info.y_res,
        };

        /** wirte buffer to display area */
        rvm_hal_display_write_area(disp_dev, &area, show_buffer);
    } else if (strcmp(argv[1], "write_async") == 0) {
        if (argc < 5) {
            printf("%s\n", HELP_USAGE);
            return;
        }

        uint8_t r = atoi(argv[2]);
        uint8_t g = atoi(argv[3]);
        uint8_t b = atoi(argv[4]);

        full_area(show_buffer, r, g, b);

        rvm_hal_display_area_t area = {
            .x_start = 0,
            .y_start = 0,
            .x_leght = disp_info.x_res,
            .y_leght = disp_info.y_res,
        };

        /** wirte buffer to display area */
        rvm_hal_display_write_area_async(disp_dev, &area, show_buffer);
    } else if (strcmp(argv[1], "pan_display") == 0) {
        if (argc < 5) {
            printf("%s\n", HELP_USAGE);
            return;
        }

        uint8_t r = atoi(argv[2]);
        uint8_t g = atoi(argv[3]);
        uint8_t b = atoi(argv[4]);

        if (disp_info.supported_feature & DISPLAY_FEATURE_ONE_FB) {
            full_area(disp_info.smem_start[0], r, g, b);
        } else if (disp_info.supported_feature & DISPLAY_FEATURE_DOUBLE_FB) {
            static uint8_t count = 0;
            full_area(disp_info.smem_start[(count ++) % 2], r, g, b);
        } else {
            assert(0);
        }

        rvm_hal_display_pan_display(disp_dev);
    } else {
        printf("%s\n", HELP_USAGE);
    }
    return;
}

void cli_reg_cmd_disp(void)
{
    static const struct cli_command cmd_info = {
        "display",
        "display cmd",
        disp_cmd,
    };
    aos_cli_register_command(&cmd_info);
}

int main(int argc, char *argv[])
{
    board_yoc_init();
    cli_reg_cmd_disp();

    touch_dev = rvm_hal_input_open("input");
    assert(touch_dev);

    rvm_hal_input_set_event(touch_dev, touchscreen_event, NULL);

    disp_dev = rvm_hal_display_open("disp");
    assert(disp_dev);

    rvm_hal_display_get_info(disp_dev, &disp_info);

    rvm_hal_display_set_event(disp_dev, disp_event, NULL);

    LOGD(TAG, "disp info x_res %d, y_res %d, bits_per_pixel: %d, supported_feature: %d, pixel_format: %s",
         disp_info.x_res, disp_info.y_res, disp_info.bits_per_pixel, disp_info.supported_feature,
         pixel_format_str[disp_info.pixel_format]);

    show_buffer = malloc(disp_info.y_res * disp_info.x_res * disp_info.bits_per_pixel / 8);

    if (show_buffer == NULL) {
        return -1;
    }
}