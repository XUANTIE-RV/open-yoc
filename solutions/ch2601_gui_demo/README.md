# 概述

ch2601_gui_demo提供TFT LCD屏幕的开发演示。该程序通过调用开源lvgl组件实现屏幕label控件显示功能，程序内还包含lvgl组件的移植程序源码。

屏幕采用WiseChip公司的128x64像素规格的单彩色屏幕，参考链接地址：[https://www.wisechip.com.tw/zh-tw](https://www.wisechip.com.tw/zh-tw)。

屏幕控制器采用solomon-systech公司的SSD1309， 参考链接地址：[https://www.solomon-systech.com/en/product/advanced-display/oled-display-driver-ic/ssd1309/](https://www.solomon-systech.com/en/product/advanced-display/oled-display-driver-ic/ssd1309/)。

ch2601开发板采用单彩色图形显示面板，屏幕分辨率128x64 pixel，屏幕背景可选，该程序中采用的是一块黄色背景的屏幕。屏幕控制器采用SSD1309，通过4 wire SPI接口与主芯片连接, 对应的pin引脚分别为PA27、PA28、PA29、PA30.

该程序通过调用开源组件lvgl实现屏幕绘制功能，lvgl是一个免费开源的图形库，提供嵌入式系统的GUI能力，该开源库有使用方便，画面美观，内存占用率低等优点，lvgl的链接地址 https://lvgl.io/



# 下载

首先安装yoc工具，参考YoCBook CDK开发快速上手 章节， 链接 https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html

下载ch2601_gui_demo代码命令如下：

```bash
yoc init

yoc install ch2601_gui_demo
```



# 配置

lvgl通过配置文件提供灵活性配置，配置文件路径：components/lvgl/lv_conf.h
该示例程序采用的主要配置项如下：

```c

/* Maximal horizontal and vertical resolution to support by the library.*/
#define LV_HOR_RES_MAX          (128)
#define LV_VER_RES_MAX          (64)

/* Color depth:
 * - 1:  1 byte per pixel
 * - 8:  RGB332
 * - 16: RGB565
 * - 32: ARGB8888
 */
#define LV_COLOR_DEPTH    1 


/*Images pixels with this color will not be drawn (with chroma keying)*/
#define LV_COLOR_TRANSP   LV_COLOR_BLACK


/* Default display refresh period.
 * Can be changed in the display driver (`lv_disp_drv_t`).*/
#define LV_DISP_DEF_REFR_PERIOD      30      /*[ms]*/

/* Dot Per Inch: used to initialize default sizes.
 * E.g. a button with width = LV_DPI / 2 -> half inch wide
 * (Not so important, you can adjust it to modify default sizes and spaces)*/
#define LV_DPI              30     /*[px]*/


/* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */
typedef int16_t lv_coord_t;


/*==================
 *    FONT USAGE
 *===================*/

/* The built-in fonts contains the ASCII range and some Symbols with  4 bit-per-pixel.
 * The symbols are available via `LV_SYMBOL_...` defines
 * More info about fonts: https://docs.lvgl.io/v7/en/html/overview/font.html
 * To create a new font go to: https://lvgl.com/ttf-font-to-c-array
 */

/* Montserrat fonts with bpp = 4
 * https://fonts.google.com/specimen/Montserrat  */
#define LV_FONT_MONTSERRAT_8     0
#define LV_FONT_MONTSERRAT_10    0
#define LV_FONT_MONTSERRAT_12    0
#define LV_FONT_MONTSERRAT_14    1
#define LV_FONT_MONTSERRAT_16    0
#define LV_FONT_MONTSERRAT_18    0
#define LV_FONT_MONTSERRAT_20    0
#define LV_FONT_MONTSERRAT_22    0
#define LV_FONT_MONTSERRAT_24    0
#define LV_FONT_MONTSERRAT_26    0
#define LV_FONT_MONTSERRAT_28    0
#define LV_FONT_MONTSERRAT_30    0
#define LV_FONT_MONTSERRAT_32    0
#define LV_FONT_MONTSERRAT_34    0
#define LV_FONT_MONTSERRAT_36    0
#define LV_FONT_MONTSERRAT_38    0
#define LV_FONT_MONTSERRAT_40    0
#define LV_FONT_MONTSERRAT_42    0
#define LV_FONT_MONTSERRAT_44    0
#define LV_FONT_MONTSERRAT_46    0
#define LV_FONT_MONTSERRAT_48    0

/* Demonstrate special features */
#define LV_FONT_MONTSERRAT_12_SUBPX      0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /*bpp = 3*/
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /*Hebrew, Arabic, PErisan letters and all their forms*/
#define LV_FONT_SIMSUN_16_CJK            1  /*1000 most common CJK radicals*/



/*=================
 *  Text settings
 *=================*/

/* Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 * */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

 /*Can break (wrap) texts on these chars*/
#define LV_TXT_BREAK_CHARS                  " ,.;:-_"


/* Minimum number of characters in a long word to put on a line before a break.
 * Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3

/* Minimum number of characters in a long word to put on a line after a break.
 * Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/* The control character to use for signalling text recoloring. */
#define LV_TXT_COLOR_CMD "#"

/* Support bidirectional texts.
 * Allows mixing Left-to-Right and Right-to-Left texts.
 * The direction will be processed according to the Unicode Bidirectional Algorithm:
 * https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
#define LV_USE_BIDI     0
#if LV_USE_BIDI
/* Set the default direction. Supported values:
 * `LV_BIDI_DIR_LTR` Left-to-Right
 * `LV_BIDI_DIR_RTL` Right-to-Left
 * `LV_BIDI_DIR_AUTO` detect texts base direction */
#define LV_BIDI_BASE_DIR_DEF  LV_BIDI_DIR_AUTO
#endif

/* Enable Arabic/Persian processing
 * In these languages characters should be replaced with
 * an other form based on their position in the text */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*Change the built in (v)snprintf functions*/
#define LV_SPRINTF_CUSTOM   0
#if LV_SPRINTF_CUSTOM
#  define LV_SPRINTF_INCLUDE <stdio.h>
#  define lv_snprintf     snprintf
#  define lv_vsnprintf    vsnprintf
#else   /*!LV_SPRINTF_CUSTOM*/
#  define LV_SPRINTF_DISABLE_FLOAT 1
#endif  /*LV_SPRINTF_CUSTOM*/

```

# LVGL移植

该demo中包含lvgl组件porting 代码， 位于目录solutions/ch2601_gui_demo/app/src/lvgl_porting。

lvgl porting 参考文档 https://docs.lvgl.io/latest/en/html/porting/index.html

demo中主要porting接口如下：

屏幕初始化接口

```c
oled_init
```

屏幕显示刷新接口

```c
oled_flush
```



# 接口

无



# 代码示例

label控件显示：


```c
void gui_label_create(void)

{

  /* 创建label控件对象 */

lv_obj_t *p = lv_label_create(lv_scr_act(), NULL);

/* 设置lable控件显示模式、对齐方式、坐标、宽高等属性 */

  lv_label_set_long_mode(p, LV_LABEL_LONG_BREAK);

  lv_label_set_align(p, LV_LABEL_ALIGN_CENTER);

  lv_obj_set_pos(p, 0, 4);

lv_obj_set_size(p, 128, 60);

/* 设置lable控件显示内容 */

  lv_label_set_text(p, "THEAD RISV-V\nJIUXUAN600\nEVB-BOARD");

}

```



## 诊断错误码

无。



## 运行资源

无。



## 依赖资源

无。



# 编译

目录solutions/ch2601_gui_demo下执行

```bash
make
```



# 烧录

目录solutions/ch2601_gui_demo下执行

```bash
make flashall
```



# 启动

烧录完成之后按复位键，屏幕会有“T-HEAD GUI DEMO”文字显示。



## 组件参考

无。