/*
* udkbd.h                                                   Version 2.40
*
* smxUSBD Keyboard Function Driver.
*
* Copyright (c) 2010 Micro Digital Inc.
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

#ifndef SUD_KBD_H
#define SUD_KBD_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* Bitmap for Modifier keys */
#define    SUD_KBD_LEFT_CTRL      0x01
#define    SUD_KBD_LEFT_SHIFT     0x02
#define    SUD_KBD_LEFT_ALT       0x04
#define    SUD_KBD_LEFT_GUI       0x08
#define    SUD_KBD_RIGHT_CTRL     0x10
#define    SUD_KBD_RIGHT_SHIFT    0x20
#define    SUD_KBD_RIGHT_ALT      0x40
#define    SUD_KBD_RIGHT_GUI      0x80

/* KeyCode */
#define    SUD_KBD_CODE_RESERVED         0
#define    SUD_KBD_CODE_ERROR_ROLL_OVER  1
#define    SUD_KBD_CODE_POSTFAIL         2
#define    SUD_KBD_CODE_ERROR_UNDEFINED  3
#define    SUD_KBD_CODE_A                4
#define    SUD_KBD_CODE_B                5
#define    SUD_KBD_CODE_C                6
#define    SUD_KBD_CODE_D                7
#define    SUD_KBD_CODE_E                8
#define    SUD_KBD_CODE_F                9
#define    SUD_KBD_CODE_G                10
#define    SUD_KBD_CODE_H                11
#define    SUD_KBD_CODE_I                12
#define    SUD_KBD_CODE_J                13
#define    SUD_KBD_CODE_K                14
#define    SUD_KBD_CODE_L                15
#define    SUD_KBD_CODE_M                16
#define    SUD_KBD_CODE_N                17
#define    SUD_KBD_CODE_O                18
#define    SUD_KBD_CODE_P                19
#define    SUD_KBD_CODE_Q                20
#define    SUD_KBD_CODE_R                21
#define    SUD_KBD_CODE_S                22
#define    SUD_KBD_CODE_T                23
#define    SUD_KBD_CODE_U                24
#define    SUD_KBD_CODE_V                25
#define    SUD_KBD_CODE_W                26
#define    SUD_KBD_CODE_X                27
#define    SUD_KBD_CODE_Y                28
#define    SUD_KBD_CODE_Z                29
#define    SUD_KBD_CODE_1                30
#define    SUD_KBD_CODE_2                31
#define    SUD_KBD_CODE_3                32
#define    SUD_KBD_CODE_4                33
#define    SUD_KBD_CODE_5                34
#define    SUD_KBD_CODE_6                35
#define    SUD_KBD_CODE_7                36
#define    SUD_KBD_CODE_8                37
#define    SUD_KBD_CODE_9                38
#define    SUD_KBD_CODE_0                39
#define    SUD_KBD_CODE_ENTER            40
#define    SUD_KBD_CODE_ESCAPE           41
#define    SUD_KBD_CODE_DELETE           42
#define    SUD_KBD_CODE_TAB              43
#define    SUD_KBD_CODE_SPACE            44
#define    SUD_KBD_CODE_UNDERSCORE       45
#define    SUD_KBD_CODE_EQUAL            46
#define    SUD_KBD_CODE_EQUAL            46
#define    SUD_KBD_CODE_LEFT_BRACKET     47
#define    SUD_KBD_CODE_RIGHT_BRACKET    48
#define    SUD_KBD_CODE_BACKSLASH        49
#define    SUD_KBD_CODE_NON_US_POUND     50
#define    SUD_KBD_CODE_SEMI_COMMA       51
#define    SUD_KBD_CODE_QUOTATION_MARK   52
#define    SUD_KBD_CODE_GRAVE_ACCENT     53
#define    SUD_KBD_CODE_COMMA            54
#define    SUD_KBD_CODE_DOT              55
#define    SUD_KBD_CODE_SLASH            56
#define    SUD_KBD_CODE_CAPS_LOCK        57
#define    SUD_KBD_CODE_F1               58
#define    SUD_KBD_CODE_F2               59
#define    SUD_KBD_CODE_F3               60
#define    SUD_KBD_CODE_F4               61
#define    SUD_KBD_CODE_F5               62
#define    SUD_KBD_CODE_F6               63
#define    SUD_KBD_CODE_F7               64
#define    SUD_KBD_CODE_F8               65
#define    SUD_KBD_CODE_F9               66
#define    SUD_KBD_CODE_F10              67
#define    SUD_KBD_CODE_F11              68
#define    SUD_KBD_CODE_F12              69
#define    SUD_KBD_CODE_PRINT_SCREEN     70
#define    SUD_KBD_CODE_SCROLL_LOCK      71
#define    SUD_KBD_CODE_PAUSE            72
#define    SUD_KBD_CODE_INSERT           73
#define    SUD_KBD_CODE_HOME             74
#define    SUD_KBD_CODE_PAGE_UP          75
#define    SUD_KBD_CODE_DELETE_FORWARD   76
#define    SUD_KBD_CODE_END              77
#define    SUD_KBD_CODE_PAGE_DOWN        78
#define    SUD_KBD_CODE_RIGHT_ARROW      79
#define    SUD_KBD_CODE_LEFT_ARROW       80
#define    SUD_KBD_CODE_DOWN_ARROW       81
#define    SUD_KBD_CODE_UP_ARROW         82
#define    SUD_KBD_CODE_NUM_LOCK         83
#define    SUD_KBD_CODE_KEYPAD           84
#define    SUD_KBD_CODE_KEYPAD_STAR      85
#define    SUD_KBD_CODE_KEYPAD_MINUS     86
#define    SUD_KBD_CODE_KEYPAD_PLUS      87
#define    SUD_KBD_CODE_KEYPAD_ENTER     88
#define    SUD_KBD_CODE_KEYPAD_1         89
#define    SUD_KBD_CODE_KEYPAD_2         90
#define    SUD_KBD_CODE_KEYPAD_3         91
#define    SUD_KBD_CODE_KEYPAD_4         92
#define    SUD_KBD_CODE_KEYPAD_5         93
#define    SUD_KBD_CODE_KEYPAD_6         94
#define    SUD_KBD_CODE_KEYPAD_7         95
#define    SUD_KBD_CODE_KEYPAD_8         96
#define    SUD_KBD_CODE_KEYPAD_9         97
#define    SUD_KBD_CODE_KEYPAD_0         98
#define    SUD_KBD_CODE_KEYPAD_DOT       99
#define    SUD_KBD_CODE_NON_US           100
#define    SUD_KBD_CODE_APPLICATION      101
#define    SUD_KBD_CODE_POWER            102
#define    SUD_KBD_CODE_KEYPAD_EQUAL     103
#define    SUD_KBD_CODE_F13              104
#define    SUD_KBD_CODE_F14              105
#define    SUD_KBD_CODE_F15              106
#define    SUD_KBD_CODE_F16              107
#define    SUD_KBD_CODE_F17              108
#define    SUD_KBD_CODE_F18              109
#define    SUD_KBD_CODE_F19              110
#define    SUD_KBD_CODE_F20              111
#define    SUD_KBD_CODE_F21              112
#define    SUD_KBD_CODE_F22              113
#define    SUD_KBD_CODE_F23              114
#define    SUD_KBD_CODE_F24              115
#define    SUD_KBD_CODE_EXECUTE          116
#define    SUD_KBD_CODE_HELP             117
#define    SUD_KBD_CODE_MENU             118
#define    SUD_KBD_CODE_SELECT           119
#define    SUD_KBD_CODE_STOP             120
#define    SUD_KBD_CODE_AGAIN            121
#define    SUD_KBD_CODE_UNDO             122
#define    SUD_KBD_CODE_CUT              123
#define    SUD_KBD_CODE_COPY             124
#define    SUD_KBD_CODE_PASTE            125
#define    SUD_KBD_CODE_FIND             126
#define    SUD_KBD_CODE_MUTE             127
#define    SUD_KBD_CODE_VOLUME_UP        128
#define    SUD_KBD_CODE_VOLUME_DOWN      129
#define    SUD_KBD_CODE_LOCK_CAPS_LOCK   130
#define    SUD_KBD_CODE_LOCK_NUM_LOCK    131
#define    SUD_KBD_CODE_LOCK_SCOLL_LOCK  132
#define    SUD_KBD_CODE_KEYPAD_COMMA     133
#define    SUD_KBD_CODE_KEYPAD_EUAL_SIGN 134
#define    SUD_KBD_CODE_INTERNATIONAL_1  135
#define    SUD_KBD_CODE_INTERNATIONAL_2  136
#define    SUD_KBD_CODE_INTERNATIONAL_3  137
#define    SUD_KBD_CODE_INTERNATIONAL_4  138
#define    SUD_KBD_CODE_INTERNATIONAL_5  139
#define    SUD_KBD_CODE_INTERNATIONAL_6  140
#define    SUD_KBD_CODE_INTERNATIONAL_7  141
#define    SUD_KBD_CODE_INTERNATIONAL_8  142
#define    SUD_KBD_CODE_INTERNATIONAL_9  143
#define    SUD_KBD_CODE_LANG_1           144
#define    SUD_KBD_CODE_LANG_2           145
#define    SUD_KBD_CODE_LANG_3           146
#define    SUD_KBD_CODE_LANG_4           147
#define    SUD_KBD_CODE_LANG_5           148
#define    SUD_KBD_CODE_LANG_6           149
#define    SUD_KBD_CODE_LANG_7           150
#define    SUD_KBD_CODE_LANG_8           151
#define    SUD_KBD_CODE_LANG_9           152
#define    SUD_KBD_CODE_ALTERNATE_ERASE  153
#define    SUD_KBD_CODE_SYS_REQ          154
#define    SUD_KBD_CODE_CANCEL           155
#define    SUD_KBD_CODE_CLEAR            156
#define    SUD_KBD_CODE_PRIOR            157
#define    SUD_KBD_CODE_RETURN           158
#define    SUD_KBD_CODE_SEPARATOR        159
#define    SUD_KBD_CODE_OUT              160
#define    SUD_KBD_CODE_OPER             161
#define    SUD_KBD_CODE_CLEAR_AGAIN      162
#define    SUD_KBD_CODE_CRSEL_PROPS      163
#define    SUD_KBD_CODE_EXSEL            164
/* 165-175 is reserved */
#define    SUD_KBD_CODE_KEYPAD_00        176
#define    SUD_KBD_CODE_KEYPAD_000       177
#define    SUD_KBD_CODE_THOUSANDS_SEPARATOR        178
#define    SUD_KBD_CODE_DECIMAL_SEPARATOR          179
#define    SUD_KBD_CODE_CURRENCY_UNIT    180
#define    SUD_KBD_CODE_CURRENCY_SUBUNIT 181
#define    SUD_KBD_CODE_KEYPAD_LEFT_PARENTHESIS    182
#define    SUD_KBD_CODE_KEYPAD_RIGHT_PARENTHESIS   183
#define    SUD_KBD_CODE_KEYPAD_LEFT_BRACKET        184
#define    SUD_KBD_CODE_KEYPAD_RIGHT_BRACKET       185
#define    SUD_KBD_CODE_KEYPAD_TAB       186
#define    SUD_KBD_CODE_KEYPAD_BACKSPACE 187
#define    SUD_KBD_CODE_KEYPAD_A         188
#define    SUD_KBD_CODE_KEYPAD_B         189
#define    SUD_KBD_CODE_KEYPAD_C         190
#define    SUD_KBD_CODE_KEYPAD_D         191
#define    SUD_KBD_CODE_KEYPAD_E         192
#define    SUD_KBD_CODE_KEYPAD_F         193
#define    SUD_KBD_CODE_KEYPAD_XOR       194
#define    SUD_KBD_CODE_KEYPAD_CARET     195
#define    SUD_KBD_CODE_KEYPAD_PERCENT   196
#define    SUD_KBD_CODE_KEYPAD_LESS      197
#define    SUD_KBD_CODE_KEYPAD_GREATER   198
#define    SUD_KBD_CODE_KEYPAD_AMPERSAND 199
#define    SUD_KBD_CODE_KEYPAD_AMPERSAND_AMPERSAND 200
#define    SUD_KBD_CODE_KEYPAD_VERTICAL_BAR        201
#define    SUD_KBD_CODE_KEYPAD_DOUBLE_VERTICAL_BAR 202
#define    SUD_KBD_CODE_KEYPAD_COLON     203
#define    SUD_KBD_CODE_KEYPAD_POUND     204
#define    SUD_KBD_CODE_KEYPAD_SPACE     205
#define    SUD_KBD_CODE_KEYPAD_AT        206
#define    SUD_KBD_CODE_KEYPAD_EXCLAMATION_MARK    207
#define    SUD_KBD_CODE_KEYPAD_MEMORY_STORE        208
#define    SUD_KBD_CODE_KEYPAD_MEMORY_CLEAR        210
#define    SUD_KBD_CODE_KEYPAD_MEMORY_ADD          211
#define    SUD_KBD_CODE_KEYPAD_MEMORY_SUBTRACT     212
#define    SUD_KBD_CODE_KEYPAD_MEMORY_MULTIPLY     213
#define    SUD_KBD_CODE_KEYPAD_MEMORY_DIVIDE       214
#define    SUD_KBD_CODE_KEYPAD_POS_NEG             215
#define    SUD_KBD_CODE_KEYPAD_CLEAR               216
#define    SUD_KBD_CODE_KEYPAD_CLEAR_ENTRY         217
#define    SUD_KBD_CODE_KEYPAD_BINARY              218
#define    SUD_KBD_CODE_KEYPAD_OCTAL               219
#define    SUD_KBD_CODE_KEYPAD_DECIMAL             220
#define    SUD_KBD_CODE_KEYPAD_HEXADECIMAL         221
/* 222-223 is reserved */
#define    SUD_KBD_CODE_KEYPAD_LEFT_CONTROL        224
#define    SUD_KBD_CODE_KEYPAD_LEFT_SHIFT          225
#define    SUD_KBD_CODE_KEYPAD_LEFT_ALT            226
#define    SUD_KBD_CODE_KEYPAD_LEFT_GUI            227
#define    SUD_KBD_CODE_KEYPAD_RIGHT_CONTROL       228
#define    SUD_KBD_CODE_KEYPAD_RIGHT_SHIFT         229
#define    SUD_KBD_CODE_KEYPAD_RIGHT_ALT           230
#define    SUD_KBD_CODE_KEYPAD_RIGHT_GUI           231
/* 232-65535 is reserved */

typedef void (* SUD_PKBDSETLED)(u8 led);

/* used by smxUSB internally */
int  sud_KBDInit(void);
void sud_KBDRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_KBDGetOps(void);
void *sud_KBDGetInterface(void);
#endif

/* used by high level application */
int  sud_KBDInput(u8 bModifier, u8 *pKey, uint count);
void sud_KbdSetLEDCallback(SUD_PKBDSETLED handler);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_KBD_H */

