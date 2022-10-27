#ifndef __BLYOC_MBOX_H_
#define __BLYOC_MBOX_H_
#include "bl_ipc.h"

#define BLYOC_IPC_INTRRUPT_FIELD_ACK_MASK (0x80)
#define BLYOC_IPC_INTRRUPT_SEND_DATA_MASK (0x01)
#define BLYOC_IPC_INTRRUPT_RECEIVED_MASK  (0x03)

int blyoc_mbox_init(csi_mbox_t *mbox, uint32_t idx);
void blyoc_mbox_uninit(csi_mbox_t *mbox);
int32_t blyoc_mbox_send(csi_mbox_t *mbox, uint32_t ui32_channel_id, const void *data, uint32_t size);
int32_t blyoc_mbox_receive(csi_mbox_t *mbox, uint32_t ui32_channel_id, void *data, uint32_t size);

#endif //> __BLYOC_MBOX_H_


