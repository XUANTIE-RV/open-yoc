/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_misc.h
 * Description:
 *   MMF Programe Interface for system
 */


#ifndef __CVI_MISC_H__
#define __CVI_MISC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <cvi_type.h>

CVI_S32 CVI_EFUSE_EnableFastBoot(void);
CVI_S32 CVI_EFUSE_IsFastBootEnabled(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__CVI_MISC_H__ */

