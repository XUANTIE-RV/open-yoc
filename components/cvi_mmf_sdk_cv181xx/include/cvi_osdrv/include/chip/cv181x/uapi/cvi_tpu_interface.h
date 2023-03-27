#ifndef __CVI_TPU_INTERFACE_H__
#define __CVI_TPU_INTERFACE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "rtos_types.h"

void cvi_tpu_init(void);
void cvi_tpu_deinit(void);
void *cvi_tpu_open(void);
int cvi_tpu_close(void);
int cvi_tpu_ioctl(void *dev, unsigned int cmd, unsigned long arg);


#ifdef __cplusplus
}
#endif
#endif // __CVI_TPU_INTERFACE_H__





