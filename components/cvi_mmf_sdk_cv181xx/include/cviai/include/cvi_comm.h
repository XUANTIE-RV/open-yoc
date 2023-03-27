#ifndef __CVI_COMM_H__
#define __CVI_COMM_H__
#ifdef CONFIG_ALIOS
#include <cvi_comm_video.h>
#include <cvi_comm_vpss.h>
#include <cvi_comm_vpss.h>
#include <cvi_errno.h>
#include <cvi_math.h>
#include <cvi_type.h>
#elif defined(_MIDDLEWARE_V2_)
#include <linux/cvi_comm_gdc.h>
#include <linux/cvi_comm_video.h>
#include <linux/cvi_comm_vpss.h>
#include <linux/cvi_errno.h>
#include <linux/cvi_math.h>
#include <linux/cvi_type.h>
#else
#include <cvi_comm_gdc.h>
#include <cvi_comm_video.h>
#include <cvi_comm_vpss.h>
#include <cvi_errno.h>
#include <cvi_math.h>
#include <cvi_type.h>
#endif

#endif