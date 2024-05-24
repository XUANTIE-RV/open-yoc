/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */
#include <errno.h>
#include <stdlib.h>
#include "tiny_bbm.h"

#define LOGD(mod, fmt, ...) printf("[bbm][D][%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define LOGI(mod, fmt, ...) printf("[bbm][I][%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define LOGW(mod, fmt, ...) printf("[bbm][W][%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define LOGE(mod, fmt, ...) printf("[bbm][E][%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)

#define TAG "tinybbm"

#define BBL_MAGIC_IN_FLASH 0xA1B2C3D4
#define BBL_DATA_OFFSET 1

nand_bbm_t *nand_bbm_init(csi_spinand_t *nand_hdl, csi_spinand_dev_params_t *nand_info)
{
    int i, ret;
    int offset;
    int bbl_count;
    uint32_t read_size;
    nand_bbm_t *nand_bbm = NULL;

    if (nand_info == NULL || nand_hdl == NULL) {
        LOGE(TAG, "nand_bbm_init arg e.");
        goto failure;
    }

    bbl_count = (nand_info->total_blocks >> 5) + BBL_DATA_OFFSET;
    nand_bbm = malloc(sizeof(nand_bbm_t) + (bbl_count * sizeof(uint32_t)));
    if (nand_bbm == NULL)
        goto failure;
    nand_bbm->bbl = (void *)((unsigned long)nand_bbm + sizeof(nand_bbm_t));
    nand_bbm->nand_hdl = nand_hdl;
    nand_bbm->total_blocks = nand_info->total_blocks;
    nand_bbm->blk_size = nand_info->page_size * nand_info->pages_per_block;
#if (BBL_MAX_SIZE == -1)
        offset = 0;
#else
        offset = nand_bbm->blk_size - BBL_MAX_SIZE;
#endif
    nand_bbm->bbl_offset = offset;
    nand_bbm->bbl_count = bbl_count;
    read_size = bbl_count * sizeof(uint32_t);
    memset(nand_bbm->bbl, 0xff, read_size);

    ret = csi_spinand_read(nand_hdl, offset, nand_bbm->bbl, read_size);
    if (ret < 0 || ret != read_size) {
        LOGE(TAG, "nand_bbm_init csi_spinand_read.[offset:0x%x, %d, %d]", offset, ret, read_size);
        goto failure;
    }
    LOGD(TAG, "read bbl data ok.");

    if (nand_bbm->bbl[0] != BBL_MAGIC_IN_FLASH) {
        for (i = 0; i < nand_bbm->total_blocks; i++) {
            if (csi_spinand_block_is_bad(nand_hdl, nand_bbm->blk_size * i)) {
                nand_bbm->bbl[BBL_DATA_OFFSET + (i >> 5)] &= ~(1 << (i % 32));
                LOGD(TAG, "mark block [%d] bad.", i);
            }
        }
        LOGD(TAG, "check bad finish.");
        nand_bbm->bbl[0] = BBL_MAGIC_IN_FLASH;
        ret = csi_spinand_write(nand_hdl, offset, nand_bbm->bbl, read_size);
        if (ret < 0 || ret != read_size) {
            LOGE(TAG, "nand_bbm_init csi_spinand_write.[offset:0x%x, %d, %d]", offset, ret, read_size);
            goto failure;
        }
        LOGD(TAG, "write bbl data finish.");
    } else {
        LOGD(TAG, "bbl data in flash already.");
    }
    LOGD(TAG, "nand_bbm_init ok.");
    return nand_bbm;

failure:
    LOGE(TAG, "nand_bbm_init failure.");
    if (nand_bbm)
        free(nand_bbm);
    return NULL;
}

int nand_bbm_mark_block_bad(nand_bbm_t *nand_bbm, uint32_t block)
{
    int ret;
    int write_size;

    if (nand_bbm == NULL || block > nand_bbm->total_blocks - 1)
        return -EINVAL;
    write_size = nand_bbm->bbl_count * sizeof(uint32_t);
    nand_bbm->bbl[BBL_DATA_OFFSET + (block >> 5)] &= ~(1 << (block % 32));
    ret = csi_spinand_write(nand_bbm->nand_hdl, block * nand_bbm->blk_size, nand_bbm->bbl, write_size);
    if (ret < 0 || ret != write_size) {
        LOGE(TAG, "nand_bbm_mark_block_bad csi_spinand_write error.");
        return -1;
    }
    ret = csi_spinand_block_mark_bad(nand_bbm->nand_hdl, (uint64_t)block * nand_bbm->blk_size);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_spinand_block_mark_bad error.");
        return -1;
    }
    return 0;
}

int nand_bbm_block_is_bad(nand_bbm_t *nand_bbm, uint32_t block)
{
    if (nand_bbm == NULL || block > nand_bbm->total_blocks - 1)
        return 0;
    if ((nand_bbm->bbl[BBL_DATA_OFFSET + (block >> 5)] & (1 << (block % 32))) == 0) {
        LOGD(TAG, "check block [%d] is bad.", block);
        return 1;
    }
    return 0;
}

int nand_bbm_deinit(nand_bbm_t *nand_bbm)
{
    if (nand_bbm == NULL)
        return -EINVAL;
    free(nand_bbm);
    nand_bbm = NULL;
    return 0;
}

static int _get_next_good_block(nand_bbm_t *nand_bbm, off_t offset, size_t length, off_t *out_offset)
{
    int mod;
    uint32_t blk_cnt, good_blk_cnt;
    uint32_t blk_size;
    uint64_t device_size;
    off_t tmp_offset;

    if (nand_bbm == NULL)
        return -EINVAL;

    blk_size = nand_bbm->blk_size;
    device_size = (uint64_t)blk_size * nand_bbm->total_blocks;
    blk_cnt = offset / blk_size;
    mod = offset % blk_size;

    // LOGD(TAG, "offset:0x%lx, blk_size:0x%x, blk_cnt:%d, mod:0x%x", offset, blk_size, blk_cnt, mod);
    tmp_offset = 0;
    good_blk_cnt = 0;
    while (tmp_offset + blk_size < device_size) {
        if (!nand_bbm_block_is_bad(nand_bbm, tmp_offset / blk_size)) {
            good_blk_cnt ++;
            // LOGD(TAG, "find a good block,0x%lx", tmp_offset);
        } else {
            LOGW(TAG, "cur blk is bad.offset:0x%lx", tmp_offset);
        }
        if (blk_cnt + 1 == good_blk_cnt) {
            *out_offset = tmp_offset + mod;
            if (*out_offset + length < device_size) {
                // LOGI(TAG, "find good blk offset is 0x%lx", *out_offset);
                return 0;
            }
            LOGE(TAG, "out_offset is overflow, [0x%lx, 0x%lx, 0x%lx]", *out_offset, length, device_size);
            return -1;
        }
        tmp_offset += blk_size;
    }
    LOGE(TAG, "Can't find good blk...");
    return -1;
}

static int _bad_block_range_check(nand_bbm_t *nand_bbm, off_t offset, size_t len)
{
    int start_blk, blk_count;

    start_blk = offset / nand_bbm->blk_size;
    blk_count = (len + (offset % nand_bbm->blk_size) + nand_bbm->blk_size - 1) / nand_bbm->blk_size;
    for (int i = 0; i < blk_count; i++) {
        if (nand_bbm_block_is_bad(nand_bbm, start_blk + i)) {
            return 1;
        }
    }
    return 0;
}

int nandflash_read(nand_bbm_t *nand_bbm, off_t offset, void *buf, size_t len)
{
    csi_error_t ret;
    uint32_t blk_size;
    off_t out_offset;

    if (nand_bbm == NULL)
        return -EINVAL;

    if (_bad_block_range_check(nand_bbm, offset, len) == 0) {
        ret = csi_spinand_read(nand_bbm->nand_hdl, offset, buf, len);
        if (ret != len) {
            LOGE(TAG, "spinandflash read e[%d].", ret);
            return -1;
        }
    }

    blk_size = nand_bbm->blk_size;
    if (len < blk_size) {
        if (_get_next_good_block(nand_bbm, offset, len, &out_offset)) {
            return -1;
        }
        // LOGD(TAG, "0nand read real offset:0x%lx,len:0x%lx", out_offset, len);
        ret = csi_spinand_read(nand_bbm->nand_hdl, out_offset, buf, len);
        if (ret != len) {
            LOGE(TAG, "0spinandflash read e[%d].", ret);
            return -1;
        }
	} else {
        int i;
        for (i = 0; i < len / blk_size; i++) {
            if (_get_next_good_block(nand_bbm, offset + blk_size * i, blk_size, &out_offset)) {
                return -1;
            }
            // LOGD(TAG, "1nand read real offset:0x%lx,len:0x%x", out_offset, blk_size);
            ret = csi_spinand_read(nand_bbm->nand_hdl, out_offset, (void *)((unsigned long)buf + blk_size * i), blk_size);
            if (ret != blk_size) {
                LOGE(TAG, "1spinandflash read e[%d].", ret);
                return -1;
            }
        }
        if (len % blk_size) {
            if (_get_next_good_block(nand_bbm, offset + blk_size * i, len % blk_size, &out_offset)) {
                return -1;
            }
            // LOGD(TAG, "2nand read real offset:0x%lx,len:0x%x", out_offset, (uint32_t)(len % blk_size));
            ret = csi_spinand_read(nand_bbm->nand_hdl, out_offset, (void *)((unsigned long)buf + blk_size * i), len % blk_size);
            if (ret != (len % blk_size) ) {
                LOGE(TAG, "2spinandflash read e[%d].", ret);
                return -1;
            }
		}
    }
    return 0;
}

int nandflash_write(nand_bbm_t *nand_bbm, off_t offset, const void *buf, size_t len)
{
    csi_error_t ret;
    uint32_t blk_size;
    off_t out_offset;

    if (nand_bbm == NULL)
        return -EINVAL;

    if (_bad_block_range_check(nand_bbm, offset, len) == 0) {
        ret = csi_spinand_write(nand_bbm->nand_hdl, offset, buf, len);
        if (ret != len) {
            LOGE(TAG, "spinandflash write e[%d].", ret);
            return -1;
        }
    }

    blk_size = nand_bbm->blk_size;
    if (len < blk_size) {
        if (_get_next_good_block(nand_bbm, offset, len, &out_offset)) {
            return -1;
        }
        // LOGD(TAG, "0nand write real offset:0x%lx,len:0x%lx", out_offset, len);
        ret = csi_spinand_write(nand_bbm->nand_hdl, out_offset, buf, len);
        if (ret != len) {
            LOGE(TAG, "spinandflash write e[%d].", ret);
            return -1;
        }
    } else {
        int i;
        for (i = 0; i < len / blk_size; i++) {
            if (_get_next_good_block(nand_bbm, offset + blk_size * i, blk_size, &out_offset)) {
                return -1;
            }
            // LOGD(TAG, "1nand write real offset:0x%lx,len:0x%x", out_offset, blk_size);
            ret = csi_spinand_write(nand_bbm->nand_hdl, out_offset, (void *)((unsigned long)buf + blk_size * i), blk_size);
            if (ret != blk_size) {
                LOGE(TAG, "1spinandflash write e[%d].", ret);
                return -1;
            }
        }
        if (len % blk_size) {
            if (_get_next_good_block(nand_bbm, offset + blk_size * i, len % blk_size, &out_offset)) {
                return -1;
            }
            // LOGD(TAG, "2nand write real offset:0x%lx,len:0x%x", out_offset, (uint32_t)(len % blk_size));
            ret = csi_spinand_write(nand_bbm->nand_hdl, out_offset, (void *)((unsigned long)buf + blk_size * i), len % blk_size);
            if (ret != (len % blk_size)) {
                LOGE(TAG, "2spinandflash write e[%d].", ret);
                return -1;
            }
        }
    }
    return 0;
}

int nandflash_erase(nand_bbm_t *nand_bbm, off_t offset, size_t length)
{
    uint32_t erase_size;
    uint64_t device_size;
    uint64_t last_erase_addr;

    if (nand_bbm == NULL)
        return -EINVAL;

    // LOGD(TAG, "nand flash erase, offset:0x%lx, len:%ld", offset, length);
    erase_size = nand_bbm->blk_size;
    device_size = (uint64_t)nand_bbm->blk_size * nand_bbm->total_blocks;

    if ((offset % erase_size) != 0 || (length % erase_size) != 0) {
        LOGE(TAG, "erase offset or length not times");
        return -1;
    }
    if (length == 0 || (offset + length) > device_size) {
        LOGE(TAG, "offset+length > spiflash size");
        return -1;
    }
    if (_bad_block_range_check(nand_bbm, offset, length) == 0) {
        if (csi_spinand_erase(nand_bbm->nand_hdl, offset, length, &last_erase_addr) != CSI_OK) {
            LOGE(TAG, "csi_spinand_erase erase e.");
            return -1;
        }
    }
    off_t out_offset;
    for (int i = 0; i < length / erase_size; i++) {
        if (_get_next_good_block(nand_bbm, offset + i * erase_size, erase_size, &out_offset)) {
            return -1;
        }
        // LOGD(TAG, "nand erase real offset:0x%lx,len:0x%x", out_offset, erase_size);
        if (csi_spinand_erase(nand_bbm->nand_hdl, out_offset, erase_size, &last_erase_addr) != CSI_OK) {
            LOGE(TAG, "qspinandflash erase e.");
            return -1;
        }
    }
    // LOGD(TAG, "qspi nand flash erase ok");

    return 0;
}