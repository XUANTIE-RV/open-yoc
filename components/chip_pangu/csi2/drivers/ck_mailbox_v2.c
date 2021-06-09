/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     ck_mailbox_v2.c
 * @brief    CSI Source File for mailbox Driver
 * @version  V1.0
 * @date     06. Mar 2019
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ck_mailbox_v2.h>
#include <drv_mailbox.h>
#include <soc.h>
#include <drv/mbox.h>
#include <drv/irq.h>
#include <drv/porting.h>

#define ERR_MAILBOX(errno) CSI_ERROR

#define MBOX_ACK        0x4B6765
#define ACK_BIT         (CSKY_MBOX_SEND_MAX_MESSAGE_LENGTH >> 2)

typedef struct {
    uint32_t           base;
    int32_t            src_idx;
    int32_t            dst_idx;
    uint32_t           dst_base;
    int32_t            src_channel_idx;
    int32_t            dst_channel_idx;
} ck_mbox_t;

typedef struct {
    uint32_t           base;
    int32_t            src_idx;
    uint32_t           irq;
    mailbox_event_cb_t cb_event;           ///< Event callback
    uint32_t           receive_len;
    csi_mbox_t         *mbox;
} ck_mailbox_priv_t;

extern uint32_t soc_get_cpu_id(void);
static ck_mailbox_priv_t mailbox_instance[CONFIG_MAILBOX_NUM];

static int32_t ck_mailbox_get_channel_idx(int32_t src_idx, int32_t dst_idx, int32_t *src_channel_idx, int32_t *dst_channel_idx)
{
    if (src_idx == dst_idx) {
        return -1;
    }

    *src_channel_idx = (dst_idx > src_idx) ? (dst_idx - 1) : dst_idx;
    *dst_channel_idx = (src_idx > dst_idx) ? (src_idx - 1) : src_idx;
    return 0;
}

static void ck_mailbox_get_dst_idx(ck_mbox_t *mbox)
{
    mbox->dst_idx = (mbox->src_idx > mbox->src_channel_idx) ? mbox->src_channel_idx : (mbox->src_channel_idx + 1);
}

/**
  \brief       the interrupt service function.
  \param[in]   index of mailbox instance.
*/
void ck_mailbox_irqhandler(void *arg)
{
    int idx = soc_get_cpu_id();
    ck_mbox_t mbox = {0};
    ck_mailbox_priv_t *priv;
    priv = &mailbox_instance[idx];
    priv->mbox = (csi_mbox_t *)arg;

    uint32_t status = RX_RAW_READ_STATUS_INTERRUPT(priv);
    uint32_t rx_len;
    int32_t j;
    mbox.base    = priv->base;
    mbox.src_idx = priv->src_idx;

    for (j = 0; j < CONFIG_MAILBOX_CHANNEL_NUM; j++) {
        for (rx_len = 0; rx_len < (CSKY_MBOX_MAX_MESSAGE_LENGTH >> 2) - 1; rx_len++) {
            if (!(status & (1 << (rx_len + MAILBOX_CPU_REG_OFFSET(j))))) {
                break;
            }
        }

        if (!rx_len) {
            /* patch: rx may be incomplete */
            status &= (~(BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH >> 2) - 1) << MAILBOX_CPU_REG_OFFSET(j)));

            if (!(status & (BIT(ACK_BIT) << MAILBOX_CPU_REG_OFFSET(j)))) {
                continue;
            }
        }

        mbox.src_channel_idx = j;
        ck_mailbox_get_dst_idx(&mbox);

        /* handle the ack irq */
        if (status & (BIT(ACK_BIT) << MAILBOX_CPU_REG_OFFSET(j))) {
            //getreg32(MBOX_RX_ACK_ADDR(priv));
            if (priv->cb_event) {
                priv->cb_event(priv, mbox.dst_idx, priv->receive_len, MAILBOX_EVENT_SEND_COMPLETE);
            }
        }

        if (rx_len) {
            priv->receive_len = rx_len << 2;

            /* call callback to read the data */
            if (priv->cb_event) {
                priv->cb_event(priv, mbox.dst_idx, priv->receive_len, MAILBOX_EVENT_RECEIVED);
            }
        }

        CLEAR_WRITE_INTERRUPT(priv, status & (BIT_MASK(CSKY_MBOX_MAX_MESSAGE_LENGTH >> 2) << MAILBOX_CPU_REG_OFFSET(j)));
    }
}

static int cpuid_to_mb_chid(int cpu_id)
{
    switch(soc_get_cpu_id()) {
        case 0:
            return cpu_id - 1;
        case 1:
            if (cpu_id == 2) {
                return cpu_id - 1;
            }
            return cpu_id;
        case 2:
            return cpu_id;
        default:
            break;
    }

    return 0;
}

static void mailbox_event_cb(mailbox_handle_t handle, int32_t mailbox_id, uint32_t received_len, mailbox_event_e event)
{
    ck_mailbox_priv_t *priv = handle;
    csi_mbox_t *mbox = priv->mbox;
    uint32_t channel_id = cpuid_to_mb_chid(mailbox_id);
    if (mbox->callback != NULL) {
        mbox->callback(mbox, event, channel_id, received_len, mbox->arg);
    }
}
/**
  \brief       Initialize MAILBOX Interface. 1. Initializes the resources needed for the MAILBOX interface 2.registers event callback function
  \param[in]   cb_event  event call back function \ref mailbox_event_cb_t
  \return      return mailbox handle if success
*/
mailbox_handle_t csi_mailbox_initialize(csi_mbox_t *mbox)
{
    int32_t src_idx;
    src_idx = soc_get_cpu_id();
    csi_error_t ret = target_get(DEV_WJ_MBOX_TAG, 0, &mbox->dev);

    if (ret != CSI_OK) {
        return NULL;
    }

    ck_mailbox_priv_t *priv = &mailbox_instance[src_idx];
    priv->base              = mbox->dev.reg_base;
    priv->irq               = mbox->dev.irq_num;
    priv->cb_event          = mailbox_event_cb;
    priv->src_idx           = src_idx;

    MASK_READ_INTERRUPT(priv, 0);
    /* invalid interrupt may be come, mask it */
    MASK_WRITE_INTERRUPT(priv, 0);
    memset(MBOX_RX_MSSG_ADDR_START(priv), 0, CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM);

    CLEAR_READ_INTERRUPT(priv, BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM) >> 2));
    CLEAR_WRITE_INTERRUPT(priv, BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM) >> 2));

    /* mask interrupt default, patch for mbox */
    MASK_WRITE_INTERRUPT(priv, ((1 << 0) | (1 << 5) | (1 << 6) | (1 << 11)));

    csi_irq_attach(mbox->dev.irq_num, &ck_mailbox_irqhandler, &mbox->dev);
    csi_irq_enable(mbox->dev.irq_num);
    mbox->priv = priv;
    return priv;
}

csi_error_t csi_mbox_init(csi_mbox_t *mbox, uint32_t idx)
{
    csi_mailbox_initialize(mbox);
    return CSI_OK;
}

csi_error_t csi_mbox_attach_callback(csi_mbox_t *mbox, void *callback, void *arg)
{
    mbox->callback = callback;
    mbox->arg = arg;
    return CSI_OK;
}

void csi_mbox_detach_callback(csi_mbox_t *mbox)
{
    mbox->callback = NULL;
    mbox->arg = NULL;
}

/**
  \brief       De-initialize Mailbox Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  mailbox handle to operate.
  \return      error code
*/
int32_t csi_mailbox_uninitialize(mailbox_handle_t handle)
{
    ck_mailbox_priv_t *priv = handle;

    MASK_WRITE_INTERRUPT(priv, 0);
    MASK_READ_INTERRUPT(priv, 0);
    CLEAR_READ_INTERRUPT(priv, BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM) >> 2));
    CLEAR_WRITE_INTERRUPT(priv, BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM) >> 2));

    priv->cb_event   = NULL;

    return 0;
}

void csi_mbox_uninit(csi_mbox_t *mbox)
{
    csi_mailbox_uninitialize((mailbox_handle_t)mbox->priv);

    csi_irq_detach(mbox->dev.irq_num);
    csi_irq_disable(mbox->dev.irq_num);
}

static void mailbox_write(uint32_t *dst, uint32_t *src, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

/**
  \brief       Start sending data to MAILBOX transmitter,(received data is ignored).
               This function is non-blocking,\ref mailbox_event_e is signaled when operation completes or error happens.
               \ref csi_mailbox_get_status can get operation status.
  \param[in]   handle  mailbox handle to operate.
  \param[in]   mailbox_id index of dest mailbox
  \param[in]   data  Pointer to buffer with data to send to MAILBOX transmitter.
  \param[in]   num Number of data items to send
  \return      sent number of data
*/
static int mb_chid_to_cpuid(int ch_id)
{
    switch(soc_get_cpu_id()) {
        case 0:
            return ch_id + 1;
        case 1:
            if (ch_id == 1) {
                return ch_id + 1;
            }
            return ch_id;
        case 2:
            return ch_id;
        default:
            break;
    }

    return 0;
}

int32_t csi_mbox_send(csi_mbox_t *mbox, uint32_t channel_id, const void *data, uint32_t size)
{
    int32_t mailbox_id = mb_chid_to_cpuid(channel_id);
    if (size > 20) {
        return -1;
    }
    return csi_mailbox_send((mailbox_handle_t)mbox->priv, mailbox_id, data, size);
}

#define MCC0_BASE                  (0x32000000UL)
#define MCC1_BASE                  (0x8A000000UL)

int32_t csi_mailbox_send(mailbox_handle_t handle, int32_t mailbox_id, const void *data, uint32_t num)
{
    if (num == 0) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    ck_mbox_t mbox          = {0};
    ck_mailbox_priv_t *priv = handle;

    uint32_t base;
    if (mailbox_id == 0 || mailbox_id == 1) {
        base = MCC0_BASE;
    } else {
        base = MCC1_BASE;
    }

    int32_t src_channel_idx, dst_channel_idx;
    int ret = ck_mailbox_get_channel_idx(priv->src_idx, mailbox_id, &src_channel_idx, &dst_channel_idx);

    if (ret < 0) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    mbox.base            = priv->base;
    mbox.src_idx         = priv->src_idx;
    mbox.dst_idx         = mailbox_id;
    mbox.dst_base        = base;
    mbox.src_channel_idx = src_channel_idx;
    mbox.dst_channel_idx = dst_channel_idx;

    uint32_t tx_len = (num > CSKY_MBOX_SEND_MAX_MESSAGE_LENGTH) ? CSKY_MBOX_SEND_MAX_MESSAGE_LENGTH : num;
    uint32_t dst = (uint32_t)MBOX_TX_MSSG_ADDR((&mbox));
    int32_t count;
    uint32_t buf[(CSKY_MBOX_SEND_MAX_MESSAGE_LENGTH + 3) / 4] = {0};

    /* the data address may be jishu */
    memcpy(&buf, data, tx_len);

    for (count = (tx_len + 3) / 4; count > 0; count--) {
        /* reverse write for mailbox bug */
        mailbox_write((uint32_t *)(dst + (count << 2) - 4), &buf[count - 1], 1);
    }

    return tx_len;
}

/**
  \brief       Start Receiving data from Mailbox receiver.
  \param[in]   handle  mailbox handle to operate.
  \param[in]   mailbox_id index of dest mailbox
  \param[out]  data  Pointer to buffer with data to receive from mailbox.
  \param[in]   num   Number of data items to receive
  \return      received number or  error code
*/
int32_t csi_mbox_receive(csi_mbox_t *mbox, uint32_t channel_id, void *data, uint32_t size)
{
    int32_t mailbox_id = mb_chid_to_cpuid(channel_id);
    return csi_mailbox_receive((mailbox_handle_t)mbox->priv, mailbox_id, data, size);
}

int32_t csi_mailbox_receive(mailbox_handle_t handle, int32_t mailbox_id, void *data, uint32_t num)
{
    ck_mbox_t mbox          = {0};
    ck_mailbox_priv_t *priv = handle;

    if (num < priv->receive_len) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    uint32_t base = 0;
    if (mailbox_id == 0 || mailbox_id == 1) {
        base = MCC0_BASE;
    } else {
        base = MCC1_BASE;
    }

    int32_t src_channel_idx, dst_channel_idx;
    int ret = ck_mailbox_get_channel_idx(priv->src_idx, mailbox_id, &src_channel_idx, &dst_channel_idx);

    if (ret < 0) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    mbox.base            = priv->base;
    mbox.src_idx         = priv->src_idx;
    mbox.dst_idx         = mailbox_id;
    mbox.dst_base        = base;
    mbox.src_channel_idx = src_channel_idx;
    mbox.dst_channel_idx = dst_channel_idx;
    memcpy(data, MBOX_RX_MSSG_ADDR((&mbox)), priv->receive_len);

    /* send ack to the sender */
    uint32_t *dst = (uint32_t *)MBOX_TX_ACK_ADDR((&mbox));
    putreg32(MBOX_ACK, dst);
    return priv->receive_len;
}
