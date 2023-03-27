#include <aos/cli.h>
#include <drv/efuse.h>
#include "cvi_efuse.h"
#include "cvi_misc.h"

static void cvi_fastboot_enable()
{
    csi_efuse_t efuse = {0};
    csi_efuse_init(&efuse, 0);
    int ret = CVI_EFUSE_EnableFastBoot();
    if (ret == CVI_SUCCESS) {
        printf("fast boot enable\n");   
    }else {
        printf("CVI_EFUSE_EnableFastBoot ret=%d\n", ret);
    }
    
#if 0
    ret = CVI_EFUSE_IsFastBootEnabled();
    if (ret == CVI_SUCCESS) {
        printf("fast boot enable\n");   
    }
    else {
        printf("CVI_EFUSE_IsFastBootEnabled ret=%d\n", ret);
    }
#endif

    csi_efuse_uninit(&efuse); 
}


static void cmd_vendor_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (strcmp(argv[1], "fastboot") == 0) {
        cvi_fastboot_enable();
    }
}

void cli_reg_cmd_vendor(void)
{
    static const struct cli_command cmd_info = { "vendor", "vendor extend command.", cmd_vendor_func };

    aos_cli_register_command(&cmd_info);
}
