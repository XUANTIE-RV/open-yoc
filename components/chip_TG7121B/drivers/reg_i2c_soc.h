#ifndef REG_REG_I2C_SOC_H_
#define REG_REG_I2C_SOC_H_
#include <stdint.h>

/******************************************************************************/
/*                                  APB_I2C                                   */
/******************************************************************************/
typedef union   // 0x000 I2C_CR1
{
    struct
    {
        uint32_t PE:        1;
        uint32_t RESERVED0: 7;
        uint32_t DNF:       4;
        uint32_t ANFOFF:    1;
        uint32_t RESERVED1: 1;
        uint32_t TXDMAEN:   1;
        uint32_t RXDMAEN:   1;
        uint32_t SBC:       1;
        uint32_t NOSTRETCH: 1;
        uint32_t WUPEN:     1;
        uint32_t GCEN:      1;
        uint32_t SMBHEN:    1;
        uint32_t SMBDEN:    1;
        uint32_t ALERTEN:   1;
        uint32_t PECEN:     1;
        uint32_t TXFTH:     3;
        uint32_t RESERVED2: 1;
        uint32_t RXFTH:     3;
        uint32_t RESERVED3: 1;
    };
    uint32_t Word;
} I2C_CR1_Typedef;

typedef union   // 0x004 I2C_CR2
{
    struct
    {
        uint32_t SADD:      10;
        uint32_t RD_WRN:    1;
        uint32_t ADD10:     1;
        uint32_t HEAD10R:   1;
        uint32_t START:     1;
        uint32_t STOP:      1;
        uint32_t NACK:      1;
        uint32_t NBYTES:    8;
        uint32_t RELOAD:    1;
        uint32_t AUTOEND:   1;
        uint32_t PECBYTE:   1;
        uint32_t RESERVED0: 5;
    };
    uint32_t Word;
} I2C_CR2_Typedef;

typedef union   // 0x008 I2C_OAR1
{
    struct
    {
        uint32_t OA1:       10;
        uint32_t OA1MODE:   1;
        uint32_t RESERVED0: 4;
        uint32_t OA1EN:     1;
        uint32_t RESERVED1: 16;
    };
    uint32_t Word;
} I2C_OAR1_Typedef;


typedef union   // 0x00C I2C_OAR2
{
    struct
    {
        uint32_t RESERVED0: 1;
        uint32_t OA2:       7;
        uint32_t OA2MSK:    3;
        uint32_t RESERVED1: 4;
        uint32_t OA2EN:     1;
        uint32_t RESERVED2: 16;
    };
    uint32_t Word;
} I2C_OAR2_Typedef;

typedef union   // 0x010 I2C_TIMINGR
{
    struct
    {
        uint32_t SCLL:      8;
        uint32_t SCLH:      8;
        uint32_t SDADEL:    4;
        uint32_t SCLDEL:    4;
        uint32_t RESERVED0: 4;
        uint32_t PRESC:     4;
    };
    uint32_t Word;
} I2C_TIMINGR_Typedef;

typedef union   // 0x014 I2C_TIMEOUTR
{
    struct
    {
        uint32_t TIMEOUTA:  12;
        uint32_t TIDLE:     1;
        uint32_t RESERVED0: 2;
        uint32_t TIMEOUTEN: 1;
        uint32_t TIMEOUTB:  12;
        uint32_t RESERVED1: 3;
        uint32_t TEXTEN:    1;
    };
    uint32_t Word;
} I2C_TIMEOUTR_Typedef;

typedef union   // 0x018 I2C_SR
{
    struct
    {
        uint32_t TXE:       1;
        uint32_t RESERVED0: 1;
        uint32_t RXNE:      1;
        uint32_t ADDR:      1;
        uint32_t NACKF:     1;
        uint32_t STOPF:     1;
        uint32_t TC:        1;
        uint32_t TCR:       1;
        uint32_t BERR:      1;
        uint32_t ARLO:      1;
        uint32_t OVR:       1;
        uint32_t PECERR:    1;
        uint32_t TIMEOUT:   1;
        uint32_t ALERT:     1;
        uint32_t RESERVED1: 1;
        uint32_t BUSY:      1;
        uint32_t DIR:       1;
        uint32_t ADDCODE:   7;
        uint32_t TXFLV:     4;
        uint32_t RXFLV:     4;
    };
    uint32_t Word;
} I2C_SR_Typedef;

typedef union   // 0x01C I2C_CFR
{
    struct
    {
        uint32_t RESERVED0: 3;
        uint32_t ADDRCF:    1;
        uint32_t NACKCF:    1;
        uint32_t STOPCF:    1;
        uint32_t RESERVED1: 2;
        uint32_t BERRCF:    1;
        uint32_t ARLOCF:    1;
        uint32_t OVRCF:     1;
        uint32_t PECCF:     1;
        uint32_t TIMOUTCF:  1;
        uint32_t ALERTCF:   1;
        uint32_t RESERVED2: 18;
    };
    uint32_t Word;
} I2C_CFR_Typedef;

typedef union   // 0x020 I2C_PECR
{
    struct
    {
        uint32_t PEC:       8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} I2C_PECR_Typedef;

typedef union   // 0x024 I2C_RXDR
{
    struct
    {
        uint32_t RXDATA:    8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} I2C_RXDR_Typedef;

typedef union   // 0x028 I2C_TXDR
{
    struct
    {
        uint32_t TXDATA:    8;
        uint32_t RESERVED0: 24;
    };
    uint32_t Word;
} I2C_TXDR_Typedef;

typedef union   // 0x02C I2C_IER
{
    struct
    {
        uint32_t TXEIE:     1;
        uint32_t RESERVED0: 1;
        uint32_t RXNEIE:    1;
        uint32_t ADDRIE:    1;
        uint32_t NACKIE:    1;
        uint32_t STOPIE:    1;
        uint32_t TCIE:      1;
        uint32_t TCRIE:     1;
        uint32_t BERRIE:    1;
        uint32_t ARLOIE:    1;
        uint32_t OVRIE:     1;
        uint32_t PECEIE:    1;
        uint32_t TOUTIE:    1;
        uint32_t ALERTIE:   1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} I2C_IER_Typedef;

typedef union   // 0x030 I2C_IDR
{
    struct
    {
        uint32_t TXEID:     1;
        uint32_t RESERVED0: 1;
        uint32_t RXNEID:    1;
        uint32_t ADDRID:    1;
        uint32_t NACKID:    1;
        uint32_t STOPID:    1;
        uint32_t TCID:      1;
        uint32_t TCRID:     1;
        uint32_t BERRID:    1;
        uint32_t ARLOID:    1;
        uint32_t OVRID:     1;
        uint32_t PECEID:    1;
        uint32_t TOUTID:    1;
        uint32_t ALERTID:   1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} I2C_IDR_Typedef;

typedef union   // 0x034 I2C_IVS
{
    struct
    {
        uint32_t TXEIV:     1;
        uint32_t RESERVED0: 1;
        uint32_t RXNEIV:    1;
        uint32_t ADDRIV:    1;
        uint32_t NACKIV:    1;
        uint32_t STOPIV:    1;
        uint32_t TCIV:      1;
        uint32_t TCRIV:     1;
        uint32_t BERRIV:    1;
        uint32_t ARLOIV:    1;
        uint32_t OVRIV:     1;
        uint32_t PECEIV:    1;
        uint32_t TOUTIV:    1;
        uint32_t ALERTIV:   1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} I2C_IVS_Typedef;

typedef union   // 0x038 I2C_RIF
{
    struct
    {
        uint32_t TXERI:     1;
        uint32_t RESERVED0: 1;
        uint32_t RXNERI:    1;
        uint32_t ADDRRI:    1;
        uint32_t NACKRI:    1;
        uint32_t STOPRI:    1;
        uint32_t TCRI:      1;
        uint32_t TCRRI:     1;
        uint32_t BERRRI:    1;
        uint32_t ARLORI:    1;
        uint32_t OVRRI:     1;
        uint32_t PECERI:    1;
        uint32_t TOUTRI:    1;
        uint32_t ALERTRI:   1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} I2C_RIF_Typedef;

typedef union   // 0x03C I2C_IFM
{
    struct
    {
        uint32_t TXEFM:     1;
        uint32_t RESERVED0: 1;
        uint32_t RXNEFM:    1;
        uint32_t ADDRFM:    1;
        uint32_t NACKFM:    1;
        uint32_t STOPFM:    1;
        uint32_t TCFM:      1;
        uint32_t TCRFM:     1;
        uint32_t BERRFM:    1;
        uint32_t ARLOFM:    1;
        uint32_t OVRFM:     1;
        uint32_t PECEFM:    1;
        uint32_t TOUTFM:    1;
        uint32_t ALERTFM:   1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} I2C_IFM_Typedef;

typedef union   // 0x040 I2C_ICR
{
    struct
    {
        uint32_t TXEIC:     1;
        uint32_t RESERVED0: 1;
        uint32_t RXNEIC:    1;
        uint32_t ADDRIC:    1;
        uint32_t NACKIC:    1;
        uint32_t STOPIC:    1;
        uint32_t TCIC:      1;
        uint32_t TCRIC:     1;
        uint32_t BERRIC:    1;
        uint32_t ARLOIC:    1;
        uint32_t OVRIC:     1;
        uint32_t PECEIC:    1;
        uint32_t TOUTIC:    1;
        uint32_t ALERTIC:   1;
        uint32_t RESERVED1: 18;
    };
    uint32_t Word;
} I2C_ICR_Typedef;

typedef struct
{
    volatile I2C_CR1_Typedef        CR1;        // 0x00 I2C Control 1 Register
    volatile I2C_CR2_Typedef        CR2;        // 0x04 I2C Control 2 Register
    volatile I2C_OAR1_Typedef       OAR1;       // 0x08 I2C Own Address 1 Register
    volatile I2C_OAR2_Typedef       OAR2;       // 0x0C I2C Own Address 2 Register
    volatile I2C_TIMINGR_Typedef    TIMINGR;    // 0x10 I2C Timing Register
    volatile I2C_TIMEOUTR_Typedef   TIMEOUTR;   // 0x14 I2C Timeout Register
    volatile I2C_SR_Typedef         SR;         // 0x18 I2C Status Register
    volatile I2C_CFR_Typedef        CFR;        // 0x1C I2C Clear Flag Register
    volatile I2C_PECR_Typedef       PECR;       // 0x20 I2C PEC Register
    volatile I2C_RXDR_Typedef       RXDR;       // 0x24 I2C Receive Data Register
    volatile I2C_TXDR_Typedef       TXDR;       // 0x28 I2C Transmit Data Register
    volatile  I2C_IER_Typedef        IER;        // 0x2C I2C Interrupt Enable Register
    volatile  I2C_IDR_Typedef        IDR;        // 0x30 I2C Interrupt Disable Register
    volatile  I2C_IVS_Typedef        IVS;        // 0x34 I2C Interrupt Valid Status Register
    volatile  I2C_RIF_Typedef        RIF;        // 0x38 I2C Raw Interrupt Flag Status Register
    volatile  I2C_IFM_Typedef        IFM;        // 0x3C I2C Interrupt Flag Masked Status Register
    volatile  I2C_ICR_Typedef        ICR;        // 0x40 I2C Interrupt Clear Register
} I2C_TypeDef;

#endif