/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     ck_mbox.c
 * @brief    CSI Source File for mailbox Driver
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <drv/mailbox.h>
#include <drv/irq.h>

#include <soc.h>
#include <ck_mbox.h>

#define ERR_MBOX(errno) (CSI_DRV_ERRNO_MAILBOX_BASE | errno)

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef struct {
    uint32_t base;
    int32_t irq;
    mailbox_event_cb_t callback;
} ck_mbox_priv_t;

/************************************************************************************
 * Private Function Prototypes
 ************************************************************************************/

extern int32_t target_mbox_init(int32_t idx, uint32_t *base, int32_t *irq, void **handler);
extern const mailbox_chnl_config_t *target_get_mbox_chnl(void);

/************************************************************************************
 * Private Data
 ************************************************************************************/

static ck_mbox_priv_t mbox_instance[CONFIG_MBOX_NUM];

/************************************************************************************
 * Private Functions
 ************************************************************************************/

#define CK_MBOX_MODIFY(regs, clearbits, setbits)    \
do {                                                \
    uint32_t irq_state;                             \
    uint32_t regval;                                \
    irq_state   = csi_irq_save();                   \
    regval  = regs;                                 \
    regval &= ~clearbits;                           \
    regval |= setbits;                              \
    regs = regval;                                  \
    csi_irq_restore(irq_state);                     \
} while (0)

static inline int32_t ck_mbox_get_tx_chnl(uint32_t mailbox_id)
{
    const mailbox_chnl_config_t *chnl = target_get_mbox_chnl();
    uint32_t i;

    for (i = 0; i < chnl->count; i++) {
        if (chnl->config[i].mailbox_id == mailbox_id) {
            return chnl->config[i].tx_chnl;
        }
    }

    return -1;
}

static inline int32_t ck_mbox_get_rx_chnl(uint32_t mailbox_id)
{
    const mailbox_chnl_config_t *chnl = target_get_mbox_chnl();
    uint32_t i;

    for (i = 0; i < chnl->count; i++) {
        if (chnl->config[i].mailbox_id == mailbox_id) {
            return chnl->config[i].rx_chnl;
        }
    }

    return -1;
}

static inline int32_t ck_mbox_get_mailbox_id(uint32_t rx_chnl)
{
    const mailbox_chnl_config_t *chnl = target_get_mbox_chnl();
    uint32_t i;

    for (i = 0; i < chnl->count; i++) {
        if (chnl->config[i].rx_chnl == rx_chnl) {
            return chnl->config[i].mailbox_id;
        }
    }

    return -1;
}

void ck_mbox_irqhandler(int32_t idx)
{
    ck_mbox_priv_t *mbox_priv = &mbox_instance[idx];
    ck_mbox_reg_t *regs;
    int32_t i;
    uint32_t stat;

    regs = (ck_mbox_reg_t*)mbox_priv->base;

    stat = regs->INTR_STA;
    for (i = 0; stat; i++, stat >>= 1) {
        if (stat & 0x1) {
            regs->INTR_STA = 1 << i;
            if (i < CONFIG_MBOX_CHANNEL_NUM && mbox_priv->callback) {
                mbox_priv->callback(mbox_priv, ck_mbox_get_mailbox_id(i), 0, MAILBOX_EVENT_RECEIVED);
            }
        }
    }
}

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/**
  \brief       Initialize MAILBOX Interface. 1. Initializes the resources needed for the MAILBOX interface 2.registers event callback function
  \param[in]   cb_event  event call back function \ref mailbox_event_cb_t
  \return      return mailbox handle if success
*/
mailbox_handle_t csi_mailbox_initialize(mailbox_event_cb_t cb_event)
{
    ck_mbox_priv_t *mbox_priv;
    ck_mbox_reg_t *regs;
    void *handler;
    int32_t ret;
    uint32_t idx;

    for (idx = 0; idx < CONFIG_MBOX_NUM; idx++) {

        mbox_priv = &mbox_instance[idx];
        ret = target_mbox_init(idx, &mbox_priv->base, &mbox_priv->irq, &handler);
        if (ret < 0 || ret >= CONFIG_MBOX_NUM) {
            return NULL;
        }

        if (mbox_priv->irq > 0) {

            regs = (ck_mbox_reg_t*)mbox_priv->base;

            drv_irq_register(mbox_priv->irq, handler);
            drv_irq_enable(mbox_priv->irq);

            mbox_priv->callback = cb_event;

            CK_MBOX_MODIFY(regs->INTR_EN, 0, (1 << CONFIG_MBOX_INTR_EN_BIT));
        }
    }

    return (mailbox_handle_t)mbox_instance;
}

/**
  \brief       De-initialize MAILBOX Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  mailbox handle to operate.
  \return      error code
*/
int32_t csi_mailbox_uninitialize(mailbox_handle_t handle)
{
    ck_mbox_priv_t *mbox_priv;
    ck_mbox_reg_t *regs;
    uint32_t idx;

    for (idx = 0; idx < CONFIG_MBOX_NUM; idx++) {

        mbox_priv = &mbox_instance[idx];

        if (mbox_priv->irq > 0) {

            regs = (ck_mbox_reg_t*)mbox_priv->base;

            drv_irq_disable(mbox_priv->irq);
            drv_irq_unregister(mbox_priv->irq);

            regs->INTR_EN = 0;

            mbox_priv->callback = NULL;
        }
    }

    return 0;
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
    ck_mbox_priv_t *mbox_priv;
    ck_mbox_reg_t *regs;
    int32_t chnl;

    if (data) {
        return ERR_MBOX(DRV_ERROR_UNSUPPORTED);
    }

    chnl = ck_mbox_get_tx_chnl(mailbox_id);
    if (chnl < 0) {
        return ERR_MBOX(DRV_ERROR_PARAMETER);
    }

    mbox_priv = &mbox_instance[mailbox_id];
    regs = (ck_mbox_reg_t*)mbox_priv->base;

    regs->INTR_SET = chnl;

    return 0;
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
    return ERR_MBOX(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       enable mailbox ch irq
  \param[in]   cb_event  event call back function \ref mailbox_event_cb_t
  \return      return mailbox handle if success
*/
int32_t csi_mailbox_chnl_enable(mailbox_handle_t handle, int32_t mailbox_id)
{
    ck_mbox_priv_t *mbox_priv;
    ck_mbox_reg_t *regs;
    int32_t chnl, idx;

    for (idx = 0; idx < CONFIG_MBOX_NUM; idx++) {
        mbox_priv = &mbox_instance[idx];

        if (mbox_priv->irq > 0) {
            regs = (ck_mbox_reg_t*)mbox_priv->base;

            chnl = ck_mbox_get_rx_chnl(mailbox_id);
            CK_MBOX_MODIFY(regs->INTR_EN, 0, (1 << chnl));
        }
    }

    return 0;
}