/*
 * Copyright (C) 2020 FishSemi Limited
 */

/******************************************************************************
 * @file     pin_name.h
 * @brief    header file for the pin_name
 * @version  V1.0
 * @date     14. May 2020
 ******************************************************************************/
#ifndef _PINNAMES_H
#define _PINNAMES_H


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GP00 = 0,
    GP01,
    GP02,
    GP03,
    GP04,
    GP05,
    GP06,
    GP07,
    GP08,
    GP09,
    GP10,
    GP11,
    GP12,
    GP13,
    GP14,
    GP15,
    GP16,
    GP17,
    GP18,
    GP19,
    GP20,
    GP21,
    GP22,
    GP23,
    GP24,
    GP25,
    GP26,
    GP27,
    GP28,
    GP29,
    GP30,
    GP31,
    GP32,
    GP33,
    GP34,
    GP35,
    GP36,
    GP37,
    GP38,
    GP39,
    GP40,
    GP41,

    CKO0,
    TEST,
    DSIND,
}
pin_name_e;

typedef enum {
    GP04_FUNC_SIM0CLK     = 0,
    GP05_FUNC_SIM0IO      = 0,
    GP06_FUNC_SIM0RST     = 0,
    GP07_FUNC_U1TXD       = 0,
    GP08_FUNC_U1RXD       = 0,
    GP08_FUNC_TESTPIN1    = 3,
    GP09_FUNC_U1CTS       = 0,
    GP10_FUNC_U1RTS       = 0,
    GP11_FUNC_U2TXD       = 0,
    GP11_FUNC_TESTPIN2    = 3,
    GP12_FUNC_U2RXD       = 0,
    GP12_FUNC_TESTPIN3    = 3,
    GP13_FUNC_U3TXD       = 0,
    GP13_FUNC_TESTPIN4    = 3,
    GP14_FUNC_U3RXD       = 0,
    GP14_FUNC_TESTPIN5    = 3,
    GP15_FUNC_I2C0SCL     = 0,
    GP15_FUNC_TESTPIN6    = 3,
    GP16_FUNC_I2C0SDA     = 0,
    GP16_FUNC_TESTPIN7    = 3,
    GP17_FUNC_I2C1SCL     = 0,
    GP18_FUNC_I2C1SDA     = 0,
    GP19_FUNC_S0TXD       = 0,
    GP19_FUNC_S2TXD       = 3,
    GP20_FUNC_S0RXD       = 0,
    GP20_FUNC_S2RXD       = 3,
    GP21_FUNC_S0CLK       = 0,
    GP21_FUNC_S2CLK       = 3,
    GP22_FUNC_S0SSN       = 0,
    GP22_FUNC_S2SSN       = 3,
    GP23_FUNC_S1TXD       = 0,
    GP23_FUNC_TESTPIN8    = 3,
    GP24_FUNC_S1RXD       = 0,
    GP24_FUNC_TESTPIN9    = 3,
    GP25_FUNC_S1CLK       = 0,
    GP25_FUNC_TESTPIN10   = 3,
    GP26_FUNC_S1SSN       = 0,
    GP26_FUNC_TESTPIN11   = 3,
    GP27_FUNC_PWM0        = 0,
    GP27_FUNC_TESTPIN12   = 3,
    GP28_FUNC_PWM1        = 0,
    GP28_FUNC_TESTPIN13   = 3,
    GP29_FUNC_PWM2        = 0,
    GP29_FUNC_TESTPIN14   = 3,
    GP30_FUNC_PWM3        = 0,
    GP30_FUNC_TESTPIN15   = 3,
    GP31_FUNC_JTAGTCK     = 0,
    GP32_FUNC_JTAGTDI     = 0,
    GP33_FUNC_TPGPO0      = 3,
    GP34_FUNC_TPGPO1      = 3,
    GP35_FUNC_TPGPO2      = 3,
    GP36_FUNC_TPGPO3      = 3,
    GP37_FUNC_TPGPO4      = 3,
    GP41_FUNC_TPGPO5      = 3,

    CKO0_FUNC_CKO0        = 0,
    TEST_FUNC_PWR         = 0,
    TEST_FUNC_RFIF        = 1,

    GP00_FUNC_BUCKOCPTST  = 1,
    GP01_FUNC_BUCKRCPTST  = 1,
    GP02_FUNC_BUCKRTST    = 1,
    GP03_FUNC_BUCKSSTST   = 1,

    DSIND_FUNC_DSIND      = 0,
    DSIND_FUNC_BUCKCKATE  = 1,

    PIN_FUNC_GPIO         = 2,
    PIN_FUNC_GPIO_AON     = 0,
} pin_func_e;

typedef enum {
    PORTA = 0,
} port_name_e;

#ifdef __cplusplus
}
#endif

#endif
