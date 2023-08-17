/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#if defined(CONFIG_CHIP_D1) && CONFIG_CHIP_D1
#if defined(CONFIG_BOARD_DISPLAY) && CONFIG_BOARD_DISPLAY
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <aos/kernel.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <devices/device.h>
#include <devices/driver.h>
#include <devices/display.h>
#include <devices/input.h>
#include <devices/devicelist.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "board.h"

#define TAG "DISPLAY_INPUT"

#define HELP_USAGE                                                                                                     \
    "usage:\r\n"                                                                                                       \
    "\tdisplay brightness [num]\r\n"                                                                                      \
    "\tdisplay blank [0/1]\r\n"                                                                                           \
    "\tdisplay write [red color num] [green color num] [blue color num]\r\n"                                              \
    "\tdisplay write_async [red color num] [green color num] [blue color num]\r\n"                                        \
    "\tdisplay pan_dispaly [red color num] [green color num] [blue color num]\r\n"

static rvm_hal_display_info_t disp_info;
static void *         show_buffer;
static int            fd_input;
static int            fd_disp;

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

static void disp_event(rvm_dev_t *dev, int event_id, void *arg)
{
    //printf("%s event_id %s\n", __func__, event_str[event_id]);
}

static void touchscreen_event(rvm_dev_t *dev, int event_id, void *arg)
{
    rvm_hal_input_event_t   tevent;
    static uint8_t  down   = false;
    static uint8_t  moving = false;
    static uint16_t x      = 0;
    static uint16_t y      = 0;
    int             ret    = -1;

    while (1) {
        unsigned int read_timeout_ms = 0;
        ioctl(fd_input, INPUT_IOC_READ_TIMEOUT_MS, &read_timeout_ms);

        ret = read(fd_input, &tevent, sizeof(tevent));
        if (ret < (int)sizeof(tevent)) {
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
    rvm_display_dev_msg_t msg_disp;

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
        ioctl(fd_disp, DISPLAY_IOC_SET_BRIGHTNESS, &brightness);

        ioctl(fd_disp, DISPLAY_IOC_GET_BRIGHTNESS, &msg_disp);
        if (*msg_disp.brightness != brightness) {
            printf("DISPLAY_IOC_GET_BRIGHTNESS err\n");
        }   
    } else if ((strcmp(argv[1], "blank") == 0)) {
        if (argc < 3) {
            printf("%s\n", HELP_USAGE);
            return;
        }

        int on_off = atoi(argv[2]);

        ioctl(fd_disp, DISPLAY_IOC_BLANK_ON_OFF, &on_off);
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
        msg_disp.area = &area;
        msg_disp.data = show_buffer;
        ioctl(fd_disp, DISPLAY_IOC_WRITE_AREA, &msg_disp);

        ioctl(fd_disp, DISPLAY_IOC_READ_AREA, &msg_disp);
        if (msg_disp.data != show_buffer) {
            printf("DISPLAY_IOC_READ_AREA err\n");
        }
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
        msg_disp.area = &area;
        msg_disp.data = show_buffer;
        ioctl(fd_disp, DISPLAY_IOC_WRITE_AREA_ASYNC, &msg_disp);
    } else if (strcmp(argv[1], "pan_display") == 0) {
        if (argc < 5) {
            printf("%s\n", HELP_USAGE);
            return;
        }

        uint8_t r = atoi(argv[2]);
        uint8_t g = atoi(argv[3]);
        uint8_t b = atoi(argv[4]);

        full_area(disp_info.smem_start[0], r, g, b);

        ioctl(fd_disp, DISPLAY_IOC_PAN_DISPLAY);
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

int devfs_display_input_demo(void)
{
    int ret = -1;
    
    board_display_init();

#if defined(CONFIG_BOARD_VENDOR) && CONFIG_BOARD_VENDOR
    board_vendor_init();
#endif

    cli_reg_cmd_disp();

    char *inputdev = "/dev/input0";
    fd_input = open(inputdev, O_RDWR);
    printf("open input fd:%d\n", fd_input);
    if (fd_input < 0) {
        printf("open /dev/input failed. fd:%d\n", fd_input);
        return -1;
    }

    rvm_input_dev_msg_t msg_input;
    msg_input.event_cb = touchscreen_event;
    msg_input.priv = NULL;

    ret = ioctl(fd_input, INPUT_IOC_SET_EVENT, &msg_input);
    if (ret < 0) {
        printf("INPUT_IOC_SET_EVENT fail !\n");
        goto failure_input;
    }

    char *dispdev = "/dev/disp0";
    fd_disp = open(dispdev, O_RDWR);
    printf("open disp fd:%d\n", fd_disp);
    if (fd_disp < 0) {
        printf("open /dev/disp failed. fd:%d\n", fd_disp);
        return -1;
    }

    ret = ioctl(fd_disp, DISPLAY_IOC_GET_INFO, &disp_info);
    if (ret < 0) {
        printf("DISPLAY_IOC_GET_INFO fail !\n");
        goto failure_disp;
    }

    rvm_display_dev_msg_t msg_disp;
    msg_disp.event_cb = disp_event;
    msg_disp.priv = NULL;

    ret = ioctl(fd_disp, DISPLAY_IOC_SET_EVENT, &msg_disp);
    if (ret < 0) {
        printf("DISPLAY_IOC_SET_EVENT fail !\n");
        goto failure_disp;
    }

    unsigned int **smem_start;
    size_t smem_len = 0;
    msg_disp.smem_len = &smem_len;
    msg_disp.smem_start = (void ***)&smem_start;
    ret = ioctl(fd_disp, DISPLAY_IOC_GET_FRAMEBUFFER, &msg_disp);
    // disp_info.smem_start[0]  这个里面存储的是frambuf的地址；460800 = WINDOW_X_RES * WINDOW_Y_RES * BYTE_PRE_PIXEL
    // 这俩信息从驱动获取
    if (ret < 0 && smem_start[0] != disp_info.smem_start[0] && smem_len != 460800) {
        printf("DISPLAY_IOC_SET_EVENT fail !\n");
        goto failure_disp;
    }

    LOGD(TAG, "disp info x_res %d, y_res %d, bits_per_pixel: %d, supported_feature: %d, pixel_format: %s",
         disp_info.x_res, disp_info.y_res, disp_info.bits_per_pixel, disp_info.supported_feature,
         pixel_format_str[disp_info.pixel_format]);

    show_buffer = malloc(disp_info.y_res * disp_info.x_res * disp_info.bits_per_pixel / 8);

    if (show_buffer == NULL) {
        return -1;
    }

    return 0;

failure_input:
    close(fd_input);
    return -1;
failure_disp:
    close(fd_disp);
    return -1;
}

#endif
#endif /*CONFIG_BOARD_DISPLAY*/
#endif