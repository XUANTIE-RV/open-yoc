/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_eip28_ext.h
 *
 * EIP-28 hardware specific EIP-28 Driver Library configuration parameters:
 * EIP-28 hardware in the FPGA host HW platform
 *
 */



#ifndef INCLUDE_GUARD_CS_EIP28_EXT_H
#define INCLUDE_GUARD_CS_EIP28_EXT_H

/* Use this option to remove the capability register check.
   This register is present in this EIP-28 hardware */
//#define EIP28_REMOVE_EIPNR_CHECK

/* Use this to remove firmware download.
   Some EIP-28 hardware versions have firmware in ROM. */
//#define EIP28_REMOVE_FIRMWARE_DOWNLOAD


#endif /* Include Guard */


/* end of file cs_eip28_ext.h */
