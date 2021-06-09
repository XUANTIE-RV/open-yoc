#ifndef REG_CRYPT_TYPE_H_
#define REG_CRYPT_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t DATA0;
    volatile uint32_t DATA1;
    volatile uint32_t DATA2;
    volatile uint32_t DATA3;
    volatile uint32_t KEY0;
    volatile uint32_t KEY1;
    volatile uint32_t KEY2;
    volatile uint32_t KEY3;
    volatile uint32_t KEY4;
    volatile uint32_t KEY5;
    volatile uint32_t KEY6;
    volatile uint32_t KEY7;
    volatile uint32_t IVR0;
    volatile uint32_t IVR1;
    volatile uint32_t IVR2;
    volatile uint32_t IVR3;
    volatile uint32_t RES0;
    volatile uint32_t RES1;
    volatile uint32_t RES2;
    volatile uint32_t RES3;
    volatile uint32_t CR;
    volatile uint32_t SR;
    volatile uint32_t ICRF;
    volatile uint32_t FIFO;
}reg_crypt_t;

enum CRYPT_REG_DATA0_FIELD
{
    CRYPT_DATA0_MASK = (int)0xffffffff,
    CRYPT_DATA0_POS = 0,
};

enum CRYPT_REG_DATA1_FIELD
{
    CRYPT_DATA1_MASK = (int)0xffffffff,
    CRYPT_DATA1_POS = 0,
};

enum CRYPT_REG_DATA2_FIELD
{
    CRYPT_DATA2_MASK = (int)0xffffffff,
    CRYPT_DATA2_POS = 0,
};

enum CRYPT_REG_DATA3_FIELD
{
    CRYPT_DATA3_MASK = (int)0xffffffff,
    CRYPT_DATA3_POS = 0,
};

enum CRYPT_REG_KEY0_FIELD
{
    CRYPT_KEY0_MASK = (int)0xffffffff,
    CRYPT_KEY0_POS = 0,
};

enum CRYPT_REG_KEY1_FIELD
{
    CRYPT_KEY1_MASK = (int)0xffffffff,
    CRYPT_KEY1_POS = 0,
};

enum CRYPT_REG_KEY2_FIELD
{
    CRYPT_KEY2_MASK = (int)0xffffffff,
    CRYPT_KEY2_POS = 0,
};

enum CRYPT_REG_KEY3_FIELD
{
    CRYPT_KEY3_MASK = (int)0xffffffff,
    CRYPT_KEY3_POS = 0,
};

enum CRYPT_REG_KEY4_FIELD
{
    CRYPT_KEY4_MASK = (int)0xffffffff,
    CRYPT_KEY4_POS = 0,
};

enum CRYPT_REG_KEY5_FIELD
{
    CRYPT_KEY5_MASK = (int)0xffffffff,
    CRYPT_KEY5_POS = 0,
};

enum CRYPT_REG_KEY6_FIELD
{
    CRYPT_KEY6_MASK = (int)0xffffffff,
    CRYPT_KEY6_POS = 0,
};

enum CRYPT_REG_KEY7_FIELD
{
    CRYPT_KEY7_MASK = (int)0xffffffff,
    CRYPT_KEY7_POS = 0,
};

enum CRYPT_REG_IVR0_FIELD
{
    CRYPT_IVR0_MASK = (int)0xffffffff,
    CRYPT_IVR0_POS = 0,
};

enum CRYPT_REG_IVR1_FIELD
{
    CRYPT_IVR1_MASK = (int)0xffffffff,
    CRYPT_IVR1_POS = 0,
};

enum CRYPT_REG_IVR2_FIELD
{
    CRYPT_IVR2_MASK = (int)0xffffffff,
    CRYPT_IVR2_POS = 0,
};

enum CRYPT_REG_IVR3_FIELD
{
    CRYPT_IVR3_MASK = (int)0xffffffff,
    CRYPT_IVR3_POS = 0,
};

enum CRYPT_REG_RES0_FIELD
{
    CRYPT_RES0_MASK = (int)0xffffffff,
    CRYPT_RES0_POS = 0,
};

enum CRYPT_REG_RES1_FIELD
{
    CRYPT_RES1_MASK = (int)0xffffffff,
    CRYPT_RES1_POS = 0,
};

enum CRYPT_REG_RES2_FIELD
{
    CRYPT_RES2_MASK = (int)0xffffffff,
    CRYPT_RES2_POS = 0,
};

enum CRYPT_REG_RES3_FIELD
{
    CRYPT_RES3_MASK = (int)0xffffffff,
    CRYPT_RES3_POS = 0,
};

enum CRYPT_REG_CR_FIELD
{
    CRYPT_GO_MASK = 0x1,
    CRYPT_GO_POS = 0,
    CRYPT_ENCS_MASK = 0x2,
    CRYPT_ENCS_POS = 1,
    CRYPT_AESKS_MASK = 0xc,
    CRYPT_AESKS_POS = 2,
    CRYPT_MODE_MASK = 0x30,
    CRYPT_MODE_POS = 4,
    CRYPT_IVREN_MASK = 0x40,
    CRYPT_IVREN_POS = 6,
    CRYPT_IE_MASK = 0x80,
    CRYPT_IE_POS = 7,
    CRYPT_TYPE_MASK = 0x300,
    CRYPT_TYPE_POS = 8,
    CRYPT_TDES_MASK = 0x400,
    CRYPT_TDES_POS = 10,
    CRYPT_DESKS_MASK = 0x800,
    CRYPT_DESKS_POS = 11,
    CRYPT_FIFOEN_MASK = 0x1000,
    CRYPT_FIFOEN_POS = 12,
    CRYPT_FIFOODR_MASK = 0x2000,
    CRYPT_FIFOODR_POS = 13,
    CRYPT_DMAEN_MASK = 0x4000,
    CRYPT_DMAEN_POS = 14,
    CRYPT_CRYSEL_MASK = (int)0x80000000,
    CRYPT_CRYSEL_POS = 31,
};

enum CRYPT_REG_SR_FIELD
{
    CRYPT_AESRIF_MASK = 0x1,
    CRYPT_AESRIF_POS = 0,
    CRYPT_DESRIF_MASK = 0x2,
    CRYPT_DESRIF_POS = 1,
    CRYPT_DONE_MASK = 0x100,
    CRYPT_DONE_POS = 8,
};

enum CRYPT_REG_ICRF_FIELD
{
    CRYPT_AESIF_MASK = 0x1,
    CRYPT_AESIF_POS = 0,
    CRYPT_DESIF_MASK = 0x2,
    CRYPT_DESIF_POS = 1,
    CRYPT_MULIF_MASK = 0x4,
    CRYPT_MULIF_POS = 2,
};

enum CRYPT_REG_FIFO_FIELD
{
    CRYPT_FIFO_MASK = (int)0xffffffff,
    CRYPT_FIFO_POS = 0,
};

#endif
