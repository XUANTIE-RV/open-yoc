#ifndef MODULES_VB_IOCTL_H_
#define MODULES_VB_IOCTL_H_

#include <vb_uapi.h>
#include <cvi_base.h>
#include "cvi_comm_vb.h"

int vb_ioctl_set_config(struct cvi_vb_cfg *cfg);
int vb_ioctl_get_config(struct cvi_vb_cfg *cfg);
int vb_ioctl_init(struct cvi_vb_cfg *cfg);
int vb_ioctl_exit();
int vb_ioctl_create_pool(struct cvi_vb_pool_cfg *cfg);
int vb_ioctl_destroy_pool(VB_POOL poolId);
int vb_ioctl_phys_to_handle(struct cvi_vb_blk_info *blk_info);
int vb_ioctl_get_blk_info( struct cvi_vb_blk_info *blk_info);
int vb_ioctl_get_block(struct cvi_vb_blk_cfg *blk_cfg);
int vb_ioctl_release_block(VB_BLK blk);
int vb_ioctl_get_pool_max_cnt(CVI_U32 *vb_max_pools);
int vb_ioctl_print_pool(VB_POOL poolId);
int vb_ioctl_unit_test( CVI_U32 op);

#endif // MODULES_VB_IOCTL_H_