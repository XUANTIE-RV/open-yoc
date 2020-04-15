/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_mailbox_v2.h
 * @brief    header file for mailbox driver
 * @version  V1.0
 * @date     08. Mar 2019
 ******************************************************************************/
#ifndef _CK_MAILBOX_V2_H_
#define _CK_MAILBOX_V2_H_

#include <stdio.h>
#include <errno.h>
#include <csi_config.h>
#include <soc.h>
#include <io.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UP_ALIGN4(a)                        (((a) + 3) & 0xFFFFFFFCU)

#ifdef CONFIG_CHIP_YUNVOICE
#define CSKY_MBOX_MAX_MESSAGE_LENGTH        24
#define CSKY_MBOX_SEND_MAX_MESSAGE_LENGTH   20

/* Raw Interrupt Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_WINTRIS   0x00

/* Interrupt Unmask Register (R/W)
 * 0 means mask interrupt bit
 * 1 means unmask interrupt bit
 */
#define CSKY_MBOX_WINTMR    0x04

/* Interrupt Register Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_WINTRSR   0x08

/* Interrupt Clear Register (W1C)
 * Write 1 into it, clear the interrupt
 */
#define CSKY_MBOX_WINTCR    0x0C

/* Raw Interrupt Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_RINTRIS   0x30

/* Interrupt Unmask Register (R/W)
 * 0 means mask interrupt bit
 * 1 means unmask interrupt bit
 */
#define CSKY_MBOX_RINTMR    0x34

/* Interrupt Register Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_RINTRSR   0x38

/* Interrupt Clear Register (W1C)
 * Write 1 into it, clear the interrupt
 */
#define CSKY_MBOX_RINTCR    0x3C

#define MBOX_WINTCR_ADDR(mbox)   \
    (mbox->base + (mbox->src_idx) * 0x10 + 0x1C + CSKY_MBOX_WINTCR)
#define MBOX_RINTCR_ADDR(mbox)   \
    (mbox->base + (mbox->src_idx) * 0x10 + 0x1C + CSKY_MBOX_RINTCR)
#define MBOX_WINTMR_ADDR(mbox)   \
    (mbox->base + (mbox->src_idx) * 0x10 + 0x1C + CSKY_MBOX_WINTMR)
#define MBOX_RINTMR_ADDR(mbox)   \
    (mbox->base + (mbox->src_idx) * 0x10 + 0x1C + CSKY_MBOX_RINTMR)
#define MBOX_WINTUMR_ADDR(mbox)   \
    (mbox->dst_base + (mbox->dst_idx) * 0x10 + 0x1C + CSKY_MBOX_WINTMR)
#define MBOX_RINTUMR_ADDR(mbox)   \
    (mbox->dst_base + (mbox->dst_idx) * 0x10 + 0x1C + CSKY_MBOX_RINTMR)
#define MBOX_WINTRIS_ADDR(mbox)  \
    (mbox->base + (mbox->src_idx) * 0x10 + 0x1C + CSKY_MBOX_WINTRIS)
#define MBOX_WINTRSR_ADDR(mbox)  \
    (mbox->base + (mbox->src_idx) * 0x10 + 0x1C + CSKY_MBOX_WINTRSR)

#define MBOX_TX_ACK_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->dst_base + 0xA0 + 0x30 * (mbox->dst_idx) + CSKY_MBOX_MAX_MESSAGE_LENGTH*(mbox->dst_channel_idx)))
#define MBOX_RX_ACK_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->base + 0xA0 + 0x30 * (mbox->src_idx) + CSKY_MBOX_MAX_MESSAGE_LENGTH*(mbox->src_channel_idx)))
#define MBOX_TX_MSSG_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->dst_base + 0x8C + 0x30 * (mbox->dst_idx) + CSKY_MBOX_MAX_MESSAGE_LENGTH*(mbox->dst_channel_idx)))
#define MBOX_RX_MSSG_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x8C + 0x30 * (mbox->src_idx) + CSKY_MBOX_MAX_MESSAGE_LENGTH*(mbox->src_channel_idx)))
#define MBOX_RX_MSSG_ADDR_START(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x8C + 0x30 * (mbox->src_idx)))

#define CLEAR_WRITE_INTERRUPT(mbox, val)    putreg32(val, (volatile uint32_t *)MBOX_WINTCR_ADDR(mbox))
#define CLEAR_READ_INTERRUPT(mbox, val)     putreg32(val, (volatile uint32_t *)MBOX_RINTCR_ADDR(mbox))
#define MASK_WRITE_INTERRUPT(mbox, val)     putreg32(val, (volatile uint32_t *)MBOX_WINTMR_ADDR(mbox))
#define MASK_READ_INTERRUPT(mbox, val)      putreg32(val, (volatile uint32_t *)MBOX_RINTMR_ADDR(mbox))
#define UNMASK_WRITE_INTERRUPT(mbox, val)   putreg32(val, (volatile uint32_t *)MBOX_WINTUMR_ADDR(mbox))
#define UNMASK_READ_INTERRUPT(mbox, val)    putreg32(val, (volatile uint32_t *)MBOX_RINTUMR_ADDR(mbox))
#define RX_RAW_READ_STATUS_INTERRUPT(mbox)  getreg32((volatile uint32_t *)MBOX_WINTRIS_ADDR(mbox))
#define RX_READ_STATUS_INTERRUPT(mbox)      getreg32((volatile uint32_t *)MBOX_WINTRSR_ADDR(mbox))

#define GET_MASK_INTERRUPT(mbox)            getreg32((volatile uint32_t *)MBOX_WINTMR_ADDR(mbox))

#define GET_UNMASK_INTERRUPT(mbox)          getreg32((volatile uint32_t *)MBOX_WINTUMR_ADDR(mbox))

#define MAILBOX_CPU_REG_OFFSET(idx)         ((CSKY_MBOX_MAX_MESSAGE_LENGTH >> 2) * idx)
#else

#define CSKY_MBOX_MAX_MESSAGE_LENGTH        24
#define CSKY_MBOX_SEND_MAX_MESSAGE_LENGTH   20

/* Raw Interrupt Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_WINTRIS   0x00

/* Interrupt Unmask Register (R/W)
 * 0 means mask interrupt bit
 * 1 means unmask interrupt bit
 */
#define CSKY_MBOX_WINTMR    0x04

/* Interrupt Register Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_WINTRSR   0x08

/* Interrupt Clear Register (W1C)
 * Write 1 into it, clear the interrupt
 */
#define CSKY_MBOX_WINTCR    0x0C

/* Raw Interrupt Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_RINTRIS   0x10

/* Interrupt Unmask Register (R/W)
 * 0 means mask interrupt bit
 * 1 means unmask interrupt bit
 */
#define CSKY_MBOX_RINTMR    0x14

/* Interrupt Register Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define CSKY_MBOX_RINTRSR   0x18

/* Interrupt Clear Register (W1C)
 * Write 1 into it, clear the interrupt
 */
#define CSKY_MBOX_RINTCR    0x1C

#define MBOX_WINTCR_ADDR(mbox)   \
    (mbox->base + 0x100*(mbox->src_idx) + CSKY_MBOX_WINTCR)
#define MBOX_RINTCR_ADDR(mbox)   \
    (mbox->base + 0x100*(mbox->src_idx) + CSKY_MBOX_RINTCR)
#define MBOX_WINTMR_ADDR(mbox)   \
    (mbox->base + 0x100*(mbox->src_idx) + CSKY_MBOX_WINTMR)
#define MBOX_RINTMR_ADDR(mbox)   \
    (mbox->base + 0x100*(mbox->src_idx) + CSKY_MBOX_RINTMR)
#define MBOX_WINTUMR_ADDR(mbox)   \
    (mbox->dst_base + 0x100*(mbox->dst_idx) + CSKY_MBOX_WINTMR)
#define MBOX_RINTUMR_ADDR(mbox)   \
    (mbox->dst_base + 0x100*(mbox->dst_idx) + CSKY_MBOX_RINTMR)
#define MBOX_WINTRIS_ADDR(mbox)  \
    (mbox->base + 0x100*(mbox->src_idx) + CSKY_MBOX_WINTRIS)
#define MBOX_WINTRSR_ADDR(mbox)  \
    (mbox->base + 0x100*(mbox->src_idx) + CSKY_MBOX_WINTRSR)

#define MBOX_TX_ACK_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->dst_base + 0x34 + 0x100*(mbox->dst_idx) + CSKY_MBOX_MAX_MESSAGE_LENGTH*(mbox->dst_channel_idx)))
#define MBOX_RX_ACK_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x34 + 0x100*(mbox->src_idx) + CSKY_MBOX_MAX_MESSAGE_LENGTH*(mbox->src_channel_idx)))
#define MBOX_RX_MSSG_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x20 + 0x100*(mbox->src_idx) + CSKY_MBOX_MAX_MESSAGE_LENGTH*(mbox->src_channel_idx)))
#define MBOX_RX_MSSG_ADDR_START(mbox) (void *)((volatile uint32_t *)(mbox->base + 0x20 + 0x100*(mbox->src_idx)))
#define MBOX_TX_MSSG_ADDR(mbox) (void *)((volatile uint32_t *)(mbox->dst_base + 0x20 + 0x100*(mbox->dst_idx) + CSKY_MBOX_MAX_MESSAGE_LENGTH*(mbox->dst_channel_idx)))

#define CLEAR_WRITE_INTERRUPT(mbox, val)    putreg32(val, (volatile uint32_t *)MBOX_WINTCR_ADDR(mbox))
#define CLEAR_READ_INTERRUPT(mbox, val)     putreg32(val, (volatile uint32_t *)MBOX_RINTCR_ADDR(mbox))
#define MASK_WRITE_INTERRUPT(mbox, val)     putreg32(val, (volatile uint32_t *)MBOX_WINTMR_ADDR(mbox))
#define MASK_READ_INTERRUPT(mbox, val)      putreg32(val, (volatile uint32_t *)MBOX_RINTMR_ADDR(mbox))
#define UNMASK_WRITE_INTERRUPT(mbox, val)   putreg32(val, (volatile uint32_t *)MBOX_WINTUMR_ADDR(mbox))
#define UNMASK_READ_INTERRUPT(mbox, val)    putreg32(val, (volatile uint32_t *)MBOX_RINTUMR_ADDR(mbox))
#define RX_RAW_READ_STATUS_INTERRUPT(mbox)  getreg32((volatile uint32_t *)MBOX_WINTRIS_ADDR(mbox))
#define RX_READ_STATUS_INTERRUPT(mbox)      getreg32((volatile uint32_t *)MBOX_WINTRSR_ADDR(mbox))

#define GET_MASK_INTERRUPT(mbox)            getreg32((volatile uint32_t *)MBOX_WINTMR_ADDR(mbox))

#define GET_UNMASK_INTERRUPT(mbox)          getreg32((volatile uint32_t *)MBOX_WINTUMR_ADDR(mbox))

#define MAILBOX_CPU_REG_OFFSET(idx)         ((CSKY_MBOX_MAX_MESSAGE_LENGTH >> 2) * idx)

#endif
#ifdef __cplusplus
}
#endif

#endif /* _CK_MAILBOX_H_ */

