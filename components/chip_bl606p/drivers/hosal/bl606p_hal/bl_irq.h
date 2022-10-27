#ifndef __BL_IRQ_H__
#define __BL_IRQ_H__
void bl_irq_enable(unsigned int source);
void bl_irq_disable(unsigned int source);

void bl_irq_init(void);
void bl_irq_register_with_ctx(int irqnum, void *handler, void *ctx);
void bl_irq_register(int irqnum, void *handler);
void bl_irq_unregister(int irqnum, void *handler);
void bl_irq_ctx_get(int irqnum, void **ctx);

#endif
