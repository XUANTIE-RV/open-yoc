/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     pinmux.h
 * @brief    Header file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#ifndef _PINMUX_H_
#define _PINMUX_H_

#include <stdint.h>
#include <soc.h>
#include "pin_name.h"

#ifdef __cplusplus
extern "C" {
#endif

void ioreuse_initial(void);
int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);

/******************************************************************************
 * hobbit gpio control and gpio reuse function
 * selecting regester adddress
 ******************************************************************************/
#define YUNVOIC_PADIO_SEL0         (CSKY_PADREG_BASE + 0x00)
#define YUNVOIC_PADIO_SEL1         (CSKY_PADREG_BASE + 0x04)
#define YUNVOIC_PADIO_AFSEL_0L     (CSKY_PADREG_BASE + 0x08)
#define YUNVOIC_PADIO_AFSEL_0H     (CSKY_PADREG_BASE + 0x0C)
#define YUNVOIC_PADIO_AFSEL_1L     (CSKY_PADREG_BASE + 0x10)
#define YUNVOIC_PADIO_AFSEL_1H     (CSKY_PADREG_BASE + 0x14)
#define YUNVOIC_PADIO_IO_PA_PE     (CSKY_PADREG_BASE + 0x18)
#define YUNVOIC_PADIO_IO_PB_PE     (CSKY_PADREG_BASE + 0x1C)
#define YUNVOIC_PADIO_IO_MOD_SEL_PE     (CSKY_PADREG_BASE + 0x20)
#define YUNVOIC_PADIO_IO_FMC_DATA_PE    (CSKY_PADREG_BASE + 0x24)
#define YUNVOIC_PADIO_IO_MCURST_PE      (CSKY_PADREG_BASE + 0x28)
#define YUNVOIC_PADIO_IO_PA_DS     (CSKY_PADREG_BASE + 0x2c)
#define YUNVOIC_PADIO_IO_PB_DS     (CSKY_PADREG_BASE + 0x30)
#define YUNVOIC_PADIO_IO_FMC_ADDR_DS (CSKY_PADREG_BASE + 0x34)
#define YUNVOIC_PADIO_IO_FMC_DATA_DS (CSKY_PADREG_BASE + 0x38)
#define YUNVOIC_PADIO_IO_MEM_DS4     (CSKY_PADREG_BASE + 0x3c)

#ifdef __cplusplus
}
#endif

#endif /*_PINMUX_H_ */

