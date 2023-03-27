/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* eip28_level0.h
 *
 * This file contains all the macros and inline functions that allow access
 * to the EIP28 registers and to build the values read or written to the
 * registers and to access the PKA RAM.
 */



#ifndef INCLUDE_GUARD_EIP28_LEVEL0_H
#define INCLUDE_GUARD_EIP28_LEVEL0_H

#include "basic_defs.h"         // BIT_*, bool, uint32_t
#include "device_types.h"       // Device_Handle_t
#include "device_rw.h"          // Device_Read32, Write32
#include "rambus.h"


/*---------------------------------------------------------------------------
 * EIP28_Read32
 *
 * This routine writes to a Register or RAM location in the EIP28,
 * Endianness swapping is taken care of by Driver Framework implementation.
 */
static inline uint32_t
EIP28_Read32(
        Device_Handle_t Device,
        const unsigned int Offset)
{
    return Device_Read32((void * const)Device, Offset);
}


/*---------------------------------------------------------------------------
 * EIP28_Write32
 *
 * This routine writes to a Register or RAM location in the EIP28,
 * Endianness swapping is taken care of by Driver Framework implementation.
 */
static inline void
EIP28_Write32(
        Device_Handle_t Device,
        const unsigned int Offset,
        const uint32_t Value)
{
    Device_Write32((void * const)Device, Offset, Value);
}


/*--------------------------------------------------------------------------
 * Read/Write PKA RAM macros
 */


static inline uint32_t
EIP28_PKARAM_RD(
        Device_Handle_t Device,
        const unsigned int WordOffset)
{
    return EIP28_Read32(Device, (WordOffset << 2) | EIP28_OFFSET_PKARAM);
}


static inline void
EIP28_PKARAM_WR(
        Device_Handle_t Device,
        const unsigned int WordOffset,
        const uint32_t Value)
{
    EIP28_Write32(Device, (WordOffset << 2) | EIP28_OFFSET_PKARAM, Value);
}

static inline uint32_t
EIP28_PKARAM_FW_RD(
        Device_Handle_t Device,
        const unsigned int WordOffset)
{
    return *(uint32_t *)((uint64_t)Device + ((WordOffset << 2) | EIP28_OFFSET_PKARAM));
    // return EIP28_Read32(Device, (WordOffset << 2) | EIP28_OFFSET_PKARAM);
}


static inline void
EIP28_PKARAM_FW_WR(
        Device_Handle_t Device,
        const unsigned int WordOffset,
        const uint32_t Value)
{
    // EIP28_Write32(Device, (WordOffset << 2) | EIP28_OFFSET_PKARAM, Value);
    *(uint32_t *)((uint64_t)Device + ((WordOffset << 2) | EIP28_OFFSET_PKARAM)) = Value;
}


/* combine 4 bytes into a 32bit word */
// b3 must be the  most significant byte,
// ends up at bits 31:24 in the 32bit word
// b0 must be the least significant byte,
// ends up at bits 07:00 in the 32bit word
static inline uint32_t
EIP28_PKARAM_MAKE_WORD_FROM_BYTES(
        const uint8_t b3,
        const uint8_t b2,
        const uint8_t b1,
        const uint8_t b0)
{
    return (uint32_t)b3 << 24 |
           (uint32_t)b2 << 16 |
           (uint32_t)b1 << 8 |
           (uint32_t)b0;
}

static inline uint32_t
EIP28_PKARAM_MAKE_WORD_FROM_BYTESPTR_LSB_FIRST(
        const uint8_t * const Bytes_p)
{
    return EIP28_PKARAM_MAKE_WORD_FROM_BYTES(
                    Bytes_p[3],
                    Bytes_p[2],
                    Bytes_p[1],
                    Bytes_p[0]);
}


static inline uint32_t
EIP28_PKARAM_MAKE_WORD_FROM_BYTESPTR_MSB_FIRST(
        const uint8_t * const Bytes_p)
{
    return EIP28_PKARAM_MAKE_WORD_FROM_BYTES(
                    Bytes_p[0],
                    Bytes_p[1],
                    Bytes_p[2],
                    Bytes_p[3]);
}


// use when the data pointed to by BytesPtr point to LSB end
// of the big integer
static inline void
EIP28_PKARAM_WRITE_WORDS_FROM_BYTESPTR_LSB_FIRST(
        Device_Handle_t Device,
        unsigned int StartWord,
        unsigned int BytesCount,
        const uint8_t * Bytes_p)
{
    while(BytesCount >= 4)
    {
        uint32_t data = EIP28_PKARAM_MAKE_WORD_FROM_BYTESPTR_LSB_FIRST(Bytes_p);
        EIP28_PKARAM_WR(Device, StartWord, data);
        StartWord += 1;
        Bytes_p += 4;
        BytesCount -= 4;
    }

    // write the remainder, using zero for undefined bytes
    if (BytesCount > 0)
    {
    LOG_CRIT("%s, %d, 0x%x\n", __FUNCTION__, __LINE__, BytesCount);
        uint32_t data = (uint32_t)*Bytes_p++;

        if (BytesCount > 1)
            data |= (*Bytes_p++) << 8;

        if (BytesCount > 2)
            data |= (*Bytes_p) << 16;

        EIP28_PKARAM_WR(Device, StartWord, data);
    }
    // EIP28_PKARAM_WR(Device, StartWord, 1);
    // StartWord += 1;
    // EIP28_PKARAM_WR(Device, StartWord, 0);
}


// use when the memory pointed to by BytesPtr shall point to LSB
// end of the big integer
static inline void
EIP28_PKARAM_READ_WORDS_TO_BYTESPTR_LSB_FIRST(
        Device_Handle_t Device,
        unsigned int StartWord,
        unsigned int WordCount,
        uint8_t * Bytes_p)
{
    while(WordCount > 0)
    {
        uint32_t data = EIP28_PKARAM_RD(Device, StartWord);

        // least significant byte is written out first
        *Bytes_p++ = (uint8_t)(data >> 0);
        *Bytes_p++ = (uint8_t)(data >> 8);
        *Bytes_p++ = (uint8_t)(data >> 16);
        *Bytes_p++ = (uint8_t)(data >> 24);

        StartWord += 1;
        WordCount -= 1;
    }
}


// use when the data pointed to by BytesPtr point to MSB end
// of the big integer
static inline void
EIP28_PKARAM_WRITE_WORDS_FROM_BYTESPTR_MSB_FIRST(
        Device_Handle_t Device,
        unsigned int StartWord,
        unsigned int BytesCount,
        const uint8_t * Bytes_p)
{
    // we process the byte stream from start to tail
    // but write the words in reverse order
    //uint32_t addr;

    // write the remainder, using zero for undefined bytes
    {
        unsigned int Remainder = (BytesCount & 3);
        BytesCount -= Remainder;        // BytesCount now a multiple of 4
        // now points to where the remainder shall be written
        StartWord += (BytesCount >> 2);

        // LOG_CRIT("%s, %d, pad : %d\n", __FUNCTION__, __LINE__, StartWord);
        if (Remainder > 0)
        {
        // LOG_CRIT("%s, %d, pad : %d\n", __FUNCTION__, __LINE__, 1);
            uint32_t data = (uint32_t)*Bytes_p++;

            if (Remainder > 1)
                data = (data << 8) | *Bytes_p++;

            if (Remainder > 2)
                data = (data << 8) | *Bytes_p++;

            EIP28_PKARAM_WR(Device, StartWord, data);
            // Bytes_p has been updated
        }

        StartWord -= 1;
        // now points to first 4-byte word location
    }

    while(BytesCount >= 4)
    {
        uint32_t data =
                EIP28_PKARAM_MAKE_WORD_FROM_BYTESPTR_MSB_FIRST(Bytes_p);
        EIP28_PKARAM_WR(Device, StartWord, data);
        StartWord -= 1;
        Bytes_p += 4;
        BytesCount -= 4;
    }
}


// use when the memory pointed to by BytesPtr shall point to
// MSB end of the big integer
static inline unsigned int
EIP28_PKARAM_READ_WORDS_TO_BYTESPTR_MSB_FIRST(
        Device_Handle_t Device,
        unsigned int StartWord,
        unsigned int WordCount,
        uint8_t * Bytes_p)
{
    uint8_t * const Original_Bytes_p = Bytes_p;
    uint32_t data;

    if (WordCount == 0)
        return 0;

    do
    {
        WordCount -= 1;
        data = EIP28_PKARAM_RD(Device, StartWord + WordCount);
    }
    while (0 == data && WordCount > 0);

    // store this first word, skipping further zero MSB's
    // most significant byte is written out first
    {
        uint8_t b;
        uint8_t WriteRest = 0;

        b = (uint8_t)(data >> 24);
        if (b != 0)
        {
            // four valid bytes
            *Bytes_p++ = b;
            WriteRest = 1;
        }

        b = (uint8_t)(data >> 16);
        if ((b | WriteRest) != 0)
        {
            // three valid bytes
            *Bytes_p++ = b;
            WriteRest = 1;
        }

        b = (uint8_t)(data >> 8);
        if ((b | WriteRest) != 0)
            *Bytes_p++ = b;

        // last word is always written
        *Bytes_p++ = (uint8_t)(data >> 0);
    }

    // copy remaining words
    while(WordCount > 0)
    {
        // read the next word
        WordCount -= 1;
        data = EIP28_PKARAM_RD(Device, StartWord + WordCount);

        // store the word
        // most significant byte is written out first
        *Bytes_p++ = (uint8_t)(data >> 24);
        *Bytes_p++ = (uint8_t)(data >> 16);
        *Bytes_p++ = (uint8_t)(data >> 8);
        *Bytes_p++ = (uint8_t)(data >> 0);
    }

    return (unsigned int)(Bytes_p - Original_Bytes_p);
}


static inline unsigned int
EIP28_PKARAM_READ_WORDS_TO_BYTESPTR_MSB_FIRST_ZERO_PAD(
        Device_Handle_t Device,
        unsigned int StartWord,
        unsigned int WordCount,
        uint8_t * Bytes_p,
        const unsigned int WantedNumberLength_NrOfBytes)
{
    uint32_t data;
    uint8_t * bytes_p = &Bytes_p[WantedNumberLength_NrOfBytes - 1];
    unsigned int ByteCount;

    if (WordCount == 0)
        return 0;

    {
        unsigned int Remainder = (WantedNumberLength_NrOfBytes & 3);
        ByteCount = WantedNumberLength_NrOfBytes - Remainder;

        while (ByteCount >= 4 && WordCount > 0)
        {
            data = EIP28_PKARAM_RD(Device, StartWord);

            // least significant byte is written out first
            *bytes_p-- = (uint8_t)(data >> 0);
            *bytes_p-- = (uint8_t)(data >> 8);
            *bytes_p-- = (uint8_t)(data >> 16);
            *bytes_p-- = (uint8_t)(data >> 24);

            StartWord += 1;
            ByteCount -= 4;
            WordCount -= 1;
        }

        if (WordCount > 0)
        {
            if (Remainder > 0)
            {
                data = EIP28_PKARAM_RD(Device, StartWord);
                *bytes_p-- = (uint8_t)(data >> 0);

                if (Remainder > 1)
                    *bytes_p-- = (uint8_t)(data >> 8);

                if (Remainder > 2)
                    *bytes_p-- = (uint8_t)(data >> 16);
            }
        }
        else
        {
            while (bytes_p >= Bytes_p)
                *bytes_p-- = 0;
        }
    }

    {
        unsigned int n = 0;
        while(n < WantedNumberLength_NrOfBytes && Bytes_p[n] == 0)
            n++;
        ByteCount = WantedNumberLength_NrOfBytes - n;
    }

    return (ByteCount);
}


/*--------------------------------------------------------------------------
 * Read/Write Register macros
 */

/* byte offsets of the EIP28 registers inside the EIP28 memory range,
   assuming 32bit alignment */
#define EIP28_REGISTER_SPACING     4

// Registers below are present in EIP28_PKCP and EIP28_LNME
#define EIP28_OFFSET_A_PTR         (0x00 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_B_PTR         (0x01 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_C_PTR         (0x02 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_D_PTR         (0x03 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_A_LEN         (0x04 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_B_LEN         (0x05 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_SHIFT         (0x06 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_FUNCTION      (0x07 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_COMPARE       (0x08 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_MSW           (0x09 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_DIVMSW        (0x0A * EIP28_REGISTER_SPACING)

// Registers below are present in EIP28_PKCP and EIP28_LNME only
#define EIP28_OFFSET_SEQCTRL       (0x32 * EIP28_REGISTER_SPACING)

// This register is present in EIP28_PKCP and EIP28_LNME
#define EIP28_OFFSET_SCAP_CTRL     (0x0C * EIP28_REGISTER_SPACING)

// This register is present in EIP28_LNME only
#define EIP28_OFFSET_LNME0_STATUS  (0x20 * EIP28_REGISTER_SPACING)

// Registers below are present in EIP28_LNME only
#define EIP28_OFFSET_LNME0_STATUS  (0x20 * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_LNME0_CONTROL (0x21 * EIP28_REGISTER_SPACING)

// This register is present in EIP28_PKCP (Starting from HW2.2)
// Should not be used from the host and be left to the firmware.
#define EIP28_OFFSET_PKCP_PRIO_CTRL 0x31 * EIP28_REGISTER_SPACING)

// Registers below are present in EIP28_PKCP and EIP28_LNME
#define EIP28_OFFSET_OPTIONS       (0x3D * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_SW_REV        (0x3E * EIP28_REGISTER_SPACING)
#define EIP28_OFFSET_REVISION      (0x3F * EIP28_REGISTER_SPACING)


/*---------------------------------------------------------------------------
 * A/B/C/D_PTR
 */
#define EIP28_A_PTR_RD(_dev)      EIP28_Read32( (_dev), EIP28_OFFSET_A_PTR)
#define EIP28_B_PTR_RD(_dev)      EIP28_Read32( (_dev), EIP28_OFFSET_B_PTR)
#define EIP28_C_PTR_RD(_dev)      EIP28_Read32( (_dev), EIP28_OFFSET_C_PTR)
#define EIP28_D_PTR_RD(_dev)      EIP28_Read32( (_dev), EIP28_OFFSET_D_PTR)

#define EIP28_A_PTR_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_A_PTR, (_v))
#define EIP28_B_PTR_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_B_PTR, (_v))
#define EIP28_C_PTR_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_C_PTR, (_v))
#define EIP28_D_PTR_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_D_PTR, (_v))


/*---------------------------------------------------------------------------
 * A/B_LEN
 */
#define EIP28_A_LEN_RD(_dev)      EIP28_Read32( (_dev), EIP28_OFFSET_A_LEN)
#define EIP28_B_LEN_RD(_dev)      EIP28_Read32( (_dev), EIP28_OFFSET_B_LEN)

#define EIP28_A_LEN_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_A_LEN, (_v))
#define EIP28_B_LEN_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_B_LEN, (_v))


/*---------------------------------------------------------------------------
 * SHIFT
 */
#define EIP28_SHIFT_RD(_dev)      EIP28_Read32( (_dev), EIP28_OFFSET_SHIFT)
#define EIP28_SHIFT_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_SHIFT, (_v))

// results returned by the InvMod operations
#define EIP28_SHIFT_RESULT_NO_INVERSE_EXISTS   7
#define EIP28_SHIFT_RESULT_ERROR_MODULUS_EVEN 31

// results returned by the ECC Add/Mul operations
#define EIP28_SHIFT_RESULT_POINT_AT_INFINITY   7
#define EIP28_SHIFT_RESULT_POINT_UNDEFINED    31


/*---------------------------------------------------------------------------
 * FUNCTION
 */
#define EIP28_FUNCTION_RD(_dev) \
                EIP28_Read32( (_dev), EIP28_OFFSET_FUNCTION)
#define EIP28_FUNCTION_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_FUNCTION, (_v))


/* constants for the bits in this register */
#define EIP28_FUNCTION_MULTIPLY     BIT_0
#define EIP28_FUNCTION_ADDSUB       BIT_1
// bit 2 is reserved
#define EIP28_FUNCTION_MSONE        BIT_3
#define EIP28_FUNCTION_ADD          BIT_4
#define EIP28_FUNCTION_SUBTRACT     BIT_5
#define EIP28_FUNCTION_RSHIFT       BIT_6
#define EIP28_FUNCTION_LSHIFT       BIT_7
#define EIP28_FUNCTION_DIVIDE       BIT_8
#define EIP28_FUNCTION_MODULO       BIT_9
#define EIP28_FUNCTION_COMPARE      BIT_10
#define EIP28_FUNCTION_COPY         BIT_11
// bits12-14 are sequencer operations
#define EIP28_FUNCTION_RUN          BIT_15
// bits16-23 are reserved
#define EIP28_FUNCTION_STALL        BIT_24
// bits 25-31 are reserved

#define EIP28_FUNCTION_SEQ_NONE                 (uint32_t)(0x00 << 12)
#define EIP28_FUNCTION_SEQ_MODEXPCRT            (uint32_t)(0x01 << 12)
#define EIP28_FUNCTION_SEQ_ECCMONTMUL           (uint32_t)(0x02 << 12)
#define EIP28_FUNCTION_SEQ_ECCADD_AFFINE        (uint32_t)(0x03 << 12)
#define EIP28_FUNCTION_SEQ_RESERVED1            (uint32_t)(0x04 << 12)
#define EIP28_FUNCTION_SEQ_ECCMUL_AFFINE        (uint32_t)(0x05 << 12)
#define EIP28_FUNCTION_SEQ_MODEXP               (uint32_t)(0x06 << 12)
#define EIP28_FUNCTION_SEQ_MODINV               (uint32_t)(0x07 << 12)
#define EIP28_FUNCTION_SEQ_ECCADD_PROJECTIVE    (uint32_t)(0x10 << 12)
#define EIP28_FUNCTION_SEQ_ECCMUL_PROJECTIVE    (uint32_t)(0x11 << 12)
#define EIP28_FUNCTION_SEQ_ECCSCALE_PROJECTIVE  (uint32_t)(0x12 << 12)

#define EIP28_FUNCTION_SEQ_DSASIGN              (uint32_t)(0x20 << 12)
#define EIP28_FUNCTION_SEQ_DSAVERIFY            (uint32_t)(0x21 << 12)
#define EIP28_FUNCTION_SEQ_ECCDSASIGN           (uint32_t)(0x22 << 12)
#define EIP28_FUNCTION_SEQ_ECCDSAVERIFY         (uint32_t)(0x23 << 12)

#define EIP28_FUNCTION_SEQ_MODEXP_SCAP          (uint32_t)(0x45 << 12)
#define EIP28_FUNCTION_SEQ_MODEXPCRT_SCAP       (uint32_t)(0x43 << 12)
#define EIP28_FUNCTION_SEQ_ECCMUL_SCAP          (uint32_t)(0x61 << 12)
#define EIP28_FUNCTION_SEQ_ECCMONTMUL_SCAP      (uint32_t)(0x46 << 12)
#define EIP28_FUNCTION_SEQ_DSASIGN_SCAP         (uint32_t)(0x52 << 12)
#define EIP28_FUNCTION_SEQ_ECCDSASIGN_SCAP      (uint32_t)(0x54 << 12)
#define EIP28_FUNCTION_SEQ_PREP_BLIND_MSG       (uint32_t)(0x51 << 12)



/*---------------------------------------------------------------------------
 * SCAP
 */
#define EIP28_SCAP_CTRL_RD(_dev) \
                EIP28_Read32( (_dev), EIP28_OFFSET_SCAP_CTRL)
#define EIP28_SCAP_CTRL_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_SCAP_CTRL, (_v))

/* constants for the bits in this register */
#define EIP28_SCAP_CTRL_DUMMY_CORR BIT_0
#define EIP28_SCAP_CTRL_FSM_ERROR BIT_31

static inline void
EIP28_SCAP_CTRL_SET(
        Device_Handle_t Device,
        const bool Dummy_Corr)
{
    uint32_t SCAP_Ctrl = EIP28_SCAP_CTRL_RD(Device);
    // Only modify the dummy_corr bit
    if (Dummy_Corr)
    {
        SCAP_Ctrl |= EIP28_SCAP_CTRL_DUMMY_CORR;
    }
    else
    {
        SCAP_Ctrl &= (0xFFFFFFFF - EIP28_SCAP_CTRL_DUMMY_CORR);
    }

    EIP28_SCAP_CTRL_WR(Device, SCAP_Ctrl);
}

static inline void
EIP28_SCAP_CTRL_FSM_ERROR_RESET(
        Device_Handle_t Device)
{
    uint32_t SCAP_Ctrl = EIP28_SCAP_CTRL_RD(Device);

    // Only modify the fsm_error bit
    SCAP_Ctrl &= (0xFFFFFFFF - EIP28_SCAP_CTRL_FSM_ERROR);

    EIP28_SCAP_CTRL_WR(Device, SCAP_Ctrl);
}


/*---------------------------------------------------------------------------
 * LNME STATUS
 */

#define EIP28_LNME0_STATUS_RD(_dev) \
                EIP28_Read32( (_dev), EIP28_OFFSET_LNME0_STATUS)
#define EIP28_LNME0_STATUS_WR(_dev, _v) \
                EIP28_Write32( (_dev), EIP28_OFFSET_LNME0_STATUS, _v)

/* constants for the bits in this register */
#define EIP28_LNME0_STATUS_FSM_ERROR BIT_31

static inline void
EIP28_LNME0_STATUS_FSM_ERROR_RESET(
        Device_Handle_t Device)
{
    uint32_t LNME0_Status = EIP28_LNME0_STATUS_RD(Device);

    // Only modify the fsm_error bit
    LNME0_Status &= (0xFFFFFFFF - EIP28_LNME0_STATUS_FSM_ERROR);

    EIP28_LNME0_STATUS_WR(Device, LNME0_Status);
}


/*---------------------------------------------------------------------------
 * HW OPTIONS
 */
#define EIP28_PKCP_CONF(_option)         (((_option) >> 0)  & MASK_2_BITS)
#define EIP28_LNME_CONF(_option)         (((_option) >> 2)  & MASK_3_BITS)
#define EIP28_SEQ_CONF(_option)          (((_option) >> 5)  & MASK_2_BITS)
#define EIP28_PRG_RAM(_option)           (((_option) >> 7)  & MASK_1_BIT)
#define EIP28_PROT_OPT(_option)          (((_option) >> 8)  & MASK_3_BITS)
#define EIP28_INT_MASK(_option)          (((_option) >> 11) & MASK_1_BIT)
#define EIP28_MMM3A(_option)             (((_option) >> 12) & MASK_1_BIT)
#define EIP28_1_LNME_NR_PE(_option)      (((_option) >> 16) & MASK_6_BITS)
#define EIP28_1_LNME_FIFO_DEPTH(_option) (((_option) >> 24) & MASK_8_BITS)

#define EIP28_OPTION_PKCP_16x16_MULTIPLIER   1
#define EIP28_OPTION_PKCP_32x32_MULTIPLIER   2
#define EIP28_OPTION_LNME_NO                 0
#define EIP28_OPTION_LNME                    1
#define EIP28_OPTION_SEQ_STAND               1
#define EIP28_OPTION_PRG_ROM                 0
#define EIP28_OPTION_PRG_RAM                 1
#define EIP28_OPTION_PROT_NO                 0
#define EIP28_OPTION_PROT_SCAP               1
#define EIP28_OPTION_PROT_PROT               3
#define EIP28_OPTION_INT_REDIR               0
#define EIP28_OPTION_INT_MASK                1


/*---------------------------------------------------------------------------
 * FW EXPECTATIONS
 */
#define EIP28_FW_CAPABILITIES(_exp)                 ((_exp)        & MASK_8_BITS)
#define EIP28_FW_LONGEST_LNME_PE_CHAIN(_exp)  (4 + (((_exp) >> 8)  & MASK_6_BITS))
#define EIP28_FW_PKCP_TYPE(_exp)                   (((_exp) >> 14) & MASK_2_BITS)
#define EIP28_FW_NR_LNME(_exp)                     (((_exp) >> 16) & MASK_3_BITS)
#define EIP28_FW_PKA_RAM_SIZE(_exp)    (512 * (1 + (((_exp) >> 19) & MASK_5_BITS)))

#define EIP28_FW_CAPABILITIES_EXP                    1
#define EIP28_FW_CAPABILITIES_EXP_CRT                3
#define EIP28_FW_CAPABILITIES_EXP_CRT_MODINV         7
#define EIP28_FW_CAPABILITIES_EXP_CRT_MODINV_ECCMUL  15
#define EIP28_FW_PKCP_TYPE_DO_NOT_CARE               0
#define EIP28_FW_PKCP_TYPE_16x16                     1
#define EIP28_FW_PKCP_TYPE_32x32                     2


static inline void
EIP28_FUNCTION_START_OPERATION(
        Device_Handle_t Device,
        const uint32_t FunctionBits)
{
    // include the RUN bit to actually start the operation
    uint32_t f = FunctionBits | EIP28_FUNCTION_RUN;
    RB_PERF_START_POINT();

    EIP28_FUNCTION_WR(Device, f);
}


// while running, the interrupt is not active
static inline bool
EIP28_FUNCTION_ISRUNNING(
        Device_Handle_t Device)
{
    uint32_t runbit = EIP28_FUNCTION_RD(Device) & EIP28_FUNCTION_RUN;
    if (runbit != 0)
        return true;

    return false;
}


/*---------------------------------------------------------------------------
 * COMPARE
 */
#define EIP28_COMPARE_RD(_dev)    EIP28_Read32((_dev), EIP28_OFFSET_COMPARE)

/* constants for the bits in this register */
#define EIP28_COMPARE_A_EQUAL_B    BIT_0
#define EIP28_COMPARE_A_LESSER_B   BIT_1
#define EIP28_COMPARE_A_GREATER_B  BIT_2


/*---------------------------------------------------------------------------
 * MSW
 */
#define EIP28_MSW_RD(_dev)        EIP28_Read32((_dev), EIP28_OFFSET_MSW)

#define EIP28_MSW_RESULTISZERO  BIT_15
#define EIP28_MSW_ADDRESS_MASK  0x7FF


/*---------------------------------------------------------------------------
 * DIVMSW
 */
#define EIP28_DIVMSW_RD(_dev)    EIP28_Read32((_dev), EIP28_OFFSET_DIVMSW)

#define EIP28_DIVMSW_RESULTISZERO  BIT_15
#define EIP28_DIVMSW_ADDRESS_MASK  MASK_11_BITS


/*---------------------------------------------------------------------------
 * SEQCTRL
 */
#define EIP28_SEQCTRL_RD(_dev)      EIP28_Read32( (_dev), EIP28_OFFSET_SEQCTRL)
#define EIP28_SEQCTRL_WR(_dev, _v) \
                EIP28_Write32((_dev), EIP28_OFFSET_SEQCTRL, (_v))

#define EIP28_SEQCTRL_HAMMING_ERR BIT_16
#define EIP28_SEQCTRL_PARITY_ERR BIT_17
#define EIP28_SEQCTRL_RESET  BIT_31

#define EIP28_FWRAM_PAGE_SIZE   0x800

static inline void
EIP28_SEQUENCER_RESET_TOGGLE(
        Device_Handle_t Device)
{
    EIP28_SEQCTRL_WR(Device, EIP28_SEQCTRL_RESET);
    EIP28_SEQCTRL_WR(Device, 0);
}

static inline uint32_t
EIP28_SEQUENCER_STATUS(
        Device_Handle_t Device)
{
    uint32_t S = EIP28_Read32(Device, EIP28_OFFSET_SEQCTRL);

    return (S & 0xFF00) >> 8;
}


#define EIP28_SEQCTRL_RESET_SET(_dev) \
                EIP28_SEQCTRL_WR((_dev), EIP28_SEQCTRL_RESET)
#define EIP28_SEQCTRL_RESET_CLR(_dev)  EIP28_SEQCTRL_WR((_dev), 0)


/*---------------------------------------------------------------------------
 * REVISION
 */

// returns true when sequence number is correct
static inline bool
EIP28_REVISION_CHECK_EIP_NR(
        Device_Handle_t Device)
{
    uint32_t V = EIP28_Read32(Device, EIP28_OFFSET_REVISION);

    if ((V & MASK_16_BITS) != 0xE31C)
        return false;

    return true;
}

#endif /* Include Guard */

/* end of file eip28_level0.h */
