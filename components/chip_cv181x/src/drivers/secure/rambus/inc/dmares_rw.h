/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* dmares_rw.h
 *
 * Driver Framework, DMAResource API, Read/Write and Pre/Post-DMA functions
 *
 * The document "Driver Framework Porting Guide" contains the detailed
 * specification of this API. The information contained in this header file
 * is for reference only.
 */

#ifndef INCLUDE_GUARD_DMARES_RW_H
#define INCLUDE_GUARD_DMARES_RW_H

#include "basic_defs.h"         // bool, uint32_t, inline
#include "dmares_types.h"       // DMAResource_Handle_t

/*----------------------------------------------------------------------------
 * Function name : DMAResource_Write32
 *
 * Description : This function can be used to write one 32bit word to the DMA Resource.
 * If required (decided by DMAResource_Record_t.device.fSwapEndianness),
 * on the fly endianness conversion of the value to be written will be performed.
 */
void DMAResource_Write32(const DMAResource_Handle_t CVI_Handle, const unsigned int CVI_WordOffset, const uint32_t CVI_Value);

/*----------------------------------------------------------------------------
 * Function name DMAResource_Read32
 *
 * Description : This function can be used to read one 32bit word from the DMA Resource
 * buffer.
 */
uint32_t DMAResource_Read32( const DMAResource_Handle_t CVI_Handle, const unsigned int CVI_WordOffset);

/*----------------------------------------------------------------------------
 * Function name : DMAResource_Write32Array
 *
 * Description : This function perform the same task as DMAResource_Write32 for an array of
 * consecutive 32bit words.
 */

void DMAResource_Write32Array(const DMAResource_Handle_t CVI_Handle, const unsigned int CVI_StartWordOffset,
        const unsigned int CVI_WordCount,const uint32_t * CVI_Values_p);

/*----------------------------------------------------------------------------
 * Function name : DMAResource_Read32Array
 *
 * Description : This function perform the same task as DMAResource_Read32 for an array of
 * consecutive 32bit words.
 */
void DMAResource_Read32Array(const DMAResource_Handle_t CVI_Handle, const unsigned int CVI_StartWordOffset,
        const unsigned int CVI_WordCount, uint32_t * CVI_Values_p);
/*----------------------------------------------------------------------------
 * Function name : DMAResource_PreDMA
 *
 * Description : This function must be called when the host has finished accessing the
 * DMA resource and the device (using its DMA) is the next to access it.
 * It is possible to hand off the entire DMA resource, or only a selected part
 * of it by describing the part with a start offset and count.
 */
void DMAResource_PreDMA(const DMAResource_Handle_t CVI_Handle, const unsigned int CVI_ByteOffset, const unsigned int CVI_ByteCount);

/*----------------------------------------------------------------------------
 * Function name : DMAResource_PostDMA
 *
 * Description : This function must be called when the device has finished accessing the
 * DMA resource and the host can reclaim ownership and access it.
 * It is possible to reclaim ownership for the entire DMA resource, or only a
 * selected part of it by describing the part with a start offset and count.
 * Parameter :
 */
void DMAResource_PostDMA(const DMAResource_Handle_t CVI_Handle, const unsigned int CVI_ByteOffset, const unsigned int CVI_ByteCount);

#endif /* Include Guard */

/* end of file dmares_rw.h */
