#ifndef MODULES_VPU_INCLUDE_VPSS_IOCTL_H_
#define MODULES_VPU_INCLUDE_VPSS_IOCTL_H_

#include <cvi_common.h>
#include <cvi_comm_vpss.h>
#include <vpss_uapi.h>

extern int vpss_ioctl(unsigned int cmd,  void *arg);

/* Configured from user  */
CVI_S32 vpss_create_grp(struct vpss_crt_grp_cfg *cfg);
CVI_S32 vpss_destroy_grp(VPSS_GRP VpssGrp);
CVI_S32 vpss_get_available_grp(VPSS_GRP *pVpssGrp);

CVI_S32 vpss_start_grp(struct vpss_str_grp_cfg *cfg);
CVI_S32 vpss_stop_grp(VPSS_GRP VpssGrp);

CVI_S32 vpss_reset_grp(VPSS_GRP VpssGrp);

CVI_S32 vpss_set_grp_attr(const struct vpss_grp_attr *cfg);
CVI_S32 vpss_get_grp_attr(struct vpss_grp_attr *cfg);

CVI_S32 vpss_set_grp_crop(const struct vpss_grp_crop_cfg *cfg);
CVI_S32 vpss_get_grp_crop(struct vpss_grp_crop_cfg *cfg);

CVI_S32 vpss_send_frame(struct vpss_snd_frm_cfg *cfg);
CVI_S32 vpss_send_chn_frame(struct vpss_chn_frm_cfg *cfg);

CVI_S32 vpss_set_chn_attr(struct vpss_chn_attr *attr);
CVI_S32 vpss_get_chn_attr(struct vpss_chn_attr *attr);

CVI_S32 vpss_enable_chn(struct vpss_en_chn_cfg *cfg);
CVI_S32 vpss_disable_chn(struct vpss_en_chn_cfg *cfg);

CVI_S32 vpss_set_chn_crop(const struct vpss_chn_crop_cfg *cfg);
CVI_S32 vpss_get_chn_crop(struct vpss_chn_crop_cfg *cfg);

CVI_S32 vpss_set_chn_rotation(const struct vpss_chn_rot_cfg *cfg);
CVI_S32 vpss_get_chn_rotation(struct vpss_chn_rot_cfg *cfg);

CVI_S32 vpss_show_chn(struct vpss_en_chn_cfg *cfg);
CVI_S32 vpss_hide_chn(struct vpss_en_chn_cfg *cfg);

CVI_S32 vpss_set_chn_ldc(const struct vpss_chn_ldc_cfg *cfg);
CVI_S32 vpss_get_chn_ldc(struct vpss_chn_ldc_cfg *cfg);

CVI_S32 vpss_get_chn_frame(struct vpss_chn_frm_cfg *cfg);

CVI_S32 vpss_release_chn_frame(const struct vpss_chn_frm_cfg *cfg);

CVI_S32 vpss_attach_vbpool(const struct vpss_vb_pool_cfg *cfg);
CVI_S32 vpss_detach_vbpool(const struct vpss_vb_pool_cfg *cfg);

CVI_S32 vpss_set_chn_align(const struct vpss_chn_align_cfg *cfg);
CVI_S32 vpss_get_chn_align(struct vpss_chn_align_cfg *cfg);

CVI_S32 vpss_set_chn_yratio(const struct vpss_chn_yratio_cfg *cfg);
CVI_S32 vpss_get_chn_yratio(struct vpss_chn_yratio_cfg *cfg);

CVI_S32 vpss_set_coef_level(const struct vpss_chn_coef_level_cfg *cfg);
CVI_S32 vpss_get_coef_level(struct vpss_chn_coef_level_cfg *cfg);

CVI_S32 vpss_set_chn_wrap(const struct vpss_chn_wrap_cfg *cfg);
CVI_S32 vpss_get_chn_wrap(struct vpss_chn_wrap_cfg *cfg);

CVI_S32 vpss_trigger_snap_frame(struct vpss_snap_cfg *cfg);

/* INTERNAL */
CVI_S32 vpss_set_grp_csc(const struct vpss_grp_csc_cfg *csc_cfg);

#endif /* MODULES_VPU_INCLUDE_VPSS_IOCTL_H_ */
