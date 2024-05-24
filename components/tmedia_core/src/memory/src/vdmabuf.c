/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifdef __linux__
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "vdmabuf.h"
#include "unistd.h"

struct vd_allocation_data;
struct vd_id_to_fd_data;

#define VD_IOC_MAGIC   'G'

#define VD_IOC_ALLOC        _IOWR(VD_IOC_MAGIC, 2, struct vd_allocation_data)
#define VD_IOC_VD_ID_TO_FD  _IOWR(VD_IOC_MAGIC, 3, struct vd_id_to_fd_data)

struct vd_allocation_data
{
    size_t size;
    int align;
    void *uaddr;
    int fd;
    unsigned int buf_id;
    dma_buf_heap_type_e heap_type;
    int carveout_id;
    int flags;
};


struct vd_id_to_fd_data
{
    int vd_id;
    int fd;
};

struct dma_buf_sync
{
    uint64_t flags;
};

#define DMA_BUF_SYNC_START     (0 << 2)
#define DMA_BUF_SYNC_END       (1 << 2)
#define DMA_BUF_SYNC_VALID_FLAGS_MASK \
    (DMA_BUF_SYNC_MODE_RW | DMA_BUF_SYNC_END)

#define DMA_BUF_BASE        'b'
#define DMA_BUF_IOCTL_SYNC  _IOW(DMA_BUF_BASE, 0, struct dma_buf_sync)


int vd_dev_open()
{
    int fd = open("/dev/virtio-vdmabuf", O_RDWR);

    printf("dv_dev_open %s vd_dev:%d \n", "/dev/virtio-vdmabuf", fd);

    if (fd < 0)
    {
        printf("can not open device:/dev/virtio-vdmabuf\n");
        return -1;
    }


    return fd;
}

int vd_dev_close(int fd)
{
    printf("dv_dev_close fd:%d \n", fd);

    return close(fd);
}

static int ioctl_wrap(int fd, int req, void *arg)
{
    int ret = ioctl(fd, req, arg);
    if (ret < 0)
    {
        printf("ioctl fail,ret:%d\n", req);
        return -errno;
    }

    return ret;
}



/*
 * size          : user fill dmabuf size only when use heap VIRTIO_VDMABUF_TYPE_SYSTEM or
 *                 VIRTIO_VDMABUF_HEAP_TYPE_SYSTEM_CONTIG or VIRTIO_VDMABUF_HEAP_USER
 * align         : user fill dmabuf align, kernel use PAGE_SIZE as the align by default
 * uaddr         : user fill user buf addr only when use VIRTIO_VDMABUF_HEAP_USER heap
 * fd            : kernel fill dmabuf fd
 * buf_id        : kernel fill id of dmabuf
 * heap_type     : user fill heap type
 * carveout_type : user fill carveout type when heap type is VIRTIO_VDMABUF_HEAP_TYPE_CARVEOUT
 * flags         : user fill buf flags, eg, noncache(VIRTIO_VDAMBUF_NONCACHED),
 *                 kernel use cache mode by default
 */
int vd_dma_buf_alloc(int vd_fd, size_t len, size_t align, dma_buf_heap_type_e heap_type,
                     uint32_t flags, int carveout_id, int *dma_fd, uint64_t *vd_id)
{

    int ret;

    struct vd_allocation_data data =
    {
        .size = len,
        .align = align,
        .heap_type = heap_type,
        .flags = flags,
        .carveout_id = carveout_id
    };

    if (heap_type == DMA_BUF_HEAP_TYPE_USER)   //user space dma buf not support now
    {
        printf("DMA_BUF_HEAP_TYPE_USER not support now\n");
        return -1;
    }

    ret = ioctl_wrap(vd_fd, VD_IOC_ALLOC, &data);
    if (ret < 0)
    {
        printf("alloc ioctl fail\n");
        return -1;
    }

    *dma_fd  = data.fd;
    *vd_id = data.buf_id;

    return ret;
}


int vd_dma_buf_vd_id_to_fd(int vd_fd, int vd_id, int *dma_fd)
{
    int ret;

    struct vd_id_to_fd_data data =
    {
        .vd_id = vd_id,
    };

    ret = ioctl_wrap(vd_fd, VD_IOC_VD_ID_TO_FD, &data);
    if (ret < 0)
    {
        printf("vd to fd ioctl fail\n");
        return ret;
    }

    *dma_fd = data.fd;
    return ret;
}

int dma_buf_sync_begin(int vd_fd, int dma_fd, int mode)
{
    struct dma_buf_sync data =
    {
        .flags = DMA_BUF_SYNC_START | mode,
    };

    return ioctl_wrap(vd_fd, DMA_BUF_IOCTL_SYNC, &data);
}

int dma_buf_sync_end(int vd_fd, int dma_fd, int mode)
{
    struct dma_buf_sync data =
    {
        .flags = DMA_BUF_SYNC_END | mode,
    };

    return ioctl_wrap(vd_fd, DMA_BUF_IOCTL_SYNC, &data);
}
#endif


