
#ifndef __REG_IOMUX_H__
#define __REG_IOMUX_H__

#define    REG_IOMUX_BASE                 0x60008000

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    func_sel_0          : 7  ; // 6  : 0
        uint32_t    reserved_3          : 1  ; // 7  : 7
        uint32_t    func_sel_1          : 7  ; // 14 : 8
        uint32_t    reserved_2          : 1  ; // 15 : 15
        uint32_t    func_sel_2          : 7  ; // 22 : 16
        uint32_t    reserved_1          : 1  ; // 23 : 23
        uint32_t    func_sel_3          : 7  ; // 30 : 24
        uint32_t    reserved_0          : 1  ; // 31 : 31
    } bit_field;
} t_iomux_func_isel_0_3;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    func_sel_4          : 7  ; // 6  : 0
        uint32_t    reserved_3          : 1  ; // 7  : 7
        uint32_t    func_sel_5          : 7  ; // 14 : 8
        uint32_t    reserved_2          : 1  ; // 15 : 15
        uint32_t    func_sel_6          : 7  ; // 22 : 16
        uint32_t    reserved_1          : 1  ; // 23 : 23
        uint32_t    func_sel_7          : 7  ; // 30 : 24
        uint32_t    reserved_0          : 1  ; // 31 : 31
    } bit_field;
} t_iomux_func_isel_4_7;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    func_sel_8          : 7  ; // 6  : 0
        uint32_t    reserved_3          : 1  ; // 7  : 7
        uint32_t    func_sel_9          : 7  ; // 14 : 8
        uint32_t    reserved_2          : 1  ; // 15 : 15
        uint32_t    func_sel_10         : 7  ; // 22 : 16
        uint32_t    reserved_1          : 1  ; // 23 : 23
        uint32_t    func_sel_11         : 7  ; // 30 : 24
        uint32_t    reserved_0          : 1  ; // 31 : 31
    } bit_field;
} t_iomux_func_isel_8_11;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    func_sel_12         : 7  ; // 6  : 0
        uint32_t    reserved_3          : 1  ; // 7  : 7
        uint32_t    func_sel_13         : 7  ; // 14 : 8
        uint32_t    reserved_2          : 1  ; // 15 : 15
        uint32_t    func_sel_14         : 7  ; // 22 : 16
        uint32_t    reserved_1          : 1  ; // 23 : 23
        uint32_t    func_sel_15         : 7  ; // 30 : 24
        uint32_t    reserved_0          : 1  ; // 31 : 31
    } bit_field;
} t_iomux_func_isel_12_15;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    func_sel_16         : 7  ; // 6  : 0
        uint32_t    reserved_3          : 1  ; // 7  : 7
        uint32_t    func_sel_17         : 7  ; // 14 : 8
        uint32_t    reserved_2          : 1  ; // 15 : 15
        uint32_t    func_sel_18         : 7  ; // 22 : 16
        uint32_t    reserved_1          : 1  ; // 23 : 23
        uint32_t    func_sel_19         : 7  ; // 30 : 24
        uint32_t    reserved_0          : 1  ; // 31 : 31
    } bit_field;
} t_iomux_func_isel_16_19;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    func_sel_20         : 7  ; // 6  : 0
        uint32_t    reserved_3          : 1  ; // 7  : 7
        uint32_t    func_sel_21         : 7  ; // 14 : 8
        uint32_t    reserved_2          : 1  ; // 15 : 15
        uint32_t    func_sel_22         : 7  ; // 22 : 16
        uint32_t    reserved_1          : 1  ; // 23 : 23
        uint32_t    func_sel_23         : 7  ; // 30 : 24
        uint32_t    reserved_0          : 1  ; // 31 : 31
    } bit_field;
} t_iomux_func_isel_20_23;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    func_sel_24         : 7  ; // 6  : 0
        uint32_t    reserved_3          : 1  ; // 7  : 7
        uint32_t    func_sel_25         : 7  ; // 14 : 8
        uint32_t    reserved_2          : 1  ; // 15 : 15
        uint32_t    func_sel_26         : 7  ; // 22 : 16
        uint32_t    reserved_1          : 1  ; // 23 : 23
        uint32_t    func_sel_27         : 7  ; // 30 : 24
        uint32_t    reserved_0          : 1  ; // 31 : 31
    } bit_field;
} t_iomux_func_isel_24_27;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    func_sel_28         : 7  ; // 6  : 0
        uint32_t    reserved_3          : 1  ; // 7  : 7
        uint32_t    func_sel_29         : 7  ; // 14 : 8
        uint32_t    reserved_2          : 1  ; // 15 : 15
        uint32_t    func_sel_30         : 7  ; // 22 : 16
        uint32_t    reserved_1          : 1  ; // 23 : 23
        uint32_t    func_sel_31         : 7  ; // 30 : 24
        uint32_t    reserved_0          : 1  ; // 31 : 31
    } bit_field;
} t_iomux_func_isel_28_31;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    swd_en              : 1  ; // 0  : 0
        uint32_t    dbg_en              : 1  ; // 1  : 1
        uint32_t    sdio_en             : 1  ; // 2  : 2
        uint32_t    sdio_host           : 1  ; // 3  : 3
        uint32_t    debug_port_oen      : 16 ; // 19 : 4
        uint32_t    uart0_ir_en         : 1  ; // 20 : 20
        uint32_t    uart1_ir_en         : 1  ; // 21 : 21
        uint32_t    reserved_0          : 10 ; // 31 : 22
    } bit_field;
} t_iomux_io_en;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    dsel                : 21 ; // 20 : 0
        uint32_t    reserved_0          : 11 ; // 31 : 21
    } bit_field;
} t_iomux_dsel_en;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    pull_up0            : 32 ; // 31 : 0
    } bit_field;
} t_iomux_pull_up0;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    pull_up1            : 17 ; // 16 : 0
        uint32_t    reserved_0          : 15 ; // 31 : 17
    } bit_field;
} t_iomux_pull_up1;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    pull_down0          : 32 ; // 31 : 0
    } bit_field;
} t_iomux_pull_down0;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    pull_down1          : 17 ; // 16 : 0
        uint32_t    reserved_0          : 15 ; // 31 : 17
    } bit_field;
} t_iomux_pull_down1;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    pad_hp0             : 32 ; // 31 : 0
    } bit_field;
} t_iomux_pad_hp0;

typedef union
{
    uint32_t val;
    struct
    {
        uint32_t    pad_hp1             : 17 ; // 16 : 0
        uint32_t    reserved_0          : 15 ; // 31 : 17
    } bit_field;
} t_iomux_pad_hp1;


//--------------------------------------------------------------------------------

typedef struct
{
    volatile        t_iomux_func_isel_0_3                    func_isel_0_3                          ; // 0x0
    volatile        t_iomux_func_isel_4_7                    func_isel_4_7                          ; // 0x4
    volatile        t_iomux_func_isel_8_11                   func_isel_8_11                         ; // 0x8
    volatile        t_iomux_func_isel_12_15                  func_isel_12_15                        ; // 0xc
    volatile        t_iomux_func_isel_16_19                  func_isel_16_19                        ; // 0x10
    volatile        t_iomux_func_isel_20_23                  func_isel_20_23                        ; // 0x14
    volatile        t_iomux_func_isel_24_27                  func_isel_24_27                        ; // 0x18
    volatile        t_iomux_func_isel_28_31                  func_isel_28_31                        ; // 0x1c
    volatile        t_iomux_io_en                            io_en                                  ; // 0x20
    volatile        t_iomux_dsel_en                          dsel_en                                ; // 0x24
    volatile        t_iomux_pull_up0                         pull_up0                               ; // 0x28
    volatile        t_iomux_pull_up1                         pull_up1                               ; // 0x2c
    volatile        t_iomux_pull_down0                       pull_down0                             ; // 0x30
    volatile        t_iomux_pull_down1                       pull_down1                             ; // 0x34
    volatile        t_iomux_pad_hp0                          pad_hp0                                ; // 0x38
    volatile        t_iomux_pad_hp1                          pad_hp1                                ; // 0x3c
} t_hwp_iomux_t;


//--------------------------------------------------------------------------------

__STATIC_INLINE void iomux_func_isel_0_3_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->func_isel_0_3.val = value;
}

__STATIC_INLINE void iomux_func_sel_3_setf(uint32_t base, uint8_t func_sel_3)
{
    ((t_hwp_iomux_t*)(base))->func_isel_0_3.bit_field.func_sel_3 = func_sel_3;
}

__STATIC_INLINE void iomux_func_sel_2_setf(uint32_t base, uint8_t func_sel_2)
{
    ((t_hwp_iomux_t*)(base))->func_isel_0_3.bit_field.func_sel_2 = func_sel_2;
}

__STATIC_INLINE void iomux_func_sel_1_setf(uint32_t base, uint8_t func_sel_1)
{
    ((t_hwp_iomux_t*)(base))->func_isel_0_3.bit_field.func_sel_1 = func_sel_1;
}

__STATIC_INLINE void iomux_func_sel_0_setf(uint32_t base, uint8_t func_sel_0)
{
    ((t_hwp_iomux_t*)(base))->func_isel_0_3.bit_field.func_sel_0 = func_sel_0;
}

__STATIC_INLINE void iomux_func_isel_4_7_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->func_isel_4_7.val = value;
}

__STATIC_INLINE void iomux_func_sel_7_setf(uint32_t base, uint8_t func_sel_7)
{
    ((t_hwp_iomux_t*)(base))->func_isel_4_7.bit_field.func_sel_7 = func_sel_7;
}

__STATIC_INLINE void iomux_func_sel_6_setf(uint32_t base, uint8_t func_sel_6)
{
    ((t_hwp_iomux_t*)(base))->func_isel_4_7.bit_field.func_sel_6 = func_sel_6;
}

__STATIC_INLINE void iomux_func_sel_5_setf(uint32_t base, uint8_t func_sel_5)
{
    ((t_hwp_iomux_t*)(base))->func_isel_4_7.bit_field.func_sel_5 = func_sel_5;
}

__STATIC_INLINE void iomux_func_sel_4_setf(uint32_t base, uint8_t func_sel_4)
{
    ((t_hwp_iomux_t*)(base))->func_isel_4_7.bit_field.func_sel_4 = func_sel_4;
}

__STATIC_INLINE void iomux_func_isel_8_11_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->func_isel_8_11.val = value;
}

__STATIC_INLINE void iomux_func_sel_11_setf(uint32_t base, uint8_t func_sel_11)
{
    ((t_hwp_iomux_t*)(base))->func_isel_8_11.bit_field.func_sel_11 = func_sel_11;
}

__STATIC_INLINE void iomux_func_sel_10_setf(uint32_t base, uint8_t func_sel_10)
{
    ((t_hwp_iomux_t*)(base))->func_isel_8_11.bit_field.func_sel_10 = func_sel_10;
}

__STATIC_INLINE void iomux_func_sel_9_setf(uint32_t base, uint8_t func_sel_9)
{
    ((t_hwp_iomux_t*)(base))->func_isel_8_11.bit_field.func_sel_9 = func_sel_9;
}

__STATIC_INLINE void iomux_func_sel_8_setf(uint32_t base, uint8_t func_sel_8)
{
    ((t_hwp_iomux_t*)(base))->func_isel_8_11.bit_field.func_sel_8 = func_sel_8;
}

__STATIC_INLINE void iomux_func_isel_12_15_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->func_isel_12_15.val = value;
}

__STATIC_INLINE void iomux_func_sel_15_setf(uint32_t base, uint8_t func_sel_15)
{
    ((t_hwp_iomux_t*)(base))->func_isel_12_15.bit_field.func_sel_15 = func_sel_15;
}

__STATIC_INLINE void iomux_func_sel_14_setf(uint32_t base, uint8_t func_sel_14)
{
    ((t_hwp_iomux_t*)(base))->func_isel_12_15.bit_field.func_sel_14 = func_sel_14;
}

__STATIC_INLINE void iomux_func_sel_13_setf(uint32_t base, uint8_t func_sel_13)
{
    ((t_hwp_iomux_t*)(base))->func_isel_12_15.bit_field.func_sel_13 = func_sel_13;
}

__STATIC_INLINE void iomux_func_sel_12_setf(uint32_t base, uint8_t func_sel_12)
{
    ((t_hwp_iomux_t*)(base))->func_isel_12_15.bit_field.func_sel_12 = func_sel_12;
}

__STATIC_INLINE void iomux_func_isel_16_19_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->func_isel_16_19.val = value;
}

__STATIC_INLINE void iomux_func_sel_19_setf(uint32_t base, uint8_t func_sel_19)
{
    ((t_hwp_iomux_t*)(base))->func_isel_16_19.bit_field.func_sel_19 = func_sel_19;
}

__STATIC_INLINE void iomux_func_sel_18_setf(uint32_t base, uint8_t func_sel_18)
{
    ((t_hwp_iomux_t*)(base))->func_isel_16_19.bit_field.func_sel_18 = func_sel_18;
}

__STATIC_INLINE void iomux_func_sel_17_setf(uint32_t base, uint8_t func_sel_17)
{
    ((t_hwp_iomux_t*)(base))->func_isel_16_19.bit_field.func_sel_17 = func_sel_17;
}

__STATIC_INLINE void iomux_func_sel_16_setf(uint32_t base, uint8_t func_sel_16)
{
    ((t_hwp_iomux_t*)(base))->func_isel_16_19.bit_field.func_sel_16 = func_sel_16;
}

__STATIC_INLINE void iomux_func_isel_20_23_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->func_isel_20_23.val = value;
}

__STATIC_INLINE void iomux_func_sel_23_setf(uint32_t base, uint8_t func_sel_23)
{
    ((t_hwp_iomux_t*)(base))->func_isel_20_23.bit_field.func_sel_23 = func_sel_23;
}

__STATIC_INLINE void iomux_func_sel_22_setf(uint32_t base, uint8_t func_sel_22)
{
    ((t_hwp_iomux_t*)(base))->func_isel_20_23.bit_field.func_sel_22 = func_sel_22;
}

__STATIC_INLINE void iomux_func_sel_21_setf(uint32_t base, uint8_t func_sel_21)
{
    ((t_hwp_iomux_t*)(base))->func_isel_20_23.bit_field.func_sel_21 = func_sel_21;
}

__STATIC_INLINE void iomux_func_sel_20_setf(uint32_t base, uint8_t func_sel_20)
{
    ((t_hwp_iomux_t*)(base))->func_isel_20_23.bit_field.func_sel_20 = func_sel_20;
}

__STATIC_INLINE void iomux_func_isel_24_27_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->func_isel_24_27.val = value;
}

__STATIC_INLINE void iomux_func_sel_27_setf(uint32_t base, uint8_t func_sel_27)
{
    ((t_hwp_iomux_t*)(base))->func_isel_24_27.bit_field.func_sel_27 = func_sel_27;
}

__STATIC_INLINE void iomux_func_sel_26_setf(uint32_t base, uint8_t func_sel_26)
{
    ((t_hwp_iomux_t*)(base))->func_isel_24_27.bit_field.func_sel_26 = func_sel_26;
}

__STATIC_INLINE void iomux_func_sel_25_setf(uint32_t base, uint8_t func_sel_25)
{
    ((t_hwp_iomux_t*)(base))->func_isel_24_27.bit_field.func_sel_25 = func_sel_25;
}

__STATIC_INLINE void iomux_func_sel_24_setf(uint32_t base, uint8_t func_sel_24)
{
    ((t_hwp_iomux_t*)(base))->func_isel_24_27.bit_field.func_sel_24 = func_sel_24;
}

__STATIC_INLINE void iomux_func_isel_28_31_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->func_isel_28_31.val = value;
}

__STATIC_INLINE void iomux_func_sel_31_setf(uint32_t base, uint8_t func_sel_31)
{
    ((t_hwp_iomux_t*)(base))->func_isel_28_31.bit_field.func_sel_31 = func_sel_31;
}

__STATIC_INLINE void iomux_func_sel_30_setf(uint32_t base, uint8_t func_sel_30)
{
    ((t_hwp_iomux_t*)(base))->func_isel_28_31.bit_field.func_sel_30 = func_sel_30;
}

__STATIC_INLINE void iomux_func_sel_29_setf(uint32_t base, uint8_t func_sel_29)
{
    ((t_hwp_iomux_t*)(base))->func_isel_28_31.bit_field.func_sel_29 = func_sel_29;
}

__STATIC_INLINE void iomux_func_sel_28_setf(uint32_t base, uint8_t func_sel_28)
{
    ((t_hwp_iomux_t*)(base))->func_isel_28_31.bit_field.func_sel_28 = func_sel_28;
}

__STATIC_INLINE void iomux_io_en_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->io_en.val = value;
}

__STATIC_INLINE void iomux_uart1_ir_en_setf(uint32_t base, uint8_t uart1_ir_en)
{
    ((t_hwp_iomux_t*)(base))->io_en.bit_field.uart1_ir_en = uart1_ir_en;
}

__STATIC_INLINE void iomux_uart0_ir_en_setf(uint32_t base, uint8_t uart0_ir_en)
{
    ((t_hwp_iomux_t*)(base))->io_en.bit_field.uart0_ir_en = uart0_ir_en;
}

__STATIC_INLINE void iomux_debug_port_oen_setf(uint32_t base, uint16_t debug_port_oen)
{
    ((t_hwp_iomux_t*)(base))->io_en.bit_field.debug_port_oen = debug_port_oen;
}

__STATIC_INLINE void iomux_sdio_host_setf(uint32_t base, uint8_t sdio_host)
{
    ((t_hwp_iomux_t*)(base))->io_en.bit_field.sdio_host = sdio_host;
}

__STATIC_INLINE void iomux_sdio_en_setf(uint32_t base, uint8_t sdio_en)
{
    ((t_hwp_iomux_t*)(base))->io_en.bit_field.sdio_en = sdio_en;
}

__STATIC_INLINE void iomux_dbg_en_setf(uint32_t base, uint8_t dbg_en)
{
    ((t_hwp_iomux_t*)(base))->io_en.bit_field.dbg_en = dbg_en;
}

__STATIC_INLINE void iomux_swd_en_setf(uint32_t base, uint8_t swd_en)
{
    ((t_hwp_iomux_t*)(base))->io_en.bit_field.swd_en = swd_en;
}

__STATIC_INLINE void iomux_dsel_en_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->dsel_en.val = value;
}

__STATIC_INLINE void iomux_dsel_setf(uint32_t base, uint32_t dsel)
{
    ((t_hwp_iomux_t*)(base))->dsel_en.bit_field.dsel = dsel;
}

__STATIC_INLINE void iomux_pull_up0_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->pull_up0.val = value;
}

__STATIC_INLINE void iomux_pull_up0_setf(uint32_t base, uint32_t pull_up0)
{
    ((t_hwp_iomux_t*)(base))->pull_up0.bit_field.pull_up0 = pull_up0;
}

__STATIC_INLINE void iomux_pull_up1_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->pull_up1.val = value;
}

__STATIC_INLINE void iomux_pull_up1_setf(uint32_t base, uint32_t pull_up1)
{
    ((t_hwp_iomux_t*)(base))->pull_up1.bit_field.pull_up1 = pull_up1;
}

__STATIC_INLINE void iomux_pull_down0_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->pull_down0.val = value;
}

__STATIC_INLINE void iomux_pull_down0_setf(uint32_t base, uint32_t pull_down0)
{
    ((t_hwp_iomux_t*)(base))->pull_down0.bit_field.pull_down0 = pull_down0;
}

__STATIC_INLINE void iomux_pull_down1_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->pull_down1.val = value;
}

__STATIC_INLINE void iomux_pull_down1_setf(uint32_t base, uint32_t pull_down1)
{
    ((t_hwp_iomux_t*)(base))->pull_down1.bit_field.pull_down1 = pull_down1;
}

__STATIC_INLINE void iomux_pad_hp0_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->pad_hp0.val = value;
}

__STATIC_INLINE void iomux_pad_hp0_setf(uint32_t base, uint32_t pad_hp0)
{
    ((t_hwp_iomux_t*)(base))->pad_hp0.bit_field.pad_hp0 = pad_hp0;
}

__STATIC_INLINE void iomux_pad_hp1_set(uint32_t base, uint32_t value)
{
    ((t_hwp_iomux_t*)(base))->pad_hp1.val = value;
}

__STATIC_INLINE void iomux_pad_hp1_setf(uint32_t base, uint32_t pad_hp1)
{
    ((t_hwp_iomux_t*)(base))->pad_hp1.bit_field.pad_hp1 = pad_hp1;
}


//--------------------------------------------------------------------------------

__STATIC_INLINE uint32_t iomux_func_isel_0_3_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_0_3.val;
}

__STATIC_INLINE uint8_t iomux_func_sel_3_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_0_3.bit_field.func_sel_3;
}

__STATIC_INLINE uint8_t iomux_func_sel_2_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_0_3.bit_field.func_sel_2;
}

__STATIC_INLINE uint8_t iomux_func_sel_1_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_0_3.bit_field.func_sel_1;
}

__STATIC_INLINE uint8_t iomux_func_sel_0_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_0_3.bit_field.func_sel_0;
}

__STATIC_INLINE uint32_t iomux_func_isel_4_7_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_4_7.val;
}

__STATIC_INLINE uint8_t iomux_func_sel_7_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_4_7.bit_field.func_sel_7;
}

__STATIC_INLINE uint8_t iomux_func_sel_6_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_4_7.bit_field.func_sel_6;
}

__STATIC_INLINE uint8_t iomux_func_sel_5_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_4_7.bit_field.func_sel_5;
}

__STATIC_INLINE uint8_t iomux_func_sel_4_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_4_7.bit_field.func_sel_4;
}

__STATIC_INLINE uint32_t iomux_func_isel_8_11_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_8_11.val;
}

__STATIC_INLINE uint8_t iomux_func_sel_11_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_8_11.bit_field.func_sel_11;
}

__STATIC_INLINE uint8_t iomux_func_sel_10_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_8_11.bit_field.func_sel_10;
}

__STATIC_INLINE uint8_t iomux_func_sel_9_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_8_11.bit_field.func_sel_9;
}

__STATIC_INLINE uint8_t iomux_func_sel_8_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_8_11.bit_field.func_sel_8;
}

__STATIC_INLINE uint32_t iomux_func_isel_12_15_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_12_15.val;
}

__STATIC_INLINE uint8_t iomux_func_sel_15_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_12_15.bit_field.func_sel_15;
}

__STATIC_INLINE uint8_t iomux_func_sel_14_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_12_15.bit_field.func_sel_14;
}

__STATIC_INLINE uint8_t iomux_func_sel_13_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_12_15.bit_field.func_sel_13;
}

__STATIC_INLINE uint8_t iomux_func_sel_12_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_12_15.bit_field.func_sel_12;
}

__STATIC_INLINE uint32_t iomux_func_isel_16_19_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_16_19.val;
}

__STATIC_INLINE uint8_t iomux_func_sel_19_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_16_19.bit_field.func_sel_19;
}

__STATIC_INLINE uint8_t iomux_func_sel_18_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_16_19.bit_field.func_sel_18;
}

__STATIC_INLINE uint8_t iomux_func_sel_17_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_16_19.bit_field.func_sel_17;
}

__STATIC_INLINE uint8_t iomux_func_sel_16_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_16_19.bit_field.func_sel_16;
}

__STATIC_INLINE uint32_t iomux_func_isel_20_23_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_20_23.val;
}

__STATIC_INLINE uint8_t iomux_func_sel_23_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_20_23.bit_field.func_sel_23;
}

__STATIC_INLINE uint8_t iomux_func_sel_22_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_20_23.bit_field.func_sel_22;
}

__STATIC_INLINE uint8_t iomux_func_sel_21_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_20_23.bit_field.func_sel_21;
}

__STATIC_INLINE uint8_t iomux_func_sel_20_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_20_23.bit_field.func_sel_20;
}

__STATIC_INLINE uint32_t iomux_func_isel_24_27_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_24_27.val;
}

__STATIC_INLINE uint8_t iomux_func_sel_27_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_24_27.bit_field.func_sel_27;
}

__STATIC_INLINE uint8_t iomux_func_sel_26_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_24_27.bit_field.func_sel_26;
}

__STATIC_INLINE uint8_t iomux_func_sel_25_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_24_27.bit_field.func_sel_25;
}

__STATIC_INLINE uint8_t iomux_func_sel_24_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_24_27.bit_field.func_sel_24;
}

__STATIC_INLINE uint32_t iomux_func_isel_28_31_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_28_31.val;
}

__STATIC_INLINE uint8_t iomux_func_sel_31_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_28_31.bit_field.func_sel_31;
}

__STATIC_INLINE uint8_t iomux_func_sel_30_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_28_31.bit_field.func_sel_30;
}

__STATIC_INLINE uint8_t iomux_func_sel_29_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_28_31.bit_field.func_sel_29;
}

__STATIC_INLINE uint8_t iomux_func_sel_28_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->func_isel_28_31.bit_field.func_sel_28;
}

__STATIC_INLINE uint32_t iomux_io_en_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->io_en.val;
}

__STATIC_INLINE uint8_t iomux_uart1_ir_en_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->io_en.bit_field.uart1_ir_en;
}

__STATIC_INLINE uint8_t iomux_uart0_ir_en_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->io_en.bit_field.uart0_ir_en;
}

__STATIC_INLINE uint16_t iomux_debug_port_oen_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->io_en.bit_field.debug_port_oen;
}

__STATIC_INLINE uint8_t iomux_sdio_host_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->io_en.bit_field.sdio_host;
}

__STATIC_INLINE uint8_t iomux_sdio_en_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->io_en.bit_field.sdio_en;
}

__STATIC_INLINE uint8_t iomux_dbg_en_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->io_en.bit_field.dbg_en;
}

__STATIC_INLINE uint8_t iomux_swd_en_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->io_en.bit_field.swd_en;
}

__STATIC_INLINE uint32_t iomux_dsel_en_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->dsel_en.val;
}

__STATIC_INLINE uint32_t iomux_dsel_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->dsel_en.bit_field.dsel;
}

__STATIC_INLINE uint32_t iomux_pull_up0_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pull_up0.val;
}

__STATIC_INLINE uint32_t iomux_pull_up0_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pull_up0.bit_field.pull_up0;
}

__STATIC_INLINE uint32_t iomux_pull_up1_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pull_up1.val;
}

__STATIC_INLINE uint32_t iomux_pull_up1_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pull_up1.bit_field.pull_up1;
}

__STATIC_INLINE uint32_t iomux_pull_down0_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pull_down0.val;
}

__STATIC_INLINE uint32_t iomux_pull_down0_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pull_down0.bit_field.pull_down0;
}

__STATIC_INLINE uint32_t iomux_pull_down1_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pull_down1.val;
}

__STATIC_INLINE uint32_t iomux_pull_down1_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pull_down1.bit_field.pull_down1;
}

__STATIC_INLINE uint32_t iomux_pad_hp0_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pad_hp0.val;
}

__STATIC_INLINE uint32_t iomux_pad_hp0_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pad_hp0.bit_field.pad_hp0;
}

__STATIC_INLINE uint32_t iomux_pad_hp1_get(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pad_hp1.val;
}

__STATIC_INLINE uint32_t iomux_pad_hp1_getf(uint32_t base)
{
    return ((t_hwp_iomux_t*)(base))->pad_hp1.bit_field.pad_hp1;
}


//--------------------------------------------------------------------------------

__STATIC_INLINE void iomux_func_isel_0_3_pack(uint32_t base, uint8_t func_sel_3, uint8_t func_sel_2, uint8_t func_sel_1, uint8_t func_sel_0)
{
    ((t_hwp_iomux_t*)(base))->func_isel_0_3.val = ( \
                                    ((uint32_t )func_sel_3        << 24) \
                                  | ((uint32_t )func_sel_2        << 16) \
                                  | ((uint32_t )func_sel_1        << 8) \
                                  | ((uint32_t )func_sel_0) \
                                  );
}

__STATIC_INLINE void iomux_func_isel_4_7_pack(uint32_t base, uint8_t func_sel_7, uint8_t func_sel_6, uint8_t func_sel_5, uint8_t func_sel_4)
{
    ((t_hwp_iomux_t*)(base))->func_isel_4_7.val = ( \
                                    ((uint32_t )func_sel_7        << 24) \
                                  | ((uint32_t )func_sel_6        << 16) \
                                  | ((uint32_t )func_sel_5        << 8) \
                                  | ((uint32_t )func_sel_4) \
                                  );
}

__STATIC_INLINE void iomux_func_isel_8_11_pack(uint32_t base, uint8_t func_sel_11, uint8_t func_sel_10, uint8_t func_sel_9, uint8_t func_sel_8)
{
    ((t_hwp_iomux_t*)(base))->func_isel_8_11.val = ( \
                                    ((uint32_t )func_sel_11       << 24) \
                                  | ((uint32_t )func_sel_10       << 16) \
                                  | ((uint32_t )func_sel_9        << 8) \
                                  | ((uint32_t )func_sel_8) \
                                  );
}

__STATIC_INLINE void iomux_func_isel_12_15_pack(uint32_t base, uint8_t func_sel_15, uint8_t func_sel_14, uint8_t func_sel_13, uint8_t func_sel_12)
{
    ((t_hwp_iomux_t*)(base))->func_isel_12_15.val = ( \
                                    ((uint32_t )func_sel_15       << 24) \
                                  | ((uint32_t )func_sel_14       << 16) \
                                  | ((uint32_t )func_sel_13       << 8) \
                                  | ((uint32_t )func_sel_12) \
                                  );
}

__STATIC_INLINE void iomux_func_isel_16_19_pack(uint32_t base, uint8_t func_sel_19, uint8_t func_sel_18, uint8_t func_sel_17, uint8_t func_sel_16)
{
    ((t_hwp_iomux_t*)(base))->func_isel_16_19.val = ( \
                                    ((uint32_t )func_sel_19       << 24) \
                                  | ((uint32_t )func_sel_18       << 16) \
                                  | ((uint32_t )func_sel_17       << 8) \
                                  | ((uint32_t )func_sel_16) \
                                  );
}

__STATIC_INLINE void iomux_func_isel_20_23_pack(uint32_t base, uint8_t func_sel_23, uint8_t func_sel_22, uint8_t func_sel_21, uint8_t func_sel_20)
{
    ((t_hwp_iomux_t*)(base))->func_isel_20_23.val = ( \
                                    ((uint32_t )func_sel_23       << 24) \
                                  | ((uint32_t )func_sel_22       << 16) \
                                  | ((uint32_t )func_sel_21       << 8) \
                                  | ((uint32_t )func_sel_20) \
                                  );
}

__STATIC_INLINE void iomux_func_isel_24_27_pack(uint32_t base, uint8_t func_sel_27, uint8_t func_sel_26, uint8_t func_sel_25, uint8_t func_sel_24)
{
    ((t_hwp_iomux_t*)(base))->func_isel_24_27.val = ( \
                                    ((uint32_t )func_sel_27       << 24) \
                                  | ((uint32_t )func_sel_26       << 16) \
                                  | ((uint32_t )func_sel_25       << 8) \
                                  | ((uint32_t )func_sel_24) \
                                  );
}

__STATIC_INLINE void iomux_func_isel_28_31_pack(uint32_t base, uint8_t func_sel_31, uint8_t func_sel_30, uint8_t func_sel_29, uint8_t func_sel_28)
{
    ((t_hwp_iomux_t*)(base))->func_isel_28_31.val = ( \
                                    ((uint32_t )func_sel_31       << 24) \
                                  | ((uint32_t )func_sel_30       << 16) \
                                  | ((uint32_t )func_sel_29       << 8) \
                                  | ((uint32_t )func_sel_28) \
                                  );
}

__STATIC_INLINE void iomux_io_en_pack(uint32_t base, uint8_t uart1_ir_en, uint8_t uart0_ir_en, uint16_t debug_port_oen, uint8_t sdio_host, uint8_t sdio_en, uint8_t dbg_en, uint8_t swd_en)
{
    ((t_hwp_iomux_t*)(base))->io_en.val = ( \
                                    ((uint32_t )uart1_ir_en       << 21) \
                                  | ((uint32_t )uart0_ir_en       << 20) \
                                  | ((uint32_t )debug_port_oen    << 4) \
                                  | ((uint32_t )sdio_host         << 3) \
                                  | ((uint32_t )sdio_en           << 2) \
                                  | ((uint32_t )dbg_en            << 1) \
                                  | ((uint32_t )swd_en) \
                                  );
}


//--------------------------------------------------------------------------------

__STATIC_INLINE void iomux_func_isel_0_3_unpack(uint32_t base, uint8_t * func_sel_3, uint8_t * func_sel_2, uint8_t * func_sel_1, uint8_t * func_sel_0)
{
    t_iomux_func_isel_0_3 local_val = ((t_hwp_iomux_t*)(base))->func_isel_0_3;

    *func_sel_3         = local_val.bit_field.func_sel_3;
    *func_sel_2         = local_val.bit_field.func_sel_2;
    *func_sel_1         = local_val.bit_field.func_sel_1;
    *func_sel_0         = local_val.bit_field.func_sel_0;
}

__STATIC_INLINE void iomux_func_isel_4_7_unpack(uint32_t base, uint8_t * func_sel_7, uint8_t * func_sel_6, uint8_t * func_sel_5, uint8_t * func_sel_4)
{
    t_iomux_func_isel_4_7 local_val = ((t_hwp_iomux_t*)(base))->func_isel_4_7;

    *func_sel_7         = local_val.bit_field.func_sel_7;
    *func_sel_6         = local_val.bit_field.func_sel_6;
    *func_sel_5         = local_val.bit_field.func_sel_5;
    *func_sel_4         = local_val.bit_field.func_sel_4;
}

__STATIC_INLINE void iomux_func_isel_8_11_unpack(uint32_t base, uint8_t * func_sel_11, uint8_t * func_sel_10, uint8_t * func_sel_9, uint8_t * func_sel_8)
{
    t_iomux_func_isel_8_11 local_val = ((t_hwp_iomux_t*)(base))->func_isel_8_11;

    *func_sel_11        = local_val.bit_field.func_sel_11;
    *func_sel_10        = local_val.bit_field.func_sel_10;
    *func_sel_9         = local_val.bit_field.func_sel_9;
    *func_sel_8         = local_val.bit_field.func_sel_8;
}

__STATIC_INLINE void iomux_func_isel_12_15_unpack(uint32_t base, uint8_t * func_sel_15, uint8_t * func_sel_14, uint8_t * func_sel_13, uint8_t * func_sel_12)
{
    t_iomux_func_isel_12_15 local_val = ((t_hwp_iomux_t*)(base))->func_isel_12_15;

    *func_sel_15        = local_val.bit_field.func_sel_15;
    *func_sel_14        = local_val.bit_field.func_sel_14;
    *func_sel_13        = local_val.bit_field.func_sel_13;
    *func_sel_12        = local_val.bit_field.func_sel_12;
}

__STATIC_INLINE void iomux_func_isel_16_19_unpack(uint32_t base, uint8_t * func_sel_19, uint8_t * func_sel_18, uint8_t * func_sel_17, uint8_t * func_sel_16)
{
    t_iomux_func_isel_16_19 local_val = ((t_hwp_iomux_t*)(base))->func_isel_16_19;

    *func_sel_19        = local_val.bit_field.func_sel_19;
    *func_sel_18        = local_val.bit_field.func_sel_18;
    *func_sel_17        = local_val.bit_field.func_sel_17;
    *func_sel_16        = local_val.bit_field.func_sel_16;
}

__STATIC_INLINE void iomux_func_isel_20_23_unpack(uint32_t base, uint8_t * func_sel_23, uint8_t * func_sel_22, uint8_t * func_sel_21, uint8_t * func_sel_20)
{
    t_iomux_func_isel_20_23 local_val = ((t_hwp_iomux_t*)(base))->func_isel_20_23;

    *func_sel_23        = local_val.bit_field.func_sel_23;
    *func_sel_22        = local_val.bit_field.func_sel_22;
    *func_sel_21        = local_val.bit_field.func_sel_21;
    *func_sel_20        = local_val.bit_field.func_sel_20;
}

__STATIC_INLINE void iomux_func_isel_24_27_unpack(uint32_t base, uint8_t * func_sel_27, uint8_t * func_sel_26, uint8_t * func_sel_25, uint8_t * func_sel_24)
{
    t_iomux_func_isel_24_27 local_val = ((t_hwp_iomux_t*)(base))->func_isel_24_27;

    *func_sel_27        = local_val.bit_field.func_sel_27;
    *func_sel_26        = local_val.bit_field.func_sel_26;
    *func_sel_25        = local_val.bit_field.func_sel_25;
    *func_sel_24        = local_val.bit_field.func_sel_24;
}

__STATIC_INLINE void iomux_func_isel_28_31_unpack(uint32_t base, uint8_t * func_sel_31, uint8_t * func_sel_30, uint8_t * func_sel_29, uint8_t * func_sel_28)
{
    t_iomux_func_isel_28_31 local_val = ((t_hwp_iomux_t*)(base))->func_isel_28_31;

    *func_sel_31        = local_val.bit_field.func_sel_31;
    *func_sel_30        = local_val.bit_field.func_sel_30;
    *func_sel_29        = local_val.bit_field.func_sel_29;
    *func_sel_28        = local_val.bit_field.func_sel_28;
}

__STATIC_INLINE void iomux_io_en_unpack(uint32_t base, uint8_t * uart1_ir_en, uint8_t * uart0_ir_en, uint16_t * debug_port_oen, uint8_t * sdio_host, uint8_t * sdio_en, uint8_t * dbg_en, uint8_t * swd_en)
{
    t_iomux_io_en local_val = ((t_hwp_iomux_t*)(base))->io_en;

    *uart1_ir_en        = local_val.bit_field.uart1_ir_en;
    *uart0_ir_en        = local_val.bit_field.uart0_ir_en;
    *debug_port_oen     = local_val.bit_field.debug_port_oen;
    *sdio_host          = local_val.bit_field.sdio_host;
    *sdio_en            = local_val.bit_field.sdio_en;
    *dbg_en             = local_val.bit_field.dbg_en;
    *swd_en             = local_val.bit_field.swd_en;
}

#endif

