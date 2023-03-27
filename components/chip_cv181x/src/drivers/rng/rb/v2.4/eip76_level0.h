/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* eip76_level0.h
 *
 * This file contains all the macros and functions that allow
 * access to the EIP76 registers and to build the values
 * read or written to the registers.
 *
 */


#ifndef INCLUDE_GUARD_EIP76_LEVEL0_H
#define INCLUDE_GUARD_EIP76_LEVEL0_H

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_eip76.h"

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // BIT definitions, bool, uint32_t

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t
#include "device_rw.h"          // Read32, Write32

#include "eip76_test.h"         // EIP76_RunKAT_Result_t,
                                // EIP76_PokerKAT_Result_t

/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// EIP-76 TRNG device EIP number (0x4C) and complement (0xB3)
#define EIP76_SIGNATURE                   ((uint16_t)0xB34C)

// Events mask for TRNG_STATUS register,
// must match EIP76_Events_t
#define EIP76_EVENTS_MASK                 ((uint16_t)0x62FF)


// Maximum 24 FRO's can be supported
#define EIP76_MAX_FRO_AVAILABLE_MASK      ((uint32_t)(BIT_24-1))

// EIP-76 TRNG_INTACK register event mask
#define EIP76_INTACK_MASK                 EIP76_EVENTS_MASK
#define EIP76_INTACK_CLEAR_ALL_MASK       EIP76_INTACK_MASK
#define EIP76_INTACK_CLEAR_PRM            (0x000060FC)

// Read/Write register constants

// Byte offsets of the EIP76 registers
#define EIP76_REGISTER_SPACING     4

#define EIP76_REG_INPUT_0         (0x00 * EIP76_REGISTER_SPACING)
#define EIP76_REG_INPUT_1         (0x01 * EIP76_REGISTER_SPACING)
#define EIP76_REG_INPUT_2         (0x02 * EIP76_REGISTER_SPACING)
#define EIP76_REG_INPUT_3         (0x03 * EIP76_REGISTER_SPACING)
#define EIP76_REG_OUTPUT_0        (0x00 * EIP76_REGISTER_SPACING)
#define EIP76_REG_OUTPUT_1        (0x01 * EIP76_REGISTER_SPACING)
#define EIP76_REG_OUTPUT_2        (0x02 * EIP76_REGISTER_SPACING)
#define EIP76_REG_OUTPUT_3        (0x03 * EIP76_REGISTER_SPACING)
#define EIP76_REG_STATUS          (0x04 * EIP76_REGISTER_SPACING)
#define EIP76_REG_INTACK          (0x04 * EIP76_REGISTER_SPACING)
#define EIP76_REG_CONTROL         (0x05 * EIP76_REGISTER_SPACING)
#define EIP76_REG_CONFIG          (0x06 * EIP76_REGISTER_SPACING)
#define EIP76_REG_ALARMCNT        (0x07 * EIP76_REGISTER_SPACING)
#define EIP76_REG_FROENABLE       (0x08 * EIP76_REGISTER_SPACING)
#define EIP76_REG_FRODETUNE       (0x09 * EIP76_REGISTER_SPACING)
#define EIP76_REG_ALARMMASK       (0x0A * EIP76_REGISTER_SPACING)
#define EIP76_REG_ALARMSTOP       (0x0B * EIP76_REGISTER_SPACING)
#define EIP76_REG_SPB             (0x0E * EIP76_REGISTER_SPACING)
#define EIP76_REG_RAW_L           (0x0C * EIP76_REGISTER_SPACING)
#define EIP76_REG_RAW_H           (0x0D * EIP76_REGISTER_SPACING)
#define EIP76_REG_COUNT           (0x0F * EIP76_REGISTER_SPACING)
#define EIP76_REG_RUN_CNT         (0x10 * EIP76_REGISTER_SPACING)
#define EIP76_REG_COND_0          (0x10 * EIP76_REGISTER_SPACING)
#define EIP76_REG_COND_1          (0x11 * EIP76_REGISTER_SPACING)
#define EIP76_REG_COND_2          (0x12 * EIP76_REGISTER_SPACING)
#define EIP76_REG_COND_3          (0x13 * EIP76_REGISTER_SPACING)
#define EIP76_REG_COND_4          (0x14 * EIP76_REGISTER_SPACING)
#define EIP76_REG_COND_5          (0x15 * EIP76_REGISTER_SPACING)
#define EIP76_REG_COND_6          (0x16 * EIP76_REGISTER_SPACING)
#define EIP76_REG_COND_7          (0x17 * EIP76_REGISTER_SPACING)
#define EIP76_REG_KEY_0           (0x10 * EIP76_REGISTER_SPACING)
#define EIP76_REG_KEY_1           (0x11 * EIP76_REGISTER_SPACING)
#define EIP76_REG_KEY_2           (0x12 * EIP76_REGISTER_SPACING)
#define EIP76_REG_KEY_3           (0x13 * EIP76_REGISTER_SPACING)
#define EIP76_REG_KEY_4           (0x14 * EIP76_REGISTER_SPACING)
#define EIP76_REG_KEY_5           (0x15 * EIP76_REGISTER_SPACING)
#define EIP76_REG_KEY_6           (0x16 * EIP76_REGISTER_SPACING)
#define EIP76_REG_KEY_7           (0x17 * EIP76_REGISTER_SPACING)
#define EIP76_REG_PS_AI_0         (0x10 * EIP76_REGISTER_SPACING)
#define EIP76_REG_RUN_1           (0x11 * EIP76_REGISTER_SPACING)
#define EIP76_REG_RUN_2           (0x12 * EIP76_REGISTER_SPACING)
#define EIP76_REG_RUN_3           (0x13 * EIP76_REGISTER_SPACING)
#define EIP76_REG_RUN_4           (0x14 * EIP76_REGISTER_SPACING)
#define EIP76_REG_RUN_5           (0x15 * EIP76_REGISTER_SPACING)
#define EIP76_REG_RUN_6           (0x16 * EIP76_REGISTER_SPACING)
#define EIP76_REG_MONOBITCNT      (0x17 * EIP76_REGISTER_SPACING)
#define EIP76_REG_V_0             (0x18 * EIP76_REGISTER_SPACING)
#define EIP76_REG_V_1             (0x19 * EIP76_REGISTER_SPACING)
#define EIP76_REG_V_2             (0x1A * EIP76_REGISTER_SPACING)
#define EIP76_REG_V_3             (0x1B * EIP76_REGISTER_SPACING)
#define EIP76_REG_POKER_3_0       (0x18 * EIP76_REGISTER_SPACING)
#define EIP76_REG_POKER_7_4       (0x19 * EIP76_REGISTER_SPACING)
#define EIP76_REG_POKER_B_8       (0x1A * EIP76_REGISTER_SPACING)
#define EIP76_REG_POKER_F_C       (0x1B * EIP76_REGISTER_SPACING)
#define EIP76_REG_PS_AI_11        (0x1B * EIP76_REGISTER_SPACING)
#define EIP76_REG_TEST            (0x1C * EIP76_REGISTER_SPACING)
#define EIP76_REG_BLOCKCNT        (0x1D * EIP76_REGISTER_SPACING)
#define EIP76_REG_OPTION          (0x1E * EIP76_REGISTER_SPACING)
#define EIP76_REG_EIP_REV         (0x1F * EIP76_REGISTER_SPACING)

// Constants for the bits in EIP76_REG_CONTROL register
#define EIP76_CONTROL_ENABLE_RESEED              ((uint32_t)(BIT_15))
#define EIP76_SHA2_NOISE_BLOCKS                  (0x32)
#define EIP76_SHA1_NOISE_BLOCKS                  (0x2C)
#define EIP76_SHA_RANDOM_NOISE_BLOCKS            (0x02)
#define EIP76_ENABLE_TEST_MODE                   (0x100)
#define EIP76_CLEAR_CONTROL_REG                  (0x00000000)
#define EIP76_MIN_CYCLE                          ((uint32_t)(BIT_16))
#define EIP76_CONTROL_ENABLE_TEST_MODE           ((uint32_t)(BIT_8))
#define EIP76_CONTROL_ENABLE_TRNG                ((uint32_t)(BIT_10))
#define EIP76_CONTROL_NO_WHITENING               ((uint32_t)(BIT_11))
#define EIP76_CONTROL_ENABLE_TRNG_AND_TEST_MODE  (0x00000500)
#define EIP76_ENABLE_TRNG_AND_PP                 (0x00001400)
#define EIP76_CONTROL_POSTPROCESSOR_ENABLE       ((uint32_t)(BIT_12))
#define EIP76_CONTROL_READY_MASK                 ((uint32_t)(BIT_0))
#define EIP76_REQUEST_DATA                       ((uint32_t)(BIT_16))
#define EIP76_DEFAULT_REQUEST_BLOCK_NUMBER       ((uint32_t)(BIT_20))

#define EIP76_REQUEST_DATA_MAX_BLK_COUNT         4095

// Constants for the bits in EIP76_REG_STATUS register
#define EIP76_STATUS_READY                  ((uint32_t)(BIT_0))
#define EIP76_STATUS_TEST_READY             ((uint32_t)(BIT_8))
#define EIP76_STATUS_RESEED_AI              ((uint32_t)(BIT_10))

// Constants for the bits in the EIP76_REG_TEST register
#define EIP76_TEST_CONT_POKER               ((uint32_t)(BIT_4))
#define EIP76_TEST_RUN_POKER                ((uint32_t)(BIT_5))
#define EIP76_TEST_KNOWN_NOISE              ((uint32_t)(BIT_5))
#define EIP76_TEST_POST_PROC                ((uint32_t)(BIT_6))
#define EIP76_TEST_SP_800_90                ((uint32_t)(BIT_7))
#define EIP76_TEST_IRQ                      ((uint32_t)(BIT_31))
#define EIP76_TEST_SHA                      ((uint32_t)(BIT_15))
#define EIP76_TEST_REPCOUNTORADAPTIVE       (0x00004030)

// Constants for the bits in the EIP76_REG_SPB register and SPB tests
#define EIP76_SHOW_COUNTERS_AND_VALUES      (0x30000000)
#define EIP76_SHOW_COUNTERS                 ((uint32_t)(BIT_28))
#define EIP76_SHOW_VALUES                   ((uint32_t)(BIT_29))
#define EIP76_CUTOFF_REP                    (0x3f)
#define EIP76_CUTOFF_APROP_64               (0x3f)
#define EIP76_CUTOFF_APROP_512              (0xff)
#define EIP76_CUTOFF_APROP_4k               (0x2ff)

#define EIP76_CUTOFF_DEFAULT                (0x08C0331F)
// Value for HW2.3 and later (Aprop-512)
#define EIP76_CUTOFF_512_DEFAULT            (0x014F331F)


// Constants for the bits in the EIP76_REG_ALARMCNT register
#define EIP76_ALARMCNT_STALL_RUN_POKER      ((uint32_t)(BIT_15))
#define EIP76_ALARMCNT_FRO_TEST_ENABLE      ((uint32_t)(BIT_21))
#define EIP76_ALARM_BIT_MASK                ((uint32_t)(BIT_8-1))
#define EIP76_MAX_ALARM_THRESHOLD           255
#define EIP76_MIN_ALARM_THRESHOLD           1

// Constants for the bits in the EIP76_REG_OPTIONS register
#define EIP76_APROP_512_OPTION             ((uint32_t)(BIT_21))
#define EIP76_UPDATE_FRO_OPTION            ((uint32_t)(BIT_22))



// Other constants
#define EIP76_RUN_CNT_TEST_COUNT_MASK       ((uint32_t)(BIT_15-1))
#define EIP76_MONOBITCNT_MASK               ((uint32_t)(BIT_17-1))
#define EIP76_AUTO_DETUNE_ENABLE            ((uint32_t)(BIT_23))
#define EIP76_LOAD_THESHOLD                 ((uint32_t)(BIT_31))


/*----------------------------------------------------------------------------
 * EIP76_Read32
 *
 * This routine writes to a Register location in the EIP76.
 */
static inline uint32_t
EIP76_Read32(
        Device_Handle_t Device,
        const unsigned int Offset)
{
    return Device_Read32((void* const)RB_EIP76_BASE, Offset);
}


/*----------------------------------------------------------------------------
 * EIP76_Write32
 *
 * This routine writes to a Register location in the EIP76.
 */
static inline void
EIP76_Write32(
        Device_Handle_t Device,
        const unsigned int Offset,
        const uint32_t Value)
{
    Device_Write32((void* const)RB_EIP76_BASE, Offset, Value);
}


/*----------------------------------------------------------------------------
 * EIP76_SPB_WR and EIP76_SPB_RD
 *
 * These routines read and write from and to the SPB test register
 */

static inline void
EIP76_SPB_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_SPB, Value);
}

static inline uint32_t
EIP76_SPB_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_SPB);
}

/*----------------------------------------------------------------------------
 * EIP76_INPUT_CF_0_WR till EIP76_INPUT_CF_7_WR
 *
 * These routines write to the Conditioning function test registers
 */
static inline void
EIP76_INPUT_CF_0_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_COND_0, Value);
}

static inline void
EIP76_INPUT_CF_1_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_COND_1, Value);
}

static inline void
EIP76_INPUT_CF_2_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_COND_2, Value);
}

static inline void
EIP76_INPUT_CF_3_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_COND_3, Value);
}

static inline void
EIP76_INPUT_CF_4_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_COND_4, Value);
}

static inline void
EIP76_INPUT_CF_5_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_COND_5, Value);
}

static inline void
EIP76_INPUT_CF_6_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_COND_6, Value);
}

static inline void
EIP76_INPUT_CF_7_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_COND_7, Value);
}

/*----------------------------------------------------------------------------
 * EIP76_INPUT_CF_0_RD till EIP76_INPUT_CF_7_RD
 *
 * These routines read from the Conditioning function test registers
 */
static inline uint32_t
EIP76_INPUT_CF_0_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_COND_0);
}

static inline uint32_t
EIP76_INPUT_CF_1_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_COND_1);
}

static inline uint32_t
EIP76_INPUT_CF_2_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_COND_2);
}

static inline uint32_t
EIP76_INPUT_CF_3_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_COND_3);
}

static inline uint32_t
EIP76_INPUT_CF_4_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_COND_4);
}

static inline uint32_t
EIP76_INPUT_CF_5_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_COND_5);
}

static inline uint32_t
EIP76_INPUT_CF_6_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_COND_6);
}

static inline uint32_t
EIP76_INPUT_CF_7_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_COND_7);
}

/*----------------------------------------------------------------------------
 * EIP76_MAINSHIFTREG_L_WR and EIP76_MAINSHIFTREG_H_WR
 *
 * These routines write to the mainshift registers
 */
static inline void
EIP76_MAINSHIFTREG_L_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_RAW_L, Value);
}

static inline void
EIP76_MAINSHIFTREG_H_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_RAW_H, Value);
}

static inline void
EIP76_INPUT_0_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_INPUT_0, Value);
}

static inline void
EIP76_INPUT_1_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_INPUT_1, Value);
}


static inline void
EIP76_INPUT_2_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_INPUT_2, Value);
}


static inline void
EIP76_INPUT_3_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_INPUT_3, Value);
}


static inline uint32_t
EIP76_OUTPUT_0_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_OUTPUT_0);
}


static inline uint32_t
EIP76_OUTPUT_1_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_OUTPUT_1);
}


static inline uint32_t
EIP76_OUTPUT_2_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_OUTPUT_2);
}


static inline uint32_t
EIP76_OUTPUT_3_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_OUTPUT_3);
}


static inline void
EIP76_CONTROL_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_CONTROL, Value);
}


static inline uint32_t
EIP76_CONTROL_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_CONTROL);
}


static inline uint32_t
EIP76_STATUS_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_STATUS);
}


static inline void
EIP76_COUNT_WR(Device_Handle_t Device,
               const uint32_t Value)
{
    return EIP76_Write32(Device, EIP76_REG_COUNT, Value);
}


static inline bool
EIP76_STATUS_IS_READY(
        uint32_t StatusReg_val)
{
    return (bool)(StatusReg_val & EIP76_STATUS_READY);
}

/* Read Aprop512 bit in options register */
static inline bool
EIP76_APROP512_RD(
        Device_Handle_t Device)
{
    uint32_t RevRegVal;

    RevRegVal = EIP76_Read32(Device, EIP76_REG_OPTION);
    return (RevRegVal & EIP76_APROP_512_OPTION) != 0;
}

/* Read Update_FRO bit in options register */
static inline bool
EIP76_UPDATED_FRO_RD(
        Device_Handle_t Device)
{
    uint32_t RevRegVal;

    RevRegVal = EIP76_Read32(Device, EIP76_REG_OPTION);
    return (RevRegVal & EIP76_UPDATE_FRO_OPTION) != 0;
}


// Read SHA version from options register
// 0 = none, 1 = SHA1 and 2 is SHA2
static inline uint32_t
EIP76_SHA_VERSION_RD(
        Device_Handle_t Device)
{
    uint32_t RevRegVal;

    RevRegVal = EIP76_Read32(Device, EIP76_REG_OPTION);
    return ((RevRegVal >> 17) & 0x03);
}

static inline void
EIP76_CONFIG_NOISEBLK_WR(
        Device_Handle_t Device,
        const uint32_t Noise_BLK_Value)
        {
            EIP76_Write32(Device, EIP76_REG_CONFIG, Noise_BLK_Value);
        };

/* Write TRNG_CONFIG register */
static inline void
EIP76_CONFIG_WR(
        Device_Handle_t Device,
        const uint32_t MinRefillCycles,
        const uint32_t SampleDivider,
        const uint32_t SecureReadTimeout,
        const uint32_t MaxRefillCycles,
        const uint32_t SampleScale)
{
    uint32_t Value = 0;

    Value = ((MaxRefillCycles & 0xFFFF) << 16) |
            ((SecureReadTimeout & 0xF)  << 12) |
            ((SampleDivider & 0xF)      << 8) |
            ((SampleScale & 0x3)        << 6) |
            ( MinRefillCycles & 0x3F);

    EIP76_Write32(Device, EIP76_REG_CONFIG, Value);
}


static inline uint32_t
EIP76_CONFIG_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_CONFIG);
}


static inline void
EIP76_ALARMCNT_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_ALARMCNT, Value);
}


static inline void
EIP76_ALARMCNT_WR_CONFIG(
        Device_Handle_t Device,
        const uint32_t AlarmThreshold,
        const bool fStallRunPoker,
        const uint32_t ShutdownThreshold,
        const bool fShutdownFatal)
{
    uint32_t Value = 0;

    Value = ((uint32_t)fShutdownFatal << 23) |
            (ShutdownThreshold << 16) |
            ((uint32_t)fStallRunPoker << 15) |
            AlarmThreshold;

#ifdef EIP76_OLD_FRO_SUPPORT
    //  Write ALARMCNT register with FRO Test bit if HW supports this.
    // (HW3.0 and higher)
    if (EIP76_UPDATED_FRO_RD(Device))
    {
        Value |= EIP76_ALARMCNT_FRO_TEST_ENABLE;
    }
#endif

    EIP76_ALARMCNT_WR(Device, Value);
}


/* Read TRNG_ALARMCNT register */
static inline uint32_t
EIP76_ALARMCNT_RD(
        Device_Handle_t  Device)
{
    return EIP76_Read32(Device, EIP76_REG_ALARMCNT);
}


/* Read TRNG_RUN_CNT register */
static inline uint32_t
EIP76_RUNCNT_RD(
        Device_Handle_t  Device)
{
    return EIP76_Read32(Device, EIP76_REG_RUN_CNT);
}


/* Write TRNG_FROENABLE register */
static inline void
EIP76_FROENABLE_WR(
        Device_Handle_t Device,
        const uint32_t FROEnabledMask)
{
    EIP76_Write32(Device, EIP76_REG_FROENABLE, FROEnabledMask);
}


/* Read TRNG_FROENABLE register */
static inline uint32_t
EIP76_FROENABLE_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_FROENABLE);
}


/* Write TRNG_FRODETUNE register */
static inline void
EIP76_FRODETUNE_WR(
        Device_Handle_t Device,
        const uint32_t DetuneValue)
{
    EIP76_Write32(Device, EIP76_REG_FRODETUNE, DetuneValue);
}


/* Read TRNG_FRODETUNE register */
static inline uint32_t
EIP76_FRODETUNE_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_FRODETUNE);
}


/* Write TRNG_FRODETUNE register, this function disables the FRO's */
static inline void
EIP76_FRO_DETUNE(
        Device_Handle_t Device,
        const uint32_t FroDetuneMask)
{
    uint32_t Value;

    // FRO's must be disabled before the de-tune operation
    EIP76_FROENABLE_WR(Device, 0);

    Value = EIP76_FRODETUNE_RD(Device);

    // Toggle the bits for the requested FRO's
    Value = FroDetuneMask ^ Value;

    EIP76_FRODETUNE_WR(Device, Value);
}


/* Write TRNG_ALARMSTOP register */
static inline void
EIP76_ALARMSTOP_WR(
        Device_Handle_t Device,
        const uint32_t FroAlarmStop)
{
    EIP76_Write32(Device, EIP76_REG_ALARMSTOP, FroAlarmStop);
}


/* Read TRNG_ALARMSTOP register */
static inline uint32_t
EIP76_ALARMSTOP_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_ALARMSTOP);
}


/* Read TRNG_BLOCKCNT register */
static inline uint32_t
EIP76_BLOCKCNT_RD_BLOCKCOUNT(
        Device_Handle_t Device)
{
    return (EIP76_Read32(Device, EIP76_REG_BLOCKCNT) >> 4);
}


/* Write TRNG_ALARMMASK register */
static inline void
EIP76_ALARMMASK_WR(
        Device_Handle_t Device,
        const uint32_t FroAlarmMask)
{
    EIP76_Write32(Device, EIP76_REG_ALARMMASK, FroAlarmMask);
}


/* Read TRNG_ALARMMASK register */
static inline uint32_t
EIP76_ALARMMASK_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_ALARMMASK);
}


/* Write TRNG_INTACK register (write-only) */
static inline void
EIP76_INTACK_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_INTACK, Value);
}


static inline bool
EIP76_REV_SIGNATURE_MATCH(
        const uint32_t Rev)
{
    return (((uint16_t)Rev) == EIP76_SIGNATURE);
}


static inline uint32_t
EIP76_BUFFER_SIZE_RD(
        Device_Handle_t Device)
{
    uint32_t RevRegVal;

    RevRegVal = EIP76_Read32(Device, EIP76_REG_OPTION);

    RevRegVal = (uint8_t)((RevRegVal >> 12) & 0x07);

    // Remove value for 0 and 1
    return (1 << RevRegVal) & 0xFFFC;
}


static inline void
EIP76_EIP_REV_RD(
        Device_Handle_t Device,
        uint8_t * const EipNumber,
        uint8_t * const ComplmtEipNumber,
        uint8_t * const HWPatchLevel,
        uint8_t * const MinHWRevision,
        uint8_t * const MajHWRevision)
{
    uint32_t RevRegVal;

    RevRegVal = EIP76_Read32(Device, EIP76_REG_EIP_REV);

    *MajHWRevision     = (uint8_t)((RevRegVal >> 24) & 0x0f);
    *MinHWRevision     = (uint8_t)((RevRegVal >> 20) & 0x0f);
    *HWPatchLevel      = (uint8_t)((RevRegVal >> 16) & 0x0f);
    *ComplmtEipNumber  = (uint8_t)((RevRegVal >> 8)  & 0xff);
    *EipNumber         = (uint8_t)((RevRegVal)       & 0xff);
}


static inline void
EIP76_OPTIONS_RD(
        Device_Handle_t Device,
        uint8_t * const PostProcessorType,
        uint8_t * const NofFRO,
        uint8_t * const ConditioningFunction,
        uint8_t * const BufferSize,
        uint8_t * const AIS31Test,
        uint8_t * const AutoDetuneOption,
        uint8_t * const Aprop512,
        uint8_t * const ImprovedFRO)
{
    uint32_t RevRegVal;

    RevRegVal = EIP76_Read32(Device, EIP76_REG_OPTION);

    *ImprovedFRO          = (uint8_t)((RevRegVal >> 22)  & 0x01);
    *Aprop512             = (uint8_t)((RevRegVal >> 21)  & 0x01);
    *AutoDetuneOption     = (uint8_t)((RevRegVal >> 19)  & 0x03);
    *ConditioningFunction = (uint8_t)((RevRegVal >> 17)  & 0x03);
    *AIS31Test            = (uint8_t)((RevRegVal >> 16)  & 0x01);
    *BufferSize           = (uint8_t)((RevRegVal >> 12)  & 0x07);
    *NofFRO               = (uint8_t)((RevRegVal >> 6)   & 0x3f);
    *PostProcessorType    = (uint8_t)((RevRegVal)        & 0x07);

    // Calculate buffer size from address bits
    // Clear sizee for 0 and 1
    *BufferSize = (1 << *BufferSize) & 0xFFFC;

    // See if BC_DF has been enabled.
    if (*PostProcessorType ==  EIP76_POST_PROCESSOR_SP800_90 &&
        *ConditioningFunction == 0)
    {
        *PostProcessorType = EIP76_POST_PROCESSOR_BC_DF;
    }
}


static inline void
EIP76_OPTIONS_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_OPTION, Value);
}



/* Write TRNG_TEST register */
static inline void
EIP76_TEST_WR(
        Device_Handle_t Device,
        const uint32_t Value)
{
    EIP76_Write32(Device, EIP76_REG_TEST, Value);
}


/* Read TRNG_TEST register */
static inline uint32_t
EIP76_TEST_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_TEST);
}


#ifndef EIP76_MONOBIT_DISABLE
/* Read TRNG_MONOBITCNT register */
static inline uint32_t
EIP76_MONOBITCNT_RD(
        Device_Handle_t Device)
{
    return EIP76_Read32(Device, EIP76_REG_MONOBITCNT);
}
#endif // not EIP76_MONOBIT_DISABLE


/* Read Run test registers */
static inline void
EIP76_RUN_TEST_READ(
        Device_Handle_t Device,
        EIP76_RunKAT_Result_t * const RunKATResult_p)
{
    uint32_t RegVal;

    RegVal = EIP76_Read32(Device, EIP76_REG_RUN_1);
    RunKATResult_p->run_1_count_zeroes = RegVal & (BIT_12-1);
    RunKATResult_p->run_1_count_ones = (RegVal >> 16) & (BIT_12-1);

    RegVal = EIP76_Read32(Device, EIP76_REG_RUN_2);
    RunKATResult_p->run_2_count_zeroes = RegVal & (BIT_11-1);
    RunKATResult_p->run_2_count_ones = (RegVal >> 16) & (BIT_11-1);

    RegVal = EIP76_Read32(Device, EIP76_REG_RUN_3);
    RunKATResult_p->run_3_count_zeroes = RegVal & (BIT_10-1);
    RunKATResult_p->run_3_count_ones = (RegVal >> 16) & (BIT_10-1);

    RegVal = EIP76_Read32(Device, EIP76_REG_RUN_4);
    RunKATResult_p->run_4_count_zeroes = RegVal & (BIT_9-1);
    RunKATResult_p->run_4_count_ones = (RegVal >> 16) & (BIT_9-1);

    RegVal = EIP76_Read32(Device, EIP76_REG_RUN_5);
    RunKATResult_p->run_5_count_zeroes = RegVal & (BIT_8-1);
    RunKATResult_p->run_5_count_ones = (RegVal >> 16) & (BIT_8-1);

    RegVal = EIP76_Read32(Device, EIP76_REG_RUN_6);
    RunKATResult_p->run_6_count_zeroes = RegVal & (BIT_8-1);
    RunKATResult_p->run_6_count_ones = (RegVal >> 16) & (BIT_8-1);
}


/* Read Poker test registers */
static inline void
EIP76_POKER_TEST_READ(
        Device_Handle_t Device,
        EIP76_PokerKAT_Result_t * const PokerKATResult_p)
{
    uint32_t RegVal;

    RegVal = EIP76_Read32(Device, EIP76_REG_POKER_3_0);
    PokerKATResult_p->poker_count_0 = RegVal         & (BIT_8-1);
    PokerKATResult_p->poker_count_1 = (RegVal >> 8)  & (BIT_8-1);
    PokerKATResult_p->poker_count_2 = (RegVal >> 16) & (BIT_8-1);
    PokerKATResult_p->poker_count_3 = (RegVal >> 24) & (BIT_8-1);

    RegVal = EIP76_Read32(Device, EIP76_REG_POKER_7_4);
    PokerKATResult_p->poker_count_4 = RegVal         & (BIT_8-1);
    PokerKATResult_p->poker_count_5 = (RegVal >> 8)  & (BIT_8-1);
    PokerKATResult_p->poker_count_6 = (RegVal >> 16) & (BIT_8-1);
    PokerKATResult_p->poker_count_7 = (RegVal >> 24) & (BIT_8-1);

    RegVal = EIP76_Read32(Device, EIP76_REG_POKER_B_8);
    PokerKATResult_p->poker_count_8 = RegVal         & (BIT_8-1);
    PokerKATResult_p->poker_count_9 = (RegVal >> 8)  & (BIT_8-1);
    PokerKATResult_p->poker_count_a = (RegVal >> 16) & (BIT_8-1);
    PokerKATResult_p->poker_count_b = (RegVal >> 24) & (BIT_8-1);

    RegVal = EIP76_Read32(Device, EIP76_REG_POKER_F_C);
    PokerKATResult_p->poker_count_c = RegVal         & (BIT_8-1);
    PokerKATResult_p->poker_count_d = (RegVal >> 8)  & (BIT_8-1);
    PokerKATResult_p->poker_count_e = (RegVal >> 16) & (BIT_8-1);
    PokerKATResult_p->poker_count_f = (RegVal >> 24) & (BIT_8-1);
}


/* Write Key registers */
static inline void
EIP76_KEY_WR(
        Device_Handle_t Device,
        const uint32_t * Data_p,
        const unsigned int WordCount)
{
    unsigned int i;

    for(i = 0; i < WordCount; i++)
        EIP76_Write32(Device,
                      EIP76_REG_KEY_0 + i * sizeof(uint32_t),
                      Data_p[i]);
}


/* Write V value registers */
static inline void
EIP76_V_WR(
        Device_Handle_t Device,
        const uint32_t * Data_p,
        const unsigned int WordCount)
{
    unsigned int i;

    for(i = 0; i < WordCount; i++)
        EIP76_Write32(Device,
                      EIP76_REG_V_0 + i * sizeof(uint32_t),
                      Data_p[i]);
}


#ifdef EIP76_BLOCKS_THRESHOLD_OPTION
// Read STATUS register for blocks threshold value
static inline uint32_t
EIP76_BLOCKS_THRESHOLD_RD(
        Device_Handle_t Device)
{
    return ((EIP76_Read32(Device, EIP76_REG_STATUS) >> 24) & 0x7F);
}


// Write INTACK register to set the blocks threshold
static inline void
EIP76_BLOCKS_THRESHOLD_WR(
        Device_Handle_t Device,
        const uint32_t Threshold)
{
    EIP76_Write32(Device, EIP76_REG_INTACK, (Threshold << 24) | EIP76_LOAD_THESHOLD);
}
#endif // EIP76_BLOCKS_THRESHOLD_OPTION


#ifdef EIP76_AUTO_DETUNE_OPTION
/* Read OPTIONS register for detune_counter*/
static inline uint32_t
EIP76_AUTODETUNE_CNT_RD(
        Device_Handle_t Device)
{
    return (EIP76_Read32(Device, EIP76_REG_OPTION) >> 24);
}


/* Clear detune_counter in OPTIONS register */
static inline void
EIP76_AUTODETUNE_CNT_CLR(
        Device_Handle_t Device)
{
    EIP76_Write32(Device, EIP76_REG_OPTION, 0xFF000000);
}

#endif // EIP76_AUTO_DETUNE_OPTION


#endif /* INCLUDE_GUARD_EIP76_LEVEL0_H */

/* end of file eip76_level0.h */
