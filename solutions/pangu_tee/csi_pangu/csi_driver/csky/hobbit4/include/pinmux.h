/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     pinmux.h
 * @brief    Header file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef HOBBIT_PINMUX_H
#define HOBBIT_PINMUX_H

#include <stdint.h>
#include "soc.h"
#include "pin_name.h"

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

#endif /* HOBBIT_PINMUX_H */

