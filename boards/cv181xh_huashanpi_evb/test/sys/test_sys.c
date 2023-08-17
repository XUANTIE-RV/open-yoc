#include <aos/cli.h>
#include "cvi_vb.h"
#include "cvi_sys.h"
#include "cvi_buffer.h"







void test_sys_init(int32_t argc, char **argv)
{
	VB_CONFIG_S stVbConfig = {0};
	CVI_U32 u32BlkSize;
	CVI_U32 u32BlkSizeRot;

	stVbConfig.u32MaxPoolCnt = 1;
	u32BlkSize = COMMON_GetPicBufferSize(1920, 1088, PIXEL_FORMAT_NV21,
							DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSizeRot = COMMON_GetPicBufferSize(1088, 1920, PIXEL_FORMAT_NV21,
							DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConfig.astCommPool[0].u32BlkSize = MAX2(u32BlkSize, u32BlkSizeRot);
	stVbConfig.astCommPool[0].u32BlkCnt = 6;
	stVbConfig.astCommPool[1].u32BlkSize = COMMON_GetPicBufferSize(720, 1280, PIXEL_FORMAT_NV21,
							DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConfig.astCommPool[1].u32BlkCnt = 3;

	printf("pool 0 blk size:%d\n", stVbConfig.astCommPool[0].u32BlkSize);

	CVI_VB_SetConfig(&stVbConfig);
	CVI_SYS_Init();
	CVI_VB_Init();
}

ALIOS_CLI_CMD_REGISTER(test_sys_init,sys_init,sys_init);
