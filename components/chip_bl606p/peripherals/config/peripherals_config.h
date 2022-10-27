/*
 * peripherals_config.h
 *
 *  Created on: 2022年3月9日
 *      Author: u20-zh
 */

#ifndef COMPONENTS_CHIP_BL606P_PERIPHERALS_CONFIG_IO_SDK_CONFIG_H_
#define COMPONENTS_CHIP_BL606P_PERIPHERALS_CONFIG_IO_SDK_CONFIG_H_

#define cpu_type_config   bl606p

#include "common/misc.h"
#if  (cpu_type_config == bl606p)
#include "bl606p_config.h"   // #include "./bl606p_code/bl606p_config.h"
#else
#endif



#endif /* COMPONENTS_CHIP_BL606P_PERIPHERALS_CONFIG_IO_SDK_CONFIG_H_ */
