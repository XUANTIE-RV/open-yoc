/*
* uhid.h                                                    Version 3.00
*
* smxUSBH Generic HID Class Driver.
*
* Copyright (c) 2007-2018 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: Yingbo Hu
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SU_HID_H
#define SU_HID_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/* Usage type */
#define SU_HID_TYPE_RST         0x00
#define SU_HID_TYPE_KEY         0x01
#define SU_HID_TYPE_REL         0x02
#define SU_HID_TYPE_ABS         0x03
#define SU_HID_TYPE_MSC         0x04
#define SU_HID_TYPE_LED         0x11
#define SU_HID_TYPE_SOUND       0x12
#define SU_HID_TYPE_MAX         0x1f


/* Usage code for key */

#define SU_HID_KEY_RESERVED     0
#define SU_HID_KEY_ESC          1
#define SU_HID_KEY_1            2
#define SU_HID_KEY_2            3
#define SU_HID_KEY_3            4
#define SU_HID_KEY_4            5
#define SU_HID_KEY_5            6
#define SU_HID_KEY_6            7
#define SU_HID_KEY_7            8
#define SU_HID_KEY_8            9
#define SU_HID_KEY_9            10
#define SU_HID_KEY_0            11
#define SU_HID_KEY_MINUS        12
#define SU_HID_KEY_EQUAL        13
#define SU_HID_KEY_BACKSPACE    14
#define SU_HID_KEY_TAB          15
#define SU_HID_KEY_Q            16
#define SU_HID_KEY_W            17
#define SU_HID_KEY_E            18
#define SU_HID_KEY_R            19
#define SU_HID_KEY_T            20
#define SU_HID_KEY_Y            21
#define SU_HID_KEY_U            22
#define SU_HID_KEY_I            23
#define SU_HID_KEY_O            24
#define SU_HID_KEY_P            25
#define SU_HID_KEY_LEFTBRACE    26
#define SU_HID_KEY_RIGHTBRACE   27
#define SU_HID_KEY_ENTER        28
#define SU_HID_KEY_LEFTCTRL     29
#define SU_HID_KEY_A            30
#define SU_HID_KEY_S            31
#define SU_HID_KEY_D            32
#define SU_HID_KEY_F            33
#define SU_HID_KEY_G            34
#define SU_HID_KEY_H            35
#define SU_HID_KEY_J            36
#define SU_HID_KEY_K            37
#define SU_HID_KEY_L            38
#define SU_HID_KEY_SEMICOLON    39
#define SU_HID_KEY_APOSTROPHE   40
#define SU_HID_KEY_GRAVE        41
#define SU_HID_KEY_LEFTSHIFT    42
#define SU_HID_KEY_BACKSLASH    43
#define SU_HID_KEY_Z            44
#define SU_HID_KEY_X            45
#define SU_HID_KEY_C            46
#define SU_HID_KEY_V            47
#define SU_HID_KEY_B            48
#define SU_HID_KEY_N            49
#define SU_HID_KEY_M            50
#define SU_HID_KEY_COMMA        51
#define SU_HID_KEY_DOT          52
#define SU_HID_KEY_SLASH        53
#define SU_HID_KEY_RIGHTSHIFT   54
#define SU_HID_KEY_KPASTERISK   55
#define SU_HID_KEY_LEFTALT      56
#define SU_HID_KEY_SPACE        57
#define SU_HID_KEY_CAPSLOCK     58
#define SU_HID_KEY_F1           59
#define SU_HID_KEY_F2           60
#define SU_HID_KEY_F3           61
#define SU_HID_KEY_F4           62
#define SU_HID_KEY_F5           63
#define SU_HID_KEY_F6           64
#define SU_HID_KEY_F7           65
#define SU_HID_KEY_F8           66
#define SU_HID_KEY_F9           67
#define SU_HID_KEY_F10          68
#define SU_HID_KEY_NUMLOCK      69
#define SU_HID_KEY_SCROLLLOCK   70
#define SU_HID_KEY_KP7          71
#define SU_HID_KEY_KP8          72
#define SU_HID_KEY_KP9          73
#define SU_HID_KEY_KPMINUS      74
#define SU_HID_KEY_KP4          75
#define SU_HID_KEY_KP5          76
#define SU_HID_KEY_KP6          77
#define SU_HID_KEY_KPPLUS       78
#define SU_HID_KEY_KP1          79
#define SU_HID_KEY_KP2          80
#define SU_HID_KEY_KP3          81
#define SU_HID_KEY_KP0          82
#define SU_HID_KEY_KPDOT        83
#define SU_HID_KEY_103RD        84
#define SU_HID_KEY_F13          85
#define SU_HID_KEY_102ND        86
#define SU_HID_KEY_F11          87
#define SU_HID_KEY_F12          88
#define SU_HID_KEY_F14          89
#define SU_HID_KEY_F15          90
#define SU_HID_KEY_F16          91
#define SU_HID_KEY_F17          92
#define SU_HID_KEY_F18          93
#define SU_HID_KEY_F19          94
#define SU_HID_KEY_F20          95
#define SU_HID_KEY_KPENTER      96
#define SU_HID_KEY_RIGHTCTRL    97
#define SU_HID_KEY_KPSLASH      98
#define SU_HID_KEY_SYSRQ        99
#define SU_HID_KEY_RIGHTALT     100
#define SU_HID_KEY_LINEFEED     101
#define SU_HID_KEY_HOME         102
#define SU_HID_KEY_UP           103
#define SU_HID_KEY_PAGEUP       104
#define SU_HID_KEY_LEFT         105
#define SU_HID_KEY_RIGHT        106
#define SU_HID_KEY_END          107
#define SU_HID_KEY_DOWN         108
#define SU_HID_KEY_PAGEDOWN     109
#define SU_HID_KEY_INSERT       110
#define SU_HID_KEY_DELETE       111
#define SU_HID_KEY_MACRO        112
#define SU_HID_KEY_MUTE         113
#define SU_HID_KEY_VOLUMEDOWN   114
#define SU_HID_KEY_VOLUMEUP     115
#define SU_HID_KEY_POWER        116
#define SU_HID_KEY_KPEQUAL      117
#define SU_HID_KEY_KPPLUSMINUS  118
#define SU_HID_KEY_PAUSE        119
#define SU_HID_KEY_F21          120
#define SU_HID_KEY_F22          121
#define SU_HID_KEY_F23          122
#define SU_HID_KEY_F24          123
#define SU_HID_KEY_KPCOMMA      124
#define SU_HID_KEY_LEFTMETA     125
#define SU_HID_KEY_RIGHTMETA    126
#define SU_HID_KEY_COMPOSE      127

#define SU_HID_KEY_STOP         128
#define SU_HID_KEY_AGAIN        129
#define SU_HID_KEY_PROPS        130
#define SU_HID_KEY_UNDO         131
#define SU_HID_KEY_FRONT        132
#define SU_HID_KEY_COPY         133
#define SU_HID_KEY_OPEN         134
#define SU_HID_KEY_PASTE        135
#define SU_HID_KEY_FIND         136
#define SU_HID_KEY_CUT          137
#define SU_HID_KEY_HELP         138
#define SU_HID_KEY_MENU         139
#define SU_HID_KEY_CALC         140
#define SU_HID_KEY_SETUP        141
#define SU_HID_KEY_SLEEP        142
#define SU_HID_KEY_WAKEUP       143
#define SU_HID_KEY_FILE         144
#define SU_HID_KEY_SENDFILE     145
#define SU_HID_KEY_DELETEFILE   146
#define SU_HID_KEY_XFER         147
#define SU_HID_KEY_PROG1        148
#define SU_HID_KEY_PROG2        149
#define SU_HID_KEY_WWW          150
#define SU_HID_KEY_MSDOS        151
#define SU_HID_KEY_COFFEE       152
#define SU_HID_KEY_DIRECTION    153
#define SU_HID_KEY_CYCLEWINDOWS 154
#define SU_HID_KEY_MAIL         155
#define SU_HID_KEY_BOOKMARKS    156
#define SU_HID_KEY_COMPUTER     157
#define SU_HID_KEY_BACK         158
#define SU_HID_KEY_FORWARD      159
#define SU_HID_KEY_CLOSECD      160
#define SU_HID_KEY_EJECTCD      161
#define SU_HID_KEY_EJECTCLOSECD 162
#define SU_HID_KEY_NEXTSONG     163
#define SU_HID_KEY_PLAYPAUSE    164
#define SU_HID_KEY_PREVIOUSSONG 165
#define SU_HID_KEY_STOPCD       166
#define SU_HID_KEY_RECORD       167
#define SU_HID_KEY_REWIND       168
#define SU_HID_KEY_PHONE        169
#define SU_HID_KEY_ISO          170
#define SU_HID_KEY_CONFIG       171
#define SU_HID_KEY_HOMEPAGE     172
#define SU_HID_KEY_REFRESH      173
#define SU_HID_KEY_EXIT         174
#define SU_HID_KEY_MOVE         175
#define SU_HID_KEY_EDIT         176
#define SU_HID_KEY_SCROLLUP     177
#define SU_HID_KEY_SCROLLDOWN   178
#define SU_HID_KEY_KPLEFTPAREN  179
#define SU_HID_KEY_KPRIGHTPAREN 180

#define SU_HID_KEY_INTL1        181
#define SU_HID_KEY_INTL2        182
#define SU_HID_KEY_INTL3        183
#define SU_HID_KEY_INTL4        184
#define SU_HID_KEY_INTL5        185
#define SU_HID_KEY_INTL6        186
#define SU_HID_KEY_INTL7        187
#define SU_HID_KEY_INTL8        188
#define SU_HID_KEY_INTL9        189
#define SU_HID_KEY_LANG1        190
#define SU_HID_KEY_LANG2        191
#define SU_HID_KEY_LANG3        192
#define SU_HID_KEY_LANG4        193
#define SU_HID_KEY_LANG5        194
#define SU_HID_KEY_LANG6        195
#define SU_HID_KEY_LANG7        196
#define SU_HID_KEY_LANG8        197
#define SU_HID_KEY_LANG9        198

#define SU_HID_KEY_PLAYCD       200
#define SU_HID_KEY_PAUSECD      201
#define SU_HID_KEY_PROG3        202
#define SU_HID_KEY_PROG4        203
#define SU_HID_KEY_SUSPEND      205
#define SU_HID_KEY_CLOSE        206

#define SU_HID_KEY_UNKNOWN      220

#define SU_HID_KEY_BRIGHTNESSDOWN  224
#define SU_HID_KEY_BRIGHTNESSUP    225

/* Usage code for button */
#define SU_HID_BTN_MISC         0x100
#define SU_HID_BTN_0            0x100
#define SU_HID_BTN_1            0x101
#define SU_HID_BTN_2            0x102
#define SU_HID_BTN_3            0x103
#define SU_HID_BTN_4            0x104
#define SU_HID_BTN_5            0x105
#define SU_HID_BTN_6            0x106
#define SU_HID_BTN_7            0x107
#define SU_HID_BTN_8            0x108
#define SU_HID_BTN_9            0x109

#define SU_HID_BTN_MOUSE        0x110
#define SU_HID_BTN_LEFT         0x110
#define SU_HID_BTN_RIGHT        0x111
#define SU_HID_BTN_MIDDLE       0x112
#define SU_HID_BTN_SIDE         0x113
#define SU_HID_BTN_EXTRA        0x114
#define SU_HID_BTN_FORWARD      0x115
#define SU_HID_BTN_BACK         0x116

#define SU_HID_BTN_JOYSTICK     0x120
#define SU_HID_BTN_TRIGGER      0x120
#define SU_HID_BTN_THUMB        0x121
#define SU_HID_BTN_THUMB2       0x122
#define SU_HID_BTN_TOP          0x123
#define SU_HID_BTN_TOP2         0x124
#define SU_HID_BTN_PINKIE       0x125
#define SU_HID_BTN_BASE         0x126
#define SU_HID_BTN_BASE2        0x127
#define SU_HID_BTN_BASE3        0x128
#define SU_HID_BTN_BASE4        0x129
#define SU_HID_BTN_BASE5        0x12a
#define SU_HID_BTN_BASE6        0x12b
#define SU_HID_BTN_DEAD         0x12f

#define SU_HID_BTN_GAMEPAD      0x130
#define SU_HID_BTN_A            0x130
#define SU_HID_BTN_B            0x131
#define SU_HID_BTN_C            0x132
#define SU_HID_BTN_X            0x133
#define SU_HID_BTN_Y            0x134
#define SU_HID_BTN_Z            0x135
#define SU_HID_BTN_TL           0x136
#define SU_HID_BTN_TR           0x137
#define SU_HID_BTN_TL2          0x138
#define SU_HID_BTN_TR2          0x139
#define SU_HID_BTN_SELECT       0x13a
#define SU_HID_BTN_START        0x13b
#define SU_HID_BTN_MODE         0x13c
#define SU_HID_BTN_THUMBL       0x13d
#define SU_HID_BTN_THUMBR       0x13e

#define SU_HID_BTN_DIGI         0x140
#define SU_HID_BTN_TOOL_PEN     0x140
#define SU_HID_BTN_TOOL_RUBBER  0x141
#define SU_HID_BTN_TOOL_BRUSH   0x142
#define SU_HID_BTN_TOOL_PENCIL  0x143
#define SU_HID_BTN_TOOL_AIRBRUSH    0x144
#define SU_HID_BTN_TOOL_FINGER  0x145
#define SU_HID_BTN_TOOL_MOUSE   0x146
#define SU_HID_BTN_TOOL_LENS    0x147
#define SU_HID_BTN_TOUCH        0x14a
#define SU_HID_BTN_STYLUS       0x14b
#define SU_HID_BTN_STYLUS2      0x14c

#define SU_HID_KEY_MAX          0x1ff

/* Usage code for Relative axes */
#define SU_HID_REL_X            0x00
#define SU_HID_REL_Y            0x01
#define SU_HID_REL_Z            0x02
#define SU_HID_REL_HWHEEL       0x06
#define SU_HID_REL_DIAL         0x07
#define SU_HID_REL_WHEEL        0x08
#define SU_HID_REL_MISC         0x09
#define SU_HID_REL_MAX          0x0f

/* Usage code for Absolute axes */
#define SU_HID_ABS_X            0x00
#define SU_HID_ABS_Y            0x01
#define SU_HID_ABS_Z            0x02
#define SU_HID_ABS_RX           0x03
#define SU_HID_ABS_RY           0x04
#define SU_HID_ABS_RZ           0x05
#define SU_HID_ABS_THROTTLE     0x06
#define SU_HID_ABS_RUDDER       0x07
#define SU_HID_ABS_WHEEL        0x08
#define SU_HID_ABS_GAS          0x09
#define SU_HID_ABS_BRAKE        0x0a
#define SU_HID_ABS_HAT0X        0x10
#define SU_HID_ABS_HAT0Y        0x11
#define SU_HID_ABS_HAT1X        0x12
#define SU_HID_ABS_HAT1Y        0x13
#define SU_HID_ABS_HAT2X        0x14
#define SU_HID_ABS_HAT2Y        0x15
#define SU_HID_ABS_HAT3X        0x16
#define SU_HID_ABS_HAT3Y        0x17
#define SU_HID_ABS_PRESSURE     0x18
#define SU_HID_ABS_DISTANCE     0x19
#define SU_HID_ABS_TILT_X       0x1a
#define SU_HID_ABS_TILT_Y       0x1b
#define SU_HID_ABS_MISC         0x1c
#define SU_HID_ABS_MAX          0x1f

/* Usage code for Misc events */

#define SU_HID_MSC_SERIAL       0x00
#define SU_HID_MSC_MAX          0x07

/* Usage code for LEDs */
#define SU_HID_LED_NUML         0x00
#define SU_HID_LED_CAPSL        0x01
#define SU_HID_LED_SCROLLL      0x02
#define SU_HID_LED_COMPOSE      0x03
#define SU_HID_LED_KANA         0x04
#define SU_HID_LED_SLEEP        0x05
#define SU_HID_LED_SUSPEND      0x06
#define SU_HID_LED_MUTE         0x07
#define SU_HID_LED_MISC         0x08
#define SU_HID_LED_MAX          0x0f

/* Usage code for sounds */
#define SU_HID_SND_CLICK        0x00
#define SU_HID_SND_BELL         0x01
#define SU_HID_SND_MAX          0x07


/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct 
{
    u32  hid;            /* hid usage code */
    u16  code;           /* usage code */
    u8   type;           /* usage type, SU_HID_TYPE_* */
}SU_HID_USAGE_INFO;

typedef struct 
{
    u32  physical;       /* physical usage for this field */
    u32  logical;        /* logical usage for this field */
    u32  application;    /* application usage for this field */
    u32  flags;          /* main-item flags (i.e. volatile,array,constant) */
    s32  logicalMin;
    s32  logicalMax;
    s32  physicalMin;
    s32  physicalMax;
    u32  unitExponent;
    u32  unit;
}SU_HID_FIELD_INFO;

typedef void (* PHIDFUNC)(uint iID, SU_HID_FIELD_INFO *pFieldInfo, SU_HID_USAGE_INFO *pUsageInfo, s32 data);

int     su_HIDInit(void);
void    su_HIDRelease(void);
void    su_HIDRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

BOOLEAN su_HIDInserted(uint iID);

void    su_HIDSetCallback(PHIDFUNC handler);
int     su_HIDSetReport(uint iID, uint iReportID, u8 *pData, uint len);

#if defined(__cplusplus)
}
#endif

#endif /* SU_HID_H */

