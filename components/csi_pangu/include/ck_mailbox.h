/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_mailbox.h
 * @brief    header file for mailbox driver
 * @version  V1.0
 * @date     29. Jan 2019
 ******************************************************************************/
#ifndef _CK_MAILBOX_H_
#define _CK_MAILBOX_H_

#include <stdio.h>
#include <errno.h>
#include <soc.h>
#include <io.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UP_ALIGN4(a)                        (((a) + 3) & 0xFFFFFFFCU)
#define CSKY_MBOX_MAX_MESSAGE_LENGTH        64
#define CSKY_MBOX_SEND_MAX_MESSAGE_LENGTH   60
/* Interrupt Generate Register (R/W)
 * Write 1 into it, enable interrupt to client immediately.
 * Write 0 into it, disable interrupt to client immediately.
 */
#define CSKY_MBOX_INTGR     0x00

/* Interrupt Clear Register (W1C)
 * Write 1 into it, clear the interrupt
 */
#define CSKY_MBOX_INTCR     0x04

/* Interrupt Mask Register (R/W)
 * 1 means mask interrupt bit
 * 0 means unmask interrupt bit
 */
#define CSKY_MBOX_INTMR     0x08

/* Interrupt Register Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_INTRSR    0x0C

/* Interrupt Masked Status Register (RO)
 * Read 1 means has interrupt on masked bit
 * Read 0 means no interrupt on masked bit
 */
#define CSKY_MBOX_INTMSR    0x10

/* Interrupt Enable (R/W)
 * 1 means interrupt enabled
 * 0 means interrupt disabled
 */
#define CSKY_MBOX_INTENB    0x14


/* 0x18 is register length from CSKY_MBOX_INTGR to CSKY_MBOX_INTENB */
#define MBOX_INTGR_ADDR(mbox)   \
    (mbox->base + 0x18*(mbox->idx ? 0 : 1) + CSKY_MBOX_INTGR)
#define MBOX_INTCR_ADDR(mbox)   \
    (mbox->base + 0x18*(mbox->idx) + CSKY_MBOX_INTCR)
#define MBOX_INTMR_ADDR(mbox)   \
    (mbox->base + 0x18*(mbox->idx) + CSKY_MBOX_INTMR)
#define MBOX_INTRSR_ADDR(mbox)  \
    (mbox->base + 0x18*(mbox->idx) + CSKY_MBOX_INTRSR)
#define MBOX_INTMSR_ADDR(mbox)  \
    (mbox->base + 0x18*(mbox->idx) + CSKY_MBOX_INTMSR)
#define MBOX_INTENB_ADDR(mbox)  \
    (mbox->base + 0x18*(mbox->idx) + CSKY_MBOX_INTENB)
#define MBOX_TX_ACK_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x18*2 + (mbox->idx ? 124 : 60)))
#define MBOX_RX_ACK_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x18*2 + (mbox->idx ? 60 : 124)))
#define MBOX_TX_MSSG_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x18*2 + (mbox->idx ? 64 : 0)))
#define MBOX_RX_MSSG_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x18*2 + (mbox->idx ? 0 : 64)))

#define TX_GENERATE_INTERRUPT(mbox, val)    putreg32(val, (volatile uint32_t *)MBOX_INTGR_ADDR(mbox))
#define TX_READ_INTERRUPT(mbox)             getreg32((volatile uint32_t *)MBOX_INTGR_ADDR(mbox))
#define RX_CLEAR_INTERRUPT(mbox, val)       putreg32(val, (volatile uint32_t *)MBOX_INTCR_ADDR(mbox))
#define RX_MASK_INTERRUPT(mbox, val)        putreg32(val, (volatile uint32_t *)MBOX_INTMR_ADDR(mbox))
#define RX_UNMASK_INTERRUPT(mbox, val)      putreg32(val, (volatile uint32_t *)MBOX_INTMR_ADDR(mbox))
#define RX_READ_INTERRUPT(mbox, val)        getreg32((volatile uint32_t *)MBOX_INTRSR_ADDR(mbox))
#define RX_READ_MASKED_INTERRUPT(mbox)      getreg32((volatile uint32_t *)MBOX_INTMSR_ADDR(mbox))
#define RX_ENABLE_INTERRUPT(mbox, val)      putreg32(val, (volatile uint32_t *)MBOX_INTENB_ADDR(mbox))
#define RX_DISABLE_INTERRUPT(mbox, val)     putreg32(val, (volatile uint32_t *)MBOX_INTENB_ADDR(mbox))

#ifdef __cplusplus
}
#endif

#endif /* _CK_MAILBOX_H_ */

