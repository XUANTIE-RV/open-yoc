#include "board.h"
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include "fatfs_vfs.h"
#include "vfs.h"
#include <cvi_base.h>
#include "vi_uapi.h"
#include "cvi_vb.h"
#include "cvi_sys.h"
#include "cvi_vi.h"
#include "cvi_isp.h"
#include "cvi_ae.h"
#include "cvi_buffer.h"
#include "test_vi.h"
#include "cvi_sns_ctrl.h"
#include "cif_uapi.h"
#include "common_vi.h"
#include <pinctrl-mars.h>
#include "vi_isp.h"

// 开机后获取当前标定值
int get_FastConvCalibrationInfo(VI_PIPE ViPipe)
{
    int ret = 0;
    CVI_S16 firstFrameLuma = 0;
    CVI_S16 stableBv = 0;

    ret = CVI_ISP_GetFastConvCalibrationInfo(ViPipe, &firstFrameLuma, &stableBv);
    if(ret == 0) {
        printf("vipipe =%d firstFrameLuma =%d stableBv = %d\n", ViPipe, firstFrameLuma, stableBv);
    } 

    return ret;
}

void calibrate_ae_lut(int32_t argc, char **argv)
{
    VI_PIPE ViPipe = 0;
    get_FastConvCalibrationInfo(ViPipe);
}

void enable_ae_log(int32_t argc, char **argv)
{
    if(argc < 2) {
        printf("usage:enable_ae_log pipe level\n");
        return;
    }
    ISP_EXPOSURE_ATTR_S _ispExposure = {0};
    VI_PIPE ViPipe = atoi(argv[1]);
    CVI_U8 mode = atoi(argv[2]);
    CVI_ISP_GetExposureAttr(ViPipe, &_ispExposure);
    _ispExposure.u8DebugMode = mode;
    CVI_ISP_SetExposureAttr(ViPipe, &_ispExposure);
}

ALIOS_CLI_CMD_REGISTER(calibrate_ae_lut, calibrate_ae_lut, calibrate_ae_lut);
ALIOS_CLI_CMD_REGISTER(enable_ae_log, enable_ae_log, enable_ae_log);
