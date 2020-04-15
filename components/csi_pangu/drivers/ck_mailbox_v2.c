/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     ck_mailbox_v2.c
 * @brief    CSI Source File for mailbox Driver
 * @version  V1.0
 * @date     06. Mar 2019
 ******************************************************************************/
#include <csi_config.h>
#include <stdbool.h>
#include <string.h>
#include <drv/irq.h>
#include <drv/mailbox.h>
#include <ck_mailbox_v2.h>
#include <soc.h>
#include <csi_core.h>

#define ERR_MAILBOX(errno) (CSI_DRV_ERRNO_MAILBOX_BASE | errno)
#define MAILBOX_NULL_PARAM_CHK(para)    HANDLE_PARAM_CHK(para, ERR_MAILBOX(DRV_ERROR_PARAMETER))

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
} ck_mailbox_priv_t;

extern int32_t drv_get_cpu_id(void);
extern int32_t target_mailbox_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);

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
void ck_mailbox_irqhandler(int32_t idx)
{
    ck_mbox_t mbox = {0};
    ck_mailbox_priv_t *priv;

    priv = &mailbox_instance[idx];

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

/**
  \brief       Initialize MAILBOX Interface. 1. Initializes the resources needed for the MAILBOX interface 2.registers event callback function
  \param[in]   cb_event  event call back function \ref mailbox_event_cb_t
  \return      return mailbox handle if success
*/
mailbox_handle_t csi_mailbox_initialize(mailbox_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;
    int32_t src_idx;
    src_idx = drv_get_cpu_id();

    int32_t ret = target_mailbox_init(src_idx, &base, &irq, &handler);

    if (ret < 0 || ret >= CONFIG_MAILBOX_NUM) {
        return NULL;
    }

    ck_mailbox_priv_t *priv = &mailbox_instance[src_idx];
    priv->base              = base;
    priv->irq               = irq;
    priv->cb_event          = cb_event;
    priv->src_idx           = src_idx;

    MASK_READ_INTERRUPT(priv, 0);
    /* invalid interrupt may be come, mask it */
    MASK_WRITE_INTERRUPT(priv, 0);
    memset(MBOX_RX_MSSG_ADDR_START(priv), 0, CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM);

    CLEAR_READ_INTERRUPT(priv, BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM) >> 2));
    CLEAR_WRITE_INTERRUPT(priv, BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM) >> 2));

    /* mask interrupt default, patch for mbox */
    MASK_WRITE_INTERRUPT(priv, ((1 << 0) | (1 << 5) | (1 << 6) | (1 << 11)));

    drv_irq_register(priv->irq, handler);
    drv_irq_enable(priv->irq);

    return priv;
}

/**
  \brief       De-initialize Mailbox Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  mailbox handle to operate.
  \return      error code
*/
int32_t csi_mailbox_uninitialize(mailbox_handle_t handle)
{
    MAILBOX_NULL_PARAM_CHK(handle);

    ck_mailbox_priv_t *priv = handle;

    MASK_WRITE_INTERRUPT(priv, 0);
    MASK_READ_INTERRUPT(priv, 0);
    CLEAR_READ_INTERRUPT(priv, BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM) >> 2));
    CLEAR_WRITE_INTERRUPT(priv, BIT_MASK((CSKY_MBOX_MAX_MESSAGE_LENGTH * CONFIG_MAILBOX_CHANNEL_NUM) >> 2));

    drv_irq_disable(priv->irq);
    drv_irq_unregister(priv->irq);
    priv->cb_event   = NULL;

    return 0;
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
int32_t csi_mailbox_send(mailbox_handle_t handle, int32_t mailbox_id, const void *data, uint32_t num)
{
    MAILBOX_NULL_PARAM_CHK(handle);
    MAILBOX_NULL_PARAM_CHK(data);

    if (num == 0) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    ck_mbox_t mbox          = {0};
    ck_mailbox_priv_t *priv = handle;

    uint32_t base;
    int32_t ret = target_mailbox_init(mailbox_id, &base, NULL, NULL);

    if (ret < 0 || ret >= CONFIG_MAILBOX_NUM) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    int32_t src_channel_idx, dst_channel_idx;
    ret = ck_mailbox_get_channel_idx(priv->src_idx, mailbox_id, &src_channel_idx, &dst_channel_idx);

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
int32_t csi_mailbox_receive(mailbox_handle_t handle, int32_t mailbox_id, void *data, uint32_t num)
{
    MAILBOX_NULL_PARAM_CHK(handle);
    MAILBOX_NULL_PARAM_CHK(data);

    ck_mbox_t mbox          = {0};
    ck_mailbox_priv_t *priv = handle;

    if (num < priv->receive_len) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    uint32_t base;
    int32_t ret = target_mailbox_init(mailbox_id, &base, NULL, NULL);

    if (ret < 0 || ret >= CONFIG_MAILBOX_NUM) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    int32_t src_channel_idx, dst_channel_idx;
    ret = ck_mailbox_get_channel_idx(priv->src_idx, mailbox_id, &src_channel_idx, &dst_channel_idx);

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
