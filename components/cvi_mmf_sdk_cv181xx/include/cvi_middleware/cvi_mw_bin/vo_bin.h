#ifndef _VO_BIN_H
#define _VO_BIN_H

#include "cvi_type.h"
#include "cvi_bin.h"
#include "cvi_common.h"
#include "cvi_comm_vo.h"

CVI_S32 vo_bin_getbinsize(CVI_U32 *size);
CVI_S32 vo_bin_getparamfrombin(CVI_U8 *addr, CVI_U32 size);
CVI_S32 vo_bin_setparamtobuf(CVI_U8 *buffer);
CVI_S32 vo_bin_setparamtobin(FILE *fp);
VO_BIN_INFO_S *get_vo_bin_info_addr(void);
CVI_U32 get_vo_bin_guardmagic_code(void);

#endif
