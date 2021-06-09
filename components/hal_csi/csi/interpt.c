#include <aos/kernel.h>
#include "drv/irq.h"
#include "aos/hal/interpt.h"

int32_t hal_interpt_init(void)
{
  return 0;
}

int32_t hal_interpt_mask(int32_t vec)
{
    drv_irq_disable(vec);
    return 0;
}

int32_t hal_interpt_umask(int32_t vec)
{
    drv_irq_enable(vec);
    return 0;
}

int32_t hal_interpt_install(int32_t vec, hal_interpt_t handler, void *para,  char *name)
{
    drv_irq_register(vec, (void *)handler);
    return 0;
}