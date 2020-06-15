#include "app_config.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <devices/devicelist.h>
#include <devices/iic.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include "drv/iic.h"
#include <drv/gpio.h>
#include "pin_name.h"
#include "pin.h"

#define Addr_GND 0x34
#define R 0x80
#define G 0x80
#define B 0x80

static aos_dev_t *iic_dev;
static gpio_pin_handle_t pgpio_pin_handle;
extern void mdelay(uint32_t ms);

void led_write_byte(uint8_t reg_addr,uint8_t reg_data)
{
    uint8_t data[2] = {0};

    data[0] = reg_addr;
    data[1] = reg_data;

    iic_master_send(iic_dev, Addr_GND, data, 2, -1);
}

static void led_burst_write_bytes(uint8_t reg_start_addr, uint8_t reg_data[95])
{
    uint8_t databuf[100];

    if (reg_data) {
        databuf[0] = reg_start_addr;
        memcpy(&databuf[1], reg_data, 95);
        iic_master_send(iic_dev, Addr_GND, databuf, 96, -1);
    }
}

void sdb_pimmux_init(void)
{
    drv_pinmux_config(LED_USE_PIN, PIN_FUNC_GPIO);
    pgpio_pin_handle = csi_gpio_pin_initialize(LED_USE_PIN, NULL);
    csi_gpio_pin_config_mode(pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
}

void sdb_output(bool value)
{
    csi_gpio_pin_write(pgpio_pin_handle, value);
}

void Init_3729(uint8_t Rdata, uint8_t Gdata, uint8_t Bdata)
{
    int i;

    led_write_byte(0xA0,0x00);

    for(i=0x92;i<0x9f;i+=3)
    {
        led_write_byte(i,Rdata);//R LED Scaling
    }

    for(i=0x91;i<0x9f;i+=3)
    {
        led_write_byte(i,Gdata);//G LED Scaling
    }

    for(i=0x90;i<0x9f;i+=3)
    {
        led_write_byte(i,Bdata);//B LED Scaling
    }

    for(i=0x01;i<0x8f;i++)
    {
        led_write_byte(i,0x00);//write all PWM set 0x00
    }
    led_write_byte(0xA1, 0x7F);//global current
    led_write_byte(0xA0, 0x01);//normal operation
    printf("Init 3729 over\n");
}

/*reg_addr
01 02 03 04 05 06 07 08 09 10 11 12 13 14 81 86 91
17 18 19 20 21 22 23 24 25 26 27 28 29 30 82 87 92
33 34 35 36 37 38 39 40 41 42 43 44 45 46 83 88 93
49 50 51 52 53 54 55 56 57 58 59 60 61 62 84 89 94
65 66 67 68 69 70 71 72 73 74 75 76 77 78 85 90 95
*/
void led_write_frame2(uint8_t buffer[85])
{
#define TOTAL_COL 6
#define TOTAL_ROW 14
#define PWM_REG_BASE 0x01
#define PWM_REG_COL_OFFSET 0x10
#define LED_NUM_PER_ROW 5

    int i, j;
    int col = 0;
    int count = 0;
    uint8_t regaddr;
    uint8_t reg_addr[85];

    if (buffer) {
        for (i = 0; i < TOTAL_ROW; i++) {
            for (col = 0; col < LED_NUM_PER_ROW; col++) {
                regaddr = PWM_REG_BASE + i + col * PWM_REG_COL_OFFSET;
                reg_addr[count] = regaddr;
                count ++;
            }            
        }
        for (i = 0; i < TOTAL_ROW + 1; i++) {
            regaddr = PWM_REG_BASE + i + col * PWM_REG_COL_OFFSET;
            reg_addr[count] = regaddr;
            count ++;
        }

        // write iic
        count = 0;
        for (i = 0; i <= 4; i++) {
            for (j = 0; j <= 16; j++) {
                led_write_byte(reg_addr[j * 5 + i], buffer[count++]);
            }
        }
    }
}

void led_write_frame(uint8_t buffer[85])
{
    uint8_t re_buffer[128];
    int i, j, count;

    memset(re_buffer, 0, sizeof(re_buffer));
    count = 0;

    memcpy(&re_buffer[0], &buffer[0], 14);
    memcpy(&re_buffer[16], &buffer[17], 14);
    memcpy(&re_buffer[16*2], &buffer[17*2], 14);
    memcpy(&re_buffer[16*3], &buffer[17*3], 14);
    memcpy(&re_buffer[16*4], &buffer[17*4], 14);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 5; j++) {
            re_buffer[80 + count] = buffer[j * 17 + i + 14];
            count++;            
        }
    }
    led_burst_write_bytes(0x01, re_buffer);
}

#define R_LED 0
#define G_LED 1
#define B_LED 2
void led_pwm_write(uint8_t iColor, uint8_t iX, uint8_t iY, uint8_t idata)
{
    switch(iColor)
    {
    case R_LED:
        switch(iX)
        {
        case 1 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x03, idata);
                break;
            case 2:
                led_write_byte(0x06, idata);
                break;
            case 3:
                led_write_byte(0x09, idata);
                break;
            case 4:
                led_write_byte(0x0C, idata);
                break;
            case 5:
                led_write_byte(0x0F, idata);
                break;

            default:
                break;
            }
            break;
        case 2 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x13, idata);
                break;
            case 2:
                led_write_byte(0x16, idata);
                break;
            case 3:
                led_write_byte(0x19, idata);
                break;
            case 4:
                led_write_byte(0x1c, idata);
                break;
            case 5:
                led_write_byte(0x1f, idata);
                break;

            default:
                break;
            }
            break;
        case 3 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x23, idata);
                break;
            case 2:
                led_write_byte(0x26, idata);
                break;
            case 3:
                led_write_byte(0x29, idata);
                break;
            case 4:
                led_write_byte(0x2c, idata);
                break;
            case 5:
                led_write_byte(0x2f, idata);
                break;

            default:
                break;
            }
            break;
        case 4 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x33, idata);
                break;
            case 2:
                led_write_byte(0x36, idata);
                break;
            case 3:
                led_write_byte(0x39, idata);
                break;
            case 4:
                led_write_byte(0x3c, idata);
                break;
            case 5:
                led_write_byte(0x3f, idata);
                break;

            default:
                break;
            }
            break;
						
        case 5 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x43, idata);
                break;
            case 2:
                led_write_byte(0x46, idata);
                break;
            case 3:
                led_write_byte(0x49, idata);
                break;
            case 4:
                led_write_byte(0x4c, idata);
                break;
            case 5:
                led_write_byte(0x4f, idata);
                break;

            default:
                break;
            }
            break;
						
        case 6 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x53, idata);
                break;
            case 2:
                led_write_byte(0x56, idata);
                break;
            case 3:
                led_write_byte(0x59, idata);
                break;
            case 4:
                led_write_byte(0x5c, idata);
                break;
            case 5:
                led_write_byte(0x5f, idata);
                break;

            default:
                break;
            }
            break;
						
        case 7 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x63, idata);
                break;
            case 2:
                led_write_byte(0x66, idata);
                break;
            case 3:
                led_write_byte(0x69, idata);
                break;
            case 4:
                led_write_byte(0x6c, idata);
                break;
            case 5:
                led_write_byte(0x6f, idata);
                break;
						
            default:
                break;
            }
            break;
						
        case 8 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x73, idata);
                break;
            case 2:
                led_write_byte(0x76, idata);
                break;
            case 3:
                led_write_byte(0x79, idata);
                break;
            case 4:
                led_write_byte(0x7c, idata);
                break;
            case 5:
                led_write_byte(0x7f, idata);
                break;

            default:
                break;
            }
            break;

        case 9 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x83, idata);
                break;
            case 2:
                led_write_byte(0x86, idata);
                break;
            case 3:
                led_write_byte(0x89, idata);
                break;
            case 4:
                led_write_byte(0x8c, idata);
                break;
            case 5:
                led_write_byte(0x8f, idata);
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
        break;
    case G_LED:
        switch(iX)
        {
        case 1 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x02, idata);
                break;
            case 2:
                led_write_byte(0x05, idata);
                break;
            case 3:
                led_write_byte(0x08, idata);
                break;
            case 4:
                led_write_byte(0x0B, idata);
                break;
            case 5:
                led_write_byte(0x0E, idata);
                break;

            default:
                break;
            }
            break;

        case 2 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x12, idata);
                break;
            case 2:
                led_write_byte(0x15, idata);
                break;
            case 3:
                led_write_byte(0x18, idata);
                break;
            case 4:
                led_write_byte(0x1b, idata);
                break;
            case 5:
                led_write_byte(0x1e, idata);
                break;

            default:
                break;
            }
            break;
        case 3 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x22, idata);
                break;
            case 2:
                led_write_byte(0x25, idata);
                break;
            case 3:
                led_write_byte(0x28, idata);
                break;
            case 4:
                led_write_byte(0x2b, idata);
                break;
            case 5:
                led_write_byte(0x2e, idata);
                break;

            default:
                break;
            }
            break;
        case 4 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x32, idata);
                break;
            case 2:
                led_write_byte(0x35, idata);
                break;
            case 3:
                led_write_byte(0x38, idata);
                break;
            case 4:
                led_write_byte(0x3b, idata);
                break;
            case 5:
                led_write_byte(0x3e, idata);
                break;

            default:
                break;
            }
            break;
						
		case 5 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x42, idata);
                break;
            case 2:
                led_write_byte(0x45, idata);
                break;
            case 3:
                led_write_byte(0x48, idata);
                break;
            case 4:
                led_write_byte(0x4b, idata);
                break;
            case 5:
                led_write_byte(0x4e, idata);
                break;

            default:
                break;
            }
            break;
						
		case 6 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x52, idata);
                break;
            case 2:
                led_write_byte(0x55, idata);
                break;
            case 3:
                led_write_byte(0x58, idata);
                break;
            case 4:
                led_write_byte(0x5b, idata);
                break;
            case 5:
                led_write_byte(0x5e, idata);
                break;

            default:
                break;
            }
            break;
						
		case 7 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x62, idata);
                break;
            case 2:
                led_write_byte(0x65, idata);
                break;
            case 3:
                led_write_byte(0x68, idata);
                break;
            case 4:
                led_write_byte(0x6b, idata);
                break;
            case 5:
                led_write_byte(0x6e, idata);
                break;

            default:
                break;
            }
            break;
						
		case 8 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x72, idata);
                break;
            case 2:
                led_write_byte(0x75, idata);
                break;
            case 3:
                led_write_byte(0x78, idata);
                break;
            case 4:
                led_write_byte(0x7b, idata);
                break;
            case 5:
                led_write_byte(0x7e, idata);
                break;

            default:
                break;
            }
            break;
						
		case 9 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x82, idata);
                break;
            case 2:
                led_write_byte(0x85, idata);
                break;
            case 3:
                led_write_byte(0x88, idata);
                break;
            case 4:
                led_write_byte(0x8b, idata);
                break;
            case 5:
                led_write_byte(0x8e, idata);
                break;

            default:
                break;
            }
            break;
						
        default:
            break;
        }
        break;
    case B_LED:
        switch(iX)
        {
        case 1 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x01, idata);
                break;
            case 2:
                led_write_byte(0x04, idata);
                break;
            case 3:
                led_write_byte(0x07, idata);
                break;
            case 4:
                led_write_byte(0x0A, idata);
                break;
            case 5:
                led_write_byte(0x0D, idata);
                break;

            default:
                break;
            }
            break;
        case 2 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x11, idata);
                break;
            case 2:
                led_write_byte(0x14, idata);
                break;
            case 3:
                led_write_byte(0x17, idata);
                break;
            case 4:
                led_write_byte(0x1a, idata);
                break;
            case 5:
                led_write_byte(0x1d, idata);
                break;

            default:
                break;
            }
            break;
        case 3 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x21, idata);
                break;
            case 2:
                led_write_byte(0x24, idata);
                break;
            case 3:
                led_write_byte(0x27, idata);
                break;
            case 4:
                led_write_byte(0x2a, idata);
                break;
            case 5:
                led_write_byte(0x2d, idata);
                break;

            default:
                break;
            }
            break;
        case 4 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x31, idata);
                break;
            case 2:
                led_write_byte(0x34, idata);
                break;
            case 3:
                led_write_byte(0x37, idata);
                break;
            case 4:
                led_write_byte(0x3a, idata);
                break;
            case 5:
                led_write_byte(0x3d, idata);
                break;

            default:
                break;
            }
            break;
						
		case 5 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x41, idata);
                break;
            case 2:
                led_write_byte(0x44, idata);
                break;
            case 3:
                led_write_byte(0x47, idata);
                break;
            case 4:
                led_write_byte(0x4a, idata);
                break;
            case 5:
                led_write_byte(0x4d, idata);
                break;

            default:
                break;
            }
            break;
						
		case 6 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x51, idata);
                break;
            case 2:
                led_write_byte(0x54, idata);
                break;
            case 3:
                led_write_byte(0x57, idata);
                break;
            case 4:
                led_write_byte(0x5a, idata);
                break;
            case 5:
                led_write_byte(0x5d, idata);
                break;

            default:
                break;
            }
            break;
						
		case 7 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x61, idata);
                break;
            case 2:
                led_write_byte(0x64, idata);
                break;
            case 3:
                led_write_byte(0x67, idata);
                break;
            case 4:
                led_write_byte(0x6a, idata);
                break;
            case 5:
                led_write_byte(0x6d, idata);
                break;

            default:
                break;
            }
            break;
						
		case 8 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x71, idata);
                break;
            case 2:
                led_write_byte(0x74, idata);
                break;
            case 3:
                led_write_byte(0x77, idata);
                break;
            case 4:
                led_write_byte(0x7a, idata);
                break;
            case 5:
                led_write_byte(0x7d, idata);
                break;

            default:
                break;
            }
            break;
						
		case 9 :
            switch(iY)
            {
            case 1:
                led_write_byte(0x81, idata);
                break;
            case 2:
                led_write_byte(0x84, idata);
                break;
            case 3:
                led_write_byte(0x87, idata);
                break;
            case 4:
                led_write_byte(0x8a, idata);
                break;
            case 5:
                led_write_byte(0x8d, idata);
                break;

            default:
                break;
            }
            break;
						
						
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void led_open(void)
{
    sdb_output(false);
    Init_3729(R, G, B);
    sdb_output(true);
}

void led_close(void)
{
    uint8_t i;
    sdb_output(false);

	for(i=0x01;i<=0x8f;i++)
	{
		led_write_byte(i,0x00);
	}
}

void led_init(void)
{
    sdb_pimmux_init();
    iic_dev = iic_open_id("iic", 0);
    iic_config_t config = {
        MODE_MASTER,
        BUS_SPEED_STANDARD,
        ADDR_7BIT,
        Addr_GND
    };
    iic_config(iic_dev, &config);
    led_close();
}
