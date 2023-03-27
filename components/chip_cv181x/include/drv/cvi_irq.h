/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __CVI_IRQ_H__
#define __CVI_IRQ_H__

#include <drv/irq.h>

#ifdef __cplusplus
extern "C" {
#endif

int request_irq(unsigned int irqn, void * handler, unsigned long flags,const char *name, void *priv);

#ifdef __cplusplus
}
#endif

#endif /* __CVI_IRQ_H__ */

