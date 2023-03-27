#ifndef _CVI_SNSR_I2C_H_
#define _CVI_SNSR_I2C_H_

#include "vi_snsr.h"
#include "pthread.h"
#include "k_spin_lock.h"
#include <aos/aos.h>
#include <drv/iic.h>

#define I2C_MAX_NUM		5
#define I2C_MAX_MSG_NUM		32
#define I2C_BUF_SIZE		(I2C_MAX_MSG_NUM << 2)

int vip_sys_register_cmm_cb(unsigned long cmm, void *hdlr, void *cb);

struct i2c_msg {
	__u16 addr;	/* slave address			*/
	__u16 flags;
#define I2C_M_RD		0x0001	/* read data, from slave to master */
					/* I2C_M_RD is guaranteed to be 0x0001! */
#define I2C_M_TEN		0x0010	/* this is a ten bit chip address */
#define I2C_M_DMA_SAFE		0x0200	/* the buffer of this message is DMA safe */
					/* makes only sense in kernelspace */
					/* userspace buffers are copied anyway */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
#define I2C_M_NO_RD_ACK		0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK	0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR	0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART		0x4000	/* if I2C_FUNC_NOSTART */
#define I2C_M_STOP		0x8000	/* if I2C_FUNC_PROTOCOL_MANGLING */
	__u16 len;		/* msg length				*/
	__u8 *buf;		/* pointer to msg data			*/
};

struct cvi_i2c_ctx {
	csi_iic_t		*master_iic;
	struct i2c_msg		msg[I2C_MAX_MSG_NUM];
	uint8_t			*buf;
	uint32_t		msg_idx;
	uint16_t		addr_bytes;
	uint16_t		reg_addr;
	uint16_t		data_bytes;
};

struct cvi_i2c_dev {
	kspinlock_t		lock;
	aos_mutex_t		mutex;
	struct cvi_i2c_ctx	ctx[I2C_MAX_NUM];
};

#define CVI_SNS_I2C_IOC_MAGIC	'i'
#define CVI_SNS_I2C_WRITE	0x1000
#define CVI_SNS_I2C_BURST_QUEUE	0x1001
#define CVI_SNS_I2C_BURST_FIRE	0x1002

int cvi_snsr_i2c_probe(void);
int cvi_snsr_i2c_remove(void);

#endif
