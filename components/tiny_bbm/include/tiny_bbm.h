/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#ifndef _TINY_BBM_H__
#define _TINY_BBM_H__

#include <drv/spinand.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BBL_MAX_SIZE
#define BBL_MAX_SIZE 2048
#else
#define BBL_MAX_SIZE CONFIG_BBL_MAX_SIZE
#endif

typedef struct {
    uint32_t bbl_count;
    uint32_t bbl_offset;
    uint32_t blk_size;
    uint32_t total_blocks;
    uint32_t *bbl;
    csi_spinand_t *nand_hdl;
} nand_bbm_t;

nand_bbm_t *nand_bbm_init(csi_spinand_t *nand_hdl, csi_spinand_dev_params_t *nand_info);
int nand_bbm_mark_block_bad(nand_bbm_t *nand_bbm, uint32_t block);
int nand_bbm_block_is_bad(nand_bbm_t *nand_bbm, uint32_t block);
int nand_bbm_deinit(nand_bbm_t *nand_bbm);
int nandflash_read(nand_bbm_t *nand_bbm, off_t offset, void *buf, size_t len);
int nandflash_write(nand_bbm_t *nand_bbm, off_t offset, const void *buf, size_t len);
int nandflash_erase(nand_bbm_t *nand_bbm, off_t offset, size_t length);

#ifdef __cplusplus
}
#endif

#endif
