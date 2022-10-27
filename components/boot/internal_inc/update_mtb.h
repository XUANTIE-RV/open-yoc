/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __UPDATE_MTB_H__
#define __UPDATE_MTB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <mtb.h>

int update_imginfo(uint32_t bm_addr, uint32_t type, uint8_t *name, 
                    uint8_t *section, uint32_t section_len);
/**
 * 更新mtb镜像
 */
int update_img_mtb(uint32_t is_prim);

/**
 * 从FLASH更新到RAM
 */
int mtb_update_ram(mtb_t *mtb);

/**
 * 更新MTB的备份分区内容
 */
int mtb_update_backup(mtb_t *mtb, const char *img_name, mtb_partition_info_t *partition_info, int *update_flag);

/**
 * 更新MTB的主分区内容，(拷贝备份分区内容到主分区)
 */
int mtb_update_valid(mtb_t *mtb);

#ifdef __cplusplus
}
#endif

#endif /* __UPDATE_MTB_H__ */
