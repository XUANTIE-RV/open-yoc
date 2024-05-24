/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 */

/******************************************************************************
 * @file     spinand_vendor.h
 * @brief
 * @version
 * @date     2020-03-26
 ******************************************************************************/

#ifndef _SPIFLASH_VENDOR_H_
#define _SPIFLASH_VENDOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define  SPINAND_HAS_QE_EN           (1<<0)
/* feature register */
#define REG_BLOCK_LOCK		         (0xa0)
#define BL_ALL_UNLOCKED		         (0x00)

/* configuration register */
#define REG_CFG			             (0xb0)
#define CFG_OTP_ENABLE		         (1<<6)
#define CFG_ECC_ENABLE		         (1<<4)
#define CFG_QUAD_ENABLE		         (1<<0)

/* status register */
#define REG_STATUS		             (0xc0)
#define STATUS_BUSY		             (1<<0)
#define STATUS_ERASE_FAILED	         (1<<2)
#define STATUS_PROG_FAILED	         (1<<3)
#define STATUS_ECC_MASK		         (0x3<<4)
#define STATUS_ECC_NO_BITFLIPS	     (0<<4)
#define STATUS_ECC_HAS_BITFLIPS	     (1<<4)
#define STATUS_ECC_UNCOR_ERROR	     (2<<4)
#define STATUS_ECC_UNCOR_ERROR1      (3<<4)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define NAND_MEMORG(bpc, ps, os, ppe, epl, mbb, ppl, lpt, nt)	\
	{							\
		.bits_per_cell = (bpc),				\
		.pagesize = (ps),				\
		.oobsize = (os),				\
		.pages_per_eraseblock = (ppe),			\
		.eraseblocks_per_lun = (epl),			\
		.max_bad_eraseblocks_per_lun = (mbb),		\
		.planes_per_lun = (ppl),			\
		.luns_per_target = (lpt),			\
		.ntargets = (nt),				\
	}


#define NAND_ECCREQ(str, stp) { .strength = (str), .step_size = (stp) }


#define SPINAND_INFO(__model, __id, __memorg, __eccreq,	\
		     __flags, ...)					\
	{								\
		.model = __model,					\
		.devid = __id,						\
		.memorg = __memorg,					\
		.eccreq = __eccreq,					\
		.flags  = __flags,                  \
		__VA_ARGS__						\
	}


#define SPINAND_ID(...)					\
	{								\
		.id = (const uint8_t[]){ __VA_ARGS__ },			\
		.len = sizeof((uint8_t[]){ __VA_ARGS__ }),			\
	}

#define SPINAND_SELECT_TARGET(__func)					\
	.select_target = __func


#define SPINAND_CHECK_ECC_STATUS(__func)					\
	.check_ecc_status = __func
/*************************SPI-NAND CMD ***************************/
#define SPI_MEM_OP(__cmd, __addr, __dummy, __data)		\
	{							\
		.cmd = __cmd,					\
		.addr = __addr,					\
		.dummy = __dummy,				\
		.data = __data,					\
	}

#define CVI_SPI_MEM_OP_CMD(__opcode, __buswidth)			\
	{							\
		.buswidth = __buswidth,				\
		.opcode = __opcode,				\
	}

#define CVI_SPI_MEM_OP_ADDR(__nbytes, __val, __buswidth)		\
	{							\
		.nbytes = __nbytes,				\
		.val = __val,					\
		.buswidth = __buswidth,				\
	}

#define CVI_SPI_MEM_OP_NO_ADDR	{ }

#define CVI_SPI_MEM_OP_DUMMY(__nbytes, __buswidth)			\
	{							\
		.nbytes = __nbytes,				\
		.buswidth = __buswidth,				\
	}

#define CVI_SPI_MEM_OP_NO_DUMMY	{ }

#define CVI_SPI_MEM_OP_DATA_IN(__nbytes, __buf, __buswidth)		\
	{							\
		.dir = SPI_MEM_DATA_IN,				\
		.nbytes = __nbytes,				\
		.buf.in = __buf,				\
		.buswidth = __buswidth,				\
	}

#define CVI_SPI_MEM_OP_DATA_OUT(__nbytes, __buf, __buswidth)	\
	{							\
		.dir = SPI_MEM_DATA_OUT,			\
		.nbytes = __nbytes,				\
		.buf.out = __buf,				\
		.buswidth = __buswidth,				\
	}

#define SPI_MEM_OP_NO_DATA	{ }


#define SPINAND_RESET_OP						\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0xff, 1),				\
		   CVI_SPI_MEM_OP_NO_ADDR,					\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_WR_EN_DIS_OP(enable)					\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD((enable) ? 0x06 : 0x04, 1),		\
		   CVI_SPI_MEM_OP_NO_ADDR,					\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_READID_OP(naddr, ndummy, buf, len)			\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x9f, 1),				\
		   CVI_SPI_MEM_OP_ADDR(naddr, 0, 1),			\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 1))

#define SPINAND_SET_FEATURE_OP(reg, valptr)				\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x1f, 1),				\
		   CVI_SPI_MEM_OP_ADDR(1, reg, 1),				\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   CVI_SPI_MEM_OP_DATA_OUT(1, valptr, 1))

#define SPINAND_GET_FEATURE_OP(reg, valptr)				\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x0f, 1),				\
		   CVI_SPI_MEM_OP_ADDR(1, reg, 1),				\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   CVI_SPI_MEM_OP_DATA_IN(1, valptr, 1))

#define SPINAND_BLK_ERASE_OP(addr)					\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0xd8, 1),				\
		   CVI_SPI_MEM_OP_ADDR(3, addr, 1),				\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_PAGE_READ_OP(addr)					\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x13, 1),				\
		   CVI_SPI_MEM_OP_ADDR(3, addr, 1),				\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_PAGE_READ_FROM_CACHE_OP(fast, addr, ndummy, buf, len)	\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(fast ? 0x0b : 0x03, 1),		\
		   CVI_SPI_MEM_OP_ADDR(2, addr, 1),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 1))

#define SPINAND_PAGE_READ_FROM_CACHE_OP_3A(fast, addr, ndummy, buf, len) \
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(fast ? 0x0b : 0x03, 1),		\
		   CVI_SPI_MEM_OP_ADDR(3, addr, 1),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 1))

#define SPINAND_PAGE_READ_FROM_CACHE_X2_OP(addr, ndummy, buf, len)	\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x3b, 1),				\
		   CVI_SPI_MEM_OP_ADDR(2, addr, 1),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 2))

#define SPINAND_PAGE_READ_FROM_CACHE_X2_OP_3A(addr, ndummy, buf, len)	\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x3b, 1),				\
		   CVI_SPI_MEM_OP_ADDR(3, addr, 1),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 2))

#define SPINAND_PAGE_READ_FROM_CACHE_X4_OP(addr, ndummy, buf, len)	\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x6b, 1),				\
		   CVI_SPI_MEM_OP_ADDR(2, addr, 1),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 4))

#define SPINAND_PAGE_READ_FROM_CACHE_X4_OP_3A(addr, ndummy, buf, len)	\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x6b, 1),				\
		   CVI_SPI_MEM_OP_ADDR(3, addr, 1),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 4))

#define SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(addr, ndummy, buf, len)	\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0xbb, 1),				\
		   CVI_SPI_MEM_OP_ADDR(2, addr, 2),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 2),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 2))

#define SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP_3A(addr, ndummy, buf, len) \
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0xbb, 1),				\
		   CVI_SPI_MEM_OP_ADDR(3, addr, 2),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 2),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 2))

#define SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(addr, ndummy, buf, len)	\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0xeb, 1),				\
		   CVI_SPI_MEM_OP_ADDR(2, addr, 4),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 4),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 4))

#define SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP_3A(addr, ndummy, buf, len) \
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0xeb, 1),				\
		   CVI_SPI_MEM_OP_ADDR(3, addr, 4),				\
		   CVI_SPI_MEM_OP_DUMMY(ndummy, 4),				\
		   CVI_SPI_MEM_OP_DATA_IN(len, buf, 4))

#define SPINAND_PROG_EXEC_OP(addr)					\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(0x10, 1),				\
		   CVI_SPI_MEM_OP_ADDR(3, addr, 1),				\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_PROG_LOAD(reset, addr, buf, len)			\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(reset ? 0x02 : 0x84, 1),		\
		   CVI_SPI_MEM_OP_ADDR(2, addr, 1),				\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   CVI_SPI_MEM_OP_DATA_OUT(len, buf, 1))

#define SPINAND_PROG_LOAD_X4(reset, addr, buf, len)			\
	SPI_MEM_OP(CVI_SPI_MEM_OP_CMD(reset ? 0x32 : 0x34, 1),		\
		   CVI_SPI_MEM_OP_ADDR(2, addr, 1),				\
		   CVI_SPI_MEM_OP_NO_DUMMY,					\
		   CVI_SPI_MEM_OP_DATA_OUT(len, buf, 4))


csi_error_t csi_spinand_match_vendor(csi_spinand_t *spinand, uint8_t *id_array);


#ifdef __cplusplus
}
#endif

#endif  /* _SPIFLASH_VENDOR_H_*/
