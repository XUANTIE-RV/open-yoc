
#ifndef _AWB_BUF_H_
#define _AWB_BUF_H_

#include "cvi_comm_inc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

extern ISP_WB_Q_INFO_S *pstAwb_Q_Info[AWB_SENSOR_NUM];
extern ISP_WB_ATTR_S *pstAwbMpiAttr[AWB_SENSOR_NUM];
extern ISP_AWB_ATTR_EX_S *pstAwbMpiAttrEx[AWB_SENSOR_NUM];

void *AWB_Malloc(size_t nsize);
void AWB_Free(void *ptr);
void AWB_CheckMemFree(void);

void awb_buf_init(CVI_U8 sID);
void awb_buf_deinit(CVI_U8 sID);

#ifdef ARCH_RTOS_MARS
void AWB_RtosBufInit(CVI_U8 sID);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // _AWB_BUF_H_
