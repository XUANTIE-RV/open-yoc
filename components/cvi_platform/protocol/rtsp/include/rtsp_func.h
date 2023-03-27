#ifndef __RTSP_FUNC_H__
#define __RTSP_FUNC_H__
#include "cvi_type.h"
#include "cvi_comm_venc.h"

CVI_S32 SendToRtsp(int live, VENC_STREAM_S *pstStream);
CVI_VOID rtsp_setup(int live, int type);
void cvi_rtsp_init();







#endif
