/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _CSI_MAILBOX_H_
#define _CSI_MAILBOX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <drv_common.h>

/// definition for mailbox handle.
typedef void *mailbox_handle_t;

typedef void (*mailbox_event_cb_t)(uint8_t *data, uint32_t len);   ///< Pointer to \ref mailbox_event_cb_t : MAILBOX Event call back.

/**
  \brief       Initialize MAILBOX Interface. 1. Initializes the resources needed for the MAILBOX interface 2.registers event callback function
  \param[in]   self_idx mailbox index
  \param[in]   cb_event  event call back function \ref mailbox_event_cb_t
  \return      return mailbox handle if success
*/
mailbox_handle_t drv_mailbox_initialize(int32_t self_idx, mailbox_event_cb_t cb_event);

/**
  \brief       De-initialize MAILBOX Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  mailbox handle to operate.
  \return      error code
*/
int32_t drv_mailbox_uninitialize(mailbox_handle_t handle);

/**
  \brief       Start sending data to MAILBOX transmitter,(received data is ignored).
               This function is non-blocking,\ref mailbox_event_e is signaled when operation completes or error happens.
               \ref csi_mailbox_get_status can get operation status.
  \param[in]   handle  mailbox handle to operate.
  \param[in]   data  Pointer to buffer with data to send to MAILBOX transmitter.
  \param[in]   num Number of data items to send
  \return      sent number of data
*/
int32_t drv_mailbox_send_mail(mailbox_handle_t handle, const void *data, uint32_t num);

#ifdef __cplusplus
}
#endif

#endif /* _CSI_MAILBOX_H_ */
