/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_VDMABUF_H
#define TM_VDMABUF_H

#include <stdint.h>

/**
 *    T-Head linux virtio dma-buf mechanism is somewhat similar to ION. A device driver called VDMABUF (/dev/vdmabuf) resides in the kernel,
 *  Application in user space  apply to device a dma-buf, VDMABUF  Responsible for  export the dma-buf and returns a dma-buf FD to the user space,
 *  which transfers the FD to the another driver that needs to be imported.
 *  If working in the case of multi-core and multi-system, VDMABUF also needs to be responsible for the partner system
 *  To establish a consistent dma-buf in the same way, the user space needs to transfer the vd_id to partner system.
 */

typedef enum dma_buf_heap_type
{
    DMA_BUF_HEAP_TYPE_USER   =    0,     /* Take the address of user space as dma-buf (not supported) */
    DMA_BUF_HEAP_TYPE_SYSTEM =    1,     /* Allocated via vmalloc */
    DMA_BUF_HEAP_TYPE_SYSTEM_CONTIG = 2, /* Physically contiguous memory allocated via kmalloc */
    DMA_BUF_HEAP_TYPE_CARVEOUT = 3,      /* Memory allocated in reserved memory blocks */
    DMA_BUF_NUM_HEAPS
} dma_buf_heap_type_e;


/**
 *     allocation flags - the lower 16 bits are used by common flag,
 * the high 16 bits are used by heap type
 */
#define DMA_BUF_HEAP_FLAG_NON_CACHED 1


/**
 *   manually set the cache synchronization(not supported)
 */
#define DMA_BUF_HEAP_FLAG_CACHED_NEEDS_SYNC 2

/*
*   low 4GB memory
*/
#define DMA_BUF_HEAP_FLAG_DMA32 4

/**
 *
 * Open the VDMABUF device and return the FD of the device
 *
 *  @return Open the VDMABUF device and return the file FD of the device,
 *     If successful, return the FD of device. If failed, return - 1
 */
int vd_dev_open();

/**
 *   Turn off the VMDABUF device
 *
 * @param   vd_fd
 * @return  0  for success and - 1 for failure
 */
int vd_dev_close(int vd_fd);


/**
 *  alloc dma-buf memory from VDMABUF and get fd&vd_id of buffer
 *
 * @param vd_fd       fd of VDMABUF device
 * @param len         memory size
 * @param align       byte alignments required
 * @param heap_type   Memory type of DMA buf,reference dma_buf_heap_type_e
 * @param flags       heap flags,DMA_BUF_HEAP_FLAG_NON_CACHED|DMA_BUF_HEAP_FLAG_CACHED_NEEDS_SYNC
 * @param carveout_id   when heap_type is DMA_BUF_HEAP_TYPE_CARVEOUT,
 *                    carveout id needs to be specified,0 means default carveout
 *
 * @param dma_fd    dma-buf fd
 * @param vd_id     vd_id
 * @return 0  for success and -1 for failure
 */
int vd_dma_buf_alloc(int vd_fd, size_t len, size_t align, dma_buf_heap_type_e heap_type,
                     uint32_t flags, int carveout_id, int *dma_fd, uint64_t *vd_id);


/**
 * vd_id to fd conversion
 *
 * @param vd_fd  fd of VDMABUF device
 * @param vd_id  vd_id
 * @param dma_fd dma-buf fd
 *
 * @return: 0 for success and -1 for failure
 */
int vd_dma_buf_vd_id_to_fd(int vd_fd, int vd_id, int *dma_fd);

#define DMA_BUF_SYNC_MODE_READ      (1 << 0)
#define DMA_BUF_SYNC_MODE_WRITE     (2 << 0)
#define DMA_BUF_SYNC_MODE_RW        (DMA_BUF_SYNC_MODE_READ|DMA_BUF_SYNC_MODE_WRITE)

/**
 *  begin synchronization
 *
 *  @param vd_fd
 *  @param dma_fd
 *  @param mod  DMA_BUF_SYNC_MODE_READ,DMA_BUF_SYNC_MODE_WRITE
 * */
int dma_buf_sync_begin(int vd_fd, int dma_fd, int mode);

/**
 * end synchronization
 *
 * @param vd_fd
 * @param dma_fd
 * @param mode
 * @return
 */
int dma_buf_sync_end(int vd_fd, int dma_fd, int mode);


#endif  /* TM_VDMABUF_H */
