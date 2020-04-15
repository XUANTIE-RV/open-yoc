#include "app_config.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <devices/devicelist.h>
#include "app_led.h"

#define Addr_GND 0x34

extern void mdelay(uint32_t ms);
extern void udelay(uint32_t us);
static void delay_us(uint32_t us)
{
	udelay(us);
}

static int g_led_bb_en = 0;
static int g_led_usage_en = 0;
static int Mode_Num = 0;
#define LED_HELP "USAGE:\n\tled [mode|open|close] [1|2|3|4]\n"
static void cmd_led_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc >= 2) {
        if (strcmp(argv[1], "mode") == 0) {
            int mode = argv[2] ? atoi(argv[2]) : 1;
            Mode_Num = mode;
        }
        else if (strcmp(argv[1], "open") == 0) {
            led_open();
            Mode_Num = 3;
        }
        else if (strcmp(argv[1], "close") == 0) {
            Mode_Num = 0;
            led_close();
        }
        else if (strcmp(argv[1], "tt") == 0) {
            // TODO: cpu usage
            int enable = argv[2] ? atoi(argv[2]) : 0;
            g_led_usage_en = enable;
        }
		else if (strcmp(argv[1], "bb") == 0) {
            // TODO: burst test
            int enable = argv[2] ? atoi(argv[2]) : 0;
            g_led_bb_en = enable;
        }
        return;
    }
    printf(LED_HELP);
}

void cli_reg_cmd_led(void)
{
    static const struct cli_command cmd_info = {
        "led",
        LED_HELP,
        cmd_led_func,
    };

    aos_cli_register_command(&cmd_info);
}

#define R_LED 0
#define G_LED 1
#define B_LED 2
const unsigned char  PWM_RGB[512]=
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x08,0x09,0x0b,0x0d,0x0f,0x11,0x13,0x16,
	0x1a,0x1c,0x1d,0x1f,0x22,0x25,0x28,0x2e,
	0x34,0x38,0x3c,0x40,0x44,0x48,0x4b,0x4f,
	0x55,0x5a,0x5f,0x64,0x69,0x6d,0x72,0x77,
	0x7d,0x80,0x88,0x8d,0x94,0x9a,0xa0,0xa7,
	0xac,0xb0,0xb9,0xbf,0xc6,0xcb,0xcf,0xd6,
	0xe1,0xe9,0xed,0xf1,0xf6,0xfa,0xfe,0xff,

	0xff,0xfe,0xfa,0xf6,0xf1,0xed,0xe9,0xe1,
	0xd6,0xcf,0xcb,0xc6,0xbf,0xb9,0xb0,0xac,
	0xa7,0xa0,0x9a,0x94,0x8d,0x88,0x80,0x7d,
	0x77,0x72,0x6d,0x69,0x64,0x5f,0x5a,0x55,
	0x4f,0x4b,0x48,0x44,0x40,0x3c,0x38,0x34,
	0x2e,0x28,0x25,0x22,0x1f,0x1d,0x1c,0x1a,
	0x16,0x13,0x11,0x0f,0x0d,0x0b,0x09,0x08,
	0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,
	
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,

	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x08,0x09,0x0b,0x0d,0x0f,0x11,0x13,0x16,
	0x1a,0x1c,0x1d,0x1f,0x22,0x25,0x28,0x2e,
	0x34,0x38,0x3c,0x40,0x44,0x48,0x4b,0x4f,
	0x55,0x5a,0x5f,0x64,0x69,0x6d,0x72,0x77,
	0x7d,0x80,0x88,0x8d,0x94,0x9a,0xa0,0xa7,
	0xac,0xb0,0xb9,0xbf,0xc6,0xcb,0xcf,0xd6,
	0xe1,0xe9,0xed,0xf1,0xf6,0xfa,0xfe,0xff,

	0xff,0xfe,0xfa,0xf6,0xf1,0xed,0xe9,0xe1,
	0xd6,0xcf,0xcb,0xc6,0xbf,0xb9,0xb0,0xac,
	0xa7,0xa0,0x9a,0x94,0x8d,0x88,0x80,0x7d,
	0x77,0x72,0x6d,0x69,0x64,0x5f,0x5a,0x55,
	0x4f,0x4b,0x48,0x44,0x40,0x3c,0x38,0x34,
	0x2e,0x28,0x25,0x22,0x1f,0x1d,0x1c,0x1a,
	0x16,0x13,0x11,0x0f,0x0d,0x0b,0x09,0x08,
	0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,
	
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,

	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x08,0x09,0x0b,0x0d,0x0f,0x11,0x13,0x16,
	0x1a,0x1c,0x1d,0x1f,0x22,0x25,0x28,0x2e,
	0x34,0x38,0x3c,0x40,0x44,0x48,0x4b,0x4f,
	0x55,0x5a,0x5f,0x64,0x69,0x6d,0x72,0x77,
	0x7d,0x80,0x88,0x8d,0x94,0x9a,0xa0,0xa7,
	0xac,0xb0,0xb9,0xbf,0xc6,0xcb,0xcf,0xd6,
	0xe1,0xe9,0xed,0xf1,0xf6,0xfa,0xfe,0xff,

	0xff,0xfe,0xfa,0xf6,0xf1,0xed,0xe9,0xe1,
	0xd6,0xcf,0xcb,0xc6,0xbf,0xb9,0xb0,0xac,
	0xa7,0xa0,0x9a,0x94,0x8d,0x88,0x80,0x7d,
	0x77,0x72,0x6d,0x69,0x64,0x5f,0x5a,0x55,
	0x4f,0x4b,0x48,0x44,0x40,0x3c,0x38,0x34,
	0x2e,0x28,0x25,0x22,0x1f,0x1d,0x1c,0x1a,
	0x16,0x13,0x11,0x0f,0x0d,0x0b,0x09,0x08,
	0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00
};

unsigned char RGB_32PWM_IS31FL3729[144]=
{
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X03,0x07,0x0D,0x16,0x1F,0x2E,0x40,0x4f,
	0X64,0x77,0x8D,0xA7,0xBF,0xD6,0xF1,0xFF,
	0XFF,0xF1,0xD6,0xBF,0xA7,0x8D,0x77,0X64,
	0x4f,0x40,0x2E,0x1F,0x16,0x0D,0x07,0X03,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X03,0x07,0x0D,0x16,0x1F,0x2E,0x40,0x4f,
	0X64,0x77,0x8D,0xA7,0xBF,0xD6,0xF1,0xFF,
	0XFF,0xF1,0xD6,0xBF,0xA7,0x8D,0x77,0X64,
	0x4f,0x40,0x2E,0x1F,0x16,0x0D,0x07,0X03,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X03,0x07,0x0D,0x16,0x1F,0x2E,0x40,0x4f,
	0X64,0x77,0x8D,0xA7,0xBF,0xD6,0xF1,0xFF,
	0XFF,0xF1,0xD6,0xBF,0xA7,0x8D,0x77,0X64,
	0x4f,0x40,0x2E,0x1F,0x16,0x0D,0x07,0X03,
};

void IS31FL3729_LED_Y_Samedata(uint8_t iColor, uint8_t iY1, uint8_t iX1)
{
    led_pwm_write(iColor, 1, iY1, iX1);
    led_pwm_write(iColor, 2, iY1, iX1);
    led_pwm_write(iColor, 3, iY1, iX1);
    led_pwm_write(iColor, 4, iY1, iX1);
    led_pwm_write(iColor, 5, iY1, iX1);
    led_pwm_write(iColor, 6, iY1, iX1);
    led_pwm_write(iColor, 7, iY1, iX1);
    led_pwm_write(iColor, 8, iY1, iX1);
    led_pwm_write(iColor, 9, iY1, iX1);
}

void IS31FL3729_LED_Y(uint8_t iColor,uint8_t iY1,uint8_t iX1,uint8_t iX2,uint8_t iX3,
    uint8_t iX4,uint8_t iX5,uint8_t iX6,uint8_t iX7,uint8_t iX8,uint8_t iX9)
{
	led_pwm_write(iColor,1,iY1,iX1);
	led_pwm_write(iColor,2,iY1,iX2);
	led_pwm_write(iColor,3,iY1,iX3);
	led_pwm_write(iColor,4,iY1,iX4);
	led_pwm_write(iColor,5,iY1,iX5);
	led_pwm_write(iColor,6,iY1,iX6);
	led_pwm_write(iColor,7,iY1,iX7);
	led_pwm_write(iColor,8,iY1,iX8);
	led_pwm_write(iColor,9,iY1,iX9);
}

uint8_t Mode_1(void)
{
    uint8_t i;

    uint8_t com1;
    uint8_t x = 128;
    uint8_t y = 64;
    uint8_t z = 8;

    for(i=0x01;i<=0x8F;i++)
	{
		led_write_byte(i,0x00);
	}
    while(Mode_Num == 1)
    {
        for(com1 = 0; com1 < 189; com1++)
        {
            IS31FL3729_LED_Y_Samedata(G_LED, 1, PWM_RGB[com1]);
            IS31FL3729_LED_Y_Samedata(R_LED, 1, PWM_RGB[com1 + x]);
            IS31FL3729_LED_Y_Samedata(B_LED, 1, PWM_RGB[com1 + y]);

            IS31FL3729_LED_Y_Samedata(G_LED, 2, PWM_RGB[com1 + z * 2]);
            IS31FL3729_LED_Y_Samedata(R_LED, 2, PWM_RGB[com1 + x + z * 2]);
            IS31FL3729_LED_Y_Samedata(B_LED, 2, PWM_RGB[com1 + y + z * 2]);

            IS31FL3729_LED_Y_Samedata(G_LED, 3, PWM_RGB[com1 + z * 4]);
            IS31FL3729_LED_Y_Samedata(R_LED, 3, PWM_RGB[com1 + x + z * 4]);
            IS31FL3729_LED_Y_Samedata(B_LED, 3, PWM_RGB[com1 + y + z * 4]);

            IS31FL3729_LED_Y_Samedata(G_LED, 4, PWM_RGB[com1 + z * 6]);
            IS31FL3729_LED_Y_Samedata(R_LED, 4, PWM_RGB[com1 + x + z * 6]);
            IS31FL3729_LED_Y_Samedata(B_LED, 4, PWM_RGB[com1 + y + z * 6]);

            IS31FL3729_LED_Y_Samedata(G_LED, 5, PWM_RGB[com1 + z * 8]);
            IS31FL3729_LED_Y_Samedata(R_LED, 5, PWM_RGB[com1 + x + z * 8]);
            IS31FL3729_LED_Y_Samedata(B_LED, 5, PWM_RGB[com1 + y + z * 8]);

            mdelay(2);

            if(Mode_Num != 1)return 0;
        }
    }
    return 1;
}

uint8_t Mode_2(void)
{
	uint8_t i;
	uint8_t p_temp;
	uint8_t pb1,pr1,pg1;
	pb1 = 0;
	pr1 = 16;
	pg1 = 32;
	
	while(Mode_Num == 2)
	{
		for(i=0;i<48;i++)
		{
            p_temp=i+pr1;
            IS31FL3729_LED_Y(R_LED,1,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1;
            IS31FL3729_LED_Y(B_LED,1,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1;
            IS31FL3729_LED_Y(G_LED,1,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+2;
            IS31FL3729_LED_Y(R_LED,2,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1+2;
            IS31FL3729_LED_Y(B_LED,2,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1+2;
            IS31FL3729_LED_Y(G_LED,2,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+4;
            IS31FL3729_LED_Y(R_LED,3,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1+4;
            IS31FL3729_LED_Y(B_LED,3,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1+4;
            IS31FL3729_LED_Y(G_LED,3,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+6;
            IS31FL3729_LED_Y(R_LED,4,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1+6;
            IS31FL3729_LED_Y(B_LED,4,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1+6;
            IS31FL3729_LED_Y(G_LED,4,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+8;
            IS31FL3729_LED_Y(R_LED,5,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1+8;
            IS31FL3729_LED_Y(B_LED,5,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1+8;
            IS31FL3729_LED_Y(G_LED,5,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+10;
            IS31FL3729_LED_Y(R_LED,6,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1+10;
            IS31FL3729_LED_Y(B_LED,6,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1+10;
            IS31FL3729_LED_Y(G_LED,6,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+12;
            IS31FL3729_LED_Y(R_LED,7,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1+12;
            IS31FL3729_LED_Y(B_LED,7,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1+12;
            IS31FL3729_LED_Y(G_LED,7,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+14;
            IS31FL3729_LED_Y(R_LED,8,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1+14;
            IS31FL3729_LED_Y(B_LED,8,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1+14;
            IS31FL3729_LED_Y(G_LED,8,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+16;
            IS31FL3729_LED_Y(R_LED,9,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pb1+16;
            IS31FL3729_LED_Y(B_LED,9,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pg1+16;
            IS31FL3729_LED_Y(G_LED,9,RGB_32PWM_IS31FL3729[24+p_temp],RGB_32PWM_IS31FL3729[21+p_temp],RGB_32PWM_IS31FL3729[18+p_temp],RGB_32PWM_IS31FL3729[15+p_temp],RGB_32PWM_IS31FL3729[12+p_temp],RGB_32PWM_IS31FL3729[9+p_temp],RGB_32PWM_IS31FL3729[6+p_temp],RGB_32PWM_IS31FL3729[3+p_temp],RGB_32PWM_IS31FL3729[0+p_temp]);
            p_temp=i+pr1+18;

            if (Mode_Num != 2) return 0;
            mdelay(50);
		}
	}
	return 1;
}

uint8_t Mode_3(void)
{
    uint8_t iCnt = 0;
    uint8_t x,y,k,i;
    uint8_t irand = rand()%9;
    uint8_t R_PWM = rand()%256;
    uint8_t G_PWM = rand()%256;
    uint8_t B_PWM = rand()%256;
    uint8_t ColorPWM[9][5][3];

    for(i=0x01;i<=0x8F;i++)
	{
		led_write_byte(i,0x00);
	}
	for (y=0;y<5;y++)//Init table
	{
		for (x=0;x<9;x++)
		{
			for(k=0;k<3;k++)
			{
				ColorPWM[x][y][k] = 0x00;
			}
		}
	}
	
	while(Mode_Num == 3)
	{

		if (iCnt == 3)
		{
			iCnt = 0;
			irand = rand()%9;//Get random numbers 
			R_PWM = rand()%256;
			G_PWM = rand()%256;
			B_PWM = rand()%256;
		}
		if(Mode_Num != 3)return 0;
		for (x=0;x<9;x++)//data transformation
		{
			if (irand == x)
			{
				if (iCnt == 0)
				{
					ColorPWM[x][4][0] = R_PWM;
					ColorPWM[x][4][1] = G_PWM;
					ColorPWM[x][4][2] = B_PWM;
				}
				else
				{
					if (iCnt == 1)
					{
						ColorPWM[x][4][0] = R_PWM/2;
						ColorPWM[x][4][1] = G_PWM/2;
						ColorPWM[x][4][2] = B_PWM/2;
					}
					else
					if(iCnt == 2)
					{
						ColorPWM[x][4][0] = R_PWM/8;
						ColorPWM[x][4][1] = G_PWM/8;
						ColorPWM[x][4][2] = B_PWM/8;
					}
					else
					{
						ColorPWM[x][4][0] = 0x00;
						ColorPWM[x][4][1] = 0x00;
						ColorPWM[x][4][2] = 0x00;
					}
				}				
			}
			else
			{
				ColorPWM[x][4][0] = 0x00;
				ColorPWM[x][4][1] = 0x00;
				ColorPWM[x][4][2] = 0x00;
			}		
		}
		iCnt++;
		for (x=0;x<9;x++)//update LED data
		{
			for (y=0;y<5;y++)
			{
				led_pwm_write(R_LED,x+1,y+1,ColorPWM[x][y][0]);
				led_pwm_write(G_LED,x+1,y+1,ColorPWM[x][y][1]);
				led_pwm_write(B_LED,x+1,y+1,ColorPWM[x][y][2]);
				if(Mode_Num != 3)return 0;
			}
		}
		mdelay(100);
		if(Mode_Num != 3)return 0;
		for (x=0;x<9;x++)//Data downward movement 
		{
			for (y=0;y<4;y++)
			{
				for (k=0;k<3;k++)
				{
					ColorPWM[x][y][k] = ColorPWM[x][y+1][k];
				}
			}
		}
		
	}
	return 1;
}

uint8_t Mode_4(void)
{
	uint8_t iCnt = 0;
	uint8_t x,y,k;

	uint8_t R_PWM = rand()%256;
	uint8_t G_PWM = rand()%256;
	uint8_t B_PWM = rand()%256;
	uint8_t ColorPWM[9][5][3];
    for (y=0;y<5;y++)//Init table
	{
		for (x=0;x<9;x++)
		{
			for(k=0;k<3;k++)
			{
				ColorPWM[x][y][k] = 0x00;
			}
		}
	}
	
	while(Mode_Num == 4)
	{
		for(iCnt=0;iCnt<15;iCnt++)
		{
			if (iCnt == 14)
			{
				R_PWM = rand()%256;
				G_PWM = rand()%256;
				B_PWM = rand()%256;
			}
			if(Mode_Num != 4)return 0;	
			for (y=0;y<5;y++)//data transformation
			{
				if (iCnt == 0)
				{
					ColorPWM[0][y][0] = R_PWM;
					ColorPWM[0][y][1] = G_PWM;
					ColorPWM[0][y][2] = B_PWM;
					delay_us(5);
				}
				else
				{
					if (iCnt == 1)
					{
						ColorPWM[0][y][0] = R_PWM/2;
						ColorPWM[0][y][1] = G_PWM/2;
						ColorPWM[0][y][2] = B_PWM/2;
						delay_us(5);
					}
					else
					if(iCnt == 2)
					{
						ColorPWM[0][y][0] = R_PWM/3;
						ColorPWM[0][y][1] = G_PWM/3;
						ColorPWM[0][y][2] = B_PWM/3;
						delay_us(50);
					}
					else
					if(iCnt == 3)
					{
						ColorPWM[0][y][0] = R_PWM/4;
						ColorPWM[0][y][1] = G_PWM/4;
						ColorPWM[0][y][2] = B_PWM/4;
						delay_us(5);
					}
					else
					if(iCnt == 4)
					{
						ColorPWM[0][y][0] = R_PWM/6;
						ColorPWM[0][y][1] = G_PWM/6;
						ColorPWM[0][y][2] = B_PWM/6;
						delay_us(5);
					}
					else
					if(iCnt == 5)
					{
						ColorPWM[0][y][0] = R_PWM/8;
						ColorPWM[0][y][1] = G_PWM/8;
						ColorPWM[0][y][2] = B_PWM/8;
						delay_us(5);
					}
					else
					{
						ColorPWM[0][y][0] = 0x00;
						ColorPWM[0][y][1] = 0x00;
						ColorPWM[0][y][2] = 0x00;
						delay_us(5);
					}
				}				
			}
			for (x=0;x<9;x++)//update data
			{
				for (y=0;y<5;y++)
				{
					led_pwm_write(R_LED,x+1,y+1,ColorPWM[x][y][0]);
					led_pwm_write(G_LED,x+1,y+1,ColorPWM[x][y][1]);
					led_pwm_write(B_LED,x+1,y+1,ColorPWM[x][y][2]);
					delay_us(5);
					
				}if(Mode_Num != 4)return 0;
			}
			if(Mode_Num != 4)return 0;
			
			for(y=0;y<5;y++)//Move data right 
			{
				for (x=8;x>0;x--)
				{
					for (k=0;k<3;k++)
					{
						ColorPWM[x][y][k] = ColorPWM[x-1][y][k];
						delay_us(5);
					}
				}
 			}
		}
		for(iCnt=0;iCnt<15;iCnt++)
		{
			if (iCnt == 14)
			{
				R_PWM = rand()%256;
				G_PWM = rand()%256;
				B_PWM = rand()%256;
			}
				
			for (y=0;y<5;y++)//data transformation
			{
				if (iCnt == 0)
				{
					ColorPWM[8][y][0] = R_PWM;
					ColorPWM[8][y][1] = G_PWM;
					ColorPWM[8][y][2] = B_PWM;
					delay_us(5);
				}
				else
				{
					if (iCnt == 1)
					{
						ColorPWM[8][y][0] = R_PWM/2;
						ColorPWM[8][y][1] = G_PWM/2;
						ColorPWM[8][y][2] = B_PWM/2;
						delay_us(5);
					}
					else
					if(iCnt == 2)
					{
						ColorPWM[8][y][0] = R_PWM/3;
						ColorPWM[8][y][1] = G_PWM/3;
						ColorPWM[8][y][2] = B_PWM/3;
						delay_us(5);
					}
					else
					if(iCnt == 3)
					{
						ColorPWM[8][y][0] = R_PWM/4;
						ColorPWM[8][y][1] = G_PWM/4;
						ColorPWM[8][y][2] = B_PWM/4;
						delay_us(5);
					}
					else
					if(iCnt == 4)
					{
						ColorPWM[8][y][0] = R_PWM/6;
						ColorPWM[8][y][1] = G_PWM/6;
						ColorPWM[8][y][2] = B_PWM/6;
						delay_us(5);
					}
					else
					if(iCnt == 5)
					{
						ColorPWM[8][y][0] = R_PWM/8;
						ColorPWM[8][y][1] = G_PWM/8;
						ColorPWM[8][y][2] = B_PWM/8;
						delay_us(5);
					}
					else
					{
						ColorPWM[8][y][0] = 0x00;
						ColorPWM[8][y][1] = 0x00;
						ColorPWM[8][y][2] = 0x00;
						delay_us(5);
					}
				}				
			}
			for (x=0;x<9;x++)//update data
			{
				for (y=0;y<5;y++)
				{
					led_pwm_write(R_LED,x+1,y+1,ColorPWM[x][y][0]);
					led_pwm_write(G_LED,x+1,y+1,ColorPWM[x][y][1]);
					led_pwm_write(B_LED,x+1,y+1,ColorPWM[x][y][2]);
					delay_us(5);
					if(Mode_Num != 4)return 0;
				}
			}
			if(Mode_Num != 4)return 0;
			for (x=0;x<8;x++)//Move data left 
			{
				for (y=0;y<5;y++)
				{
					for (k=0;k<3;k++)
					{
						ColorPWM[x][y][k] = ColorPWM[x+1][y][k];
						delay_us(5);
					}
				}
 			}
  	    }
		mdelay(1000);
	}
	return 1;
}

uint8_t Mode_5(void)
{
	uint8_t buffer[85];
    int cc = 0;

	while(Mode_Num == 5) {
        if (cc++ % 2 == 0) {
            for (int i = 0; i < sizeof(buffer); i++) {
                buffer[i] = 0x05;
            }             
        } else {
            for (int i = 0; i < sizeof(buffer); i++) {
                buffer[i] = 0x99;
            }   
        }
		if (g_led_bb_en)
			led_write_frame(buffer);
		else {
        
            led_write_frame2(buffer);
        }
		    
		aos_msleep(3000);
	}
	return 0;
} 

static void led_usage(void *argv)
{
    int i, j;
    led_open();
    long long a, b;
    int cc = 0;

    while(1) {
        a = 0;
        b = 0;
        if (g_led_usage_en) {
            a = aos_now_ms();
#if 0
            for (j = 0; j < 10; j++) {
                if ((cc++ % 2) == 0) {
                    for(i=0x01;i<=85;i++)
                    {
                        led_write_byte(i,0xff);
                    }
                } else {
                    for(i=0x01;i<=85;i++)
                    {
                        led_write_byte(i,0);
                    }
                }
            }
#else
			uint8_t buffer[85];
			if ((cc++ % 2) == 0) {
				for(i=0;i<85;i++)
				{
					buffer[i] = 0xff;
				}
			} else {
				for(i=0;i<85;i++)
				{
					buffer[i] = 0;
				}
			}
			for (j = 0; j < 10; j++) {
				led_write_frame(buffer);
			}
#endif
            b = aos_now_ms();
        }
        printf("##: %lld,[%lld, %lld]\n", b - a, a, b);
        aos_msleep(1000);
    }
}

void led_test_cpu_usage_task(void)
{
    aos_task_t task;
    aos_task_new_ext(&task, "led_cpuusage", led_usage, NULL, 2*1024, 10);
}

static void led_task(void *argv)
{
    led_open();
    Mode_Num = 5;
	while(1)
	{
		switch(Mode_Num)
		{
			case 1:
                Mode_1();break;//DEMO mode
            case 2:
                Mode_2();break;
            case 3:
                Mode_3();break;
            case 4:
                Mode_4();break;
			case 5:
				Mode_5();break;
			default: break;
		}
        aos_msleep(500);
	}
}

#include "app_main.h"
#include "pin.h"
#include "pin_name.h"
#include <devices/devicelist.h>
void led_test_task(void)
{
    aos_task_t task;
    // TODO:/*LED*/
    drv_pinmux_config(USI0_SCLK, 0);
    drv_pinmux_config(USI0_SD0, 0);
    iic_csky_register(0);
    led_init();

    // FIXME: 优先级不能太高
    aos_task_new_ext(&task, "led_test_task", led_task, NULL, 6*1024, 40);
	cli_reg_cmd_led();
}