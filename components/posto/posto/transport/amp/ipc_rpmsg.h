/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef POSTO_TRANSPORT_AMP_IPC_RPMSG_H_
#define POSTO_TRANSPORT_AMP_IPC_RPMSG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MESSAGE_ASYNC 0x00
#define AOS_WAIT_FOREVER 0xffffffffu

typedef struct {
  uint8_t flag;
  uint8_t service_id;
  uint16_t command;
  void *req_data;
  int req_len;
  void *resp_data;
  int resp_len;
} ipc_rpmsg_message_t;

typedef struct ipc_rpmsg_ipc_s ipc_rpmsg_ipc_t;

typedef void (*ipc_rpmsg_ipc_process_t)(ipc_rpmsg_ipc_t *ipc, ipc_rpmsg_message_t *msg, void *priv);

ipc_rpmsg_ipc_t *ipc_rpmsg_ipc_get(int des_cpu_id);
int ipc_rpmsg_ipc_add_service(ipc_rpmsg_ipc_t *ipc, int service_id, ipc_rpmsg_ipc_process_t cb, void *priv);
int ipc_rpmsg_ipc_message_send(ipc_rpmsg_ipc_t *ipc, ipc_rpmsg_message_t *msg, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif  // POSTO_TRANSPORT_AMP_IPC_RPMSG_H_
