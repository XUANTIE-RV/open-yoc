/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     silan_cxcmb.c
 * @brief    CSI Source File for silan cxc Driver
 * @version  V1.0
 * @date     20. Mar 2019
 ******************************************************************************/
#include <string.h>
#include <stdbool.h>
#include <drv/mailbox.h>

#include <silan_pic.h>
#include <silan_cxc_regs.h>
#include <silan_syscfg.h>

#define EN_INT_RECV_SENDACK 1
#define EN_INT_SEND_SENDACK 0
#define EN_INT_MODE 1

typedef enum {
    SILAN_CXC_DIR_M0toDSP = 0,
    SILAN_CXC_DIR_DSPtoM0,
    SILAN_CXC_DIR_M0toRISC,
    SILAN_CXC_DIR_RISCtoM0,
    SILAN_CXC_DIR_RISCtoDSP, /* 803 -> HiFi3 */
    SILAN_CXC_DIR_DSPtoRISC, /* HiFi3 -> 803 */

    SILAN_CXC_DIR_RISCtoDSP_DSP_SENDACK = 16,  /* 803 -> HiFi3 */
    SILAN_CXC_DIR_DSPtoRISC_RISC_SENDACK, /* HiFi3 -> 803 */

    SILAN_CXC_DIR_LMT,
} silan_cxc_dir_t;

#define CXC_MB_RISCtoDSP_DATA (0xa)
#define CXC_MB_RISCtoDSP_SIZE (0xb)

#define CXC_MB_DSPtoRISC_DATA (0x1)
#define CXC_MB_DSPtoRISC_SIZE (0x2)

#define ERR_MAILBOX(errno) (CSI_DRV_ERRNO_MAILBOX_BASE | errno)
#define MAILBOX_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_MAILBOX(DRV_ERROR_PARAMETER))

#define MAX_MAILBOX_BUF_SIZE 64

typedef struct {
    int32_t idx;
    //uint32_t base;
    uint32_t           irq;
    silan_cxc_dir_t    dir_recv;
    silan_cxc_dir_t    dir_send;
#if EN_INT_RECV_SENDACK
    silan_cxc_dir_t    dir_recv_sendack;
#endif
#if EN_INT_SEND_SENDACK
    silan_cxc_dir_t    dir_send_sendack;
#endif
    uint16_t           mbid_recv_data;
    uint16_t           mbid_send_data;
    uint16_t           mbid_recv_size;
    uint16_t           mbid_send_size;
    uint8_t            mailbox_buf[MAX_MAILBOX_BUF_SIZE] __attribute__((aligned(64)));
    mailbox_event_cb_t cb_event; ///< Event callback
} ck_mailbox_priv_t;

extern int32_t drv_get_cpu_id(void);

#define CONFIG_MAILBOX_NUM 1
static ck_mailbox_priv_t mailbox_instance[CONFIG_MAILBOX_NUM];

/**
  \brief       the interrupt service function.
  \param[in]   index of mailbox instance.
*/
#if EN_INT_MODE
static void cxc_mailbox_irqhandler(int32_t idx)
{
    ck_mailbox_priv_t *mailbox_priv = &mailbox_instance[0];

    if (mailbox_priv->irq != PIC_IRQID_CXC) {
        return;
    }

    int32_t sts = CXC_INT_STATUS;
#if EN_INT_RECV_SENDACK

    if (sts & (1 << mailbox_priv->dir_recv_sendack)) {
        /* 处理接收回调 */
        if (mailbox_priv->cb_event) {
            mailbox_priv->cb_event(mailbox_priv, 1, 0, MAILBOX_EVENT_SEND_COMPLETE);
        }

        /* 清中断 */
        CXC_RAW_INT_CLR |= (1 << mailbox_priv->dir_recv_sendack);
    }

#endif

    if (sts & (1 << mailbox_priv->dir_recv)) {
        /* 处理接收回调 */
        if (mailbox_priv->cb_event) {
            mailbox_priv->cb_event(mailbox_priv, 1, (int)CXC_MBOX(mailbox_priv->mbid_recv_size), MAILBOX_EVENT_RECEIVED);
        }

        /* 清中断 */
        CXC_RAW_INT_CLR |= (1 << mailbox_priv->dir_recv);
    }
}
#endif

/**
  \brief       Initialize MAILBOX Interface. 1. Initializes the resources needed for the MAILBOX interface 2.registers event callback function
  \param[in]   dst_idx mailbox 0:AP(RISC) 1:CP(DSP)
  \param[in]   cb_event  Pointer to \ref mailbox_event_cb_t
  \return      return mailbox handle if success
*/
mailbox_handle_t csi_mailbox_initialize(mailbox_event_cb_t cb_event)
{
    ck_mailbox_priv_t *mailbox_priv = &mailbox_instance[0];

    int32_t self_idx = drv_get_cpu_id();

    //reinit check
    if (mailbox_priv->irq == PIC_IRQID_CXC) {
        return NULL;
    }

    //mailbox_priv->base = base;
    mailbox_priv->irq      = PIC_IRQID_CXC;
    mailbox_priv->cb_event = cb_event;
    mailbox_priv->idx      = self_idx;

    mailbox_priv->dir_recv       = self_idx ? SILAN_CXC_DIR_RISCtoDSP : SILAN_CXC_DIR_DSPtoRISC;
    mailbox_priv->dir_send       = self_idx ? SILAN_CXC_DIR_DSPtoRISC : SILAN_CXC_DIR_RISCtoDSP;
    mailbox_priv->mbid_recv_data = self_idx ? CXC_MB_RISCtoDSP_DATA : CXC_MB_DSPtoRISC_DATA;
    mailbox_priv->mbid_send_data = self_idx ? CXC_MB_DSPtoRISC_DATA : CXC_MB_RISCtoDSP_DATA;
    mailbox_priv->mbid_recv_size = self_idx ? CXC_MB_RISCtoDSP_SIZE : CXC_MB_DSPtoRISC_SIZE;
    mailbox_priv->mbid_send_size = self_idx ? CXC_MB_DSPtoRISC_SIZE : CXC_MB_RISCtoDSP_SIZE;

#if EN_INT_RECV_SENDACK
    mailbox_priv->dir_recv_sendack =
        self_idx ? SILAN_CXC_DIR_RISCtoDSP_DSP_SENDACK : SILAN_CXC_DIR_DSPtoRISC_RISC_SENDACK;
#endif

#if EN_INT_SEND_SENDACK
    mailbox_priv->dir_send_sendack =
        self_idx ? SILAN_CXC_DIR_DSPtoRISC_RISC_SENDACK : SILAN_CXC_DIR_RISCtoDSP_DSP_SENDACK;
#endif

    CXC_MBOX(mailbox_priv->mbid_recv_data) = 0;
    CXC_MBOX(mailbox_priv->mbid_recv_size) = 0;

#if EN_INT_MODE
    silan_pic_request(PIC_IRQID_CXC, 0, (hdl_t)cxc_mailbox_irqhandler);

    /* 开中断 */
    CXC_INT_MASK &= ~(1 << mailbox_priv->dir_recv);
#if EN_INT_RECV_SENDACK
    CXC_INT_MASK &= ~(1 << mailbox_priv->dir_recv_sendack);
#endif

#endif

    return mailbox_priv;
}

/**
  \brief       De-initialize Mailbox Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  mailbox handle to operate.
  \return      error code
*/
int32_t csi_mailbox_uninitialize(mailbox_handle_t handle)
{
    MAILBOX_NULL_PARAM_CHK(handle);

#if EN_INT_MODE
    ck_mailbox_priv_t *mailbox_priv = handle;

    silan_pic_free(mailbox_priv->irq, 0);
    mailbox_priv->cb_event = NULL;
#endif
    return 0;
}

/**
  \brief       Start sending data to Mailbox transmitter,(received data is ignored).
               The function is non-blocking,UART_EVENT_TRANSFER_COMPLETE is signaled when transfer completes.
               drv_mailbox_get_status can indicates if transmission is still in progress or pending
  \param[in]   handle  mailbox handle to operate.
  \param[in]   data  Pointer to buffer with data to send to UART transmitter.
  \param[in]   num   Number of data items to send
  \return      sent number of data
*/
int32_t csi_mailbox_send(mailbox_handle_t handle, int32_t mailbox_id, const void *data, uint32_t num)
{
    MAILBOX_NULL_PARAM_CHK(handle);
    MAILBOX_NULL_PARAM_CHK(data);

    if (num == 0 || num > MAX_MAILBOX_BUF_SIZE) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    ck_mailbox_priv_t *mailbox_priv = handle;

    /* 忙判断 */
    if (CXC_RAW_INT_STATUS & (1 << mailbox_priv->dir_send)) {
        return ERR_MAILBOX(DRV_ERROR_BUSY);
    }

    /* 判断数据是否被使用了 */
    void *chk_data = (void *)CXC_MBOX(mailbox_priv->mbid_send_data);
    uint32_t chk_size = CXC_MBOX(mailbox_priv->mbid_send_size);

    if (chk_data != NULL || chk_size != 0) {
        return ERR_MAILBOX(DRV_ERROR_BUSY);
    }

    /* 复制邮件头 */
    memcpy(mailbox_priv->mailbox_buf, data, num);
    //dcache_writeback(mailbox_priv->mailbox_buf, num);
    csi_dcache_clean_range((uint32_t *)mailbox_priv->mailbox_buf, num);

    /* 数据指针和大小保存到寄存器 */
    CXC_MBOX(mailbox_priv->mbid_send_data) = (uint32_t)mailbox_priv->mailbox_buf;
    CXC_MBOX(mailbox_priv->mbid_send_size) = (uint32_t)num;

    /* 发中断，通知对方 */
    CXC_RAW_INT_SET |= (1 << mailbox_priv->dir_send);

    return num;
}

/**
  \brief       Start Receiving data from Mailbox receiver.
  \param[in]   handle  mailbox handle to operate.
  \param[in]   data  Pointer to buffer with data to receive from mailbox.
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t csi_mailbox_receive(mailbox_handle_t handle, int32_t mailbox_id, void *data, uint32_t num)
{
    MAILBOX_NULL_PARAM_CHK(handle);
    MAILBOX_NULL_PARAM_CHK(data);

    if (num == 0) {
        return ERR_MAILBOX(DRV_ERROR_PARAMETER);
    }

    ck_mailbox_priv_t *mailbox_priv = handle;

#if EN_INT_MODE == 0
    /* 查询模式,判断是否有数据 */
    int32_t sts = CXC_RAW_INT_STATUS;

    if (!(sts & (1 << mailbox_priv->dir_recv))) {
        return 0;
    }

#endif

    /* 数据寄存器内容判断 */
    void *recv_src  = (void *)CXC_MBOX(mailbox_priv->mbid_recv_data);
    uint32_t   recv_size = (int)CXC_MBOX(mailbox_priv->mbid_recv_size);

    //printf("rq = %x %x\n", ((uint8_t*)recv_src)[8], recv_size);

    if (recv_src == NULL || recv_size == 0 || recv_size > MAX_MAILBOX_BUF_SIZE) {
        return 0;
    }

    csi_dcache_invalid_range((uint32_t *)recv_src, recv_size);
    memcpy(data, recv_src, recv_size);

    /* 发送完成,清除寄存器,发送端也可以使用查询方式确认对方是否接收 */
    CXC_MBOX(mailbox_priv->mbid_recv_data) = 0;
    CXC_MBOX(mailbox_priv->mbid_recv_size) = 0;

#if EN_INT_MODE == 0
    /* 接收完成后，清接收中断，允许对方继续发送 */
    CXC_RAW_INT_CLR |= (1 << mailbox_priv->dir_recv);
#endif

#if EN_INT_SEND_SENDACK
    /* 发中断,通知对方,数据已经被接收 */
    CXC_RAW_INT_SET |= (1 << mailbox_priv->dir_send_sendack);
#endif

    return recv_size;
}
