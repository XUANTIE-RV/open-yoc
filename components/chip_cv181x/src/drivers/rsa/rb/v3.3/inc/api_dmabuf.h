/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* api_dmabuf.h
 *
 * Management of buffers that can be shared between the host and hardware
 * devices that utilize Direct Memory Access (DMA).
 *
 * Issues to take into account for these buffers:
 * - Start address alignment
 * - Cache line sharing for buffer head and tail
 * - Cache coherence
 * - Address translation to device memory view
 */



#ifndef INCLUDE_GUARD_API_DMABUF_H
#define INCLUDE_GUARD_API_DMABUF_H

#include "basic_defs.h"


/*----------------------------------------------------------------------------
 *Function name: DMABuf_Handle_t
 * Description: This handle is a reference to a DMA buffer. It is returned when a buffer
 * is allocated or registered and it remains valid until the buffer is freed
 * or de-registered.
 *
 * The handle is set to NULL when DMABuf_Handle_t handle.p is equal to NULL.
 *
 */
// typedef struct
// {
//     void * p;
// } DMABuf_Handle_t;


/*----------------------------------------------------------------------------
 * DMABuf_Status_t
 *
 *Description: Enumeration data represents the return values for all the API functions.
 */
typedef enum
{
    DMABUF_STATUS_OK,
    DMABUF_ERROR_BAD_ARGUMENT,
    DMABUF_ERROR_INVALID_HANDLE,
    DMABUF_ERROR_OUT_OF_MEMORY
} CVI_DMABuf_Status_t;




typedef struct api_dmabuf
{
    uint32_t *p;
    uint32_t size;

} CVI_DMABuf_Handle_t;



/*----------------------------------------------------------------------------
 * DMABuf_NULLHandle
 *
 * This handle can be assigned to a variable of type DMABuf_Handle_t.
 *
 */
extern const CVI_DMABuf_Handle_t DMABuf_NULLHandle;

/*----------------------------------------------------------------------------
 * DMABuf_HostAddress_t
 *
 *Description: This pointer is used to descript that the buffer address that can be used by the host to access the buffer. 
 *             Input this address into a structure to ensure it type-safe.
 */
typedef struct 
{
    void * p;
} CVI_DMABuf_HostAddress_t;

/*----------------------------------------------------------------------------
 * DMABuf_Properties_t
 *
 * Description:When allocating a DMA buffer, these are the requested
 * properties used for that buffer. Also, when registering an externally allocated DMA buffer,
 * these propertie struct describe that buffer.
 *
 * The data structure should be initialized to all zeros for both uses.
 *
 * Example usage:
 *     DMABuf_Properties_t Props = {0};
 *     Props.fIsCached = true;
 */
typedef struct 
{
    uint32_t Size;       // size of the described DMA buffer
    uint8_t Alignment;   // start address alignment of the dma buffer, for example
                         // 4 for 32bit
    uint8_t Bank;        // This member can be used to indicate on-chip memory
    bool fCached;        // if fCached = true: SW needs to do coherency management
} CVI_DMABuf_Properties_t;



/*----------------------------------------------------------------------------
 * Function name : DMABuf_Handle_IsSame
 * Description : Check whether  the provided Handle1_p and the provided Handle2_p are equal.
 * Parameter :
 *  @Handle1_p
 *      First handle
 *
 *  @Handle2_p
 *      Second handle
 *
 * Return 
 *      true:  provided handles are equal
 *      false: provided handles are not equal
 *
 */
bool
DMABuf_Handle_IsSame(
        const CVI_DMABuf_Handle_t * const Handle1_p,
        const CVI_DMABuf_Handle_t * const Handle2_p);


/*----------------------------------------------------------------------------
 * Function name : DMABuf_Alloc
 * Description : Allocate buffer of requested size that used for device DMA.
 *     Requested properties of the buffer that will be allocated, including
 *     the size, start address alignment, etc. See above.
 *  Parameter ¡êo
 *  @Buffer_p (output)
 *     Pointer to the memory location where the address of the buffer will be
 *     written by this function when allocation is successful. This address
 *     can then be used to access the driver on the host in the domain of the
 *     driver.
 *
 *  @Handle_p (output)
 *     Pointer to the memory location when the handle will be returned.
 *
 * Return Values
 *     DMABUF_STATUS_OK: Success, Handle_p was written.
 *     DMABUF_ERROR_BAD_ARGUMENT
 *     DMABUF_ERROR_OUT_OF_MEMORY: Failed to allocate a buffer or handle.
 */
CVI_DMABuf_Status_t DMABuf_Alloc( const CVI_DMABuf_Properties_t RequestedProperties,
        CVI_DMABuf_HostAddress_t * const Buffer_p, CVI_DMABuf_Handle_t * const Handle_p);

/*----------------------------------------------------------------------------
 * Function name : DMABuf_Register
 * Description : This function must be used to register an "alien" buffer that was allocated
 * somewhere else.
 * 
 * Parameter ¡êo
 *  @ActualProperties (input)
 *     Properties that describe the buffer that is being registered.
 *  @Buffer_p (input)
 *     Pointer to the buffer. This pointer must be valid to use on the host
 *     in the domain of the driver.
 *  @Alternative_p (input)
 *     Some allocators return two addresses. This parameter can be used to
 *     pass this second address to the driver. The type is pointer to ensure
 *     it is always large enough to hold a system address, also in LP64
 *     architecture. Set to NULL if not used.
 *   @AllocatorRef (input)
 *     Number to describe the source of this buffer. The exact numbers
 *     supported is implementation specific. This provides some flexibility
 *     for a specific implementation to support a number of "alien" buffers
 *     from different allocator and properly interpret and use the
 *     Alternative_p parameter when translating the address to the device
 *     memory map. Set to zero when a default allocator is used. The type
 *     of the default allocator is implementation specific,
 *     refer to the DMABuf API Implementation Notes for details.
 *  @Handle_p (output)
 *     Pointer to the memory location when the handle will be returned.
 *
 * Return Values
 *     DMABUF_STATUS_OK: Success, Handle_p was written.
 *     DMABUF_ERROR_BAD_ARGUMENT
 *     DMABUF_ERROR_OUT_OF_MEMORY: Failed to allocate a handle.
 */
CVI_DMABuf_Status_t DMABuf_Register(const CVI_DMABuf_Properties_t ActualProperties, void * Buffer_p,
        void * Alternative_p, const char AllocatorRef, CVI_DMABuf_Handle_t * const Handle_p);


/*----------------------------------------------------------------------------
 * Function name : DMABuf_Release
 * Description : This function will close the handle that was returned by DMABuf_Alloc or
 * DMABuf_Register, meaning it must not be used anymore.
 * If the buffer was allocated through DMABuf_Alloc, this function will also
 * free the buffer, meaning it must not be accessed anymore.
 * Parameter ¡êo
 *  @Handle (input)
 *     The handle that may be released.
 *
 * Return Values
 *     DMABUF_STATUS_OK
 *     DMABUF_ERROR_INVALID_HANDLE
 */
CVI_DMABuf_Status_t DMABuf_Release(CVI_DMABuf_Handle_t Handle);


#endif /* Include Guard */

/* end of file api_dmabuf.h */

