/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef SASC_H
#define SASC_H
#include <stdint.h>

#if ((CONFIG_PLATFORM_PHOBOS > 0) || (CONFIG_PLATFORM_HOBBIT1_2 > 0) || (CONFIG_PLATFORM_HOBBIT3 > 0))

#define EFLASH_BLOCK_SIZE           512
#define EFLASH_SECTOR_SIZE          512
#define EFLASH_MAX_SIZE             0x40000
#define EFLASH_ADDR_START           0x10000000
#define EFLASH_ADDR_END             0x1003FFFF

#if (CONFIG_PLATFORM_PHOBOS > 0)
#define EFLASH_REG_BASE             0x40005000
#elif (CONFIG_PLATFORM_HOBBIT1_2 > 0)
#define EFLASH_REG_BASE             0x4003f000
#elif (CONFIG_PLATFORM_HOBBIT3 > 0)
#define EFLASH_REG_BASE             0x40005000
#endif

#define EFLASH_NVR                  0x00
#define EFLASH_ADDR                 0x04
#define EFLASH_RSVR0                0x08
#define EFLASH_RSVR1                0x0C
#define EFLASH_PE                   0x10
#define EFLASH_RSVR2                0x14
#define EFLASH_WE                   0x18
#define EFLASH_WDATA                0x1C
#define EFLASH_TRC                  0x20
#define EFLASH_TNVS                 0x24
#define EFLASH_TPGS                 0x28
#define EFLASH_TPROG                0x2C
#define EFLASH_TRCV                 0x30
#define EFLASH_TERASE               0x34
#define EFLASH_CAR                  0x38
#define EFLASH_CR                   0x3C
#define EFLASH_RG0                  0x40
#define EFLASH_RG1                  0x44
#define EFLASH_RG2                  0x48
#define EFLASH_RG3                  0x4C
#define EFLASH_RG4                  0x50
#define EFLASH_RG5                  0x54
#define EFLASH_RG6                  0x58
#define EFLASH_RG7                  0x5C
#define EFLASH_CFG0                 0x60
#define EFLASH_CFG1                 0x64

#define EFLASH_REG_NVR_IFREN        0x00000001
#define EFLASH_REG_ADDR             0xFFFFFFFF
#define EFLASH_REG_PE               0x00000001
#define EFLASH_REG_WE               0x00000001
#define EFLASH_REG_TRC              0x00000003
#define EFLASH_REG_TNVS             0x000001FF
#define EFLASH_REG_TPGS             0x000000FF
#define EFLASH_REG_TPROG            0x000001FF
#define EFLASH_REG_TRCV             0x00000FFF
#define EFLASH_REG_TERASE           0x000FFFFF

#define EFLASH_REG_CAR_CAR0         0x00000001
#define EFLASH_REG_CAR_CAR1         0x00000002
#define EFLASH_REG_CAR_CAR2         0x00000004
#define EFLASH_REG_CAR_CAR3         0x00000008
#define EFLASH_REG_CAR_CAR4         0x00000010
#define EFLASH_REG_CAR_CAR5         0x00000020
#define EFLASH_REG_CAR_CAR6         0x00000040
#define EFLASH_REG_CAR_CAR7         0x00000080

#define EFLASH_REG_CA_CA0           0x00000001
#define EFLASH_REG_CA_CA1           0x00000002
#define EFLASH_REG_CA_CA2           0x00000004
#define EFLASH_REG_CA_CA3           0x00000008
#define EFLASH_REG_CA_CA4           0x00000010
#define EFLASH_REG_CA_CA5           0x00000020
#define EFLASH_REG_CA_CA6           0x00000040
#define EFLASH_REG_CA_CA7           0x00000080
#define EFLASH_REG_RG_SZ            0x0000000F
#define EFLASH_REG_RG_BA            0xFF800000

#define EFLASH_REG_RG_1S            0
#define EFLASH_REG_RG_2S            1
#define EFLASH_REG_RG_4S            2
#define EFLASH_REG_RG_8S            3
#define EFLASH_REG_RG_16S           4
#define EFLASH_REG_RG_32S           5
#define EFLASH_REG_RG_64S           6
#define EFLASH_REG_RG_128S          7
#define EFLASH_REG_RG_256S          8
#define EFLASH_REG_RG_512S          9

#define EFLASH_REG_CFG0_RG0AP        0x00000003
#define EFLASH_REG_CFG0_RG1AP        0x0000000C
#define EFLASH_REG_CFG0_RG2AP        0x00000030
#define EFLASH_REG_CFG0_RG3AP        0x000000C0
#define EFLASH_REG_CFG0_RG4AP        0x00000300
#define EFLASH_REG_CFG0_RG5AP        0x00000C00
#define EFLASH_REG_CFG0_RG6AP        0x00003000
#define EFLASH_REG_CFG0_RG7AP        0x0000C000
#define EFLASH_REG_CFG0_RG0CD        0x00030000
#define EFLASH_REG_CFG0_RG1CD        0x000C0000
#define EFLASH_REG_CFG0_RG2CD        0x00300000
#define EFLASH_REG_CFG0_RG3CD        0x00C00000
#define EFLASH_REG_CFG0_RG4CD        0x03000000
#define EFLASH_REG_CFG0_RG5CD        0x0C000000
#define EFLASH_REG_CFG0_RG6CD        0x30000000
#define EFLASH_REG_CFG0_RG7CD        0xC0000000

#define EFLASH_REG_CFG1_RG0AP        0x00000003
#define EFLASH_REG_CFG1_RG1AP        0x0000000C
#define EFLASH_REG_CFG1_RG2AP        0x00000030
#define EFLASH_REG_CFG1_RG3AP        0x000000C0
#define EFLASH_REG_CFG1_RG4AP        0x00000300
#define EFLASH_REG_CFG1_RG5AP        0x00000C00
#define EFLASH_REG_CFG1_RG6AP        0x00003000
#define EFLASH_REG_CFG1_RG7AP        0x0000C000

#define EFLASH_REG_DEF_SIZE            1024
#define EFLASH_REG_MAX_SIZE            0xa
#define EFLASH_REG_MIN_SIZE            0x0

#define AP_RW       0x00
#define AP_RO       0x01
#define AP_WO       0x10
#define AP_DN       0x11

#define CD_DI       0x00
#define CD_DA       0x01
#define CD_IN       0x10
#define CD_DN       0x11


#define SRAM_MINI_WIDTH           4
#if (CONFIG_PLATFORM_PHOBOS > 0)
#define SRAM_REG_BASE             0x40009000
#elif (CONFIG_PLATFORM_HOBBIT1_2 > 0)
#define SRAM_REG_BASE             0x40007000
#elif (CONFIG_PLATFORM_ZX297100 > 0)
#define SRAM_REG_BASE             0x40007000
#elif (CONFIG_PLATFORM_HOBBIT3 > 0)
#define SRAM_REG_BASE             0x40009000
#endif

#define SRAM_ADDR_START           0x20000000

#define SRAM_CAR                  0x00
#define SRAM_CR                   0x04
#define SRAM_RG0                  0x08
#define SRAM_RG1                  0x0c
#define SRAM_RG2                  0x10
#define SRAM_RG3                  0x14
#define SRAM_RG4                  0x18
#define SRAM_RG5                  0x1c
#define SRAM_RG6                  0x20
#define SRAM_RG7                  0x24
#define SRAM_CFG0                 0x28
#define SRAM_CFG1                 0x2c

#define SRAM_REG_CAR_CAR0         0x00000001
#define SRAM_REG_CAR_CAR1         0x00000002
#define SRAM_REG_CAR_CAR2         0x00000004
#define SRAM_REG_CAR_CAR3         0x00000008
#define SRAM_REG_CAR_CAR4         0x00000010
#define SRAM_REG_CAR_CAR5         0x00000020
#define SRAM_REG_CAR_CAR6         0x00000040
#define SRAM_REG_CAR_CAR7         0x00000080

#define SRAM_REG_CA_CA0           0x00000001
#define SRAM_REG_CA_CA1           0x00000002
#define SRAM_REG_CA_CA2           0x00000004
#define SRAM_REG_CA_CA3           0x00000008
#define SRAM_REG_CA_CA4           0x00000010
#define SRAM_REG_CA_CA5           0x00000020
#define SRAM_REG_CA_CA6           0x00000040
#define SRAM_REG_CA_CA7           0x00000080

#define SRAM_REG_RG_SZ            0x0000001F
#define SRAM_REG_RG_BA            0xFFF00000

#define SRAM_REG_RG_4B              0x05
#define SRAM_REG_RG_8B              0x06
#define SRAM_REG_RG_16B             0x07
#define SRAM_REG_RG_32B             0x08
#define SRAM_REG_RG_64B             0x09
#define SRAM_REG_RG_128B            0x0a
#define SRAM_REG_RG_256B            0x0b
#define SRAM_REG_RG_512B            0x0c
#define SRAM_REG_RG_1KB             0x0d
#define SRAM_REG_RG_2KB             0x0e
#define SRAM_REG_RG_4KB             0x0f
#define SRAM_REG_RG_8KB             0x10
#define SRAM_REG_RG_16KB            0x11

#define SRAM_REG_CFG0_RG0AP        0x00000003
#define SRAM_REG_CFG0_RG1AP        0x0000000C
#define SRAM_REG_CFG0_RG2AP        0x00000030
#define SRAM_REG_CFG0_RG3AP        0x000000C0
#define SRAM_REG_CFG0_RG4AP        0x00000300
#define SRAM_REG_CFG0_RG5AP        0x00000C00
#define SRAM_REG_CFG0_RG6AP        0x00003000
#define SRAM_REG_CFG0_RG7AP        0x0000C000
#define SRAM_REG_CFG0_RG0CD        0x00030000
#define SRAM_REG_CFG0_RG1CD        0x000C0000
#define SRAM_REG_CFG0_RG2CD        0x00300000
#define SRAM_REG_CFG0_RG3CD        0x00C00000
#define SRAM_REG_CFG0_RG4CD        0x03000000
#define SRAM_REG_CFG0_RG5CD        0x0C000000
#define SRAM_REG_CFG0_RG6CD        0x30000000
#define SRAM_REG_CFG0_RG7CD        0xC0000000

#define SRAM_REG_CFG1_RG0AP        0x00000003
#define SRAM_REG_CFG1_RG1AP        0x0000000C
#define SRAM_REG_CFG1_RG2AP        0x00000030
#define SRAM_REG_CFG1_RG3AP        0x000000C0
#define SRAM_REG_CFG1_RG4AP        0x00000300
#define SRAM_REG_CFG1_RG5AP        0x00000C00
#define SRAM_REG_CFG1_RG6AP        0x00003000
#define SRAM_REG_CFG1_RG7AP        0x0000C000

#define SRAM_REG_DEF_SIZE          (16*1024)
#define SRAM_REG_MAX_SIZE           0x11
#define SRAM_REG_MIN_SIZE           0x5

#elif (CONFIG_PLATFORM_ZX297100 > 0)
#define REG32(addr)         (*(volatile uint32_t *)(addr))

/*EFLASH_MPU_SEC_ENABLE OFFESET*/
#define EFC_MPU_SEC_ENABLE     (0)

/*EFC_REGION_CFG */
#define EFC_REGION_AP_ACCESS_PRI        (0)
#define EFC_REGION_IS_SECURE            (2)
#define EFC_REGION_NX                   (3)
#define EFC_REGION_SECURE_EN            (5)
#define EFC_REGION_LOCK                 (6)
#define EFC_REGION_ADDR                 (10)

#define EFC1_SEC_REGION_ALLIG_SIZE      (2048)
#define EFC2_SEC_REGION_ALLIG_SIZE      (1024)
#define REGION_ADDR_ALIG_SHIFT          (10)
#define IRAM_SEC_REGION_ALLIG_SIZE      (1024)

#define OP_EN_PASSWORD                  (0x12345AA5)
#define EFC_START_BUSY_FLAG             (0)
#define EFC_WRITE_ENABLE                (1)
#define EFLASH_REGN_NUM                 (4)
#define CK_TO_MATRIX                    (0x2FFF0000)
#define ROUNDUP(a, b)        (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDDOWN(a, b)      ((a) & ~((b)-1))
#define CKTOMATRIX(ck_iram_addr) (ck_iram_addr-CK_TO_MATRIX)

#define _SIZE_256K          (0x00040000)
#define _SIZE_224K          (0x00038000)
#define _SIZE_192K          (0x00030000)
#define _SIZE_184K          (0x0002E000)
#define _SIZE_96K           (0x00018000)
#define _SIZE_64K           (0x00010000)
#define _SIZE_48K           (0x0000c000)
#define _SIZE_34K           (0x00008800)
#define _SIZE_32K           (0x00008000)
#define _SIZE_16K           (0x00004000)
#define _SIZE_8K            (0x00002000)
#define _SIZE_4K            (0x00001000)
#define _SIZE_1K            (0x00000400)

#ifdef CONFIG_RUN_IN_FLASH
#define FLASH_START           (0xFFF4000)
#else
#define FLASH_START           (0x90180000)
#endif
#define FLASH_SIZE            (_SIZE_34K)
#define FLASH_END             (FLASH_START + FLASH_SIZE)
#define SRAM_START            (0x30000000)
#define SRAM_SIZE             (_SIZE_8K)
#define SRAM_END              (SRAM_START + SRAM_SIZE)

#define SRAM1_START           (0x30000000)
#define SRAM1_SIZE            (_SIZE_64K)
#define SRAM1_END             (SRAM1_START + SRAM1_SIZE)
#define SRAM2_START           (0x30010000)
#define SRAM2_SIZE            (_SIZE_16K)
#define SRAM2_END             (SRAM2_START + SRAM2_SIZE)

#define FLASH1_START           (0x0FED0000)
#define FLASH1_SIZE            (1216 * 1024UL)
#define FLASH1_END             (FLASH1_START + FLASH1_SIZE)
#define FLASH2_START           (0x10000000)
#define FLASH2_SIZE            (_SIZE_256K)
#define FLASH2_END             (FLASH2_START + FLASH2_SIZE)

#define SOC_SYS_BASE           (0x21307000)
#define AP_IRAM1_ROSZ_CFG      (SOC_SYS_BASE + 0xd8)
#define AP_IRAM1_UP_ROSZ_CFG   (SOC_SYS_BASE + 0xdc)
#define AP_IRAM2_ROSZ_CFG      (SOC_SYS_BASE + 0xe0)
#define AP_IRAM2_UP_ROSZ_CFG   (SOC_SYS_BASE + 0xe4)
#define EFLASH1_MPU_SEC_ENABLE_CFG (SOC_SYS_BASE + 0x154)
#define EFLASH2_MPU_SEC_ENABLE_CFG (SOC_SYS_BASE + 0x184)


#define EFLASH1_CTRL_BASE      (0x21301000)
#define EFLASH1_REGION_BASE    (EFLASH1_CTRL_BASE + 0x100)

#define EFLASH2_CTRL_BASE      (0x10080000)
#define EFLASH2_REGION_BASE    (EFLASH2_CTRL_BASE + 0x100)

struct asc_si_efc_regs {
    uint32_t version;               /* 0x00 */
    uint32_t sec_flag;              /* 0x04         [4:0]region0-4 flag,[31]default flag,0:non-sec  1:sec*/
    uint32_t password;              /* 0x08*/
    uint32_t op_code;               /* 0x0c         [2:0]   000:set config  001:setup  010:program  011:page erase  100:macro erase 101:standby*/
    uint32_t op_addr;               /* 0x10 */
    uint32_t op_len;                /* 0x14 */
    uint32_t data[4];               /* 0x18-0x24 */
    uint32_t start_status;          /* 0x28         [0]read as busy flag, when not busy write 1 will start operation, write 0 has no affection;
                                                    [1]write enable   [2]command not support  [3]addr out of range  [4]master non-secure  [5]unwriteable zone
                                                    [6]otp locked   [7]error standby req,already in standby mode  [8]data request in fast program
                                                    [31]write 1 will refresh work clock domain timing parameter.write 0 has no affection.*/
    uint32_t timing1;               /* 0x2c */
    uint32_t timing2;               /* 0x30 */
    uint32_t lock;                  /* 0x34         [0]this bit controll recall pin status  [1]ctrl opera priority  [3:2]page0-1 lock reg,0 not lock,can be programmed*/
    uint32_t int_mask;              /* 0x38             [0]transfer done int mask   [1]data req int mask,  write 1 to mask*/
    uint32_t int_status;            /* 0x3c         [0]transfer done int  [1]data req int,  write 1 to clear*/
    uint32_t debug;                 /* 0x40         [7:0]Operation state machine  [9:8]Power state machine*/
    uint32_t reserved0[15];
    uint32_t buf_ctrl;              /* 0x80         [5:0]buf clear reg, bit0:DSP_I, bit1:DSP_D, bit2:CK_I, bit3:CK_D, bit4:M0, bit5:DMA   write 1 to clear.
                                                    [10:8]buffer prefetch reg, bit8:DSP_I, bit9:CK_I, bit10:M0   write 1 to enable*/
    uint32_t pri_cfg[7];            /* 0x84-0x9c    [3:0]default priority [7:4]highest priority [8]priority take effect flag*/
    uint32_t reserved1[24];         /* 0xA0-0xFC */
    uint32_t region0_cfg;           /* 0x100        [1:0]access priority  [2]1-secure aera, 0-nonsecure aera  [3]1-none executive  [5]read lock status, write 1 to lock*/
    uint32_t reserved2;
    uint32_t region1_cfg0;          /* 0x108        [1:0]access priority    [2]1-secure aera, 0-nonsecure aera  [3]1-none executive  [5]read lock status, write 1 to lock
                                                    [31:10]start address of region1*/
    uint32_t region1_cfg1;          /* 0x10c            [31:10]end address of region1*/
    uint32_t region2_cfg0;          /* 0x110 */
    uint32_t region2_cfg1;          /* 0x114 */
    uint32_t region3_cfg0;          /* 0x118 */
    uint32_t region3_cfg1;          /* 0x11c */
    uint32_t region4_cfg0;          /* 0x120 */
    uint32_t region4_cfg1;          /* 0x124 */
};

struct asc_si_efc_dev {
    volatile struct asc_si_efc_regs *reg;
};

enum asc_si_efc_ap_attr {
    NOACCESS_AND_NOACCESS   = 0,
    RW_AND_NOACCESS             = 1,
    RW_AND_RO               = 2,
    RW_AND_RW               = 3,
    MAX_EFC_SECUAP          = 4
};

enum asc_si_efc_nx_attr {
    ZTE_AND_EXE     =   0,
    ZTE_AND_NONEEXE =   1,
    CSKY_AND_EXE        =   2,
    CSKY_AND_NONEEXE    =   3,
    MAX_EFC_SECUNX  =   4
};
enum asc_si_efc_secregn_reg {
    EFC_REGION0_REG = 0,
    EFC_REGION1_REG = 1,
    EFC_REGION2_REG = 2,
    EFC_REGION3_REG = 3,
    EFC_REGION4_REG = 4,
    EFC_DEFUT_REG   = 31,/****VERSION\IMING\ LOCK\ DEBUG\ BUF_CTL\ SI*_CFG ****/
    MAX_EFC_SECUREG = 32
};

enum  asc_si_efc_id {
    EFLASH_DEV1         = 0,
    EFLASH_DEV2         = 1,
    MAX_EFLASHDEV_NUM   = 2
};

enum asc_si_ap_ram {
    AP_IRAM1    = 0,
    AP_IRAM2    = 1,
    MAX_AP_IRAM = 3
};

#endif
void eflash_secure_config(void);
void sram_secure_config(void);
#endif

