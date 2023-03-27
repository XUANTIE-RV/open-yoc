/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _UPDATE_MISC_MAN_H_
#define _UPDATE_MISC_MAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <yoc/partition.h>
#include "update_mtb.h"

#define UPDATE_CHECK_FAIL                  -1
#define UPDATE_PATCH_FAIL                  -2
#define UPDATE_DRIVER_FAIL                 -3
#define UPDATE_NOT_SUPPORT				   -4

#define MISC_FILE_HEADER_RCVY_E           0x52525245   //"ERRR" = 0x52525245
#define MISC_FILE_KEY_LEN                 (256)
#define MISC_FILE_SIG_LEN                 (128)

#define MISC_UPDATA_STATUS_CONTINUE 0x7447abba

#define MAX_FOTA_TB_SIZE    (4096)

enum {
	MISC_UPDATA_CONTROL_SET      = 1,
	MISC_UPDATA_CONTROL_OSVERS   = 2,
	MISC_UPDATA_CONTROL_REST     = 3
};

enum misc_status_type_e {
	MISC_STATUS_IMG_SET                  = 0,
	MISC_STATUS_RCVY_MANFEST_SET         = 1,
	MISC_STATUS_PRIM_MANFEST_SET         = 2,
	MISC_STATUS_FD_ADDR                  = 3
};

#define MISC_STATUS_MAGIC                    0x4774caac
#define MISC_STATUS_MAGIC_TAIL               0xd88d3553

int misc_file_check(partition_info_t *misc_info);
int misc_reset(partition_info_t *misc_info);
int misc_init(int partition_fd, partition_info_t *misc_info);
unsigned long misc_next_imager(unsigned long fd);
int misc_get_imager_info(unsigned long fd, img_info_t *img_f);
int misc_update_path(unsigned long bm_addr, img_info_t *img_update, uint32_t type, unsigned long fd);

int misc_get_update_fd(unsigned long *fd, uint32_t *status, partition_info_t *misc_info);
int misc_set_update_fd(uint32_t type, unsigned long fd, uint32_t status);

int misc_get_scn_img_info(img_info_t *img_info, scn_img_t *scn_img);

int misc_get_app_version(uint8_t *out, uint32_t *olen);
#ifdef __cplusplus
}
#endif

#endif /* _UPDATE_MISC_MAN_H_ */

