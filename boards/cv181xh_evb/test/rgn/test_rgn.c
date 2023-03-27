#include "board.h"
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include "fatfs_vfs.h"
#include "vfs.h"
#include <cvi_base.h>
#include "vo_uapi.h"
#include "cvi_vb.h"
#include "cvi_sys.h"
#include "cvi_vo.h"
#include "cvi_buffer.h"
#include "cvi_comm_vo.h"
#include <pinctrl-mars.h>
#include "rgn_uapi.h"
#include "cvi_region.h"
#include "test_rgn.h"
#include "cvi_comm_region.h"
#include "media_osd.h"

void start_rgn(int32_t argc, char **argv)
{
	aos_cli_printf("******start rgn******\n");
	APP_CHECK_RET(APP_OSD_Init(), "APP_OSD_Init failed!\n");
}
ALIOS_CLI_CMD_REGISTER(start_rgn, start_rgn, start rgn on vpss);

void stop_rgn(int32_t argc, char **argv)
{
	APP_CHECK_RET(APP_OSD_DeInit(), "APP_OSD_DeInit failed!\n");
	aos_cli_printf("******stop rgn******\n");
}
ALIOS_CLI_CMD_REGISTER(stop_rgn, stop_rgn, stop rgn on vpss);
