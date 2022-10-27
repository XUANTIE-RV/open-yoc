#ifndef __BL_IPC_H__
#define __BL_IPC_H__

#include <stdint.h>
typedef void(ipcIntCallback)(uint32_t src);
void ipc_m0_init(ipcIntCallback *onLPTriggerCallBack, ipcIntCallback *onD0TriggerCallBack);

#endif
