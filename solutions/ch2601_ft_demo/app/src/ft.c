/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include "drv/gpio_pin.h"
#include <drv/pin.h>
#include <drv/pwm.h>
#include "drv/codec.h"
#include "drv/dma.h"
#include "lv_conf.h"
#include "lvgl.h"
#include "src/lv_hal/lv_hal.h"
#include <drv/gpio_pin.h>
#include <drv/ringbuf.h>
#include "es8156.h"
#include <yoc/netmgr_service.h>

#include "main.h"
#define TAG "ft"

static csi_pwm_t  r;

uint32_t g_lcd         = 0;
uint32_t g_led         = 0;
uint32_t g_led_refresh = 0;

void led_refresh_high()
{
        csi_pwm_out_start(&r, 7 / 2);
        csi_pwm_out_start(&r, 2 / 2);
        csi_pwm_out_start(&r, 4 / 2);
}

void led_refresh_low()
{
        csi_pwm_out_stop(&r, 2 / 2);
        csi_pwm_out_stop(&r, 7 / 2);
        csi_pwm_out_stop(&r, 4 / 2);
}

void led_pinmux_init()
{
        //7
    csi_error_t ret;
    csi_pin_set_mux(PA7, PA7_PWM_CH7);
    csi_pin_set_mux(PA25, PA25_PWM_CH2);
    csi_pin_set_mux(PA4, PA4_PWM_CH4);
    ret = csi_pwm_init(&r, 0);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
    ret = csi_pwm_out_config(&r, 7 / 2, 300, 100, PWM_POLARITY_HIGH);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
        //25
    ret = csi_pwm_out_config(&r, 2 / 2, 300, 100, PWM_POLARITY_HIGH);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
        //4
    ret = csi_pwm_out_config(&r, 4 / 2, 300, 100, PWM_POLARITY_HIGH);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
}

void ft_lcd(void)
{
    lv_obj_t *p = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(p, LV_LABEL_LONG_BREAK);
    lv_label_set_align(p, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_pos(p, 0, 4);
    lv_obj_set_size(p, 128, 60);
    lv_label_set_text(p, "THEAD RISV-V\nFT\nDEMO");
}
static void cmd_ft_lcd_handler(char *wbuf, int wbuf_len, int argc, char **argv)
{
    printf("lcd %s\n", argv[1]);
    g_lcd = 1;

	 if (g_lcd == 1) {
		lv_task_handler();
	}

}

static void cmd_ft_btn_handler(char *wbuf, int wbuf_len, int argc, char **argv)
{
    g_led = 1;
}

extern netmgr_hdl_t app_netmgr_hdl;
static void cmd_ft_wifi_handler(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 3 && argv[1] != NULL && argv[2] != NULL) {
        LOGD(TAG, "ssid:%s, psk:%s", argv[1], argv[2]);
        netmgr_config_wifi(app_netmgr_hdl, argv[1], strlen(argv[1]), argv[2], strlen(argv[2]));
        netmgr_start(app_netmgr_hdl);        
    }
}

csi_codec_t codec;
csi_codec_output_t codec_output_ch;
csi_codec_input_t  codec_input_ch;

csi_dma_ch_t dma_ch_output_handle;
csi_dma_ch_t dma_ch_input_handle;
#define OUTPUT_BUF_SIZE 2048
uint8_t output_buf[OUTPUT_BUF_SIZE];

#define INPUT_BUF_SIZE  2048
uint8_t input_buf[INPUT_BUF_SIZE];

csi_ringbuf_t output_ring_buffer;
csi_ringbuf_t input_ring_buffer;

volatile uint32_t cb_output_transfer_flag = 0;
volatile uint32_t cb_input_transfer_flag = 0;

volatile uint32_t new_data_flag = 0U;
volatile uint32_t old_data_flag = 0U;

uint8_t repeater_data_addr[49152];

extern es8156_dev_t es8156_dev;

static void codec_output_event_cb_fun(csi_codec_output_t *output, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        cb_output_transfer_flag += 1;
    }
}

static void codec_input_event_cb_fun(csi_codec_input_t *i2s, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        cb_input_transfer_flag += 1;
    }
}

static void cmd_ft_mic_handler(uint32_t channel_status)
{
    csi_error_t ret;
    csi_codec_output_config_t output_config;
    csi_codec_input_config_t input_config;
    ret = csi_codec_init(&codec, 0);

    if (ret != CSI_OK) {
        printf("csi_codec_init error\n");
        return ;
    }

    codec_output_ch.ring_buf = &output_ring_buffer;
    csi_codec_output_open(&codec, &codec_output_ch, 0);
    /* output ch config */
    csi_codec_output_attach_callback(&codec_output_ch, codec_output_event_cb_fun, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = 8000;
    output_config.buffer = output_buf;
    output_config.buffer_size = OUTPUT_BUF_SIZE;
    output_config.period = 1024;
    output_config.mode = CODEC_OUTPUT_SINGLE_ENDED;
    csi_codec_output_config(&codec_output_ch, &output_config);
    csi_codec_output_analog_gain(&codec_output_ch, 0xbf);
    csi_codec_output_buffer_reset(&codec_output_ch);

    csi_codec_output_link_dma(&codec_output_ch, &dma_ch_output_handle);

    codec_input_ch.ring_buf = &input_ring_buffer;
    csi_codec_input_open(&codec, &codec_input_ch, 0);
    /* input ch config */
    csi_codec_input_attach_callback(&codec_input_ch, codec_input_event_cb_fun, NULL);
    input_config.bit_width = 16;
    input_config.sample_rate = 8000;
    input_config.buffer = input_buf;
    input_config.buffer_size = INPUT_BUF_SIZE;
    input_config.period = 1024;
    input_config.mode = CODEC_INPUT_DIFFERENCE;
    csi_codec_input_config(&codec_input_ch, &input_config);
    csi_codec_input_analog_gain(&codec_input_ch, 0xbf);
    csi_codec_input_link_dma(&codec_input_ch, &dma_ch_input_handle);

    if (channel_status == 0) {
        es8156_all_data_left_channel(&es8156_dev);
    } else {
        es8156_all_data_right_channel(&es8156_dev);
    }

    printf("start repeater\n");
    csi_codec_output_start(&codec_output_ch);
    csi_codec_input_start(&codec_input_ch);

    uint32_t times = 0U;

    while (1) {
        while (new_data_flag < 48) {
            if (cb_input_transfer_flag) {
                csi_codec_input_read_async(&codec_input_ch, repeater_data_addr + (new_data_flag * 1024), 1024);
                cb_input_transfer_flag = 0U;
                new_data_flag ++;
            }
        }

        times ++;
        csi_codec_output_mute(&codec_output_ch, false);
        new_data_flag = 0;
        csi_codec_output_write_async(&codec_output_ch, repeater_data_addr + (old_data_flag * 1024), 1024);
        old_data_flag ++;

        while (old_data_flag < 48) {
            if (cb_output_transfer_flag) {
                csi_codec_output_write_async(&codec_output_ch, repeater_data_addr + (old_data_flag * 1024), 1024);
                old_data_flag ++;
                cb_output_transfer_flag = 0;
            }
        }

        old_data_flag = 0;
        memset(output_buf, 0, sizeof(output_buf));
        csi_codec_output_mute(&codec_output_ch, true);

        if (times == 10) {
            break;
        }
    }

    printf("stop repeater\n");
    csi_codec_input_stop(&codec_input_ch);
    csi_codec_output_stop(&codec_output_ch);

    csi_codec_input_link_dma(&codec_input_ch, NULL);
    csi_codec_output_link_dma(&codec_output_ch, NULL);

    csi_codec_output_detach_callback(&codec_output_ch);
    csi_codec_input_detach_callback(&codec_input_ch);

    csi_codec_uninit(&codec);

    return;
}

static void cmd_ft_micl_handler(char *wbuf, int wbuf_len, int argc, char **argv)
{
    cmd_ft_mic_handler(0);
}

static void cmd_ft_micr_handler(char *wbuf, int wbuf_len, int argc, char **argv)
{
    cmd_ft_mic_handler(1);
}

int cli_reg_cmd_ft(void)
{
    static const struct cli_command led_cmd_info = {
        "AT+BTN",
        "AT BTN",
        cmd_ft_btn_handler,
    };

    aos_cli_register_command(&led_cmd_info);
    static const struct cli_command lcd_cmd_info = {
        "AT+LCD",
        "AT LCD",
        cmd_ft_lcd_handler,
    };
    aos_cli_register_command(&lcd_cmd_info);

    static const struct cli_command wifi_cmd_info = {
        "AT+WIFI",
        "AT WIFI",
        cmd_ft_wifi_handler,
    };
    aos_cli_register_command(&wifi_cmd_info);

    static const struct cli_command micl_cmd_info = {
        "AT+MICL",
        "AT MICL",
        cmd_ft_micl_handler,
    };
    aos_cli_register_command(&micl_cmd_info);

    static const struct cli_command micr_cmd_info = {
        "AT+MICR",
        "AT MICR",
        cmd_ft_micr_handler,
    };

    aos_cli_register_command(&micr_cmd_info);

    return 0;
}

static void gpio_pin_callback(csi_gpio_pin_t *pin, void *arg)
{

    if (g_led) {
        if (pin->pin_idx == PA11) {
            g_led_refresh = 1;
        } else if (pin->pin_idx == PA12) {
            g_led_refresh = 0;
            led_refresh_low();
        }
        ft_led_refresh();
    }
}

csi_gpio_pin_t g_handle;
csi_gpio_pin_t g_handle2;
int            ft_btn_init()
{
    memset(&g_handle, 0, sizeof(g_handle));
    csi_pin_set_mux(PA11, PIN_FUNC_GPIO);
    csi_gpio_pin_init(&g_handle, PA11);
    csi_gpio_pin_dir(&g_handle, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_mode(&g_handle, GPIO_MODE_PULLUP);
    csi_gpio_pin_debounce(&g_handle, true);
    csi_gpio_pin_attach_callback(&g_handle, gpio_pin_callback, &g_handle);
    csi_gpio_pin_irq_mode(&g_handle, GPIO_IRQ_MODE_FALLING_EDGE);
    csi_gpio_pin_irq_enable(&g_handle, true);
    //2
    csi_pin_set_mux(PA12, PIN_FUNC_GPIO);
    csi_gpio_pin_init(&g_handle2, PA12);
    csi_gpio_pin_dir(&g_handle2, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_mode(&g_handle2, GPIO_MODE_PULLUP);
    csi_gpio_pin_debounce(&g_handle2, true);
    csi_gpio_pin_attach_callback(&g_handle2, gpio_pin_callback, &g_handle2);
    csi_gpio_pin_irq_mode(&g_handle2, GPIO_IRQ_MODE_FALLING_EDGE);
    csi_gpio_pin_irq_enable(&g_handle2, true);

    return 0;
}

uint32_t g_ctr = 0;
void     ft_led_refresh()
{
    if (g_led_refresh) {
        g_ctr++;
        if (g_ctr == 3) {
                g_ctr = 0;
        }
        if (g_ctr == 0)
        {
            csi_pwm_out_start(&r, 7 / 2);
            csi_pwm_out_stop(&r, 2 / 2);
            csi_pwm_out_stop(&r, 4 / 2);

        }
        else if (g_ctr == 1)
        {
            csi_pwm_out_start(&r, 2 / 2);
            csi_pwm_out_stop(&r, 7 / 2);
            csi_pwm_out_stop(&r, 4 / 2);

        }
        else //2
        {
            csi_pwm_out_start(&r, 4 / 2);
            csi_pwm_out_stop(&r, 7 / 2);
            csi_pwm_out_stop(&r, 2 / 2);
        }
    }

}
