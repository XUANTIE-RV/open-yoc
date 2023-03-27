#ifndef __CVI_TPU_PROC_H__
#define __CVI_TPU_PROC_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "drv/list.h"


/***************************************
 * cvi_tpu_ioctl ↓
 **************************************/
struct cvi_cache_op_arg {
	uintptr_t paddr;
	unsigned long long size;
	int dma_fd;
};

struct cvi_submit_dma_arg {
	uintptr_t *addr;
	unsigned int seq_no;
};

struct cvi_wait_dma_arg {
	unsigned int seq_no;
	int ret;
};

struct cvi_pio_mode {
	unsigned long long cmdbuf;
	unsigned long long sz;
};

struct cvi_load_tee_arg {
	// normal domain
	unsigned long long cmdbuf_addr_ree;
	unsigned int cmdbuf_len_ree;
	unsigned long long weight_addr_ree;
	unsigned int weight_len_ree;
	unsigned long long neuron_addr_ree;

	// security domain
	unsigned long long dmabuf_addr_tee;
};

struct cvi_submit_tee_arg {
	unsigned long long dmabuf_tee_addr;
	unsigned long long gaddr_base2;
	unsigned long long gaddr_base3;
	unsigned long long gaddr_base4;
	unsigned long long gaddr_base5;
	unsigned long long gaddr_base6;
	unsigned long long gaddr_base7;
	unsigned int seq_no;
};

struct cvi_unload_tee_arg {
	uintptr_t addr;
	unsigned long long size;
};

struct cvi_tdma_copy_arg {
	uintptr_t paddr_src;
	uintptr_t paddr_dst;
	unsigned int h;
	unsigned int w_bytes;
	unsigned int stride_bytes_src;
	unsigned int stride_bytes_dst;
	unsigned int enable_2d;
	unsigned int leng_bytes;
	unsigned int seq_no;
};

struct cvi_tdma_wait_arg {
	unsigned int seq_no;
	int ret;
};
/***************************************
 * cvi_tpu_ioctl ↑
 **************************************/


#define CVITPU_SUBMIT_DMABUF	0x01
#define CVITPU_DMABUF_FLUSH_FD	0x02
#define CVITPU_DMABUF_INVLD_FD	0x03
#define CVITPU_DMABUF_FLUSH		0x04
#define CVITPU_DMABUF_INVLD		0x05
#define CVITPU_WAIT_DMABUF		0x06
#define CVITPU_PIO_MODE			0x07

#define CVITPU_LOAD_TEE			0x08
#define CVITPU_SUBMIT_TEE		0x09
#define CVITPU_UNLOAD_TEE		0x0A
#define CVITPU_SUBMIT_PIO		0x0B
#define CVITPU_WAIT_PIO			0x0C





#ifdef __cplusplus
}
#endif
#endif	/* __CVI_TPU_PROC_H__ */