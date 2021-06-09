/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>
#include <soc.h>
#include "oled.h"
#include "board_config.h"
#include "drv/gpio_pin.h"
#include <drv/pin.h>

#define code

#define SPI // 4-wire Serial

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Delay Time
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void uDelay(unsigned char l)
{
    udelay(l * 5);
}

void Delay(unsigned char n)
{
    udelay(n * 1000);
}

#ifdef SPI // 4-wire Serial
csi_gpio_pin_t pin_clk;
csi_gpio_pin_t pin_mosi;
csi_gpio_pin_t pin_cs;
csi_gpio_pin_t pin_miso;

static void oled_pinmux_init()
{
    csi_pin_set_mux(PA28, PIN_FUNC_GPIO); //clk
    csi_pin_set_mux(PA29, PIN_FUNC_GPIO); //mosi
    csi_pin_set_mux(PA27, PIN_FUNC_GPIO); //cs
    csi_pin_set_mux(PA30, PIN_FUNC_GPIO); //miso
}

static void oled_gpio_init()
{
    //
    csi_gpio_pin_init(&pin_clk, PA28);
    csi_gpio_pin_dir(&pin_clk, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_init(&pin_mosi, PA29);
    csi_gpio_pin_dir(&pin_mosi, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_init(&pin_cs, PA27);
    csi_gpio_pin_dir(&pin_cs, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_init(&pin_miso, PA30); //dc
    csi_gpio_pin_dir(&pin_miso, GPIO_DIRECTION_OUTPUT);
}

static void lcd_cs(uint8_t d)
{
    if (d == 1) {
        csi_gpio_pin_write(&pin_cs, GPIO_PIN_HIGH);
    } else {
        csi_gpio_pin_write(&pin_cs, GPIO_PIN_LOW);
    }
}

static void lcd_dc(uint8_t d)
{
    if (d == 1) {
        csi_gpio_pin_write(&pin_miso, GPIO_PIN_HIGH);
    } else {
        csi_gpio_pin_write(&pin_miso, GPIO_PIN_LOW);
    }
}

static void lcd_sclk(uint8_t d)
{
    if (d == 1) {
        csi_gpio_pin_write(&pin_clk, GPIO_PIN_HIGH);
    } else {
        csi_gpio_pin_write(&pin_clk, GPIO_PIN_LOW);
    }
}

static void lcd_sdin(uint8_t d)
{
    if (d == 1) {
        csi_gpio_pin_write(&pin_mosi, GPIO_PIN_HIGH);
    } else {
        csi_gpio_pin_write(&pin_mosi, GPIO_PIN_LOW);
    }
}

void Write_Command(unsigned char Data)
{
    unsigned char i;

    lcd_cs(0);
    lcd_dc(0);
    for (i = 0; i < 8; i++) {
        lcd_sclk(0);
        lcd_sdin((Data & 0x80) >> 7);
        Data = Data << 1;
        lcd_sclk(1);
    }
    lcd_dc(1);
    lcd_cs(1);
}

void Write_Data(unsigned char Data)
{
    unsigned char i;

    lcd_cs(0);
    lcd_dc(1);
    for (i = 0; i < 8; i++) {
        lcd_sclk(0);
        lcd_sdin((Data & 0x80) >> 7);
        Data = Data << 1;
        lcd_sclk(1);
    }
    lcd_dc(1);
    lcd_cs(1);
}
#endif

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Start_Column(unsigned char d)
{
    Write_Command(0x00 + d % 16); // Set Lower Column Start Address for Page Addressing Mode
                                  //   Default => 0x00
    Write_Command(0x10 + d / 16); // Set Higher Column Start Address for Page Addressing Mode
                                  //   Default => 0x10
}

void Set_Addressing_Mode(unsigned char d)
{
    Write_Command(0x20); // Set Memory Addressing Mode
    Write_Command(d);    //   Default => 0x02
                         //     0x00 => Horizontal Addressing Mode
                         //     0x01 => Vertical Addressing Mode
                         //     0x02 => Page Addressing Mode
}

void Set_Column_Address(unsigned char a, unsigned char b)
{
    Write_Command(0x21); // Set Column Address
    Write_Command(a);    //   Default => 0x00 (Column Start Address)
    Write_Command(b);    //   Default => 0x7F (Column End Address)
}

void Set_Page_Address(unsigned char a, unsigned char b)
{
    Write_Command(0x22); // Set Page Address
    Write_Command(a);    //   Default => 0x00 (Page Start Address)
    Write_Command(b);    //   Default => 0x07 (Page End Address)
}

void Set_Start_Line(unsigned char d)
{
    Write_Command(0x40 | d); // Set Display Start Line
                             //   Default => 0x40 (0x00)
}

void Set_Contrast_Control(unsigned char d)
{
    Write_Command(0x81); // Set Contrast Control for Bank 0
    Write_Command(d);    //   Default => 0x7F
}

void Set_Segment_Remap(unsigned char d)
{
    Write_Command(d); // Set Segment Re-Map
                      //   Default => 0xA0
                      //     0xA0 => Column Address 0 Mapped to SEG0
                      //     0xA1 => Column Address 0 Mapped to SEG127
}

void Set_Entire_Display(unsigned char d)
{
    Write_Command(d); // Set Entire Display On / Off
                      //   Default => 0xA4
                      //     0xA4 => Normal Display
                      //     0xA5 => Entire Display On
}

void Set_Inverse_Display(unsigned char d)
{
    Write_Command(d); // Set Inverse Display On/Off
                      //   Default => 0xA6
                      //     0xA6 => Normal Display
                      //     0xA7 => Inverse Display On
}

void Set_Multiplex_Ratio(unsigned char d)
{
    Write_Command(0xA8); // Set Multiplex Ratio
    Write_Command(d);    //   Default => 0x3F (1/64 Duty)
}

void Set_Display_On_Off(unsigned char d)
{
    Write_Command(d); // Set Display On/Off
                      //   Default => 0xAE
                      //     0xAE => Display Off
                      //     0xAF => Display On
}

void Set_Start_Page(unsigned char d)
{
    Write_Command(0xB0 | d); // Set Page Start Address for Page Addressing Mode
                             //   Default => 0xB0 (0x00)
}

void Set_Common_Remap(unsigned char d)
{
    Write_Command(d); // Set COM Output Scan Direction
                      //   Default => 0xC0
                      //     0xC0 => Scan from COM0 to 63
                      //     0xC8 => Scan from COM63 to 0
}

void Set_Display_Offset(unsigned char d)
{
    Write_Command(0xD3); // Set Display Offset
    Write_Command(d);    //   Default => 0x00
}

void Set_Display_Clock(unsigned char d)
{
    Write_Command(0xD5); // Set Display Clock Divide Ratio / Oscillator Frequency
    Write_Command(d);    //   Default => 0x70
                         //     D[3:0] => Display Clock Divider
                         //     D[7:4] => Oscillator Frequency
}

void Set_Low_Power(unsigned char d)
{
    Write_Command(0xD8); // Set Low Power Display Mode
    Write_Command(d);    //   Default => 0x04 (Normal Power Mode)
}

void Set_Precharge_Period(unsigned char d)
{
    Write_Command(0xD9); // Set Pre-Charge Period
    Write_Command(d); //   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
                      //     D[3:0] => Phase 1 Period in 1~15 Display Clocks
                      //     D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

void Set_Common_Config(unsigned char d)
{
    Write_Command(0xDA); // Set COM Pins Hardware Configuration
    Write_Command(d);    //   Default => 0x12
                         //     Alternative COM Pin Configuration
                         //     Disable COM Left/Right Re-Map
}

void Set_VCOMH(unsigned char d)
{
    Write_Command(0xDB); // Set VCOMH Deselect Level
    Write_Command(d);    //   Default => 0x34 (0.78*VCC)
}

void Set_NOP()
{
    Write_Command(0xE3); // Command for No Operation
}

void Set_Command_Lock(unsigned char d)
{
    Write_Command(0xFD); // Set Command Lock
    Write_Command(d);    //   Default => 0x12
                         //     0x12 => Driver IC interface is unlocked from entering command.
                         //     0x16 => All Commands are locked except 0xFD.
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Global Variables
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define XLevelL 0x00
#define XLevelH 0x10
#define XLevel ((XLevelH & 0x0F) * 16 + XLevelL)
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xBF

#define SET_BIT(b, n) b |= (1 << n)
#define CLR_BIT(b, n) b &= ~(uint8_t)(1 << n)

uint8_t g_oled_ram[8][128];

void oled_draw_point(uint8_t r, uint8_t c, uint8_t t)
{
    if (t) {
        SET_BIT(g_oled_ram[r / 8][c], ((r % 8)));
    } else {
        CLR_BIT(g_oled_ram[r / 8][c], (r % 8));
    }
}

void oled_draw_frame(uint8_t p[Max_Row][Max_Column])
{
    unsigned char i, j;

    for (i = 0; i < Max_Row; i++) {
        for (j = 0; j < Max_Column; j++) {
            oled_draw_point(i, j, p[i][j]);
        }
    }
}

void oled_reflesh()
{
    unsigned char i, j;
    for (i = 0; i < 8; i++) {
        Set_Start_Page(i);
        Set_Start_Column(0x00);

        for (j = 0; j < 128; j++) {
            Write_Data(g_oled_ram[i][j]);
        }
    }
}

void Fill_RAM(unsigned char Data)
{
    unsigned char i, j;

    for (i = 0; i < 8; i++) {
        Set_Start_Page(i);
        Set_Start_Column(0x00);

        for (j = 0; j < 128; j++) {
            Write_Data(Data);
        }
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Initialization
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
static void oled_initialize()
{
    Set_Command_Lock(0x12);           // Unlock Driver IC (0x12/0x16)
    Set_Display_On_Off(0xAE);         // Display Off (0xAE/0xAF)
    Set_Display_Clock(0xA0);          // Set Clock as 116 Frames/Sec
    Set_Multiplex_Ratio(0x3F);        // 1/64 Duty (0x0F~0x3F)
    Set_Display_Offset(0x00);         // Shift Mapping RAM Counter (0x00~0x3F)
    Set_Start_Line(0x00);             // Set Mapping RAM Display Start Line (0x00~0x3F)
    Set_Low_Power(0x04);              // Set Normal Power Mode (0x04/0x05)
    Set_Addressing_Mode(0x02);        // Set Page Addressing Mode (0x00/0x01/0x02)
    Set_Segment_Remap(0xA1);          // Set SEG/Column Mapping (0xA0/0xA1)
    Set_Common_Remap(0xC8);           // Set COM/Row Scan Direction (0xC0/0xC8)
    Set_Common_Config(0x12);          // Set Alternative Configuration (0x02/0x12)
    Set_Contrast_Control(Brightness); // Set SEG Output Current
    Set_Precharge_Period(0x82);       // Set Pre-Charge as 8 Clocks & Discharge as 2 Clocks
    Set_VCOMH(0x34);                  // Set VCOM Deselect Level
    Set_Entire_Display(0xA4);         // Disable Entire Display On (0xA4/0xA5)
    Set_Inverse_Display(0xA6);        // Disable Inverse Display On (0xA6/0xA7)

    Fill_RAM(0x00); // Clear Screen

    Set_Display_On_Off(0xAF); // Display On (0xAE/0xAF)
}
/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
// static void oled_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
// {
//     oled_draw_frame((uint8_t(*)[Max_Column])color_p);
//     oled_reflesh();

//     /* IMPORTANT!!!
//      * Inform the graphics library that you are ready with the flushing*/
//     lv_disp_flush_ready(disp_drv);
// }

// static lv_disp_buf_t disp_buf1;
// static lv_color_t    buf1[64 * 128];
// static lv_color_t    buf2[64 * 128];
extern uint8_t g_bm_rv[64][128];
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Main Program
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// #define CONFIG_OLED_TEST
void oled_init()
{
    oled_pinmux_init();
    oled_gpio_init();
    oled_initialize();

    oled_draw_frame(g_bm_rv);
    oled_reflesh();
}
