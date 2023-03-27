/*
 * disp_board_config.c
 *
 * Copyright (c) 2007-2020 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <soc.h>

typedef enum
{
    D1_GPIO_DIRECTION_INPUT  = 0,              /**<  GPIO input direction. */
    D1_GPIO_DIRECTION_OUTPUT = 1               /**<  GPIO output direction. */
} gpio_direction_t;

enum REGULATOR_TYPE_ENUM{
	AXP2101_REGULATOR,
	PWM_REGULATOR,
	GPIO_REGULATOR,
};

enum REGULATOR_ID_ENUM {
	AXP2101_ID_DCDC1 = 0,
	AXP2101_ID_DCDC2,
	AXP2101_ID_DCDC3,
	AXP2101_ID_DCDC4,
	AXP2101_ID_DCDC5,
	AXP2101_ID_ALDO1,
	AXP2101_ID_ALDO2,
	AXP2101_ID_ALDO3,
	AXP2101_ID_ALDO4,
	AXP2101_ID_BLDO1,
	AXP2101_ID_BLDO2,
	AXP2101_ID_DLDO1,
	AXP2101_ID_DLDO2,
	AXP2101_ID_CPUSLDO,
	AXP2101_ID_MAX,
};

typedef enum
{
    PWM_POLARITY_INVERSED = 0,
    PWM_POLARITY_NORMAL = 1,
} hal_pwm_polarity;

struct disp_gpio_set_t {
	char gpio_name[32];
	uint32_t port;
	uint32_t port_num;
	uint32_t mul_sel;
	uint32_t pull;
	uint32_t drv_level;
	uint32_t data;
	uint32_t gpio;
};

typedef struct pwm_config
{
    uint32_t        duty_ns;
    uint32_t        period_ns;
    hal_pwm_polarity    polarity;
} pwm_config_t;

struct disp_pwm_dev {
	uint32_t pwm_channel_id;
	struct pwm_config cfg;
	bool enable;
};

/**
 * disp_power_t
 */
struct disp_power_t {
	char power_name[32];
	/*see sunxi_hal_regulator.h */
	enum REGULATOR_TYPE_ENUM power_type;
	enum REGULATOR_ID_ENUM power_id;
	/*unit:uV, 1V=1000000uV */
	uint32_t power_vol;
	bool always_on;
};

enum proerty_type {
	PROPERTY_UNDEFINED = 0,
	PROPERTY_INTGER,
	PROPERTY_STRING,
	/*for the pins which function is GPIO(IN or OUT)*/
	PROPERTY_GPIO,
	/*for the pins which function is not GPIO*/
	PROPERTY_PIN,
	/*for axp power*/
	PROPERTY_POWER,
};

/**
 * pro
 */
struct property_t {
	char name[40];
	enum proerty_type type;
	union value_t {
		uint32_t value;
		char str[40];
		struct disp_gpio_set_t gpio_list;
		struct disp_power_t power;
	} v;
};


struct property_t g_lcd0_config[] = {
	
	#if 1
	{
		.name = "lcd_used",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_driver_name",
		.type = PROPERTY_STRING,
		.v.str = "re_tft720p",
	},
	{
		.name = "lcd_backlight",
		.type = PROPERTY_INTGER,
		.v.value = 100,
	},
	{
		.name = "lcd_if",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
	{
		.name = "lcd_x",
		.type = PROPERTY_INTGER,
		.v.value = 720,
	},
	{
		.name = "lcd_y",
		.type = PROPERTY_INTGER,
		.v.value = 720,
	},
	{
		.name = "lcd_width",
		.type = PROPERTY_INTGER,
		.v.value = 84,
	},
	{
		.name = "lcd_height",
		.type = PROPERTY_INTGER,
		.v.value = 84,
	},
	{
		.name = "lcd_dclk_freq",
		.type = PROPERTY_INTGER,
		.v.value = 35,
	},
	{
		.name = "lcd_pwm_used",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_pwm_ch",
		.type = PROPERTY_INTGER,
		.v.value = 7,
	},
	{
		.name = "lcd_pwm_freq",
		.type = PROPERTY_INTGER,
		.v.value = 5000,
	},
	{
		.name = "lcd_pwm_pol",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_pwm_max_limit",
		.type = PROPERTY_INTGER,
		.v.value = 255,
	},
	{
		.name = "lcd_hbp",
		.type = PROPERTY_INTGER,
		.v.value = 26,
	},
	{
		.name = "lcd_ht",
		.type = PROPERTY_INTGER,
		.v.value = 790,
	},
	{
		.name = "lcd_hspw",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
	{
		.name = "lcd_vbp",
		.type = PROPERTY_INTGER,
		.v.value = 25,
	},
	{
		.name = "lcd_vt",
		.type = PROPERTY_INTGER,
		.v.value = 755,
	},
	{
		.name = "lcd_vspw",
		.type = PROPERTY_INTGER,
		.v.value = 6,
	},

	{
		.name = "lcd_dsi_if",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_dsi_lane",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
	{
		.name = "lcd_lvds_if",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_lvds_colordepth",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_lvds_mode",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_frm",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_hv_clk_phase",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_hv_sync_polarity",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_io_phase",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_gamma_en",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_bright_curve_en",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_cmap_en",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_fsync_en",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_fsync_act_time",
		.type = PROPERTY_INTGER,
		.v.value = 1000,
	},
	{
		.name = "lcd_fsync_dis_time",
		.type = PROPERTY_INTGER,
		.v.value = 1000,
	},
	{
		.name = "lcd_fsync_pol",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},

	{
		.name = "deu_mode",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcdgamma4iep",
		.type = PROPERTY_INTGER,
		.v.value = 22,
	},
	{
		.name = "smart_color",
		.type = PROPERTY_INTGER,
		.v.value = 90,
	},
	
	#endif

	//
	{
		#if 0
		.name = "lcd_gpio_0", // DSI-RESET
		.type = PROPERTY_GPIO,
		.v.gpio_list = {
			.gpio = GPIOG(13),
			.mul_sel = D1_GPIO_DIRECTION_OUTPUT,
			.pull = 0,
			.drv_level = 3,
			.data = 1,
		},
		#endif
		
		#if 1
		.name = "lcd_gpio_0", // DSI-RESET
		.type = PROPERTY_GPIO,
		.v.gpio_list = {
			.gpio = PD21,
			.mul_sel = D1_GPIO_DIRECTION_OUTPUT,
			.pull = 0,
			.drv_level = 3,
			.data = 1,
		},
		#endif
	},

    //gpio
	{
		.name = "DSI-D0P",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD0,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-D0N",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD1,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-D1P",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD2,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-D1N",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD3,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-CKP",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD4,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-CKN",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD5,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-D2P",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD6,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-D2N",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD7,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-D3P",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD8,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},
	{
		.name = "DSI-D3N",
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD9,
			.mul_sel = 4,
			.pull = 0,
			.drv_level = 3,
		},
	},

	{
		#if 0
		.name = "PWM2", // BL-PWM
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD18),
			.mul_sel = 5,
			.pull = 0,
			.drv_level = 3,
		},
		#endif
		
		#if 1
		.name = "PWM7", // BL-PWM
		.type = PROPERTY_PIN,
		.v.gpio_list = {
			.gpio = PD22,
			.mul_sel = 5,
			.pull = 0,
			.drv_level = 3,
		},
		#endif		
		
	},
};

struct property_t g_lcd1_config[] = {
	{
		.name = "lcd_used",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
};

struct property_t g_disp_config[] = {
	{
		.name = "disp_init_enable",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "disp_mode",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "screen0_output_type",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "screen0_output_mode",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
	{
		.name = "screen1_output_type",
		.type = PROPERTY_INTGER,
		.v.value = 3,
	},
	{
		.name = "screen1_output_mode",
		.type = PROPERTY_INTGER,
		.v.value = 10,
	},
    {
		.name = "disp_rotation_used",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
    {
		.name = "degree0",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
    {
		.name = "fb0_format",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	
	#if 1
    {
		.name = "fb0_width",
		.type = PROPERTY_INTGER,
		.v.value = 720,
	},
    {
		.name = "fb0_height",
		.type = PROPERTY_INTGER,
		.v.value = 720,
	},
	#endif
	
};

uint32_t g_lcd0_config_len = sizeof(g_lcd0_config) / sizeof(struct property_t);
uint32_t g_lcd1_config_len = sizeof(g_lcd1_config) / sizeof(struct property_t);
uint32_t g_disp_config_len = sizeof(g_disp_config) / sizeof(struct property_t);
