/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef RISCV_CSR_H
#define RISCV_CSR_H

#if __riscv_xlen == 64
	#define portWORD_SIZE 8
	#define store_x sd
	#define load_x ld
#elif __riscv_xlen == 32
	#define store_x sw
	#define load_x lw
	#define portWORD_SIZE 4
#else
	#error Assembler did not define __riscv_xlen
#endif

#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
#define MODE_PREFIX(suffix) s##suffix
#else
#define MODE_PREFIX(suffix) m##suffix
#endif

/* Status register flags */
#define SR_SIE      0x00000002UL        /*  Supervisor Interrupt Enable */
#define SR_MIE      0x00000008UL        /*  Machine Interrupt Enable */
#define SR_SPIE     0x00000020UL        /*  Previous Supervisor IE */
#define SR_MPIE     0x00000080UL        /*  Previous Machine IE */
#define SR_SPP_U    0x00000000UL        /*  Previously User mode */
#define SR_SPP_S    0x00000100UL        /*  Previously Supervisor mode */
#define SR_MPP_U    0x00000000UL        /*  Previously User mode */
#define SR_MPP_S    0x00000800UL        /*  Previously Supervisor mode */
#define SR_MPP_M    0x00001800UL        /*  Previously Machine mode */
#define SR_SUM      0x00040000UL        /*  Supervisor User Memory Access */

#define SR_FS           0x00006000UL    /*  Floating-point Status */
#define SR_FS_OFF       0x00000000UL
#define SR_FS_INITIAL   0x00002000UL
#define SR_FS_CLEAN     0x00004000UL
#define SR_FS_DIRTY     0x00006000UL

/* Interrupt-enable Registers */
#define IE_MTIE         0x00000080UL
#define IE_MEIE         0x00000800UL

/*  ===== Trap/Exception Causes ===== */
#define CAUSE_MISALIGNED_FETCH          0x0
#define CAUSE_FETCH_ACCESS              0x1
#define CAUSE_ILLEGAL_INSTRUCTION       0x2
#define CAUSE_BREAKPOINT                0x3
#define CAUSE_MISALIGNED_LOAD           0x4
#define CAUSE_LOAD_ACCESS               0x5
#define CAUSE_MISALIGNED_STORE          0x6
#define CAUSE_STORE_ACCESS              0x7
#define CAUSE_USER_ECALL                0x8
#define CAUSE_SUPERVISOR_ECALL          0x9
#define CAUSE_VIRTUAL_SUPERVISOR_ECALL  0xa
#define CAUSE_MACHINE_ECALL             0xb
#define CAUSE_FETCH_PAGE_FAULT          0xc
#define CAUSE_LOAD_PAGE_FAULT           0xd
#define CAUSE_STORE_PAGE_FAULT          0xf

#define PRV_U                           0
#define PRV_S                           1
#define PRV_M                           3

#define MSTATUS_SIE                     0x00000002
#define MSTATUS_MIE                     0x00000008
#define MSTATUS_SPIE_SHIFT              5
#define MSTATUS_SPIE                    (1 << MSTATUS_SPIE_SHIFT)
#define MSTATUS_UBE                     0x00000040
#define MSTATUS_MPIE                    0x00000080
#define MSTATUS_SPP_SHIFT               8
#define MSTATUS_SPP                     (1 << MSTATUS_SPP_SHIFT)
#define MSTATUS_MPP_SHIFT               11
#define MSTATUS_MPP                     (3 << MSTATUS_MPP_SHIFT)

#define INSERT_FIELD(val, which, fieldval)	(((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))


#endif

